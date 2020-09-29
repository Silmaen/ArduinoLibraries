/**
 * \file CButton.h
 *
 * \date 3 août 2014
 * \author dla
 * \brief Class générique pour la définition d'un bouton
 */
#ifndef CBUTTON_H_
#define CBUTTON_H_

#if (ARDUINO >= 100)
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif


class CMyGraphics;

class CButton {
public:
	/**
	 * \brief base constructor
	 * \param p the parent graphic system
	 */
	CButton(CMyGraphics* p);
	/**
	 * \brief complete constructoe
	 * \param p the parent graphic system
	 * \param X the hoirizontal position
	 * \param Y the vertical position
	 * \param W the width
	 * \param H the height
	 */
	CButton(CMyGraphics* p,uint16_t X,uint16_t Y,uint16_t W,uint16_t H);
	/**
	 * \brief destructor
	 */
	virtual ~CButton();
	/**
	 * \brief should be called for updated status
	 */
	virtual void frame();
	/**
	 * \brief  defines if the button is touched
	 * \return true if the button is touched
	 */
	boolean isTouched();
	/**
	 * \brief  defines if the button is pressed (touched then released)
	 *         this function will also reset the status!
	 * \return true if the button is touched
	 */
	boolean isPressed();
	/**
	 * \brief define the position of the button
	 * \param X the horizontal postion
	 * \param Y the vertical position
	 */
	void setXY(uint16_t X,uint16_t Y);
	/**
	 * \brief   define the size of the button
	 * \param W the width of the button
	 * \param H the height of the button
	 */
	void setDimension(uint16_t W,uint16_t H);
	/**
	 * \brief define if the button is visible in the current context
	 * \param vis true if the button should be visible false else
	 */
	void setVisible(boolean vis);
	/**
	 * \brief define if the button is enable or disable but still visible
	 * \param en true for enable false for disable
	 */
	void setEnable(boolean en);
	/**
	 * \brief the drawing function
	 */
	virtual void draw()=0;
protected:
	CMyGraphics* parent; //!< pointer to the parent graphic system
	boolean touched;//!< touched status
	boolean pressed;//!< pressed status
	boolean todraw; //!< if the button should be draw or redraw at the next frame
	uint16_t posX; //!< horizontal position of the button
	uint16_t posY; //!< vertical position of the button
	uint16_t width; //!< the width of the button
	uint16_t height; //!< the height of the button
	boolean visible; //!< define if the button is visible
	boolean enable; //!< define if the button is currently active
};

#endif /* CBUTTON_H_ */
