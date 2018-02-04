/*
    pjdfInternal.h
    PJDF internal device driver interface for driver developers

    Developed for University of Washington embedded systems programming certificate
    
    2016/2 Nick Strathy wrote/arranged it after a framework by Paul Lever
*/
 
#ifndef __PJDFINTERNAL_H__
#define __PJDFINTERNAL_H__

#include "pjdf.h"


struct _DriverInternal; // forward declaration
typedef struct _DriverInternal DriverInternal; // forward declaration

// Device interface to be implemented by developers
struct _DriverInternal
{
    char *pName;     // name used by applications and internally to identify the device
    
    // Method for initializing the device driver before exposing it to applications
    PjdfErrCode (*Init)(DriverInternal *pDriver, char *pName);
    
    BOOLEAN initialized; // true if Init() ran successfully otherwise false.
    OS_EVENT *sem;  // semaphore to serialize operations on the device 
    INT8U refCount; // current number of Open handles to the device
    INT8U maxRefCount; // Maximum Open handles allowed for the device
    void *deviceContext; // device dependent data
    
    // Device-specific methods for operating on the device
    PjdfErrCode (*Open)(DriverInternal *pDriver, INT8U flags);
    PjdfErrCode (*Close)(DriverInternal *pDriver);
    PjdfErrCode (*Read)(DriverInternal *pDriver, void* pBuffer, INT32U* pCount);
    PjdfErrCode (*Write)(DriverInternal *pDriver, void* pBuffer, INT32U* pCount);
    PjdfErrCode (*Ioctl)(DriverInternal *pDriver, INT8U request, void* pArgs, INT32U* pSize);
};


// PJDF DEVELOPER TODO: add the prototype of your driver's Init() implementation here:
PjdfErrCode InitSPI(DriverInternal *pDriver, char *pName);
PjdfErrCode InitMp3VS1053(DriverInternal *pDriver, char *pName);
PjdfErrCode InitLcdILI9341(DriverInternal *pDriver, char *pName);
PjdfErrCode InitSDAdafruit(DriverInternal *pDriver, char *pName);
PjdfErrCode InitTouchFT6206(DriverInternal *pDriver, char *pName);

#endif