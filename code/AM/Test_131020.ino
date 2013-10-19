////////////////////////////////////////////////////////
//****************************************************//
//*    (C) 2013 by Matthias Lehmann & David Arenz    *//
//* Test_131020.ino - 20.10.2013 by Matthias Lehmann *//
//* 	Version 0.01.131020-ML						 *//
//****************************************************//
//* Allgemeiner Test des RFM12B - aktuell keine SPI- *//
//* Kommunikation funktionsf�hig!					 *//
//****************************************************//
////////////////////////////////////////////////////////

// Includes
#include <SPI.h>

// Defines
#define DATAOUT 11 		//MOSI
#define DATAIN  12 		//MISO 
#define SPICLOCK  13 	//SCK
#define SLAVESELECT 10 	//SS, RF12B

// Hilfsvariable f�r Schleifentaktung
int i = 0;
int data;

// the Setup-Routine
void setup()
{
/* HW-Defs */
	pinMode(9, OUTPUT);
  
/* Kommunikationsparameter */
//SPI
	pinMode(DATAOUT, OUTPUT);
	pinMode(DATAIN, INPUT);
	pinMode(SPICLOCK,OUTPUT);
	pinMode(SLAVESELECT,OUTPUT);

	// SPI-Parameter konfigurieren
	SPI.setBitOrder(MSBFIRST);
	SPI.setClockDivider(SPI_CLOCK_DIV4);
	SPI.setDataMode(SPI_MODE0);
	
	// SPI initialisieren
	SPI.begin();   
	
//Serial
	Serial.begin(9600);
	
//RFM12B konfigurieren
	RF12B_config();
}

// the loop routine runs over and over again forever:
void loop() 
{
	data = SPIRead(); //get data byte
	Serial.print('\n');
	Serial.print(data,BIN);
	// Blinker-LED aktivieren
	if(i>500) digitalWrite(9, HIGH);
	else digitalWrite(9, LOW);
	// Blinktakt generieren
	if(i<1000) i++;
	else i=0;
	// Schleife takten
	delay(1);  
}

void SPIWrite(int value) 
{
	// Setzte SS-Pin LOW um Slave zu aktivieren
	digitalWrite(SLAVESELECT,LOW);
	//  Daten �bertragen
	SPI.transfer(value);
	// Setzte SS-Pin HIGH um Slave zu deaktivieren
	digitalWrite(SLAVESELECT,HIGH); 
}

unsigned int SPIRead()
{  
	// Setzte SS-Pin LOW um Slave zu aktivieren
	digitalWrite(SLAVESELECT,LOW);
	//  Daten �bertragen
	SPI.transfer(0x00);
	// Setzte SS-Pin HIGH um Slave zu deaktivieren
	digitalWrite(SLAVESELECT,HIGH); 
}

/* �bersichten zu den Konfigs hier:
	https://www.mikrocontroller.net/articles/RFM12#Kann_man_SEL_permanent_auf_LOW_halten.3F
	https://www.sparkfun.com/datasheets/Wireless/General/RF12B_code.pdf
*/
void RF12B_config()
{
	/* MSB jeweisl f�hrend */

	/* Standart Konfiguration */
	// Grundkonfiguration: Latch ein, FIFO ein, Band 433MHz, XTAL-Cap 12,5pF
	SPIWrite(0x80D8);	
	// Komponentenkonfiguration: Emp�nger aus, Empf�ngerband aus, Sender ein, PLL aus, Quarz ein, Betriebsspannungs�berwachung aus, WakeUp aus, Taktausgabe aus
	SPIWrite(0x8229);	
	// Taktgeneratorkonfiguration: Treiberverst�rkung <=2,5MHz, LOW-Power-Mode des Quarz aus, Phasenverz�gerung aus, Dithering in PLL aus, PLL-Bandbreite -102dBc/Hz
	SPIWrite(0xCC07);
	// Frequenzkonfiguration: 433 MHz auf 10MHz Quarz, EU
	SPIWrite(0xA1B1);
	// Datenratenkonfiguration: Basistakt 10MHz/29, Ratenteilerfaktor 9600s^-1 (cs=0)
	SPIWrite(0xC621);
	// Senderkonfiguration: Signalinvertierung aus, Frequenzhub +-15kHz, rel. Sendeleistung 0dB (Maximum)
	SPIWrite(0x9800);	
}

void RF12B_send(char byte)
{
	// Prefix f�r RFM12B als MSB anf�gen
	int senddata = 0xB800;
	// LSB mit zu sendenden Date anf�gen
	senddata += byte;
	SPIWrite(senddata);
}
// EOF