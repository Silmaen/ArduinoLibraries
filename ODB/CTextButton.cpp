/**
 * \file CTextButton.cpp
 *
 * \date 3 août 2014
 * \author dla
 * \brief button with a text inside
 */
#include "CTextButton.h"
#include "CMyGraphics.h"


CTextButton::CTextButton(CMyGraphics* p):CButton(p) {
	foreground=RA8875_WHITE;
	background=RA8875_BLACK;
	tcol=RA8875_WHITE;
	for(char i=0;i<20;i++){
		caption[i]='\0';
	}
	sz=0;
}

CTextButton::~CTextButton() {

}

void CTextButton::setColors(uint16_t fore,uint16_t bg) {
	foreground=fore;
	background=bg;
}

void CTextButton::setCaption(char* text,uint8_t size,uint16_t col) {
	tcol=col;
	sz=size;
	bool fini=false;
	for(char i=0;i<20;i++){
		if (i==19) fini=true;
		if (!fini) {
			if (text[i]=='\0') fini=true;
		}
		if (fini)
			caption[i]='\0';
		else
			caption[i]=text[i];
	}
}

void CTextButton::draw() {
	parent->drawRect(posX,posY,width,height,foreground);
	parent->fillRect(posX+1,posY+1,width-2,height-2,background);
	int16_t X,Y,l,h;
	l=strlen(caption)*8*(sz+1);
	h=16*(sz+1);
	X=posX+width/2;
}




