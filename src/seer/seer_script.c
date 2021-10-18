/************************************************************************
 SeeR a C-scripting library v 0.95a
 http://przemekp.prv.pl/seer/        dev@przemekp.prv.pl

 File:script.c
 Desc:debug & initialization & other

 Copyright (c) 1998-99,2002, Przemyslaw Podsiadly
 All rights reserved.
 See seer.h for license information.
************************************************************************/
#ifndef __TRIX_SEER_SCRIPT_C__
#define __TRIX_SEER_SCRIPT_C__

#define INTERNAL_FILE
#include <seer.h>
#include "seer_internal.h"
#include <sys/stat.h>
#include "mem.h"


int scErrorNo = scOK;
char scErrorMsg[1024] = "";
char scErrorLine[1024] = "";

bool SeeR_Inited = false;
bool scIgnore_Rights = false;
int scParamCount;
scInstance *scActual_Instance = NULL;
char scScript_Title[128] = "Untitled";
char scScript_Author[128] = "Anonymous";
char *scScript_SeeR = "SeeR (" SeeR_VERSION_STR ") scripting language by Przemyslaw Podsiadly " SeeR_DATE_STR;
int ( *scProgressCallback ) ( int percent ) = NULL;
int seer_stack_size = 3 * 1024*1024;

scFunctionDispatcher scUserFunctionDispatcher[SC_MAX_FUNCTION_DISPATCHERS];

#ifdef DEBUG
FILE *debfile = NULL;
int debug_flag = 0;
#endif

//from callasm.c:
int callfunc ( void *func, void *params, int count );
double callfunc_d ( void *func, void *params, int count );
int callmember ( void *func, void *params, int count );
double callmember_d ( void *func, void *params, int count );

int  scCDispatcher ( int *result, void *function, int *params, int paramcount, unsigned int options )
{
    if ( options & scDispatch_Member )
    {                           //C++ call - convert to typical C call, so 'this' is the first param.
        options ^= scDispatch_Member;   //erase member flag
    }
    //call the default dispatcher
    return scStandardDispatcher ( result, function, params, paramcount, options );
}

int
scSeeR_Get ( int subject, ... )
{
    int i;
    va_list va;
    va_start ( va, subject );
    switch ( subject )
    {
        case scSeeR_Version:
			va_end(va);
            return SeeR_VERSION;

        case scSeeR_Build:
			va_end(va);
#ifdef DEBUG
            return scSeeR_Build_Debug;
#else
            return scSeeR_Build_Release;
#endif

        case scSeeR_LinesCompiled:
			va_end(va);
            return act.lines_compiled;

        case scSeeR_Debugging:
			va_end(va);
#ifdef DEBUG
            return ( debug_flag );
#else
            return 0;
#endif

        case scSeeR_Dispatcher:
            i = va_arg ( va, int );
			va_end(va);
            if ( i < 0 || i >= SC_MAX_FUNCTION_DISPATCHERS )
                return 0;
            return ( int ) scUserFunctionDispatcher[i];

        case scSeeR_ProgressCallback:
			va_end(va);
            return ( int ) scProgressCallback;

        case scSeeR_ParamCount:
			va_end(va);
            return 0;
        case scSeeR_ExprOpcode:
			va_end(va);
            return ( act.options & optEXPR ) ? true : false;
        case scSeeR_GlobalExpression:
			va_end(va);
            return ( act.options & optGLOBALEXPR ) ? true : false;
        case scSeeR_StackSize:
			va_end(va);
            return ( act.stacksize );
    }
	va_end(va);
    return 0;
}

bool
scSeeR_Set ( int subject, ... )
{
    int i = 0;
    void *ptr;
    va_list va;
    va_start ( va, subject );
    switch ( subject )
    {
        case scSeeR_Debugging:
#ifdef DEBUG
            i = va_arg ( va, int );
            debug_flag = ( bool ) i;
#endif
			va_end(va);
            return true;
        case scSeeR_OpenDebug:
            scOpen_Debug ( va_arg ( va, char * ) );
			va_end(va);
            return true;
        case scSeeR_CloseDebug:
            scClose_Debug (  );
			va_end(va);
            return true;
        case scSeeR_Dispatcher:
            i = va_arg ( va, int );
            ptr = va_arg ( va, void * );
            if ( i < 0 || i >= SC_MAX_FUNCTION_DISPATCHERS )
			{
				va_end(va);
                return false;
			}
            scUserFunctionDispatcher[i] = ptr;
			va_end(va);
            return true;
        case scSeeR_ProgressCallback:
            ptr = va_arg ( va, void * );
            scProgressCallback = ptr;
			va_end(va);
            return true;
        case scSeeR_ExprOpcode:
            i = va_arg ( va, int );
            if ( !i )
            {
                if ( act.options & optEXPR )
                    act.options ^= optEXPR;
            }
            else
            {
                if ( !( act.options & optEXPR ) )
                    act.options ^= optEXPR;
            }
			va_end(va);
            return true;
        case scSeeR_GlobalExpression:
            i = va_arg ( va, int );
            if ( !i )
            {
                if ( act.options & optGLOBALEXPR )
                    act.options ^= optGLOBALEXPR;
            }
            else
            {
                if ( !( act.options & optGLOBALEXPR ) )
                    act.options ^= optGLOBALEXPR;
            }
			va_end(va);
            return true;
        case scSeeR_StackSize:
            act.stacksize = va_arg ( va, int );
			va_end(va);
            return true;
    }
	va_end(va);
    return false;
}

void scExport_Stdlib (  );

void
scInit_SeeR (  )
{
    int i;

	// always configured
    scSeeR_Set ( scSeeR_StackSize, seer_stack_size );

	if ( SeeR_Inited )
        return;

    SeeR_Inited = true;
    act.during = duringIDLE;
	scInitAlloc (  );

    for ( i = 0; i < SC_MAX_FUNCTION_DISPATCHERS; i++ )
        scUserFunctionDispatcher[i] = NULL;

    scSeeR_Set ( scSeeR_Dispatcher, 0, scStandardDispatcher );
    scSeeR_Set ( scSeeR_Dispatcher, 1, scCDispatcher );
    scSeeR_Set ( scSeeR_ExprOpcode, 0 );    //faster
    scSeeR_Set ( scSeeR_GlobalExpression, 1 );

    if ( !scGet_Scheduler (  ) )
        scSet_Scheduler ( NULL );
    scAdd_Internal_Header ( "Instances", hdrInstances );
    scExport_Stdlib (  );

//Instances imports:
    scAddExtSym ( scKill_My_Forked_Instances );
    scAddExtSym ( scKill_Forked );
    scAddExtSym ( scGet_Forked_Status );
    scAddExtSym ( scGet_Actual_Priority );

//Done.
}
static int
scGet_Field ( scScript s, char *id )
{
    int a, z;
    if ( !s )
        return 0;
    if ( *( ( int * ) s ) != CHARS2INT ( 'S', 'e', 'e', 'R' ) )
        return 0;               //"Not a SeeR script!\n";
    if ( *( ( int * ) s + 1 ) != CHARS2INT ( 'V', 'C', 'P', 'U' ) )
        return 0;               //"Not a VCPU SeeR script!\n";
    if ( strlen ( id ) != 4 )
        return 0;
    a = ( ( int * ) s )[hdrHeaderSize];
    ALIGN_INT ( a );
    while ( *( ( int * ) ( s + a ) ) != CHARS2INT ( 'E', 'N', 'D', '!' ) )
    {
        deb2 ( "Field %c%c", *( s + a ), *( s + a + 1 ) );
        deb2 ( "%c%c...\n", *( s + a + 2 ), *( s + a + 3 ) );
        if ( *( ( int * ) ( s + a ) ) == CHARS2INT ( id[0], id[1], id[2], id[3] ) )
            return a + 8;
        a += 4;
        z = *( ( int * ) ( s + a ) );
        a += z + 4;
    }
    return 0;
}

char *
scGet_Title ( scScript s )
{
    int i = scGet_Field ( s, "TITL" );
    if ( !i )
        return NULL;
    deb2 ( "%d=%s\n", i, s + i );
    return s + i;
}

char *
scGet_Author ( scScript s )
{
    int i = scGet_Field ( s, "AUTH" );
    if ( !i )
        return NULL;
    return s + i;
}

char *
scGet_Instance_Title ( scInstance * I )
{
    if ( !I )
        return NULL;
    return scGet_Title ( I->code );
}

scInstance *
scGet_This (  )
{
    return scActual_Instance;
}

int
scGet_Script_Size ( scScript scrpt )
{
    if ( !scrpt )
        return 0;
    if ( *( ( int * ) scrpt ) != CHARS2INT ( 'S', 'e', 'e', 'R' ) )
        return 0;
    return ( ( int * ) scrpt )[3];
}

int
STfilesize ( const char *s )
{
    struct stat l;
    stat ( s, &l );
    return l.st_size;
}

scScript
scLoad_Script ( char *s )
{
    int size;
    FILE *file;
    scScript script;
//<ERASED> scKernelOnly(NULL);
    size = STfilesize ( s );
    file = fopen ( s, "rb" );
    if ( !file )
        return NULL;
    script = malloc ( size );
    if ( fread ( script, size, 1, file ) == -1 )
    {
        fclose ( file );
        free ( script );
        return NULL;
    };
    fclose ( file );
    if ( scGet_Script_Size ( script ) != size )
    {
        free ( script );
        return NULL;
    };
    return script;
}

#ifdef DEBUG
void
scToggle_Debug ( bool debon )
{
    debug_flag = debon;
}

void
scOpen_Debug ( char *filename )
{
    scInit_SeeR (  );
    debfile = fopen ( filename, "w" );
	if ( !debfile )
		return;
    debug_flag = true;
    deb0 ( "SeeRC debugger data:\n" );
}

void
scClose_Debug (  )              //also turns debuggin off
{
    debug_flag = 0;
    if ( !debfile )
        return;
    fclose ( debfile );
    debfile = NULL;
}
#else
void
scToggle_Debug ( bool debon )
{
}
void
scOpen_Debug ( char *filename )
{
}
void
scClose_Debug (  )
{
}
#endif

#ifdef MALLOC_DEBUG

#undef malloc
#undef free
#undef realloc

struct
{
    void *addr;
    int num;
    int size;
} allocTable[64000];
int allocTableNum = 0;

void *
my_malloc ( int s )
{
    void *x = malloc ( s );
    allocTable[allocTableNum].addr = x;
    allocTable[allocTableNum].num = 0;
    allocTable[allocTableNum].size = s;
    allocTableNum++;
    return x;
}

void
my_free ( void *s )
{
    int i = 0, n = 0, e = 0;
    for ( i = allocTableNum - 1; i >= 0; i-- )
        if ( allocTable[i].addr == s )
        {
            if ( allocTable[i].num > 0 && n > 0 )
                break;          //already found all relevant
            if ( allocTable[i].num > 0 && n == 0 )
            {
                deb1 ( "\nfreeBUG:already freed %x\n", s );
                e++;
            }
            allocTable[i].num++;
            n++;
        }
    if ( n == 0 )
        deb1 ( "\nfreeBUG:not malloced %x\n", s );
    if ( n > 1 )
        deb1 ( "\nfreeBUG:malloced more than once %x\n", s );
    if ( n == 0 && e == 0 )
        free ( s );
}

void *
my_realloc ( void *p, int s )
{
    void *x = realloc ( p, s );
    int i = 0, n = 0, e = 0;
    for ( i = allocTableNum - 1; i >= 0; i-- )
        if ( allocTable[i].addr == p )
        {
            n++;
            allocTable[i].addr = x;
            allocTable[i].size = s;
            if ( allocTable[i].num > 0 )
            {
                if ( n > 1 )
                    break;
                deb1 ( "\nreallocBUG:reallocing already freed: %x\n", p );
                e++;
            }
            if ( n > 1 )
            {
                deb1 ( "\nreallocBUG:reallocing more than once: %x\n", p );
                e++;
            }
        }
    return x;
}
#endif //MALLOC_DEBUG

#endif