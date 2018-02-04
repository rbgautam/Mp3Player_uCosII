/*
    bspSD.c

    Board support for controlling the SD on the Adafruit shields via NUCLEO-F401RE MCU

    Developed for University of Washington embedded systems programming certificate
    
    2016/2 Nick Strathy wrote/arranged it
*/

#include "bsp.h"



// Initializes GPIO pins for the SD device.
void BspSDInitAdafruit()
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

    GPIO_InitTypeDef GPIO_InitStruct;
    
    /*-------- Configure CS ChipSelect Pin PB5 --------*/ 
 
    GPIO_InitStruct.GPIO_Pin = SD_ADAFRUIT_CS_GPIO_Pin;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
     
    GPIO_Init(SD_ADAFRUIT_CS_GPIO, &GPIO_InitStruct);
    SD_ADAFRUIT_CS_DEASSERT();
}