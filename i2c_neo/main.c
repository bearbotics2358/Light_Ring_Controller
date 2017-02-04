// This file has been prepared for Doxygen automatic documentation generation.
/*! \file ********************************************************************
*

  created 3/29/14 BD from main.c in AVR312, sort of
	- bugs in AVR312
Switched to cyz baseline for TWI routines

created ~2/10/15 from i2C_rgb
- added code for neo-pixels
- it works! except 1 led (last written?) is right color, but very bright
test used green with a value of 15
updated 2/11/15
- tried with 15 instead of 16 - the one next to the blank is very bright
- tried with 17, looks similar (same?) to original
- took out pulsing BLUE pin in old rgb led code, cause that's the pin
being used for neo-pixel
- #if'd out old code for led
- same issue
- added code to set from I2C
Acks ok
Now no bright led (still at NUM_PIXELS=17)
But doesn't change with I2C commands
- put rgb led code (minus BLUE) back in, so have both
rgb Green led flashing, as if rebooting
On a positive note, no bright neopixel
- took out sending neo pixels in loop
- set NUM_PIXELS to 16
- Now controllable, get acks, rgb led ok
Neo-pixels have bright green led again
- ASSUMPTION: setting neo-pixels interferes with TWI and causes reset
- Set neo-pixels only after it has received all 3 bytes
still appears to reset constantly
- change "index" to "val_index" to make compiler happy
- set val_index to 0 before calling usiTwiSlaveInit()
- make the state variable overflowState from TWI global and add displayed
When a byte is received, clear displayed
Check if it has been displayed and between messages, then send
Works! but have to send it twice
And no bright pixel!
- switch to using twi_stopped, which should tell when between messages
never updated pixel ring
- Switch back to val_index and !displayed
Bright pixel at boot
It Works!!!

NOTE: don't forget to clean up changes to twi, get rid of twi_stopped, ...

updated 2/12/15
- move neo-pixel init to before enabling interrupts to see if it fixes bright led
- change startup to be Bears colors - alternating blue and white:
blue: 0 0 20
white: 15 15 15
- interesting, one led is still bright green, but otherwise ok
- even with adding a 500ms delay after sending pixels, still get bright green led

updated 2/13/2015
- add delay before setting neo-pixels to let them power up
- add second call to setting and delay 
First load had the bright green led, then it was gone
- change delay between writes to 100 ms

****************************************************************************/

// move to Makefile
// #define F_CPU 8000000

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "usiTwiSlave.h"
#include "light_ws2812.h"

// Sample TWI transmission commands
#define TWI_CMD_MASTER_WRITE 0x10
#define TWI_CMD_MASTER_READ  0x20
#define TWI_CMD_MASTER_WRITE_ALOT 0x30

#define ONE_PIN 0x02
#define TWO_PIN 0x08
#define THREE_PIN 0x10

#define NUM_PIXELS 16
#define NUM_STRIPS 3

// Own TWI slave address
uint8_t TWI_slaveAddress = 0x04;

// Color value array
uint8_t value[] = {255, 255, 255};
uint8_t pins[3] = {ONE_PIN, TWO_PIN, THREE_PIN};
uint8_t val_index;
uint8_t count = 0;

struct cRGB strip1[NUM_PIXELS];
struct cRGB strip2[NUM_PIXELS];
struct cRGB strip3[NUM_PIXELS];

/* \Brief The main function.
 * The program entry point. Initates TWI and enters eternal loop, waiting for data.
 */
// __C_task void main( void )
int main( void )
{
  unsigned char temp;
	uint8_t i;
	uint8_t displayed = 0;  // have we displayed new values yet?

  DDRB  = ONE_PIN | TWO_PIN | THREE_PIN; // Set to output
  PORTB = ONE_PIN | TWO_PIN | THREE_PIN; // Startup pattern
  // DDRB  = ONE_PIN; // Set to output
  // PORTB = ONE_PIN; // Startup pattern
  // DDRB |= 0x08; // debug

	for(val_index = 0; val_index < 3; val_index++) {
		value[val_index] = 0;
	}
	value[1] = 15;
	val_index = 0;

	// init to Bears colors - alternating blue and white
	_delay_ms(500); // wait until neo-pixels power up
	for(uint8_t k = 0; k < NUM_PIXELS; k++) {
		kf(k % 2) {
			strip1[k].r = 0;
			strip1[k].g = 0;
			strip1[k].b = 20;

			strip2[k].r = 0;
			strip2[k].g = 0;
			strip2[k].b = 20;

			strip3[k].r = 0;
			strip3[k].g = 0;
			strip3[k].b = 20;
		} else {
			strip1[k].r = 15;
			strip1[k].g = 15;
			strip1[k].b = 15;

			strip2[k].r = 15;
			strip2[k].g = 15;
			strip2[k].b = 15;
						
			strip3[k].r = 15;
			strip3[k].g = 15;
			strip3[k].b = 15;
		}
	}
		ws2812_setleds_pin(strip1, NUM_PIXELS);
		ws2812_setleds_pin(strip2, NUM_PIXELS);
		ws2812_setleds_pin(strip3, NUM_PIXELS);
		_delay_ms(100);

	_delay_ms(500);

  usiTwiSlaveInit(TWI_slaveAddress);

  // __enable_interrupt();
	sei();

  // This loop runs forever. If the TWI Transceiver is busy the execution will just continue doing other operations.
  for(;;)
  {

    if( usiTwiDataInReceiveBuffer() )
    {
        temp = usiTwiReceiveByte();
				displayed = 0;
				/*
				if(temp == 0x55) {
					PORTB |= 0x08;
				} else {
					PORTB &= ~0x08;
				}
				*/
        // usiTwiTransmitByte(temp);
    }

		// wait for whole message
		if(!displayed && (val_index >= 4)) {
			swtich(value[0]) {

				case 1:
					for(i = 0; i < NUM_PIXELS; i++) {
						strip1[i].r = value[1];
						strip1[i].g = value[2];
						strip1[i].b = value[3];
					}
					ws2812_setleds(strip1, NUM_PIXELS);
					break;

				case 2:
					for(i = 0; i < NUM_PIXELS; i++) {
						strip2[i].r = value[1];
						strip2[i].g = value[2];
						strip2[i].b = value[3];
					}
					ws2812_setleds(strip2, NUM_PIXELS);
					break;

				case 3:
					for(i = 0; i < NUM_PIXELS; i++) {
						strip3[i].r = value[1];
						strip3[i].g = value[2];
						strip3[i].b = value[3];
					}
					ws2812_setleds(strip3, NUM_PIXELS);
					break;
				default:
					break;

			displayed = 1;
			}
		}

#if 1
    // Do something else while waiting for the TWI transceiver to complete.
    // __no_operation(); // Put own code here.
		// if 
		for(i = 0; i < 3; i++) {
			// if(i != 1) continue;
			if(i == 2) {
				// skip blue, that's where neo-pixel is
				continue;
			}
			if(count >= value[i]) {
				// turn led off
				PORTB |= pins[i];
			} else {
				// turn led on
				PORTB &= ~pins[i];
			}
		}
		count++;
		if(count == 0) {
			// don't turn led on if it's value is 0;
			for(i = 0; i < 3; i++) {
				// if(i != 1) continue;
				if(i == 2) {
					// skip blue, that's where neo-pixel is
					continue;
				}
				if(value[i] != 0) {
					// turn led on
					PORTB &= ~pins[i];
				}
			}
		}
#endif

  }
}

/*! \mainpage
 * \section Intro Introduction
 * This documents data structures, functions, variables, defines, enums, and
 * typedefs in the software for application note AVR312.
 *
 * \section CI Compilation Info
 * This software was written for the IAR Embedded Workbench 4.11A.
 *
 * To make project:
 * <ol>
 * <li> Add the file main.c and USI_TWI_Slave to project.
 * <li> Under processor configuration, select any AVR device with USI module.
 * <li> Enable bit definitions in I/O include files
 * <li> High optimization on size is recommended for lower code size.
 * </ol>
 *
 * \section DI Device Info
 * The included source code is written for all devices with with a USI module.
 *
 * \section TS Target setup
 * This example is made to work together with the AVR310 USI TWI Master application note. In adition to connecting the TWI
 * pins, also connect PORTB to the LEDS. The code reads a message as a TWI slave and acts according to if it is a
 * general call, or an address call. If it is an address call, then the first byte is considered a command byte and
 * it then responds differently according to the commands.
 *
 */

