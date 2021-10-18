/************************************************************************
 SeeR a C-scripting library v 0.95a
 http://przemekp.prv.pl/seer/        dev@przemekp.prv.pl

 File:seer.h
 Desc:main header file

Copyright (c) 1998-99,2002, Przemyslaw Podsiadly
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. The names of ite author may not be used to endorse or promote products
   derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR "AS IS" AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
************************************************************************/
#ifndef __TRIX_SEER_H__

#ifdef __cplusplus
extern "C"
{
#endif
//#define SC_STRING_UNUSED
#include <stdarg.h>

#if defined WIN32 && !defined GCC_WIN32
//*****************************VC
#ifdef INTERNAL_FILE
#define EXTERN __declspec(dllexport)
#else
#define EXTERN __declspec(dllimport)
#endif

#else
//*****************************GCC
#define EXTERN extern
#endif

/**************************  SeeR INFO ***********************************/
#define SeeR_VERSION          95    /* SeeR_VERSION/100.0 */
#define SeeR_VERSION_STR      "0.95a"
#define SeeR_DATE             20020801  /* yyyymmdd */
#define SeeR_DATE_STR         "2002-08-01"

#ifndef __cplusplus
    typedef enum bool
    { false, true } bool;
#endif
    typedef char *scScript;

/*
   Here you can check some information on current state of SeeR.
   e.g.
   printf("%d lines compiled.\n",scSeeR_Get(scSeeR_LinesCompiled));
*/
/* Get Only */
#define scSeeR_Version            0 /* Returns SeeR_VERSION */

#define scSeeR_Build              1 /* Build version, returns: */
#define scSeeR_Build_Release      1 /* SeeR may not debug, but is fastest */
#define scSeeR_Build_Debug        2 /* if SeeR may generate debug files */

#define scSeeR_LinesCompiled      2 /* Number of lines compiled recently */
#define scSeeR_ParamCount         3 /* Size in bytes of parameters passed to imported function */

/* Set/Get */
#define scSeeR_SafeMode           1000  /* SG If SeeR restricts user from using some     */
    /*
     * unsafe features (like pointers - in future)
     */
    /*
     * UNIMPLEMENTED
     */
#define scSeeR_Debugging          1001  /* SG If debugging is on                         */
#define scSeeR_OpenDebug          1002  /* S  Open debug file, debugging on,             */
    /*
     * parameter:filename
     */
#define scSeeR_CloseDebug         1003  /* S  Close file,Stop debugging                  */
#define scSeeR_Dispatcher         1004  /* SG function dispatcher,                       */
    /*
     * parameters:
     */
    /*
     * int nr,scFunctionDispatcher fun
     */
#define scSeeR_ProgressCallback   1005  /* SG progress callback                          */
    /*
     * parameter:
     */
    /*
     * int (*fun)(int percent)
     */
#define scSeeR_ExprOpcode         1100  /* SG output statement separator opcode (EXPR)?  */
    /*
     * false - is faster, but in multitasking script
     */
    /*
     * may not be interrupted every X instructions
     */
    /*
     * parameter:
     */
    /*
     * bool EXPR
     */
    /*
     * default:false
     */
#define scSeeR_StackSize          1101  /* SG set stack size in bytes                    */
    /*
     * parameter:
     */
    /*
     * int stack_size_in_bytes
     */
    /*
     * default:4000
     */
#define scSeeR_GlobalExpression   1201  /* SG global expressions allowed                 */
    /*
     * parameter:
     */
    /*
     * bool allowed
     */
    /*
     * default:true
     */
    int scSeeR_Get ( int, ... );    //these "..." reserved for future parameters
    bool scSeeR_Set ( int, ... );   //returns true, if set successful

    void scInit_SeeR (  );

/**************************Data definitions*******************************/
    EXTERN char scScript_Title[128], scScript_Author[128], *scScript_SeeR;
    EXTERN int ( *scProgressCallback ) ( int percent );

    typedef struct scFlags
    {
        int priority;
        int forkno;             //0 if not a forked instance
        int speed;
        int paused;
    } scFlags;

    typedef struct scInstance
    {
        scScript code;
        char *data_stack;       //here, first 256 ints are registers,then data, then stack
        scFlags flags;
        struct scInstance *forks, *next_fork, *forked;
        struct scInstance *next;
    } scInstance;

    typedef struct scScheduler
    {
        scInstance *first;
    } scScheduler;

//Script type info
    typedef struct
    {
        int regDS;              //for variable access-the address of global variables
        scInstance *ins;        //the instance script is refering to
        int reserved;
    } scScriptTypeHeader;

#define SC_MAX_FUNCTION_DISPATCHERS 16

#define scDispatch_Void         ((unsigned)1<<21)
#define scDispatch_Struct       ((unsigned)((1<<24)|(1<<23)|(1<<22)))
#define scDispatch_Script       ((unsigned)1<<25)
#define scDispatch_Double       ((unsigned)1<<26)
#define scDispatch_Member       ((unsigned)1<<27)
    typedef void ( *scFunctionDispatcher ) ( int *result, void *function, int *params, int paramcount, /*void *_this, */ unsigned int options );
    void scStandardDispatcher ( int *result, void *function, int *params, int paramcount, unsigned int options );
    void scCDispatcher ( int *result, void *function, int *params, int paramcount, unsigned int options );

/**************************Errors*****************************************/

    enum
    {
        scOK = 0,
        scErr_Parse = 1,
        scErr_Declaration = 2,
        scErr_CodeGen = 3,
        scErr_Operand = 4,
        scErr_Cast = 5,
        scErr_Runtime = 101,
        scErr_Violation = 102,
        scErr_Internal = 255
    };

    EXTERN int scErrorNo;
    EXTERN char scErrorMsg[1024];
    EXTERN char scErrorLine[1024];
//throw an error connected with scripts:
    void scRuntime_Error ( char *er, ... );

/**************************Instances**************************************/
    scInstance *scCreate_Instance ( scScript, char *, ... );
    void scFree_Instance ( scInstance * sci );
    scInstance *scGet_This (  );    //returns scActual_Instance
    bool scAssign_Instance ( scScriptTypeHeader * script, scInstance * ins, char *typeinfo );
    EXTERN scInstance *scActual_Instance;
    EXTERN int scParamCount;

/**************************Compilation************************************/
    scScript scCompile_File ( char *, int * );
    scScript scCompile_Text ( char *, int * );
    scScript scLoad_Script ( char * );
    void scFree_Script ( scScript x );

/**************************Running and conversation***********************/
    int scStart_Instance ( scInstance *, ... ); // calls main(...) in script

    int scCall_Instance ( scInstance *, int address, ... );
    int scVCall_Instance ( scInstance * inst, int address, int paramc, int *params );
    //paramc is size of params in bytes!
    int scGet_Symbol ( scInstance *, char * );

#define scVar(inst,adr,typ) (*((typ*)((inst)->data_stack+256*4+(adr))))
#define scPtr(inst,adr,typ) (((typ*)((inst)->data_stack+256*4+(adr))))
#define scNameVar(ins,name,typ) scVar(ins,scGet_Symbol(ins,name),typ)

/**************************Externals**************************************/

    void scAdd_Internal_Header ( char *, char * );
    void scAdd_External_Symbol ( char *, void * );
    void scAdd_External_Symbol_Int ( char *name, void *addr );
	char *scGet_External_Symbolname_By_Addr ( void *addr );
    void scAdd_Member_Symbol ( char *name, ... );
#define scAddExtSym(a) scAdd_External_Symbol(#a,&a);
#define scAddExtSymInt(a) scAdd_External_Symbol_Int(#a,&a);
#define add_symbol(a) add_symbol_int(#a,&a);
#define seer_add_symbol(a) seer_add_symbol_int(#a,&a);
    void scExport_Kernel (  );

/**************************MULTITASKING**********************************/
    scScheduler *scGet_Scheduler (  );
    scScheduler *scSet_Scheduler ( scScheduler * ); //returns previous scheduler,
    //if you pass NULL, it'll create a new one

    int scVLaunch_Instance ( scInstance * inst, int spd, int address, int paramc, int *params );
    //paramc is size of params in bytes!
    int scLaunch_Instance ( scInstance * inst, int spd, int address, ... );
    int scContinue_Instances (  );  //returns number of instances remaining to run

    void scPause_Instance ( scInstance *, int );    //pause if (int)
    void scKill_Instance ( scInstance * );

    scInstance *scGet_Forked ( scInstance *, int );
    scInstance *scFork_Instance ( scInstance * );
    void scKill_Forked_Instances ( scInstance * );

/****************Priorities**********************************************/
//can call all imported kernel API function
#define scrights_KERNEL 256
//cannot ---
#define scrights_USER     0
//when no instance is being run:
#define scrights_TOP MAXINT

/****************Status of instance**************************************/
#define scstatus_FREE    0
#define scstatus_RUNNING 1
#define scstatus_PAUSED  2
    int scGet_Instance_Status ( scInstance * );

/*****************Operating on Actual_Instance***************************/
    scInstance *scGet_Actual_Forked ( int );
    int scGet_Forked_Status ( int );
    void scKill_My_Forked_Instances (  );
    void scKill_Forked ( int );

/**************************Information ***********************************/
    int scGet_Script_Size ( scScript );
    char *scGet_Title ( scScript );
    char *scGet_Author ( scScript );
    char *scGet_Instance_Title ( scInstance * );

    char *scGet_Instance_DataSeg ( scInstance * i, int *size );
    int scGet_Instance_State_Size ( scInstance * i );
    int scGet_Instance_State ( scInstance * i, char *buf );
    bool scSet_Instance_State ( scInstance * i, char *buf );

/*********************DEBUGGING******************************************/
    void scToggle_Debug ( bool debon );
    void scOpen_Debug ( char *filename );
    void scClose_Debug (  );    //also turns debuggin off

/*********************PRIORITIES*****************************************/
    EXTERN bool scIgnore_Rights;
    void scSet_Priority ( scInstance *, int priority_level );
    int scGet_Priority ( scInstance * );    //if NULL returns scNONE
    int scGet_Actual_Priority (  );

//These macros are to check if an instance calling a function containing
//them has at least required priority - if not it'll set
//an error and return rets (if function returns void, rets should be `;')
#define scPriorityGuard(priority,rets) \
        if ((scGet_Actual_Priority()<priority)&&!scIgnore_Rights)\
        {\
        scRuntime_Error("Priority violation at instance `%s'",\
        scGet_Instance_Title(scActual_Instance));\
        scErrorNo=scErr_Violation; return rets;\
        }

#define scKernelOnly(rets) scPriorityGuard(scrights_KERNEL,rets);

//#ifndef SC_STRING_UNUSED
#ifdef __cplusplus
    struct scintString
    {
        char *cstr;
    };
#else
    typedef struct
    {
        char *cstr;
    } scintString;
#endif

#ifndef SC_STRING_UNUSED
// string(char *x);
    void string_string_char ( scintString * _this, const char *x );

// string();
    void string_string ( scintString * _this );

    scintString *string_operatorSET_int ( scintString * _this, const int x );
    void string_string_int ( scintString * _this, const int x );

//" c_import ~string();\n"
    void string_destroy ( scintString * _this );

//" c_import string(string& f);\n"
    void string_copy ( scintString * _this, scintString * f );
//" c_import string& operator=(string& s);\n"
// a=x=s; <=> a.operator=(x.operator=(s));
    scintString *string_operatorSET ( scintString * _this, const scintString * f );
//" c_import string& operator=(char * f);\n"
    scintString *string_operatorSET_char ( scintString * _this, const char *f );
//" c_import string operator+(string& x);\n"
// a=x+s; <=> a=x.operator+(s); <=> a.operator=(x.operator+(s));
    scintString string_operatorPLUS ( scintString * _this, const scintString * x );

//" c_import string operator+(char* x);\n"
    scintString string_operatorPLUS_char ( scintString * _this, const char *x );

//" c_import string operator()(int start,int len);\n"
    scintString string_sub ( scintString * _this, int start, int len );

//" c_import bool operator<?>(string& x);\n"
    int string_cmp ( const scintString * _this, const scintString * x );
//" c_import bool operator==(string& x);\n"
    bool string_eq ( const scintString * _this, const scintString * x );
//" c_import bool operator==(char* x);\n"
    bool string_eq_char ( const scintString * _this, const char *x );
//" c_import bool operator>(string& x);\n"
    bool string_gt ( const scintString * _this, const scintString * x );
//" c_import bool operator<(string& x);\n"
    bool string_lt ( const scintString * _this, const scintString * x );
//" c_import bool operator>=(string& x);\n"
    bool string_ge ( const scintString * _this, const scintString * x );
//" c_import bool operator<=(string& x);\n"
    bool string_le ( const scintString * _this, const scintString * x );

//" c_import void del(int start,int len);\n"
    void string_del ( scintString * _this, int start, int len );

//" c_import void insert(int start,string& x);\n"
    void string_insert ( scintString * _this, int start, scintString * x );

//index of first found occurance, or -1 if not found
    int string_find ( scintString * _this, scintString * x );

//returns number of replaces
    int string_replace ( scintString * _this, scintString * what, scintString * with, bool global );

//length of string
    int string_length ( scintString * _this );

//" c_import void operator+=(string& x);\n"
// a+=x; <=> a.operator+=(x);
    void string_append ( scintString * _this, scintString * x );
    void string_append_char ( scintString * _this, char *x );
//" c_import int operator(int)();\n"//toint
    int string_toInt ( scintString * _this );
//" c_import char operator[](int idx);\n"
    char *string_operatorIDX ( scintString * _this, int idx );

    void string_reconstruct ( scintString * _this );
#endif



#ifdef __cplusplus

/*********************C++ INTERFACE**************************************/
    enum
    { scriptLoad, scriptCompileFile, scriptCompileText };

    class seerScript
    {
      protected:
        scScript script;
      public:

        seerScript (  ):script ( NULL )
        {
        }

        seerScript ( int a, char *text ):script ( NULL )
        {
            switch ( a )
            {
                case scriptLoad:
                    script = scLoad_Script ( text );
                    break;
                    case scriptCompileText:script = scCompile_Text ( text, NULL );
                    break;
                    case scriptCompileFile:script = scCompile_File ( text, NULL );
                    break;
            }
        }

         ~seerScript (  )
        {                       /*free(); */
        }

        void free ( )
        {
            if ( script )
                scFree_Script ( script );
        }

        int size (  )
        {
            return scGet_Script_Size ( script );
        }

        char *title (  )
        {
            return scGet_Title ( script );
        }

        char *author (  )
        {
            return scGet_Author ( script );
        }

        scScript get (  )
        {
            return script;
        }

        int compileFile ( char *filename )
        {
            script = scCompile_File ( filename, NULL );
            return scErrorNo;
        }

        int compileText ( char *text )
        {
            script = scCompile_Text ( text, NULL );
            return scErrorNo;
        }

        bool load ( char *filename )
        {
            script = scLoad_Script ( filename );
            return script != NULL;
        }

        bool valid (  )
        {
            if ( !script || !size (  ) )
                return false;
            return true;
        }
    };

#define seerVar(inst,adr,typ) (*(typ*)inst.var(adr))
#define seerNameVar(ins,name,typ) (*(typ*)inst.var(name))

    class seerInstance
    {
      protected:
        scInstance * ins;
      public:

        seerInstance (  )
        {
            ins = NULL;
        }

        seerInstance ( scScript sc )
        {
            ins = scCreate_Instance ( sc, "" );
        }

        seerInstance ( scInstance * i )
        {
            ins = i;
        }
//  seerInstance(seerInstance& i) //make fork
//    {ins=scFork_Instance(i.ins);}

        void free (  )
        {
            if ( ins )
                scFree_Instance ( ins );
        }

        ~seerInstance (  )
        {                       /*free(); */
        }


        scInstance *get (  )
        {
            return ins;
        }

        int symbol ( char *s )
        {
            return scGet_Symbol ( ins, s );
        }

        int status (  )
        {
            return scGet_Instance_Status ( ins );
        }

        char *title (  )
        {
            return scGet_Instance_Title ( ins );
        }

        int create ( seerScript & sc )
        {
            return ( int ) ( ins = scCreate_Instance ( sc.get (  ), "" ) );
        }

        int call ( int addr, ... )
        {
            va_list va;
			int *params = NULL;
            va_start ( va, addr );
//			params = &( va_arg ( va, int ) );
			va_end(va);
            return scVCall_Instance ( ins, addr, -1, params );
            //-1 - default number of parameters
        }

        int call ( char *_symbol, ... )
        {
            va_list va;
			int *params = NULL;
            va_start ( va, _symbol );
//			params = &( va_arg ( va, int ) );
			va_end(va);
            return scVCall_Instance ( ins, symbol ( _symbol ), -1, params );
        }

        int start (  )
        {
            return call ( "main" );
        }

        int vcall ( int address, int paramc, int *params )
        {
            return scVCall_Instance ( ins, address, paramc, params );
        }

        int vlaunch ( int spd, int address, int paramc, int *params )
        {
            return scVLaunch_Instance ( ins, spd, address, paramc, params );
        }

        int launch ( int spd, int addr, ... )
        {
            va_list va;
			int *params = NULL;
            va_start ( va, addr );
//			params = &( va_arg ( va, int ) );
			va_end(va);
            return scVLaunch_Instance ( ins, spd, addr, -1, params );
        }

        int launch ( int spd, char *_symbol, ... )
        {
            va_list va;
			int *params = NULL;
            va_start ( va, _symbol );
//			params = &( va_arg ( va, int ) );
			va_end(va);
            return scVLaunch_Instance ( ins, spd, symbol ( _symbol ), -1, params );
        }

        void pause ( int p )
        {
            scPause_Instance ( ins, p );
        }

        void kill (  )
        {
            scKill_Instance ( ins );
        }

        scInstance *forked ( int i )
        {
            return scGet_Forked ( ins, i );
        }

        void killForked (  )
        {
            scKill_Forked_Instances ( ins );
        }

        void *var ( char *name )    //returns pointer to variable
        {
            return var ( symbol ( name ) );
        }

        void *var ( int offset )    //returns pointer to variable
        {
            if ( offset == -1 )
                return NULL;
            return ( void * ) ( ins->data_stack + 256 * 4 + offset );
        }

        bool valid (  )
        {
            return ( ins != NULL && scGet_Script_Size ( ins->code ) );
        }
    };

#ifndef SC_STRING_UNUSED

    class seerString:public scintString
    {
      public:
// char *cstr;

// seerString(char *x) {string_string_char(this,x);}
// seerString(int x) {string_string_int(this,x);}
// seerString(const seerString& f) {string_copy(this,(seerString*)&f);}
// seerString() {string_string(this);}

        seerString & operator= ( int x )
        {
            return *( seerString * ) ( string_operatorSET_int ( this, x ) );
        }
// {return /*(seerString&)*/(*string_operatorSET_int(this,x));}
/*  {seerString& sS;
   sS=(seerString&)(*string_operatorSET_int(this,x));
   return sS;
   }*/

        seerString ( const char *s )
        {
            string_string_char ( this, s );
        }

        seerString & operator= ( seerString & s )
        {
            return ( seerString & ) * string_operatorSET ( this, &s );
        }
        seerString & operator= ( char *s )
        {
            return ( seerString & ) * string_operatorSET_char ( this, s );
        }
// a=x=s; <=> a.operator=(x.operator=(s));

// seerString operator+(seerString& x) {return *(seerString*)&string_operatorPLUS(this,&x);}
// seerString operator+(char *s) {return *(seerString*)&string_operatorPLUS_char(this,s);}
        seerString operator+ ( seerString & x )
        {
            scintString z = string_operatorPLUS ( this, &x );
            return *( seerString * ) & z;
        }
        seerString operator+ ( char *s )
        {
            scintString z = string_operatorPLUS_char ( this, s );
            return *( seerString * ) & z;
        }
// a=x+s; <=> a=x.operator+(s); <=> a.operator=(x.operator+(s));

        seerString sub ( int start, int len )
        {
            scintString x = string_sub ( this, start, len );
            return *( ( seerString * ) & x );
        }
// seerString sub(int start,int len) {return *(seerString*)&string_sub(this,start,len);}
        seerString operator (  ) ( int start, int len )
        {
            return sub ( start, len );
        }

        int length (  )
        {
            return string_length ( this );
        }

        int cmp ( seerString & x )
        {
            return string_cmp ( this, &x );
        }
//  int operator<?>(seerString& x);
        bool operator== ( seerString & x )
        {
            return string_eq ( this, &x );
        }
        bool operator== ( char *x )
        {
            return string_eq_char ( this, x );
        }
        bool operator> ( seerString & x )
        {
            return string_gt ( this, &x );
        }
        bool operator< ( seerString & x )
        {
            return string_lt ( this, &x );
        }
        bool operator>= ( seerString & x )
        {
            return string_ge ( this, &x );
        }
        bool operator<= ( seerString & x )
        {
            return string_le ( this, &x );
        }
        void operator+= ( seerString & x )
        {
            string_append ( this, &x );
        }
        void operator+= ( char *x )
        {
            string_append_char ( this, x );
        }
// a+=x; <=> a.operator+=(x);

        void del ( int start, int len )
        {
            string_del ( this, start, len );
        }
        void insert ( int start, seerString & x )
        {
            string_insert ( this, start, &x );
        }
//index of first found occurance
        int find ( seerString & x )
        {
            return string_find ( this, &x );
        }
//returns number of replaces
        int replace ( seerString & what, seerString & with, bool global = false )
        {
            return string_replace ( this, &what, &with, global );
        }

        int toInt (  )
        {
            return string_toInt ( this );
        }
// int operator(int)();//toint
        char &operator[] ( int idx )
        {
            return *string_operatorIDX ( this, idx );
        }

        void reconstruct (  )
        {
            string_reconstruct ( this );
        }
        ~seerString (  )
        {
            string_destroy ( this );
        }
    };


//};????
#endif
}
#endif

void MyDispatcher ( int *result, void *function, int *params, int paramcount, unsigned int options);
char *str ( char *str);
unsigned int seer_init ( void);
unsigned int seer_execute ( scScript sc);
unsigned int seer_progresscallback ( int percent);
unsigned int seer_run ( char *script);

void seer_abort_script ();
void seer_allow_script ();
unsigned char *seer_in_system_name ();
int seer_in_system ();
unsigned int seer_handle_include ( char **script );
unsigned int seer_add_header ( unsigned char *section, unsigned char *header );
unsigned int seer_add_symbol_int ( char *name, void *func );
unsigned int seer_del_header ( unsigned char *section );
unsigned int seer_del_symbol_int ( char *name );

/*
struct seer_funcs
{
    void (*scAdd_Internal_Header) ( char *, char * );
    void (*scAdd_External_Symbol) ( char *, void * );
    void (*scAdd_External_Symbol_Int) ( char *name, void *addr );
	scScript (*scCompile_Text) ( char *source, int *length );
};
*/
#define __TRIX_SEER_H__

#include "trixplug_seer.h"
#endif
