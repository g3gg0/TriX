/* bigd.h */

/* 
Interface to BIGD library.

Multiple-precision arithmetic using non-negative numbers
and memory management.
*/
/******************** SHORT COPYRIGHT NOTICE**************************
This source code is part of the BigDigits multiple-precision
arithmetic library Version 2.0 originally written by David Ireland,
copyright (c) 2001-5 D.I. Management Services Pty Limited, all rights
reserved. It is provided "as is" with no warranties. You may use
this software under the terms of the full copyright notice
"bigdigitsCopyright.txt" that should have been included with this
library or can be obtained from <www.di-mgt.com.au/bigdigits.html>.
This notice must always be retained in any copy.
******************* END OF COPYRIGHT NOTICE***************************/
/*
    Last updated:
    $Date: 2006/04/12 21:09:13 $
    $Revision: 1.1 $
	$Author: geggo $
*/

#ifndef BIGD_H_
#define BIGD_H_ 1

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

/*
This interface uses opaque pointers of type BIGD using
the conventions in "C Interfaces and Implementions" by
David R. Hanson, Addison-Wesley, 1996, pp21-4.
*/
#define T BIGD
typedef struct T *T;

/* DIGIT_T type is not exposed by this library so we expose 
   a synonym `bdigit_t' for a single digit */
typedef unsigned long bdigit_t;

#define OCTETS_PER_DIGIT (sizeof(bdigit_t))
/* Options for bdPrint */
#define BD_PRINT_NL   0x1	/* append a newline after printing */
#define BD_PRINT_TRIM 0x2	/* trim leading zero digits */


/* PROGRAMMER'S NOTES
Where the function computes a new BIGD value,
the result is returned in the first argument.
Some functions do not allow variables to overlap.
Functions of type int generally return 0 to denote success 
but some return True/False (1/0) or borrow (+1) or error (-1).
Functions of type size_t (an unsigned int) return a length.

Memory is allocated if the function might need it. 
It is only released when freed with bdFree(), e.g.:

	BIGD b;
	b = bdNew();
	...
	bdFree(&b);
*/

/******************************/
/* CONSTRUCTOR AND DESTRUCTOR */
/******************************/

BIGD bdNew(void);
	/* Return handle to new BIGD object */

void bdFree(BIGD *bd);	
	/* Zeroise and free BIGD memory
	   - NB pass a pointer to the handle */

/*************************/
/* ARITHMETIC OPERATIONS */
/*************************/

int bdAdd(BIGD w, BIGD u, BIGD v);
	/* Compute w = u + v, w#v */

int bdSubtract(BIGD w, BIGD u, BIGD v);
	/* Compute w = u - v, return borrow, w#v */

int bdMultiply(BIGD w, BIGD u, BIGD v);
	/* Compute w = u * v, w#u */

int bdDivide(BIGD q, BIGD r, BIGD u, BIGD v);
	/* Computes quotient q = u / v and remainder r = u mod v 
	   Trashes q and r first. q#(u,v), r#(u,v) */

int bdModulo(BIGD r, BIGD u, BIGD v);
	/* Computes r = u mod v, r#u */

int bdSquare(BIGD w, BIGD x);
	/* Computes w = x^2, w#x */

int bdIncrement(BIGD a);
	/* Sets a = a + 1, returns carry */

int bdDecrement(BIGD a);
	/* Sets a = a - 1, returns borrow */

/* 'Safe' versions with no restrictions on overlap */
int bdAddEx(BIGD w, BIGD u, BIGD v);
int bdSubtractEx(BIGD w, BIGD u, BIGD v);
int bdMultiplyEx(BIGD w, BIGD u, BIGD v);
int bdDivideEx(BIGD q, BIGD r, BIGD u, BIGD v);
int bdModuloEx(BIGD r, BIGD u, BIGD v);
int bdSquareEx(BIGD w, BIGD x);

/*************************/
/* COMPARISON OPERATIONS */
/*************************/

int bdIsEqual(BIGD a, BIGD b);
	/* Returns true if a == b, else false */

int bdCompare(BIGD a, BIGD b);
	/* Returns sign of (a-b) */

int bdIsZero(BIGD a);
	/* Returns true if a == 0, else false */

int bdIsEven(BIGD a);
int bdIsOdd(BIGD a);
	/* Return TRUE or FALSE */

/*************************/
/* ASSIGNMENT OPERATIONS */
/*************************/

int bdSetEqual(BIGD a, BIGD b);
	/* Sets a = b */

int bdSetZero(BIGD a);
	/* Sets a = 0 */

/****************************/
/* NUMBER THEORY OPERATIONS */
/****************************/

int bdModExp(BIGD y, BIGD x, BIGD e, BIGD m);
	/* Compute y = x^e mod m */

int bdModMult(BIGD a, BIGD x, BIGD y, BIGD m);
	/* Compute a = (x * y) mod m */

int bdModInv(BIGD x, BIGD a, BIGD m);
	/* Compute x = a^-1 mod m */

int bdGcd(BIGD g, BIGD x, BIGD y);
	/* Compute g = gcd(x, y) */

int bdIsPrime(BIGD b, size_t ntests);
	/* Returns true if passes ntests x Miller-Rabin tests with trial division, b > 2 */

int bdRabinMiller(BIGD b, size_t ntests);
	/* Returns true if passes ntests x Miller-Rabin tests without trial division, b > 2 */

/**********************************************/
/* FUNCTIONS THAT OPERATE WITH A SINGLE DIGIT */
/**********************************************/

int bdSetShort(BIGD b, bdigit_t d);
	/* Converts single digit d into a BIGD b */

int bdShortAdd(BIGD w, BIGD u, bdigit_t d);
	/* Compute w = u + d */

int bdShortSub(BIGD w, BIGD u, bdigit_t d);
	/* Compute w = u - d, return borrow */

int bdShortMult(BIGD w, BIGD u, bdigit_t d);
	/* Compute w = u * d */

int bdShortDiv(BIGD q, BIGD r, BIGD u, bdigit_t d);
	/* Computes quotient q = u / d and remainder r = u mod d */

bdigit_t bdShortMod(BIGD r, BIGD u, bdigit_t d);
	/* Computes r = u mod d */

int bdShortCmp(BIGD a, bdigit_t d);
	/* Returns sign of (a-d) */

/***********************/
/* BIT-WISE OPERATIONS */
/***********************/

size_t bdBitLength(BIGD bd);
	/* Returns number of significant bits in bd */

int bdSetBit(BIGD a, size_t n, int value);
	/* Set bit n (0..nbits-1) with value 1 or 0 */

int bdGetBit(BIGD a, size_t ibit);
	/* Returns value 1 or 0 of bit n (0..nbits-1) */

int bdShiftLeft(BIGD a, BIGD b, size_t n);
	/* Computes a = b << n, n < BITS_PER_DIGIT */

int bdShiftRight(BIGD a, BIGD b, size_t n);
	/* Computes a = b >> n, n < BITS_PER_DIGIT */

/*******************/
/* MISC OPERATIONS */
/*******************/

size_t bdSizeof(BIGD b);
	/* Returns number of significant digits in b */

void bdPrint(BIGD bd, size_t flags);
	/* Print bd to stdout (see flags above) */

/***************/
/* CONVERSIONS */
/***************/

/* NB `octet' = an 8-bit byte */

size_t bdConvFromOctets(BIGD b, const unsigned char *c, size_t nbytes);
	/* Converts nbytes octets into big digit b, resizing if necessary.
	   Returns number of significant digits actually set. */

size_t bdConvToOctets(BIGD b, unsigned char *c, size_t nbytes);
	/* Convert big digit b into string of octets, in big-endian order,
	   padding to nbytes or truncating if necessary.
	   Returns number of significant bytes required for c. 
	   If c is NULL or nbytes == 0 then just return required size. */

size_t bdConvFromHex(BIGD b, const char *s);
	/* Convert string s of hex chars into big digit b. 
	   Returns number of significant digits actually set or 0 if error. */

size_t bdConvToHex(BIGD b, char *s, size_t smax);
	/* Convert big digit b into string of hex characters
	   where s has size smax including the terminating zero.
	   Returns number of chars required for s excluding leading zeroes. 
	   If s is NULL or smax == 0 then just return required size. */

size_t bdConvFromDecimal(BIGD b, const char *s);
	/* Convert string s of decimal chars into big digit b. 
	   Returns number of significant digits actually set or 0 if error. */

size_t bdConvToDecimal(BIGD b, char *s, size_t smax);
	/* Convert big digit b into string of decimal characters
	   where s has size smax including the terminating zero.
	   Returns number of chars required for s excluding leading zeroes. 
	   If s is NULL or smax == 0 then just return required size. */

/* Note that the bdConvToHex and bdConvToDecimal functions return the 
   total length of the string they tried to create */

/****************************/
/* RANDOM NUMBER OPERATIONS */
/****************************/

bdigit_t bdRandDigit(void);
	/* Generate a pseudo-random digit. Not thread-safe. */

size_t bdSetRandTest(BIGD a, size_t ndigits);
	/* Fill a with _up to_ ndigits pseudo-random digits. Not thread safe. */

/* TYPEDEF for user-defined random byte generator function */
typedef int (* BD_RANDFUNC)(unsigned char *buf, size_t nbytes, unsigned char *seed, size_t seedlen);

int bdRandomSeeded(BIGD a, size_t nbits, unsigned char *seed, 
	size_t seedlen, BD_RANDFUNC RandFunc);
	/* Generate a random number nbits long using RandFunc */

int bdGeneratePrime(BIGD b, size_t nbits, size_t ntests, unsigned char *seed, 
	size_t seedlen, BD_RANDFUNC RandFunc);
	/* Generate a prime number nbits long; carry out ntests x primality tests */

#undef T /* (for opaque BIGD pointer) */

#ifdef __cplusplus
}
#endif

#endif /* BIGD_H_ */
