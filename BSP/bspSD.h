/*
    bspSD.h

    Board support for controlling the SD on the Adafruit shields via NUCLEO-F401RE MCU

    Developed for University of Washington embedded systems programming certificate
    
    2016/2 Nick Strathy wrote/arranged it
*/

#include "stm32f4xx.h"

#ifndef __BSPSD_H
#define __BSPSD_H

#define SD_ADAFRUIT_CS_GPIO               GPIOB
#define SD_ADAFRUIT_CS_GPIO_Pin           GPIO_Pin_5


#define SD_ADAFRUIT_CS_ASSERT()        GPIO_ResetBits(SD_ADAFRUIT_CS_GPIO, SD_ADAFRUIT_CS_GPIO_Pin);
#define SD_ADAFRUIT_CS_DEASSERT()      GPIO_SetBits(SD_ADAFRUIT_CS_GPIO, SD_ADAFRUIT_CS_GPIO_Pin);

#define SD_ADAFRUIT_DC_LOW()        GPIO_ResetBits(SD_ADAFRUIT_DC_GPIO, SD_ADAFRUIT_DC_GPIO_Pin);
#define SD_ADAFRUIT_DC_HIGH()       GPIO_SetBits(SD_ADAFRUIT_DC_GPIO, SD_ADAFRUIT_DC_GPIO_Pin);

#define SD_SPI_DEVICE_ID  PJDF_DEVICE_ID_SPI1

#define SD_SPI_DATARATE  SPI_BaudRatePrescaler_2  // Tune to find optimal value SD controller will work with

void BspSDInitAdafruit();

#endif