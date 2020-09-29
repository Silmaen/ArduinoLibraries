/**
 * \file CExtConn.h
 *
 * \date 7 août 2014
 * \author dla
 */
#ifndef CEXTCONN_H_
#define CEXTCONN_H_

#if ARDUINO >= 100
 #include <Arduino.h>
#if defined (__AVR__) && !defined(__AVR_ATmega32U4__)
 #include <SoftwareSerial.h>
#endif
#else
 #include <WProgram.h>
 #include <NewSoftSerial.h>
#endif

class CExtConn {
public:
	#ifdef __AVR__
	  #if ARDUINO >= 100
		CExtConn(SoftwareSerial *ser); // Constructor when using SoftwareSerial
	  #else
		CExtConn(NewSoftSerial  *ser); // Constructor when using NewSoftSerial
	  #endif
	#endif
	CExtConn(HardwareSerial *ser); // Constructor when using HardwareSerial
//
	void begin(long baud);

	char *lastLINE(void);
	boolean newLINEreceived();

	void common_init(void);
	void sendCommand(char *);

	void pause(boolean b);
	char read(void);

	/**
	 * \brief parse the actual line
	 * \param line the line to parse
	 * \return 0 if unknown command or error
	 *         1 command for GPS
	 *         2 command for ODB
	 */
	uint8_t parse(char *line);
	void interruptReads(boolean r);

private:
	boolean paused;
	uint8_t parseHex(char c);
	uint8_t parseResponse(char *response);
	#ifdef __AVR__
	  #if ARDUINO >= 100
		SoftwareSerial *extSwSerial;
	  #else
		NewSoftSerial  *extSwSerial;
	  #endif
	#endif
	HardwareSerial *extHwSerial;
};

#endif /* CEXTCONN_H_ */
