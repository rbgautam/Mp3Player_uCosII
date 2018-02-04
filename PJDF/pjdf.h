/*
    pjdf.h
    This is a generic device driver interface exposed to applications.
    This header is the main application interface of the PJ Driver Framework.

    Developed for University of Washington embedded systems programming certificate
    
    2016/2 Nick Strathy wrote/arranged it after a framework by Paul Lever
*/
 
#ifndef __PJDF_H__
#define __PJDF_H__

#include "bsp.h"
#include "pjdfCtrlSpi.h"
#include "pjdfCtrlLcdILI9341.h"
#include "pjdfCtrlMp3VS1053.h"
#include "pjdfCtrlSDAdafruit.h"
#include "pjdfInternalTouchFT6206.h"

typedef INT8S HANDLE;
#define PJDF_IS_VALID_HANDLE(x)  (x > 0) // A valid device driver handle is a positive number


// PJDF DEVELOPER TODO LIST FOR ADDING A NEW DRIVER: 
//    - define a new PJDF_DEVICE_ID_<MYDEVICE> below
//    - reference it under PJDF_DEVICE_IDS below
//    - add a new pjdfInternal<mydevice>.c module to implement the pjdfInternal.h interface
//    - reference the Init() function of your driver in the driversInternal array in pjdf.c
//    - add a new pjdfCtrl<mydevice>.h interface to define the Ioctl() functionality of your device
//    - #include your pjdfCtrl<mydevice>.h in the present header file above
//    - add modules as needed to the BSP folder to keep board-dependent code out of your PJDF implementation

// Master list of device drivers.
// These are the identifiers used by applications to Open device drivers.
#define PJDF_DEVICE_ID_SPI1  "/dev/spi1"
#define PJDF_DEVICE_ID_MP3_VS1053   "/dev/mp3_vs1053"
#define PJDF_DEVICE_ID_LCD_ILI9341   "/dev/lcd_ili9341"
#define PJDF_DEVICE_ID_SD_ADAFRUIT   "/dev/sd_adafruit"
#define PJDF_DEVICE_ID_TOUCH_FT6206  "/dev/touch_ft6206"   

#define PJDF_DEVICE_IDS \
        PJDF_DEVICE_ID_SPI1, \
        PJDF_DEVICE_ID_MP3_VS1053, \
        PJDF_DEVICE_ID_LCD_ILI9341, \
        PJDF_DEVICE_ID_SD_ADAFRUIT, \
        PJDF_DEVICE_ID_TOUCH_FT6206 
           
// Driver error codes
// Definition: all driver error codes are negative numbers except PJDF_ERR_NONE.
#define PJDF_IS_ERROR(x) (x < 0)
           
typedef INT8S PjdfErrCode;
#define PJDF_ERR_NONE  0
#define PJDF_ERR_DEVICE_NOT_FOUND -1  // Search for a given device failed to find it
#define PJDF_ERR_TOO_MANY_REFS  -2 // The maximum number of handles for a given device was exceeded
#define PJDF_ERR_DEVICE_NOT_INIT -3 // Device was not initialized before use
#define PJDF_ERR_INVALID_HANDLE -4 // Handle is not in allowed range of handle values
#define PJDF_ERR_ARG -5  // Error in argument to Ioctl()
#define PJDF_ERR_UNKNOWN_CTRL_REQUEST -6 // A given Ctrl request was not defined for the driver
#define PJDF_ERR_CHIP_SELECT -7 // Incorrect chip selection or no chip selected
#define PJDF_ERR_DEVICE_NOT_OPEN -8 // Attempted operation on device that is not open

// Generic API methods exposed to applications for operating on devices
HANDLE Open(char *pName, INT8U flags);
PjdfErrCode Close(HANDLE handle);
PjdfErrCode Read(HANDLE handle, void* pBuffer, INT32U* pLength);
PjdfErrCode Write(HANDLE handle, void* pBuffer, INT32U* pLength);
PjdfErrCode Ioctl(HANDLE handle, INT8U request, void* pArgs, INT32U* pSize);

// Method called by the OS to initialize the driver framework
PjdfErrCode InitPjdf();

#endif