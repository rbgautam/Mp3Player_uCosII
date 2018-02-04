/*
    bspMp3.c

    Board support for controlling Adafruit Music Maker VS1053 MP3 decoder shield via NUCLEO-F401RE MCU

    Developed for University of Washington embedded systems programming certificate
    
    2016/2 Nick Strathy wrote/arranged it
*/

#include "bsp.h"

// some command strings to send to the VS1053 MP3 decoder:
const INT8U BspMp3SineWave[] =  { 0x53, 0xEF, 0x6E, 0x44, 0, 0, 0, 0 };
const INT8U BspMp3Deact[] =     { 0x45, 0x78, 0x69, 0x74, 0, 0, 0, 0 };
const INT8U BspMp3TestMode[] =  { 0x02, 0x00, 0x08, 0x20 };
const INT8U BspMp3PlayMode[] =  { 0x02, 0x00, 0x08, 0x00 };
const INT8U BspMp3SoftReset[] = { 0x02, 0x00, 0x08, 0x04 };
const INT8U BspMp3SetClockF[] = { 0x02, 0x03, 0x98, 0x00 };
const INT8U BspMp3SetVol1010[] = { 0x02, 0x0B, 0x10, 0x10 }; //Volume = 93.75%
const INT8U BspMp3SetVol6060[] = { 0x02, 0x0B, 0x60, 0x60 }; // Volume = 62.35%
//More Volume commands
const INT8U BspMp3SetVol4040[] = { 0x02, 0x0B, 0x40, 0x40 }; // Volume = 75%
const INT8U BspMp3SetVol0000[] = { 0x02, 0x0B, 0x00, 0x00 }; // Volume = 100%
const INT8U BspMp3SetVolFFFF[] = { 0x02, 0x0B, 0xFF, 0xFF }; // Volume =0


const INT8U BspMp3ReadVol[] = { 0x3, 0x0B, 0x00, 0x00 };

// Lengths of the above commands
const INT8U BspMp3SineWaveLen = sizeof(BspMp3SineWave);
const INT8U BspMp3DeactLen = sizeof(BspMp3Deact);
const INT8U BspMp3TestModeLen = sizeof(BspMp3TestMode);
const INT8U BspMp3PlayModeLen = sizeof(BspMp3PlayMode);
const INT8U BspMp3SoftResetLen = sizeof(BspMp3SoftReset);
const INT8U BspMp3SetClockFLen = sizeof(BspMp3SetClockF);
const INT8U BspMp3SetVol1010Len = sizeof(BspMp3SetVol1010);
const INT8U BspMp3SetVol6060Len = sizeof(BspMp3SetVol6060);

//more volume options
const INT8U BspMp3SetVol4040Len = sizeof(BspMp3SetVol4040);
const INT8U BspMp3SetVol0000Len = sizeof(BspMp3SetVol0000);
const INT8U BspMp3SetVolFFFFLen = sizeof(BspMp3SetVolFFFF);



const INT8U BspMp3ReadVolLen = sizeof(BspMp3ReadVol);




// Initializes GPIO pins for the VS1053 MP3 device.
void BspMp3InitVS1053()
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB , ENABLE);

    GPIO_InitTypeDef GPIO_InitStruct;
    
    /*-------- Configure MCS ChipSelect Pin, asserts low, selects command interface PA8--------*/ 
 
    GPIO_InitStruct.GPIO_Pin = MP3_VS1053_MCS_GPIO_Pin;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
     
    GPIO_Init(MP3_VS1053_MCS_GPIO, &GPIO_InitStruct);
    MP3_VS1053_MCS_DEASSERT();
 
    /*-------- Configure DCS ChipSelect Pin, asserts low, selects data interface PB10--------*/ 
 
    GPIO_InitStruct.GPIO_Pin = MP3_VS1053_DCS_GPIO_Pin;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
     
    GPIO_Init(MP3_VS1053_DCS_GPIO, &GPIO_InitStruct);
    MP3_VS1053_DCS_DEASSERT();
   
    /*-------- Configure DREQ data request interrupt, asserts high when ready for data PB3--------*/ 
 
    GPIO_InitStruct.GPIO_Pin = MP3_VS1053_DREQ_GPIO_Pin;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
     
    GPIO_Init(MP3_VS1053_DREQ_GPIO, &GPIO_InitStruct);
}