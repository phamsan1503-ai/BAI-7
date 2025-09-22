#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_spi.h"
#include "stm32f10x_tim.h"

#define CS_PIN   GPIO_Pin_4
#define CS_PORT  GPIOA

void delay_ms(uint32_t ms) {
    uint32_t i, j;
    for(i=0; i<ms; i++) {
        for(j=0; j<8000; j++) __NOP();
    }
}

void GPIO_Config(void) {
    GPIO_InitTypeDef gpio;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    // CS: PA4
    gpio.GPIO_Pin = CS_PIN;
    gpio.GPIO_Mode = GPIO_Mode_Out_PP;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(CS_PORT, &gpio);

    // SPI1 SCK (PA5), MOSI (PA7)
    gpio.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;
    gpio.GPIO_Mode = GPIO_Mode_AF_PP;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpio);

   
}

void SPI_Config(void) {
    SPI_InitTypeDef spi;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

    spi.SPI_Direction = SPI_Direction_1Line_Tx;
    spi.SPI_Mode = SPI_Mode_Master;
    spi.SPI_DataSize = SPI_DataSize_16b;   
    spi.SPI_CPOL = SPI_CPOL_Low;
    spi.SPI_CPHA = SPI_CPHA_1Edge;
    spi.SPI_NSS = SPI_NSS_Soft;
    spi.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
    spi.SPI_FirstBit = SPI_FirstBit_MSB;
    spi.SPI_CRCPolynomial = 7;
    SPI_Init(SPI1, &spi);

    SPI_Cmd(SPI1, ENABLE);
}

void MAX7219_Send(uint8_t address, uint8_t data) {
    GPIO_ResetBits(CS_PORT, CS_PIN);
    SPI_I2S_SendData(SPI1, (address << 8) | data);
    while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY));
    GPIO_SetBits(CS_PORT, CS_PIN);
}

void MAX7219_Init(void) {
    MAX7219_Send(0x09, 0x00); 
    MAX7219_Send(0x0A, 0x0F); 
    MAX7219_Send(0x0B, 0x07); 
    MAX7219_Send(0x0C, 0x01); 
    MAX7219_Send(0x0F, 0x00); 
}

int main(void) {
    SystemInit();
    GPIO_Config();
    SPI_Config();
    MAX7219_Init();

    while(1) {
       
        for(int d=1; d<=8; d++) {
            MAX7219_Send(d, 0x7F); 
            delay_ms(300);
        }


        for(int d=8; d>=1; d--) {
            MAX7219_Send(d, 0x00); 
            delay_ms(300);
        }
    }
}


