/********************************************************************************

Header file for the USI TWI Slave driver.

Created by Donald R. Blake
donblake at worldnet.att.net

---------------------------------------------------------------------------------

Created from Atmel source files for Application Note AVR312: Using the USI Module
as an I2C slave.

This program is free software; you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.

---------------------------------------------------------------------------------

Change Activity:

    Date       Description
   ------      -------------
  15 Mar 2007  Created.
  3/29/14 BD - updated, removed cyz include files
  2/11/15 - moved overflowstate enum and variable to .h so can monitor from
main
  - added twi_stopped variable for tracking when not in a message instead

********************************************************************************/



#ifndef _USI_TWI_SLAVE_H_
#define _USI_TWI_SLAVE_H_



/********************************************************************************

                                    includes

********************************************************************************/

#include <stdbool.h>

// #include "../cyz/cyz_cmd.h"

/********************************************************************************

                                   prototypes

********************************************************************************/

typedef enum
{
  USI_SLAVE_CHECK_ADDRESS                = 0x00,
  USI_SLAVE_SEND_DATA                    = 0x01,
  USI_SLAVE_REQUEST_REPLY_FROM_SEND_DATA = 0x02,
  USI_SLAVE_CHECK_REPLY_FROM_SEND_DATA   = 0x03,
  USI_SLAVE_REQUEST_DATA                 = 0x04,
  USI_SLAVE_GET_DATA_AND_SEND_ACK        = 0x05
} overflowState_t;

volatile overflowState_t overflowState;
volatile twi_stopped;


void flushTwiBuffers(void);

void    usiTwiSlaveInit( uint8_t address );
void    usiTwiTransmitByte( uint8_t );
uint8_t usiTwiReceiveByte( void );
bool    usiTwiDataInReceiveBuffer( void );



/********************************************************************************

                           driver buffer definitions

********************************************************************************/

// permitted RX buffer sizes: 1, 2, 4, 8, 16, 32, 64, 128 or 256

#define TWI_RX_BUFFER_SIZE  ( 16 )
#define TWI_RX_BUFFER_MASK  ( TWI_RX_BUFFER_SIZE - 1 )

#if ( TWI_RX_BUFFER_SIZE & TWI_RX_BUFFER_MASK )
#  error TWI RX buffer size is not a power of 2
#endif

// permitted TX buffer sizes: 1, 2, 4, 8, 16, 32, 64, 128 or 256

#define TWI_TX_BUFFER_SIZE ( 16 )
#define TWI_TX_BUFFER_MASK ( TWI_TX_BUFFER_SIZE - 1 )

#if ( TWI_TX_BUFFER_SIZE & TWI_TX_BUFFER_MASK )
#  error TWI TX buffer size is not a power of 2
#endif



#endif  // ifndef _USI_TWI_SLAVE_H_
