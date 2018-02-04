/*
    pjdfInternalLcdILI9341.c
    The implementation of the internal PJDF interface pjdfInternal.h targeted for the
    ILI9341 LCD controller.

    Developed for University of Washington embedded systems programming certificate
    
    2016/2 Nick Strathy wrote/arranged it after a framework by Paul Lever
*/

#include "bsp.h"
#include "pjdf.h"
#include "pjdfInternal.h"


// SPI link, etc for ILI8341 LCD controller
typedef struct _PjdfContextLcdILI9341
{
    HANDLE spiHandle; // SPI communication link to ILI9341
} PjdfContextLcdILI9341;

static PjdfContextLcdILI9341 ili9341Context = { 0 };

static const INT16U LcdSpiDataRate = LCD_SPI_DATARATE;
static const INT32U SizeofLcdSpiDataRate = sizeof(LcdSpiDataRate);


// OpenLCD
// Nothing to do.
static PjdfErrCode OpenLCD(DriverInternal *pDriver, INT8U flags)
{
    return PJDF_ERR_NONE; 
}

// CloseLCD
// Ensure that the dependent SPI handle is closed.
static PjdfErrCode CloseLCD(DriverInternal *pDriver)
{
    PjdfContextLcdILI9341 *pContext = (PjdfContextLcdILI9341*) pDriver->deviceContext;
    return Close(pContext->spiHandle);
}

// ReadLCD
// Writes the contents of the buffer to the given device, and concurrently
// gets the resulting data back from the device via full duplex SPI. 
// Before writing, select the ILI9341 command interface by passing  
// he following request to Ioctl():
//     PJDF_CTRL_LCD_SELECT_COMMAND
//
// The above selection will persist until changed by another call to Ioctl()
//
// pDriver: pointer to an initialized ILI9341 LCD driver
// pBuffer: on entry the data to write to the device, on exit, OVERWRITTEN by
//     the resulting data from the device
// pCount: the number of bytes to write/read
// Returns: PJDF_ERR_NONE if there was no error, otherwise an error code.
static PjdfErrCode ReadLCD(DriverInternal *pDriver, void* pBuffer, INT32U* pCount)
{
    PjdfErrCode retval;
    PjdfContextLcdILI9341 *pContext = (PjdfContextLcdILI9341*) pDriver->deviceContext;
    HANDLE hSPI = pContext->spiHandle;
    
    retval = Ioctl(hSPI, PJDF_CTRL_SPI_WAIT_FOR_LOCK, 0, 0); // wait for exclusive access
    if (retval != PJDF_ERR_NONE) while(1);

    // adjust SPI transmission rate
    retval = Ioctl(hSPI, PJDF_CTRL_SPI_SET_DATARATE, (void*)&LcdSpiDataRate, (INT32U*)&SizeofLcdSpiDataRate); 
    if (retval != PJDF_ERR_NONE) while(1);

    LCD_ILI9341_CS_ASSERT(); // assert LCD SPI
    retval = Read(hSPI, pBuffer, pCount);
    LCD_ILI9341_CS_DEASSERT(); // de-assert LCD SPI
    
    retval = Ioctl(hSPI, PJDF_CTRL_SPI_RELEASE_LOCK, 0, 0);
    if (retval != PJDF_ERR_NONE) while(1);
    return retval;
}


// WriteLCD
// Writes the contents of the buffer to the given device.
// Before writing, select the ILI9341 command or data interface by passing one 
// of the following requests to Ioctl():
//     PJDF_CTRL_LCD_SELECT_COMMAND
//     PJDF_CTRL_LCD_SELECT_DATA
//
// The above selection will persist until changed by another call to Ioctl()
//
// pDriver: pointer to an initialized ILI9341 LCD driver
// pBuffer: the data to write to the device
// pCount: the number of bytes to write
// Returns: PJDF_ERR_NONE if there was no error, otherwise an error code.
static PjdfErrCode WriteLCD(DriverInternal *pDriver, void* pBuffer, INT32U* pCount)
{
    PjdfErrCode retval;
    PjdfContextLcdILI9341 *pContext = (PjdfContextLcdILI9341*) pDriver->deviceContext;
    HANDLE hSPI = pContext->spiHandle;
    
    retval = Ioctl(hSPI, PJDF_CTRL_SPI_WAIT_FOR_LOCK, 0, 0);  // wait for exclusive access
    if (retval != PJDF_ERR_NONE) while(1);
    
    // adjust SPI transmission rate
    retval = Ioctl(hSPI, PJDF_CTRL_SPI_SET_DATARATE, (void*)&LcdSpiDataRate, (INT32U*)&SizeofLcdSpiDataRate); 
    if (retval != PJDF_ERR_NONE) while(1);

    LCD_ILI9341_CS_ASSERT(); // assert LCD SPI
    retval = Write(hSPI, pBuffer, pCount);
    LCD_ILI9341_CS_DEASSERT(); // de-assert LCD SPI
    
    retval = Ioctl(hSPI, PJDF_CTRL_SPI_RELEASE_LOCK, 0, 0);
    if (retval != PJDF_ERR_NONE) while(1);
    return retval;
}

// IoctlLCD
// pDriver: pointer to an initialized ILI9341 LCD driver
// request: a request code chosen from those in pjdfCtrlLcdILI9341.h
// pArgs [in/out]: pointer to any data needed to fulfill the request
// pSize: the number of bytes pointed to by pArgs
// Returns: PJDF_ERR_NONE if there was no error, otherwise an error code.
static PjdfErrCode IoctlLCD(DriverInternal *pDriver, INT8U request, void* pArgs, INT32U* pSize)
{
    HANDLE handle;
    PjdfErrCode retval = PJDF_ERR_NONE;
    PjdfContextLcdILI9341 *pContext = (PjdfContextLcdILI9341*) pDriver->deviceContext;
    switch (request)
    {
    case PJDF_CTRL_LCD_SELECT_COMMAND:
        LCD_ILI9341_DC_LOW();
        break;
    case PJDF_CTRL_LCD_SELECT_DATA:
        LCD_ILI9341_DC_HIGH();
        break;
    case PJDF_CTRL_LCD_SET_SPI_HANDLE:
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


// Initializes the given ILI9341 LCD driver.
PjdfErrCode InitLcdILI9341(DriverInternal *pDriver, char *pName)
{
    if (strcmp (pName, pDriver->pName) != 0) while(1); // pName should have been initialized in driversInternal[] declaration
    
    // Initialize semaphore for serializing operations on the device 
    pDriver->sem = OSSemCreate(1); 
    if (pDriver->sem == NULL) while (1);  // not enough semaphores available
    pDriver->refCount = 0; // number of Open handles to the device
    pDriver->maxRefCount = 1; // only one open handle allowed
    pDriver->deviceContext = &ili9341Context;
    
    BspLcdInitILI9341(); // Initialize related GPIO
  
    // Assign implemented functions to the interface pointers
    pDriver->Open = OpenLCD;
    pDriver->Close = CloseLCD;
    pDriver->Read = ReadLCD;
    pDriver->Write = WriteLCD;
    pDriver->Ioctl = IoctlLCD;
    
    pDriver->initialized = OS_TRUE;
    return PJDF_ERR_NONE;
}


