#ifndef SSD1306_pins_H
    #define SSD1306_pins_H

#if defined(STM32F10X_MD)
    #include "stm32f10x.h"
    #include "stm32f10x_gpio.h"

    #ifdef SPI1_USE
        #define OLED_PORT GPIOA

        #define OLED_PIN_DATA   GPIO_Pin_7
        #define OLED_PIN_SCK    GPIO_Pin_5

        #define OLED_PIN_C_S    GPIO_Pin_4
        #define OLED_PIN_D_C    GPIO_Pin_3
        #define OLED_PIN_RESET  GPIO_Pin_2
    #endif

    #ifdef SPI2_USE
        #define OLED_PORT GPIOB

        #define OLED_PIN_DATA   GPIO_Pin_15
        #define OLED_PIN_SCK    GPIO_Pin_13

        #define OLED_PIN_C_S    GPIO_Pin_12
        #define OLED_PIN_D_C    GPIO_Pin_11
        #define OLED_PIN_RESET  GPIO_Pin_10
    #endif

    #ifdef I2C1_USE
        #define OLED_PORT GPIOB

        #define OLED_PIN_DATA   GPIO_Pin_7
        #define OLED_PIN_SCK    GPIO_Pin_6
    #endif

    #if !defined (SPI1_USE) && !defined (SPI2_USE) && !defined (I2C1_USE)
        #error "Please select INTERFACE (in SSD1306.c file)"
    #endif

    #define SET_Pins(Pins) (OLED_PORT->BSRR |= (uint32_t)(Pins))
    #define RESET_Pins(Pins) (OLED_PORT->BSRR |= (uint32_t)(Pins)<<16)

    void OLED_GPIO_INIT (void)
    {
        #ifdef SPI1_USE
            //включение тактирования альтернативных функций
            RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
            //включение тактирования порта РА
            RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
        #endif

        #ifdef SPI2_USE
            //включение тактирования альтернативных функций
            RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
            //включение тактирования порта РB
            RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
        #endif

        #ifdef I2C1_USE
            //включение тактирования альтернативных функций
            RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
            //включение тактирования порта РB
            RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
        #endif
            
        GPIO_InitTypeDef OLED_GPIO_INIT_STRUCT = 
        {
            .GPIO_Pin = GPIO_Pin_0,
            .GPIO_Speed = GPIO_Speed_50MHz,
            .GPIO_Mode = GPIO_Mode_Out_PP
        };

        #if defined (SPI1_USE) || defined (SPI2_USE)
            OLED_GPIO_INIT_STRUCT.GPIO_Pin = OLED_PIN_D_C;
            GPIO_Init (OLED_PORT, &OLED_GPIO_INIT_STRUCT);

            OLED_GPIO_INIT_STRUCT.GPIO_Pin = OLED_PIN_RESET;
            GPIO_Init (OLED_PORT, &OLED_GPIO_INIT_STRUCT);

            OLED_GPIO_INIT_STRUCT.GPIO_Mode = GPIO_Mode_AF_PP;
            OLED_GPIO_INIT_STRUCT.GPIO_Pin = OLED_PIN_C_S;
            GPIO_Init (OLED_PORT, &OLED_GPIO_INIT_STRUCT);
        #endif
            
        #if defined (I2C1_USE)
            OLED_GPIO_INIT_STRUCT.GPIO_Mode = GPIO_Mode_AF_OD;
        #endif
        
        OLED_GPIO_INIT_STRUCT.GPIO_Pin = OLED_PIN_DATA;
        GPIO_Init (OLED_PORT, &OLED_GPIO_INIT_STRUCT);
        OLED_GPIO_INIT_STRUCT.GPIO_Pin = OLED_PIN_SCK;
        GPIO_Init (OLED_PORT, &OLED_GPIO_INIT_STRUCT);
    }
#endif  //#if defined(STM32F10X_MD)

#endif  //#ifndef SSD1306_pins_H