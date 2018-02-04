/*
    mp3Util.c
    Some utility functions for controlling the MP3 decoder.

    Developed for University of Washington embedded systems programming certificate
    
    2016/2 Nick Strathy wrote/arranged it
*/

#include "bsp.h"
#include "print.h"
#include "SD.h"


void delay(uint32_t time);


static File dataFile;

extern BOOLEAN nextSong;
extern BOOLEAN playState;


static void Mp3StreamInit(HANDLE hMp3)
{
    INT32U length;
    
    // Place MP3 driver in command mode (subsequent writes will be sent to the decoder's command interface)
    Ioctl(hMp3, PJDF_CTRL_MP3_SELECT_COMMAND, 0, 0);
    
    // Reset the device
    length = BspMp3SoftResetLen;
    Write(hMp3, (void*)BspMp3SoftReset, &length);
 
    // Set volume
    length = BspMp3SetVol6060Len;
    Write(hMp3, (void*)BspMp3SetVol1010, &length);

    // To allow streaming data, set the decoder mode to Play Mode
    length = BspMp3PlayModeLen;
    Write(hMp3, (void*)BspMp3PlayMode, &length);
   
    // Set MP3 driver to data mode (subsequent writes will be sent to decoder's data interface)
    Ioctl(hMp3, PJDF_CTRL_MP3_SELECT_DATA, 0, 0);
}

// Mp3StreamSDFile
// Streams the given file from the SD card to the given MP3 decoder.
// hMP3: an open handle to the MP3 decoder
// pFilename: The file on the SD card to stream. 
void Mp3StreamSDFile(HANDLE hMp3, char *pFilename)
{
    INT32U length;

    Mp3StreamInit(hMp3);
    
    char printBuf[PRINTBUFMAX];
    
    dataFile = SD.open(pFilename, O_READ);
    if (!dataFile) 
    {
        PrintWithBuf(printBuf, PRINTBUFMAX, "Error: could not open SD card file '%s'\n", pFilename);
        return;
    }

    INT8U mp3Buf[MP3_DECODER_BUF_SIZE];
    INT32U iBufPos = 0;
    nextSong = OS_FALSE;
    while (dataFile.available())
    {
        iBufPos = 0;
        while (dataFile.available() && iBufPos < MP3_DECODER_BUF_SIZE)
        {
            mp3Buf[iBufPos] = dataFile.read();
            //delay(10);
            iBufPos++;
        }
       
        Write(hMp3, mp3Buf, &iBufPos);
        //OSTimeDly(1);
        if (nextSong)
        {
          //TODO: GOTO next song
            break;
        }
        if(!playState)
        {
          break;
        }
    }
    
    dataFile.close();
    
    Ioctl(hMp3, PJDF_CTRL_MP3_SELECT_COMMAND, 0, 0);
    length = BspMp3SoftResetLen;
    Write(hMp3, (void*)BspMp3SoftReset, &length);
}

// Mp3Stream
// Streams the given buffer of MP3 data to the given MP3 decoder
// hMp3: an open handle to the MP3 decoder
// pBuf: MP3 data to stream to the decoder
// bufLen: number of bytes of MP3 data to stream
void Mp3Stream(HANDLE hMp3, INT8U *pBuf, INT32U bufLen)
{
    INT8U *bufPos = pBuf;
    INT32U iBufPos = 0;
    INT32U length;
    INT32U chunkLen;
    BOOLEAN done = OS_FALSE;
        
    Mp3StreamInit(hMp3);
    
    chunkLen = MP3_DECODER_BUF_SIZE;

    while (!done)
    {
        // detect last chunk of pBuf
        if (bufLen - iBufPos < MP3_DECODER_BUF_SIZE)
        {
            chunkLen = bufLen - iBufPos;
            done = OS_TRUE;
        }
                
        Write(hMp3, bufPos, &chunkLen);
                
        bufPos += chunkLen;
        iBufPos += chunkLen;
    }
    
    Ioctl(hMp3, PJDF_CTRL_MP3_SELECT_COMMAND, 0, 0);
    length = BspMp3SoftResetLen;
    Write(hMp3, (void*)BspMp3SoftReset, &length);
}


// Mp3Init
// Send commands to the MP3 device to initialize it.
void Mp3Init(HANDLE hMp3)
{
    INT32U length;
    
    if (!PJDF_IS_VALID_HANDLE(hMp3)) while (1);
    
    // Place MP3 driver in command mode (subsequent writes will be sent to the decoder's command interface)
    Ioctl(hMp3, PJDF_CTRL_MP3_SELECT_COMMAND, 0, 0);
    
    length = BspMp3SetClockFLen;
    Write(hMp3, (void*)BspMp3SetClockF, &length);

    length = BspMp3SetVol1010Len;//BspMp3SetVol1010Len;
   // Write(hMp3, (void*)BspMp3SetVolC0C0, &length);
    Write(hMp3, (void*)BspMp3SetVol1010, &length);

    length = BspMp3SoftResetLen;
    Write(hMp3, (void*)BspMp3SoftReset, &length);
}

// Mp3GetRegister
// Gets a register value from the MP3 decoder.
// hMp3: an open handle to the MP3 decoder
// cmdInDataOut: on entry this buffer must contain the 
//     command to get the desired register from the decoder. On exit
//     this buffer will be OVERWRITTEN by the data output by the decoder in 
//     response to the command. Note: the buffer must not reside in readonly memory.
// bufLen: the number of bytes in cmdInDataOut.
// Returns: PJDF_ERR_NONE if no error otherwise an error code.
PjdfErrCode Mp3GetRegister(HANDLE hMp3, INT8U *cmdInDataOut, INT32U bufLen)
{
    PjdfErrCode retval;
    if (!PJDF_IS_VALID_HANDLE(hMp3)) while (1);
    
    // Place MP3 driver in command mode (subsequent writes will be sent to the decoder's command interface)
    Ioctl(hMp3, PJDF_CTRL_MP3_SELECT_COMMAND, 0, 0);
    
    retval = Read(hMp3, cmdInDataOut, &bufLen);
    return retval;
}


// Mp3Test
// Runs sine wave sound test on the MP3 decoder.
// For VS1053, the sine wave test only works if run immediately after a hard 
// reset of the chip.
void Mp3Test(HANDLE hMp3)
{
    INT32U length;
    
    if (!PJDF_IS_VALID_HANDLE(hMp3)) while (1);
    
    // Place MP3 driver in command mode (subsequent writes will be sent to the decoder's command interface)
    Ioctl(hMp3, PJDF_CTRL_MP3_SELECT_COMMAND, 0, 0);
    
    // Make sure we can communicate with the device by sending a command to read a register value
 
    // First set volume to a known value
    length = BspMp3SetVol1010Len;
    Write(hMp3, (void*)BspMp3SetVol1010, &length);
    
    // Now get the volume setting on the device
    INT8U buf[10];
    memcpy(buf, BspMp3ReadVol, BspMp3ReadVolLen); // copy command from flash to a ram buffer
    Mp3GetRegister(hMp3, buf, BspMp3ReadVolLen);
    if (buf[2] != 0x10 || buf[3] != 0x10)
    {
        while(1); // failed to get data back from the device
    }
        
    for (int i = 0; i < 10; i++)
    {
        // set louder volume if i is odd
        if (i & 1)
        {
            length = BspMp3SetVol1010Len;
            Write(hMp3, (void*)BspMp3SetVol1010, &length);
        }
        else
        {
            length = BspMp3SetVol6060Len;
            Write(hMp3, (void*)BspMp3SetVol6060, &length);
        }
            
        // Put MP3 decoder in test mode
        length = BspMp3TestModeLen;
        Write(hMp3, (void*)BspMp3TestMode, &length);

        // Play a sine wave
        length = BspMp3SineWaveLen;
        Write(hMp3, (void*)BspMp3SineWave, &length);
        Write(hMp3, (void*)BspMp3SineWave, &length); // Sending once sometimes doesn't work!
            
        OSTimeDly(500);
        
        // Stop playing the sine wave
        length = BspMp3DeactLen;
        Write(hMp3, (void*)BspMp3Deact, &length);
        
        OSTimeDly(500);
    }
}


