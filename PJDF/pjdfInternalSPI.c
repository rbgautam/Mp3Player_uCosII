/*
    pjdfInternalSPI.c
    The implementation of the internal PJDF interface pjdfInternal.h targetted for the
    Serial Peripheral Interface (SPI)

    Developed for University of Washington embedded systems programming certificate
    
    2016/2 Nick Strathy wrote/arranged it after a framework by Paul Lever
*/

#include "bsp.h"
#include "pjdf.h"
#include "pjdfInternal.h"

// Control registers etc for SPI hardware
typedef struct _PjdfContextSpi
{
    SPI_TypeDef *spiMemMap; // Memory mapped register block for a SPI interface
} PjdfContextSpi;

static PjdfContextSpi spi1Context = { PJDF_SPI1 };



// OpenSPI
// No special action required to open SPI device
static PjdfErrCode OpenSPI(DriverInternal *pDriver, INT8U flags)
{
    // Nothing to do
    return PJDF_ERR_NONE;
}

// CloseSPI
// No special action required to close SPI device
static PjdfErrCode CloseSPI(DriverInternal *pDriver)
{
    // Nothing to do
    return PJDF_ERR_NONE;
}

// ReadSPI
// Writes the contents of the buffer to the given device while concurrently reading
// the full duplex output of the device. The caller must first
// assert the appropriate slave chip select line before calling writeSPI.
//
// pDriver: pointer to an initialized SPI device
// pBuffer: on input the data to write to the device, on output the data output by the device
//     note: the buffer must not reside in readonly memory.
// pCount: the number of bytes to write/read.
// Returns: PJDF_ERR_NONE if there was no error, otherwise an error code.
static PjdfErrCode ReadSPI(DriverInternal *pDriver, void* pBuffer, INT32U* pCount)
{
    PjdfContextSpi *pContext = (PjdfContextSpi*) pDriver->deviceContext;
    if (pContext == NULL) while(1);
    SPI_GetBuffer(pContext->spiMemMap, (INT8U*) pBuffer, *pCount);
    return PJDF_ERR_NONE;
}


// WriteSPI
// Writes the contents of the buffer to the given device. The caller must first
// assert the appropriate slave chip select line before calling writeSPI.
//
// pDriver: pointer to an initialized SPI device
// pBuffer: the data to write to the device
// pCount: the number of bytes to write
// Returns: PJDF_ERR_NONE if there was no error, otherwise an error code.
static PjdfErrCode WriteSPI(DriverInternal *pDriver, void* pBuffer, INT32U* pCount)
{
    PjdfContextSpi *pContext = (PjdfContextSpi*) pDriver->deviceContext;
    if (pContext == NULL) while(1);
    SPI_SendBuffer(pContext->spiMemMap, (INT8U*) pBuffer, *pCount);
    return PJDF_ERR_NONE;
}

// IoctlSPI
// Handles the request codes defined in pjdfCtrlSpi.h
static PjdfErrCode IoctlSPI(DriverInternal *pDriver, INT8U request, void* pArgs, INT32U* pSize)
{
    INT8U osErr;
    PjdfContextSpi *pContext = (PjdfContextSpi*) pDriver->deviceContext;
    if (pContext == NULL) while(1);
    switch (request)
    {
    case PJDF_CTRL_SPI_WAIT_FOR_LOCK:
        OSSemPend(pDriver->sem, 0, &osErr);
        break;
    case PJDF_CTRL_SPI_RELEASE_LOCK:
        osErr = OSSemPost(pDriver->sem);
        break;
    case PJDF_CTRL_SPI_SET_DATARATE: // Call BSP code to adjust transmission speed of SPI
        if (*pSize != sizeof(INT16U)) while (1);
        SPI_SetDataRate(pContext->spiMemMap, *(INT16U*)pArgs);
        break;
    default:
        while(1);
        break;
    }
    return PJDF_ERR_NONE;
}


// Initializes the given SPI driver.
PjdfErrCode InitSPI(DriverInternal *pDriver, char *pName)
{   
    if (strcmp (pName, pDriver->pName) != 0) while(1); // pName should have been initialized in driversInternal[] declaration
    
    // Initialize semaphore for serializing operations on the device 
    pDriver->sem = OSSemCreate(1); 
    if (pDriver->sem == NULL) while (1);  // not enough semaphores available
    pDriver->refCount = 0; // initial number of Open handles to the device
    
    // We may choose to handle multiple hardware instances of the SPI interface
    // each of which gets its own DriverInternal struct. Here we initialize 
    // the context of the SPI hardware instance specified by pName.
    if (strcmp(pName, PJDF_DEVICE_ID_SPI1) == 0)
    {
        pDriver->maxRefCount = 10; // Maximum refcount allowed for the device
        pDriver->deviceContext = (void*) &spi1Context;
        BspSPI1Init(); // init SPI1 hardware
    }
  
    // Assign implemented functions to the interface pointers
    pDriver->Open = OpenSPI;
    pDriver->Close = CloseSPI;
    pDriver->Read = ReadSPI;
    pDriver->Write = WriteSPI;
    pDriver->Ioctl = IoctlSPI;
    
    pDriver->initialized = OS_TRUE;
    return PJDF_ERR_NONE;
}

