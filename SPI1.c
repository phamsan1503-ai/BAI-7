#include "stm32f10x.h"


#define MAX7219_CS_LOW()   GPIO_ResetBits(GPIOA, GPIO_Pin_4)
#define MAX7219_CS_HIGH()  GPIO_SetBits(GPIOA, GPIO_Pin_4)

uint8_t SPI1_Send(uint8_t data) {
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
    SPI_I2S_SendData(SPI1, data);
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
    return SPI_I2S_ReceiveData(SPI1);
}


void MAX7219_Send(uint8_t address, uint8_t data) {
    MAX7219_CS_LOW();
    SPI1_Send(address);
    SPI1_Send(data);
    MAX7219_CS_HIGH();
}


void MAX7219_Init(void) {
    MAX7219_Send(0x09, 0xFF); 
    MAX7219_Send(0x0A, 0x08); 
    MAX7219_Send(0x0B, 0x07);
    MAX7219_Send(0x0C, 0x01); 
    MAX7219_Send(0x0F, 0x00); 

   
    for (uint8_t i = 1; i <= 8; i++) {
        MAX7219_Send(i, 0x0F); // blank
    }
}


void SPI1_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    SPI_InitTypeDef  SPI_InitStructure;


    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_SPI1, ENABLE);


    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);


    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    MAX7219_CS_HIGH();


    SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_Init(SPI1, &SPI_InitStructure);

    SPI_Cmd(SPI1, ENABLE);
}


void Show_Effect(void) {

    for (uint8_t i = 1; i <= 8; i++) {
        MAX7219_Send(i, 8); // Hi?n th? s? 8 t?i digit i
        for (volatile int d = 0; d < 500000; d++); // delay thô
    }


    for (int i = 8; i >= 1; i--) {
        MAX7219_Send(i, 0x0F); // Blank
        for (volatile int d = 0; d < 500000; d++);
    }
}


int main(void) {
    SPI1_Init();
    MAX7219_Init();

    while (1) {
        Show_Effect();
    }
}

