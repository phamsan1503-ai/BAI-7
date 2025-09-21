#include "stm32f10x.h"

// ==== Ð?nh nghia chân CS ====
#define MAX7219_CS_LOW()   GPIO_ResetBits(GPIOA, GPIO_Pin_4)
#define MAX7219_CS_HIGH()  GPIO_SetBits(GPIOA, GPIO_Pin_4)

// ==== SPI g?i 1 byte ====
uint8_t SPI1_Send(uint8_t data) {
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
    SPI_I2S_SendData(SPI1, data);
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
    return SPI_I2S_ReceiveData(SPI1);
}

// ==== G?i 2 byte t?i MAX7219 ====
void MAX7219_Send(uint8_t address, uint8_t data) {
    MAX7219_CS_LOW();
    SPI1_Send(address);
    SPI1_Send(data);
    MAX7219_CS_HIGH();
}

// ==== Kh?i t?o MAX7219 ====
void MAX7219_Init(void) {
    MAX7219_Send(0x09, 0xFF); // Decode mode: t?t c? 8 digit
    MAX7219_Send(0x0A, 0x08); // Intensity: trung bình (0x00–0x0F)
    MAX7219_Send(0x0B, 0x07); // Scan limit: hi?n th? d? 8 digit
    MAX7219_Send(0x0C, 0x01); // Shutdown: 1 = normal
    MAX7219_Send(0x0F, 0x00); // Test display: off

    // Clear display
    for (uint8_t i = 1; i <= 8; i++) {
        MAX7219_Send(i, 0x0F); // blank
    }
}

// ==== C?u hình SPI1 và GPIO ====
void SPI1_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    SPI_InitTypeDef  SPI_InitStructure;

    // Clock GPIOA + SPI1
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_SPI1, ENABLE);

    // PA5 (SCK), PA7 (MOSI) = AF_PP
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // PA4 (CS) = Output push-pull
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    MAX7219_CS_HIGH();

    // SPI1 config
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

// ==== Hi?n th? s? 8 ch?y t? digit 1 -> digit 8 r?i t?t d?n ====
void Show_Effect(void) {
    // B?t d?n t?ng digit = s? 8
    for (uint8_t i = 1; i <= 8; i++) {
        MAX7219_Send(i, 8); // Hi?n th? s? 8 t?i digit i
        for (volatile int d = 0; d < 500000; d++); // delay thô
    }

    // T?t d?n t?ng digit
    for (int i = 8; i >= 1; i--) {
        MAX7219_Send(i, 0x0F); // Blank
        for (volatile int d = 0; d < 500000; d++);
    }
}

// ==== MAIN ====
int main(void) {
    SPI1_Init();
    MAX7219_Init();

    while (1) {
        Show_Effect();
    }
}
