/*
    pjdfCtrlMp3VS1053.h
    MP3 control definitions exposed to applications

    Developed for University of Washington embedded systems programming certificate
    
    2016/2 Nick Strathy wrote/arranged it after a framework by Paul Lever
*/

#ifndef __PJDFCTRLMP3VS1053_H__
#define __PJDFCTRLMP3VS1053_H__

// Control definitions for VS1053 MP3 device driver

// Selecting one of the the following two deselects the other one:
#define PJDF_CTRL_MP3_SELECT_DATA  0x01   // Assert data chip-select when operating SPI link to VS1053
#define PJDF_CTRL_MP3_SELECT_COMMAND  0x02   // Assert command chip-select when operating the SPI link to VS1053


#define PJDF_CTRL_MP3_SET_SPI_HANDLE 0x3  // Passes the required SPI handle to the MP3 driver to enable it to talk to the VS1053

#endif