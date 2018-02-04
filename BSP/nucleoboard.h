/**********************************************************
FILE NAME  	: nucleoboard.h
DESCRIPTION     : Board specific definitions live here
                : These generally come from the Nucleo schematics
Copyright	: David Allegre
		: 2015
		: All Rights Reserved
**********************************************************/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __NUCLEOBOARD_H
#define __NUCLEOBOARD_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_usart.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_spi.h"

/* Exported define -----------------------------------------------------------*/
// Clock speed
#define CLOCK_HSI               HSI_VALUE

// User button
#define GPIO_PIN_USER_BUTTON    GPIO_Pin_13
#define GPIO_PORT_USER_BUTTON   GPIOC

// Green LED
#define GPIO_PIN_LD2            GPIO_Pin_5
#define GPIO_PORT_LD2           GPIOA

// USART GPIO
#define GPIO_TX_AF_SOURCE       GPIO_PinSource2
#define GPIO_RX_AF_SOURCE       GPIO_PinSource3
#define GPIO_USART_AF           GPIO_AF_USART2
#define GPIO_PIN_TX             GPIO_Pin_2
#define GPIO_PIN_RX             GPIO_Pin_3
#define GPIO_PORT_USART         GPIOA

// USART 
#define BAUD_RATE               38400
#define COMM                    USART2

#endif