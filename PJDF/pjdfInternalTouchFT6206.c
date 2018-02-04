/*
    PJDFINTERNALTOUCHFT6206.c
    The implementation of the internal PJDF interface pjdfInternal.h targeted for the
    FT6206 Touch Controller.

       
    2017/02/25 Rahul Gautam wrote/arranged it after a framework by Paul Lever
*/

#include "bsp.h"
#include "pjdf.h"
#include "pjdfInternal.h"
#include "PJDFINTERNALTOUCHFT6206.h"

// SPI link, etc for VS1053 MP3 decoder hardware
typedef struct _PjdfContextTouchFT6206
{
    DataHandle i2cHandle; // I2C communication link 
    INT8U commandSelect; // 0
} PjdfContextTouchFT6206;

static PjdfContextTouchFT6206 touchFT6206Context = { 0 };



// OpenTouch
// Nothing to do.
static PjdfErrCode OpenTouch(DriverInternal *pDriver, INT8U flags)
{
    return PJDF_ERR_NONE; 
}

// CloseTouch
// Ensure that the dependent SPI handle is closed.
static PjdfErrCode CloseTouch(DriverInternal *pDriver)
{
    PjdfContextTouchFT6206 *pContext = (PjdfContextTouchFT6206*) pDriver->deviceContext;
    Close(pContext->i2cHandle);
    return PJDF_ERR_NONE;
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
static PjdfErrCode ReadTouch(DriverInternal *pDriver, void* pBuffer, INT32U* pCount)
{
    PjdfErrCode retval;
    PjdfContextTouchFT6206 *pContext = (PjdfContextTouchFT6206*) pDriver->deviceContext;

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
static PjdfErrCode WriteTouch(DriverInternal *pDriver, void* pBuffer, INT32U* pCount)
{
    PjdfErrCode retval;

    return retval;
}

// IoctlMP3
// pDriver: pointer to an initialized VS1053 MP3 driver
// request: a request code chosen from those in pjdfCtrlMp3VS1053.h
// pArgs [in/out]: pointer to any data needed to fulfill the request
// pSize: the number of bytes pointed to by pArgs
// Returns: PJDF_ERR_NONE if there was no error, otherwise an error code.
static PjdfErrCode IoctlTouch(DriverInternal *pDriver, INT8U request, void* pArgs, INT32U* pSize)
{
    HANDLE handle;
    PjdfErrCode retval = PJDF_ERR_NONE;
    PjdfContextTouchFT6206 *pContext = (PjdfContextTouchFT6206*) pDriver->deviceContext;
    switch (request)
    {
    case PJDF_CTRL_TOUCH_I2C_START_TRANSMIT:
        I2C_start(I2C1, *(INT16U*)pArgs, I2C_Direction_Transmitter);
        break;
    case PJDF_CTRL_TOUCH_I2C_START_RECEIVE:
        I2C_start(I2C1, *(INT16U*)pArgs, I2C_Direction_Receiver );
        break;    
    case PJDF_CTRL_TOUCH_I2C_WRITE:
        I2C_write(I2C1, *(uint8_t*)pArgs);
        break;
    case PJDF_CTRL_TOUCH_I2C_READ_NACK:
        *(uint8_t*)pArgs = I2C_read_nack(I2C1);
        break;
    case PJDF_CTRL_TOUCH_I2C_READ_ACK:
         *(uint8_t*)pArgs = I2C_read_ack(I2C1);
        break;    
    case PJDF_CTRL_TOUCH_I2C_STOP:
        I2C_stop(I2C1);
        break;
    default:
        retval = PJDF_ERR_UNKNOWN_CTRL_REQUEST;
        break;
    }
    return retval;
}


// Initializes the given VS1053 MP3 driver.
PjdfErrCode InitTouchFT6206(DriverInternal *pDriver, char *pName)
{
    if (strcmp (pName, pDriver->pName) != 0) while(1); // pName should have been initialized in driversInternal[] declaration
    
    // Initialize semaphore for serializing operations on the device 
    pDriver->sem = OSSemCreate(1); 
    if (pDriver->sem == NULL) while (1);  // not enough semaphores available
    pDriver->refCount = 0; // number of Open handles to the device
    pDriver->maxRefCount = 1; // only one open handle allowed
    pDriver->deviceContext = &touchFT6206Context;
    
    BspMp3InitVS1053(); // Initialize related GPIO
  
    // Assign implemented functions to the interface pointers
    pDriver->Open = OpenTouch;
    pDriver->Close = CloseTouch;
    pDriver->Read = ReadTouch;
    pDriver->Write = WriteTouch;
    pDriver->Ioctl = IoctlTouch;
    
    pDriver->initialized = OS_TRUE;
    return PJDF_ERR_NONE;
}


