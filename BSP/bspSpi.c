/*
    bspSpi.c

    Board support for controlling SPI interfaces on NUCLEO-F401RE MCU

    Developed for University of Washington embedded systems programming certificate
    
    2016/2 Nick Strathy wrote/arranged it
*/

#include "bsp.h"

// BspSPI1Init
// Initializes the SPI1 memory mapped register block and enables it for use
// as a master SPI device.
void BspSPI1Init()
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

    SPI_InitTypeDef SPI_InitTypeDefStruct;
     
    SPI_InitTypeDefStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex; 
    SPI_InitTypeDefStruct.SPI_Mode = SPI_Mode_Master; 
    SPI_InitTypeDefStruct.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitTypeDefStruct.SPI_CPOL = SPI_CPOL_Low; 
    SPI_InitTypeDefStruct.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitTypeDefStruct.SPI_NSS = SPI_NSS_Soft;
    SPI_InitTypeDefStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;
    SPI_InitTypeDefStruct.SPI_FirstBit = SPI_FirstBit_MSB;
     
    SPI_Init(SPI1, &SPI_InitTypeDefStruct);
 
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
 
    GPIO_InitTypeDef GPIO_InitStruct;
    
    /*-------- Configure SCK, MISO, MOSI --------*/
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
     
    GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    /*--------- Configure alternate GPIO functions to SPI1 ------*/
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_SPI1);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_SPI1);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_SPI1);

    SPI_Cmd(SPI1, ENABLE);
}


// SPI_SendBuffer
// Sends the given data to the given SPI device
void SPI_SendBuffer(SPI_TypeDef *spi, uint8_t *buffer, uint16_t bufLength)
{    
    for (int i = 0; i < bufLength; i++) {
        while(!SPI_I2S_GetFlagStatus(spi, SPI_I2S_FLAG_TXE)); 
        SPI_I2S_SendData(spi, buffer[i]);
        while(!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE));
        SPI_I2S_ReceiveData(SPI1);
    }
 }

// SPI_GetBuffer
// Sends the given data to the given SPI device.
// buffer: on entry contains the command to retrieve data from the spi device.
//    On exit the buffer is OVERWRITTEN with the data output by the device
//    in response to the command.
void SPI_GetBuffer(SPI_TypeDef *spi, uint8_t *buffer, uint16_t bufLength)
{
    int iread = 0;

    for (int i = 0; i < bufLength; i++) {
        while(!SPI_I2S_GetFlagStatus(spi, SPI_I2S_FLAG_TXE)); 
        SPI_I2S_SendData(spi, buffer[i]);
        while(!SPI_I2S_GetFlagStatus(spi, SPI_I2S_FLAG_RXNE));
        buffer[iread++] = SPI_I2S_ReceiveData(spi);
    }
 }


// Set a value to control the data rate of the given SPI interface
void SPI_SetDataRate(SPI_TypeDef *spi, uint16_t value)
{
  uint16_t tmpreg = 0;
  tmpreg = spi->CR1;
  tmpreg &= ~(7 << 3);  // clear NUCLEO Baud Rate Prescaler bits 5:3
  tmpreg |= value; // set NUCLEO Baud Rate Prescaler bits 5:3 to the given value
  
  spi->CR1 = tmpreg;  // write back the register
}

