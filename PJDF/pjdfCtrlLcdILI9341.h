/*
    pjdfCtrlLcdILI9341.h
    LCD control definitions exposed to applications

    Developed for University of Washington embedded systems programming certificate
    
    2016/2 Nick Strathy wrote/arranged it after a framework by Paul Lever
*/

#ifndef __PJDFCTRLLCDILI9341_H__
#define __PJDFCTRLLCDILI9341_H__

// Control definitions for VS1053 MP3 device driver

// Selecting one of the the following two deselects the other one:
#define PJDF_CTRL_LCD_SELECT_DATA  0x01   // Assert data chip-select when operating SPI link to ILI9341
#define PJDF_CTRL_LCD_SELECT_COMMAND  0x02   // Assert command-chip select when operating the SPI linke ILI9341


#define PJDF_CTRL_LCD_SET_SPI_HANDLE 0x3  // Passes the required SPI handle to the LCD driver to enable it to talk to the ILI9341

#endif