#include "fonts.h"

#include "SSD1306_def.h"

/* Absolute value */
#ifndef ABS
    #define ABS(x)   ((x) > 0 ? (x) : -(x))
#endif

static uint8_t SSD1306_Buffer [SSD1306_buf_size];

/**
 * @brief  SSD1306 color enumeration
 */
typedef enum {
	SSD1306_COLOR_BLACK = 0x00, /*!< Black color, no pixel */
	SSD1306_COLOR_WHITE = 0xFF,  /*!< Pixel is set. Color depends on LCD */

	SSD1306_COLOR_ERROR = -0x01
} SSD1306_COLOR_t;

void Delay_oled (uint32_t );

void SSD1306_INIT (void);

void SSD1306_UpdateScreen (void);

uint8_t SSD1306_UpdatePixel (uint8_t x, uint8_t y);

uint8_t SSD1306_set_XY(uint8_t , uint8_t );

char SSD1306_WriteChar(char , FontDef_t , SSD1306_COLOR_t );

char SSD1306_WriteString(char* , FontDef_t , SSD1306_COLOR_t );

char SSD1306_Putc(char , FontDef_t* , SSD1306_COLOR_t );

char SSD1306_Puts(char* , FontDef_t* , SSD1306_COLOR_t );

void SSD1306_Putn (uint32_t , FontDef_t* , SSD1306_COLOR_t );

char* CodingCP866(char* );

void SSD1306_Fill_Buffer (SSD1306_COLOR_t );

void SSD1306_Fill_Screen (SSD1306_COLOR_t );

void SSD1306_UpdateScreen (void);

uint8_t SSD1306_Return (uint16_t );

SSD1306_COLOR_t SSD1306_GetPixel (uint8_t x, uint8_t y);

uint8_t SSD1306_DrawPixel (uint8_t , uint8_t , SSD1306_COLOR_t );

uint8_t SSD1306_DrawLine (uint8_t , uint8_t , uint8_t , uint8_t , SSD1306_COLOR_t );

uint8_t SSD1306_DrawRectangle (uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t width_Line, SSD1306_COLOR_t color);

uint8_t SSD1306_DrawFillRectangle (uint8_t , uint8_t , uint8_t , uint8_t , SSD1306_COLOR_t );

void SSD1306_FillImage(uint8_t , uint8_t , uint8_t , uint8_t , const uint8_t * , uint32_t );

void OLED_init (void);