#pragma once
#include <stdint.h>

static const uint32_t screenWidth = 240;
static const uint32_t screenHeight = 240;

/* ---------------------------------------------------------------------------
 Pins
--------------------------------------------------------------------------- */

#define I2C_SDA 4
#define I2C_SCL 5
#define TP_INT 0
#define TP_RST 1

#define CS_PIN 10
#define BACKLIGHT_PIN 3


/* ---------------------------------------------------------------------------
 Colors
--------------------------------------------------------------------------- */

#ifdef WHITE_THEME
    #define BACKGROUND_COLOR    0xFBFDFA
    #define TEXT_COLOR          0x000000
    #define POPUP_COLOR         0xDDCCDD
#else
    #define BACKGROUND_COLOR    0x000000
    #define TEXT_COLOR          0xFBFDFA
    #define POPUP_COLOR         0x404040
#endif

#define ACCENT_COLOR        0x22DD11
