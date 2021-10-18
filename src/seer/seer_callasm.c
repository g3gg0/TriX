/************************************************************************
SeeR a C-scripting library v 0.95a
http://przemekp.prv.pl/seer/        dev@przemekp.prv.pl

File:callasm.c (portable)
Desc:calls functions with parameters in void* params and size count
returning int or double.

Copyright (c) 1998-99,2002, Przemyslaw Podsiadly
All rights reserved.
See seer.h for license information.

[99.10.17]
Split of callasm for each compiler

Implementing functions returning struct....
---VC:switch(sizeof(struct))
1 via al,2 via ax,4 via eax,8 via eax:edx, else :push adr
---DJGPP:switch(sizeof(struct))
4 via eax, else :push adr
---SeeR:4 always push adr,
so:
push params...
push this
push return_adr
call

[99.04.02]
Dedicated versions for
GCC on intel,MS Visual C
Portable version for others (limited to 12*4 bytes long arguments)

DJGPP:Don't use `-fomit-frame-pointer' while compiling it !!!


Problems (after reading GCC info on function passing - gcc:Interface):
1.Returning struct and union: 1, 2, 4, or 8 bytes - returned normally,
other - a pointer to dest is passed (other compilers may do it also for
1-8 long structs, RISC other than GCC).
2. On some machines, the first few arguments are passed in registers; in
others, all are passed on the stack [ibidem].
-=>hope "portable" is OK in that metter (at least with Sparc)
3. On some machines (particularly the Sparc), certain types of arguments
are passed "by invisible reference".  This means that the value is
stored in memory, and the address of the memory location is passed to
the subroutine [ibidem].
-=>so SPARC may need a dedicated callasm implementation. More - it should
know the argument types. Where from? Script should give this info.
How? How to avoid this? Maybe disallow passing those "certain types".
************************************************************************/
#define INTERNAL_FILE
#include <seer.h>
#include "seer_internal.h"
#include "defines.h"
#include "mem.h"

//**************** portable(?) version **************
#define _I_PA(x) (((int*)params)[x])
#define _INT_FUNC(x) ((int (*)x)func)
#define _DBL_FUNC(x) ((double (*)x)func)

extern int seer_in_system_call;
extern unsigned char* seer_system_call;

int callfunc(void *func,void *params,int count)
{
	HEAP_CHECK;

	switch(count/4)
	{
	case 0:
		return _INT_FUNC(())
			();
	case 1:
		return _INT_FUNC((int))
			(_I_PA(0));
	case 2:
		return _INT_FUNC((int,int))
			(_I_PA(0),_I_PA(1));
	case 3:
		return _INT_FUNC((int,int,int))
			(_I_PA(0),_I_PA(1),_I_PA(2));
	case 4:
		return _INT_FUNC((int,int,int,int))
			(_I_PA(0),_I_PA(1),_I_PA(2),_I_PA(3));
	case 5:
		return _INT_FUNC((int,int,int,int,int))
			(_I_PA(0),_I_PA(1),_I_PA(2),_I_PA(3),_I_PA(4));
	case 6:
		return _INT_FUNC((int,int,int,int,int,int))
			(_I_PA(0),_I_PA(1),_I_PA(2),_I_PA(3),_I_PA(4),_I_PA(5));
	case 7:
		return _INT_FUNC((int,int,int,int,int,int,int))
			(_I_PA(0),_I_PA(1),_I_PA(2),_I_PA(3),_I_PA(4),_I_PA(5),_I_PA(6));
	case 8:
		return _INT_FUNC((int,int,int,int,int,int,int,int))
			(_I_PA(0),_I_PA(1),_I_PA(2),_I_PA(3),_I_PA(4),_I_PA(5),_I_PA(6),_I_PA(7));
	case 9:
		return _INT_FUNC((int,int,int,int,int,int,int,int,int))
			(_I_PA(0),_I_PA(1),_I_PA(2),_I_PA(3),_I_PA(4),_I_PA(5),_I_PA(6),_I_PA(7),_I_PA(8));
	case 10:
		return _INT_FUNC((int,int,int,int,int,int,int,int,int,int))
			(_I_PA(0),_I_PA(1),_I_PA(2),_I_PA(3),_I_PA(4),_I_PA(5),_I_PA(6),_I_PA(7),_I_PA(8),_I_PA(9));
	case 11:
		return _INT_FUNC((int,int,int,int,int,int,int,int,int,int,int))
			(_I_PA(0),_I_PA(1),_I_PA(2),_I_PA(3),_I_PA(4),_I_PA(5),_I_PA(6),_I_PA(7),_I_PA(8),_I_PA(9),_I_PA(10));
	case 12:
		return _INT_FUNC((int,int,int,int,int,int,int,int,int,int,int,int))
			(_I_PA(0),_I_PA(1),_I_PA(2),_I_PA(3),_I_PA(4),_I_PA(5),_I_PA(6),_I_PA(7),_I_PA(8),_I_PA(9),_I_PA(10),_I_PA(11));
	default:
		return -1;//set error also (unhandled number of paramters)
	}
}

double callfunc_d(void *func,void *params,int count)
{
	switch(count/4)
	{
	case 0:
		return _DBL_FUNC(())
			();
	case 1:
		return _DBL_FUNC((int))
			(_I_PA(0));
	case 2:
		return _DBL_FUNC((int,int))
			(_I_PA(0),_I_PA(1));
	case 3:
		return _DBL_FUNC((int,int,int))
			(_I_PA(0),_I_PA(1),_I_PA(2));
	case 4:
		return _DBL_FUNC((int,int,int,int))
			(_I_PA(0),_I_PA(1),_I_PA(2),_I_PA(3));
	case 5:
		return _DBL_FUNC((int,int,int,int,int))
			(_I_PA(0),_I_PA(1),_I_PA(2),_I_PA(3),_I_PA(4));
	case 6:
		return _DBL_FUNC((int,int,int,int,int,int))
			(_I_PA(0),_I_PA(1),_I_PA(2),_I_PA(3),_I_PA(4),_I_PA(5));
	case 7:
		return _DBL_FUNC((int,int,int,int,int,int,int))
			(_I_PA(0),_I_PA(1),_I_PA(2),_I_PA(3),_I_PA(4),_I_PA(5),_I_PA(6));
	case 8:
		return _DBL_FUNC((int,int,int,int,int,int,int,int))
			(_I_PA(0),_I_PA(1),_I_PA(2),_I_PA(3),_I_PA(4),_I_PA(5),_I_PA(6),_I_PA(7));
	case 9:
		return _DBL_FUNC((int,int,int,int,int,int,int,int,int))
			(_I_PA(0),_I_PA(1),_I_PA(2),_I_PA(3),_I_PA(4),_I_PA(5),_I_PA(6),_I_PA(7),_I_PA(8));
	case 10:
		return _DBL_FUNC((int,int,int,int,int,int,int,int,int,int))
			(_I_PA(0),_I_PA(1),_I_PA(2),_I_PA(3),_I_PA(4),_I_PA(5),_I_PA(6),_I_PA(7),_I_PA(8),_I_PA(9));
	case 11:
		return _DBL_FUNC((int,int,int,int,int,int,int,int,int,int,int))
			(_I_PA(0),_I_PA(1),_I_PA(2),_I_PA(3),_I_PA(4),_I_PA(5),_I_PA(6),_I_PA(7),_I_PA(8),_I_PA(9),_I_PA(10));
	case 12:
		return _DBL_FUNC((int,int,int,int,int,int,int,int,int,int,int,int))
			(_I_PA(0),_I_PA(1),_I_PA(2),_I_PA(3),_I_PA(4),_I_PA(5),_I_PA(6),_I_PA(7),_I_PA(8),_I_PA(9),_I_PA(10),_I_PA(11));
	default:
		return -1;//set error also (unhandled number of paramters)
	}
}

//how to write a portable member caller, while in pure C?
//assume this is passed via the stack (like gcc does it)
int callmember(void *func,void *params,int count)
{//here it is fun - same as callfunc -
	//this is normally the first parameter
	return callfunc(func,params,count);
}
double callmember_d(void *func,void *params,int count)
{//here it is fun - same as callfunc -
	//this is normally the first parameter
	return callfunc_d(func,params,count);
}


void scStandardDispatcher (int *result,void *function,int *params,int paramcount,unsigned int options)
{
	seer_in_system_call = 1;
	seer_system_call = scGet_External_Symbolname_By_Addr ( function );

	HEAP_CHECK;
	deb0("Dispatcher ");
	if ((options&scDispatch_Struct))
	{deb0("struct ");
	}

	if (!(options&scDispatch_Member))
	{//not a class-member!
		if (!(options&scDispatch_Double))
		{
			deb0("int ");
	HEAP_CHECK;
			*result = callfunc( function, params, paramcount);
	HEAP_CHECK;
		}
		else
		{
			deb0("double ");debflush();
	HEAP_CHECK;
			(*((double *)result))=callfunc_d( function, params, paramcount);
	HEAP_CHECK;
			deb0("Done.\n");debflush();
		}
	}
	else
	{//a class-member!
		if (!(options&scDispatch_Double))
		{
	HEAP_CHECK;
			*result=callmember( function, params, paramcount);
	HEAP_CHECK;
		}
		else
		{
	HEAP_CHECK;
			(*((double *)result))=callmember_d( function, params, paramcount);
	HEAP_CHECK;
		}
	}
	HEAP_CHECK;

	seer_in_system_call = 0;

}

