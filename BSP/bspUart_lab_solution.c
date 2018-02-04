/*
    bspSpi.c

    Board support for controlling UART interfaces on NUCLEO-F401RE MCU

    Developed for University of Washington embedded systems programming certificate
    
    2016/2 Nick Strathy wrote/arranged it
*/

#include "bsp.h"

/**
  * @brief  Print a character on the HyperTerminal
  * @param  c: The character to be printed
  * @retval None
  */
void PrintByte(char c)
{
  USART_SendData(COMM, c);
  while (USART_GetFlagStatus(COMM, USART_FLAG_TXE) == RESET);
}

/**
  * @brief  Busy wait for a character on the HyperTerminal
  * @retval: The character that was read
  */
char ReadByte()
{
    while (USART_GetFlagStatus(COMM, USART_FLAG_RXNE) == RESET);
    uint16_t c = USART_ReceiveData(COMM);
    USART_ClearFlag(COMM, USART_FLAG_RXNE);
    return c;
}