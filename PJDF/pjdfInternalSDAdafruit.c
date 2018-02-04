/*
    pjdfInternalSDAdafruit.c
    The implementation of the internal PJDF interface pjdfInternal.h targeted for the
    SD card on the Adafruit Music Maker shield.

    Developed for University of Washington embedded systems programming certificate
    
    2016/2 Nick Strathy wrote/arranged it after a framework by Paul Lever
*/

#include "bsp.h"
#include "pjdf.h"
#include "pjdfInternal.h"


// SPI link, etc for SD hardware
typedef struct _PjdfContextSD
{
    HANDLE spiHandle; // SPI communication link to SD card on Adafruit shield
    BOOLEAN spiLocked; // true iff we have exclusive access to the SPI
    BOOLEAN csAsserted; // true iff SPI chip select is asserted
} PjdfContextSD;

static PjdfContextSD SDContext = { 0 };

static const INT16U SDSpiDataRate = SD_SPI_DATARATE;
static const INT32U SizeofSDSpiDataRate = sizeof(SDSpiDataRate);

// OpenSDAdafruit
// Nothing to do.
static PjdfErrCode OpenSDAdafruit(DriverInternal *pDriver, INT8U flags)
{
    return PJDF_ERR_NONE; 
}

// CloseSDAdafruit
// Ensure that the dependent SPI handle is closed.
static PjdfErrCode CloseSDAdafruit(DriverInternal *pDriver)
{
    PjdfContextSD *pContext = (PjdfContextSD*) pDriver->deviceContext;
    return Close(pContext->spiHandle);
}

// ReadSDAdafruit
// Writes the contents of the buffer to the given device, and concurrently
// gets the resulting data back from the device via full duplex SPI.
//
// Before reading, you need to first use Ioctl:
//      Get exclusive acces to the SD's SPI with PJDF_CTRL_SD_LOCK_SPI
//      Assert the Sd's SPI chip select with PJDF_CTRL_SD_ASSERT_CS
//      After reading:
//           Deassert the SD's SPI with PJDF_CTRL_SD_DEASSERT_CS
//           Release the SD's SPI lock with PJDF_CTRL_SD_RELEASE_SPI
//
// pDriver: pointer to an initialized SD driver
// pBuffer: on entry the data to write to the device, on exit, OVERWRITTEN by
//     the resulting data from the device
// pCount: the number of bytes to write/read
// Returns: PJDF_ERR_NONE if there was no error, otherwise an error code.
static PjdfErrCode ReadSDAdafruit(DriverInternal *pDriver, void* pBuffer, INT32U* pCount)
{
    PjdfErrCode retval;
    PjdfContextSD *pContext = (PjdfContextSD*) pDriver->deviceContext;
    HANDLE hSPI = pContext->spiHandle;
    
    if (!pContext->spiLocked) while(1);
    if (!pContext->csAsserted) while(1);
    
    // adjust SPI transmission rate
    retval = Ioctl(hSPI, PJDF_CTRL_SPI_SET_DATARATE, (void*)&SDSpiDataRate, (INT32U*)&SizeofSDSpiDataRate); 
    if (retval != PJDF_ERR_NONE) while(1);
    
    retval = Read(hSPI, pBuffer, pCount);
    
    return retval;
}


// WriteSDAdafruit
// Writes the contents of the buffer to the given device.
// Before writing, you need to first use Ioctl:
//      Get exclusive acces to the SD's SPI with PJDF_CTRL_SD_LOCK_SPI
//      Assert the Sd's SPI chip select with PJDF_CTRL_SD_ASSERT_CS
//      After writing:
//           Deassert the SD's SPI with PJDF_CTRL_SD_DEASSERT_CS
//           Release the SD's SPI lock with PJDF_CTRL_SD_RELEASE_SPI
//
// pDriver: pointer to an initialized SD driver
// pBuffer: the data to write to the device
// pCount: the number of bytes to write
// Returns: PJDF_ERR_NONE if there was no error, otherwise an error code.
static PjdfErrCode WriteSDAdafruit(DriverInternal *pDriver, void* pBuffer, INT32U* pCount)
{
    PjdfErrCode retval;
    PjdfContextSD *pContext = (PjdfContextSD*) pDriver->deviceContext;
    HANDLE hSPI = pContext->spiHandle;
    
    if (!pContext->spiLocked) while(1);
    // if (!pContext->csAsserted) while(1); // TODO: does initialization require no assert?
    
    // adjust SPI transmission rate
    retval = Ioctl(hSPI, PJDF_CTRL_SPI_SET_DATARATE, (void*)&SDSpiDataRate, (INT32U*)&SizeofSDSpiDataRate); 
    if (retval != PJDF_ERR_NONE) while(1);
    
    retval = Write(hSPI, pBuffer, pCount);
        
    return retval;
}

// IoctlSDAdafruit
// pDriver: pointer to an initialized SD driver
// request: a request code chosen from those in pjdfCtrlSDAdafruit.h
// pArgs [in/out]: pointer to any data needed to fulfill the request
// pSize: the number of bytes pointed to by pArgs
// Returns: PJDF_ERR_NONE if there was no error, otherwise an error code.
static PjdfErrCode IoctlSDAdafruit(DriverInternal *pDriver, INT8U request, void* pArgs, INT32U* pSize)
{
    HANDLE handle;
    PjdfErrCode retval = PJDF_ERR_NONE;
    PjdfContextSD *pContext = (PjdfContextSD*) pDriver->deviceContext;
    switch (request)
    {
    case PJDF_CTRL_SD_ASSERT_CS:
        if (!pContext->spiLocked) while(1);
        SD_ADAFRUIT_CS_ASSERT();
        pContext->csAsserted = true;
        break;
    case PJDF_CTRL_SD_DEASSERT_CS:
        if (!pContext->csAsserted) while(1);
        SD_ADAFRUIT_CS_DEASSERT();
        pContext->csAsserted = false;
        break;
    case PJDF_CTRL_SD_LOCK_SPI:
        if (pContext->spiLocked) 
            return PJDF_ERR_NONE; // already locked
        retval = Ioctl(pContext->spiHandle, PJDF_CTRL_SPI_WAIT_FOR_LOCK, 0, 0);
        if (PJDF_IS_ERROR(retval)) while(1);
        pContext->spiLocked = true;
        break;
    case PJDF_CTRL_SD_RELEASE_SPI:
        if (!pContext->spiLocked) while(1); // not currently locked
        retval = Ioctl(pContext->spiHandle, PJDF_CTRL_SPI_RELEASE_LOCK, 0, 0);
        if (PJDF_IS_ERROR(retval)) while(1);
        pContext->spiLocked = false;
        break;
    case PJDF_CTRL_SD_SET_SPI_HANDLE:
        if (*pSize < sizeof(HANDLE))
        {
            return PJDF_ERR_ARG;
        }
        handle = *((HANDLE*)pArgs);
        if (!PJDF_IS_VALID_HANDLE(handle))
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


// Initializes the given SD driver.
PjdfErrCode InitSDAdafruit(DriverInternal *pDriver, char *pName)
{
    if (strcmp (pName, pDriver->pName) != 0) while(1); // pName should have been initialized in driversInternal[] declaration
    
    // Initialize semaphore for serializing operations on the device 
    pDriver->sem = OSSemCreate(1); 
    if (pDriver->sem == NULL) while (1);  // not enough semaphores available
    pDriver->refCount = 0; // number of Open handles to the device
    pDriver->maxRefCount = 1; // only one open handle allowed
    pDriver->deviceContext = &SDContext;
    
    BspSDInitAdafruit(); // Initialize related GPIO
  
    // Assign implemented functions to the interface pointers
    pDriver->Open = OpenSDAdafruit;
    pDriver->Close = CloseSDAdafruit;
    pDriver->Read = ReadSDAdafruit;
    pDriver->Write = WriteSDAdafruit;
    pDriver->Ioctl = IoctlSDAdafruit;
    
    pDriver->initialized = OS_TRUE;
    return PJDF_ERR_NONE;
}


