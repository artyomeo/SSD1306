#include "SSD1306.h"

#define OLED_USE_SPI1

#ifdef OLED_USE_SPI1
    #define SPI1_USE
    #define SSD1306_ADDR_COLUMN_BIAS 0x00
#elif defined(OLED_USE_SPI2)
    #define SPI2_USE
    #define SSD1306_ADDR_COLUMN_BIAS 0x00
#elif defined(OLED_USE_I2C1)
    #define I2C1_USE
    #define SSD1306_ADDR_COLUMN_BIAS 0x02
#else
    #error "Choose type interface for connection OLED" 
#endif

#if defined (SPI1_USE) || defined (SPI2_USE)
    #include "SSD1306_SPI_INIT.h"
#endif

#if defined (I2C1_USE)
    #include "SSD1306_I2C_INIT.h"
#endif

#include "SSD1306_pins.h"

#define CHECK_XY(x,y) ((SSD1306_WIDTH > x) && (SSD1306_HEIGHT > y))

#define SSD1306_SWAP(x, y) (((x) ^= (y)), ((y) ^= (x)), ((x) ^= (y)))

/* Private SSD1306 structure */
static struct {
	uint8_t CurrentX;
	uint8_t CurrentY;
	uint8_t Inverted;
	uint8_t Initialized;
} SSD1306;

void _delay_oled (uint32_t count)
{  for(;count > 0; count--);  }

void SSD1306_INIT (void)
{   
#if defined (SPI1_USE) || defined (SPI2_USE)
    OLED_GPIO_INIT ();
    _delay_oled (100);
    SPI_INIT ();

    //запись данных, reset = 0
    RESET_Pins(OLED_PIN_RESET);
    _delay_oled (100);
    //запись данных, reset = 1
    SET_Pins(OLED_PIN_RESET);
#endif
    
#if defined (I2C1_USE)
    I2C_INIT ();
    _delay_oled (100);
    OLED_GPIO_INIT ();
#endif
    
    _delay_oled (100);
    
    OLED_init ();

    /* Set default values */
    SSD1306.CurrentX = 0;
    SSD1306.CurrentY = 0;

    /* Initialized OK */
    SSD1306.Initialized = 1;
}

uint8_t SSD1306_set_XY (uint8_t x, uint8_t y) 
{
    /* Проверка входных параметров */
    if (CHECK_XY(x,y))
    {
        /* Set write pointers */
        SSD1306.CurrentX = x;
        SSD1306.CurrentY = y;

        return 0;
    }

    /* Error */
    return 1;
}

char SSD1306_Putc(char ch, FontDef_t* Font, SSD1306_COLOR_t color)
{
    // Check remaining space on current line
    if (CHECK_XY((SSD1306.CurrentX + Font->FontWidth),(SSD1306.CurrentY + Font->FontHeight)))
    {
        // Use the font to write
        for (uint8_t i = 0; i < Font->FontHeight; i++)
        {
            uint16_t b = Font->data[(ch - 32) * Font->FontHeight + i];

            for (uint8_t j = 0; j < Font->FontWidth; j++)
            {
                if ((b << j) & 0x8000)
                {
                    SSD1306_DrawPixel((SSD1306.CurrentX + j), (SSD1306.CurrentY + i), color);
                } 
                else 
                {
                    SSD1306_DrawPixel((SSD1306.CurrentX + j), (SSD1306.CurrentY + i), !color);
                }
            }
        }

        // The current space is now taken
        SSD1306.CurrentX += Font->FontWidth;

        // Return written char for validation
        return ch;
    }

    /* Error */
    return 1;
}

char SSD1306_Puts(char* str, FontDef_t* Font, SSD1306_ALIGN_t align, SSD1306_COLOR_t color)
{   
    CodingCP866 (str);

    _Align_text ((strlen(str) * Font->FontWidth), align);
    
    /* Write characters */
    while (*str)
    {
        /* Write character by character */
        if (SSD1306_Putc(*str, Font, color) != *str)
        {
            /* Return error */
            return *str;
        }
        /* Increase string pointer */
        str++;
    }
    /* Everything OK, zero should be returned */
    return *str;
}

/**
  * @brief  Вывод беззнакового числа на OLED
  * @param  num_in: число типа uint32_t
  * @param  Font: тип шрифта для отображения числа
  * @param  color: цвет отображаемого числа 
  * @retval None
  */
void SSD1306_Putn (uint32_t num_in, FontDef_t* Font, SSD1306_ALIGN_t align, SSD1306_COLOR_t color)
{
    char buf_num [12] = {'\0'};
    uint8_t count_buf = 0;

    uint32_t num_var = num_in;
    uint32_t num_div = 1000000000;

    num_var = num_in / num_div;

    while (num_div)
    {
        if ((num_var) || (count_buf))
        {
            buf_num[count_buf++] = num_var + BIAS_ASCII;
        }

        num_var = num_in % num_div;
        num_div = num_div / 10;
        if (num_div)
            num_var = num_var / num_div;
    }

    SSD1306_Puts (buf_num, Font, align, color);
}

uint8_t _Align_text (uint8_t width_num, SSD1306_ALIGN_t align)
{
    uint8_t var_width = 0;
    switch (align)
    {
        case SSD1306_ALIGN_CENTER_P:
            var_width = (width_num >> 1);
            if (CHECK_XY (SSD1306.CurrentX+var_width, SSD1306.CurrentY))
            {
                if (SSD1306_set_XY(SSD1306.CurrentX-var_width, SSD1306.CurrentY))
                    SSD1306_set_XY(0, SSD1306.CurrentY);

                break;  // относится к ближайшему оператору do, for, switch или while
            }
        
        case SSD1306_ALIGN_RIGHT:
            if (SSD1306_set_XY(SSD1306.CurrentX-width_num, SSD1306.CurrentY))
                SSD1306_set_XY(0, SSD1306.CurrentY);
        break;

        case SSD1306_ALIGN_CENTER_S:
            if (SSD1306_set_XY((SSD1306_WIDTH >> 1) - (width_num >> 1), SSD1306.CurrentY))
                SSD1306_set_XY(0, SSD1306.CurrentY);
        break;
    
        default:
        break;
    }

    /* No error */
    return 0;
}

/**
  * @brief  Отрисовка пикселя в буффере
  * @param  x: координата по оси Х (0 <= x <= (SSD1306_WIDTH - 1))
  * @param  y: координата по оси Y (0 <= y <= (SSD1306_HEIGHT - 1))
  * @param  color: цвет отображаемого числа: SSD1306_COLOR_BLACK or SSD1306_COLOR_WHITE
  * @retval uint8_t: 0 - успешно, 1 - ошибка
  */
uint8_t SSD1306_DrawPixel (uint8_t x, uint8_t y, SSD1306_COLOR_t color)
{
    if (CHECK_XY(x,y))
    {
        /* Set color */
        if (color) 
            SSD1306_Buffer[x + ((y / 8) * SSD1306_WIDTH)] |=  (1 << (y % 8));
        else
            SSD1306_Buffer[x + ((y / 8) * SSD1306_WIDTH)] &= ~(1 << (y % 8));

        return 0;
    }

    /* Error */
    return 1;
}

/**
  * @brief  Отрисовка линии по двум группам координат
  * @param  x1: начальная координата по оси Х (0 <= x <= (SSD1306_WIDTH - 1))
  * @param  y1: начальная координата по оси Y (0 <= y <= (SSD1306_HEIGHT - 1))
  * @param  x2: конечная координата по оси Х (0 <= x <= (SSD1306_WIDTH - 1))
  * @param  y2: конечная координата по оси Y (0 <= y <= (SSD1306_HEIGHT - 1))
  * @param  color: цвет отображаемого числа: SSD1306_COLOR_BLACK or SSD1306_COLOR_WHITE
  * @retval uint8_t: 0 - успешно, 1 - ошибка
  */
uint8_t SSD1306_DrawLine (uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, SSD1306_COLOR_t color)
{
    if (CHECK_XY(x1,y1) && CHECK_XY(x2,y2))
    {
        int8_t dx = x2 - x1;
        int8_t dy = y2 - y1;

        if (ABS(dx) > ABS(dy))
        {
            if (dx < 0)
            {
                SSD1306_SWAP (x1, x2);
                SSD1306_SWAP (y1, y2);
            }
            for (uint16_t x = x1; x <= x2; x++)
                SSD1306_DrawPixel(x, ((x-x1)*dy/dx+y1), color);
        }
        else
        {
            if (dy < 0)
            {
                SSD1306_SWAP (x1, x2);
                SSD1306_SWAP (y1, y2);
            }
            for (uint16_t y = y1; y <= y2; y++)
                SSD1306_DrawPixel(((y-y1)*dx/dy+x1), y, color);
        }

        return 0;
    }

    /* Error */
    return 1;
}

uint8_t SSD1306_DrawRectangle (uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t width_Line, SSD1306_COLOR_t color)
{
    if (CHECK_XY(x1,y1) && CHECK_XY(x2,y2))
    {
        if (x1 > x2)
            SSD1306_SWAP (x1, x2);

        if (y1 > y2)
            SSD1306_SWAP (y1, y2);
        
        if ((x2-x1) > (y2-y1))
        {
            if (((y2-y1)>>1) < width_Line)
                width_Line = ((y2-y1)>>1)+1;
        }
        else
        {
            if (((x2-x1)>>1) < width_Line)
                width_Line = ((x2-x1)>>1)+1;
        }
        
        SSD1306_DrawFillRectangle (x1, y1, x2, (y1+width_Line-1), color);
        SSD1306_DrawFillRectangle (x1, y1, (x1+width_Line-1), y2, color);
        SSD1306_DrawFillRectangle ((x2-width_Line+1), y1, x2, y2, color);
        SSD1306_DrawFillRectangle (x1, (y2-width_Line+1), x2, y2, color);

        return 0;
    }

    /* Error */
    return 1;
}

uint8_t SSD1306_DrawFillRectangle (uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, SSD1306_COLOR_t color)
{
    if (CHECK_XY(x1,y1) && CHECK_XY(x2,y2))
    {    
        for(int i = y1; i <= y2; i++)
        {
            for(int j = x1; j <= x2; j++)
            {
                SSD1306_DrawPixel(j, i, color);
            }
        }
        
        return 0;
    }

    /* Error */
    return 1;
}

void SSD1306_FillImage(uint8_t x, uint8_t y, uint8_t width, uint8_t height, const uint8_t * data, uint32_t size)
{
    uint32_t i = 0;
    uint8_t nbit = 0;
    uint32_t ii = sizeof(*data);

    if(ii+x > width) ii = 0;

    if((x >= SSD1306_WIDTH) || (y >= SSD1306_HEIGHT)) return;

    for(uint8_t row = 0; row < height; row++)
    {
        for(uint8_t col = 0; col < width; col++)
        {
            SSD1306_DrawPixel(x + col, y + row, ((data[i] & (0x80 >> nbit++)) ? SSD1306_COLOR_BLACK : SSD1306_COLOR_WHITE));
            if(nbit == 8)
            {
                i++;
                nbit = 0;
                if(i >= size) return;
            }
        }
    }
}

/**
  * @brief  заполнение буффера выбранным цветом
  * @param  color: цвет отображаемого числа: SSD1306_COLOR_BLACK or SSD1306_COLOR_WHITE
  * @retval None
  */
void SSD1306_Fill_Buffer (SSD1306_COLOR_t color)
{
    /* Set memory */
    for(int i = 0; i < SSD1306_buf_size; i++)
    {
        SSD1306_Buffer[i] = color;
    }
}

/**
  * @brief  заполнение экрана выбранным цветом (заполняет буффер и обновляет экран)
  * @param  color: цвет отображаемого числа: SSD1306_COLOR_BLACK or SSD1306_COLOR_WHITE
  * @retval None
  */
void SSD1306_Fill_Screen (SSD1306_COLOR_t color)
{
    SSD1306_Fill_Buffer ( color );
    SSD1306_UpdateScreen ();
}

/**
  * @brief  обновление данных всего экрана
  * @param  None
  * @retval None
  */
void SSD1306_UpdateScreen (void)
{
    for (uint8_t m = 0; m < 8; m++)
    {
        OLED_SEND_DATA (SSD1306_ADDR_ONE_PAGE + m);     //выбор страницы
        OLED_SEND_DATA (SSD1306_ADDR_COLUMN_BIAS);      //установка верхней границы страницы (в процессе записи данных внутренний счетки смещается)
        OLED_SEND_DATA (SSD1306_ADDR_COLUMN_MBS);       //установка нижней границы страницы

        OLED_SEND_DATA (SSD1306_SET_START_LINE);        //регистр памяти для записи данных в экран
        
        /* Запись потока данных из буфера на выбранную страницу */
        OLED_WRITE_BUFFER (&SSD1306_Buffer[SSD1306_WIDTH * m], SSD1306_WIDTH);
    }
}

/**
  * @brief  обновление данных одной страницы до указанного пикселя
  * @param  x: координата пикселя по ширине
  * @param  y: координата пикселя по высоте
  * @retval uint8_t: 0 - успешно, 1 - ошибка
  */
uint8_t SSD1306_UpdatePixel (uint8_t x, uint8_t y)
{
    if (CHECK_XY(x,y))
    {
        OLED_SEND_DATA (SSD1306_ADDR_ONE_PAGE + (y / 8));   //выбор страницы
        OLED_SEND_DATA (SSD1306_ADDR_COLUMN_BIAS);          //установка верхней границы страницы (в процессе записи данных внутренний счетки смещается)
        OLED_SEND_DATA (SSD1306_ADDR_COLUMN_MBS);           //установка нижней границы страницы

        OLED_SEND_DATA (SSD1306_SET_START_LINE);            //регистр памяти для записи данных в экран
        
        /* Запись потока данных из буфера на выбранную страницу */
        OLED_WRITE_BUFFER (&SSD1306_Buffer[(y / 8) * SSD1306_WIDTH], x);

        return 0;
    }

    /* Error */
    return 1;
}

//получение элемента массива буфера из основной функции
uint8_t SSD1306_Return (uint16_t number)
{
    if (number < SSD1306_buf_size)
        return (SSD1306_Buffer[number]);

    return 0;
}

/**
  * @brief  возврат цвета пикселя
  * @param  x: координата пикселя по ширине
  * @param  y: координата пикселя по высоте
  * @retval SSD1306_COLOR_t: SSD1306_COLOR_WHITE или SSD1306_COLOR_BLACK
  */
SSD1306_COLOR_t SSD1306_GetPixel (uint8_t x, uint8_t y)
{
    if (CHECK_XY(x,y))
    {
        if (SSD1306_Buffer[x + ((y / 8) * SSD1306_WIDTH)] & ((uint8_t)1 << (y % 8)))
            return SSD1306_COLOR_WHITE;
        else
            return SSD1306_COLOR_BLACK;
    }
    
    return SSD1306_COLOR_ERROR;
}

/**
  * @brief  ДЛЯ ВНУТРЕННЕГО ПОЛЬЗОВАНИЯ!!!
  *         Функция декодирования из расширенного UTF-8 (16 бит) в  простой UTF-8 (8 бит) или CP-866
  * @param  *StrIn: строка в кодировке расширенного UTF-8 (16 бит)
  * @retval StrOut: адрес буффера, в котором записаны новые элементы
  */
char* CodingCP866(char* StrIn)
{
  char*	    StrOut=StrIn;																			//	Определяем строку для вывода результата.
  uint8_t	numIn    =	0,																			//	Определяем переменную хранящую номер символа в строке StrIn.
            numOut   =	0,																			//	Определяем переменную хранящую номер символа в строке StrOut.
            charThis =	StrIn[0],																    //	Определяем переменную хранящую код текущего символа в строке StrIn.
            charNext =	StrIn[1];																	//	Определяем переменную хранящую код следующего символа в строке StrIn.
																							        //	Тип кодировки строки StrIn.
  //	Преобразуем текст из кодировки UTF-8:
  while ((charThis > 0) && (numIn < 0xFF))
  {												                                                    //	Если код текущего символа строки StrIn больше 0 и № текушего символа строки StrIn меньше 255, то ...
    if(charThis==0xD0&&charNext>=0x90&&charNext<=0xBF){StrOut[numOut]=charNext-0x10; numIn++;}else	//	Если код текущего символа равен 208, а за ним следует символ с кодом 144...191, значит это буква «А»...«п» требующая преобразования к коду 128...175
    if(charThis==0xD0&&charNext==0x81                ){StrOut[numOut]=         0xF0; numIn++;}else	//	Если код текущего символа равен 208, а за ним следует символ с кодом 129      , значит это буква «Ё»       требующая преобразования к коду 240
    if(charThis==0xD1&&charNext>=0x80&&charNext<=0x8F){StrOut[numOut]=charNext+0x60; numIn++;}else	//	Если код текущего символа равен 209, а за ним следует символ с кодом 128...143, значит это буква «р»...«я» требующая преобразования к коду 224...239
    if(charThis==0xD1&&charNext==0x91                ){StrOut[numOut]=         0xF1; numIn++;}else	//	Если код текущего символа равен 209, а за ним следует символ с кодом 145      , значит это буква «ё»       требующая преобразования к коду 241
                                                      {StrOut[numOut]=charThis;}     numIn++;		//	Иначе не меняем символ.
    numOut++; charThis=StrIn[numIn]; charNext=StrIn[numIn+1];										//	Переходим к следующему символу.
  }
  
  StrOut[numOut]='\0';																                //	Добавляем символ конца строки и возвращаем строку StrOut.
  
  return StrOut;																				    //	Возвращаем строку StrOut.
}

void OLED_init (void)
{
    /* Init LCD */
    OLED_SEND_DATA( SSD1306_DISPLAY_OFF );       //	Выключить дисплей (спящий режим).

    //	Выбор режима адресации. За данной командой должен быть отправлен байт младшие биты которого определяют режим:
    //  00-горизонтальная (с переходом на новую страницу (строку)) / 01-вертикальная (с переходом на новую колонку) / 10-страничная (только по выбранной странице).
    OLED_SEND_DATA( SSD1306_ADDR_MODE );                OLED_SEND_DATA(0x10);

    OLED_SEND_DATA( SSD1306_ADDR_ONE_PAGE );     //	Установка номера страницы которая будет выбрана для режима страничной адресации. У данной команды младщие 3 бита должны быть изменены на номер страницы. Комадна предназначена только для режима страничной адресации.
    
    OLED_SEND_DATA( SSD1306_SET_REMAP_T_TO_D );  //	Установить режим кадровой развёртки сверху-вниз
    OLED_SEND_DATA( SSD1306_ADDR_COLUMN_LBS );   //	Установка младшей части адреса колонки на странице. У данной команды младщие 4 бита должны быть изменены на младшие биты адреса. Комадна предназначена только для режима страничной адресации.
    OLED_SEND_DATA( SSD1306_ADDR_COLUMN_MBS );   //	Установка старшей части адреса колонки на странице. У данной команды младщие 4 бита должны быть изменены на старшие биты адреса. Комадна предназначена только для режима страничной адресации.
    OLED_SEND_DATA( SSD1306_SET_START_LINE );    //	Установить начальный адрес ОЗУ (смещение памяти). У данной команды младщие 6 битов должны быть изменены на начальный адрес ОЗУ.

    //	Установка контрастности. За данной командой должен быть отправлен байт контрастности от 00 до FF (по умолчанию 7F=127).
    OLED_SEND_DATA( SSD1306_SET_CONTRAST );             OLED_SEND_DATA(0xFF);

    OLED_SEND_DATA( SSD1306_SET_REMAP_L_TO_R );  //	Установить режим строчной развёртки слева-направо

    //--set normal display
    OLED_SEND_DATA( SSD1306_INVERT_OFF );        //	Активный бит данных = 1. Пиксель установленный в 1 будет светиться.

    //  set multiplex ratio(1 to 64)
    //	Установить multiplex ratio (количество используемых выводов COM для вывода данных на дисплей). За данной командой должны быть отправлен 1 байт с указанием количества COM выводов (от 15 до 63).
    OLED_SEND_DATA( SSD1306_SET_MULTIPLEX_RATIO );      OLED_SEND_DATA(0x3F);

    OLED_SEND_DATA( SSD1306_RAM_ON );            //	Включить  изображение. Разрешить отображать содержимое RAM памяти.

    //	Установить смещение дисплея. За данной командой должны быть отправлен 1 байт с указанием вертикального сдвига чтения выходов COM (от 0 до 63).
    OLED_SEND_DATA( SSD1306_SET_DISPLAY_OFFSET );       OLED_SEND_DATA(0x00); //-not offset

    //	Установить частоту обновления дисплея. За данной командой должны быть отправлен 1 байт, старшие 4 бита которого определяют частоту, а младшие делитель.
    OLED_SEND_DATA( SSD1306_SET_DISPLAY_CLOCK );
    OLED_SEND_DATA(0xF0); //--set divide ratio

    //	Установить Фазы DC/DC преобразователя. За данной командой должны быть отправлен 1 байт, старшие 4 бита которого определяют период, а младшие период.
    OLED_SEND_DATA( SSD1306_SET_PRECHARGE_PERIOD );     OLED_SEND_DATA(0x22);

    //	Установить тип аппаратной конфигурации COM выводов. За данной командой должны быть отправлен 1 байт, у которого 4-ый бит: 0-последовательная / 1-альтернативная, 5-ой бит: 0-отключить COM Left/Right remap / 1-включить COM Left/Right remap.
    OLED_SEND_DATA( SSD1306_SET_COM_PINS );             OLED_SEND_DATA(0x12);

    //  Установить VcomH (влияет на яркость).  За данной командой должны быть отправлен 1 байт, старшие 4 бита которого определяют напряжение (пример: 0000 - VcomH=0.65Vcc, 0010 - VcomH=0.77Vcc, 0011 - VcomH=0.83Vcc и т.д.).
    OLED_SEND_DATA( SSD1306_SET_VCOM_DESELECT );        OLED_SEND_DATA(0x20); //0x20,0.77xVcc

    //	Управление DC/DC преобразователем.     За данной командой должны быть отправлен 1 байт: 0x10 - отключить (VCC подается извне), 0x14 - запустить внутренний DC/DC.
    OLED_SEND_DATA( SSD1306_CHARGE_DCDC_PUMP );         OLED_SEND_DATA(0x14);

    OLED_SEND_DATA( SSD1306_DISPLAY_ON );        //	Включить  дисплей (нормальный режим).

    SSD1306_Fill_Screen (SSD1306_COLOR_BLACK);
}
