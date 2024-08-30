/**
 * @file mobidot.hpp
 * Main header file for MobiDOT display library
 * 
 * Arduino library for communicating with the Mobitec MobiDOT flipdot displays based on the RS485 protocol. 
 * 
 * Copyright (c) 2021 Arne van Iterson
 */

#include <Arduino.h>
#include <SoftwareSerial.h>
#include "gfxfont/gfxfont.h"

/* Library constants */
#define DEBUG true

/* RS485 constants */
#define RS485_TX_PIN_VALUE HIGH
#define RS485_RX_PIN_VALUE LOW
#define RS485_BAUDRATE 4800
#define RS485_BUFFER_SIZE 2048

/* Protocol constants */
#define MOBIDOT_BYTE_START 0xff
#define MOBIDOT_BYTE_STOP 0xff
#define MOBIDOT_MODE_ASCII 0xa2

/* Front sign constants */
#define MOBIDOT_ADDRESS_FRONT 0x06
#define MOBIDOT_WIDTH_FRONT 112
#define MOBIDOT_HEIGHT_FRONT 16

/* Rear sign constants */
#define MOBIDOT_ADDRESS_REAR 0x08
#define MOBIDOT_WIDTH_REAR 21
#define MOBIDOT_HEIGHT_REAR 14

/* Side sign constants */
#define MOBIDOT_ADDRESS_SIDE 0x07
#define MOBIDOT_WIDTH_SIDE 84
#define MOBIDOT_HEIGHT_SIDE 7

/**
 * @class MobiDOT class
 */
class MobiDOT
{
public:
    /**
     * @enum Display
     * Contains display types
     */
    enum class Display
    {
        FRONT,
        REAR,
        SIDE
    };

    /**
     * @enum Font
     * Contains all fonts available in ASCII mode
     */
    enum class Font
    {
        TEXT_5PX = 0x72,
        TEXT_6PX = 0x66,
        TEXT_7PX = 0x65,
        TEXT_7PX_BOLD = 0x64,
        TEXT_9PX = 0x75,
        TEXT_9PX_BOLD = 0x70,
        TEXT_9PX_BOLDER = 0x62,
        TEXT_13PX = 0x73,
        TEXT_13PX_BOLD = 0x69,
        TEXT_13PX_BOLDER = 0x61,
        TEXT_13PX_BOLDEST = 0x79,
        NUMBERS_14PX = 0x00,
        TEXT_15PX = 0x71,
        TEXT_16PX = 0x68,
        TEXT_16PX_BOLD = 0x78,
        TEXT_16PX_BOLDER = 0x74,
        SYMBOLS = 0x67,
        BITWISE = 0x77
    };

    /**
     * MobiDOT class constructor
     * Sets up everything to start sending data to the integrated controller of a MobiDOT display
     * @param rx RX(RO) pin of RS485 module / ic
     * @param tx TX(DI) pin of RS485 module / ic
     * @param ctrl DE and RE pins of the RS485 module / ic
     * @param light Pin connected to the control pin of a relais to controls the led's of the display
     */
    MobiDOT(const uint8_t rx, const uint8_t tx, const uint8_t ctrl, const uint8_t light = -1);

    /**
     * MobiDOT class deconstructor
     * End communication with display and closes RS485 serial port
     */
    ~MobiDOT();

    /**
     * selectDisplay function
     * Selects a display to print to, this will be used until this function is called again
     * @param type MobiDOT::Display type
     */
    void selectDisplay(MobiDOT::Display type);

    /**
     * setLight function
     * Sets the light pin set when calling the constructor high or low depending on parameter state.
     * This pin can control a relay that will turn on the frontlight.
     * @param state Boolean; true is on, false is off.
     */
    void setLight(bool state);

    /**
     * toggleLight function
     * Toggles the state of the light pin set when calling the contstructor. 
     * This pin can control a relay that will turn on the frontlight.
     */
    void toggleLight();

    /**
     * print function
     * Prints a string to the display using the currently selected font, 
     * keep in mind that some fonts require a certain offset in order to start on the first pixel of the display.
     * This function can only use fonts built into the MobiDOT units
     * @param c[] String to print
     * @param font Font to use (optional, will use default font when not specified)
     * @param offsetX Horizontal offset (optional, will print at 0, 0 when not specified)
     * @param offsetY Vertical offset (see offsetX)
     * @param invert Invert text, only works with GFXfonts
     */
    void print(const char c[]);
    void print(const char c[], int offsetX, int offsetY);
    void print(const char c[], MobiDOT::Font font);
    void print(const char c[], MobiDOT::Font font, int offsetX, int offsetY);

    void print(const char c[], const GFXfont *font, bool invert = false);
    void print(const char c[], const GFXfont *font, int offsetX, int offsetY, bool invert = false);

    /**
     * update function
     * Send the current display buffer to the display
     * @returns True or false based on wether the transfer was successfull
     */
    bool update();

    /**
     * clear function
     * Clears the currently selected display
     * @param value Determines wether all dots will be turned 'on' (yellow side) or 'off' (black side)
     */
    void clear(bool value = false);

    /**
     * drawBitmap function
     * Draws a bitmap image encoded using image2cpp on the specified coordinates
     * @param data Bitmap data
     * @param width Width of the to be printed image
     * @param height Height of the to be printed image
     * @param x Horizontal offset (optional, will display at 0, 0 if not specified)
     * @param y Vertical offset (see x)
     * @param invert Inverts image data if true (optional, false if not specified)
     */
    void drawBitmap(const unsigned char data[], uint width, uint height, bool invert = false);
    void drawBitmap(const unsigned char data[], uint width, uint height, int x, int y, bool invert = false);

    /**
     * drawRect function
     * Draws a rectangle with the supplied width and height at the given coordinates
     * If fill is true it will draw a rectange with all pixels on, otherwise just the outline
     * @param width Width of the rectangle
     * @param height Height of the rectangle
     * @param x Horizontal offset (optional, will display at 0, 0 if not specified)
     * @param y Vertical offset (see x)
     * @param fill Fill or not
     */
    void drawRect(uint width, uint height, bool fill = true);
    void drawRect(uint width, uint height, int x, int y, bool fill = true);

    /**
     * drawLine function
     * Draws a line between two points with given coordinates x1, y1 and x2, y2
     * @param x1 
     * @param y1 
     * @param x2 
     * @param y2 
     */
    void drawLine(int x1, int y1, int x2, int y2);

private:
    // Serial communication parameters
    SoftwareSerial RS485;
    uint8_t PIN_CTRL;

    // Current display storage
    MobiDOT::Display DISPLAY_DEFAULT;

    // Command buffer, MobiDOT displays allow for commands to be stringed together.
    // In fact, this is required if you want to draw more than one thing on the display, like a bitmap and text
    // The buffer will be sent and cleared when update() is called.
    // If the size is too small for your project you can change the buffer size using the compiler macro
    char BUFFER_DATA[RS485_BUFFER_SIZE] = {0};
    uint BUFFER_SIZE = 0;

    // State of the relay that controls the frontlight
    int8_t PIN_LIGHT = -1;
    bool STATE_LIGHT = false;

    /**
     * @struct DisplayAttribute
     * Contains information about the displays defined in Display
     */
    struct DisplayAttribute
    {
        char address;
        MobiDOT::Font defaultFont;
        uint8_t width;
        uint8_t height;
    };

    /**
     * display array
     * Contains the compiler macros at the top of mobidot.hpp for easy access using Display
     */
    const struct DisplayAttribute display[3] = {
        {
            MOBIDOT_ADDRESS_FRONT,
            MobiDOT::Font::TEXT_16PX_BOLD,
            MOBIDOT_WIDTH_FRONT,
            MOBIDOT_HEIGHT_FRONT,
        },
        {
            MOBIDOT_ADDRESS_REAR,
            MobiDOT::Font::TEXT_13PX_BOLD,
            MOBIDOT_WIDTH_REAR,
            MOBIDOT_HEIGHT_REAR,
        },
        {
            MOBIDOT_ADDRESS_SIDE,
            MobiDOT::Font::TEXT_7PX_BOLD,
            MOBIDOT_WIDTH_SIDE,
            MOBIDOT_HEIGHT_SIDE,
        },
    };

    /**
     * addHeader function
     * Adds MobiDOT header to output data
     * @param type Display type, see Display
     * @param data Input data array
     * @param size Size of input data array
     */
    void addHeader(MobiDOT::Display type, char data[], uint &size);

    /**
     * addFooter function
     * Adds MobiDOT footer to input data
     * @param data Input data array
     * @param size Pointer to the size of input data array
     */
    void addFooter(char data[], uint &size);

    /**
     * sendBuffer function
     * Sends input data to display over the RS485 bus
     * @param data Input data array
     * @param size Size of input data array
     * @returns True if transfer successfull, false if not
     */
    bool sendBuffer(char data[], uint size);
};