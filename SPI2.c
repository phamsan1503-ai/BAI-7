#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_spi.h"
#include "stm32f10x_usart.h"
#include <stdio.h>

// ==== CS MAX7219 ====
#define CS_LOW()   GPIO_ResetBits(GPIOA, GPIO_Pin_4)
#define CS_HIGH()  GPIO_SetBits(GPIOA, GPIO_Pin_4)

// ==== Prototype ====
void USART1_Init(uint32_t baudrate);
void USART1_SendString(char *str);
void SPI1_Init(void);
uint8_t SPI1_Transmit(uint8_t data);
void MAX7219_Send(uint8_t address, uint8_t data);
void MAX7219_Init(void);
void delay_ms(uint32_t ms);

// ==== MAIN ====
int main(void)
{
    SystemInit();
    USART1_Init(9600);
    SPI1_Init();
    MAX7219_Init();

    USART1_SendString("MAX7219 Test Start...\r\n");

   while(1)
{
    for (int i=1; i<=8; i++) {
        MAX7219_Send(i, 8);  // hi?n th? s? 8 ? t?t c? digit
    }
    USART1_SendString("All digits = 8\r\n");
    delay_ms(1000);

    for (int i=1; i<=8; i++) {
        MAX7219_Send(i, 0x0F); // t?t t?t c? digit
    }
    USART1_SendString("All digits OFF\r\n");
    delay_ms(1000);
}

}


// ==== SPI1 ====
void SPI1_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1 | RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitTypeDef GPIO_InitStruct;

    // PA5 = SCK, PA7 = MOSI
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    // PA6 = MISO (không dùng nhung v?n c?u hình input)
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    // PA4 = CS
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
    GPIO_SetBits(GPIOA, GPIO_Pin_4);

    SPI_InitTypeDef SPI_InitStruct;
    SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
    SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16; // ~4.5MHz
    SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStruct.SPI_CRCPolynomial = 7;
    SPI_Init(SPI1, &SPI_InitStruct);

    SPI_Cmd(SPI1, ENABLE);
}

uint8_t SPI1_Transmit(uint8_t data)
{
    while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
    SPI_I2S_SendData(SPI1, data);

    while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
    return SPI_I2S_ReceiveData(SPI1);
}

// ==== MAX7219 ====
void MAX7219_Send(uint8_t address, uint8_t data)
{
    CS_LOW();
    SPI1_Transmit(address);
    SPI1_Transmit(data);
    CS_HIGH();
}

void MAX7219_Init(void)
{
    MAX7219_Send(0x09, 0xFF); // Decode mode: t?t c? digits = BCD
    MAX7219_Send(0x0A, 0x0F); // Intensity = max
    MAX7219_Send(0x0B, 0x07); // Scan limit = 7 (c? 8 digit ho?t d?ng)
    MAX7219_Send(0x0C, 0x01); // Normal operation (thoát shutdown)
    MAX7219_Send(0x0F, 0x00); // Display test off
}


// ==== USART1 ====
void USART1_Init(uint32_t baudrate)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitTypeDef GPIO_InitStruct;

    // PA9 = TX
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    // PA10 = RX
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    USART_InitTypeDef USART_InitStruct;
    USART_InitStruct.USART_BaudRate = baudrate;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;
    USART_InitStruct.USART_StopBits = USART_StopBits_1;
    USART_InitStruct.USART_Parity = USART_Parity_No;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_Init(USART1, &USART_InitStruct);

    USART_Cmd(USART1, ENABLE);
}

void USART1_SendString(char *str)
{
    while(*str)
    {
        while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
        USART_SendData(USART1, *str++);
    }
}

// ==== Delay (SysTick) ====
void delay_ms(uint32_t ms)
{
    SysTick->LOAD = 72000 - 1;       // 72MHz / 1000 = 72000
    SysTick->VAL = 0;
    SysTick->CTRL = 5;               // Enable SysTick, HCLK

    for(uint32_t i=0; i<ms; i++)
    {
        while(!(SysTick->CTRL & (1<<16))); // ch? h?t 1ms
    }
    SysTick->CTRL = 0; // T?t SysTick
}
