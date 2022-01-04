#include "stm32f10x.h"

#ifndef SSD1306_pins_H
    #include "SSD1306_pins.h"
#endif

#if defined (SPI1_USE) || defined (SPI2_USE)
    #ifdef SPI1_USE
        #define SPI_USE SPI1
    #endif

    #ifdef SPI2_USE
        #define SPI_USE SPI2
    #endif

void SPI_INIT (void)
{
    #ifdef SPI1_USE
        //включение тактирования SPI1
        RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
    #endif
    
    #ifdef SPI2_USE
        //включение тактирования SPI2
        RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;
    #endif
    
    //настройка скорости передачи
    /*!
     * 000: fPCLK/2
     * 001: fPCLK/4
     * 010: fPCLK/8
     * 011: fPCLK/16
     * 100: fPCLK/32
     * 101: fPCLK/64
     * 110: fPCLK/128
     * 111: fPCLK/256
    */
    //SPI1->CR1 |= SPI_CR1_BR;      //установил b111
    //SPI1->CR1 |= SPI_CR1_BR & ((uint16_t)~SPI_CR1_BR_0);  //установил b110
    //SPI1->CR1 |= SPI_CR1_BR & ((uint16_t)~SPI_CR1_BR_1);  //установил b101
    
    /*! ПРЕДЕЛ СКОРОСТИ!!!! */
    SPI_USE->CR1 |= SPI_CR1_BR_2;     //установил b100
    
    //Настройка вывода SCK для периферийного устройства
    /*!
     * Бит 0 CPHA: фаза синхронизации
     * 0: первый тактовый переход является первым фронтом захвата данных
    */
    SPI_USE->CR1 &= (uint16_t)~SPI_CR1_CPHA;
    /*!
     * Бит 1 CPOL: полярность часов
     * 0: CK до 0 в режиме ожидания
    */
    SPI_USE->CR1 &= (uint16_t)~SPI_CR1_CPOL;
    
    //выбор 8 или 16 битного формата
    /*!
     * 0: 8-bit data frame format is selected for transmission/reception
     * 1: 16-bit data frame format is selected for transmission/reception
    */
    SPI_USE->CR1 &= (uint16_t)~SPI_CR1_DFF;  //установил 8 бит
    
    //направление передачи
    /*!
     * 0: MSB transmitted first
     * 1: LSB transmitted first
    */
    SPI_USE->CR1  &= (uint16_t)~SPI_CR1_LSBFIRST;    //установил MSB
    //SPI1->CR1  |= SPI_CR1_LSBFIRST;    //установил LSB
    
    //выбор режима SPI в контроллере
    /*!
     * 0: Slave configuration
     * 1: Master configuration
    */
    SPI_USE->CR1 |= SPI_CR1_MSTR;    //выбираю режим мастера
    
    //Настройка SPI для полудуплексной связи
    /*!
      0: 2-line unidirectional data mode selected
      1: 1-line bidirectional data mode selected
    */
    //SPI1->CR1 &= (uint16_t)~SPI_CR1_BIDIMODE;     //выбираю режим однонаправленной передачи данных по двум линиям
    SPI_USE->CR1 |= SPI_CR1_BIDIMODE;        //выбираю режим двунаправленной передачи данных по одной линии
    
    //Включение вывода в двунаправленном режиме
    /*!
     * Этот бит в сочетании с битом BIDImode выбирает направление передачи в двунаправленном режиме
     * 0: выход отключен (режим только приема)
     * 1: Выход включен (режим только передачи)
    */
    SPI_USE->CR1 |= SPI_CR1_BIDIOE;        //настраиваю на передачу
    //SPI1->CR1 &= (uint16_t)~SPI_CR1_BIDIOE;     //настраиваю на приём
    
    //включение режима только передачи
    /*!
     * 0: Full duplex (Transmit and receive)
     * 1: Output disabled (Receive-only mode)
    */
    SPI_USE->CR1 &= (uint16_t)~SPI_CR1_RXONLY;       //выбираю передачу и прием
    
    //настройка вывода NSS как выход или вход
    /*!
     * 0: выход SS отключен в режиме мастера, и ячейка может работать в конфигурации с несколькими хозяевами
     * 1: выход SS включен в режиме мастера и при включенной ячейке. Ячейка не может работать в среде с несколькими хозяевами.
    */
    SPI_USE->CR2 |= SPI_CR2_SSOE;    //настраиваю NSS как выход в режиме "мастер"
    
    //настройка контакта NSS на аппаратное/программное управление
    /*!
     * В режиме программного обеспечения NSS установите биты SSM и SSI в регистре SPI_CR1.
     * Если вывод NSS требуется на выходе режим, только бит SSOE должен быть установлен.
     * 
     * SSM: Software slave management
     *   Когда бит SSM установлен, вход контакта NSS заменяется значением из бита SSI.
     *   0: Software slave management disabled
     *   1: Software slave management enabled
     * SSI: выбор внутреннего ведомого
     *   Этот бит действует только тогда, когда установлен бит SSM.
     *   Значение этого бита подается на вывод NSS, а значение IO на выводе NSS игнорируется.
    */
    SPI_USE->CR1 &= (uint16_t)~SPI_CR1_SSM;  //выбираю аппаратное управление
    
    //включение SPI1
    SPI_USE->CR1 |= SPI_CR1_SPE;
}

void OLED_SEND_DATA (uint8_t Data)
{    
    /* Write in the DR register the data to be sent */
    SPI_USE->DR = Data;

    while ((SPI_USE->SR & SPI_SR_BSY))
    {
        //ожидание загрузки из буфера DR в регистр сдвига
    }
}

void OLED_WRITE_BUFFER (uint8_t* data, uint16_t count)
{
    //запись данных, DC = 1
    SET_Pins(OLED_PIN_D_C);

    for (uint8_t i = 0; i < count; i++)
    {
        OLED_SEND_DATA(data[i]);
    }

    //запись данных, DC = 0
    RESET_Pins(OLED_PIN_D_C);
}
#endif