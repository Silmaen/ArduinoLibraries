/**
 * \file CImageButton.h
 *
 * \date 3 août 2014
 * \author dla
 * \brief class for buton with images
 */
#ifndef CIMAGEBUTTON_H_
#define CIMAGEBUTTON_H_

#include "CButton.h"

class CImageButton: public CButton {
public:
	/**
	 * \brief default constructor
	 * \param p
	 */
	CImageButton(CMyGraphics* p);
	/**
	 * \brief destructor
	 */
	virtual ~CImageButton();
	/**
	 * \brief set image for the button
	 * \param filename the filename for the image
	 */
	void setImage(char* filename);
	/**
	 * \brief set the image for the second state of the button
	 * \param filename the filename of the image
	 */
	void setSecondImage(char* filename);
	/**
	 * \brief routines that draw the button
	 */
	void draw();
	/**
	 * \brief toggle between the two state images
	 */
	void toggleImage();
private:
	uint8_t img;//!< indice of the image to draw
	char img1[50];//!< filename for the first image
	char img2[50];//!< filename for the second image
};

#endif /* CIMAGEBUTTON_H_ */
