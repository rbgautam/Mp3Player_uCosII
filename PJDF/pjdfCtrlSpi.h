/*
    pjdfCtrlSpi.h
    SPI control definitions exposed to applications

    Developed for University of Washington embedded systems programming certificate
    
    2016/2 Nick Strathy wrote/arranged it after a framework by Paul Lever
*/

#ifndef __PJDFCTRLSPI_H__
#define __PJDFCTRLSPI_H__


// Control definitions for SPI1

#define PJDF_CTRL_SPI_WAIT_FOR_LOCK  0x01   // Wait for exclusive access to SPI, then lock it
#define PJDF_CTRL_SPI_RELEASE_LOCK   0x02   // Release exclusive SPI lock
#define PJDF_CTRL_SPI_SET_DATARATE   0x03   // Set transmission rate of the SPI interface

#endif