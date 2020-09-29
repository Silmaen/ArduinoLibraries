/**
 * \file CImageButton.cpp
 *
 * \date 3 août 2014
 * \author dla
 */

#include "CImageButton.h"
#include "CMyGraphics.h"

CImageButton::CImageButton(CMyGraphics* p):CButton(p) {
	img=0;
	for(char i=0;i<50;i++){
		img1[i]='\0';
		img2[i]='\0';
	}
}

CImageButton::~CImageButton() {

}

void CImageButton::setImage(char* filename) {
	bool fini=false;
	for(char i=0;i<50;i++){
		if (i==49) fini=true;
		if (!fini) {
			if (filename[i]=='\0') fini=true;
		}
		if (fini)
			img1[i]='\0';
		else
			img1[i]=filename[i];
	}
}

void CImageButton::setSecondImage(char* filename) {
	bool fini=false;
	for(char i=0;i<50;i++){
		if (i==49) fini=true;
		if (!fini) {
			if (filename[i]=='\0') fini=true;
		}
		if (fini)
			img2[i]='\0';
		else
			img2[i]=filename[i];
	}
}

void CImageButton::draw() {
	if (img==0) parent->drawpicture(img1,posX,posY,width,height);
	if (img==1) parent->drawpicture(img1,posX,posY,width,height);
}

void CImageButton::toggleImage() {
	if ((img=0)&&(img2!="")){
		img=1;
	}
	if (img==1){
		img=0;
	}
}


