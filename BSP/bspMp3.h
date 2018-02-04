/*
    bspMp3.h

    Board support for controlling Adafruit Music Maker VS1053 MP3 decoder shield via NUCLEO-F401RE MCU

    Developed for University of Washington embedded systems programming certificate
    
    2016/2 Nick Strathy wrote/arranged it
*/

#include "stm32f4xx.h"

#ifndef __BSPMP3_H
#define __BSPMP3_H

#define MP3_VS1053_MCS_GPIO               GPIOA
#define MP3_VS1053_MCS_GPIO_Pin           GPIO_Pin_8

#define MP3_VS1053_DCS_GPIO               GPIOB
#define MP3_VS1053_DCS_GPIO_Pin           GPIO_Pin_10

#define MP3_VS1053_DREQ_GPIO               GPIOB
#define MP3_VS1053_DREQ_GPIO_Pin           GPIO_Pin_3

#define MP3_VS1053_MCS_ASSERT()       GPIO_ResetBits(MP3_VS1053_MCS_GPIO, MP3_VS1053_MCS_GPIO_Pin);
#define MP3_VS1053_MCS_DEASSERT()      GPIO_SetBits(MP3_VS1053_MCS_GPIO, MP3_VS1053_MCS_GPIO_Pin);

#define MP3_VS1053_DCS_ASSERT()       GPIO_ResetBits(MP3_VS1053_DCS_GPIO, MP3_VS1053_DCS_GPIO_Pin);
#define MP3_VS1053_DCS_DEASSERT()      GPIO_SetBits(MP3_VS1053_DCS_GPIO, MP3_VS1053_DCS_GPIO_Pin);


#define MP3_DECODER_BUF_SIZE       32    // number of bytes to stream at one time to the decoder

#define MP3_SPI_DEVICE_ID  PJDF_DEVICE_ID_SPI1

#define MP3_SPI_DATARATE  SPI_BaudRatePrescaler_16  // Tune to find optimal value MP3 decoder will work with

// some command strings to send to the VS1053 MP3 decoder:
extern const INT8U BspMp3SineWave[];
extern const INT8U BspMp3Deact[];
extern const INT8U BspMp3TestMode[];
extern const INT8U BspMp3PlayMode[];
extern const INT8U BspMp3SoftReset[];
extern const INT8U BspMp3SetClockF[];
extern const INT8U BspMp3SetVol1010[];
extern const INT8U BspMp3SetVol6060[];

extern const INT8U BspMp3SetVol0000[];
extern const INT8U BspMp3SetVol4040[];
extern const INT8U BspMp3SetVolFFFF[];


extern const INT8U BspMp3ReadVol[];

// Lengths of the above commands
extern const INT8U BspMp3SineWaveLen;
extern const INT8U BspMp3DeactLen;
extern const INT8U BspMp3TestModeLen;
extern const INT8U BspMp3PlayModeLen;
extern const INT8U BspMp3SoftResetLen;
extern const INT8U BspMp3SetClockFLen;
extern const INT8U BspMp3SetVol1010Len;
extern const INT8U BspMp3SetVol6060Len;

extern const INT8U BspMp3SetVol0000Len;
extern const INT8U BspMp3SetVol4040Len;
extern const INT8U BspMp3SetVolFFFFLen;



extern const INT8U BspMp3ReadVolLen;


void BspMp3InitVS1053();

#endif