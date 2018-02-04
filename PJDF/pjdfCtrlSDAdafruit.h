/*
    pjdfCtrlSDAdfruitMusicMaker.h
    Control definitions for the SD card on the Adafruit Music Maker shield exposed to applications

    Developed for University of Washington embedded systems programming certificate
    
    2016/2 Nick Strathy wrote/arranged it after a framework by Paul Lever
*/

#ifndef __PJDFCTRLSDADFRUITMUSICMAKER_H__
#define __PJDFCTRLSDADFRUITMUSICMAKER_H__

// Control definitions for SD card device driver

#define PJDF_CTRL_SD_ASSERT_CS 0x1  // Asserts the SPI chip select for the SD card
#define PJDF_CTRL_SD_DEASSERT_CS 0x2  // De-asserts the SPI chip select for the SD card
#define PJDF_CTRL_SD_LOCK_SPI 0x3  // Waits for exclusive access to the SD's SPI and locks it
#define PJDF_CTRL_SD_RELEASE_SPI 0x4  // Release exclusive access to the SD's SPI

#define PJDF_CTRL_SD_SET_SPI_HANDLE 0x5  // Passes the required SPI handle to the SD driver to enable it to talk to the SD card

#endif