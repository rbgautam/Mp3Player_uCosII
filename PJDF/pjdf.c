/*
    pjdf.c
    PJ Driver Framework generic device driver implementation.
    This is the implementation of the pjdf.h driver interface exposed to applications.

    Developed for University of Washington embedded systems programming certificate
    
    2016/2 Nick Strathy wrote/arranged it after a framework by Paul Lever
*/

#include "bsp.h"
#include "pjdf.h"
#include "pjdfInternal.h"

static char *DeviceDriverIDs [] =
{
    PJDF_DEVICE_IDS
};

#define MAXDEVICES (sizeof(DeviceDriverIDs)/sizeof(char*))


// PJDF DEVELOPER TODO: add the reference to your driver's pName and Init() function here:
// IMPORTANT: maintain the same order as in PJDF_DEVICE_IDS
static DriverInternal driversInternal[MAXDEVICES] = 
{
    {PJDF_DEVICE_ID_SPI1, InitSPI},
    {PJDF_DEVICE_ID_MP3_VS1053, InitMp3VS1053},
    {PJDF_DEVICE_ID_LCD_ILI9341, InitLcdILI9341},
    {PJDF_DEVICE_ID_SD_ADAFRUIT, InitSDAdafruit},
    {PJDF_DEVICE_ID_TOUCH_FT6206, InitTouchFT6206},
};


// Opens a handle to the specified device.
// pName: the identifier of the device chosen from PJDF_DEVICE_IDS.
// flags: a device-defined bit string used to configure options of the device
// Returns: if no error occured, a valid handle is returned which may be used to
//    operate on the device. If an error occurs, an error code is returned. 
//    Valid handles are positive numbers; error codes are negative numbers.
HANDLE Open(char *pName, INT8U flags)
{
    HANDLE retval;
    int i;
    DriverInternal *pDriver;
    INT8U osErr;
    
    for (i = 0, pDriver = &driversInternal[0]; i < MAXDEVICES; i++, pDriver++)
    {
        if (strcmp(pName, pDriver->pName) == 0)
        {
            // We found the device.

            // Enter a critical section to increment the device reference count and call device specific Open()
            OSSemPend(pDriver->sem, 0, &osErr);
            if (osErr != OS_ERR_NONE) while (1);
            if (pDriver->refCount < pDriver->maxRefCount)
            {
                pDriver->refCount += 1;
            }
            else
            {
                retval = PJDF_ERR_TOO_MANY_REFS;
                OSSemPost(pDriver->sem);
                break;
            }
            
            // Call the Open() function of the device
            retval = pDriver->Open(pDriver, flags);
            if (PJDF_IS_ERROR(retval))
            {
                // Failed to open so decrement ref count
                pDriver->refCount -= 1;
            }
            else
            {
                retval = i + 1; // add 1 to ensure handle is positive
            }
            OSSemPost(pDriver->sem);
            break;
        }
    }
    if (i >= MAXDEVICES)
    {
        // we failed to find the device
        retval = PJDF_ERR_DEVICE_NOT_FOUND;
    }
    return retval;
}

PjdfErrCode Close(HANDLE handle)
{
    PjdfErrCode retval;
    DriverInternal *pDriver;
    INT8U osErr;
    
    if (handle <= 0 || handle > MAXDEVICES)
    {
        retval = PJDF_ERR_INVALID_HANDLE;
        while (1);
    }
    
    pDriver = &driversInternal[handle-1];
    if (!pDriver->initialized)
    {
        retval = PJDF_ERR_DEVICE_NOT_INIT;
        while (1);
    }
    
    // Enter a critical section to call device specific Close() and decrement the device reference count
    OSSemPend(pDriver->sem, 0, &osErr);
    if (osErr != OS_ERR_NONE) while (1);
    
    if (pDriver->refCount == 0)
    {
        retval = PJDF_ERR_DEVICE_NOT_OPEN;
    }
    else
    {
        retval = pDriver->Close(pDriver);
    }

    if (!PJDF_IS_ERROR(retval))
    {
        pDriver->refCount -= 1;
    }
    
    OSSemPost(pDriver->sem);
    return retval;
}

PjdfErrCode Read(HANDLE handle, void* pBuffer, INT32U* pLength)
{
    PjdfErrCode retval;
    DriverInternal *pDriver;
    if (handle <= 0 || handle > MAXDEVICES)
    {
        retval = PJDF_ERR_INVALID_HANDLE;
        while (1);
    }
    
    pDriver = &driversInternal[handle-1];
    if (!pDriver->initialized)
    {
        retval = PJDF_ERR_DEVICE_NOT_INIT;
        while (1);
    }
    retval = pDriver->Read(pDriver, pBuffer, pLength);
    return retval;
}

PjdfErrCode Write(HANDLE handle, void* pBuffer, INT32U* pLength)
{
    PjdfErrCode retval;
    DriverInternal *pDriver;
    if (handle <= 0 || handle > MAXDEVICES)
    {
        retval = PJDF_ERR_INVALID_HANDLE;
        while (1);
    }
    
    pDriver = &driversInternal[handle-1];
    if (!pDriver->initialized)
    {
        retval = PJDF_ERR_DEVICE_NOT_INIT;
        while (1);
    }
    retval = pDriver->Write(pDriver, pBuffer, pLength);
    return retval;
}

PjdfErrCode Ioctl(HANDLE handle, INT8U request, void* pArgs, INT32U* pSize)
{
    PjdfErrCode retval;
    DriverInternal *pDriver;
    if (handle <= 0 || handle > MAXDEVICES)
    {
        retval = PJDF_ERR_INVALID_HANDLE;
        while (1);
    }
    
    pDriver = &driversInternal[handle-1];
    if (!pDriver->initialized)
    {
        retval = PJDF_ERR_DEVICE_NOT_INIT;
        while (1);
    }
    retval = pDriver->Ioctl(pDriver, request, pArgs, pSize);
    return retval;
}


// InitPjdf
// Initialize the device driver framework.
// Calls the Init function of each driver in driversInternal.
//
// The purpose of this function is to put the driver framework in a valid initial 
// state before applications start to use it.
//
// We initialize the hardware for the devices and ensure that statically
// allocated device driver memory is properly initialized. Embedded device
// drivers do not typically use dynamically allocated (heap) memory, instead all of 
// their memory is allocated at compile time and we now initialize 
// that memory before the drivers are exposed to applications.
PjdfErrCode InitPjdf()
{
    PjdfErrCode retval = PJDF_ERR_NONE;
    for (int i = 0; i < MAXDEVICES; i++)
    {
        retval = driversInternal[i].Init(&driversInternal[i], DeviceDriverIDs[i]);
        if (PJDF_IS_ERROR(retval))
        {
            while (1); // a driver Init() function failed
        }
    }
    
    return retval;
}
