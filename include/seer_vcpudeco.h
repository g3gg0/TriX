/************************************************************************
 SeeR a C-scripting library v 0.95a
 http://przemekp.prv.pl/seer/        dev@przemekp.prv.pl

 File:vcpudeco.h (portable)
 Desc:fixed arythmetics, included from vcpudeco.c

 Copyright (c) 1998-99,2002, Przemyslaw Podsiadly
 All rights reserved.
 See seer.h for license information.
************************************************************************/

#ifndef __inline__
#if defined(WIN32) && defined (_MSC_VER)
#define __inline__      __inline
#else
#define __inline__      inline
#endif
#endif

//assume 'portable' has 64bit long called 'long long'
//this is standard gcc
//it would be cool if I could use 32 bit ints only...
//typedef int64 long long;

__inline__ int fixedmul(int x,int y)
{
	int64 d=x;
 d*=y;
 d>>=16;
 return (int)d;
}

__inline__ int fixeddiv(int x,int y)
{
	int64 d=x;
 d<<=16;
 if (y) d/=y;
 return (int)d;
}

