/***********************************************
 *                                             *
 *  ma librairie graphique                     *
 *    en gros elle gère tout ce qui est sur le *
 *    bus SPI:                                 *
 *     - le contrôler TFT RA8875               *
 *     - le contrôler de carte SD              *
 *                                             *
 ***********************************************/

#ifndef CMYGRAPHICS_H
#define CMYGRAPHICS_H

#if ARDUINO >= 100
 #include <Arduino.h>
 #include <Print.h>
#else
 #include <WProgram.h>
#endif

#ifdef __AVR__
  #include <avr/pgmspace.h>
#endif

#include <SPI.h>
#include <Wire.h>
#include "SdFat.h"
#include "RTC_DS3231.h"
#include "RTClib.h"
#include "Adafruit_GPS.h"
#include "RA8875_driver.h"
#include "NStrutils.h"
#include "CMyImu.h"
#include "CExtConn.h"

// SD chip select pin
//const uint8_t SD_CS = 40;
//const uint8_t SD_CD = 41;
#define SD_CS 40
#define SD_CD 41

// définition des pins pour le module d'écran
#define RA8875_INT 44
#define RA8875_WAIT 45
#define RA8875_CS 47
#define RA8875_RESET 46


class CMyGraphics : public RA8875_driver {
public:
	/**
	 *  \brief Constructor
	 */
	CMyGraphics(CExtConn* extConn,uint8_t tftcs,uint8_t tftrst,uint8_t tftwait,uint8_t tftint,uint8_t sdcs,uint8_t sdcd);
	void initAll(enum RA8875GPUclock spd=RA8875_GPU_STD_CLK);
	/**
	 *  \brief Dessine une image système
	 */
	void drawpicture(char* file,uint16_t X=0, uint16_t Y=0,uint16_t W=800,uint16_t H=480);
	/**
	 * \brief access to the SD card reader
	 */
	SdFat* getSD(void){return &sd;}
	/**
	 * \brief draw button
	 */
	void drawButton(uint8_t index,boolean active=false);
	/**
	 * \brief treat the tft interupt
	 */
	void TFTinterurp();
	/**
	 * \brief called every loop
	 */
	void frame(double dt);
	void drawHome(bool mc);
	void attachGPS(Adafruit_GPS *gps);
	void attachIMU(CMyIMU *_imu);
	boolean updategps;
	// fonctions de log
	void toggleLoggeur(void);
	void logFile(void);
	void getFingerPos(uint16_t* fX,uint16_t* fY,boolean* touch,boolean* relea);
	void setExternalConnexion(boolean ext);
private:
	void treatcommand(char* line);
	void drawBlack();
	void drawSettings(bool mc);
	void drawLines(bool mc);
	void drawGauges(bool mc);
	void drawExternal(bool mc);
	void drawBtnChar(uint16_t X,uint16_t Y,uint16_t W,uint16_t H,uint16_t charcol,uint16_t rectcol,uint16_t bg,char* c,uint8_t sz);
	// Classe de la carte SD
	SdFat sd;
	/* pins pour TFT */
	uint8_t _tftcs,_tftrst,_tftwait,_tftint;
	/* pins pour SD */
	uint8_t _sdcs,_sdcd;
	/* touch screen */
	bool _touched,release;
	uint16_t X,Y;
	/* gestion menu */
	uint8_t menu;
	/* la montre */
	RTC_DS3231 montre;
	DateTime currentTime,lastTime;
	DateTime configTime;
	// fonction du temps
	uint8_t DateToString(char* str,DateTime  t);
	// module GPS
	Adafruit_GPS *GPS;
	boolean lastfix;
	// timer
	uint32_t timer1,timer2;
	//IMU
	CMyIMU *imu;
	// loggeur
	boolean logon;
	SdFile filelog;
	uint32_t timerlog,loginterval;
	// external connexion
	boolean external;
	CExtConn* extconn;
};

#endif

