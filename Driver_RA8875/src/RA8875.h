/**
 * \author argawaen
 * \date 02/10/2020
 *
 */

#pragma once

/**
 * \brief list of the possible screen resolution
 */
enum struct ScreenResolution {
    RA8875_480x80,  ///<  480x80 Pixel Display
    RA8875_480x128, ///< 480x128 Pixel Display
    RA8875_480x272, ///< 480x272 Pixel Display
    RA8875_800x480  ///< 800x480 Pixel Display
};

class RA8875 {
public:
    /**
     * \brief constructor
     * \param cs    the pin number for Cable Select
     * \param reset the pin number for the Reset function
     * \param tint  the pin number for the Touch interrupt
     */
    RA8875(uint8_t cs, uint8_t reset, uint8_t tint):_cs{cs}, _reset{reset}, _touchInterrupt{tint}{}

    /**
     * \brief initialization of the driver
     * \param scr the resolution of the screen
     * \return true if setup success
     */
    bool setup(const ScreenResolution& scr);

    /**
     * \brief do a reset of the device
     */
    void softReset();

    /**
     * \brief turn on/off the display
     * \param on
     */
    void displayOn(bool on=true);

    /**
     * \brief
     * \param sleep
     */
    void sleep(bool sleep);

private:
    uint8_t _cs;
    uint8_t _reset;
    uint8_t _touchInterrupt;
};
