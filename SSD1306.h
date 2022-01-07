/**
 * @file SSD1306.h
 * @author Egorov Artem (artyomeo@gmail.com)
 * @brief 
 * @version 2.1
 * @date 2022-01-07
 * 
 * @copyright Copyright (c) 2022
 * This library was based on the developments of the following authors:
 * original author:  Tilen Majerle<tilen@majerle.eu>
 * modification for STM32f10x: Alexander Lutsai<s.lyra@ya.ru>
 */

#include <stdio.h>
#include <string.h>

#include "fonts.h"

#include "SSD1306_def.h"

#ifndef ABS
	/* Absolute value of number (x) */
    #define ABS(x)   ((x) > 0 ? (x) : -(x))
#endif

#define SSD1306_SWAP(x, y) (((x) ^= (y)), ((y) ^= (x)), ((x) ^= (y)))

#define SSD1306_OPPOSITE_COLOR(x) (x != SSD1306_COLOR_BLACK ? SSD1306_COLOR_BLACK : SSD1306_COLOR_WHITE)

/* bufer of data screen */
uint8_t SSD1306_Buffer [SSD1306_buf_size];

/**
 * @brief  SSD1306 Inversion enumeration
 */
typedef enum {
	SSD1306_INVERSION_OFF = 0x00,		/* Inversion off */
	SSD1306_INVERSION_ON = 0xFF,		/* Inversion on  */

	SSD1306_INVERSION_ERROR = -0x01
} SSD1306_INVERSION_t;

/**
 * @brief  SSD1306 color enumeration
 */
typedef enum {
	SSD1306_COLOR_BLACK = 0x00,		/* Black color, no pixel	 */
	SSD1306_COLOR_WHITE = 0xFF,		/* Light color, pixel is set */

	SSD1306_COLOR_ERROR = -0x01
} SSD1306_COLOR_t;

/**
 * @brief  SSD1306 align enumeration relatively point XY (P) or Screen (S)
 */
typedef enum {
	SSD1306_ALIGN_LEFT     = 0x00,	/* the text will be to the right of the specified point  */
	SSD1306_ALIGN_RIGHT    = 0x0F,	/* the text will be to the left of the specified point	 */

	SSD1306_ALIGN_CENTER_P = 0x07,	/* the text will be in the middle of the specified point */

	SSD1306_ALIGN_CENTER_S = 0xF7,	/* the text will be in the middle of the OLED screen	 */

	SSD1306_ALIGN_ERROR    = -0x01
} SSD1306_ALIGN_t;

/**
 * @brief  SSD1306 change display the image on the screen
 */
typedef enum {
	SSD1306_IMAGE_NORM     = 0x00,	/* the image is drawn normally and does not change  */
	SSD1306_IMAGE_MIRROR_V = 0x07,	/* the image is mirrored along a vertical line		*/

	SSD1306_IMAGE_ERROR    = -0x01
} SSD1306_IMAGE_t;

void SSD1306_INIT (void);

void SSD1306_UpdateScreen (void);

uint8_t SSD1306_UpdatePixel (uint8_t x, uint8_t y);

uint8_t SSD1306_set_XY(uint8_t , uint8_t );

char SSD1306_WriteChar(char , FontDef_t , SSD1306_COLOR_t );

char SSD1306_WriteString(char* , FontDef_t , SSD1306_COLOR_t );

char SSD1306_Putc(char , FontDef_t* , SSD1306_COLOR_t );

char SSD1306_Puts(char* , FontDef_t* , SSD1306_ALIGN_t align, SSD1306_COLOR_t );

void SSD1306_Putn (uint32_t , FontDef_t* , SSD1306_ALIGN_t, SSD1306_COLOR_t );

uint8_t _Align_text (uint8_t width_num, SSD1306_ALIGN_t align);

char* CodingCP866(char* );

void SSD1306_Invert_Screen (SSD1306_INVERSION_t);

void SSD1306_Fill_Buffer (SSD1306_COLOR_t );

void SSD1306_Fill_Screen (SSD1306_COLOR_t );

void SSD1306_UpdateScreen (void);

uint8_t SSD1306_Return (uint16_t );

SSD1306_COLOR_t SSD1306_GetPixel (uint8_t x, uint8_t y);

uint8_t SSD1306_DrawPixel (uint8_t , uint8_t , SSD1306_COLOR_t );

uint8_t SSD1306_DrawLine (uint8_t , uint8_t , uint8_t , uint8_t , SSD1306_COLOR_t );

uint8_t SSD1306_DrawRectangle (uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t width_Line, SSD1306_COLOR_t color);

uint8_t SSD1306_DrawFillRectangle (uint8_t , uint8_t , uint8_t , uint8_t , SSD1306_COLOR_t );

uint8_t SSD1306_FillImage (uint8_t x, uint8_t y, const uint8_t *data, SSD1306_IMAGE_t display, SSD1306_COLOR_t color);

void OLED_init (void);

void _delay_oled (uint32_t );
