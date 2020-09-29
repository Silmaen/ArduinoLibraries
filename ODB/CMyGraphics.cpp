/***********************************************
 *                                             *
 *  ma librairie graphique                     *
 *    en gros elle gère tout ce qui est sur le *
 *    bus SPI:                                 *
 *     - le contrôler TFT RA8875               *
 *     - le contrôler de carte SD              *
 *                                             *
 ***********************************************/

#include "CMyGraphics.h"

DateTime gcurrentTime;

CMyGraphics::CMyGraphics(CExtConn* extConn,uint8_t tftcs,uint8_t tftrst,uint8_t tftwait,uint8_t tftint,uint8_t sdcs,uint8_t sdcd):
		RA8875_driver(tftcs, tftrst,tftint,tftwait) {
	_tftcs=tftcs;
	_tftrst=tftrst;
	_tftwait=tftwait;
	_tftint=tftint;
	_sdcs=sdcs;
	_sdcd=sdcd;
	X=0;
	Y=0;
	_touched=false;
	release=false;
	menu=1;
	GPS = NULL;
	updategps=false;
	imu=NULL;
	logon=false;
	lastfix=false;
	timer1=0;
	timer2=0;
	timerlog=0;
	loginterval=0;
	external=false;
	extconn=extConn;
}

void dateTime(uint16_t* date, uint16_t* time) {
  // User gets date and time from GPS or real-time
  // clock in real callback function
  // return date using FAT_DATE macro to format fields
  *date = FAT_DATE(gcurrentTime.year(), gcurrentTime.month(), gcurrentTime.day());
  // return time using FAT_TIME macro to format fields
  *time = FAT_TIME(gcurrentTime.hour(), gcurrentTime.minute(), gcurrentTime.second());
}

void CMyGraphics::initAll(enum RA8875GPUclock spd){
	// Initialization of pins
	/* pins for SDcard */
	pinMode(SD_CS, OUTPUT);
	digitalWrite(SD_CS, HIGH);
	pinMode(SD_CD, INPUT);
	/* Pins for TFT */
	pinMode(RA8875_CS, OUTPUT);
	digitalWrite(RA8875_CS, HIGH);
	pinMode(RA8875_RESET, OUTPUT);
	digitalWrite(RA8875_RESET, LOW);
	pinMode(RA8875_RESET, INPUT);
	pinMode(RA8875_INT, INPUT);
	/* Initialization of the output */
	/* Demmarage graphique */
	extconn->sendCommand("CMyGraphics :Demarrage...");
	_size = RA8875_800x480;
	_width = 800;
	_height = 480;
	/* Initialise l'écran 'RA8875_800x480' */
/*	if (!begin(RA8875_800x480,RA8875_SPI_FULL_SPEED,spd)) {
		cout << "CMyGraphics: RA8875 Not Found!" << endl;
		cout << "Ecran            [ FAIL ]"<< endl;
		cout << "CMyGraphics: processus stoppé!" << endl;
		while (1);
	}*/
	// demmarage manuel...
	delay(100);
	digitalWrite(RA8875_RESET, HIGH);
	delay(100);
	/* Start SPI interface */
	SPI.begin();
	SPI.setDataMode(SPI_MODE0);
	SPI.setClockDivider(SPI_CLOCK_DIV128);
	if (readReg(0) != 0x75) {
		extconn->sendCommand("FATAL ERROR: No screen found");
		while(1);
	}
	initialize(RA8875_GPU_DBL_CLK);
	SPI.setClockDivider(SPI_CLOCK_DIV2);
	displayOn(true);
	// backlight
	GPIOX(true);
	PWM1config(true, RA8875_PWM_CLK_DIV1024); // PWM output for backlight
	PWM1out(255);
	textMode();
	extconn->sendCommand("Ecran            [  OK  ]");
	textSetCursor(0,0);
	textEnlarge(1);
	/* Initialise la carte SD */
	/* initialize the SD card at SPI_HALF_SPEED to avoid bus errors with
	 * breadboards.  use SPI_FULL_SPEED for better performance.*/
	if (!sd.begin(SD_CS, SPI_FULL_SPEED)) {
		extconn->sendCommand("Controleur SD    [ FAIL ]");
		textWrite("Controleur SD    [ FAIL ]",25);
		sd.initErrorHalt();
		while(1);
	}
	extconn->sendCommand("Controleur SD    [  OK  ]");
	textWrite("Controleur SD    [  OK  ]",25);
	textSetCursor(0,32);
	int SD_here = 0;
/*	SD_here = digitalRead(SD_CD);
	if (!SD_here) {
		extconn->sendCommand("Insert a SD Card\n");
		textWrite("Insert a SD Card",16);
		while (!SD_here){
			SD_here = digitalRead(SD_CD);
		}
	}*/
	// contrôle de l'arborescence
	if (!sd.exists("obl")){
		if (sd.mkdir("obl")) extconn->sendCommand("Creation de la racine OBL");
		if (sd.chdir("obl")){
			if (sd.mkdir("log")) extconn->sendCommand("Creation du répertoire LOG");
			if (sd.mkdir("img")) extconn->sendCommand("Creation du répertoire IMG");
		}
		extconn->sendCommand("Creation de l'arborescence\n");
	} else {
		if (sd.chdir("obl")) {
			//cout << "Utilisation de l'arborescence existante" << endl;
		} else {
			extconn->sendCommand("Impossible d'utiliser l'arborescence");
			extconn->sendCommand("Carte SD         [ FAIL ]");
		}
	}
	SdFile::dateTimeCallback(dateTime);
	extconn->sendCommand("Carte SD         [  OK  ]");
	textSetCursor(0,32);
	textWrite("Carte SD         [  OK  ]",25);
	delay(100);
	// tentative de dessin
	//graphicsMode();
	// activation du touch screen
	touchEnable(true,RA8875_TOUCH_DBL_SPD);
	extconn->sendCommand("Touchpad         [  OK  ]");
	textSetCursor(0,64);
	textWrite("Touchpad         [  OK  ]",25);
	delay(100);
	// la montre
	montre.begin();
	currentTime=montre.now();
	lastTime=currentTime;
	gcurrentTime = currentTime;
	extconn->sendCommand("Montre           [  OK  ]");
	textSetCursor(0,96);
	textWrite("Montre           [  OK  ]",25);
	delay(100);
	timer1=millis();
	timer2=millis();
	updategps=false;
	timerlog=millis();
	loginterval=100;
}

void CMyGraphics::drawpicture(char* file,uint16_t X, uint16_t Y,uint16_t W,uint16_t H){
	SdFile Ifile(file, O_READ);
	/*if (!Ifile.isOpen()){
		cout << "ERROR opening file" << endl;
	}else{*/
		uint16_t n,val[800];
		uint16_t YY=Y;
		digitalWrite(RA8875_CS, LOW);
		setXY(X,YY);
		writeCommand(RA8875_MRWC);
		digitalWrite(RA8875_CS, HIGH);
		SPI.transfer(0xFF);
		if (W!=800) {
			while ((n = Ifile.read(&val, 2*W)) > 0) {
				digitalWrite(RA8875_CS, LOW);
				SPI.transfer(RA8875_DATAWRITE);
				for(uint16_t i=0;i<W;i++){
					SPI.transfer(val[i]);
					SPI.transfer(val[i]>>8);
				}
				YY++;
				digitalWrite(RA8875_CS, HIGH);
				SPI.transfer(0xFF);
				setXY(X,YY);
				writeCommand(RA8875_MRWC);
			}
		}else{
			while ((n = Ifile.read(&val, 1600)) > 0) {
				digitalWrite(RA8875_CS, LOW);
				SPI.transfer(RA8875_CMDWRITE);
				SPI.transfer(RA8875_MRWC);
				SPI.transfer(RA8875_DATAWRITE);
				for(uint16_t i=0;i<800;i++){
					SPI.transfer(val[i]);
					SPI.transfer(val[i]>>8);
				}
				digitalWrite(RA8875_CS, HIGH);
				SPI.transfer(0xFF);
			}
		}
	///}
	Ifile.close();
}

void CMyGraphics::drawButton(uint8_t index,boolean active){
	if (!getSD()->exists("IMG/TESTBTNH.BIN")) extconn->sendCommand("erreur fichier TESTBTNH.BIN non present");
	if (!getSD()->exists("IMG/TESTBTN.BIN")) extconn->sendCommand("erreur fichier TESTBTN.BIN non present");
	switch(index){
	case 1:
		if (active)
			drawpicture("IMG/BTNHOMEH.BIN",662,10,125,50);
		else
			drawpicture("IMG/BTNHOME.BIN",662,10,125,50);
		break;
	case 2:
		if (active)
			drawpicture("IMG/BTNLINEH.BIN",662,70,125,50);
		else
			drawpicture("IMG/BTNLINE.BIN",662,70,125,50);
		break;
	case 3:
		if (active)
			drawpicture("IMG/BTNGAUGH.BIN",662,130,125,50);
		else
			drawpicture("IMG/BTNGAUG.BIN",662,130,125,50);
		break;
	case 4:
		if (active)
			drawpicture("IMG/TESTBTNH.BIN",662,190,125,50);
		else
			drawpicture("IMG/TESTBTN.BIN",662,190,125,50);
		break;
	case 5:
		if (active)
			drawpicture("IMG/BTNLOGH.BIN",662,250,125,50);
		else
			drawpicture("IMG/BTNLOG.BIN",662,250,125,50);
		break;
	case 6:
		if (active)
			drawpicture("IMG/BTNSETH.BIN",662,310,125,50);
		else
			drawpicture("IMG/BTNSET.BIN",662,310,125,50);
		break;
	default:
		extconn->sendCommand("ane batté va!!");
	}
}

void CMyGraphics::TFTinterurp(){
	release=false;
	if (! digitalRead(RA8875_INT)) {
		if (touched()){
			uint16_t Xr,Yr;
			touchRead(&Xr,&Yr);
			X=min(max((Xr-70)/900.0*800,0),800);
			Y=min(max((Yr-120)/800.0*480,0),480);
			_touched=true;
		}else{
			if (_touched) release=true;
			_touched=false;
		}
	}else{
		if (_touched) release=true;
		_touched=false;
	}
}

void CMyGraphics::frame(double dt){
	TFTinterurp();
	currentTime=montre.now();
	gcurrentTime = currentTime;

	if (extconn->newLINEreceived()){
		char rec[50];
		// echo de la commande reçue
		extconn->sendCommand(extconn->lastLINE());
		// traitement de la commande
		switch(extconn->parse(extconn->lastLINE())) {
		case 1:
			// command for the GPS: directly sent to the device
			extconn->sendCommand("Commande pour le GPS.");
			GPS->sendCommand(extconn->lastLINE());
			break;
		case 2:
			extconn->sendCommand("Bascule en mode connexion externe.");
			setExternalConnexion(true);
			break;
		case 3:
			extconn->sendCommand("Bascule en mode de connexion normale.");
			setExternalConnexion(false);
			break;
		case 4:
			if (external) {
				treatcommand(extconn->lastLINE());
			} else {
				extconn->sendCommand("ERREUR: Ce type de commande nécessite la bascule en mode de connexion externe.");
			}
			break;
		default:
			extconn->sendCommand("Type de commande inconnue.");
			break;
		}
	}

	if (!external) {
		bool menuchange=false;
		if (release) {
			if ((X>661)&&(X<786)){
				for (uint8_t i=0;i<6;i++){
					if ((i==3)) continue;
					if ((Y>=10+60*i )&&(Y<=60+60*i )) {
						if (i==4) {
							toggleLoggeur();
							drawButton(i+1,logon);
						}else{
							if (menu!=i+1){
								drawButton(menu);
								menu=i+1;
								drawButton(menu,true);
								menuchange=true;
							}
						}
					}
				}
			}
		}
		logFile();
		switch (menu) {
		case 1:
			drawHome(menuchange);
			break;
		case 2:
			drawLines(menuchange);
			break;
		case 3:
			drawGauges(menuchange);
			break;
		case 6:
			drawSettings(menuchange);
			break;
		default:
			break;
		}
	}
	if (lastTime.second()!=currentTime.second()) lastTime=currentTime;
	char str[10];
	double fps=1.0/dt;
	nstd::mftoa(str,sizeof(str),fps,1);
	drawString(str,5,0,460,0,RA8875_WHITE,RA8875_BLACK);
}

void CMyGraphics::treatcommand(char* line) {
	char *p = line;
	p = strchr(p, ',')+1;
	// first keyword
	if (strstr(p,"lslog")){
		// listing of the current directory for SD card
		sd.setStdOut(&Serial);
		if (!sd.chdir("LOG")) {
			extconn->sendCommand("ERREUR: Repertoire LOG inaccessible");
			return;
		}
		extconn->sendCommand("$RODB,debutls*4A");
		sd.ls(LS_SIZE|LS_DATE);
		extconn->sendCommand("$RODB,finls*49");
		if (!sd.chdir("/OBL")) {
			extconn->sendCommand("ERREUR: Impissoble de revenir en mode normal");
			return;
		}
		return;
	}
	if (strstr(p,"lsdir")){
		// listing of the current directory for SD card
		sd.setStdOut(&Serial);
		sd.ls();
		return;
	}
	if (strstr(p,"chdir")){
		// listing of the current directory for SD card
		sd.setStdOut(&Serial);
		p = strchr(p, ',')+1;
		uint8_t strleng=0;
		char fname[25];
		for (uint8_t i=0;(p[i]!=',')&&(p[i]!='\n')&&(p[i]!='\0')&&(p[i]!='*')&&(strleng<25);i++){
			fname[strleng]=p[i];
			strleng++;
		}
		fname[strleng]='\0';
		strleng++;
		if (!sd.chdir(fname)) {
			extconn->sendCommand("Directory error");
			return;
		}
		if (sd.vwd()->isRoot()){
			if (!sd.chdir("OBL")) {
				extconn->sendCommand("Directory error");
				return;
			}
		}
		return;
	}
	if (strstr(p,"pwd")){
		// listing of the current directory for SD card
		sd.setStdOut(&Serial);
		sd.vwd()->printName();
		extconn->sendCommand("");
		return;
	}
	if (strstr(p,"printlog")){
		// listing of the current directory for SD card
		sd.setStdOut(&Serial);
		p = strchr(p, ',')+1;
		uint8_t strleng=0;
		char fname[25];
		for (uint8_t i=0;(p[i]!=',')&&(p[i]!='\n')&&(p[i]!='\0')&&(p[i]!='*')&&(strleng<25);i++){
			fname[strleng]=p[i];
			strleng++;
		}
		if (fname[strleng-4]!='.') {
			fname[strleng]='.';
			fname[strleng+1]='L';
			fname[strleng+2]='O';
			fname[strleng+3]='G';
			fname[strleng+4]='\0';
			strleng+=4;
		}
		fname[strleng]='\0';
		strleng++;
		if (!sd.chdir("LOG")) {
			extconn->sendCommand("Directory error");
			return;
		}
		if (!sd.exists(fname)){
			Serial.print("ERROR: log file '");
			Serial.print(fname);
			Serial.println("' does not exists!");
			sd.chdir("/OBL");
			return;
		}
		SdFile file;
		if (!file.open(fname, O_READ)) {
			extconn->sendCommand("error opening file");
			sd.chdir("/OBL");
			return;
		}
		extconn->sendCommand("$RODB,debutlog*31");
		int res,nline=0;
		char c,line[255];
		bool ok=true;
		while (ok){
			int iline=0;
			bool ok2=true;
			while (ok2){
				res=file.read(&c,1);
				if (res<1) {
					ok2=false;
					ok=false;
				} else if ((c=='\n')||(c=='\r')){
					ok2=false;
					line[iline]='\0';
				} else {
					line[iline]=c;
					iline++;
				}
			}
			if ((ok)&&(iline!=0)) {
				extconn->sendCommand(line);
			}
		}
		file.close();
		extconn->sendCommand("$RODB,finlog*32");
		if (!sd.chdir("/OBL")) {
			extconn->sendCommand("Directory error");
			return;
		}

		return;
	}
	extconn->sendCommand("unknown command");
}

void CMyGraphics::setExternalConnexion(boolean ext) {
	if (ext){
		if (logon) toggleLoggeur();
		external=true;
		drawExternal(true);
		extconn->sendCommand("$RODB,external*2A");
	}else{
		if (!sd.chdir("/OBL")) {
			extconn->sendCommand("Directory error");
			return;
		}
		external=false;
		drawButton(menu);
		menu=1;
		drawButton(menu,true);
		drawHome(true);
		extconn->sendCommand("$RODB,normal*24");
	}
}

void CMyGraphics::drawExternal(bool mc) {
	if (mc) {
		drawBlack();
		drawString("  EXTERNAL",10,70,168,3,RA8875_WHITE,RA8875_BLACK);
		drawString("CONNEXION...",12,70,232,3,RA8875_WHITE,RA8875_BLACK);
	}
}

void CMyGraphics::drawHome(bool mc) {
	char sdate[36],sgps[37];
	uint8_t ldate;
	if (mc) { 
		drawBlack();
		drawString("Accueil",7,240,10,3,RA8875_WHITE,RA8875_BLACK);
		if (GPS){
			if (GPS->fix){
				snprintf(sgps,33,"GPS:    %2u satellites, qualite:%u",GPS->satellites,GPS->fixquality);
				drawString(sgps,33,20,140,1,RA8875_WHITE,RA8875_BLACK);
			}else{
				if (lastfix==false) {
					snprintf(sgps,34,"GPS:    en cours d'acquisition   ");
					drawString(sgps,34,20,140,1,RA8875_WHITE,RA8875_BLACK);
				} else {
					snprintf(sgps,34,"GPS:    Signal perdu, acquisition");
					drawString(sgps,34,20,140,1,RA8875_WHITE,RA8875_BLACK);
				}
			}
		}else{
			drawString("GPS:    offline",36,20,140,1,RA8875_WHITE,RA8875_BLACK);
		}
		drawString("IMU:    online",14,20,172,1,RA8875_WHITE,RA8875_BLACK);
		drawString("RTC:    online",14,20,204,1,RA8875_WHITE,RA8875_BLACK);
		ldate=DateToString(sdate,currentTime);
		drawString(sdate,ldate,190,450,0,RA8875_WHITE,RA8875_BLACK);
	} else {
		if (lastTime.second()!=currentTime.second()){
			ldate=DateToString(sdate,currentTime);
			drawString(sdate,ldate,190,450,0,RA8875_WHITE,RA8875_BLACK);
		}
		if (GPS){
			timer1=millis();
			if (GPS->fix){
				if (updategps){
					snprintf(sgps,34,"GPS:    %2u satellites, qualite:%u ",GPS->satellites,GPS->fixquality);
					drawString(sgps,34,20,140,1,RA8875_WHITE,RA8875_BLACK);
					lastfix=GPS->fix;
				}
			}else{
				if (lastfix==false) {
					snprintf(sgps,34,"GPS:    en cours d'acquisition   ");
					drawString(sgps,34,20,140,1,RA8875_WHITE,RA8875_BLACK);
				} else {
					snprintf(sgps,34,"GPS:    Signal perdu, acquisition");
					drawString(sgps,34,20,140,1,RA8875_WHITE,RA8875_BLACK);
				}
			}
		}
	}
}

void CMyGraphics::drawBtnChar(uint16_t X,uint16_t Y,uint16_t W,uint16_t H,uint16_t charcol,uint16_t rectcol,uint16_t bg,char* c,uint8_t sz) {
	fillRect(X,Y,W,H,bg);
	drawRect(X,Y,W,H,rectcol);
	drawRect(X+1,Y+1,W-2,H-2,rectcol);
	int16_t tX,tY,l,h;
	l=strlen(c)*8*(sz+1);
	h=16*(sz+1);
	tX=X+W/2-l/2;
	tY=Y+H/2-h/2;
	drawTString(c,strlen(c),tX,tY,sz,charcol);
}

void CMyGraphics::drawSettings(bool mc) {
	char sdate[21];
	uint16_t lp=105,lm=178;
	uint16_t col1=20,col2=92,col3=188,col4=284,col5=356;
	uint16_t lok=530,cok=140;
	if (mc) {
		drawBlack();
		drawString("Configuration",13,140,10,2,RA8875_WHITE,RA8875_BLACK);
		configTime=currentTime;
		drawString("Horloge:",8,20,70,1,RA8875_WHITE,RA8875_BLACK);
		snprintf(sdate,20,"%02u/%02u/%04u %02u:%02u:%02u",configTime.day(),configTime.month(),configTime.year(),configTime.hour(),configTime.minute(),configTime.second());
		drawTString(sdate,20,20,130,2,RA8875_WHITE);
		
		drawBtnChar(col1,lp,48,35,RA8875_YELLOW,RA8875_WHITE,RA8875_BLACK,"+",1);
		drawBtnChar(col1,lm,48,35,RA8875_YELLOW,RA8875_WHITE,RA8875_BLACK,"-",1);
		drawBtnChar(col2,lp,48,35,RA8875_YELLOW,RA8875_WHITE,RA8875_BLACK,"+",1);
		drawBtnChar(col2,lm,48,35,RA8875_YELLOW,RA8875_WHITE,RA8875_BLACK,"-",1);
		drawBtnChar(col3,lp,48,35,RA8875_YELLOW,RA8875_WHITE,RA8875_BLACK,"+",1);
		drawBtnChar(col3,lm,48,35,RA8875_YELLOW,RA8875_WHITE,RA8875_BLACK,"-",1);
		drawBtnChar(col4,lp,48,35,RA8875_YELLOW,RA8875_WHITE,RA8875_BLACK,"+",1);
		drawBtnChar(col4,lm,48,35,RA8875_YELLOW,RA8875_WHITE,RA8875_BLACK,"-",1);
		drawBtnChar(col5,lp,48,35,RA8875_YELLOW,RA8875_WHITE,RA8875_BLACK,"+",1);
		drawBtnChar(col5,lm,48,35,RA8875_YELLOW,RA8875_WHITE,RA8875_BLACK,"-",1);
		drawBtnChar(lok,cok,48,35,RA8875_GREEN,RA8875_WHITE,RA8875_BLACK,"OK",1);

		// pressure/altitude
		drawString("QNH     :",9,20,232,1,RA8875_WHITE,RA8875_BLACK);
		drawBtnChar(290,232,48,32,RA8875_YELLOW,RA8875_WHITE,RA8875_BLACK,"+",1);
		drawBtnChar(340,232,48,32,RA8875_YELLOW,RA8875_WHITE,RA8875_BLACK,"-",1);
		drawString("gps alt :",9,20,264,1,RA8875_WHITE,RA8875_BLACK);
		drawString("pres alt:",9,20,296,1,RA8875_WHITE,RA8875_BLACK);
		drawBtnChar(290,296,74,32,RA8875_YELLOW,RA8875_WHITE,RA8875_BLACK,"auto",1);

	}
	char str[16];
	nstd::mftoa(str,sizeof(str),imu->getCurrentQNH(),1);
	drawString(str,0,164,232,1,RA8875_WHITE,RA8875_BLACK);
	nstd::mftoa(str,sizeof(str),imu->getAltitude(),1);
	drawString(str,0,164,296,1,RA8875_WHITE,RA8875_BLACK);
	if (GPS) {
		nstd::mftoa(str,sizeof(str),GPS->altitude,1);
		drawString(str,0,164,264,1,RA8875_WHITE,RA8875_BLACK);
	}
	// test appui sur bouton
	if (release){
		// +/- QNH
		if ((Y>232)&&(Y<264)){
			if ((X>290)&&(X<338)) imu->adjustQNH(imu->getCurrentQNH()+1);
			if ((X>340)&&(X<388)) imu->adjustQNH(imu->getCurrentQNH()-1);
		}
		// auto QNH
		if ((Y>296)&&(Y<328)){
			if ((X>290)&&(X<364)) imu->alttoQNH(GPS->altitude);
		}
		// les boutons +
		if ((Y>lp)&&(Y<lp+35)){
			if ((X>col1)&&(X<col1+48))configTime=DateTime(configTime.year(),configTime.month(),configTime.day()+1,configTime.hour(),configTime.minute(),configTime.second()); // +1 jour
			if ((X>col2)&&(X<col2+48))configTime=DateTime(configTime.year(),configTime.month()+1,configTime.day(),configTime.hour(),configTime.minute(),configTime.second()); // +1 mois
			if ((X>col3)&&(X<col3+48))configTime=DateTime(configTime.year()+1,configTime.month(),configTime.day(),configTime.hour(),configTime.minute(),configTime.second());; // +1 annee
			if ((X>col4)&&(X<col4+48))configTime=DateTime(configTime.year(),configTime.month(),configTime.day(),configTime.hour()+1,configTime.minute(),configTime.second());; // +1 heure
			if ((X>col5)&&(X<col5+48))configTime=DateTime(configTime.year(),configTime.month(),configTime.day(),configTime.hour(),configTime.minute()+1,configTime.second());; // +1 minute
			fillRect(20,141,400,35,RA8875_BLACK);
		}
		// les boutons -
		if ((Y>lm)&&(Y<lm+35)){
			if ((X>col1)&&(X<col1+48))configTime=DateTime(configTime.year(),configTime.month(),configTime.day()-1,configTime.hour(),configTime.minute(),configTime.second()); // -1 jour
			if ((X>col2)&&(X<col2+48))configTime=DateTime(configTime.year(),configTime.month()-1,configTime.day(),configTime.hour(),configTime.minute(),configTime.second()); // -1 mois
			if ((X>col3)&&(X<col3+48))configTime=DateTime(configTime.year()-1,configTime.month(),configTime.day(),configTime.hour(),configTime.minute(),configTime.second());; // -1 annee
			if ((X>col4)&&(X<col4+48))configTime=DateTime(configTime.year(),configTime.month(),configTime.day(),configTime.hour()-1,configTime.minute(),configTime.second());; // -1 heure
			if ((X>col5)&&(X<col5+48))configTime=DateTime(configTime.year(),configTime.month(),configTime.day(),configTime.hour(),configTime.minute()-1,configTime.second());; // -1 minute
			fillRect(20,141,400,35,RA8875_BLACK);
		}
		// le bouton check
		if ((Y>cok)&&(Y<cok+35)){
			if ((X>lok)&&(X<lok+35)){
				// btncheck pressed
				montre.adjust(configTime);
				drawBlack();
				drawString("Configuration",13,140,10,3,RA8875_WHITE,RA8875_BLACK);
				drawString("Horloge mise a jour.",20,40,170,2,RA8875_WHITE,RA8875_BLACK);
				delay(1000);
				drawButton(menu);
				menu=1;
				drawButton(menu,true);
				drawHome(true);
				return;
			}
		}
		snprintf(sdate,20,"%02u/%02u/%04u %02u:%02u:%02u",configTime.day(),configTime.month(),configTime.year(),configTime.hour(),configTime.minute(),configTime.second());
		drawTString(sdate,20,20,130,2,RA8875_WHITE);
	}
}

void CMyGraphics::drawLines(bool mc) {
	if (mc) { 
		drawBlack();
		drawTString(" GPS  "    ,5 ,40,0,2,RA8875_WHITE);
		drawTString("Latitude :",10,40,46,1,RA8875_WHITE);
		drawTString("Longitude:",10,40,76,1,RA8875_WHITE);
		drawTString("Altitude :",10,40,106,1,RA8875_WHITE);
		drawTString("Vitesse  :",10,40,136,1,RA8875_WHITE);
		drawTString(" IMU  "    ,5 ,40,166,2,RA8875_WHITE);
		drawTString("cap      :",10,40,202,1,RA8875_WHITE);
		drawTString("Tangage  :",10,40,232,1,RA8875_WHITE);
		drawTString("Roulis   :",10,40,262,1,RA8875_WHITE);
		drawTString("Temp     :",10,40,292,1,RA8875_WHITE);
		drawTString("Altitude :",10,40,322,1,RA8875_WHITE);
		drawTString("Pressure :",10,40,352,1,RA8875_WHITE);
		drawTString("Roll Spd :",10,40,382,1,RA8875_WHITE);
		drawTString("Turn Spd :",10,40,412,1,RA8875_WHITE);
		drawTString("Tang Spd :",10,40,442,1,RA8875_WHITE);
	}
	char str[16];
	if (GPS){
		if (updategps){
			nstd::mftoa(str,sizeof(str),GPS->dlat,6);
			drawString(str,15,200,46,1,RA8875_WHITE,RA8875_BLACK);
			nstd::mftoa(str,sizeof(str),GPS->dlon,6);
			drawString(str,15,200,76,1,RA8875_WHITE,RA8875_BLACK);
			nstd::mftoa(str,sizeof(str),GPS->altitude,1);
			drawString(str,15,200,106,1,RA8875_WHITE,RA8875_BLACK);
			nstd::mftoa(str,sizeof(str),GPS->kmspeed,1);
			drawString(str,15,200,136,1,RA8875_WHITE,RA8875_BLACK);
		} else {
			if (lastfix==false) {
				snprintf(str,15,"               ");
			} else {
				snprintf(str,15,"signal perdu   ");
			}
			drawString(str,15,200,46,1,RA8875_WHITE,RA8875_BLACK);
			drawString(str,15,200,76,1,RA8875_WHITE,RA8875_BLACK);
			drawString(str,15,200,106,1,RA8875_WHITE,RA8875_BLACK);
			drawString(str,15,200,136,1,RA8875_WHITE,RA8875_BLACK);
		}
	}

	//IMU
	Cardan3D direction;
	direction=imu->getCurrentOrientation();
	nstd::mftoa(str,sizeof(str),direction.Cap,1);
	drawString(str,15,200,202,1,RA8875_WHITE,RA8875_BLACK);
	nstd::mftoa(str,sizeof(str),direction.Tangage,1);
	drawString(str,15,200,232,1,RA8875_WHITE,RA8875_BLACK);
	nstd::mftoa(str,sizeof(str),direction.Roulis,1);
	drawString(str,15,200,262,1,RA8875_WHITE,RA8875_BLACK);
	nstd::mftoa(str,sizeof(str),imu->getTemperature(),1);
	drawString(str,15,200,292,1,RA8875_WHITE,RA8875_BLACK);
	nstd::mftoa(str,sizeof(str),imu->getAltitude(),1);
	drawString(str,15,200,322,1,RA8875_WHITE,RA8875_BLACK);
	nstd::mftoa(str,sizeof(str),imu->getPression(),1);
	drawString(str,15,200,352,1,RA8875_WHITE,RA8875_BLACK);

	nstd::mftoa(str,sizeof(str),imu->getGyro(1,true),1);
	drawString(str,15,200,382,1,RA8875_WHITE,RA8875_BLACK);
	nstd::mftoa(str,sizeof(str),imu->getGyro(2,true),1);
	drawString(str,15,200,412,1,RA8875_WHITE,RA8875_BLACK);
	nstd::mftoa(str,sizeof(str),imu->getGyro(3,true),1);
	drawString(str,15,200,442,1,RA8875_WHITE,RA8875_BLACK);
}

void CMyGraphics::drawGauges(bool mc) {
	if (mc) { 
		drawBlack();
		//------ debug lines ------------------------
		//  horizontale jaune
		/*drawLine(0,143,600,143,RA8875_YELLOW);
		drawLine(0,367,600,367,RA8875_YELLOW);
		//  verticales jaunes
		drawLine(100,30,100,479,RA8875_YELLOW);
		drawLine(300,30,300,479,RA8875_YELLOW);
		drawLine(500,30,500,479,RA8875_YELLOW);
		//  horizontale bleue
		drawLine(0,30,600,30,RA8875_BLUE);
		drawLine(0,255,600,255,RA8875_BLUE);
		drawLine(0,479,600,479,RA8875_BLUE);
		//  verticale bleue
		drawLine(0,30,0,479,RA8875_BLUE);
		drawLine(200,30,200,479,RA8875_BLUE);
		drawLine(400,30,400,479,RA8875_BLUE);
		drawLine(600,30,600,479,RA8875_BLUE);*/
		//-------------------------------------------
		drawString("Lat:",4,10,10,1,RA8875_WHITE,RA8875_BLACK);
		drawString("Lon:",4,280,10,1,RA8875_WHITE,RA8875_BLACK);
		// speed gauge
		drawCircle(100,143,90,RA8875_WHITE);
		drawCircle(100,143,89,RA8875_WHITE);
		fillRect(41,210,118,40,RA8875_BLACK);
		drawLine( 27,196, 35,190,RA8875_WHITE);
		drawLine( 11,156, 21,154,RA8875_WHITE);
		drawLine( 15,112, 25,116,RA8875_WHITE);
		drawLine( 40, 76, 46, 84,RA8875_WHITE);
		drawLine( 78, 56, 81, 65,RA8875_WHITE);
		drawLine(122, 56,119, 65,RA8875_WHITE);
		drawLine(160, 76,154, 84,RA8875_WHITE);
		drawLine(185,112,175,116,RA8875_WHITE);
		drawLine(189,156,179,154,RA8875_WHITE);
		drawLine(173,196,165,190,RA8875_WHITE);
		drawLine( 39,216, 49,204,RA8875_WHITE);
		drawString("0"  ,1,40 ,200,0,RA8875_WHITE,RA8875_BLACK);
		drawLine( 12,179, 26,173,RA8875_WHITE);
		drawString("20" ,2,8 ,168,0,RA8875_WHITE,RA8875_BLACK);
		drawLine(  6,133, 20,135,RA8875_WHITE);
		drawString("40" ,2,0 ,125,0,RA8875_WHITE,RA8875_BLACK);
		drawLine( 21, 90, 34, 98,RA8875_WHITE);
		drawString("60" ,2,18 ,82 ,0,RA8875_WHITE,RA8875_BLACK);
		drawLine( 55, 59, 62, 72,RA8875_WHITE);
		drawString("80" ,2,53 ,53 ,0,RA8875_WHITE,RA8875_BLACK);
		drawLine(100, 48,100, 63,RA8875_WHITE);
		drawString("100",3,90 ,45 ,0,RA8875_WHITE,RA8875_BLACK);
		drawLine(145, 59,138, 72,RA8875_WHITE);
		drawString("120",3,133,53 ,0,RA8875_WHITE,RA8875_BLACK);
		drawLine(179, 90,166, 98,RA8875_WHITE);
		drawString("140",3,158,82 ,0,RA8875_WHITE,RA8875_BLACK);
		drawLine(194,133,180,135,RA8875_WHITE);
		drawString("160",3,170,125,0,RA8875_WHITE,RA8875_BLACK);
		drawLine(188,179,174,173,RA8875_WHITE);
		drawString("180",3,162,168,0,RA8875_WHITE,RA8875_BLACK);
		drawLine(161,216,151,204,RA8875_WHITE);
		drawString("200",3,150,200,0,RA8875_WHITE,RA8875_BLACK);
		
		// indicateur de gite
		drawCircle(100,367,90,RA8875_WHITE);
		drawCircle(100,367,89,RA8875_WHITE);
		fillRect(41,260,118,40,RA8875_BLACK);
		fillRect(41,420,118,40,RA8875_BLACK);
		drawLine( 39,440, 49,428,RA8875_WHITE);
		drawLine( 18,415, 31,407,RA8875_WHITE);
		drawLine( 11,399, 25,394,RA8875_WHITE);
		 drawLine( 13,390, 23,388,RA8875_WHITE);
		drawLine(  6,383, 21,381,RA8875_WHITE);
		 drawLine( 10,375, 20,374,RA8875_WHITE);
		drawLine(  5,367, 20,367,RA8875_WHITE);
		 drawLine( 10,359, 20,360,RA8875_WHITE);
		drawLine(  6,351, 21,353,RA8875_WHITE);
		 drawLine( 13,344, 23,346,RA8875_WHITE);
		drawLine( 11,335, 25,340,RA8875_WHITE);
		drawLine( 18,320, 31,327,RA8875_WHITE);
		drawLine( 39,294, 49,306,RA8875_WHITE);
		drawLine(161,440,151,428,RA8875_WHITE);
		drawLine(182,415,169,407,RA8875_WHITE);
		drawLine(189,399,175,394,RA8875_WHITE);
		 drawLine(187,390,177,388,RA8875_WHITE);
		drawLine(194,383,179,381,RA8875_WHITE);
		 drawLine(190,359,180,360,RA8875_WHITE);
		drawLine(195,367,180,367,RA8875_WHITE);
		 drawLine(190,375,180,374,RA8875_WHITE);
		drawLine(194,351,179,353,RA8875_WHITE);
		 drawLine(187,344,177,346,RA8875_WHITE);
		drawLine(189,335,175,340,RA8875_WHITE);
		drawLine(182,320,169,327,RA8875_WHITE);
		drawLine(161,294,151,306,RA8875_WHITE);
		
		// indicateur de tangage
		drawCircle(300,367,90,RA8875_WHITE);
		drawCircle(300,367,89,RA8875_WHITE);
		fillRect(241,260,118,40,RA8875_BLACK);
		fillRect(241,420,118,40,RA8875_BLACK);
		drawLine(239,440,249,428,RA8875_WHITE);
		drawLine(218,415,231,407,RA8875_WHITE);
		drawLine(211,399,225,394,RA8875_WHITE);
		 drawLine(213,390,223,388,RA8875_WHITE);
		drawLine(206,383,221,381,RA8875_WHITE);
		 drawLine(210,375,220,374,RA8875_WHITE);
		drawLine(205,367,220,367,RA8875_WHITE);
		 drawLine(210,359,220,360,RA8875_WHITE);
		drawLine(206,351,221,353,RA8875_WHITE);
		 drawLine(213,344,223,346,RA8875_WHITE);
		drawLine(211,335,225,340,RA8875_WHITE);
		drawLine(218,320,231,327,RA8875_WHITE);
		drawLine(239,294,249,306,RA8875_WHITE);
		drawLine(361,440,351,428,RA8875_WHITE);
		drawLine(382,415,369,407,RA8875_WHITE);
		drawLine(389,399,375,394,RA8875_WHITE);
		 drawLine(387,390,377,388,RA8875_WHITE);
		drawLine(394,383,379,381,RA8875_WHITE);
		 drawLine(390,359,380,360,RA8875_WHITE);
		drawLine(395,367,380,367,RA8875_WHITE);
		 drawLine(390,375,380,374,RA8875_WHITE);
		drawLine(394,351,379,353,RA8875_WHITE);
		 drawLine(387,344,377,346,RA8875_WHITE);
		drawLine(389,335,375,340,RA8875_WHITE);
		drawLine(382,320,369,327,RA8875_WHITE);
		drawLine(361,294,351,306,RA8875_WHITE);
		
		// boussole
		drawCircle(300,143,90,RA8875_WHITE);
		drawCircle(300,143,89,RA8875_WHITE);
		drawLine(367, 76,357, 86,RA8875_WHITE);
		drawLine(233, 76,243, 86,RA8875_WHITE);
		drawLine(233,210,243,200,RA8875_WHITE);
		drawLine(367,210,357,200,RA8875_WHITE);
		drawString("N",1,295,45 ,0,RA8875_RED,RA8875_BLACK);
		drawString("S",1,295,225,0,RA8875_WHITE,RA8875_BLACK);
		drawString("O",1,206,135,0,RA8875_WHITE,RA8875_BLACK);
		drawString("E",1,385,135,0,RA8875_WHITE,RA8875_BLACK);
	}
	char str[16];
	float spd=0;
	if (GPS){
		if (updategps){
			nstd::mftoa(str,sizeof(str),GPS->dlat,6);
			drawString(str,0,74,10,1,RA8875_WHITE,RA8875_BLACK);
			nstd::mftoa(str,sizeof(str),GPS->dlon,6);
			drawString(str,0,344,10,1,RA8875_WHITE,RA8875_BLACK);
		}
		spd=GPS->kmspeed;
	}
	uint16_t X1,Y1,X2,Y2,X3,Y3,X4,Y4;
	if (millis()-timer2>100){
	// dessin de l'aiguille de vitesse
		fillCircle(100,143,71,RA8875_BLACK);
		// angle min=-230 angle max 50 vitesse max 200km/h
		float angle=(-230+1.4*spd)/180.0*PI;
		X1=100+70*cos(angle);Y1=143+70*sin(angle);
		X2=100+5*cos(angle+PI/2.0);Y2=143+5*sin(angle+PI/2.0);
		X3=100+5*cos(angle-PI/2.0);Y3=143+5*sin(angle-PI/2.0);
		fillTriangle(X1,Y1,X2,Y2,X3,Y3,RA8875_RED);
		fillCircle(100,143,10,RA8875_YELLOW);
	// dessin de l'aiguille de la boussole
		fillCircle(300,143,71,RA8875_BLACK);
		Cardan3D direction;
		direction=imu->getCurrentOrientation();
		angle=(direction.Cap+90)*PI/180.0;
		X1=300+70*cos(angle-PI);Y1=143+70*sin(angle-PI);
		X2=300+12*cos(angle+PI/2.0);Y2=143+12*sin(angle+PI/2.0);
		X3=300+12*cos(angle-PI/2.0);Y3=143+12*sin(angle-PI/2.0);
		X4=300+70*cos(angle);Y4=143+70*sin(angle);
		fillTriangle(X1,Y1,X2,Y2,X3,Y3,RA8875_RED);
		drawTriangle(X4,Y4,X2,Y2,X3,Y3,RA8875_WHITE);
	// dessin de l'aiguille de roulis (gauche)
		fillCircle(100,367,71,RA8875_BLACK);
		angle=(-direction.Roulis+90)*PI/180.0;
		X1=100+70*cos(angle-PI);Y1=367+70*sin(angle-PI);
		X2=100+12*cos(angle+PI/2.0);Y2=367+12*sin(angle+PI/2.0);
		X3=100+12*cos(angle-PI/2.0);Y3=367+12*sin(angle-PI/2.0);
		X4=100+50*cos(angle);Y4=367+50*sin(angle);
		drawTriangle(X1,Y1,X2,Y2,X3,Y3,RA8875_WHITE);
		drawTriangle(X4,Y4,X2,Y2,X3,Y3,RA8875_WHITE);
		X2=100+70*cos(angle+PI/2.0);Y2=367+70*sin(angle+PI/2.0);
		X3=100+70*cos(angle-PI/2.0);Y3=367+70*sin(angle-PI/2.0);
		drawLine(X2,Y2,X3,Y3,RA8875_RED);
	// dessin de l'aiguille de tangage (droite)
		fillCircle(300,367,71,RA8875_BLACK);
		angle=(direction.Tangage+90)*PI/180.0;
		X1=300+30*cos(angle-PI);Y1=367+30*sin(angle-PI);
		X2=300+60*cos(angle+PI/2.0);Y2=367+60*sin(angle+PI/2.0);
		X3=300+40*cos(angle-PI/2.0);Y3=367+40*sin(angle-PI/2.0);
		X4=300+20*cos(angle);Y4=367+20*sin(angle);
		drawTriangle(X1,Y1,X2,Y2,X3,Y3,RA8875_WHITE);
		drawTriangle(X4,Y4,X2,Y2,X3,Y3,RA8875_WHITE);
		X2=300+70*cos(angle+PI/2.0);Y2=367+70*sin(angle+PI/2.0);
		X3=300+70*cos(angle-PI/2.0);Y3=367+70*sin(angle-PI/2.0);
		drawLine(X2,Y2,X3,Y3,RA8875_RED);
		timer2=millis();
	}
}

void CMyGraphics::drawBlack() {
	fillRect(0,0,600,480, RA8875_BLACK);
}

uint8_t CMyGraphics::DateToString(char* str,DateTime  t) {
	const char* jours[] = { "Dimanche", "Lundi", "Mardi", "Mercredi", "Jeudi", "Vendredi", "Samedi" };
	const uint8_t ljour[] = { 8, 5, 5, 8, 5, 8, 6 };
	const char* mois[] = { "Janvier", "Fevrier", "Mars", "Avril", "Mai", "Juin", "Juillet", "Aout", "Septembre", "Octobre", "Novembre", "Decembre" };
	const uint8_t lmois[] = { 7, 7, 4, 5, 3, 4, 7, 4, 9, 7, 8, 8 };
	uint8_t longu;
	longu=18+ljour[t.dayOfWeek()]+lmois[t.month()-1];
	//      max    35  // sans les noms 18
	char blanc[17];
	int i;
	for(i=0;i+longu<35;i++)blanc[i]=' ';
	blanc[i]='\0';
	snprintf(str,35,"%s %02u %s %04u %02u:%02u:%02u%s",jours[t.dayOfWeek()],t.day(),mois[t.month()-1],t.year(),t.hour(),t.minute(),t.second(),blanc);
	return 35;
}

void CMyGraphics::attachGPS(Adafruit_GPS *gps) {
	GPS=gps;
}

void CMyGraphics::attachIMU(CMyIMU *_imu) {
	imu=_imu;
}

void CMyGraphics::toggleLoggeur(void) {
	ArduinoOutStream fout(filelog);
	if( logon ){
		logon=false;
		fout << "------ FIN ------" << endl;
		filelog.close();
		extconn->sendCommand("Fin Log");
	} else {
		logon=true;
		char fname[17];
		snprintf(fname,17,"log/%02d%02d%02d.log",currentTime.year()-2000,currentTime.month(),currentTime.day());
		if (sd.exists(fname)) {
			uint8_t i=0;
			while(sd.exists(fname)&&i<10){
				snprintf(fname,17,"log/%02d%02d%02d_%d.log",currentTime.year()-2000,currentTime.month(),currentTime.day(),i);
				i++;
			}
			if (i>=10) {
				logon=false;
				extconn->sendCommand("ERROR No more log today!");
				return;
			}
		}
		filelog.open(fname, O_RDWR | O_CREAT | O_AT_END);
		if (!filelog.isOpen()){
			extconn->sendCommand("ERROR opening log file");
			logon=false;
			return;
		}
		fout << "Date Time\t"; 
		fout << "GPS fixe\tLatitude\tLongitude\tAltitude GPS\tVitesse\tAngle\tvariation magnetique\tPrecision\tsatelites\tgeoaltitude\t";
		fout << "timestamp\tGyroX\tGyroY\tGyroZ\tAccelX\tAccelY\tAccelZ\tmagnetX\tmagnetY\tmagnetZ\tTemerature\tpression\taltitude";
		fout << endl;
		fout << "----- DEBUT -----" << endl;
		extconn->sendCommand("Debut Log");
	}
}

void CMyGraphics::logFile(void) {
	ArduinoOutStream fout(filelog);
	char strlat[25],strlon[25];
	char str2[20];
	
	if (logon){
		if (millis()-timerlog>loginterval){
			timerlog=millis();
			snprintf(str2,20,"%4d/%02d/%02d %02d:%02d:%02d",currentTime.year(),currentTime.month(),currentTime.day(),currentTime.hour(),currentTime.minute(),currentTime.second());
			fout<< str2 << '\t';
			if (GPS->fix) {
				nstd::mftoa(strlat,24,GPS->dlat,6);
				nstd::mftoa(strlon,24,GPS->dlon,6);
				fout<< int(GPS->fix) << '\t' << strlat << '\t' << strlon << '\t' << GPS->altitude << '\t' << GPS->kmspeed << '\t'
					<< GPS->angle << '\t' << GPS->magvariation << '\t' << GPS->HDOP << '\t' << (int)GPS->satellites << '\t' << GPS->geoidheight << '\t';
			}else{
				fout<< int(GPS->fix) << '\t' << "0" << '\t' << "0" << '\t' << "0" << '\t' << "0" << '\t'
					<< "0" << '\t' << "0" << '\t' << "0" << '\t'  << "0" << '\t' << "0" << '\t' ;

			}
			fout<< imu->getTimeStamp() << "\t" << imu->getGyro(1) << "\t" << imu->getGyro(2) << "\t" << imu->getGyro(3) << "\t" << imu->getAccel(1) << "\t" << imu->getAccel(2) << "\t" << imu->getAccel(3) << "\t" << imu->getMagnetic(1) << "\t" << imu->getMagnetic(2) << "\t" << imu->getMagnetic(3) << "\t" << imu->getTemperature() << "\t" << imu->getPression() << "\t" << imu->getAltitude();
			fout<< endl;
		}
	}
}

void CMyGraphics::getFingerPos(uint16_t* fX,uint16_t* fY,boolean* touch,boolean* relea) {
	*fX=X;
	*fY=Y;
	*touch=_touched;
	*relea=release;
}


