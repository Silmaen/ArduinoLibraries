#ifndef _RA8875_DRIVER_H
#define _RA8875_DRIVER_H

#if ARDUINO >= 100
 #include "Arduino.h"
 #include "Print.h"
#else
 #include "WProgram.h"
#endif
#include "SdSpi.h"

#define swap(a, b) { int16_t t = a; a = b; b = t; }

//------------------------------------------------------------------------------
/** init timeout ms */
uint16_t const TFT_INIT_TIMEOUT = 2000;
/** erase timeout ms */
uint16_t const TFT_ERASE_TIMEOUT = 10000;
/** read timeout ms */
uint16_t const TFT_READ_TIMEOUT = 300;
/** write time out ms */
uint16_t const TFT_WRITE_TIMEOUT = 600;

// Sizes!
enum RA8875sizes { RA8875_480x272, RA8875_800x480 };

// speeds
enum RA8875spirate { RA8875_SPI_HALF_SPEED, RA8875_SPI_FULL_SPEED };

// GPU clocking
enum RA8875GPUclock { RA8875_GPU_STD_CLK, RA8875_GPU_DBL_CLK, RA8875_GPU_QUA_CLK };

// !!! IMPORTANT !!!
// this is the supposed frequency of the crystal. this value is used to compute the system clock!
#define RA8875_FIN 20

// Touchpad reactivity
enum RA8875touchspd { RA8875_TOUCH_STD_SPD, RA8875_TOUCH_DBL_SPD };

// Touch screen cal structs
typedef struct Point {
  int32_t x;
  int32_t y;
} tsPoint_t;

typedef struct Matrix {
  int32_t An,
          Bn,
          Cn,
          Dn,
          En,
          Fn,
          Divider ;
} tsMatrix_t;

class RA8875_driver : public Print {
public:
	/**
	 *  \brief Constructor
	 *  \param CS   pin number for CS cable
	 *  \param RST  pin number for RST cable
	 *  \param INT  pin number for INT cable
	 *  \param WAIT pin number for WAIT cable
	 */
	RA8875_driver(uint8_t CS, uint8_t RST,uint8_t INT, uint8_t WAIT);
	/**
	 *  \brief Start the screen and the touchpad
	 *  \param s    The display size, which can be either:
     *               - 'RA8875_480x272' (4.3" displays) r
     *               - 'RA8875_800x480' (5" and 7" displays)
	 *  \param spd  The speed rate of the SPI communication
	 *               - 'RA8875_SPI_HALF_SPEED' for 4MHz bus speed (used for compatibility)
	 *               - 'RA8875_SPI_FULL_SPEED' for 8MHz bus speed (for more performances)
	 *  \param gclk The different configuration for system clock:
	 *               - 'RA8875_GPU_STD_CLK' the standard configuration (55MHz if RA8875_FIN=20)
	 *               - 'RA8875_GPU_DBL_CLK' the double configuration, recomended with SPI_FULL_SPPED (105MHz if RA8875_FIN=20)
	 *  \return returns false if something goes wrong, else returns true
	 */
	boolean begin(enum RA8875sizes s,enum RA8875spirate spd,enum RA8875GPUclock gclk);
	/**
	 *  \brief perform a soft reset of the screen (not using the designated pin)
	 */
	void    softReset(void);
	/**
	 *  \brief Set the display on or off
	 *  \param on the status of the display
	 */
	void    displayOn(boolean on);
	/**
	 *  \brief set the screen in sleep mode
	 *  \brief sleep the sleep mode of the screen
	 */
	void    sleep(boolean sleep);

	/* Backlight */
	/**
	 *  \brief set the GPIO in read or write mode
	 *  \param on : false = read mode true = write mode
	 */
	void    GPIOX(boolean on);
	/**
	 *  \brief configue the first chanel of backlight
	 *  \param on enable or disable the chanel
	 *  \param clock the clock of the backlick  as PWM1_CLK=SYS_CLK/2^clock
	 */
	void    PWM1config(boolean on, uint8_t clock);
	/**
	 *  \brief configue the second chanel of backlight
	 *  \param on enable or disable the chanel
	 *  \param clock the clock of the backlick  as PWM1_CLK=SYS_CLK/2^clock
	 */
	void    PWM2config(boolean on, uint8_t clock);
	/**
	 * \brief set backlight intensity for channel 1
	 * \param p the intensity
	 */
	void    PWM1out(uint8_t p);
	/**
	 * \brief set backlight intensity for channel 2
	 * \param p the intensity
	 */
	void    PWM2out(uint8_t p);

	/* Touch screen */
	/**
	 *  \brief activate or deactivate the touchpad
	 *  \param on  true: activate the touchpad false: deactivate
	 *  \param spd the response speed of ther touchpad:
	 *              - 'RA8875_TOUCH_STD_SPD' standard reactivity of the pad
	 *              - 'RA8875_TOUCH_DBL_SPD' double speed
	 */
	void    touchEnable(boolean on,enum RA8875touchspd spd);
	/**
	 *  \brief check if a touch event has occured
	 *  \return True is a touch event has occured (reading it via
     *           touchRead() will clear the interrupt in memory)
	 */
	boolean touched(void);
	/**
	 *  \brief  reads the last touch event
	 *  \param x Pointer to the uint16_t field to assign the raw X value
	 *  \param y Pointer to the uint16_t field to assign the raw Y value
	 *
	 *  \note Calling this function will clear the touch panel interrupt on
     *      the RA8875, resetting the flag used by the 'touched' function
	 */
	boolean touchRead(uint16_t *x, uint16_t *y);

	/* Text functions */
	/**
	 *  \brief Sets the display in text mode (as opposed to graphics mode)
	 */
	void    textMode(void);
	/**
	 *  \brief Sets the display in text mode (as opposed to graphics mode)
	 *  \param x The x position of the cursor (in pixels, 0..1023)
     *  \param y The y position of the cursor (in pixels, 0..511)
	 */
	void    textSetCursor(uint16_t x, uint16_t y);
	/**
	 *  \brief Sets the fore and background color when rendering text
	 *
	 *  \param foreColor The RGB565 color to use when rendering the text
	 *  \param bgColor   The RGB565 colot to use for the background
	 */
	void    textColor(uint16_t foreColor, uint16_t bgColor);
	/**
	 *   \brief Sets the fore color when rendering text with a transparent bg
	 *   \param foreColor The RGB565 color to use when rendering the text
	 */
	void    textTransparent(uint16_t foreColor);
	/**
	 *   \brief Sets the text enlarge settings, using one of the following values:
	 *           0 = 1x zoom
	 *           1 = 2x zoom
	 *           2 = 3x zoom
	 *           3 = 4x zoom
	 * 
	 *   \param scale The zoom factor (0..3 for 1-4x zoom)
	 */
	void    textEnlarge(uint8_t scale);
	/** 
	 *  \brief Renders some text on the screen when in text mode
	 *  \param buffer    The buffer containing the characters to render
	 *  \param len       The size of the buffer in bytes
	 */
	void    textWrite(const char* buffer, uint16_t len=0);
	/**
	 * \brief Draw a string on a background
	 * \param str   the string to write
	 * \param len   the length of the string
	 * \param X     the horizontal position (in pixel) of the string
	 * \param Y     the vertical position (in pixel) of the string
	 * \param size  the scale factor of the string
	 * \param color the color of the character
	 * \param bg    the background color
	 */
	void    drawString(char* str,uint8_t len,uint16_t X,uint16_t Y,uint8_t size,uint16_t color,uint16_t bg);
	/**
	 * \brief draw a string with transparent background
	 * \param str   the string to write
	 * \param len   the length of the string
	 * \param X     the horizontal position (in pixel) of the string
	 * \param Y     the vertical position (in pixel) of the string
	 * \param size  the scale factor of the string
	 * \param color the color of the character
	 */
	void    drawTString(char* str,uint8_t len,uint16_t X,uint16_t Y,uint8_t size,uint16_t color);
	/* Graphics functions */
	/**
	 *  \brief Sets the display in graphics mode (as opposed to text mode)
	 */
	void    graphicsMode(void);
	/**
	 *   \brief Sets the current X/Y position on the display before drawing
	 *   \param x The 0-based x location
	 *   \param y The 0-based y location
	 */
	void    setXY(uint16_t x, uint16_t y);
	/**
	 *   \brief HW accelerated function to push a chunk of raw pixel data 
	 *   \param num The number of pixels to push
	 *   \param p   The pixel color to use
	 */
	void    pushPixels(uint32_t num, uint16_t p);
	/**
	 *   \brief Draws a single pixel at the specified location
	 *   \param x     The 0-based x location
	 *   \param y     The 0-base y location
	 *   \param color The RGB565 color to use when drawing the pixel
	 */
	void    drawPixel(int16_t x, int16_t y, uint16_t color);
#ifdef SOFTCHAR
	/**

	 * \brief draw a char at the screen (very slow)
	 * \param x     horizontal coordinate for the text
	 * \param y     vertical coordinate for the text
	 * \param c     the char to draw
	 * \param color the color of the char
	 * \param bg    the background color
	 * \param size  the size of the char (multiplier)
	 */
	void drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size);
#endif
	/**
	 *   \brief Draws a HW accelerated line on the display
	 *   \param x0    The 0-based starting x location
	 *   \param y0    The 0-base starting y location
	 *   \param x1    The 0-based ending x location
	 *   \param y1    The 0-base ending y location
	 *   \param color The RGB565 color to use when drawing the pixel
	 */
	void    drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
	/**
	 *   \brief Draws a HW accelerated vertical line on the display
	 *   \param x     The 0-based starting x location
	 *   \param y     The 0-base starting y location
	 *   \param h     The length of the line
	 *   \param color The RGB565 color to use when drawing the pixel
	 */
	void    drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
	/**
	 *   \brief Draws a HW accelerated horizontal line on the display
	 *   \param x     The 0-based starting x location
	 *   \param y     The 0-base starting y location
	 *   \param w     The length of the line
	 *   \param color The RGB565 color to use when drawing the pixel
	 */
	void    drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);

	/* HW accelerated wrapper functions */
	/**
	 *   \brief Fills the screen with the spefied RGB565 color
	 *   \param color The RGB565 color to use when drawing the pixel
	 */
	void    fillScreen(uint16_t color);
	/**
	 *   \brief Draws a HW accelerated rectangle on the display
	 *   \param x     The 0-based x location of the top-right corner
	 *   \param y     The 0-based y location of the top-right corner
	 *   \param w     The rectangle width
	 *   \param h     The rectangle height
	 *   \param color The RGB565 color to use when drawing the pixel
	 */
	void    drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
	/**
	 *   \brief Draws a HW accelerated filled rectangle on the display
	 *   \param x     The 0-based x location of the top-right corner
	 *   \param y     The 0-based y location of the top-right corner
	 *   \param w     The rectangle width
	 *   \param h     The rectangle height
	 *   \param color The RGB565 color to use when drawing the pixel
	 */
	void    fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
	/**
	 *   \brief Draws a HW accelerated circle on the display
	 *   \param x     The 0-based x location of the center of the circle
	 *   \param y     The 0-based y location of the center of the circle
	 *   \param w     The circle's radius
	 *   \param color The RGB565 color to use when drawing the pixel
	 */
	void    drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
	/**
	 *   \brief Draws a HW accelerated circle on the display
	 *   \param x     The 0-based x location of the center of the circle
	 *   \param y     The 0-based y location of the center of the circle
	 *   \param w     The circle's radius
	 *   \param color The RGB565 color to use when drawing the pixel
	 */
	void    fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
	/**
	 *   \brief Draws a HW accelerated triangle on the display
	 *   \param x0    The 0-based x location of point 0 on the triangle
	 *   \param y0    The 0-based y location of point 0 on the triangle
	 *   \param x1    The 0-based x location of point 1 on the triangle
	 *   \param y1    The 0-based y location of point 1 on the triangle
	 *   \param x2    The 0-based x location of point 2 on the triangle
	 *   \param y2    The 0-based y location of point 2 on the triangle
	 *   \param color The RGB565 color to use when drawing the pixel
	 */
	void    drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
	/**
	 *   \brief Draws a HW accelerated filled triangle on the display
	 *   \param x0    The 0-based x location of point 0 on the triangle
	 *   \param y0    The 0-based y location of point 0 on the triangle
	 *   \param x1    The 0-based x location of point 1 on the triangle
	 *   \param y1    The 0-based y location of point 1 on the triangle
	 *   \param x2    The 0-based x location of point 2 on the triangle
	 *   \param y2    The 0-based y location of point 2 on the triangle
	 *   \param color The RGB565 color to use when drawing the pixel
	 */
	void    fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
	/**
	 *   \brief Draws a HW accelerated ellipse on the display
	 *   \param xCenter   The 0-based x location of the ellipse's center
	 *   \param yCenter   The 0-based y location of the ellipse's center
	 *   \param longAxis  The size in pixels of the ellipse's long axis
	 *   \param shortAxis The size in pixels of the ellipse's short axis
	 *   \param color     The RGB565 color to use when drawing the pixel
	 */
	void    drawEllipse(int16_t xCenter, int16_t yCenter, int16_t longAxis, int16_t shortAxis, uint16_t color);
	/**
	 *   \brief Draws a HW accelerated filled ellipse on the display
	 *   \param xCenter   The 0-based x location of the ellipse's center
	 *   \param yCenter   The 0-based y location of the ellipse's center
	 *   \param longAxis  The size in pixels of the ellipse's long axis
	 *   \param shortAxis The size in pixels of the ellipse's short axis
	 *   \param color     The RGB565 color to use when drawing the pixel
	 */
	void    fillEllipse(int16_t xCenter, int16_t yCenter, int16_t longAxis, int16_t shortAxis, uint16_t color);
	/**
	 *   \brief Draws a HW accelerated curve on the display
	 *   \param xCenter   The 0-based x location of the ellipse's center
	 *   \param yCenter   The 0-based y location of the ellipse's center
	 *   \param longAxis  The size in pixels of the ellipse's long axis
	 *   \param shortAxis The size in pixels of the ellipse's short axis
	 *   \param curvePart The corner to draw, where in clock-wise motion:
	 *                      0 = 180-270°
	 *                      1 = 270-0°
	 *                      2 = 0-90°
	 *                      3 = 90-180°
	 *   \param color[in]     The RGB565 color to use when drawing the pixel
	 */
	void    drawCurve(int16_t xCenter, int16_t yCenter, int16_t longAxis, int16_t shortAxis, uint8_t curvePart, uint16_t color);
	/**
	 *   \brief Draws a HW accelerated filled curve on the display
	 *   \param xCenter   The 0-based x location of the ellipse's center
	 *   \param yCenter   The 0-based y location of the ellipse's center
	 *   \param longAxis  The size in pixels of the ellipse's long axis
	 *   \param shortAxis The size in pixels of the ellipse's short axis
	 *   \param curvePart The corner to draw, where in clock-wise motion:
	 *                      0 = 180-270°
	 *                      1 = 270-0°
	 *                      2 = 0-90°
	 *                      3 = 90-180°
	 *   \param color[in]     The RGB565 color to use when drawing the pixel
	 */
	void    fillCurve(int16_t xCenter, int16_t yCenter, int16_t longAxis, int16_t shortAxis, uint8_t curvePart, uint16_t color);

	/* Play nice with Arduino's Print class */
	virtual size_t write(uint8_t b) {
		textWrite((const char *)&b, 1);
		return 1;
	}
	virtual size_t write(const uint8_t *buffer, size_t size) {
		textWrite((const char *)buffer, size);
		return size;
	}

	/**
	 * \brief Get the current height of the screen
	 * \return the height of the screen
	 */
	int16_t height(void);
	/**
	 * \brief Get the current width of the screen
	 * \return the width of the screen
	 */
	int16_t width(void);

	/* Low level access */
	void    writeReg(uint8_t reg, uint8_t val);
	uint8_t readReg(uint8_t reg);
	void    writeData(uint8_t d);
	uint8_t readData();
	void    writeCommand(uint8_t d);
	uint8_t readStatus();
	boolean waitPoll(uint8_t r, uint8_t f);

protected:
	/**
	 *  \brief Initilization of system clock
	 *  \param gclk The different configuration for system clock:
	 *               - 'RA8875_GPU_STD_CLK' the standard configuration (55MHz if RA8875_FIN=20)
	 *               - 'RA8875_GPU_DBL_CLK' the double configuration, recomended with SPI_FULL_SPPED (105MHz if RA8875_FIN=20)
	 */
	void PLLinit(enum RA8875GPUclock gclk);
	/**
	 *  \brief Initialization of the screen
	 *  \param gclk The different configuration for system clock:
	 *               - 'RA8875_GPU_STD_CLK' the standard configuration (55MHz if RA8875_FIN=20)
	 *               - 'RA8875_GPU_DBL_CLK' the double configuration, recomended with SPI_FULL_SPPED (105MHz if RA8875_FIN=20)
	 */
	void initialize(enum RA8875GPUclock gclk);

	/* GFX Helper Functions */
	void circleHelper(int16_t x0, int16_t y0, int16_t r, uint16_t color, bool filled);
	void rectHelper  (int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color, bool filled);
	void triangleHelper(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color, bool filled);
	void ellipseHelper(int16_t xCenter, int16_t yCenter, int16_t longAxis, int16_t shortAxis, uint16_t color, bool filled);
	void curveHelper(int16_t xCenter, int16_t yCenter, int16_t longAxis, int16_t shortAxis, uint8_t curvePart, uint16_t color, bool filled);

	static SdSpi m_spi;
	uint8_t _cs;   //!< Pin connexion for CS cable
	uint8_t _rst;  //!< Pin connexion for RST cable
	uint8_t _wait; //!< Pin connexion for WAIT cable
	uint8_t _int;  //!< Pin connexion for INT cable

	uint16_t _width;  //!< the width of the screen in pixel
	uint16_t _height; //!< the height of the screen in pixel
	float sysclock;   //!< the system clock in MHz
	float pxclock;    //!< the pixel clock in MHz
	enum RA8875sizes _size; //!< the size type of the screen
	uint8_t _textScale;
};

// Colors (RGB565)
#define	RA8875_BLACK            0x0000
#define	RA8875_BLUE             0x001F
#define	RA8875_RED              0xF800
#define	RA8875_GREEN            0x07E0
#define RA8875_CYAN             0x07FF
#define RA8875_MAGENTA          0xF81F
#define RA8875_YELLOW           0xFFE0  
#define RA8875_WHITE            0xFFFF

// Command/Data pins for SPI
#define RA8875_DATAWRITE        0x00
#define RA8875_DATAREAD         0x40
#define RA8875_CMDWRITE         0x80
#define RA8875_CMDREAD          0xC0

// Registers & bits
#define RA8875_PWRR             0x01
#define RA8875_PWRR_DISPON      0x80
#define RA8875_PWRR_DISPOFF     0x00
#define RA8875_PWRR_SLEEP       0x02
#define RA8875_PWRR_NORMAL      0x00
#define RA8875_PWRR_SOFTRESET   0x01

#define RA8875_MRWC             0x02

#define RA8875_GPIOX            0xC7

#define RA8875_PLLC1            0x88
#define RA8875_PLLC1_PLLDIV2    0x80
#define RA8875_PLLC1_PLLDIV1    0x00

#define RA8875_PLLC2            0x89
#define RA8875_PLLC2_DIV1       0x00
#define RA8875_PLLC2_DIV2       0x01
#define RA8875_PLLC2_DIV4       0x02
#define RA8875_PLLC2_DIV8       0x03
#define RA8875_PLLC2_DIV16      0x04
#define RA8875_PLLC2_DIV32      0x05
#define RA8875_PLLC2_DIV64      0x06
#define RA8875_PLLC2_DIV128     0x07

#define RA8875_SYSR             0x10
#define RA8875_SYSR_8BPP        0x00
#define RA8875_SYSR_16BPP       0x0C
#define RA8875_SYSR_MCU8        0x00
#define RA8875_SYSR_MCU16       0x03

#define RA8875_PCSR             0x04
#define RA8875_PCSR_PDATR       0x00
#define RA8875_PCSR_PDATL       0x80
#define RA8875_PCSR_CLK         0x00
#define RA8875_PCSR_2CLK        0x01
#define RA8875_PCSR_4CLK        0x02
#define RA8875_PCSR_8CLK        0x03

#define RA8875_HDWR             0x14

#define RA8875_HNDFTR           0x15
#define RA8875_HNDFTR_DE_HIGH   0x00
#define RA8875_HNDFTR_DE_LOW    0x80

#define RA8875_HNDR             0x16
#define RA8875_HSTR             0x17
#define RA8875_HPWR             0x18
#define RA8875_HPWR_LOW         0x00
#define RA8875_HPWR_HIGH        0x80

#define RA8875_VDHR0            0x19
#define RA8875_VDHR1            0x1A
#define RA8875_VNDR0            0x1B
#define RA8875_VNDR1            0x1C
#define RA8875_VSTR0            0x1D
#define RA8875_VSTR1            0x1E
#define RA8875_VPWR             0x1F
#define RA8875_VPWR_LOW         0x00
#define RA8875_VPWR_HIGH        0x80

#define RA8875_HSAW0            0x30
#define RA8875_HSAW1            0x31
#define RA8875_VSAW0            0x32
#define RA8875_VSAW1            0x33

#define RA8875_HEAW0            0x34
#define RA8875_HEAW1            0x35
#define RA8875_VEAW0            0x36
#define RA8875_VEAW1            0x37

#define RA8875_MCLR             0x8E
#define RA8875_MCLR_START       0x80
#define RA8875_MCLR_STOP        0x00
#define RA8875_MCLR_READSTATUS  0x80
#define RA8875_MCLR_FULL        0x00
#define RA8875_MCLR_ACTIVE      0x40

#define RA8875_DCR                    0x90
#define RA8875_DCR_LINESQUTRI_START   0x80
#define RA8875_DCR_LINESQUTRI_STOP    0x00
#define RA8875_DCR_LINESQUTRI_STATUS  0x80
#define RA8875_DCR_CIRCLE_START       0x40
#define RA8875_DCR_CIRCLE_STATUS      0x40
#define RA8875_DCR_CIRCLE_STOP        0x00
#define RA8875_DCR_FILL               0x20
#define RA8875_DCR_NOFILL             0x00
#define RA8875_DCR_DRAWLINE           0x00
#define RA8875_DCR_DRAWTRIANGLE       0x01
#define RA8875_DCR_DRAWSQUARE         0x10


#define RA8875_ELLIPSE                0xA0
#define RA8875_ELLIPSE_STATUS         0x80

#define RA8875_MWCR0            0x40
#define RA8875_MWCR0_GFXMODE    0x00
#define RA8875_MWCR0_TXTMODE    0x80

#define RA8875_CURH0            0x46
#define RA8875_CURH1            0x47
#define RA8875_CURV0            0x48
#define RA8875_CURV1            0x49

#define RA8875_P1CR             0x8A
#define RA8875_P1CR_ENABLE      0x80
#define RA8875_P1CR_DISABLE     0x00
#define RA8875_P1CR_CLKOUT      0x10
#define RA8875_P1CR_PWMOUT      0x00

#define RA8875_P1DCR            0x8B

#define RA8875_P2CR             0x8C
#define RA8875_P2CR_ENABLE      0x80
#define RA8875_P2CR_DISABLE     0x00
#define RA8875_P2CR_CLKOUT      0x10
#define RA8875_P2CR_PWMOUT      0x00

#define RA8875_P2DCR            0x8D

#define RA8875_PWM_CLK_DIV1     0x00
#define RA8875_PWM_CLK_DIV2     0x01
#define RA8875_PWM_CLK_DIV4     0x02
#define RA8875_PWM_CLK_DIV8     0x03
#define RA8875_PWM_CLK_DIV16    0x04
#define RA8875_PWM_CLK_DIV32    0x05
#define RA8875_PWM_CLK_DIV64    0x06
#define RA8875_PWM_CLK_DIV128   0x07
#define RA8875_PWM_CLK_DIV256   0x08
#define RA8875_PWM_CLK_DIV512   0x09
#define RA8875_PWM_CLK_DIV1024  0x0A
#define RA8875_PWM_CLK_DIV2048  0x0B
#define RA8875_PWM_CLK_DIV4096  0x0C
#define RA8875_PWM_CLK_DIV8192  0x0D
#define RA8875_PWM_CLK_DIV16384 0x0E
#define RA8875_PWM_CLK_DIV32768 0x0F

#define RA8875_TPCR0                  0x70
#define RA8875_TPCR0_ENABLE           0x80
#define RA8875_TPCR0_DISABLE          0x00
#define RA8875_TPCR0_WAIT_512CLK      0x00
#define RA8875_TPCR0_WAIT_1024CLK     0x10
#define RA8875_TPCR0_WAIT_2048CLK     0x20
#define RA8875_TPCR0_WAIT_4096CLK     0x30
#define RA8875_TPCR0_WAIT_8192CLK     0x40
#define RA8875_TPCR0_WAIT_16384CLK    0x50
#define RA8875_TPCR0_WAIT_32768CLK    0x60
#define RA8875_TPCR0_WAIT_65536CLK    0x70
#define RA8875_TPCR0_WAKEENABLE       0x08
#define RA8875_TPCR0_WAKEDISABLE      0x00
#define RA8875_TPCR0_ADCCLK_DIV1      0x00
#define RA8875_TPCR0_ADCCLK_DIV2      0x01
#define RA8875_TPCR0_ADCCLK_DIV4      0x02
#define RA8875_TPCR0_ADCCLK_DIV8      0x03
#define RA8875_TPCR0_ADCCLK_DIV16     0x04
#define RA8875_TPCR0_ADCCLK_DIV32     0x05
#define RA8875_TPCR0_ADCCLK_DIV64     0x06
#define RA8875_TPCR0_ADCCLK_DIV128    0x07

#define RA8875_TPCR1            0x71
#define RA8875_TPCR1_AUTO       0x00
#define RA8875_TPCR1_MANUAL     0x40
#define RA8875_TPCR1_VREFINT    0x00
#define RA8875_TPCR1_VREFEXT    0x20
#define RA8875_TPCR1_DEBOUNCE   0x04
#define RA8875_TPCR1_NODEBOUNCE 0x00
#define RA8875_TPCR1_IDLE       0x00
#define RA8875_TPCR1_WAIT       0x01
#define RA8875_TPCR1_LATCHX     0x02
#define RA8875_TPCR1_LATCHY     0x03

#define RA8875_TPXH             0x72
#define RA8875_TPYH             0x73
#define RA8875_TPXYL            0x74

#define RA8875_INTC1            0xF0
#define RA8875_INTC1_KEY        0x10
#define RA8875_INTC1_DMA        0x08
#define RA8875_INTC1_TP         0x04
#define RA8875_INTC1_BTE        0x02

#define RA8875_INTC2            0xF1
#define RA8875_INTC2_KEY        0x10
#define RA8875_INTC2_DMA        0x08
#define RA8875_INTC2_TP         0x04
#define RA8875_INTC2_BTE        0x02

#endif // _RA8875_DRIVER_H
