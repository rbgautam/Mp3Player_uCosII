/*
    PJDFINTERNALTOUCHFT6206.h
    Touch control definitions exposed to applications
   
    
    2017/02/25 Rahul Gautam wrote/arranged it after a framework by Paul Lever
*/



/*
I2C_start
I2C_write
I2C_stop
I2C_read_nack
I2C_read_ack
*/

#ifndef __PJDFINTERNALTOUCHFT6206_H__
#define __PJDFINTERNALTOUCHFT6206_H__

// Control definitions for FT6206 device driver

// Selecting one of the the following deselects the other one:
#define PJDF_CTRL_TOUCH_I2C_START_TRANSMIT  0x01   
#define PJDF_CTRL_TOUCH_I2C_START_RECEIVE  0x02   
#define PJDF_CTRL_TOUCH_I2C_WRITE  0x03     
#define PJDF_CTRL_TOUCH_I2C_READ_NACK  0x04
#define PJDF_CTRL_TOUCH_I2C_READ_ACK  0x05
#define PJDF_CTRL_TOUCH_I2C_STOP  0x06   

typedef uint8_t DataHandle;



#endif

