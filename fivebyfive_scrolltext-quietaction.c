/*
 * fivebyfive_scrolltext.c - simple "text scrolling" test for FiveByFive badge circuit
 *
 * Copyright (c) 2012 - Rolf van Widenfelt - Some rights reserved.
 *
 * Note: This source code is licensed under a Creative Commons License, CC-by-nc-sa.
 *		(attribution, non-commercial, share-alike)
 *  	see http://creativecommons.org/licenses/by-nc-sa/3.0/ for details.
 *
 *
 *	TODO:
 *		need to work out an efficient scrolling bitmap and display.
 *
 *		(answer)
 *		use "pixel groups" which are groups of 4-5 pixels which
 *		can be lit simultaneously.
 *		there are 6 pixel groups that cover all 25 pixels, yielding a 1/6 duty cycle for each LED.
 *
 *	revision history:
 *
 *	- jun 28, 2012 - rolf
 *		(comment)
 *
 *	- may 26, 2012 - rolf
 *		implement a "scroller" based on a given input string.
 *		to change the message, simply search for "change this string!"
 *
 *	- may 14, 2012 - rolf
 *		make version for AVTOKYO (messages are: AVTOKYO and HACKER!)
 *
 *	- mar 11, 2012 - rolf
 *		add code to poll "mode" switch.. if pressed, the message should change.
 *		this required additional "blanking" tests in the Refresh function.
 *		it also required, upon a "mode" switch, re-setting parameters related to the message, so that the new one
 *		starts at the beginning.
 *
 *		TODO:  
 *			mode switch works surprisingly well,
 *			but it needs a better pause between messages!  (it's a bit abrupt if mid-message)
 *
 *	- feb 1, 2012 - rolf
 *		change message to "PIZZA".
 *
 *	- jan 8, 2012 - rolf
 *		changed message.
 *		also, now we detect whether the F_LINE works, and handle the switch code accordingly.
 *		this adds some code, but makes debugging before burning the RSTDISBL fuse possible.
 *		TODO: still needs "blanking" code updated with these flags!!
 *			also, use button press to change "mode".
 *
 *	- dec 25, 2011 - rolf
 *		add switch polling code to the Refresh() function.
 *		we want a switch press to also blank the LEDs (i.e. lines A-E should stay hi-z).
 *		note: for testing (with RESET* pin still enabled) 
 *			we don't know what PB5 will actually do, since it is deactivated.
 *			may need to test with the "RSTDISBL" fuse programmed.
 *
 *	- dec 24, 2011 - rolf
 *		(start with "scroll.cs" code)
 *		this version tries to actually scroll a short message in the available code memory.
 *		scrolling "XOX " takes 1840 bytes!  (.lst file says: 0x0716 + 0x001a == 1840 bytes)
 *
 *	- dec 23, 2011 - rolf
 *		NOTE: for "hacked" V0.9 of board... we invert FLINE signal in setpixel().
 *		it now needs to drive LOW in order to turn on any LEDs in the top ("F") row.
 *
 *	- dec 21, 2011 - rolf
 *		try code on V0.9 of board... top row is not right!  but it appears to be
 *		a drive current problem with PB5 (F LINE).
 *
 *	- dec 18, 2011 - rolf
 *		created.
 *
 */

#include "fivebyfive.h"		/* XXX for now, this defines clock speed F_CPU */

extern "C" {
#include <inttypes.h>
#include <avr/io.h>				/* this takes care of definitions for our specific AVR */
#include <avr/interrupt.h>		/* for interrupts, ISR macro, etc. */
#include <string.h>				/* for strlen, strcpy, etc. */
}

#include "fivebyfive_iodefs.h"		/* defines IO pins, and output_low(), output_high() macros */

#include "font.h"		/* a simple font library */


/* lookup table and related macros for charlie-plexed LEDs */

#define PACK(h,l)	((h<<4)|l)
#define UNPACKH(b)	((b>>4)&0xf)
#define UNPACKL(b)	(b&0xf)
#define A 0
#define B 1
#define C 2
#define D 3
#define E 4
#define F 5

uint8_t LEDS[25] = {
	PACK(F,E),		/* top row, left to right */
	PACK(F,D),
	PACK(F,C),
	PACK(F,B),
	PACK(F,A),
	
	PACK(D,E),
	PACK(E,D),
	PACK(E,C),
	PACK(E,B),
	PACK(E,A),
	
	PACK(C,E),
	PACK(C,D),
	PACK(D,C),
	PACK(D,B),
	PACK(D,A),
	
	PACK(B,E),
	PACK(B,D),
	PACK(B,C),
	PACK(C,B),
	PACK(C,A),
	
	PACK(A,E),		/* bottom row, left to right */
	PACK(A,D),
	PACK(A,C),
	PACK(A,B),
	PACK(B,A),
};


// display buffer (we use the 5 LSBs of each byte)
uint8_t Disp[5];

// font buffer (we use the MSBs of each byte, depending on the char)
// XXX defined in font.c how
//uint8_t FontChar[5];

// global flag that indicates whether we can use F_LINE for detecting switch presses
uint8_t FLineEnabled;

// forward references
static void avrinit(void);
//void start_timer1(void);

void setpixel(uint8_t x, uint8_t y, uint8_t val);


// quick and dirty delay (each count is roughly 1ms)
void mydelay1(uint8_t c)
{
	volatile int dummy = 0;
	int i;

	while (c--) {
		for (i = 0;  i < 740; i++) {
			dummy = 0;
		}
	}
}

// quick and dirty delay (each count is roughly 10ms)
void mydelay10(uint8_t c)
{
	volatile int dummy = 0;
	int i;

	while (c--) {
		for (i = 0;  i < 7400; i++) {
			dummy = 0;
		}
	}
}

// blink a specific LED (at x,y) "n" times
void blinkn(uint8_t n)
{
	uint8_t del;
	uint8_t x = 0;
	uint8_t y = 1;
	uint8_t i;

	/* blink LED6 (at 0,1) "n" times */

	del = 25;
	for (i = 0; i < n; i++) {
		setpixel(x, y, 1);	// LED(x,y) on
		mydelay10(del);
		setpixel(x, y, 0);	// LED(x,y) off
		mydelay10(del);
	}
}


// draw (i.e. "refresh") the display buffer (see Disp array).
void Refresh()
{
	uint8_t i;
	uint8_t del1 = 1;	// delay between "pixel groups" (there are 6)

	uint8_t a,b,c,d,e;
	
	//uint8_t blank = 0;					// determines whether part (or all) of display is "blanked" (i.e. turned off)
	uint8_t blankEn = FLineEnabled;		// check global flag whether F_LINE should be looked at or not


	for (i = 0; i < 6; i++) {

		switch (i) {

		case 0:
			// decide which pixels are on
			a = Disp[0] & 0x10;
			b = Disp[0] & 0x08;
			c = Disp[0] & 0x04;
			d = Disp[0] & 0x02;
			e = Disp[0] & 0x01;
			
			// first, poll switch - switch pin is active low (i.e. low if pressed)
			input_test(F_LINE);	// dummy read
			NOP();
			if (blankEn && (input_test(F_LINE) == 0)) {
				;
			} else {
				if (a) setpixel(0, 0, 1);
				if (b) setpixel(1, 0, 1);
				if (c) setpixel(2, 0, 1);
				if (d) setpixel(3, 0, 1);
				if (e) setpixel(4, 0, 1);
			}
			mydelay1(del1);
			if (a) setpixel(0, 0, 0);
			if (b) setpixel(1, 0, 0);
			if (c) setpixel(2, 0, 0);
			if (d) setpixel(3, 0, 0);
			if (e) setpixel(4, 0, 0);
			break;
		
		case 1:
			b = Disp[1] & 0x08;
			c = Disp[1] & 0x04;
			d = Disp[1] & 0x02;
			e = Disp[1] & 0x01;

			input_test(F_LINE);	// dummy read
			NOP();
			if (blankEn && (input_test(F_LINE) == 0)) {
				;
			} else {
			if (b) setpixel(1, 1, 1);
			if (c) setpixel(2, 1, 1);
			if (d) setpixel(3, 1, 1);
			if (e) setpixel(4, 1, 1);
			}
			mydelay1(del1);
			if (b) setpixel(1, 1, 0);
			if (c) setpixel(2, 1, 0);
			if (d) setpixel(3, 1, 0);
			if (e) setpixel(4, 1, 0);
			break;
		
		case 2:
			a = Disp[1] & 0x10;
			c = Disp[2] & 0x04;
			d = Disp[2] & 0x02;
			e = Disp[2] & 0x01;
			
			input_test(F_LINE);	// dummy read
			NOP();
			if (blankEn && (input_test(F_LINE) == 0)) {
				;
			} else {
			if (a) setpixel(0, 1, 1);
			if (c) setpixel(2, 2, 1);
			if (d) setpixel(3, 2, 1);
			if (e) setpixel(4, 2, 1);
			}
			mydelay1(del1);
			if (a) setpixel(0, 1, 0);
			if (c) setpixel(2, 2, 0);
			if (d) setpixel(3, 2, 0);
			if (e) setpixel(4, 2, 0);
			break;
			
		case 3:
			a = Disp[2] & 0x10;
			b = Disp[2] & 0x08;
			d = Disp[3] & 0x02;
			e = Disp[3] & 0x01;

			input_test(F_LINE);	// dummy read
			NOP();
			if (blankEn && (input_test(F_LINE) == 0)) {
				;
			} else {
			if (a) setpixel(0, 2, 1);
			if (b) setpixel(1, 2, 1);
			if (d) setpixel(3, 3, 1);
			if (e) setpixel(4, 3, 1);
			}
			mydelay1(del1);
			if (a) setpixel(0, 2, 0);
			if (b) setpixel(1, 2, 0);
			if (d) setpixel(3, 3, 0);
			if (e) setpixel(4, 3, 0);
			break;

		case 4:
			a = Disp[3] & 0x10;
			b = Disp[3] & 0x08;
			c = Disp[3] & 0x04;
			e = Disp[4] & 0x01;

			input_test(F_LINE);	// dummy read
			NOP();
			if (blankEn && (input_test(F_LINE) == 0)) {
				;
			} else {
			if (a) setpixel(0, 3, 1);
			if (b) setpixel(1, 3, 1);
			if (c) setpixel(2, 3, 1);
			if (e) setpixel(4, 4, 1);
			}
			mydelay1(del1);
			if (a) setpixel(0, 3, 0);
			if (b) setpixel(1, 3, 0);
			if (c) setpixel(2, 3, 0);
			if (e) setpixel(4, 4, 0);
			break;
		
		case 5:
			a = Disp[4] & 0x10;
			b = Disp[4] & 0x08;
			c = Disp[4] & 0x04;
			d = Disp[4] & 0x02;

			input_test(F_LINE);	// dummy read
			NOP();
			if (blankEn && (input_test(F_LINE) == 0)) {
				;
			} else {
			if (a) setpixel(0, 4, 1);
			if (b) setpixel(1, 4, 1);
			if (c) setpixel(2, 4, 1);
			if (d) setpixel(3, 4, 1);
			}
			//blank = 0;	/* clear blanking flag at the last row (here) */

			mydelay1(del1);
			if (a) setpixel(0, 4, 0);
			if (b) setpixel(1, 4, 0);
			if (c) setpixel(2, 4, 0);
			if (d) setpixel(3, 4, 0);
			break;
		}
	}
}


int main()
{
	
	avrinit();

	//
	// this is not the official way of figuring out whether the RSTDISBL fuse has been programmed,
	//	but it is a good substitute!
	//	(just don't press the button while powering on)
	//

	// immediately we poll switch - if it "seems" on (i.e. low), then probably it isn't working!  (leave it disabled)
	input_test(F_LINE);	// dummy read
	NOP();
	if (input_test(F_LINE) != 0) {		// enable switch (i.e. F_LINE) if this reads high
		FLineEnabled = 1;
	}


	// debug
	if (FLineEnabled) {
		blinkn(4);
	} else {
		blinkn(3);
	}
	
	//mydelay10(20);

	{
		uint8_t tmp, width;
		uint8_t tmpb;
		uint8_t i, j;
		
		uint8_t buttonevent = 0;
		uint8_t prevbuttonpress = 0;
		uint8_t buttonpress;

		uint8_t msgnum = 0;		// this is "toggled" by a button press
		//uint8_t msglen = 0;

		uint8_t len = 0;

		//
		// change this string!
		// the "str" array below sets the current message.
		// you can use cap letters, numbers, and a limited set of punctuation.
		//
		// check font.c to see all the chars that are available, or possibly add more.
		//
		
		//static const unsigned char str[]   = "I \xF0 OAKLAND  ";
		//static const unsigned char str[]   = "I \xF0 SOLDERING  ";

		static const unsigned char str[]   = "\xF0 QUIETACTION  ";

		uint8_t strlength = strlen((char*) str);			// note: strlen expects char*

		const uint8_t rep = 35;	// adjusts scrolling speed (higher number == slower, 35-40 seems about right)

		while (1)  {  /* infinite loop */

			// toggle message if we see a button event (press)
			if (buttonevent) {
				msgnum = !msgnum;
				len = 0;
				buttonevent = 0;	// clear event flag
			}
			

			//
			// this loads the pixels for a letter (c) into the global FontChar array, and sets FontWidth.
			//
			uint8_t c;
			c = str[len];
			loadfontchar(c);
			
			len++;
	
			if (len >= strlength) {
				len = 0;
			}
			//if (len >= msglen)  len = 0;		// XXX handles 3 chars + 2 spaces
	
			width = FontWidth & 0xf;
	
			for (j = 0; j < width; j++) {		// do "width" times...
				for (i = 0; i < 5; i++) {			// for each row of the display...
					if (FontChar[i] & 0x80) {
						tmpb = 1;
					} else {
						tmpb = 0;
					}
					
					tmp = FontChar[i] << 1;
					FontChar[i] = tmp;
					
					tmp = Disp[i] << 1;
					Disp[i] = tmp;
					if (tmpb) {
						Disp[i]++;
					}
				}
	
				//swapbuffers();
				for (i = 0; i < rep; i++) {
				
					// catch button press "event" (also use Refresh as a "delay")
					if (FLineEnabled) {
						if (input_test(F_LINE) == 0) {
							buttonpress = 1;
						} else {
							buttonpress = 0;
						}
						if (!prevbuttonpress && buttonpress) {		// detect transition from unpressed to pressed
							buttonevent = 1;						// and set event flag
						}
						prevbuttonpress = buttonpress;
					}

					Refresh();  // "refresh" LEDs
					
					// XXX for a button event, may need to wait here until button is released
					if (buttonevent) {
						break;
					}
				}
	
			}
		}
	}

}


//
// set pixel at (x, y) to val (1 = on, 0 = off)
//
//	note:  top left is (0, 0) and bottom right is (4, 4).
//
void setpixel(uint8_t x, uint8_t y, uint8_t val)
{
	uint8_t n, b;
	uint8_t hi, lo;
	
	//n = x + 5*y;
	n = x + (y<<2) + y;		// equivalent to "n = x + 5*y" but avoids mult operation (attiny doesn't have MUL!)
	
	b = LEDS[n];			// get hi,lo nibbles from LED lookup table
	hi = UNPACKH(b);
	lo = UNPACKL(b);

	
	// enable/disable the high pin
	switch (hi) {
	case A:
		if (val) {
			output_enable(A_LINE);
			output_high(A_LINE);
		} else {
			output_low(A_LINE);
			output_disable(A_LINE);
		}
		break;
		
	case B:
		if (val) {
			output_enable(B_LINE);
			output_high(B_LINE);
		} else {
			output_low(B_LINE);
			output_disable(B_LINE);
		}
		break;
		
	case C:
		if (val) {
			output_enable(C_LINE);
			output_high(C_LINE);
		} else {
			output_low(C_LINE);
			output_disable(C_LINE);
		}
		break;
		
	case D:
		if (val) {
			output_enable(D_LINE);
			output_high(D_LINE);
		} else {
			output_low(D_LINE);
			output_disable(D_LINE);
		}
		break;
		
	case E:
		if (val) {
			output_enable(E_LINE);
			output_high(E_LINE);
		} else {
			output_low(E_LINE);
			output_disable(E_LINE);
		}
		break;
		
	case F:
		if (val) {
			output_enable(F_LINE);
			//output_high(F_LINE);
			output_low(F_LINE);			// XXX hack for V0.9 board - F_LINE is ALWAYS low
		} else {
			//output_low(F_LINE);
			output_disable(F_LINE);
		}
		break;
		
	}


	// low pin
	switch (lo) {
	case A:
		if (val) {
			output_enable(A_LINE);
			output_low(A_LINE);
		} else {
			output_disable(A_LINE);
		}
		break;
		
	case B:
		if (val) {
			output_enable(B_LINE);
			output_low(B_LINE);
		} else {
			output_disable(B_LINE);
		}
		break;
		
	case C:
		if (val) {
			output_enable(C_LINE);
			output_low(C_LINE);
		} else {
			output_disable(C_LINE);
		}
		break;
		
	case D:
		if (val) {
			output_enable(D_LINE);
			output_low(D_LINE);
		} else {
			output_disable(D_LINE);
		}
		break;
		
	case E:
		if (val) {
			output_enable(E_LINE);
			output_low(E_LINE);
		} else {
			output_disable(E_LINE);
		}
		break;
#ifdef NOTDEF
	// note: this case does not occur!
	case F:
		if (val) {
			output_enable(F_LINE);
			output_low(F_LINE);
		} else {
			output_disable(F_LINE);
		}
		break;
#endif		
	}
}


/*
 *
 *	low level init needed for AVR.  (attiny25)
 *
 */
static void avrinit(void)
{

	cli();	// needed??
	
	// note: these MUST be in sync with actual hardware!  (also see fivebyfive_iodefs.h)

	// note: DDR pins are set to "1" to be an output, "0" for input.

	// bits   76543210
	PORTB = 0b00000000;			// (see above)
	DDRB  = 0b00000000;			// (see above)

	sei();					// enable interrupts (individual interrupts still need to be enabled)
}


#ifdef NOTDEF
//
//	set up timer1 in "fast PWM" mode 14 (see waveform generation, pg 137 of atmega168/328 doc - doc8271).
//
//	note: this causes TIMER1 to interrupt every 1ms! (TIMER1_OVF)
//	note: it's super important to call this AFTER arduino environment does it's good (and bad)
//		things to various TIMER settings.
//
void start_timer1(void)
{

	// make sure interrupts are off
	cli();

	// disable other timers?  not sure if this is needed XXX
	//TIMSK0 &= ~_BV(TOIE0);
	TIMSK1 &= ~_BV(TOIE1);
	TIMSK2 &= ~_BV(TOIE2);

	// initialize ICR1, which sets the "TOP" value for the counter to interrupt and start over
	// note: value of 125-1 ==> 1khz (assumes 8mhz clock, prescaled by 1/64)

	ICR1 = 125-1;
	OCR1A = 0;		// needed?? (no)
	
	//
	// timer1 config:
	// fast pwm (mode 14) - set WGM13,WGM12,WGM11 bits
	// prescaler clock/64 - set CS11,CS10 bits
	//
	TCCR1A = _BV(WGM11);
	TCCR1B = _BV(WGM13) | _BV(WGM12) | _BV(CS11) | _BV(CS10);

	TCCR1C = 0;	// not needed?

	TIMSK1 = _BV(TOIE1);		// enable timer1 overflow interrupt.. now were running!

	sei();
}
#endif


#ifdef NOTDEF
volatile uint8_t _Toggle = 0;		// debug only - cause LED1 to blink at 1/2 interrupt rate


ISR(TIMER1_OVF_vect)
{
	_Toggle = !_Toggle;
}
#endif

