/**
 * \file CTextButton.h
 *
 * \date 3 août 2014
 * \author dla
 * \brief button with a text inside
 */
#ifndef CTEXTBUTTON_H_
#define CTEXTBUTTON_H_

#include "CButton.h"

class CTextButton: public CButton {
public:
	/**
	 * \brief   Constructor
	 * \param p parent graphic system
	 */
	CTextButton(CMyGraphics* p);
	/**
	 * \brief destructor
	 */
	virtual ~CTextButton();
	/**
	 * \brief defines the color of the button
	 * \param fore foreground color
	 * \param bg  background color
	 */
	void setColors(uint16_t fore,uint16_t bg);
	/**
	 * \brief defines the text inside and its color
	 * \param text the text of the button
	 * \param size the size of the text
	 * \param col the color of the text
	 */
	void setCaption(char* text,uint8_t size,uint16_t col);
	/**
	 * \brief the drawing function
	 */
	void draw();
private:
	uint16_t foreground; //!< foreground color
	uint16_t background; //!< background color
	uint16_t tcol; //!< the color of the text
	uint8_t sz; //!< size of the text
	char caption[20];//!< the text of the button
};

#endif /* CTEXTBUTTON_H_ */
