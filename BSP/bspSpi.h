/*
    bspSpi.h

    Board support for controlling SPI interfaces on NUCLEO-F401RE MCU

    Developed for University of Washington embedded systems programming certificate
    
    2016/2 Nick Strathy wrote/arranged it
*/
 
#ifndef __BSPSPI_H
#define __BSPSPI_H

#include "stm32f4xx_spi.h"

#define PJDF_SPI1 SPI1 // Address of SPI1 memory mapped register block

// Application interface to hardware

void BspSPI1Init();
void SPI_SendBuffer(SPI_TypeDef *spi, uint8_t *buffer, uint16_t bufLength);
void SPI_GetBuffer(SPI_TypeDef *spi, uint8_t *buffer, uint16_t bufLength);
void SPI_SetDataRate(SPI_TypeDef *spi, uint16_t value);

#endif /* __SPI_H */