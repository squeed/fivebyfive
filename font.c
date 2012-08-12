/*
 *	font.c - simple font library
 *
 *	revision history:
 *
 *	- dec 2, 2007 - rolf
 *		created.
 *
 */

#include <inttypes.h>
#include <avr/io.h>			/* this takes care of definitions for our specific AVR */
#include <avr/pgmspace.h>	/* needed for printf_P, etc */
#include <avr/interrupt.h>	/* for interrupts, ISR macro, etc. */
#include <stdio.h>			// for sprintf, etc.
//#include <string.h>			// for strcpy, etc.

#include "font.h"


// globals
uint8_t		FontChar[5];	// temporary buffer for a character data
uint8_t		FontWidth;		// character width

// typing saver
typedef uint16_t uns16;


//
//	loads given character into global font array (and width too)
//
void loadfontchar(unsigned char c)
{
	switch (c) {
	case 'A':
		FontChar[0] = (uns16) 0x06 << 4;
		FontChar[1] = (uns16) 0x09 << 4;
		FontChar[2] = (uns16) 0x0f << 4;
		FontChar[3] = (uns16) 0x09 << 4;
		FontChar[4] = (uns16) 0x09 << 4;
		FontWidth = 5;
		break;
	case 'B':
		FontChar[0] = (uns16) 0x0e << 4;
		FontChar[1] = (uns16) 0x09 << 4;
		FontChar[2] = (uns16) 0x0e << 4;
		FontChar[3] = (uns16) 0x09 << 4;
		FontChar[4] = (uns16) 0x0e << 4;
		FontWidth = 5;
		break;
	case 'C':
		FontChar[0] = (uns16) 0x07 << 4;
		FontChar[1] = (uns16) 0x08 << 4;
		FontChar[2] = (uns16) 0x08 << 4;
		FontChar[3] = (uns16) 0x08 << 4;
		FontChar[4] = (uns16) 0x07 << 4;
		FontWidth = 5;
		break;
	case 'D':
		FontChar[0] = (uns16) 0x0e << 4;
		FontChar[1] = (uns16) 0x09 << 4;
		FontChar[2] = (uns16) 0x09 << 4;
		FontChar[3] = (uns16) 0x09 << 4;
		FontChar[4] = (uns16) 0x0e << 4;
		FontWidth = 5;
		break;
	case 'E':
		FontChar[0] = (uns16) 0x0f << 4;
		FontChar[1] = (uns16) 0x08 << 4;
		FontChar[2] = (uns16) 0x0e << 4;
		FontChar[3] = (uns16) 0x08 << 4;
		FontChar[4] = (uns16) 0x0f << 4;
		FontWidth = 5;
		break;
	case 'F':
		FontChar[0] = (uns16) 0x0f << 4;
		FontChar[1] = (uns16) 0x08 << 4;
		FontChar[2] = (uns16) 0x0e << 4;
		FontChar[3] = (uns16) 0x08 << 4;
		FontChar[4] = (uns16) 0x08 << 4;
		FontWidth = 5;
		break;
	case 'G':
		FontChar[0] = (uns16) 0x07 << 4;
		FontChar[1] = (uns16) 0x08 << 4;
		FontChar[2] = (uns16) 0x0b << 4;
		FontChar[3] = (uns16) 0x09 << 4;
		FontChar[4] = (uns16) 0x06 << 4;
		FontWidth = 5;
		break;
	case 'H':
		FontChar[0] = (uns16) 0x09 << 4;
		FontChar[1] = (uns16) 0x09 << 4;
		FontChar[2] = (uns16) 0x0f << 4;
		FontChar[3] = (uns16) 0x09 << 4;
		FontChar[4] = (uns16) 0x09 << 4;
		FontWidth = 5;
		break;
	case 'I':
		FontChar[0] = (uns16) 0x01 << 7;
		FontChar[1] = (uns16) 0x01 << 7;
		FontChar[2] = (uns16) 0x01 << 7;
		FontChar[3] = (uns16) 0x01 << 7;
		FontChar[4] = (uns16) 0x01 << 7;
		FontWidth = 2;
		break;
	case 'J':
		FontChar[0] = (uns16) 0x01 << 4;
		FontChar[1] = (uns16) 0x01 << 4;
		FontChar[2] = (uns16) 0x01 << 4;
		FontChar[3] = (uns16) 0x09 << 4;
		FontChar[4] = (uns16) 0x06 << 4;
		FontWidth = 5;
		break;
	case 'K':
		FontChar[0] = (uns16) 0x09 << 4;
		FontChar[1] = (uns16) 0x0a << 4;
		FontChar[2] = (uns16) 0x0c << 4;
		FontChar[3] = (uns16) 0x0a << 4;
		FontChar[4] = (uns16) 0x09 << 4;
		FontWidth = 5;
		break;
	case 'L':
		FontChar[0] = (uns16) 0x08 << 4;
		FontChar[1] = (uns16) 0x08 << 4;
		FontChar[2] = (uns16) 0x08 << 4;
		FontChar[3] = (uns16) 0x08 << 4;
		FontChar[4] = (uns16) 0x0f << 4;
		FontWidth = 5;
		break;
	case 'M':
		FontChar[0] = (uns16) 0x011 << 3;
		FontChar[1] = (uns16) 0x01b << 3;
		FontChar[2] = (uns16) 0x015 << 3;
		FontChar[3] = (uns16) 0x011 << 3;
		FontChar[4] = (uns16) 0x011 << 3;
		FontWidth = 6;
		break;
	case 'N':
		FontChar[0] = (uns16) 0x09 << 4;
		FontChar[1] = (uns16) 0x0d << 4;
		FontChar[2] = (uns16) 0x0b << 4;
		FontChar[3] = (uns16) 0x09 << 4;
		FontChar[4] = (uns16) 0x09 << 4;
		FontWidth = 5;
		break;
	case 'O':
	case '0':	// zero
		FontChar[0] = (uns16) 0x06 << 4;
		FontChar[1] = (uns16) 0x09 << 4;
		FontChar[2] = (uns16) 0x09 << 4;
		FontChar[3] = (uns16) 0x09 << 4;
		FontChar[4] = (uns16) 0x06 << 4;
		FontWidth = 5;
		break;
	case 'P':
		FontChar[0] = (uns16) 0x0e << 4;
		FontChar[1] = (uns16) 0x09 << 4;
		FontChar[2] = (uns16) 0x0e << 4;
		FontChar[3] = (uns16) 0x08 << 4;
		FontChar[4] = (uns16) 0x08 << 4;
		FontWidth = 5;
		break;
	case 'Q':
		FontChar[0] = (uns16) 0x0e << 3;
		FontChar[1] = (uns16) 0x11 << 3;
		FontChar[2] = (uns16) 0x11 << 3;
		FontChar[3] = (uns16) 0x12 << 3;
		FontChar[4] = (uns16) 0x0d << 3;
		FontWidth = 6;
		break;
	case 'R':
		FontChar[0] = (uns16) 0x0e << 4;
		FontChar[1] = (uns16) 0x09 << 4;
		FontChar[2] = (uns16) 0x0e << 4;
		FontChar[3] = (uns16) 0x09 << 4;
		FontChar[4] = (uns16) 0x09 << 4;
		FontWidth = 5;
		break;
	case 'S':
		FontChar[0] = (uns16) 0x07 << 4;
		FontChar[1] = (uns16) 0x08 << 4;
		FontChar[2] = (uns16) 0x06 << 4;
		FontChar[3] = (uns16) 0x01 << 4;
		FontChar[4] = (uns16) 0x0e << 4;
		FontWidth = 5;
		break;
	case 'T':
		FontChar[0] = (uns16) 0x01f << 3;
		FontChar[1] = (uns16) 0x04 << 3;
		FontChar[2] = (uns16) 0x04 << 3;
		FontChar[3] = (uns16) 0x04 << 3;
		FontChar[4] = (uns16) 0x04 << 3;
		FontWidth = 6;
		break;
	case 'U':
		FontChar[0] = (uns16) 0x09 << 4;
		FontChar[1] = (uns16) 0x09 << 4;
		FontChar[2] = (uns16) 0x09 << 4;
		FontChar[3] = (uns16) 0x09 << 4;
		FontChar[4] = (uns16) 0x06 << 4;
		FontWidth = 5;
		break;
	case 'V':
		FontChar[0] = (uns16) 0x011 << 3;
		FontChar[1] = (uns16) 0x011 << 3;
		FontChar[2] = (uns16) 0x011 << 3;
		FontChar[3] = (uns16) 0x0a << 3;
		FontChar[4] = (uns16) 0x04 << 3;
		FontWidth = 6;
		break;
	case 'W':
		FontChar[0] = (uns16) 0x015 << 3;
		FontChar[1] = (uns16) 0x015 << 3;
		FontChar[2] = (uns16) 0x015 << 3;
		FontChar[3] = (uns16) 0x015 << 3;
		FontChar[4] = (uns16) 0x00a << 3;
		FontWidth = 6;
		break;
	case 'X':
		FontChar[0] = (uns16) 0x011 << 3;
		FontChar[1] = (uns16) 0x0a << 3;
		FontChar[2] = (uns16) 0x04 << 3;
		FontChar[3] = (uns16) 0x0a << 3;
		FontChar[4] = (uns16) 0x011 << 3;
		FontWidth = 6;
		break;
	case 'Y':
		FontChar[0] = (uns16) 0x011 << 3;
		FontChar[1] = (uns16) 0x0a << 3;
		FontChar[2] = (uns16) 0x04 << 3;
		FontChar[3] = (uns16) 0x04 << 3;
		FontChar[4] = (uns16) 0x04 << 3;
		FontWidth = 6;
		break;
	case 'Z':
		FontChar[0] = (uns16) 0x0f << 4;
		FontChar[1] = (uns16) 0x01 << 4;
		FontChar[2] = (uns16) 0x06 << 4;
		FontChar[3] = (uns16) 0x08 << 4;
		FontChar[4] = (uns16) 0x0f << 4;
		FontWidth = 5;
		break;
	case ' ':
		FontChar[0] = (uns16) 0x0;
		FontChar[1] = (uns16) 0x0;
		FontChar[2] = (uns16) 0x0;
		FontChar[3] = (uns16) 0x0;
		FontChar[4] = (uns16) 0x0;
		FontWidth = 4;
		break;
	case '!':
		FontChar[0] = (uns16) 0x01 << 7;
		FontChar[1] = (uns16) 0x01 << 7;
		FontChar[2] = (uns16) 0x01 << 7;
		FontChar[3] = (uns16) 0x00 << 7;
		FontChar[4] = (uns16) 0x01 << 7;
		FontWidth = 4;
		break;
	case '\'':
		FontChar[0] = (uns16) 0x01 << 7;
		FontChar[1] = (uns16) 0x01 << 7;
		FontChar[2] = (uns16) 0x00 << 7;
		FontChar[3] = (uns16) 0x00 << 7;
		FontChar[4] = (uns16) 0x00 << 7;
		FontWidth = 2;
		break;
	case ':':
		FontChar[0] = (uns16) 0x00 << 7;
		FontChar[1] = (uns16) 0x01 << 7;
		FontChar[2] = (uns16) 0x00 << 7;
		FontChar[3] = (uns16) 0x01 << 7;
		FontChar[4] = (uns16) 0x00 << 7;
		FontWidth = 2;
		break;
#ifdef NOTDEF
	case '(':		// old one - too close to next char
		FontChar[0] = (uns16) 0x01 << 4;
		FontChar[1] = (uns16) 0x02 << 4;
		FontChar[2] = (uns16) 0x02 << 4;
		FontChar[3] = (uns16) 0x02 << 4;
		FontChar[4] = (uns16) 0x01 << 4;
		FontWidth = 4;
		break;
#endif
	case '(':
		FontChar[0] = (uns16) 0x01 << 5;
		FontChar[1] = (uns16) 0x02 << 5;
		FontChar[2] = (uns16) 0x02 << 5;
		FontChar[3] = (uns16) 0x02 << 5;
		FontChar[4] = (uns16) 0x01 << 5;
		FontWidth = 4;
		break;
	case ')':
		FontChar[0] = (uns16) 0x02 << 6;
		FontChar[1] = (uns16) 0x01 << 6;
		FontChar[2] = (uns16) 0x01 << 6;
		FontChar[3] = (uns16) 0x01 << 6;
		FontChar[4] = (uns16) 0x02 << 6;
		FontWidth = 4;
		break;
	case ',':
		FontChar[0] = (uns16) 0x00 << 6;
		FontChar[1] = (uns16) 0x00 << 6;
		FontChar[2] = (uns16) 0x00 << 6;
		FontChar[3] = (uns16) 0x01 << 6;
		FontChar[4] = (uns16) 0x02 << 6;
		FontWidth = 3;
		break;
	case '.':		// 0x2e
		FontChar[0] = (uns16) 0x00 << 7;
		FontChar[1] = (uns16) 0x00 << 7;
		FontChar[2] = (uns16) 0x00 << 7;
		FontChar[3] = (uns16) 0x00 << 7;
		FontChar[4] = (uns16) 0x01 << 7;
		FontWidth = 4;
		break;
#ifndef OPT_FONTS
	case 0xc9:		// ... (ellipses)
		FontChar[0] = (uns16) 0x00 << 3;
		FontChar[1] = (uns16) 0x00 << 3;
		FontChar[2] = (uns16) 0x00 << 3;
		FontChar[3] = (uns16) 0x00 << 3;
		FontChar[4] = (uns16) 0x15 << 3;
		FontWidth = 6;
		break;
#endif
	case '1':	// 1
		FontChar[0] = (uns16) 0x02 << 4;
		FontChar[1] = (uns16) 0x06 << 4;
		FontChar[2] = (uns16) 0x02 << 4;
		FontChar[3] = (uns16) 0x02 << 4;
		FontChar[4] = (uns16) 0x07 << 4;
		FontWidth = 5;
		break;
	case '2':
		FontChar[0] = (uns16) 0x0e << 4;
		FontChar[1] = (uns16) 0x01 << 4;
		FontChar[2] = (uns16) 0x06 << 4;
		FontChar[3] = (uns16) 0x08 << 4;
		FontChar[4] = (uns16) 0x0f << 4;
		FontWidth = 5;
		break;
	case '3':
		FontChar[0] = (uns16) 0x0e << 4;
		FontChar[1] = (uns16) 0x01 << 4;
		FontChar[2] = (uns16) 0x06 << 4;
		FontChar[3] = (uns16) 0x01 << 4;
		FontChar[4] = (uns16) 0x0e << 4;
		FontWidth = 5;
		break;
	case '4':
		FontChar[0] = (uns16) 0x09 << 4;
		FontChar[1] = (uns16) 0x09 << 4;
		FontChar[2] = (uns16) 0x0f << 4;
		FontChar[3] = (uns16) 0x01 << 4;
		FontChar[4] = (uns16) 0x01 << 4;
		FontWidth = 5;
		break;
	case '5':
		FontChar[0] = (uns16) 0x0f << 4;
		FontChar[1] = (uns16) 0x08 << 4;
		FontChar[2] = (uns16) 0x0e << 4;
		FontChar[3] = (uns16) 0x01 << 4;
		FontChar[4] = (uns16) 0x0e << 4;
		FontWidth = 5;
		break;
	case '6':
		FontChar[0] = (uns16) 0x06 << 4;
		FontChar[1] = (uns16) 0x08 << 4;
		FontChar[2] = (uns16) 0x0e << 4;
		FontChar[3] = (uns16) 0x09 << 4;
		FontChar[4] = (uns16) 0x06 << 4;
		FontWidth = 5;
		break;
	case '7':
		FontChar[0] = (uns16) 0x0f << 4;
		FontChar[1] = (uns16) 0x01 << 4;
		FontChar[2] = (uns16) 0x02 << 4;
		FontChar[3] = (uns16) 0x04 << 4;
		FontChar[4] = (uns16) 0x08 << 4;
		FontWidth = 5;
		break;
	case '8':
		FontChar[0] = (uns16) 0x06 << 4;
		FontChar[1] = (uns16) 0x09 << 4;
		FontChar[2] = (uns16) 0x06 << 4;
		FontChar[3] = (uns16) 0x09 << 4;
		FontChar[4] = (uns16) 0x06 << 4;
		FontWidth = 5;
		break;
	case '9':
		FontChar[0] = (uns16) 0x06 << 4;
		FontChar[1] = (uns16) 0x09 << 4;
		FontChar[2] = (uns16) 0x07 << 4;
		FontChar[3] = (uns16) 0x01 << 4;
		FontChar[4] = (uns16) 0x06 << 4;
		FontWidth = 5;
		break;
	case '?':
		FontChar[0] = (uns16) 0x0e << 4;
		FontChar[1] = (uns16) 0x01 << 4;
		FontChar[2] = (uns16) 0x06 << 4;
		FontChar[3] = (uns16) 0x00 << 4;
		FontChar[4] = (uns16) 0x04 << 4;
		FontWidth = 5;
		break;
#ifndef OPT_FONTS
	case 0xF0:		// XXX	(heart - normally apple logo)
		FontChar[0] = (uns16) 0x36 << 1;
		FontChar[1] = (uns16) 0x49 << 1;
		FontChar[2] = (uns16) 0x22 << 1;
		FontChar[3] = (uns16) 0x14 << 1;
		FontChar[4] = (uns16) 0x08 << 1;
		FontWidth = 8;
		break;
#endif
	default:	// badchar
#ifndef OPT_FONTS
		FontChar[0] = (uns16) 0x15 << 3;
		FontChar[1] = (uns16) 0x0a << 3;
		FontChar[2] = (uns16) 0x15 << 3;
		FontChar[3] = (uns16) 0x0a << 3;
		FontChar[4] = (uns16) 0x15 << 3;
		FontWidth = 6;
#endif
		break;
	}
}
