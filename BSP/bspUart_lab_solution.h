/*
    bspSpi.h

    Board support for controlling UART interfaces on NUCLEO-F401RE MCU

    Developed for University of Washington embedded systems programming certificate
    
    2016/2 Nick Strathy wrote/arranged it
*/

#ifndef __BSPUART_H
#define __BSPUART_H

// Application interface to hardware

void PrintByte(char c);
char ReadByte();


#endif /* __BSPUART_H */