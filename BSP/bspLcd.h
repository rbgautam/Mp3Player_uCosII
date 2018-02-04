/*
    bspLcd.h

    Board support for controlling the ILI9341 LCD on the Adafruit '2.8" TFT LCD w/Cap Touch' via NUCLEO-F401RE MCU

    Developed for University of Washington embedded systems programming certificate
    
    2016/2 Nick Strathy wrote/arranged it
*/

#include "stm32f4xx.h"

#ifndef __BSPLCD_H
#define __BSPLCD_H

#define LCD_ILI9341_CS_GPIO               GPIOB
#define LCD_ILI9341_CS_GPIO_Pin           GPIO_Pin_6

#define LCD_ILI9341_DC_GPIO               GPIOC
#define LCD_ILI9341_DC_GPIO_Pin           GPIO_Pin_7


#define LCD_ILI9341_CS_ASSERT()        GPIO_ResetBits(LCD_ILI9341_CS_GPIO, LCD_ILI9341_CS_GPIO_Pin);
#define LCD_ILI9341_CS_DEASSERT()      GPIO_SetBits(LCD_ILI9341_CS_GPIO, LCD_ILI9341_CS_GPIO_Pin);

#define LCD_ILI9341_DC_LOW()        GPIO_ResetBits(LCD_ILI9341_DC_GPIO, LCD_ILI9341_DC_GPIO_Pin);
#define LCD_ILI9341_DC_HIGH()       GPIO_SetBits(LCD_ILI9341_DC_GPIO, LCD_ILI9341_DC_GPIO_Pin);

#define LCD_SPI_DEVICE_ID  PJDF_DEVICE_ID_SPI1

#define LCD_SPI_DATARATE  SPI_BaudRatePrescaler_2  // Tune to find optimal value LCD controller will work with

void BspLcdInitILI9341();

#endif