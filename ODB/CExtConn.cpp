/**
 * \file CExtConn.cpp
 *
 * \date 7 août 2014
 * \author dla
 */
#include "CExtConn.h"

// how long are max NMEA lines to parse?
#define MAXLINELENGTH 120

// we double buffer: read one line in and leave one for the main program
volatile char eline1[MAXLINELENGTH];
volatile char eline2[MAXLINELENGTH];
// our index into filling the current line
volatile uint8_t elineidx=0;
// pointers to the double buffers
volatile char *ecurrentline;
volatile char *elastline;
volatile boolean erecvdflag;
volatile boolean einStandbyMode;


#ifdef __AVR__
// Constructor when using SoftwareSerial or NewSoftSerial
#if ARDUINO >= 100
CExtConn::CExtConn(SoftwareSerial *ser) {
#else
CExtConn::CExtConn(NewSoftSerial *ser) {
#endif
	common_init();     // Set everything to common state, then...
	extSwSerial = ser; // ...override gpsSwSerial with value passed.
	extHwSerial = NULL;
}
#endif

CExtConn::CExtConn(HardwareSerial *ser) {
	common_init();  // Set everything to common state, then...
	extHwSerial = ser; // ...override gpsHwSerial with value passed.
	extSwSerial = NULL;
}

void CExtConn::common_init(void) {
	erecvdflag   = false;
	paused      = false;
	elineidx     = 0;
	ecurrentline = eline1;
	elastline    = eline2;
}

char CExtConn::read(void) {
	char c = 0;

	if (paused) return c;

#ifdef __AVR__
	if(extSwSerial) {
		if(!extSwSerial->available()) return c;
		c = extSwSerial->read();
	} else {
		if(!extHwSerial->available()) return c;
		c = extHwSerial->read();
	}
#else
	//   if(!gpsHwSerial->available()) return c;
	//  c = gpsHwSerial->read();
	if(!Serial1.available()) return c;
	c = Serial1.read();
#endif


	//Serial.print(c);

	if (c == '$') {
		ecurrentline[elineidx] = 0;
		elineidx = 0;
	}
	if (c == '\n') {
		ecurrentline[elineidx] = 0;

		if (ecurrentline == eline1) {
			ecurrentline = eline2;
			elastline = eline1;
		} else {
			ecurrentline = eline1;
			elastline = eline2;
		}

		//Serial.println("----");
		//Serial.println((char *)lastline);
		//Serial.println("----");
		elineidx = 0;
		erecvdflag = true;
	}

	ecurrentline[elineidx++] = c;
	if (elineidx >= MAXLINELENGTH)
		elineidx = MAXLINELENGTH-1;

	return c;
}

void CExtConn::begin(long baud) {
#ifdef __AVR__
	if(extSwSerial)
		extSwSerial->begin(baud);
	else
		extHwSerial->begin(baud);
#else
	//  gpsHwSerial->begin(baud);
	Serial1.begin(baud);
#endif
	delay(10);
}

void CExtConn::sendCommand(char *str) {
#ifdef __AVR__
	if(extSwSerial)
		extSwSerial->println(str);
	else
		extHwSerial->println(str);
#else
	//  gpsHwSerial->println(str);
	Serial1.println(str);
#endif
	delay(50);
}

boolean CExtConn::newLINEreceived(void) {
	return erecvdflag;
}

void CExtConn::pause(boolean p) {
	paused = p;
}

char *CExtConn::lastLINE(void) {
	erecvdflag = false;
	return (char *)elastline;
}

uint8_t CExtConn::parse(char *line) {
	// do checksum check
	// first look if we even have one
	if (line[strlen(line)-4] == '*') {
		uint16_t sum = parseHex(line[strlen(line)-3]) * 16;
		sum += parseHex(line[strlen(line)-2]);
		// check checksum
		for (uint8_t i=1; i < (strlen(line)-4); i++) {
			sum ^= line[i];
		}
		if (sum != 0) {
			// bad checksum :(
			//return false;
		}
	}

	// look for a few common sentences
	if (strstr(line,"$PMTK")) {
		return 1;
	}
	if (strstr(line,"$IODB")) {
		return 2;
	}
	if (strstr(line,"$FODB")) {
		return 3;
	}
	if (strstr(line,"$CODB")) {
		return 4;
	}

	return 0;
}

// read a Hex value and return the decimal equivalent
uint8_t CExtConn::parseHex(char c) {
	if (c < '0')
		return 0;
	if (c <= '9')
		return c - '0';
	if (c < 'A')
		return 0;
	if (c <= 'F')
		return (c - 'A')+10;
	return 0;
}
