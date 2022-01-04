#include "stm32f10x.h"
#include "stm32f10x_i2c.h"

#if defined (I2C1_USE)
    #ifdef I2C1_USE
        #define I2C_USE I2C1
    #endif

/* Private defines */
#define I2C_TRANSMITTER_MODE   0
#define I2C_RECEIVER_MODE      1
#define I2C_ACK_ENABLE         1
#define I2C_ACK_DISABLE        0

/* I2C address */
#ifndef SSD1306_I2C_ADDR
	#define SSD1306_I2C_ADDR         0x78
//#define SSD1306_I2C_ADDR       0x7A
#endif

#ifndef ssd1306_I2C_TIMEOUT
    #define ssd1306_I2C_TIMEOUT	20000
#endif

uint16_t ssd1306_I2C_Timeout = 0;

void I2C_INIT (void)
{
    //включение тактирования I2C1
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;

    I2C_InitTypeDef i2c;

    i2c.I2C_ClockSpeed = 400000;
    i2c.I2C_Mode = I2C_Mode_I2C;
    i2c.I2C_DutyCycle = I2C_DutyCycle_2;
    i2c.I2C_OwnAddress1 = 0x15;
    i2c.I2C_Ack = I2C_Ack_Disable;
    i2c.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_Init(I2C_USE, &i2c);

    I2C_Cmd(I2C_USE, ENABLE);
}

/* Private functions */
int16_t SSD1306_I2C_Start(uint8_t ack) {
	/* Generate I2C start pulse */
	I2C_USE->CR1 |= I2C_CR1_START;
	
	/* Wait till I2C is busy */
	ssd1306_I2C_Timeout = ssd1306_I2C_TIMEOUT;
	while (!(I2C_USE->SR1 & I2C_SR1_SB)) {
		if (--ssd1306_I2C_Timeout == 0x00) {
			return 1;
		}
	}

	/* Enable ack if we select it */
	if (ack) {
		I2C_USE->CR1 |= I2C_CR1_ACK;
	}

	/* Send address with zero last bit */
	I2C_USE->DR = SSD1306_I2C_ADDR & ~I2C_OAR1_ADD0;
	
	/* Wait till finished */
	ssd1306_I2C_Timeout = ssd1306_I2C_TIMEOUT;
	while (!(I2C_USE->SR1 & I2C_SR1_ADDR)) {
		if (--ssd1306_I2C_Timeout == 0x00) {
			return 1;
		}
	}
	
	/* Read status register to clear ADDR flag */
	I2C_USE->SR2;
	
	/* Return 0, everything ok */
	return 0;
}

uint8_t SSD1306_I2C_Stop(void) {
	/* Wait till transmitter not empty */
	ssd1306_I2C_Timeout = ssd1306_I2C_TIMEOUT;
	while (((!(I2C_USE->SR1 & I2C_SR1_TXE)) || (!(I2C_USE->SR1 & I2C_SR1_BTF)))) {
		if (--ssd1306_I2C_Timeout == 0x00) {
			return 1;
		}
	}
	
	/* Generate stop */
	I2C_USE->CR1 |= I2C_CR1_STOP;
	
	/* Return 0, everything ok */
	return 0;
}

void ssd1306_I2C_WriteData(uint8_t data) {
	/* Wait till I2C is not busy anymore */
	ssd1306_I2C_Timeout = ssd1306_I2C_TIMEOUT;
	while (!(I2C_USE->SR1 & I2C_SR1_TXE) && ssd1306_I2C_Timeout) {
		ssd1306_I2C_Timeout--;
	}

	/* Send I2C data */
	I2C_USE->DR = data;
}

void OLED_SEND_DATA(uint8_t data) {
	SSD1306_I2C_Start(I2C_ACK_DISABLE);
	ssd1306_I2C_WriteData(0x80);
	ssd1306_I2C_WriteData(data);
	SSD1306_I2C_Stop();
}

void OLED_WRITE_BUFFER(uint8_t* data, uint16_t count) {
	uint8_t i;
	SSD1306_I2C_Start(I2C_ACK_DISABLE);
	ssd1306_I2C_WriteData(0x40);
	for (i = 0; i < count; i++) {
		ssd1306_I2C_WriteData(data[i]);
	}
	SSD1306_I2C_Stop();
}
#endif