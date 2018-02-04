/*
    bsp.c
    Miscellaneous  board support routines for use by applications

    Developed for University of Washington embedded systems programming certificate
    
    2016/2 Nick Strathy wrote/arranged it
*/

#include "bsp.h"

void SetLED(BOOLEAN On)
{
    if (On) {
        GPIO_SetBits(GPIO_PORT_LD2, GPIO_PIN_LD2);
      } else {
        /* The high 16 bits of BSRR reset the pin */
        GPIO_ResetBits(GPIO_PORT_LD2, GPIO_PIN_LD2);
     }
}

