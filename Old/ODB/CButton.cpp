/**
 * \file CButton.cpp
 *
 * \date 3 août 2014
 * \author dla
 * \brief class générique de définition d'un bouton
 */
#include "CButton.h"
#include "CMyGraphics.h"

CButton::CButton(CMyGraphics* p) {
	parent=p;
	touched=false;
	pressed=false;
	todraw=false;
	posX=0;
	posY=0;
	width=0;
	height=0;
	visible=false;
	enable=false;
}

CButton::CButton(CMyGraphics* p,uint16_t X,uint16_t Y,uint16_t W,uint16_t H){
	parent=p;
	touched=false;
	pressed=false;
	todraw=false;
	posX=X;
	posY=Y;
	width=W;
	height=H;
	visible=false;
	enable=false;
}

CButton::~CButton() {

}

void CButton::frame() {
	touched=false;
	pressed=false;
	if (enable) {
		uint16_t FX,FY;
		boolean touch,release;
		parent->getFingerPos(&FX,&FY,&touch,&release);
		if (release) {
			if ((FX>=posX)&&(FX<=posX+width)&&(FY>=posY)&&(FY<=posY+height)) {
				touched=true;
			}
		}
		if (touch) {
			if ((FX>=posX)&&(FX<=posX+width)&&(FY>=posY)&&(FY<=posY+height)) {
				pressed=true;
			}
		}
	}
}

boolean CButton::isTouched() {
	return touched;
}

boolean CButton::isPressed() {
	return pressed;
}

void CButton::setXY(uint16_t X,uint16_t Y) {
	posX=X;
	posY=Y;
}

void CButton::setDimension(uint16_t W,uint16_t H) {
	width=W;
	height=H;
}

void CButton::setVisible(boolean vis) {
	visible=vis;
	if (!vis) enable=false;
}

void CButton::setEnable(boolean en) {
	enable=en;
}


