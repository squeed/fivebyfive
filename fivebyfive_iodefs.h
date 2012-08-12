/*
 *	fivebyfive_iodefs.h - project-specific I/O pin definitions and some macros/functions to go with them
 *
 *		the inline functions, output_high() and output_low(), allow a simple syntax for
 *		setting/clearing I/O pins without having to remember which port (e.g. PORTA, PORTB, etc)
 *		the pin is assigned to.
 *		similarly, the inline function input_test() allows reading a pin.
 *
 *		for example, to set the SPKR I/O pin high (PB1), we do:
 *			output_high(SPKR);
 *
 *		because these are inline functions, they generate efficient code.  (1 instruction)
 *
 *		note: each I/O pin still needs to be configured as input or output.
 *		this is typically done in the avrinit() function.
 *
 *
 *	author: rolf van widenfelt (rolfvw _at_ pizzicato _dot_ com)
 *
 *
 *	specific hardware setup:
 *
 *	- FiveByFive V.1 (attiny25)
 *
 *	revision history:
 *
 *	- nov 20, 2011 - rolf
 *		add functions output_enable(pin) and output_disable(pin), since
 *		we are going to be doing a lot of setting pins to tristate (disabled).
 *
 *	- nov 19, 2011 - rolf
 *		created.
 *
 */

//
// note: simply add 0,8,16 or 24 for PORTA,B,C or D respectively, to the pin define for this AVR pin

//#define _PA		0	/* no PORTA on attiny25 */
//#define _PC		16	/* no PORTC on attiny25 */
//#define _PD		24	/* no PORTD on attiny25 */

#define _PB		8		/* for PORTB */


//
// project-specific pin definitions go here.
//
// note: RxD (PD0) is used for the UART (as an input) and not available as an IO pin
//
// note: changes here MUST be consistent with "init" code!  (see PORTn and DDRn settings)
//

#define A_LINE		_PB+PB0		/* also MOSI */
#define B_LINE		_PB+PB1		/* also MISO */
#define C_LINE		_PB+PB2		/* also SCK */
#define D_LINE		_PB+PB3
#define E_LINE		_PB+PB4
#define F_LINE		_PB+PB5		/* also RESET* */
#define NC1			_PB+PB6		/* unused */
#define NC2			_PB+PB7		/* unused */


//
// end of project-specific pin definitions.
//


//
// private macros!
// this is the recommended way to do a bit set/clear operation in avr-gcc
// but this macro should only be used by the inline functions below.
//

#define _output_high(port,bit)	port |= _BV(bit)
#define _output_low(port,bit)	port &= ~_BV(bit)


#define _input_test(port,bit)	(port & _BV(bit))

//
// these inline functions will generate efficient code for I/O port bit set and clear operations.
//
// this determines the actual port by examining the pin number (e.g. PORTA is 0 - 7, PORTB is 8-15, etc)
//

static inline void output_high(unsigned char pin)
{
#ifdef NOTDEF
	if (pin < 8) {
		_output_high(PORTA,pin);
	} else
#endif
	if (pin >= 8 && pin < 16) {
		_output_high(PORTB,(pin-8));
#ifdef NOTDEF
	} else if (pin >= 16 && pin < 24) {
		_output_high(PORTC,(pin-16));
	} else if (pin >= 24 && pin < 32) {
		_output_high(PORTD,(pin-24));
#endif
	}
}

static inline void output_low(unsigned char pin)
{
#ifdef NOTDEF
	if (pin < 8) {
		_output_low(PORTA,pin);
	} else
#endif
	if (pin >= 8 && pin < 16) {
		_output_low(PORTB,(pin-8));
#ifdef NOTDEF
	} else if (pin >= 16 && pin < 24) {
		_output_low(PORTC,(pin-16));
	} else if (pin >= 24 && pin < 32) {
		_output_low(PORTD,(pin-24));
#endif
	}
}


static inline void output_enable(unsigned char pin)
{
#ifdef NOTDEF
	if (pin < 8) {
		_output_high(DDRA,pin);
	} else
#endif
	if (pin >= 8 && pin < 16) {
		_output_high(DDRB,(pin-8));
#ifdef NOTDEF
	} else if (pin >= 16 && pin < 24) {
		_output_high(DDRC,(pin-16));
	} else if (pin >= 24 && pin < 32) {
		_output_high(DDRD,(pin-24));
#endif
	}
}

//
// note: in other words, set this pin to be an input
//
static inline void output_disable(unsigned char pin)
{
#ifdef NOTDEF
	if (pin < 8) {
		_output_low(DDRA,pin);
	} else
#endif
	if (pin >= 8 && pin < 16) {
		_output_low(DDRB,(pin-8));
#ifdef NOTDEF
	} else if (pin >= 16 && pin < 24) {
		_output_low(DDRC,(pin-16));
	} else if (pin >= 24 && pin < 32) {
		_output_low(DDRD,(pin-24));
#endif
	}
}


//
//	returns the value of the input pin (but does not shift it!)
//
//	note: the most efficient code is generated when testing the return value against 0.
//
static inline unsigned char input_test(unsigned char pin)
{
#ifdef NOTDEF
	if (pin < 8) {
		return _input_test(PINA,pin);
	} else
#endif
	if (pin >= 8 && pin < 16) {
		return _input_test(PINB,(pin-8));
#ifdef NOTDEF
	} else if (pin >= 16 && pin < 24) {
		return _input_test(PINC,(pin-16));
	} else if (pin >= 24 && pin < 32) {
		return _input_test(PIND,(pin-24));
#endif
	} else {
		return 0;	/* notreached */
	}
}
