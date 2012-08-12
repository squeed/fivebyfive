/*
 * fivebyfive.h - various definitions for this project
 *
 * Copyright (c) 2011 - Rolf van WidenfeltSome rights reserved.
 *
 * Note: This source code is licensed under a Creative Commons License, CC-by-nc-sa.
 *		(attribution, non-commercial, share-alike)
 *  	see http://creativecommons.org/licenses/by-nc-sa/3.0/ for details.
 *
 *	revision history:
 *
 *	12/19/2011 - rolf
 *		added NOP() macro
 *
 */

#ifndef fivebyfive_h
#define fivebyfive_h


#include <inttypes.h>

/* CPU frequency (8000000UL == 8 MHz) */
#define F_CPU 8000000UL


/* public constants go here */
// XXX none!

/* NOP (adds a 1 cycle delay to your code) */
#define NOP()	__asm__ volatile("nop"::)


#endif /* fivebyfive_h */
