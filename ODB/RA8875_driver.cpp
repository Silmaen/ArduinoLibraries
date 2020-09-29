

#include <SPI.h>
#include "RA8875_driver.h"
#ifdef SOFTCHAR
#include "glcdfont.c"
#endif

SdSpi RA8875_driver::m_spi;

RA8875_driver::RA8875_driver(uint8_t CS, uint8_t RST,uint8_t INT, uint8_t WAIT){
	_cs = CS;
	_rst = RST;
	_int=INT;
	_wait=WAIT;
	_width = 0;
	_height = 0;
	_size = RA8875_480x272;
	pxclock=0;
	sysclock=0;
	_textScale=1;
}

int16_t RA8875_driver::width(void) {
	return _width;
}
 
int16_t RA8875_driver::height(void) {
	return _height;
}

boolean RA8875_driver::begin(enum RA8875sizes s,enum RA8875spirate spd,enum RA8875GPUclock gclk) {

	_size = s;

	/* define of screen resolution */
	if (_size == RA8875_480x272) {
		_width = 480;
		_height = 272;
	} 
	if (_size == RA8875_800x480) {
		_width = 800;
		_height = 480;
	}

	/* pins configuration */
	pinMode(_cs, OUTPUT);
	digitalWrite(_cs, HIGH);
	pinMode(_rst, OUTPUT); 
	digitalWrite(_rst, LOW);
	pinMode(_wait, INPUT);
	pinMode(_int, INPUT);

	/* Reset screen */
	digitalWrite(_rst, LOW);
	delay(100);
	digitalWrite(_rst, HIGH);
	delay(100);
  
	/* Start SPI interface */
	SPI.begin();
#ifdef __AVR__
	/* use low SPI speed for configuration to ensure compatibility */
	SPI.setClockDivider(SPI_CLOCK_DIV128);
	SPI.setDataMode(SPI_MODE0);
#endif
	/* Check the presence of the chip */
	if (readReg(0) != 0x75) {
		return false;
	}

	/* Initialize the screen and touchpad. */
	initialize(gclk);

#ifdef __AVR__
	/* switch to Operation SPI speed! */
	if (spd==RA8875_SPI_HALF_SPEED){
		SPI.setClockDivider(SPI_CLOCK_DIV4);
	}else if(spd==RA8875_SPI_FULL_SPEED){
		SPI.setClockDivider(SPI_CLOCK_DIV2);
	}
#endif
	return true;
}

void RA8875_driver::PLLinit(enum RA8875GPUclock gclk) {
	if (gclk==RA8875_GPU_STD_CLK) {
		writeReg(RA8875_PLLC1, RA8875_PLLC1_PLLDIV1 + 10);
		delay(1);
		writeReg(RA8875_PLLC2, RA8875_PLLC2_DIV4);
		delay(1);
	} else if (gclk==RA8875_GPU_DBL_CLK) {
		writeReg(RA8875_PLLC1, RA8875_PLLC1_PLLDIV1 + 20);
		delay(1);
		writeReg(RA8875_PLLC2, RA8875_PLLC2_DIV4);
		delay(1);
	} else if (gclk==RA8875_GPU_QUA_CLK) {
		writeReg(RA8875_PLLC1, RA8875_PLLC1_PLLDIV1 + 20);
		delay(1);
		writeReg(RA8875_PLLC2, RA8875_PLLC2_DIV2);
		delay(1);
	}
}

void RA8875_driver::initialize(enum RA8875GPUclock gclk) {

	/* Compute true clock frequency (in MHz) */
	/** \todo more speed needed */
	if (gclk==RA8875_GPU_STD_CLK) 
		sysclock=RA8875_FIN*(10+1)/4;
	else if (gclk==RA8875_GPU_DBL_CLK) 
		sysclock=RA8875_FIN*(20+1)/4;
	else if (gclk==RA8875_GPU_QUA_CLK)
		sysclock=RA8875_FIN*(20+1)/2;

	/* Initialize and configure interal clocks */
	PLLinit(gclk);
	/* Initialize System color to 16bits and SPI transfert to 8bits */
	/** \todo more options */
	writeReg(RA8875_SYSR, RA8875_SYSR_16BPP | RA8875_SYSR_MCU8);

	/* Timing values */
	uint8_t pixclk;
	uint8_t hsync_start;
	uint8_t hsync_pw;
	uint8_t hsync_finetune;
	uint8_t hsync_nondisp;
	uint8_t vsync_pw; 
	uint16_t vsync_nondisp;
	uint16_t vsync_start;

	/* Set the correct values for the display being used */  
	if (_size == RA8875_480x272) {
		if (gclk==RA8875_GPU_STD_CLK) {
			pixclk          = RA8875_PCSR_PDATL | RA8875_PCSR_4CLK;
			pxclock = sysclock/4;
		}
		if (gclk==RA8875_GPU_DBL_CLK) {
			pixclk          = RA8875_PCSR_PDATL | RA8875_PCSR_8CLK;
			pxclock = sysclock/8;
		}
		if (gclk==RA8875_GPU_QUA_CLK) {
			pixclk          = RA8875_PCSR_PDATL | RA8875_PCSR_8CLK;
			pxclock = sysclock/8;
		}
		hsync_nondisp   = 10;
		hsync_start     = 8;
		hsync_pw        = 48;
		hsync_finetune  = 0;
		vsync_nondisp   = 3;
		vsync_start     = 8;
		vsync_pw        = 10;
	} else if (_size == RA8875_800x480) {
		if (gclk==RA8875_GPU_STD_CLK) {
			pixclk          = RA8875_PCSR_PDATL | RA8875_PCSR_2CLK;
			pxclock = sysclock/2;
		}
		if (gclk==RA8875_GPU_DBL_CLK) {
			pixclk          = RA8875_PCSR_PDATL | RA8875_PCSR_4CLK;
			pxclock = sysclock/4;
		}
		if (gclk==RA8875_GPU_QUA_CLK) {
			pixclk          = RA8875_PCSR_PDATL | RA8875_PCSR_8CLK;
			pxclock = sysclock/8;
		}
		hsync_nondisp   = 26;
		hsync_start     = 32;
		hsync_pw        = 96;
		hsync_finetune  = 0;
		vsync_nondisp   = 32;
		vsync_start     = 23;
		vsync_pw        = 2;
	}

	/* apply the pixel clock */
	writeReg(RA8875_PCSR, pixclk);
	delay(1);

	/* Horizontal settings registers */
	writeReg(RA8875_HDWR, (_width / 8) - 1);                          // H width: (HDWR + 1) * 8 = 480
	writeReg(RA8875_HNDFTR, RA8875_HNDFTR_DE_HIGH + hsync_finetune);
	writeReg(RA8875_HNDR, (hsync_nondisp - hsync_finetune - 2)/8);    // H non-display: HNDR * 8 + HNDFTR + 2 = 10
	writeReg(RA8875_HSTR, hsync_start/8 - 1);                         // Hsync start: (HSTR + 1)*8 
	writeReg(RA8875_HPWR, RA8875_HPWR_LOW + (hsync_pw/8 - 1));        // HSync pulse width = (HPWR+1) * 8
  
	/* Vertical settings registers */
	writeReg(RA8875_VDHR0, (uint16_t)(_height - 1) & 0xFF);
	writeReg(RA8875_VDHR1, (uint16_t)(_height - 1) >> 8);
	writeReg(RA8875_VNDR0, vsync_nondisp-1);                          // V non-display period = VNDR + 1
	writeReg(RA8875_VNDR1, vsync_nondisp >> 8);
	writeReg(RA8875_VSTR0, vsync_start-1);                            // Vsync start position = VSTR + 1
	writeReg(RA8875_VSTR1, vsync_start >> 8);
	writeReg(RA8875_VPWR, RA8875_VPWR_LOW + vsync_pw - 1);            // Vsync pulse width = VPWR + 1
  
	/* Set active window X */
	writeReg(RA8875_HSAW0, 0);                                        // horizontal start point
	writeReg(RA8875_HSAW1, 0);
	writeReg(RA8875_HEAW0, (uint16_t)(_width - 1) & 0xFF);            // horizontal end point
	writeReg(RA8875_HEAW1, (uint16_t)(_width - 1) >> 8);
  
	/* Set active window Y */
	writeReg(RA8875_VSAW0, 0);                                        // vertical start point
	writeReg(RA8875_VSAW1, 0);  
	writeReg(RA8875_VEAW0, (uint16_t)(_height - 1) & 0xFF);           // horizontal end point
	writeReg(RA8875_VEAW1, (uint16_t)(_height - 1) >> 8);
  
	/* ToDo: Setup touch panel? */
  
	/* Clear the entire window */
	writeReg(RA8875_MCLR, RA8875_MCLR_START | RA8875_MCLR_FULL);
	delay(100); 
}

void RA8875_driver::softReset(void) {
	writeCommand(RA8875_PWRR);
	writeData(RA8875_PWRR_SOFTRESET);
	writeData(RA8875_PWRR_NORMAL);
	delay(1);
}

void RA8875_driver::displayOn(boolean on) {
	if (on) 
		writeReg(RA8875_PWRR, RA8875_PWRR_NORMAL | RA8875_PWRR_DISPON);
	else
		writeReg(RA8875_PWRR, RA8875_PWRR_NORMAL | RA8875_PWRR_DISPOFF);
}

void RA8875_driver::sleep(boolean sleep) {
	if (sleep) 
		writeReg(RA8875_PWRR, RA8875_PWRR_DISPOFF | RA8875_PWRR_SLEEP);
	else
		writeReg(RA8875_PWRR, RA8875_PWRR_DISPOFF);
}

void RA8875_driver::GPIOX(boolean on) {
	if (on)
		writeReg(RA8875_GPIOX, 1);
	else 
		writeReg(RA8875_GPIOX, 0);
}

void RA8875_driver::PWM1config(boolean on, uint8_t clock) {
	if (on) {
		writeReg(RA8875_P1CR, RA8875_P1CR_ENABLE | (clock & 0xF));
	} else {
		writeReg(RA8875_P1CR, RA8875_P1CR_DISABLE | (clock & 0xF));
	}
}

void RA8875_driver::PWM2config(boolean on, uint8_t clock) {
	if (on) {
		writeReg(RA8875_P2CR, RA8875_P2CR_ENABLE | (clock & 0xF));
	} else {
		writeReg(RA8875_P2CR, RA8875_P2CR_DISABLE | (clock & 0xF));
	}
}

void RA8875_driver::PWM1out(uint8_t p) {
	writeReg(RA8875_P1DCR, p);
}

void RA8875_driver::PWM2out(uint8_t p) {
	writeReg(RA8875_P2DCR, p);
}

void RA8875_driver::touchEnable(boolean on,enum RA8875touchspd spd) {
	if (on) {
		/* Enable Touch Panel (Reg 0x70) */
		if (spd==RA8875_GPU_STD_CLK){
			writeReg(RA8875_TPCR0, RA8875_TPCR0_ENABLE        | 
							RA8875_TPCR0_WAIT_4096CLK  |
							RA8875_TPCR0_WAKEDISABLE   | 
							RA8875_TPCR0_ADCCLK_DIV4); // 10mhz max!
		}else if(spd==RA8875_GPU_DBL_CLK){
			writeReg(RA8875_TPCR0, RA8875_TPCR0_ENABLE        | 
							RA8875_TPCR0_WAIT_2048CLK  |
							RA8875_TPCR0_WAKEDISABLE   | 
							RA8875_TPCR0_ADCCLK_DIV2); // 10mhz max!
		}
		/* Set Auto Mode      (Reg 0x71) */
		writeReg(RA8875_TPCR1, RA8875_TPCR1_AUTO    | 
                           // RA8875_TPCR1_VREFEXT | 
                           RA8875_TPCR1_DEBOUNCE);
		/* Enable TP INT */
		writeReg(RA8875_INTC1, readReg(RA8875_INTC1) | RA8875_INTC1_TP);
	} else {
		/* Disable TP INT */
		writeReg(RA8875_INTC1, readReg(RA8875_INTC1) & ~RA8875_INTC1_TP);
		/* Disable Touch Panel (Reg 0x70) */
		writeReg(RA8875_TPCR0, RA8875_TPCR0_DISABLE);
	}
}

boolean RA8875_driver::touched(void) {
	if (readReg(RA8875_INTC2) & RA8875_INTC2_TP) return true;
	return false;
}

boolean RA8875_driver::touchRead(uint16_t *x, uint16_t *y) {
  uint16_t tx, ty;
  uint8_t temp;
  tx = readReg(RA8875_TPXH);
  ty = readReg(RA8875_TPYH);
  temp = readReg(RA8875_TPXYL);
  tx <<= 2;
  ty <<= 2;
  tx |= temp & 0x03;        // get the bottom x bits
  ty |= (temp >> 2) & 0x03; // get the bottom y bits
  *x = tx;
  *y = ty;
  /* Clear TP INT Status */
  writeReg(RA8875_INTC2, RA8875_INTC2_TP);
  return true;
}

void RA8875_driver::textMode(void) {
	/* Set text mode */
	writeCommand(RA8875_MWCR0);
	uint8_t temp = readData();
	temp |= RA8875_MWCR0_TXTMODE; // Set bit 7
	writeData(temp);
	/* Select the internal (ROM) font */
	writeCommand(0x21);
	temp = readData();
	temp &= ~((1<<7) | (1<<5)); // Clear bits 7 and 5
	writeData(temp);
}

void RA8875_driver::textSetCursor(uint16_t x, uint16_t y) {
	/* Set cursor location */
	writeCommand(0x2A);
	writeData(x & 0xFF);
	writeCommand(0x2B);
	writeData(x >> 8);
	writeCommand(0x2C);
	writeData(y & 0xFF);
	writeCommand(0x2D);
	writeData(y >> 8);
}

void RA8875_driver::textColor(uint16_t foreColor, uint16_t bgColor) {
	/* Set Fore Color */
	writeCommand(0x63);
	writeData((foreColor & 0xf800) >> 11);
	writeCommand(0x64);
	writeData((foreColor & 0x07e0) >> 5);
	writeCommand(0x65);
	writeData((foreColor & 0x001f));
	/* Set Background Color */
	writeCommand(0x60);
	writeData((bgColor & 0xf800) >> 11);
	writeCommand(0x61);
	writeData((bgColor & 0x07e0) >> 5);
	writeCommand(0x62);
	writeData((bgColor & 0x001f));
	/* Clear transparency flag */
	writeCommand(0x22);
	uint8_t temp = readData();
	temp &= ~(1<<6); // Clear bit 6
	writeData(temp);
}

void RA8875_driver::textTransparent(uint16_t foreColor) {
	/* Set Fore Color */
	writeCommand(0x63);
	writeData((foreColor & 0xf800) >> 11);
	writeCommand(0x64);
	writeData((foreColor & 0x07e0) >> 5);
	writeCommand(0x65);
	writeData((foreColor & 0x001f));
	/* Set transparency flag */
	writeCommand(0x22);
	uint8_t temp = readData();
	temp |= (1<<6); // Set bit 6
	writeData(temp);  
}

void RA8875_driver::textEnlarge(uint8_t scale) {
	if (scale > 3) scale = 3;
	/* Set font size flags */
	writeCommand(0x22);
	uint8_t temp = readData();
	temp &= ~(0xF); // Clears bits 0..3
	temp |= scale << 2;
	temp |= scale;
	writeData(temp);  
	_textScale = scale;
}

void RA8875_driver::textWrite(const char* buffer, uint16_t len) {
	if (len == 0) len = strlen(buffer);
	writeCommand(RA8875_MRWC);
	for (uint16_t i=0;i<len;i++) {
		writeData(buffer[i]);
#if defined(__AVR__)
		if (_textScale > 1) delay(1);
#elif defined(__arm__)
		// This delay is needed with textEnlarge(1) because
		// Teensy 3.X is much faster than Arduino Uno
		if (_textScale > 0) delay(1);
#endif
	}
}

void RA8875_driver::drawString(char* str,uint8_t len,uint16_t X,uint16_t Y,uint8_t size,uint16_t color,uint16_t bg){
	textMode();
	textEnlarge(size);
	textSetCursor(X,Y);
	textColor(color,bg);
	textWrite(str,len);
	graphicsMode();
}

void RA8875_driver::drawTString(char* str,uint8_t len,uint16_t X,uint16_t Y,uint8_t size,uint16_t color){
	textMode();
	textEnlarge(size);
	textSetCursor(X,Y);
	textTransparent(color);
	textWrite(str,len);
	graphicsMode();
}

void RA8875_driver::graphicsMode(void) {
	writeCommand(RA8875_MWCR0);
	uint8_t temp = readData();
	temp &= ~RA8875_MWCR0_TXTMODE; // bit #7
	writeData(temp);
}

void RA8875_driver::setXY(uint16_t x, uint16_t y) {
/*	digitalWrite(_cs, LOW);
	writeReg(RA8875_CURH0, x,false);
	writeReg(RA8875_CURH1, x >> 8,false);
	writeReg(RA8875_CURV0, y,false);
	writeReg(RA8875_CURV1, y >> 8,false);
	digitalWrite(_cs, HIGH);
	SPI.transfer(0xFF); */
	writeReg(RA8875_CURH0, x);
	writeReg(RA8875_CURH1, x >> 8);
	writeReg(RA8875_CURV0, y);
	writeReg(RA8875_CURV1, y >> 8);
}

void RA8875_driver::pushPixels(uint32_t num, uint16_t p) {
	digitalWrite(_cs, LOW);
	SPI.transfer(RA8875_DATAWRITE);
	while (num--) {
		SPI.transfer(p >> 8);
		SPI.transfer(p);
	}
	digitalWrite(_cs, HIGH);
}

void RA8875_driver::drawPixel(int16_t x, int16_t y, uint16_t color) {
	writeReg(RA8875_CURH0, x);
	writeReg(RA8875_CURH1, x >> 8);
	writeReg(RA8875_CURV0, y);
	writeReg(RA8875_CURV1, y >> 8);  
	writeCommand(RA8875_MRWC);
	digitalWrite(_cs, LOW);
	SPI.transfer(RA8875_DATAWRITE);
	SPI.transfer(color >> 8);
	SPI.transfer(color);
	digitalWrite(_cs, HIGH);
}

#ifdef SOFTCHAR
void RA8875_driver::drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size){
	if((x >= _width)            || // Clip right
			(y >= _height)           || // Clip bottom
			((x + 6 * size - 1) < 0) || // Clip left
			((y + 8 * size - 1) < 0))   // Clip top
		return;

	for (int8_t i=0; i<6; i++ ) {
		uint8_t line;
		if (i == 5)
			line = 0x0;
		else
			line = pgm_read_byte(font+(c*5)+i);
		for (int8_t j = 0; j<8; j++) {
			if (line & 0x1) {
				if (size == 1) // default size
					drawPixel(x+i, y+j, color);
				else {  // big size
					fillRect(x+(i*size), y+(j*size), size, size, color);
				}
			} else if (bg != color) {
				if (size == 1) // default size
					drawPixel(x+i, y+j, bg);
				else {  // big size
					fillRect(x+i*size, y+j*size, size, size, bg);
				}
			}
			line >>= 1;
		}
	}
}
#endif

void RA8875_driver::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) {
	/* Set X */
	writeCommand(0x91);
	writeData(x0);
	writeCommand(0x92);
	writeData(x0 >> 8);
	/* Set Y */
	writeCommand(0x93);
	writeData(y0); 
	writeCommand(0x94);
	writeData(y0 >> 8);
	/* Set X1 */
	writeCommand(0x95);
	writeData(x1);
	writeCommand(0x96);
	writeData((x1) >> 8);
	/* Set Y1 */
	writeCommand(0x97);
	writeData(y1); 
	writeCommand(0x98);
	writeData((y1) >> 8);
	/* Set Color */
	writeCommand(0x63);
	writeData((color & 0xf800) >> 11);
	writeCommand(0x64);
	writeData((color & 0x07e0) >> 5);
	writeCommand(0x65);
	writeData((color & 0x001f));
	/* Draw! */
	writeCommand(RA8875_DCR);
	writeData(0x80);
	/* Wait for the command to finish */
	waitPoll(RA8875_DCR, RA8875_DCR_LINESQUTRI_STATUS);
}

void RA8875_driver::drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {
	drawLine(x, y, x, y+h, color);
}

void RA8875_driver::drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {
	drawLine(x, y, x+w, y, color);
}

void RA8875_driver::fillScreen(uint16_t color) {
	rectHelper(0, 0, _width-1, _height-1, color, true);
}

void RA8875_driver::drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
	rectHelper(x, y, x+w, y+h, color, false);
}

void RA8875_driver::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
	rectHelper(x, y, x+w, y+h, color, true);
}

void RA8875_driver::drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color) {
	circleHelper(x0, y0, r, color, false);
}

void RA8875_driver::fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color) {
	circleHelper(x0, y0, r, color, true);
}

void RA8875_driver::drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color) {
	triangleHelper(x0, y0, x1, y1, x2, y2, color, false);
}

void RA8875_driver::fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color) {
	triangleHelper(x0, y0, x1, y1, x2, y2, color, true);
}

void RA8875_driver::drawEllipse(int16_t xCenter, int16_t yCenter, int16_t longAxis, int16_t shortAxis, uint16_t color) {
	ellipseHelper(xCenter, yCenter, longAxis, shortAxis, color, false);
}

void RA8875_driver::fillEllipse(int16_t xCenter, int16_t yCenter, int16_t longAxis, int16_t shortAxis, uint16_t color) {
	ellipseHelper(xCenter, yCenter, longAxis, shortAxis, color, true);
}

void RA8875_driver::drawCurve(int16_t xCenter, int16_t yCenter, int16_t longAxis, int16_t shortAxis, uint8_t curvePart, uint16_t color) {
	curveHelper(xCenter, yCenter, longAxis, shortAxis, curvePart, color, false);
}

void RA8875_driver::fillCurve(int16_t xCenter, int16_t yCenter, int16_t longAxis, int16_t shortAxis, uint8_t curvePart, uint16_t color) {
	curveHelper(xCenter, yCenter, longAxis, shortAxis, curvePart, color, true);
}

void RA8875_driver::circleHelper(int16_t x0, int16_t y0, int16_t r, uint16_t color, bool filled) {
	/* Set X */
	writeCommand(0x99);
	writeData(x0);
	writeCommand(0x9a);
	writeData(x0 >> 8);

	/* Set Y */
	writeCommand(0x9b);
	writeData(y0); 
	writeCommand(0x9c);	   
	writeData(y0 >> 8);

	/* Set Radius */
	writeCommand(0x9d);
	writeData(r);  

	/* Set Color */
	writeCommand(0x63);
	writeData((color & 0xf800) >> 11);
	writeCommand(0x64);
	writeData((color & 0x07e0) >> 5);
	writeCommand(0x65);
	writeData((color & 0x001f));

	/* Draw! */
	writeCommand(RA8875_DCR);
	if (filled) {
		writeData(RA8875_DCR_CIRCLE_START | RA8875_DCR_FILL);
	} else {
		writeData(RA8875_DCR_CIRCLE_START | RA8875_DCR_NOFILL);
	}

	/* Wait for the command to finish */
	waitPoll(RA8875_DCR, RA8875_DCR_CIRCLE_STATUS);
}

void RA8875_driver::rectHelper(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color, bool filled) {
	/* Set X */
	writeCommand(0x91);
	writeData(x);
	writeCommand(0x92);
	writeData(x >> 8);

	/* Set Y */
	writeCommand(0x93);
	writeData(y); 
	writeCommand(0x94);	   
	writeData(y >> 8);

	/* Set X1 */
	writeCommand(0x95);
	writeData(w);
	writeCommand(0x96);
	writeData((w) >> 8);

	/* Set Y1 */
	writeCommand(0x97);
	writeData(h); 
	writeCommand(0x98);
	writeData((h) >> 8);

	/* Set Color */
	writeCommand(0x63);
	writeData((color & 0xf800) >> 11);
	writeCommand(0x64);
	writeData((color & 0x07e0) >> 5);
	writeCommand(0x65);
	writeData((color & 0x001f));

	/* Draw! */
	writeCommand(RA8875_DCR);
	if (filled) {
		writeData(0xB0);
	} else {
		writeData(0x90);
	}

	/* Wait for the command to finish */
	waitPoll(RA8875_DCR, RA8875_DCR_LINESQUTRI_STATUS);
}

void RA8875_driver::triangleHelper(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color, bool filled) {
	/* Set Point 0 */
	writeCommand(0x91);
	writeData(x0);
	writeCommand(0x92);
	writeData(x0 >> 8);
	writeCommand(0x93);
	writeData(y0); 
	writeCommand(0x94);
	writeData(y0 >> 8);

	/* Set Point 1 */
	writeCommand(0x95);
	writeData(x1);
	writeCommand(0x96);
	writeData(x1 >> 8);
	writeCommand(0x97);
	writeData(y1); 
	writeCommand(0x98);
	writeData(y1 >> 8);

	/* Set Point 2 */
	writeCommand(0xA9);
	writeData(x2);
	writeCommand(0xAA);
	writeData(x2 >> 8);
	writeCommand(0xAB);
	writeData(y2); 
	writeCommand(0xAC);
	writeData(y2 >> 8);

	/* Set Color */
	writeCommand(0x63);
	writeData((color & 0xf800) >> 11);
	writeCommand(0x64);
	writeData((color & 0x07e0) >> 5);
	writeCommand(0x65);
	writeData((color & 0x001f));

	/* Draw! */
	writeCommand(RA8875_DCR);
	if (filled) {
		writeData(0xA1);
	} else {
		writeData(0x81);
	}

	/* Wait for the command to finish */
	waitPoll(RA8875_DCR, RA8875_DCR_LINESQUTRI_STATUS);
}

void RA8875_driver::ellipseHelper(int16_t xCenter, int16_t yCenter, int16_t longAxis, int16_t shortAxis, uint16_t color, bool filled) {
	digitalWrite(_cs, LOW);
	/* Set Center Point */
	writeCommand(0xA5);
	writeData(xCenter);
	writeCommand(0xA6);
	writeData(xCenter >> 8);
	writeCommand(0xA7);
	writeData(yCenter);
	writeCommand(0xA8);
	writeData(yCenter >> 8);

	/* Set Long and Short Axis */
	writeCommand(0xA1);
	writeData(longAxis);
	writeCommand(0xA2);
	writeData(longAxis >> 8);
	writeCommand(0xA3);
	writeData(shortAxis);
	writeCommand(0xA4);
	writeData(shortAxis >> 8);

	/* Set Color */
	writeCommand(0x63);
	writeData((color & 0xf800) >> 11);
	writeCommand(0x64);
	writeData((color & 0x07e0) >> 5);
	writeCommand(0x65);
	writeData((color & 0x001f));

	/* Draw! */
	writeCommand(0xA0);
	if (filled) {
		writeData(0xC0);
	} else {
		writeData(0x80);
	}
  
	/* Wait for the command to finish */
	waitPoll(RA8875_ELLIPSE, RA8875_ELLIPSE_STATUS);
	digitalWrite(_cs, HIGH);
	SPI.transfer(0xFF);
}

void RA8875_driver::curveHelper(int16_t xCenter, int16_t yCenter, int16_t longAxis, int16_t shortAxis, uint8_t curvePart, uint16_t color, bool filled) {
	/* Set Center Point */
	writeCommand(0xA5);
	writeData(xCenter);
	writeCommand(0xA6);
	writeData(xCenter >> 8);
	writeCommand(0xA7);
	writeData(yCenter); 
	writeCommand(0xA8);
	writeData(yCenter >> 8);

	/* Set Long and Short Axis */
	writeCommand(0xA1);
	writeData(longAxis);
	writeCommand(0xA2);
	writeData(longAxis >> 8);
	writeCommand(0xA3);
	writeData(shortAxis); 
	writeCommand(0xA4);
	writeData(shortAxis >> 8);
  
	/* Set Color */
	writeCommand(0x63);
	writeData((color & 0xf800) >> 11);
	writeCommand(0x64);
	writeData((color & 0x07e0) >> 5);
	writeCommand(0x65);
	writeData((color & 0x001f));

	/* Draw! */
	writeCommand(0xA0);
	if (filled) {
		writeData(0xD0 | (curvePart & 0x03));
	} else {
		writeData(0x90 | (curvePart & 0x03));
	}
  
	/* Wait for the command to finish */
	waitPoll(RA8875_ELLIPSE, RA8875_ELLIPSE_STATUS);
}

void  RA8875_driver::writeReg(uint8_t reg, uint8_t val) {
	writeCommand(reg);
	writeData(val);
}

uint8_t  RA8875_driver::readReg(uint8_t reg) {
	writeCommand(reg);
	return readData();
}

void  RA8875_driver::writeData(uint8_t d) {
	digitalWrite(_cs, LOW);
	SPI.transfer(RA8875_DATAWRITE);
	SPI.transfer(d);
	digitalWrite(_cs, HIGH);
}

uint8_t  RA8875_driver::readData() {
	digitalWrite(_cs, LOW);
	SPI.transfer(RA8875_DATAREAD);
	uint8_t x = SPI.transfer(0x0);
	digitalWrite(_cs, HIGH);
	return x;
}

void  RA8875_driver::writeCommand(uint8_t d) {
	digitalWrite(_cs, LOW);
	SPI.transfer(RA8875_CMDWRITE);
	SPI.transfer(d);
	digitalWrite(_cs, HIGH);
}

uint8_t  RA8875_driver::readStatus() {
	digitalWrite(_cs, LOW);
	SPI.transfer(RA8875_CMDREAD);
	uint8_t x = SPI.transfer(0x0);
	digitalWrite(_cs, HIGH);
	return x;
}

boolean RA8875_driver::waitPoll(uint8_t regname, uint8_t waitflag) {
  /* Wait for the command to finish */
  while (1) {
    uint8_t temp = readReg(regname);
    if (!(temp & waitflag))
      return true;
  }
  return false; // MEMEFIX: yeah i know, unreached! - add timeout?
}

