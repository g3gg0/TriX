/************************************************************************
 SeeR a C-scripting library v 0.95a 
 http://przemekp.prv.pl/seer/        dev@przemekp.prv.pl
             
 File:internal.h
 Desc:internal header file

 Copyright (c) 1998-99,2002, Przemyslaw Podsiadly
 All rights reserved.
 See seer.h for license information.
************************************************************************/

#ifndef __SCRIPT_INTERNAL_H__

#define MAXLENGTH  8192
/*
               -processor:
                SC_INTEL,
               -compiler:
                SC_MSVC,
                SC_GCC,
               -system:
                SC_DOS,
                SC_WIN32,
                SC_LINUX,
                SC_BEOS,
                SC_FREEBSD,
                SC_UNIXWARE,
                SC_SCOUNIX,
*/


#define STR_SeeR_Version "95"

#ifdef __cplusplus
extern "C"
{
#endif
#include <stdio.h>
//#include <stdlib.h>
#include <ctype.h>
#include <setjmp.h>
#include "defines.h"

    /*
     * try to detect some things 
     */
/*
#ifdef WIN32
#define SC_INTEL
#define SC_WIN32
#ifdef _MSC_VER
#define SC_MSVC
#endif
#endif

#if defined SC_INTEL            //intel

#define _LITTLE_ENDIAN

#if defined SC_WIN32            //win32

#if defined SC_MSVC             //msvc
#ifndef _MSC_VER
#error Makefile error:MSVC compiler not detected
#endif
#include <string.h>
#define MAXINT 0x7fffffff
#ifdef _DEBUG
#define  DEBUG
#endif
    typedef __int64 int64;
#define INT64SUPPORTED
#else
#error Makefile error:unknown compiler under Win32
#endif

#elif defined SC_DOS || defined SC_BEOS || defined SC_LINUX || defined SC_FREEBSD || defined SC_UNIXWARE || defined SC_SCOUNIX
#if defined SC_GCC              //gcc
#include <strings.h>
#include <values.h>
    typedef long long int64;
#define INT64SUPPORTED

#else
#error Makefile error:unknown compiler under DOS/BEOS/LINUX/FreeBSD/Linux/SCOUnix
#endif

#else
#error Makefile error:unknown OS
#endif                          //system

#elif defined SC_ANY            //any proc/any OS
#if defined SC_GCC              //gcc
#include <strings.h>
#include <values.h>
    typedef long long int64;
#define INT64SUPPORTED
#else                           //any compiler
#include <strings.h>
#include <values.h>
#endif

#else                           //non-intel
#error Non-intel processor set up (seems like makefile error)
#endif

	*/

#include <string.h>
#define MAXINT 0x7fffffff
typedef long int64;

//**************************************************************************

#define _SHOW_MACHINE_CODE

#define  EXACT_DEBUG
//#define  MALLOC_DEBUG
//fflush every debX... - slows down everything as hell

//#define DEBUG

//When DEBUG is on, the DEBUGLEVEL says:
//0 - log all
//1 - some are skipped - declare.c
//2 ? more skipped
//3 ? "clean" - assembler and some comments
//4 - "clean" - only assembler + lines
//5 - "clean" - only assembler
#define DEBUGLEVEL 0

#ifdef MALLOC_DEBUG
#define malloc my_malloc
#define free my_free
#define realloc my_realloc

    extern void *my_malloc ( int s );
    extern void my_free ( void *p );
    extern void *my_myrealloc ( void *p, int s );
#endif

#ifdef DEBUG
#define debflush()  fflush(debfile)
    extern int debug_flag;
    extern FILE *debfile;
#ifdef EXACT_DEBUG
#define adeb0(x) {if (debug_flag){printf(x);}}
#define adeb1(x,y) {if (debug_flag){printf(x,y);}}
#define adeb2(x,y,z) {if (debug_flag){printf(x,y,z);}}
#define adeb3(x,y,z,q) {if (debug_flag){printf(x,y,z,q);}}
#else                           //!EXACT_DEBUG
#define adeb0(x) {if (debug_flag){fprintf(debfile,x);}}
#define adeb1(x,y) {if (debug_flag){fprintf(debfile,x,y);}}
#define adeb2(x,y,z) {if (debug_flag){fprintf(debfile,x,y,z);}}
#define adeb3(x,y,z,q) {if (debug_flag){fprintf(debfile,x,y,z,q);}}
#endif


#if DEBUGLEVEL >= 3
//clean
#define deb0(x)
#define deb1(x,y)
#define deb2(x,y,z)
#define deb3(x,y,z,q)
#else
//normal
#define deb0 adeb0
#define deb1 adeb1
#define deb2 adeb2
#define deb3 adeb3
#endif
//with gcc this one would work:
//#define deb(x...) {if (debug_flag)fprintf(debfile,x);}

#else                           //!DEBUG:

#define debflush()
#define adeb0(x) while (0) { };
#define adeb1(x,y) while (0) { };
#define adeb2(x,y,z) while (0) { };
#define adeb3(x,y,z,q) while (0) { };

#define deb0 adeb0
#define deb1 adeb1
#define deb2 adeb2
#define deb3 adeb3
//#define deb(x...)
//#define debprintf(x...)
#define debif(x)
#endif

//#endif


//#ifdef __GNUC__
#ifdef _LITTLE_ENDIAN
#define CHARS2INT(a,b,c,d) ((d<<24)+(c<<16)+(b<<8)+(a))
#else
#define CHARS2INT(a,b,c,d) ((a<<24)+(b<<16)+(c<<8)+(d))
#endif

#ifndef DIMENSION
#define DIMENSION(x)    ((int)sizeof(x)/sizeof(x[0]))
#endif
#define TRUE 1
#define FALSE 0

#define scAssert(func,v) if(!(v)) serr3(scErr_Internal, internal_compiler_error," <"#func":"#v"> "__FILE__,__LINE__);
#define scError(func) serr3(scErr_Internal, internal_compiler_error," <"#func"> "__FILE__,__LINE__);

#define IsSymbol(c) (((c)>' '&&(c)<'0')||((c)>'9'&&(c)<'A')||\
                    ((c)>'Z'&&(c)<'_')||((c)>'z'&&(c)<127)||c=='`')

#define STALine ST->lines[ST->act_line]

#define New(T) scAlloc(sizeof(T))

#define lexeq(a) (strcmp(ST->lexema,a)==0)

#define Advance if (!scget_lexema(ST)) {serr(scErr_Parse,"Parse error");}

#define ALIGN_INT(a) {if (((unsigned int)a) % 4) a += (4- (((unsigned int)a) % 4));}


/* scScript - seer script header (as (int []))*/
#define hdrAllSize              3
#define hdrHeaderSize           4
#define hdrSymbolOffset         5
#define hdrCodeOffset           6
#define hdrDataOffset           7
#define hdrConstsOffset         8
#define hdrInitOffset           9
#define hdrCodeSize             10
#define hdrDataSize             11
#define hdrConstsSize           12
#define hdrStackSize            13
#define hdrImportedOffset       14
#define hdrConstructOffset      15
#define hdrDestructOffset       16
//EOH! is next                  17
#define hdrHeaderSizeOf         18*4
//**************************************************************************

    struct scSymbol;
    struct _value;
//PREDEFINED TYPES,returned by scvalGetValType
    typedef enum valTYPE
    {
        valINT,
        valLONG,
        valCHAR,
        valFIXED,
        valFLOAT,
        valDOUBLE,
        valSHORT,
        valREGISTER,
        valVOID,
        //not predefined types:
        valPOINTER,
        valSTRUCT,
        valSCRIPT,
    } valTYPE;
//TYPE descriptor
    enum
    {
        typTYPE = 1 + 2 + 4 + 8,    //switch(flags&typTYPE) {case typPREDEFINED:}
        typPREDEFINED = 0,      //in main :index of basic types
        typUSERDEFINED = 1,     //in main:pointer to symbol with type
        typSTRUCT = 2, typUNION = 3, typENUM = 4,   //in main there is a list
        typSCRIPT = 8,          //alike struct (in main)
        typINVALID = 15,

        typOPTION = 16 + 32 + 64 + 128, //option standing before
        typCONST = 16,
        typUNSIGNED = 32,
        typSTATIC = 64,
        typREGISTER = 128,
        typCONSTRUCTOR = 256,
        typDESTRUCTOR = 512,
/*We don't need these, do we?
      typSIZE=512+1024+2048,
      typDEFAULT=0,
      typSHORT=512,typLONG=1024,
*/
    };

    typedef struct scType
    {
        int flags;              //typPREDEFINED...
        char *params;           //it is e.g.  (1aff32a) or **[12]* (p->p->table[12]of p->T
        //what about function
        struct scSymbol *main;  //Main type or Table with scSymbol, last has flags==-1(structs)
    } scType;

    typedef enum scAddrFlag
    {
        adrPOINTER = 1 + 2 + 4,
        adrSTOREPOINTER = 1 + 2,
        adrES = 1,
        adrDS = 2,
        adrBP = 3,
        adrCPU = 4,
        adrSTORECPU = adrCPU & adrSTOREPOINTER,
        adrCS = 5,
        adrIMMIDIATE = 8,
        adrREGISTER = 16,
        adrIMPORTED = 32,
        adrTYPEONLY = 64,
        adrSCRIPT = 128,
        adrADDRESS = 256,       //we have eg [ES+12], but we should have address of it
        //,like a0=ES+12; with this we use PUSHADR etc.
        //with * or [] we need only to delete this flag.
        //for address we have to scvalPutToRegister.
    } scAddrFlag;
/*
 a0    adrREGISTER
 12    adrIMMIDIATE
 [CS+a0]adrREGISTER|adrCS
*/

    typedef struct scAddress
    {
        int address;            //here we store offset, register, but not value!
        scAddrFlag flags;
        union
        {
            int ival;
            float fval;
            double dval;
        } val;                  //typically value,
        //but when imported, ival stores dispatcher number
    } scAddress;

    typedef struct scSymbol
    {
        scAddress adr;          //copy it to val
        char *name;
        char *import_name;      //usually NULL
//        int used;
        int params;             //size of params
        struct _value *val;
        scType type;
        struct scSymbol *next;  //TODO:here should be a place for parent
        struct scSymbol *parent;
        void *reserved;
    } scSymbol;

    typedef struct
    {
        scAddress adr;          //IMPORTED flag must be correct
        scType type;
        scSymbol *sym;
    } _value;
/*  Examples of description
                Adres:Adres,flags,val        Type:flags,params,main
int x;  _value={{12,adrDS|adrIMMIDIATE,0}, {typPREDEFINED,NULL,valINT},x}
a5      _value={{5,adrREGISTER,0}          {typPREDEFINED,NULL,valINT},x}
[DS+a4] _value={{4,adrDS|adrREGISTER,0},   {typSTRUCT,NULL,xxxx},xxx}
[12]    _value={{12,adrIMPORT,0},          {typPREDEFINED,NULL,valINT},x}
int *x; _value={{12,adrDS|adrIMMIDIATE,0}, {typPREDEFINED,"*",valINT},x}
*/
//**************************************************************************
//vars.c:
    bool sciValidIdentifier ( char *name );
    bool sctypIsReference ( scType * type );
    int sctypSizeOf ( scType * type );
    int scvalSizeOf ( _value * val );
    void sctypTypeOfStr ( scType * type, char *name, char *buf );
    int sctypInsistsAlignment ( scType * type );
    void scvalFreeValue ( _value * v );
    int scvalFindFreeRegister ( _value * val );
    int scvalFindFreeDoubleRegister ( _value * val );
    valTYPE scvalGetValType ( _value * v );
    valTYPE sctypGetValType ( scType * type );
    void scvalPutToRegister ( _value * val, int freeit );
    void scvalArrayToPointer ( _value * v );
    char *sciValTypeStr ( valTYPE v );
    bool sciValTypeFloating ( valTYPE fl );
    bool scvalIsOfType ( _value * v, valTYPE vt );
    bool sctypIsFunction ( scType * type );
    bool scvalIsImmidiate ( _value * v );
    bool scvalIsImmidiateOfType ( _value * v, valTYPE vt );
    double scvalImmidiateValue ( _value * v );
    _value *scvalSetImmidiate ( _value * v, valTYPE vt, int ival, double dval );
    _value *scvalSetINT ( _value * v, int ival );
    _value *scvalSetRegister ( _value * v, valTYPE vt, int reg );
    _value *scvalSetSymbol ( _value * v, scSymbol * s );
    void scvalGetAddress ( _value * val, int situ );
    void scvalAccessReference ( _value * val );
    void sctypGoDeeper ( scType * typ );
    bool scvalSameType ( _value * val1, _value * val2 );
    void scvalConvToDouble ( _value * val );
    void scvalConvToFloat ( _value * val );
    void scvalConvToInt ( _value * val );
    void scvalConvToFixed ( _value * val );
    void scvalCast ( _value * val, scSymbol * sc );
    void scvalDisplayValue ( char *x, _value * v );
    void sctypSimplifyType ( scType * type );
    void sciAddImportedFunction ( scSymbol * sym, scSymbol * in_struct );
    int sciReadIntegerInBrackets ( int def );
    void sciCountFunctionParams ( scSymbol * sym, scSymbol * in_struct, int Add_Params_To_Dict );
    void scvalBothTypes ( _value * v1, _value * v2 );
    scSymbol *sctypGetStruct ( scType * typ );
    void scvalConstantString ( _value * val, char *s, int len );
//expr.c:
    void scvalCallFunction ( scSymbol * sym, _value * val, scSymbol * in_struct, _value * thisval, _value * scriptval, _value * params, bool CallerIgnore );
    _value *scvalReadParameters (  );
    scSymbol *scsFind_Function ( char *name, scSymbol * find_among, scSymbol * also_find, _value * params, char *desc );
    scSymbol *sctypFindExactFunction ( char *name, scSymbol * find_among, scSymbol * also_find, scType * params );
    bool sctypExactFunction ( scType * t1, scType * t2 );
    bool scsTypesExact ( scType * t1, scType * t2 );
    int sciReadImport ( char **import_name );
    int *scvalPushUsedRegisters (  );
//void scvalPushUsedRegisters(int *regs);
    void scvalPopUsedRegisters ( int *regs );
    void sciAdjustScriptConstructor ( int level );



//**************************************************************************
//int sciReadSubExp(int i,_value *val,bool ExpRequired);


//**************************************************************************


    typedef struct
    {
        scSymbol *first;
    } scDictionary;

    void scdicAddSymbol ( scDictionary * dict, char *nam, int addr, int typeflags, void *reserved );
    void scdicAddSymbol_Ex ( scDictionary * dict, scSymbol * s );
    void scdicAddSymbolToEnd_Ex ( scDictionary * dict, scSymbol * s );
    void scdicRemoveSymbol ( scDictionary * dict, char *nam );
//before store and restore you can only add symbols to the dict ,else
//horrible crash possible!
    scSymbol *scdicStoreDictionary ( scDictionary * dict );
    void scdicRestoreDictionary ( scDictionary * dict, scSymbol * frst, void Remove ( scSymbol * ) );
    void scdicFreeFunctionSymbol ( scSymbol * );

    scSymbol *scdicFoundSymbol ( scSymbol * first, char *nam );
	scSymbol *scdicFoundSymbolByAddr ( scSymbol * first, void *addr );
    scDictionary *scdicNewDictionary (  );
    scDictionary *scdicDeleteDictionary ( scDictionary * l );

//**************************************************************************

//source
    struct Source_Text;
    typedef struct
    {
        struct Source_Text *ST;
        int lin, chr;
    } scPosition;

    typedef struct Source_Text
    {
        char **lines;
        char **filename;
		int *linenum;
        int act_line, act_char;
        int num_lines, alloced_lines;
        int erase;              //permission for deletion after use
        char name[MAXLENGTH];
        int linedif;
        unsigned char lexema[MAXLENGTH];
		int lexema_len;
        int lextype;            //==0 - unknown
        int lexstart;
        int whitespaces;        //before readed lexema.
        int newline;            //if newline apeared
        scPosition lastpos;
    } Source_Text;


    void scSetPosition ( scPosition n );
    void scGetPosition ( Source_Text * ST, scPosition * n );

    Source_Text *new_Source_Text (  );
    void delete_Source_Text ( Source_Text * );
    Source_Text *scPreprocessor ( Source_Text * );

    bool scomit_whitespaces ( Source_Text * ST );
    bool scget_lexema ( Source_Text * );
    char *scget_line ( Source_Text * ST, int ignore_whitespaces );
    void scSwap ( char *temp, char *source, char *toins, int beg, int end );
    void scget_back ( Source_Text * ST );

    int read_integer ( Source_Text * ST, char *s, int *integ );
    int getStructMemberFromName ( char *name, scSymbol ** in_struct );

//**************************************************************************
    char *scStrdup ( char *x );
    void *scAlloc ( int size );
    void *scRealloc ( void *mem, int size );
    void scFree ( void *x );
    void scInitAlloc (  );
    void scCleanUp (  );
    void scmemTestUp (  );

//**************************************************************************
    scScript scCompile ( Source_Text * ST, int *length );

    void scScript_Error ( Source_Text * ST, char *filename, int linenum, int err_num, char *er, ... );
/*************************ERROR HANDLING********************************
MACROS:
check_error
***********************************************************************/
/*
#define serr(num,x) {scScript_Error(ST,__FILE__,__LINE__,num,x);goto scError_Exit;}
#define serr2(num,x,y) {scScript_Error(ST,__FILE__,__LINE__,num,x,y);goto scError_Exit;}
#define serr3(num,x,y,z) {scScript_Error(ST,__FILE__,__LINE__,num,x,y,z);goto scError_Exit;}
#define serr4(num,x,y,z,q) {scScript_Error(ST,__FILE__,__LINE__,num,x,y,z,q);goto scError_Exit;}
#define cerr {if (scErrorNo) goto scError_Exit;}
#define rerr(x) {scRuntime_Error(x);goto scError_Exit;}
#define rerr2(x,y) {scRuntime_Error(x,y);goto scError_Exit;}
*/
#define serr(num,x) {scScript_Error(ST,__FILE__,__LINE__,num,x);longjmp(act.jump,1);}
#define serr2(num,x,y) {scScript_Error(ST,__FILE__,__LINE__,num,x,y);longjmp(act.jump,1);}
#define serr3(num,x,y,z) {scScript_Error(ST,__FILE__,__LINE__,num,x,y,z);longjmp(act.jump,1);}
#define serr4(num,x,y,z,q) {scScript_Error(ST,__FILE__,__LINE__,num,x,y,z,q);longjmp(act.jump,1);}
//#define cerr {}
#define rerr(x) {scRuntime_Error(x);goto scError_Exit;}
#define rerr2(x,y) {scRuntime_Error(x,y);goto scError_Exit;}
#define rerr3(x,y,z) {scRuntime_Error(x,y,z);goto scError_Exit;}
//Language definition,FIRST THE LONGEST!:

    typedef struct
    {
        char *name;
        int size;
    } NameNInt;

    typedef struct
    {
        char *name;
        int pars;
    } NameNPar;

    enum
    { regCS = 240, regDS, regES, regSS, regIP, regSP, regBP,
        /*
         * counter=1
         *//*
         * FORKED N.
         */
        regCX, regCP, regIE, regFN, regOPTIONS
    };
    /*
     * Call Stack Ptr
     */

    void real_Gen_Opcode ( int opcode, _value * v1, _value * v2, int line, unsigned char *file );
    void Gen_Align (  );

#define Gen_Opcode(x,y,z) real_Gen_Opcode(x,y,z, (ST->act_line<=ST->num_lines)?ST->linenum[ST->act_line]:0,  (char*)((ST->act_line<=ST->num_lines)?(ST->filename[ST->act_line]):NULL))

//*************************************************************************

    typedef struct
    {
        char *mem;
        int size;
        int pos;                //used
    } pmem;

    int new_pmem ( pmem * mem, int init_size );
    void free_pmem ( pmem * );
    void pop_pmem ( pmem * mem, int count );
    void push_pmem ( pmem * mem, int size, void *data );
    void pushint_pmem ( pmem * mem, int x );
    void pushstr_pmem ( pmem * mem, char *s, int len );
    void align_pmem ( pmem * mem );

//**************************************************************************
    typedef struct
    {
        _value *used;           //if used=NULL, it can be temporary!
        int locked;
    } RegsTable[256];

#define duringIDLE                      0
#define duringPREPROCESSING             1
#define duringCOMPILING                 2

#define optEXPR                         1
#define optGLOBALEXPR                   2

    typedef struct
    {
        int during;
        RegsTable registers;
        int afterbp, dataused;  //first free byte in data segment (used by global vars)
        int stacksize;
        int ip;                 //exact copy of code.pos
        int unreal;             //the code generated may not apear on output!
        int lines_compiled;
        jmp_buf jump;
        int options;
        int lastInitIp, lastFuncIp;
        pmem code, inits, consts, final, imports;
    } ActData__;

//**************************************************************************
//vcpudeco:
    extern scFunctionDispatcher scUserFunctionDispatcher[SC_MAX_FUNCTION_DISPATCHERS];

    int Executor ( scInstance * Ins, int speed );
//other:
    void scInit_SeeR (  );
    int scLaunch_Instance_GOTO ( scInstance * inst, int spd, int address );

//**************************************************************************
//variables:

//extern scScript *code;

    extern pmem scExternals;    //name\0addr...til name[0]==0;
    extern scDictionary *scInternalHeaders;

    extern char *Special_Symbols[];
    extern NameNInt Standard_Types[];
    extern NameNPar opcodes_vcpu[];
    extern char *Keywords[];

    extern char *parse_error_concatstring, *parse_error, *invalid_operand_to, *illegal_cast, *internal_compiler_error, *illegal_type_conversion;
//compiler
    extern Source_Text *ST;
    extern ActData__ act;
    extern scDictionary *SC_types, *SC_symbols, *SC_temporary;
//static still scDictionary *SC_exports;

//declare
    scSymbol *sctypGetDefinition (  );
    scSymbol *sctypGetShortDefinition ( scType * typ );
    scSymbol *scsGetTypeDeli (  );
//expr
    int Read_Expression ( _value * val, bool RequiredExp );
    int sciReadSubExp ( int i, _value * val, bool ExpRequired );

//initial
    void sciReadInitialization ( scSymbol * deli, scSymbol * sym );

    extern scSymbol this_symbol;
    extern char *hdrInstances;
    extern char *hdrKernel;

    extern bool SeeR_Inited;
//**************************************************************************

#ifdef __cplusplus
}
#endif

#endif
