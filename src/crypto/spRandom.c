/* spRandom.c */

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
    $Date: 2006/04/13 00:20:56 $
    $Revision: 1.1 $
	$Author: geggo $
*/

#include <stdlib.h>
#include <time.h>
#include "bigdigits.h"

static unsigned long btrrand(void);

DIGIT_T spBetterRand(void)
{	/*	Returns a "better" pseudo-random digit. */
	return (DIGIT_T)btrrand();
}

/*
Generates a pseudo-random DIGIT value by using
the ANSI X9.31 algorithm but with the `Tiny Encryption Algorithm' 
replacing the `Triple DES' algorithm (much less code to copy, and faster).

CAUTION: not thread-safe as it uses a static variable.

Not quite cryptographically secure but much better than 
just using the plain-old-rand() function. 
Output should always pass the FIPS 140-2 statistical test.
Users can make their own call as to the security of this approach.
It's certainly sufficient for generating random digits for tests.
*/

/*
ANSI X9.31 ALGORITHM:
Given

    * D, a 64-bit representation of the current date-time
    * S, a secret 64-bit seed that will be updated by this process
    * K, a secret (Triple DES) key

Step 1. Compute the 64-bit block X = G(S, K, D) as follows:

   1. I = E(D, K)
   2. X = E(I XOR S, K)
   3. S' = E(X XOR I, K)

where E(p, K) is the (Triple DES) encryption of the 64-bit block p using key K.

Step 2. Return X and set S = S' for the next cycle. 
*/

#define KEY_WORDS 4
static void encipher(unsigned long *const v,unsigned long *const w, const unsigned long *const k);

/* CAUTION: We use a static structure to store our values in. */
static struct {
	int seeded;
	unsigned long seed[2];
	unsigned long key[KEY_WORDS];
} m_generator;

/* Cross-platform ways to get a 64-bit time value */
#ifdef unix
static void get_time64(unsigned long t[2])
{
	#include <sys/time.h>
	struct timeval tv;
	gettimeofday(&tv, NULL);
	memcpy(t, &tv, 2*sizeof(unsigned long));
}
#elif _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
static void get_time64(unsigned long t[2])
{
	FILETIME ft;
	GetSystemTimeAsFileTime (&ft);
	t[0] = ft.dwHighDateTime;
	t[1] = ft.dwLowDateTime;
}
#else
static void get_time64(unsigned long t[2])
{
	/* Best we can do with strict ANSI */
	t[0] = time(NULL);
	t[1] = 0;
}
#endif

static void btrseed(unsigned long seed)
{
	int i;
	unsigned long t[2];

	/* Use plain old rand function to generate our internal seed and key */
	srand(seed);
	for (i = 0; i < 2; i++)
		m_generator.seed[i] = (rand() & 0xFFFF) << 16 | (rand() & 0xFFFF);
	for (i = 0; i < KEY_WORDS; i++)
		m_generator.key[i] = (rand() & 0xFFFF) << 16 | (rand() & 0xFFFF);

	/* Set flag so we only do it once */
	m_generator.seeded = 1;

	/* Set key = key XOR time */
	get_time64(t);
	m_generator.key[0] ^= t[0];
	m_generator.key[1] ^= t[1];

}

static unsigned long btrrand(void)
/* Returns one 32-bit word */
{
	unsigned long inter[2], x[2];

	/* Set seed if not already seeded */
	if (!m_generator.seeded)
	{
		btrseed(time(NULL));
	}

	/* I = E(D, K) */
	get_time64(inter);
	encipher(inter, inter, m_generator.key);

	/* X = E(I XOR S, K) */
	x[0] = inter[0] ^ m_generator.seed[0];
	x[1] = inter[1] ^ m_generator.seed[1];
	encipher(x, x, m_generator.key);

	/* S' = E(X XOR I, K) */
	inter[0] ^= x[0];
	inter[1] ^= x[1];
	encipher(inter, m_generator.seed, m_generator.key);

	return x[0];
}

/************************************************

The Tiny Encryption Algorithm (TEA) by 
David Wheeler and Roger Needham of the
Cambridge Computer Laboratory

**** ANSI C VERSION ****

Notes:

TEA is a Feistel cipher with XOR and
and addition as the non-linear mixing
functions. 

Takes 64 bits of data in v[0] and v[1].
Returns 64 bits of data in w[0] and w[1].
Takes 128 bits of key in k[0] - k[3].

************************************************/

static void encipher(unsigned long *const v,unsigned long *const w,
const unsigned long *const k)
{
	register unsigned long y=v[0],z=v[1],sum=0,delta=0x9E3779B9,
	a=k[0],b=k[1],c=k[2],d=k[3],n=32;

	while(n-- > 0)
	{
		sum += delta;
		y += ((z<<4)+a) ^ (z+sum) ^ ((z>>5)+b);
		z += ((y<<4)+c) ^ (y+sum) ^ ((y>>5)+d);
	}

	w[0]=y; w[1]=z;
}

