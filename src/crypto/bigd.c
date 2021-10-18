/* bigd.c */

/* Wrapper functions around "bigdigits" with memory management */

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
    $Date: 2007-04-09 20:26:09 +0200 (Mo, 09 Apr 2007) $
    $Revision: 2525 $
	$Author: geggo $
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "bigd.h"
#include "bigdigits.h"

#include "defines.h"
#include "mem.h"

#define T BIGD

struct T
{
	DIGIT_T *digits;	/* Ptr to array of digits, least sig. first */
	size_t ndigits;		/* No of non-zero significant digits */
	size_t maxdigits;	/* Max size allocated */
};

/*
All these functions MUST make sure that there are always
enough digits before doing anything, and SHOULD reset <ndigits>
afterwards to reflect the final significant size.
<maxdigits> is the size allocated.
<ndigits> may be zero.
<ndigits> may be too long if MS digits compute to zero so
consider it a limit on significant digits, not gospel.

It is an error to pass a NULL BIGD parameter except to bdFree.
*/

#ifdef _DEBUG
static int debug = 0; /* <= change this to > 0 for console debugging */
#else
static int debug = 0; /* <= ALWAYS ZERO */
#endif

/* Useful definitions */
#ifndef FALSE
#define FALSE               0
#endif
#ifndef TRUE
#define TRUE                1
#endif
#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif


T bdNew(void)
{
	struct T *p;
	p = (struct T*)calloc(1, (long)sizeof(*p));
	if (!p)
	{
		mpFail("bdNew: Failed to calloc memory.");
	}
	copyright_notice();
	/* set up with single zero digit */
	p->digits = mpAlloc(1);
	p->digits[0] = 0;
	p->ndigits = 0;
	p->maxdigits = 1;
	return p;
}

void bdFree(T *p)
/* Zeroise and free memory. Set ptr to NULL. */
{
	T bd = *p;
	if (*p)
	{
		/* Zeroise them all, just in case */
		if (bd->digits)
		{
			mpSetZero(bd->digits, bd->maxdigits);
			free(bd->digits);
		}
		bd->maxdigits = 0;
		bd->ndigits = 0;
		free(*p);
	}
	*p = NULL;
}

static int bd_resize(T b, size_t newsize)
{
/*
Internal fn to re-size a BIGD structure before a calc.
Use carefully!
1. If growing, it allocs more digits and increases maxdigits
2. If shrinking, it decreases ndigits and zeroises the excess.
3. It does not increase b->ndigits; that's up to you later.
4. It does not release excess digits; use bdFree.

In other words, it's like middle-aged spread:
you go from a 32" waist to a 38 but can never go backwards.

Be careful doing the following:-
	n = new_size_we_expect;
	bd_resize(b, n);
	mpFunctionOfSorts(b->digits, n);
	b->ndigits = mpSizeof(b->digits, b->ndigits); // NO!

b->ndigits may be set too short

Better:
	n = new_size_we_expect;
	bd_resize(b, n);
	mpFunctionOfSorts(b->digits, n);
	b->ndigits = mpSizeof(b->digits, n);  // Yes.

*/

	size_t i;

	/* Check just in case NULL */
	assert(b);

	/* If we are shrinking, clear high digits */
	if (newsize < b->ndigits)
	{
		for (i = newsize; i < b->ndigits; i++)
			b->digits[i] = 0;
		b->ndigits = newsize;
		return 0;
	}

	/* We need more room */
	if (b->maxdigits < newsize)
	{
		/* Increase size of digit array */
		b->digits = (DIGIT_T *)realloc(b->digits, newsize * sizeof(DIGIT_T));

		/* Check for failure */
		if (!b->digits)
		{
			mpFail("bd_resize: Failed to realloc memory.");
		}

		b->maxdigits = newsize;	/* Remember new max */
	}

	/* Make sure new digits are zero */
	for (i = b->ndigits; i < newsize; i++)
		b->digits[i] = 0;

	return 0;
}

size_t bdConvFromOctets(T b, const unsigned char *c, size_t nbytes)
/* Converts nbytes octets into big digit b, resizing if necessary */
{
	size_t ndigits, n;
	if ( !c )
		return 0;

	assert(b);
	ndigits = (nbytes + OCTETS_PER_DIGIT - 1) / OCTETS_PER_DIGIT;

	bd_resize(b, ndigits);

	n = mpConvFromOctets(b->digits, ndigits, c, nbytes);
	b->ndigits = mpSizeof(b->digits, n);

	return n;
}

size_t bdConvToOctets(T b, unsigned char *c, size_t nbytes)
/* Convert big digit b into string of octets, in big-endian order,
   padding to nbytes or truncating if necessary.
   Returns # significant bytes.
   If c is NULL or nbytes == 0 then just return required size.
*/
{
	size_t noctets, nbits, n;

	if ( !c )
		return 0;
	assert(b);

	nbits = mpBitLength(b->digits, b->ndigits);
	noctets = (nbits + 7) / 8;

	if (!c || 0 == nbytes)
	{
		return noctets;
	}

	n = mpConvToOctets(b->digits, b->ndigits, c, nbytes);

	return noctets;
}

size_t bdConvFromHex(T b, const char *s)
/* Converts a hex string into big digit b */
{
	unsigned char *data = NULL;
	int length = 0;

	if ( !s )
		return 0;

	length = strlen ( s ) / 2;
	data = (unsigned char*)util_hexunpack ( s, &length );

	if ( !data )
		return 0;

	bdConvFromOctets ( b, data, length );
	free ( data );

	return length;
}

size_t bdConvToHex(T b, char *s, size_t smax)
{
	int bytes = 0;
	unsigned char *string = NULL;
	unsigned char *buffer = NULL;
	assert(b);
	/*
	return mpConvToHex(b->digits, b->ndigits, s, smax);
	*/
	if ( !s )
		return 0;

	bytes = b->ndigits * 4;
	buffer = (unsigned char*)malloc ( (bytes * 2) +1 );
	bdConvToOctets ( b, buffer, bytes );

	string = (unsigned char*)util_hexpack ( buffer, bytes );

	if ( strlen ( string ) < smax )
		strcpy ( s, string );
	free ( string );
	free ( buffer );

	return bytes;
}

size_t bdConvFromDecimal(BIGD b, const char *s)
{
	size_t ndigits, n;

	if ( !s )
		return 0;
	assert(b);
	/* approx size but never too small */
	ndigits = (strlen(s) / 2 + OCTETS_PER_DIGIT) / OCTETS_PER_DIGIT;
	bd_resize(b, ndigits);

	n = mpConvFromDecimal(b->digits, ndigits, s);
	b->ndigits = n;

	return n;
}

size_t bdConvToDecimal(BIGD b, char *s, size_t smax)
{
	assert(b);
	return mpConvToDecimal(b->digits, b->ndigits, s, smax);
}

int bdSetShort(T b, unsigned long value)
	/* Converts value into a (single-digit) big digit b */
{
	assert(b);
	bd_resize(b, 1);
	b->digits[0] = (DIGIT_T)value;
	b->ndigits = (value ? 1 : 0);
	return 0;
}

size_t bdBitLength(T b)
	/* Returns base-1 index to most significant bit in b */
{
	assert(b);
	return mpBitLength(b->digits, b->ndigits);
}

size_t bdSizeof(T b)
	/* Returns number of significant non-zero bytes in b */
{
	assert(b);
	return mpSizeof(b->digits, b->ndigits);
}

/* Print function for bigdigit_t structures */

void bdPrint(T p, size_t flags)
{
	size_t n;

	assert(p);
	n = p->ndigits;
	if (n == 0) n = 1;

	if (flags & BD_PRINT_TRIM)	/* Trim leading zeroes */
	{
		if (flags & BD_PRINT_NL)	/* add newlines */
			mpPrintTrimNL(p->digits, n);
		else
			mpPrintTrim(p->digits, n);
	}
	else
	{
		if (flags & BD_PRINT_NL)	/* add newlines */
			mpPrintNL(p->digits, n);
		else
			mpPrint(p->digits, n);
	}
}

int bdIsEqual(T a, T b)
{
	/*	Returns true if a == b, else false */
	size_t n, na, nb;

	assert(a && b);
	/* We can't trust ndigits */
	na = mpSizeof(a->digits, a->ndigits);
	nb = mpSizeof(b->digits, b->ndigits);

	if (na != nb)
		return FALSE;
	if (na == 0 && nb == 0)
		return TRUE;

	/* Otherwise we have equal lengths */
	n = na;
	while (n--)
	{
		if (a->digits[n] != b->digits[n])
			return FALSE;
	}

	return TRUE;
}

int bdIsZero(T a)
	/* Returns true if a == 0, else false */
{
	assert(a);
	return mpIsZero(a->digits, a->ndigits);
}

int bdShortCmp(T a, unsigned long d)
	/* Returns sign of (a-d) */
{
	assert(a);
	return mpShortCmp(a->digits, d, a->ndigits);
}

int bdCompare(T a, T b)
	/*	Returns sign of (a-b) */
{
	size_t n, na, nb;

	assert(a && b);
	if (a->ndigits != b->ndigits)
	{
		na = mpSizeof(a->digits, a->ndigits);
		nb = mpSizeof(b->digits, b->ndigits);
		if (na > nb) return 1;
		if (na < nb) return -1;
		n = na;
	}
	else
		n = a->ndigits;

	return mpCompare(a->digits, b->digits, n);
}

int bdIsEven(T a)
{
	assert(a);
	return ISEVEN(a->digits[0]);
}

int bdIsOdd(T a)
{
	assert(a);
	return ISODD(a->digits[0]);
}


int bdSetEqual(T a, T b)
	/*	Sets a = b */
{
	assert(a && b);
	bd_resize(a, b->ndigits);
	mpSetEqual(a->digits, b->digits, b->ndigits);
	a->ndigits = b->ndigits;
	return 0;
}

int bdSetZero(T a)
	/* Sets a = 0 */
{
	assert(a);
	mpSetZero(a->digits, a->ndigits);
	a->ndigits = 0;
	return 0;
}

int bdShortAdd(T w, T u, unsigned long d)
	/* Compute w = u + d,
	returns 1 if we had a carry */
{
	DIGIT_T carry;
	size_t dig_size = max(u->ndigits, 1);

	assert(w && u);
	bd_resize(w, dig_size + 1);

	carry = mpShortAdd(w->digits, u->digits, d, dig_size);

	/* Cope with overflow */
	if (carry)
	{
		w->digits[dig_size] = carry;
		w->ndigits = dig_size + 1;
	}
	else
		w->ndigits = dig_size;

	return carry;
}

int bdAdd(T w, T u, T v)
	/* Compute w = u + v, w#v*/
{
	size_t dig_size;
	DIGIT_T carry;

	assert(w && u && v);
	/* Check for cheaper option */
	if (v->ndigits == 1)
		return bdShortAdd(w, u, v->digits[0]);

	/* Make sure u and v are the same size */
	dig_size = max(u->ndigits, v->ndigits);
	bd_resize(v, dig_size);
	bd_resize(u, dig_size);
	/* Now make sure w is big enough for sum (incl carry) */
	bd_resize(w, dig_size + 1);

	/* Finally, do the business */
	carry = mpAdd(w->digits, u->digits, v->digits, dig_size);

	/* Make sure we've set the right size for w */
	if (carry)
	{
		w->digits[dig_size] = carry;
		w->ndigits = dig_size + 1;
	}
	else
		w->ndigits = mpSizeof(w->digits, dig_size);

	return carry;
}

int bdAddEx(T w, T u, T v)
	/* Compute w = u + v (safe) */
{
	DIGIT_T carry;
	T ww;

	assert(w && u && v);
	/* Use temp */
	ww = bdNew();
	bdSetEqual(ww, w);

	carry = bdAdd(ww, u, v);

	bdSetEqual(w, ww);
	bdFree(&ww);

	return carry;
}

int bdShortSub(T w, T u, unsigned long d)
	/* Compute w = u - d, return borrow */
{
	DIGIT_T borrow;
	size_t dig_size = max(u->ndigits, 1);

	assert(w && u);
	bd_resize(w, dig_size);

	borrow = mpShortSub(w->digits, u->digits, d, dig_size);

	w->ndigits = dig_size;

	return borrow;
}

int bdSubtract(T w, T u, T v)
	/* Compute w = u - v, return borrow, w#v */
{
	size_t dig_size;
	DIGIT_T borrow;

	assert(w && u && v);
	/* Check for cheaper option */
	if (v->ndigits == 1)
		return bdShortSub(w, u, v->digits[0]);

	/* Make sure u and v are the same size */
	dig_size = max(u->ndigits, v->ndigits);
	bd_resize(v, dig_size);
	bd_resize(u, dig_size);
	bd_resize(w, dig_size);

	/* Finally, do the business */
	borrow = mpSubtract(w->digits, u->digits, v->digits, dig_size);

	/* Make sure we've set the right size for w */
	w->ndigits = mpSizeof(w->digits, dig_size);

	return borrow;
}

int bdSubtractEx(T w, T u, T v)
	/* Compute w = u - v (safe) */
{
	DIGIT_T carry;
	T ww;

	assert(w && u && v);
	/* Use temp */
	ww = bdNew();
	bdSetEqual(ww, w);

	carry = bdSubtract(ww, u, v);

	bdSetEqual(w, ww);
	bdFree(&ww);

	return carry;
}

int bdIncrement(T a)
	/* Sets a = a + 1, returns carry */
{
	assert(a);
	return bdShortAdd(a, a, 1);
}

int bdDecrement(T a)
	/* Sets a = a - 1, returns borrow */
{
	assert(a);
	return bdShortSub(a, a, 1);
}

int bdShortMult(T w, T u, unsigned long d)
	/* Compute w = u * d */
{
	DIGIT_T overflow;
	size_t dig_size = u->ndigits;

	assert(w && u);
	bd_resize(w, dig_size+1);

	overflow = mpShortMult(w->digits, u->digits, d, dig_size);

	/* Cope with overflow */
	if (overflow)
	{
		w->digits[dig_size] = overflow;
		w->ndigits = dig_size + 1;
	}
	else
		w->ndigits = mpSizeof(w->digits, dig_size);

	return 0;
}

int bdMultiply(T w, T u, T v)
	/* Compute w = u * v
	   -- no overlap permitted
	*/
{
	size_t dig_size;

	assert(w && u && v);
	/* Check for cheaper option */
	if (v->ndigits == 1)
		return bdShortMult(w, u, v->digits[0]);

	/* Make sure u and v are the same size */
	dig_size = max(u->ndigits, v->ndigits);
	bd_resize(v, dig_size);
	bd_resize(u, dig_size);
	/* Now make sure w is big enough for product */
	bd_resize(w, 2 * dig_size);

	/* Finally, do the business */
	mpMultiply(w->digits, u->digits, v->digits, dig_size);

	/* Make sure we've set the right size for w */
	w->ndigits = mpSizeof(w->digits, 2 * dig_size);

	return 0;
}

int bdMultiplyEx(T w, T u, T v)
	/* Compute w = u * v (safe) */
{
	T ww;

	assert(w && u && v);
	/* Use temp */
	ww = bdNew();
	bdSetEqual(ww, w);

	bdMultiply(ww, u, v);

	bdSetEqual(w, ww);
	bdFree(&ww);

	return 0;
}

int bdSquare(T w, T x)
	/* Computes w = x^2, w#x */
{
	size_t dig_size;

	assert(w && x);

	dig_size = max(x->ndigits, 1);
	/* Make sure w is big enough for product */
	bd_resize(w, 2 * dig_size);

	/* Finally, do the business */
	mpSquare(w->digits, x->digits, dig_size);

	/* Make sure we've set the right size for w */
	w->ndigits = mpSizeof(w->digits, 2 * dig_size);

	return 0;
}

int bdSquareEx(T w, T x)
	/* Compute w = x^2 (safe) */
{
	T ww;

	assert(w && x);
	/* Use temp */
	ww = bdNew();
	bdSetEqual(ww, w);

	bdSquare(ww, x);

	bdSetEqual(w, ww);
	bdFree(&ww);

	return 0;
}

int bdShortDiv(T q, T r, T u, unsigned long d)
	/* Computes quotient q = u / d and remainder r = u mod d */
{
	DIGIT_T rem;
	size_t dig_size;

	assert(q && r && u);
	dig_size = u->ndigits;
	bd_resize(q, dig_size);

	rem = mpShortDiv(q->digits, u->digits, d, dig_size);
	bdSetShort(r, rem);

	q->ndigits = mpSizeof(q->digits, dig_size);

	return 0;
}

int bdDivide(T q, T r, T u, T v)
	/* Computes quotient q = u / v and remainder r = u mod v
	   trashes q and r first
	*/
{
	size_t dig_size;

	assert(q && r && u && v);
	dig_size = u->ndigits;
	bd_resize(q, dig_size);
	bd_resize(r, dig_size);

	/* Do the business */
	mpDivide(q->digits, r->digits, u->digits, dig_size, v->digits, v->ndigits);

	/* Set final sizes */
	q->ndigits = mpSizeof(q->digits, dig_size);
	r->ndigits = mpSizeof(r->digits, dig_size);

	return 0;
}

int bdDivideEx(T q, T r, T u, T v)
	/* Computes quotient q = u / v and remainder r = u mod v (safe) */
{
	size_t dig_size;
	BIGD qq, rr;

	assert(q && r && u && v);
	/* Use temps because mpDivide trashes q and r */
	qq = bdNew();
	rr = bdNew();

	dig_size = u->ndigits;
	bd_resize(qq, dig_size);
	bd_resize(rr, dig_size);

	/* Do the business */
	mpDivide(qq->digits, rr->digits, u->digits, dig_size, v->digits, v->ndigits);

	/* Copy temps */
	qq->ndigits = dig_size;
	rr->ndigits = dig_size;
	bdSetEqual(q, qq);
	bdSetEqual(r, rr);

	/* Set final sizes */
	q->ndigits = mpSizeof(q->digits, dig_size);
	r->ndigits = mpSizeof(r->digits, dig_size);

	/* Free temps */
	bdFree(&qq);
	bdFree(&rr);

	return 0;
}

bdigit_t bdShortMod(T r, T u, unsigned long d)
	/* Returns r = u mod d */
{
	DIGIT_T rr;

	assert(r && u);
	rr = mpShortMod(u->digits, d, u->ndigits);
	bdSetShort(r, rr);

	return rr;
}

int bdModulo(T r, T u, T v)
	/* Computes r = u mod v, r#u */
{
	size_t nr;

	assert(r && u && v);

	/* NB r is only vdigits long at most */
	nr = v->ndigits;
	bd_resize(r, nr);

	/* Do the business */
	mpModulo(r->digits, u->digits, u->ndigits, v->digits, v->ndigits);

	/* Set final size */
	r->ndigits = mpSizeof(r->digits, nr);

	return 0;
}

int bdModuloEx(T r, T u, T v)
	/* Computes r = u mod v (safe) */
{
	T rr;

	assert(r && u && v);
	/* Use temp */
	rr = bdNew();
	bdSetEqual(rr, r);

	bdModulo(rr, u, v);

	bdSetEqual(r, rr);
	bdFree(&rr);

	return 0;
}

int bdSetBit(T a, size_t ibit, int value)
/* Set bit ibit (0..nbits-1) with value 1 or 0
   -- increases size if a too small but does not shrink */
{
	size_t idigit, bit_to_set;
	DIGIT_T mask;
	DIGIT_T *p;

	assert(a);
	/* Which digit? (0-based) */
	idigit = ibit / BITS_PER_DIGIT;
	/* Check size */
	if (idigit >= a->maxdigits)
	{
		bd_resize(a, idigit+1);
		a->ndigits = idigit+1;
	}

	/* use a ptr */
	p = a->digits;

	/* Set mask */
	bit_to_set = ibit % BITS_PER_DIGIT;
	mask = 0x01 << bit_to_set;

	if (value)
		p[idigit] |= mask;
	else
		p[idigit] &= (~mask);

	/* Set the right size */
	a->ndigits = mpSizeof(a->digits, a->ndigits);

	return 0;
}

int bdGetBit(T a, size_t ibit)
	/* Returns value 1 or 0 of bit n (0..nbits-1) */
{
	size_t idigit, bit_to_get;
	DIGIT_T mask;
	DIGIT_T *p;

	assert(a);
	/* Which digit? (0-based) */
	idigit = ibit / BITS_PER_DIGIT;
	/* Check size */
	if (idigit >= a->maxdigits)
		return 0;

	/* use a ptr */
	p = a->digits;

	/* Set mask */
	bit_to_get = ibit % BITS_PER_DIGIT;
	mask = 0x01 << bit_to_get;

	/* [EDIT v2.0.1:] return ((p[idigit] &= mask) ? 1 : 0);*/
	return ((p[idigit] & mask) ? 1 : 0);
}

int bdShiftLeft(T a, T b, size_t n)
	/* Computes a = b << n, n < BITS_PER_DIGIT */
{
	/* Increases the size of a if nec.
	Returns 0 if OK, or -1 if error */

	DIGIT_T carry;
	size_t dig_size = b->ndigits;

	assert(a && b);
	if (n >= BITS_PER_DIGIT)
		return -1;

	bd_resize(a, dig_size);
	carry = mpShiftLeft(a->digits, b->digits, n, dig_size);
	a->ndigits = dig_size;

	/* Cope with overflow */
	if (carry)
	{
		bd_resize(a, dig_size + 1);
		a->digits[dig_size] = carry;
		a->ndigits = dig_size + 1;
	}
	else
		a->ndigits = mpSizeof(a->digits, dig_size);

	return 0;
}

int bdShiftRight(T a, T b, size_t n)
	/* Computes a = b >> n, n < BITS_PER_DIGIT */
{
	/* Throws away shifted bits
	Returns 0 if OK, or -1 if error */

	size_t dig_size = b->ndigits;

	assert(a && b);
	if (n >= BITS_PER_DIGIT)
		return -1;

	bd_resize(a, dig_size);
	mpShiftRight(a->digits, b->digits, n, dig_size);

	/* Set the right size */
	a->ndigits = mpSizeof(a->digits, dig_size);

	return 0;
}

int bdModExp(T y, T x, T e, T m)
{
	/* Compute y = x^e mod m
	   x,e < m */

	size_t n;
	int status;

	assert(y && x && e && m);
	/* Make sure all variables are the same size */
	n = max(e->ndigits, m->ndigits);
	n = max(x->ndigits, n);

	bd_resize(y, n);
	bd_resize(x, n);
	bd_resize(e, n);
	bd_resize(m, n);

	/* Finally, do the business */
	status = mpModExp(y->digits, x->digits, e->digits, m->digits, n);

	y->ndigits = mpSizeof(y->digits, n);

	return status;
}

int bdModMult(T a, T x, T y, T m)
	/* Compute a = (x * y) mod m */
{
	size_t n;
	int status;

	assert(a && x && y && m);
	/* Make sure all variables are the same size */
	n = max(y->ndigits, m->ndigits);
	n = max(x->ndigits, n);

	bd_resize(a, n);
	bd_resize(y, n);
	bd_resize(x, n);
	bd_resize(m, n);

	/* Do the business */
	status = mpModMult(a->digits, x->digits, y->digits, m->digits, n);

	a->ndigits = mpSizeof(a->digits, n);

	return status;
}


int bdModInv(T x, T a, T m)
	/* Compute x = a^-1 mod m */
{
	size_t n;
	int status;

	assert(x && a && m);
	/* Make sure all variables are the same size */
	n = max(a->ndigits, m->ndigits);

	bd_resize(x, n);
	bd_resize(a, n);
	bd_resize(m, n);

	/* Do the business */
	status = mpModInv(x->digits, a->digits, m->digits, n);

	x->ndigits = mpSizeof(x->digits, n);

	return status;
}

int bdGcd(T g, T x, T y)
	/* Compute g = gcd(x, y) */
{
	size_t n;
	int status;

	assert(g && x && y);
	n = max(x->ndigits, y->ndigits);

	bd_resize(g, n);
	bd_resize(y, n);
	bd_resize(x, n);

	/* Do the business */
	status = mpGcd(g->digits, x->digits, y->digits, n);

	g->ndigits = mpSizeof(g->digits, n);

	return status;
}

int bdIsPrime(T b, size_t ntests)
/* Returns true if passes ntests x Miller-Rabin tests */
{
	assert(b);
	return (mpIsPrime(b->digits, b->ndigits, ntests));
}

int bdRabinMiller(T b, size_t ntests)
/* Returns true if passes ntests x Miller-Rabin tests without trial division */
{
	assert(b);
	return (mpRabinMiller(b->digits, b->ndigits, ntests));
}

bdigit_t bdRandDigit(void)
/* Return a random digit. */
{
	return spBetterRand();
}

size_t bdSetRandTest(T a, size_t ndigits)
/* Make a random bigd a of up to ndigits digits
   -- NB just for testing
   Return # digits actually set */
{
	size_t i, n, bits;
	DIGIT_T mask;

	assert(a);
	/* Pick a random size */
	n = (size_t)spSimpleRand(1, ndigits);

	/* Check allocated memory */
	bd_resize(a, n);

	/* Now fill with random digits */
	for (i = 0; i < n; i++)
		a->digits[i] = spBetterRand();

	a->ndigits = n;

	/*	Zero out a random number of bits in leading digit
		about half the time */
	bits = (size_t)spSimpleRand(0, 2*BITS_PER_DIGIT);
	if (bits != 0 && bits < BITS_PER_DIGIT)
	{
		mask = HIBITMASK;
		for (i = 1; i < bits; i++)
		{
			mask |= (mask >> 1);
		}
		mask = ~mask;
		a->digits[n-1] &= mask;
	}
	return n;
}

int bdRandomSeeded(T a, size_t nbits, unsigned char *seed,
	size_t seedlen, BD_RANDFUNC RandFunc)
/* Create a random mp digit at most nbits long
   -- high bit may or may not be set
   -- Uses whatever RNG function the user specifies */
{
	size_t i, hibit, ndigits, nbytes;
	DIGIT_T chop;

	assert(a);
	/* Make sure big enough */
	ndigits = (nbits + BITS_PER_DIGIT - 1) / BITS_PER_DIGIT;
	bd_resize(a, ndigits);
	nbytes = ndigits * sizeof(DIGIT_T);

	/* Generate random bytes using callback function */
	RandFunc((unsigned char *)a->digits, nbytes, seed, seedlen);

	/* Clear unwanted high bits */
	hibit = (nbits-1) % BITS_PER_DIGIT;
	for (chop = 0x01, i = 0; i < hibit; i++)
		chop = (chop << 1) | chop;

	a->digits[ndigits-1] &= chop;

	a->ndigits = ndigits;

	return 0;
}


int bdGeneratePrime(T b, size_t nbits, size_t ntests,
	unsigned char *seed, size_t seedlen, BD_RANDFUNC RandFunc)
{
	size_t i, hibit, ndigits, nbytes;
	DIGIT_T mask, chop;
	DIGIT_T *p;
	int done;
	int iloop, maxloops, j, maxodd;

	assert(b);
	/* Make sure big enough */
	ndigits = (nbits + BITS_PER_DIGIT - 1) / BITS_PER_DIGIT;
	bd_resize(b, ndigits);
	nbytes = ndigits * sizeof(DIGIT_T);

	/* use a ptr */
	p = b->digits;

	maxloops = 5;
	maxodd = 100 * nbits;
	done = 0;
	for (iloop = 0; !done && iloop < maxloops; iloop++)
	{
		/* Generate random digits using callback function */
		RandFunc((unsigned char *)p, nbytes, seed, seedlen);

		/* Set high and low bits */
		hibit = (nbits-1) % BITS_PER_DIGIT;
		mask = 0x01 << hibit;
		for (chop = 0x01, i = 0; i < hibit; i++)
			chop = (chop << 1) | chop;

		p[ndigits-1] |= mask;
		p[ndigits-1] &= chop;
		p[0] |= 0x01;

		/* Try each odd number until success or too many tries */
		for (j = 0; !done && j < maxodd; j++, mpShortAdd(p, p, 2, ndigits))
		{
			if (!(p[ndigits-1] & mask))
				break;	/* Catch overflow */

			if (debug) mpPrintNL(p, ndigits);

			if (mpIsPrime(p, ndigits, ntests))
			{
				done = 1;
				break;
			}
		}
	}

	if (debug) mpPrintNL(p, ndigits);

	b->ndigits = ndigits;

	return (done ? 0 : 1);
}


