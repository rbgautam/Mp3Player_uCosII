/*
    pjdfInternalMp3Vs1053.c
    The implementation of the internal PJDF interface pjdfInternal.h targeted for the
    VS1053 MP3 decoder.

    Developed for University of Washington embedded systems programming certificate
    
    2016/2 Nick Strathy wrote/arranged it after a framework by Paul Lever
*/

#include "bsp.h"
#include "pjdf.h"
#include "pjdfInternal.h"


// SPI link, etc for VS1053 MP3 decoder hardware
typedef struct _PjdfContextMp3VS1053
{
    HANDLE spiHandle; // SPI communication link to VS1053
    INT8U chipSelect; // 0 means command, 1 means data
} PjdfContextMp3VS1053;

static PjdfContextMp3VS1053 mp3VS1053Context = { 0 };

static const INT16U Mp3SpiDataRate = MP3_SPI_DATARATE;
static const INT32U SizeofMp3SpiDataRate = sizeof(Mp3SpiDataRate);

// OpenMP3
// Nothing to do.
static PjdfErrCode OpenMP3(DriverInternal *pDriver, INT8U flags)
{
    return PJDF_ERR_NONE; 
}

// CloseMP3
// Ensure that the dependent SPI handle is closed.
static PjdfErrCode CloseMP3(DriverInternal *pDriver)
{
    PjdfContextMp3VS1053 *pContext = (PjdfContextMp3VS1053*) pDriver->deviceContext;
    return Close(pContext->spiHandle);
}

// ReadMP3
// Writes the contents of the buffer to the given device, and concurrently
// gets the resulting data back from the device via full duplex SPI. 
// Before writing, select the VS1053 command interface by passing  
// he following request to Ioctl():
//     PJDF_CTRL_MP3_SELECT_COMMAND
//
// The above selection will persist until changed by another call to Ioctl()
//
// pDriver: pointer to an initialized VS1053 MP3 driver
// pBuffer: on entry the data to write to the device, on exit, OVERWRITTEN by
//     the resulting data from the device
// pCount: the number of bytes to write/read
// Returns: PJDF_ERR_NONE if there was no error, otherwise an error code.
static PjdfErrCode ReadMP3(DriverInternal *pDriver, void* pBuffer, INT32U* pCount)
{
    PjdfErrCode retval;
    PjdfContextMp3VS1053 *pContext = (PjdfContextMp3VS1053*) pDriver->deviceContext;
    HANDLE hSPI = pContext->spiHandle;
    
    retval = Ioctl(hSPI, PJDF_CTRL_SPI_WAIT_FOR_LOCK, 0, 0);   // wait for exclusive access
    if (retval != PJDF_ERR_NONE) while(1);
    
    // adjust SPI transmission rate
    retval = Ioctl(hSPI, PJDF_CTRL_SPI_SET_DATARATE, (void*)&Mp3SpiDataRate, (INT32U*)&SizeofMp3SpiDataRate); 
    if (retval != PJDF_ERR_NONE) while(1);

    // Wait for device ready
    while (!GPIO_ReadInputDataBit(MP3_VS1053_DREQ_GPIO, MP3_VS1053_DREQ_GPIO_Pin));
    
    switch (pContext->chipSelect) {
    case 0: /* send command */
        MP3_VS1053_MCS_ASSERT(); // assert command chip-select
        retval = Read(hSPI, pBuffer, pCount);
        MP3_VS1053_MCS_DEASSERT(); // de-assert command chip-select
        break;
    default:
        while(1); // must be in command mode
    }
    retval = Ioctl(hSPI, PJDF_CTRL_SPI_RELEASE_LOCK, 0, 0);
    if (retval != PJDF_ERR_NONE) while(1);
    return retval;
}


// WriteMP3
// Writes the contents of the buffer to the given device.
// Before writing, select the VS1053 command or data interface by passing one 
// of the following requests to Ioctl():
//     PJDF_CTRL_MP3_SELECT_COMMAND
//     PJDF_CTRL_MP3_SELECT_DATA
//
// The above selection will persist until changed by another call to Ioctl()
//
// pDriver: pointer to an initialized VS1053 MP3 driver
// pBuffer: the data to write to the device
// pCount: the number of bytes to write
// Returns: PJDF_ERR_NONE if there was no error, otherwise an error code.
static PjdfErrCode WriteMP3(DriverInternal *pDriver, void* pBuffer, INT32U* pCount)
{
    PjdfErrCode retval;
    PjdfContextMp3VS1053 *pContext = (PjdfContextMp3VS1053*) pDriver->deviceContext;
    HANDLE hSPI = pContext->spiHandle;
    
    retval = Ioctl(hSPI, PJDF_CTRL_SPI_WAIT_FOR_LOCK, 0, 0); // wait for exclusive access
    if (retval != PJDF_ERR_NONE) while(1);
    
    // Wait for device ready
    while (!GPIO_ReadInputDataBit(MP3_VS1053_DREQ_GPIO, MP3_VS1053_DREQ_GPIO_Pin))
    {
        // Device not ready so release it and delay before trying again
        retval = Ioctl(hSPI, PJDF_CTRL_SPI_RELEASE_LOCK, 0, 0);
        if (retval != PJDF_ERR_NONE) while(1);
        
        OSTimeDly(5); // can optimize this to handle highest audio bit rate while allowing delay for other tasks 
        
        retval = Ioctl(hSPI, PJDF_CTRL_SPI_WAIT_FOR_LOCK, 0, 0); // wait for exclusive access
        if (retval != PJDF_ERR_NONE) while(1);
    }
        
        
    
    // adjust SPI transmission rate
    retval = Ioctl(hSPI, PJDF_CTRL_SPI_SET_DATARATE, (void*)&Mp3SpiDataRate, (INT32U*)&SizeofMp3SpiDataRate); 
    if (retval != PJDF_ERR_NONE) while(1);

    
    switch (pContext->chipSelect) {
    case 0: /* send command */
        MP3_VS1053_MCS_ASSERT(); // assert command chip-select
        retval = Write(hSPI, pBuffer, pCount);
        MP3_VS1053_MCS_DEASSERT(); // de-assert command chip-select
        break;
    case 1:  /* send data */
        MP3_VS1053_DCS_ASSERT(); // assert data chip-select
        retval = Write(hSPI, pBuffer, pCount);
        MP3_VS1053_DCS_DEASSERT(); // de-assert data chip-select
        break;
    default:
        while(1);
    }
    retval = Ioctl(hSPI, PJDF_CTRL_SPI_RELEASE_LOCK, 0, 0);
    if (retval != PJDF_ERR_NONE) while(1);
    return retval;
}

// IoctlMP3
// pDriver: pointer to an initialized VS1053 MP3 driver
// request: a request code chosen from those in pjdfCtrlMp3VS1053.h
// pArgs [in/out]: pointer to any data needed to fulfill the request
// pSize: the number of bytes pointed to by pArgs
// Returns: PJDF_ERR_NONE if there was no error, otherwise an error code.
static PjdfErrCode IoctlMP3(DriverInternal *pDriver, INT8U request, void* pArgs, INT32U* pSize)
{
    HANDLE handle;
    PjdfErrCode retval = PJDF_ERR_NONE;
    PjdfContextMp3VS1053 *pContext = (PjdfContextMp3VS1053*) pDriver->deviceContext;
    switch (request)
    {
    case PJDF_CTRL_MP3_SELECT_COMMAND:
        pContext->chipSelect = 0;
        break;
    case PJDF_CTRL_MP3_SELECT_DATA:
        pContext->chipSelect = 1;
        break;
    case PJDF_CTRL_MP3_SET_SPI_HANDLE:
        if (*pSize < sizeof(HANDLE))
        {
            return PJDF_ERR_ARG;
        }
        handle = *((HANDLE*)pArgs);
        if (handle <= 0)
        {
            return PJDF_ERR_INVALID_HANDLE;
        }
        pContext->spiHandle = handle;
        break;
    default:
        retval = PJDF_ERR_UNKNOWN_CTRL_REQUEST;
        break;
    }
    return retval;
}


// Initializes the given VS1053 MP3 driver.
PjdfErrCode InitMp3VS1053(DriverInternal *pDriver, char *pName)
{
    if (strcmp (pName, pDriver->pName) != 0) while(1); // pName should have been initialized in driversInternal[] declaration
    
    // Initialize semaphore for serializing operations on the device 
    pDriver->sem = OSSemCreate(1); 
    if (pDriver->sem == NULL) while (1);  // not enough semaphores available
    pDriver->refCount = 0; // number of Open handles to the device
    pDriver->maxRefCount = 1; // only one open handle allowed
    pDriver->deviceContext = &mp3VS1053Context;
    
    BspMp3InitVS1053(); // Initialize related GPIO
  
    // Assign implemented functions to the interface pointers
    pDriver->Open = OpenMP3;
    pDriver->Close = CloseMP3;
    pDriver->Read = ReadMP3;
    pDriver->Write = WriteMP3;
    pDriver->Ioctl = IoctlMP3;
    
    pDriver->initialized = OS_TRUE;
    return PJDF_ERR_NONE;
}


