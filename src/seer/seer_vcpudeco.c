/************************************************************************
 SeeR a C-scripting library v 0.95a
 http://przemekp.prv.pl/seer/        dev@przemekp.prv.pl

 File:vcpudeco.c
 Desc:decoding and running script

 Copyright (c) 1998-99,2002, Przemyslaw Podsiadly
 All rights reserved.
 See seer.h for license information.
************************************************************************/
#define INTERNAL_FILE
#include <stdio.h>
#include <seer.h>
#include "defines.h"
#include "seer_internal.h"
#include "seer_code.h"

#include "seer_vcpudeco.h"

#include "mem.h"

int seer_script_aborted = 0;
int seer_in_system_call = 0;
int seer_memory_check = 1;
unsigned char *seer_system_call = NULL;

extern unsigned int seer_debug_ins;


void seer_abort_script ()
{
	seer_script_aborted = 1;
}

void seer_allow_script ()
{
	seer_script_aborted = 0;
}

int seer_in_system ()
{
	return seer_in_system_call;
}

unsigned char *seer_in_system_name ()
{
	return seer_system_call;
}

int seer_is_aborted ()
{
	return seer_script_aborted;
}

#define SC_ANY
#undef SC_INTEL

/*
#if defined SC_INTEL
#if defined SC_NASM || defined SC_FREEBSD || defined SC_UNIXWARE || defined SC_SCOUNIX
#include "nasm/vcpudeco.h"
#elif defined SC_GCC && (defined SC_DOS || defined SC_BEOS || defined SC_LINUX)
#include "linux/vcpudeco.h"
#elif defined SC_WIN32 && defined SC_MSVC
#include "msvc/vcpudeco.h"
#else
#error Uknown compiler or OS
#endif
#elif defined SC_ANY
#else
#error Non-intel machine
#endif
*/

#define ToINT(ch) ((int*)(ch))
#define ReadINT(ptr,x) memcpy(&x,ptr,4)
#define WriteINT(ptr,x) memcpy(ptr,&x,4)
#define ToCodeINT ((int*)Ins->code)
#define Register ((int*)Ins->data_stack)
#define CodeSeg ((char *)(Ins->code+Register[regCS]+Register[regIP]))
#define GetAtIP(typ) ((typ *)(Ins->code+Register[regCS]+(Register[regIP]+=sizeof(typ))-sizeof(typ)))

#define DoOper(typ1,typ2,op) *((typ1*)v1.what) op *((typ2*)v2.what);
#define DoOper2(typ1,typ2,op) *((typ1*)v1.what)=(*((typ1*)v1.what)) op (*((typ2*)v2.what));
#define DoOper3(typ1,typ2,op) *((int*)v1.what)=(*((typ1*)v1.what)) op (*((typ2*)v2.what));

#define MEMCHECK_MESSAGE \
		printf ( "###\n" ); \
		printf ( "### ERROR: SCRIPT ACCESSED NON-ALLOCATED MEMORY\n"  ); \
		printf ( "###\n" ); \
		\
		switch ( seer_debug_ins )\
		{\
			case 1:\
				printf ( "###  file: <%s>\n", current_file );\
				printf ( "###  line: <%i>\n", current_line );\
				printf ( "###\n" ); \
				printf ( "###  set 'core.seer.seer_debug_ins' to 2 to\n" ); \
				printf ( "###      enable more debug information     \n" ); \
				printf ( "###\n" ); \
				break;\
			\
			case 2:\
				printf ( "###  file: <%s>\n", current_file );\
				printf ( "###  line: <%i>\n", current_line );\
				printf ( "###  code: <%s>\n", current_srcline );\
				printf ( "###\n" ); \
				break;\
			\
			default:\
				printf ( "### set 'core.seer.seer_debug_ins' to 1 or 2 to\n" ); \
				printf ( "###       enable more debug information        \n" ); \
				printf ( "###\n" ); \
				break;\
			\
		}\


#define MEMCHECK_ADDR(x) \
	if ( seer_memory_check && !mem_is_valid(x)  ) \
    { \
		MEMCHECK_MESSAGE; \
		return 0; \
	}

#define MEMCHECK \
	if ( seer_memory_check && ((!memcheck_skip_v1 && !mem_is_valid(v1.what)) || (!memcheck_skip_v2 && !mem_is_valid(v2.what) )) ) \
    { \
		MEMCHECK_MESSAGE; \
		return 0; \
	}
//#define MEMCHECK


#define DOUBLEFix                                    \
       if(v2.what==&imINT2)                          \
        {int *x=(int*)(&imDOUBLE2);                  \
         imINT3=*GetAtIP(int);                       \
         *x=imINT2;                                  \
         *(x+1)=imINT3;                              \
         deb1("(2nd=$%2.2f)",imDOUBLE2);             \
         v2.what=(int*)&imDOUBLE2;                   \
        }
#define DOUBLEFix1                                   \
       if(v1.what==&imINT1)                          \
        {int *x=(int*)(&imDOUBLE1);                  \
         imINT3=*GetAtIP(int);                       \
         *x=imINT1;                                  \
         *(x+1)=imINT3;                              \
         deb1("(BOTH$%2.2f)",imDOUBLE1);             \
         v1.what=(int*)&imDOUBLE1;                   \
        }

#define DOUBLEdeb0(c)                                 \
        {                                             \
         deb2("<%c%f",c,                              \
                      *((double*)v2.what));           \
        }

#define DOUBLEdeb1(c)                                 \
        {                                             \
         deb3("%f%c%f",*((double*)v1.what),c,         \
                      *((double*)v2.what));           \
        }

#define DOUBLEdeb2                                    \
        {                                             \
         deb1("=>%f\n",*((double*)v1.what));          \
        }

#define DOUBLEint2                                    \
        {                                             \
         deb1("=>%d\n",*((int*)v1.what));             \
        }

#define PushInt(co)\
        {\
         Register[regSP]-=4;\
         *((int *)(Register[regSS]+Register[regSP]))=co;\
        }

#define PopInt(co)\
        {\
         co=*((int *)(Register[regSS]+Register[regSP]));\
         Register[regSP]+=4;\
        }

typedef struct eval
{
    int *what;
//        char type;//=0 - immidiate
//        char ptr;
} eval;

//scInstance *Ins;
//if speed<0 - til end, else speed tacts
int
Executor ( scInstance * Ins, int speed )
{
	int current_line = -1;
	int filename_length = 0;
	unsigned char *current_file = "UNKNOWN";
	int srcline_length = 0;
	unsigned char *current_srcline = "UNKNOWN";
	int memcheck_skip_v1 = 0;
	int memcheck_skip_v2 = 0;
    int opcode, i;
    unsigned char *opc = ( unsigned char * ) &opcode;
    int imINT1, imINT2, imINT3;
    double imDOUBLE1, imDOUBLE2;
    int c, actspeed = speed;
    bool immi1 = false, immi2 = false;
    eval v1, v2;
    scActual_Instance = Ins;    //Set Global Variable...



    if ( Ins->flags.forkno > 0 )
    {
        deb2 ( "\n\nExecuting function from %s (fork:%d)!\n", scGet_Instance_Title ( Ins ), Ins->flags.forkno );
    }
    else
    {
        deb1 ( "\n\nExecuting function from %s!\n", scGet_Instance_Title ( Ins ) );
    }
    do
    {
		if ( seer_script_aborted )
			rerr2 ( "script aborted at %d\n", Register[regIP] - 4 );

		HEAP_CHECK;
		memcheck_skip_v1 = 0;
		memcheck_skip_v2 = 0;
		MEMCHECK_ADDR ( Ins->code + Register[regCS] + Register[regIP] );
        deb1 ( " %d:", Register[regIP] );
        opcode = *GetAtIP ( int );

        while ( opcode == opvcpuEXPR )  //EXTENDED1 - EXPR opcode is normally ignored
        {
            //only here the execution can break
            if ( actspeed > 0 )
            {
                actspeed--;
            }
            if ( ( !actspeed ) && !Register[regIE] )    //every number of instructions wait
            {
                scActual_Instance = NULL;
                actspeed = speed;
                return 0;
            }
			MEMCHECK_ADDR ( Register + regIP );
            deb1 ( "EXPR ---- \n %d:", Register[regIP] );
            //get next opcode
            opcode = *GetAtIP ( int );
        }

        immi1 = immi2 = false;

		if ( opc[0] == opvcpuADDSRCLINE )
		{
			deb0 ( "ADDSRCLINE " );
			srcline_length = (opc[1]<<16) | (opc[2]<<8) | opc[3];
			current_srcline = Ins->code+Register[regCS]+Register[regIP];

			deb1 ( "[%d]", srcline_length );
            deb1 ( "[%s]", current_srcline );

			Register[regIP] += srcline_length;
			while ( Register[regIP] % 4 )
				Register[regIP]++;

            deb0 ( "\n" );
		}
		else if ( opc[0] == opvcpuADDFILENAME )
		{
			deb0 ( "ADDFILENAME " );
			filename_length = (opc[1]<<16) | (opc[2]<<8) | opc[3];
			current_file = Ins->code+Register[regCS]+Register[regIP];

			deb1 ( "[%d]", filename_length );
            deb1 ( "[%s]", current_file );

			Register[regIP] += filename_length;
			while ( Register[regIP] % 4 )
				Register[regIP]++;

            deb0 ( "\n" );
		}
        else if ( ( opc[0] & 63 ) >= opvcpuRET && ( opc[0] & 63 ) < opvcpuLINENUM ) //4 instructions with no params:
        {
            for ( i = 0; i < 4; i++ )
            {
                c = opc[i] & 0xff;
                switch ( c )
                {
                    case opvcpuRET:
                        deb0 ( "RET " );
						MEMCHECK_ADDR ( Register + regSP );
						MEMCHECK_ADDR ( Register + regCP );
						MEMCHECK_ADDR ( Register + regIP );
						MEMCHECK_ADDR ( Register + regIE );
                        deb2 ( "SP=%d,CP=%d\n", Register[regSP], Register[regCP] );
                        if ( Register[regSP] == Register[regCP] )
                        {
                            scActual_Instance = NULL;
                            deb0 ( "EXIT\n" );
                            return 1;   //end of instance
                        }
                        PopInt ( Register[regIP] );
                        deb1 ( " a0=%d ", Register[0] );
                        i = 3;  //exit
                        break;

                    case opvcpuCLI:
                        deb0 ( "CLI " );
						MEMCHECK_ADDR ( Register + regIE );
                        Register[regIE]++;
                        break;  // \Turn OFF\ Multitasking

                    case opvcpuSTI:
						MEMCHECK_ADDR ( Register + regIE );
                        if ( Register[regIE] > 0 )
                            Register[regIE]--;  //if 0, interrupts enabled
                        else
                            rerr ( "multitasking violation" );
                        deb0 ( "STI " );
                        break;  // /     ON /

                    case opvcpuWAIT:
                        deb0 ( "WAIT " );
                        if ( speed > 0 )
                        {
                            scActual_Instance = NULL;
                            return 0;
                        }
                        break;  // -
                    case opvcpuENTER:
                        deb0 ( "ENTER " );
						MEMCHECK_ADDR ( Register + regBP );
						MEMCHECK_ADDR ( Register + regSS );
                        PushInt ( Register[regBP] - Register[regSS] );
                        Register[regBP] = Register[regSP] + Register[regSS];
                        deb1 ( "%d ", Register[regBP] );
                        break;  //push bp;mov bp,sp

                    case opvcpuLEAVE:
						MEMCHECK_ADDR ( Register + regBP );
						MEMCHECK_ADDR ( Register + regSS );
						MEMCHECK_ADDR ( Register + regSP );
                        Register[regSP] = Register[regBP] - Register[regSS];
                        PopInt ( Register[regBP] );
                        Register[regBP] += Register[regSS];
                        deb0 ( "LEAVE " );
                        break;  //mov sp,bp; pop bp

                    case opvcpuNOP:
                        deb0 ( "NOP " );
                        break;

                    default:
                        rerr3 ( "Unknown opcode %d in the script at %d!\n", opc[i] & 0xff, Register[regIP] - 4 );
                }
                deb0 ( "\n" );
            }                   //for
        }
        else if ( ( opc[0] & 63 ) < opvcpuMOV  ) //1 arg
        {
			memcheck_skip_v2 = 1;
            immi1 = false;
            //EXTENDED opcode
            if ( !opc )
            {                   //other extended opcode?
                continue;
            }

            if ( opc[1] & 1 )   //pointer
            {
                if ( opc[1] & 2 )   //register
                {
                    v1.what = ( int * ) Register[opc[2]];
                    deb1 ( "[a%d]", opc[2] );
					memcheck_skip_v1 = 0;
                }
                else            //immidiate
                {
                    v1.what = ( int * ) ( *GetAtIP ( int ) );
                    deb1 ( "[%d]", ( int ) v1.what );
                    immi1 = true;
					memcheck_skip_v1 = 0;
                }
                //make register shift
                switch ( ( opc[1] >> 2 ) & adrSTOREPOINTER )    //base register
                {
                    case adrSTORECPU:
                        if ( immi1 )    //import
                        {
                            ReadINT ( ( char * ) Ins->code + ToCodeINT[hdrImportedOffset] + ( int ) v1.what, imINT1 );
                            v1.what = &imINT1;
			    memcheck_skip_v1 = 1;
                        }
                        break;
                    case adrES:
                        ( char * ) ( v1.what ) += Register[regES];
                        deb0 ( "ES" );
                        break;
                    case adrDS:
                        ( char * ) ( v1.what ) += Register[regDS];
                        deb0 ( "DS" );
                        break;
                    case adrBP:
                        ( char * ) ( v1.what ) += Register[regBP];
                        deb0 ( "BP" );
                        break;
                }
//        debprintf("(value=%d)\n",*((int*)v1.what));
            }
            else                //imidiate
            {
                if ( opc[1] & 2 )   //register
                {
                    v1.what = Register + opc[2];
                    deb1 ( "a%d", opc[2] );
                }
                else
                {
                    imINT1 = *GetAtIP ( int );
                    deb1 ( "$%04x", imINT1 );
                    v1.what = &imINT1;
					memcheck_skip_v1 = 1;
                }
            }
            deb1 ( "(=$%d) ", *( ( int * ) v1.what ) );
            switch ( opc[0] & 63 )
            {
                case opvcpuPUSH:
					MEMCHECK;
                    deb0 ( "PUSH " );
                    PushInt ( *( ( int * ) v1.what ) );
                    deb3 ( "(SP=%d,@%d,BP=%d) ", Register[regSP], Register[regSS] + Register[regSP], Register[regBP] );
                    break;

                case opvcpuPUSHADR:
                    deb0 ( "PUSHADR " );
                    PushInt ( ( int ) v1.what );
                    deb3 ( "(SP=%d,@%d,BP=%d) ", Register[regSP], Register[regSS] + Register[regSP], Register[regBP] );
                    break;

                case opvcpuDPUSH:
					MEMCHECK;
                    deb0 ( "DPUSH " );
                    DOUBLEFix1;
                    PushInt ( *( ( ( int * ) v1.what ) + 1 ) );
                    PushInt ( *( ( ( int * ) v1.what ) ) );
                    deb2 ( "of %08x%08x", *( ( ( int * ) v1.what ) ), *( ( ( int * ) v1.what ) + 1 ) );
                    deb2 ( " or %f", *( ( ( int * ) v1.what ) ), *( ( ( int * ) v1.what ) + 1 ) );
                    deb1 ( "(SP=%d) ", Register[regSP] );
                    break;

                case opvcpuDNEG:
					MEMCHECK;
                    deb0 ( "DNEG " );
                    DOUBLEFix1;
                    *( ( double * ) v1.what ) = -*( ( double * ) v1.what );
                    break;

                case opvcpuPOP:
					MEMCHECK;
                    deb0 ( "POP " );
                    PopInt ( *( ( int * ) v1.what ) );
                    deb1 ( "(SP=%d) ", Register[regSP] );
                    break;

                case opvcpuCALL:
					MEMCHECK;
                    deb0 ( "CALL " );
                    PushInt ( Register[regIP] );
                    if ( *( ( int * ) v1.what ) == -1 )
                    {
                        rerr ( "not defined function called" );
                    }
                    Register[regIP] = *( ( int * ) v1.what );   //call
                    break;

                case opvcpuJMP:
					MEMCHECK;
                    deb0 ( "JMP " );
                    Register[regIP] += *v1.what;
                    break;

                case opvcpuNEG:
					MEMCHECK;
                    deb0 ( "NEG " );
                    *( ( int * ) v1.what ) = -*( ( int * ) v1.what );
                    break;

                case opvcpuNOT:
					MEMCHECK;
                    deb0 ( "NOT " );
                    *( ( int * ) v1.what ) = !*( ( int * ) v1.what );
                    deb1 ( "(%d)", *( ( int * ) v1.what ) );
                    break;

                case opvcpuOPTIONS:
					MEMCHECK;
                    deb0 ( "OPTIONS " );
                    Register[regOPTIONS] = *( ( int * ) v1.what );
                    break;

                default:
                    rerr3 ( "Unknown opcode:%d in the script at %d!\n", opc[0] & 63, Register[regIP] - 4 );
            }
            deb0 ( "\n" );
        }
        else                    //2 arg
        {
            //read params to v1 and v2
            //1st
            deb0 ( "{" );

            if ( opc[0] & 64 )  //pointer
            {
                if ( opc[1] & 1 )   //register
                {
                    v1.what = ( int * ) Register[opc[2]];
                    deb1 ( "[a%d]", opc[2] );
					memcheck_skip_v1 = 0;
                }
                else            //immidiate
                {
                    v1.what = ( int * ) ( *GetAtIP ( int ) );
                    deb1 ( "[%d]", ( int ) v1.what );
                    immi1 = true;
					memcheck_skip_v1 = 0;
                }
                //make register shift
                switch ( ( ( opc[1] >> 1 ) & adrSTOREPOINTER ) )    //base register
                {
                    case adrSTORECPU:
                        if ( immi1 )    //import
                        {
                            ReadINT ( ( char * ) Ins->code + ToCodeINT[hdrImportedOffset] + ( int ) v1.what, imINT1 );
                            v1.what = &imINT1;
							memcheck_skip_v1 = 1;
                        }
                        break;
                    case adrES:
                        ( char * ) ( v1.what ) += Register[regES];
                        deb0 ( "ES" );
                        break;
                    case adrDS:
                        ( char * ) ( v1.what ) += Register[regDS];
                        deb0 ( "DS" );
                        break;
                    case adrBP:
                        ( char * ) ( v1.what ) += Register[regBP];
                        deb1 ( "{%d}", ( int ) v1.what );
                        deb0 ( "BP" );
                        break;
                }
//        debprintf("(value=%d)\n",*((int*)v1.what));
            }
            else                //imidiate
            {
                if ( opc[1] & 1 )   //register    
                {
                    v1.what = Register + opc[2];
					memcheck_skip_v1 = 0;
					memcheck_skip_v2 = 1;
                    deb1 ( "a%d", opc[2] );
                    switch ( opc[0] & 63 )
                    {
                        case opvcpuWMOV:
                        case opvcpuCMOV:
							MEMCHECK;
                            *v1.what = 0;
                            deb0 ( " CLEARED " );
                            break;
                    }
                }
                else
                {
                    imINT1 = *GetAtIP ( int );
                    deb1 ( "$%04x", imINT1 );
                    v1.what = &imINT1;
					memcheck_skip_v1 = 1;
                }
            }
            deb1 ( "(=$%d)", *( ( int * ) v1.what ) );
            deb0 ( "," );
            //2nd parameter
            if ( opc[0] & 128 ) //pointer
            {
                if ( opc[1] & ( 1 << 4 ) )  //register
                {
                    v2.what = ( int * ) Register[opc[3]];
                    deb1 ( "[a%d]", opc[3] );
					memcheck_skip_v2 = 0;
                }
                else            //immidiate
                {
                    v2.what = ( int * ) ( *GetAtIP ( int ) );
                    deb1 ( "[%d]", ( int ) v2.what );
                    immi2 = true;
					memcheck_skip_v2 = 0;
                }
                //make register shift
//       switch ( ( opc[1]&((valBP>>2)<<5) ) >>3)//base register
                switch ( ( opc[1] >> 5 ) & adrSTOREPOINTER )    //base register
                {
                    case adrSTORECPU:
                        if ( immi2 )    //import
                        {
                            ReadINT ( ( char * ) Ins->code + ToCodeINT[hdrImportedOffset] + ( int ) v2.what, imINT2 );
                            v2.what = &imINT2;
							memcheck_skip_v2 = 1;
                        }
                        break;
                    case adrES:
                        ( char * ) ( v2.what ) += Register[regES];
                        deb0 ( "ES" );
                        break;
                    case adrDS:
                        ( char * ) ( v2.what ) += Register[regDS];
                        deb0 ( "DS" );
                        break;
                    case adrBP:
                        ( char * ) ( v2.what ) += /*Register[regSS]+ */ Register[regBP];
                        deb1 ( "{%d}", ( int ) v2.what );
                        deb0 ( "BP" );
                        break;
                }
            }
            else                //imidiate
            {
                if ( opc[1] & ( 1 << 4 ) )  //register
                {
                    v2.what = Register + opc[3];
                    deb1 ( "a%d", opc[3] );
					memcheck_skip_v2 = 0;
                }
                else
                {
                    imINT2 = *GetAtIP ( int );
                    deb1 ( "$%04x", imINT2 );
                    v2.what = &imINT2;
					memcheck_skip_v2 = 1;
                }
            }
            deb1 ( "(=$%d)", ( ( int * ) v2.what ) );
            deb0 ( "}" );
            switch ( opc[0] & 63 )
            {
                case opvcpuLINENUM:
                    deb0 ( "LINENUM " );

					current_line = *( ( int * ) v1.what );
					current_file = Ins->code+Register[regCS] + *( ( int * ) v2.what );

					deb1 ( "[%s]:", current_file );
					deb1 ( "[%d]", current_line );
                    break;


                case opvcpuLINENUM2:
                    deb0 ( "LINENUM2 " );

					current_line = *( ( int * ) v1.what );
					current_srcline = Ins->code+Register[regCS] + *( ( int * ) v2.what );

					deb1 ( "[%s]:", current_srcline );
					deb1 ( "[%d]", current_line );
                    break;


                case opvcpuMOV:
					MEMCHECK;
                    DoOper ( int, int, = );
                    deb0 ( "MOV" );
                    break;

                case opvcpuCMOV:
					MEMCHECK;
                    DoOper ( char, char, = );
                    deb0 ( "CMOV" );
                    break;

                case opvcpuWMOV:
					MEMCHECK;
                    DoOper ( short, short, = );
                    deb0 ( "WMOV" );
                    break;

                case opvcpuDMOV:
					MEMCHECK;
                    DOUBLEFix;
                    DOUBLEdeb0 ( '=' );
                    DoOper ( double, double, = );
                    DOUBLEdeb2;
                    deb0 ( "DMOV" );
                    break;

                case opvcpuCALLEX: //also add sp,paramcount
                    deb0 ( "CALLEX" );
                    {
                        int x, nr, num, *result;
						HEAP_CHECK;
						MEMCHECK ( v1.what );
                        x = ToCodeINT[hdrImportedOffset] + *( ( int * ) v1.what );
						MEMCHECK ( Ins->code + x );
                        ReadINT ( ( char * ) Ins->code + x, x );
                        scErrorNo = scOK;
                        debflush (  );
/*
   (1-16) First 16 bits of v2.what for param count,
   (17-22)next 9 bits reserved,
   (22)   1 bit for result (0-non-void, 1-void)
(23,24,25)3 bits for struct:0-none,1-normal,2-7(size=1<<(bits-2))
   (26)   1 bit for calling script function from other instance
   (27)   1 bit for result type (0-int, 1-double)
   (28)   1 bit for member function (0-not a member)
   (29-32)   last 4 bits for function dispatcher number
*/
                        nr = ( *( ( unsigned int * ) v2.what ) ) >> 28;
                        num = ( *( unsigned int * ) v2.what ) & 0x0ffff;

                        if ( !scUserFunctionDispatcher[nr] )
                            rerr2 ( "Undefined dispatcher (%d) used!\n", nr );
                        scParamCount = num;

                        //another script
                        //TODO:if instance is running, call forked instance, else call instance
                        if ( *( ( unsigned int * ) v2.what ) & scDispatch_Script )
                        {
                            scScriptTypeHeader *h = *( void ** ) ( Register[regSS] + Register[regSP] );

                            deb1 ( "\nCalling another script with %d params\n\n", num );

                            scVCall_Instance ( h->ins, ( ( int * ) h )[*v1.what / 4], num, ( ( int * ) ( Register[regSS] + Register[regSP] ) ) + 1 );

                            deb0 ( "\nEnd of calling another script\n\n" );

                            if ( scErrorNo )
                                goto scError_Exit;

                            if ( *( ( unsigned int * ) v2.what ) & scDispatch_Double )
                                *( ( double * ) Ins->data_stack ) = *( ( double * ) h->ins->data_stack );
                            else
                                *( ( int * ) Ins->data_stack ) = *( ( int * ) h->ins->data_stack );

                        }
                        else
                            //imported function
                        {
                            if ( ( *( ( unsigned int * ) v2.what ) ) & scDispatch_Void )
                                result = &x;
                            else
                                result = &Register[0];
							HEAP_CHECK;
							if ( scUserFunctionDispatcher[nr] && scUserFunctionDispatcher[nr] != 0xFFFFFFFF )
								scUserFunctionDispatcher[nr] ( result, ( void * ) x, ( int * ) ( Register[regSS] + Register[regSP] ), num, *( ( unsigned int * ) v2.what ) );
							HEAP_CHECK;
                        }
                        scActual_Instance = Ins;    //in case it changes Actual_Instance (like Kernel call)
                        Register[regSP] += num; //add sp,num
                        if ( scErrorNo == scErr_Violation )
                            rerr ( "Priority violation!" );
                        scErrorNo = scOK;

                        if ( Ins->flags.paused )
                        {       //somebody paused us
                            scActual_Instance = NULL;
                            actspeed = speed;
                            return 0;
                        }

                    }
                    break;
//        deb1(" Return from func=%f\n",*((double *)Register));

                case opvcpuCOPY:
					MEMCHECK;
                    if ( Register[regCX] > 0 )
                        memcpy ( v1.what, v2.what, Register[regCX] );
                    deb0 ( "COPY" );
                    break;

                case opvcpuADD:
					MEMCHECK;
                    DoOper ( int, int, += );
                    deb0 ( "ADD" );
                    break;

                case opvcpuDADD:
					MEMCHECK;
                    DOUBLEFix;
                    DOUBLEdeb1 ( '+' );
                    DoOper ( double, double, += );
                    DOUBLEdeb2;
                    deb1 ( " {%f} ", ( *( ( double * ) v1.what ) ) );
                    deb0 ( "DADD" );
                    break;

                case opvcpuSUB:
					MEMCHECK;
                    DoOper ( int, int, -= );
                    deb0 ( "SUB" );
                    break;

                case opvcpuDSUB:
					MEMCHECK;
                    DOUBLEFix;
                    DOUBLEdeb1 ( '-' );
                    DoOper ( double, double, -= );
                    deb0 ( "DSUB" );
                    DOUBLEdeb2;
                    break;

                case opvcpuCMPE:
					MEMCHECK;
                    DoOper2 ( int, int, == );
                    deb0 ( "CMPE" );
                    break;

                case opvcpuCMPG:
                    if ( Register[regOPTIONS] & optionsvcpuUNSIGNED )
                    {
                        DoOper2 ( unsigned, unsigned, > );
                        Register[regOPTIONS] ^= optionsvcpuUNSIGNED;
                    }
                    else
                        DoOper2 ( int, int, > );
                    deb0 ( "CMPG" );
                    break;

                case opvcpuCMPL:
                    if ( Register[regOPTIONS] & optionsvcpuUNSIGNED )
                    {
                        DoOper2 ( unsigned, unsigned, < );
                        Register[regOPTIONS] ^= optionsvcpuUNSIGNED;
                    }
                    else
                        DoOper2 ( int, int, < );
                    deb0 ( "CMPL" );
                    break;

                case opvcpuCMPNG:
                    if ( Register[regOPTIONS] & optionsvcpuUNSIGNED )
                    {
                        DoOper2 ( unsigned, unsigned, <= );
                        Register[regOPTIONS] ^= optionsvcpuUNSIGNED;
                    }
                    else
                        DoOper2 ( int, int, <= );
                    deb0 ( "CMPNG" );
                    break;

                case opvcpuCMPNL:
                    if ( Register[regOPTIONS] & optionsvcpuUNSIGNED )
                    {
                        DoOper2 ( unsigned, unsigned, >= );
                        Register[regOPTIONS] ^= optionsvcpuUNSIGNED;
                    }
                    else
                        DoOper2 ( int, int, >= );
                    deb0 ( "CMPNL" );
                    break;

                case opvcpuMUL:
					MEMCHECK;
                    DoOper ( int, int, *= );
                    deb0 ( "MUL" );
                    break;

                case opvcpuDMUL:
					MEMCHECK;
                    DOUBLEFix;
                    DOUBLEdeb1 ( '*' );
                    DoOper ( double, double, *= );
                    DOUBLEdeb2;
                    deb0 ( "DMUL" );
                    break;

                case opvcpuDIV:
					MEMCHECK;
                    if ( Register[regOPTIONS] & optionsvcpuUNSIGNED )
                    {
                        DoOper ( unsigned, unsigned, /= );
                        Register[regOPTIONS] ^= optionsvcpuUNSIGNED;
                    }
                    else
                        DoOper ( int, int, /= );
                    deb0 ( "DIV" );
                    break;

                case opvcpuFIXMUL:
					MEMCHECK;
                    *( ( int * ) v1.what ) = fixedmul ( *( ( int * ) v1.what ), *( ( int * ) v2.what ) );
                    deb0 ( "FIXMUL" );
                    break;

                case opvcpuFIXDIV:
					MEMCHECK;
                    *( ( int * ) v1.what ) = fixeddiv ( *( ( int * ) v1.what ), *( ( int * ) v2.what ) );
                    deb0 ( "FIXDIV" );
                    break;

                case opvcpuDFIX:
					MEMCHECK;
                    DOUBLEFix;
                    deb1 ( " {%f} ", ( *( ( double * ) v2.what ) ) );
                    *( ( int * ) v1.what ) = ( int ) ( ( *( ( double * ) v2.what ) ) * 65536 );
                    deb0 ( "DFIX" );
                    break;

                case opvcpuFIXDBL:
					MEMCHECK;
                    *( ( double * ) v1.what ) = *( ( int * ) v2.what ) / 65536.0;
                    deb1 ( " {%f} ", ( *( ( double * ) v1.what ) ) );
                    deb0 ( "FIXDBL" );
                    break;

                case opvcpuDDIV:
					MEMCHECK;
                    DOUBLEFix;
                    DOUBLEdeb1 ( '/' );
                    DoOper ( double, double, /= );
                    DOUBLEdeb2;
                    deb0 ( "DDIV" );
                    break;

                case opvcpuDCMPE:
                    DOUBLEFix;
                    DOUBLEdeb1 ( '=' );
                    DoOper3 ( double, double, == );
                    DOUBLEint2;
                    deb0 ( "DCMPE" );
                    break;

                case opvcpuDCMPG:
					MEMCHECK;
                    DOUBLEFix;
                    DOUBLEdeb1 ( '^' );
                    DoOper3 ( double, double, > );
                    DOUBLEint2;
                    deb0 ( "DCMPG" );
                    break;

                case opvcpuDCMPL:
					MEMCHECK;
                    DOUBLEFix;
                    DOUBLEdeb1 ( '_' );
                    DoOper3 ( double, double, < );
                    DOUBLEint2;
                    deb0 ( "DCMPL" );
                    break;

                case opvcpuDCMPNG:
                    DOUBLEFix;
                    DOUBLEdeb1 ( '#' );
                    DoOper3 ( double, double, <= );
                    DOUBLEint2;
                    deb0 ( "DCMPNG" );
                    break;

                case opvcpuDCMPNL:
                    DOUBLEFix;
                    DOUBLEdeb1 ( '#' );
                    DoOper3 ( double, double, >= );
                    DOUBLEint2;
                    deb0 ( "DCMPNL" );
                    break;

                case opvcpuMOD:
					MEMCHECK;
                    if ( Register[regOPTIONS] & optionsvcpuUNSIGNED )
                    {
                        DoOper ( unsigned, unsigned, %= );
                        Register[regOPTIONS] ^= optionsvcpuUNSIGNED;
                    }
                    else
                        DoOper ( int, int, %= );
                    deb0 ( "MOD" );
                    break;

                case opvcpuAND:
					MEMCHECK;
                    DoOper ( int, int, &= );
                    deb0 ( "AND" );
                    break;

                case opvcpuOR:
					MEMCHECK;
                    DoOper ( int, int, |= );
                    deb0 ( "OR" );
                    break;

                case opvcpuXOR:
                    DoOper ( int, int, ^= );
                    deb0 ( "XOR" );
                    break;

                case opvcpuANDL:
					MEMCHECK;
                    DoOper2 ( int, int, && );
                    deb0 ( "ANDL" );
                    break;

                case opvcpuORL:
					MEMCHECK;
                    DoOper2 ( int, int, || );
                    deb0 ( "ORL" );
                    break;

                case opvcpuSHR:
					MEMCHECK;
                    if ( Register[regOPTIONS] & optionsvcpuUNSIGNED )
                    {
                        DoOper ( unsigned, unsigned, >>= );
                        Register[regOPTIONS] ^= optionsvcpuUNSIGNED;
                    }
                    else
                        DoOper ( int, int, >>= );
                    deb0 ( "SHR" );
                    break;

                case opvcpuSHL:    //does not care if unsigned
					MEMCHECK;
                    DoOper ( int, int, <<= );
                    deb0 ( "SHL" );
                    break;

                case opvcpuIDBL:
					MEMCHECK;
                    DoOper ( double, int, = );
                    deb0 ( "IDBL" );
                    break;

                case opvcpuDINT:
					MEMCHECK;
                    DoOper ( int, double, = );
                    deb0 ( "DINT" );
                    break;

                case opvcpuFDBL:
					MEMCHECK;
                    DoOper ( double, float, = );
                    deb0 ( "FDBL" );
                    break;

                case opvcpuDFLT:
					MEMCHECK;
                    DoOper ( float, double, = );
                    deb0 ( "DFLT" );
                    break;

                case opvcpuJTRUE:
                    if ( *v1.what )
                        Register[regIP] += *v2.what;
                    deb0 ( "JTRUE" );
                    break;

                case opvcpuJFALSE:
                    if ( !*v1.what )
                        Register[regIP] += *v2.what;
                    deb0 ( "JFALSE" );
                    break;

                case opvcpuFORK:
                    if ( ( !( opc[0] & 64 ) ) && *v1.what == -1 )   //exit the forked instance.
                    {           /*delete instance */
                        scFree_Instance ( scActual_Instance );
                        scActual_Instance = NULL;
                        deb0 ( "EXIT" );
                        return 1;   //end of instance
                    }
                    else        //fork this instance.
                    {
                        scInstance *i;  //TODO:
                        i = scFork_Instance ( Ins );
                        scLaunch_Instance_GOTO ( i, speed, Register[regIP] );
                        *v1.what = i->flags.forkno;
                        Register[regIP] = *v2.what;
                    }
                    deb0 ( "FORK" );
                    break;

                default:
                    deb0 ( "Unknown!" );
                    rerr3 ( "Unknown opcode:%d in the script at %d!\n", opc[0] & 63, Register[regIP] - 4 );

            }
            deb0 ( "\n" );
        }
    }
    while ( 1 );
	scError_Exit:
    scActual_Instance = NULL;
    return -1;                  //error happened
}
