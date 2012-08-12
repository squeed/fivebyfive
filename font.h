/*
 *	font.h - simple font library
 *
 *	revision history:
 *
 *	- dec 2, 2007 - rolf
 *		created.
 *
 */


extern	uint8_t		FontChar[5];	// temporary buffer for a character data
extern	uint8_t		FontWidth;		// character width

void loadfontchar(unsigned char c);
