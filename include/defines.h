#ifndef __TRIX_DEFINES_H__
#define __TRIX_DEFINES_H__

#include <stdarg.h>

#ifdef GCC_WIN32
#define __forceinline inline
#endif

#ifdef WIN32
#define INLINE_MODE __forceinline
#else
#define INLINE_MODE inline
#define __cdecl
#define Sleep(x) usleep ( (x) *10)

#endif

typedef unsigned int t_address;

#define __TRIX_VERSION__ "v0.9 "

#define STRUCT_HEADER \
	int struct_magic;\
	char *struct_name;\
	int struct_size;\
	int struct_refs;\
	int flags

#define STRUCT_HEADER_LIST(type) \
	STRUCT_HEADER;\
	type *prev;\
	type *next

#ifdef WIN32
#define strncasecmp strnicmp
#define strcasecmp stricmp

#define DIR_SEP '\\'
#define DIR_SEP_STR "\\"
#else
#define DIR_SEP '/'
#define DIR_SEP_STR "/"
#endif

#define DEFAULT_SCRIPT \
	"#include trix\n" \
	"\n" \
	"int main ( )\n" \
	"{\n"\
	"\tchar *buffer = NULL;\n"\
	"\tUiDlgString ( \"Just press enter if you're bored ;).\", &buffer );\n" \
	"}\n"


//#define DEBUG_FAULTS

#ifdef DEBUG_FAULTS
#define HEAP_CHECK _heapchk()
#else
#define HEAP_CHECK while(0){}
#endif

#define TRIX_PRINTF_BUFSIZE 4*1024*1024

#if defined(_MSC_VER) && ( _MSC_VER >= 1600 )
// _set_sbh_threshold is removed from VS 2010 since there is no long small block heap.
// http://connect.microsoft.com/VisualStudio/feedback/details/511177/set-sbh-threshold-identifier-not-found
#define _set_sbh_threshold(...)
#endif

#ifndef NULL
#define NULL (0L)
#endif

#ifdef E_FAIL
#undef E_FAIL
#endif
/*
   because of working with int's
   switching to unsigned int will
   cause problems with returncode 
   on error :(
*/
#define MEM_MAX  0xFFFFFFFD
#define MEM_AUTO 0xFFFFFFFE
#define MEM_FAIL 0xCCCCCCCC

#define E_FAIL   0xFFFFFFFF
#define E_OK     0

#define DEFAULT_ERROR_LEVEL 255
#define DEFAULT_DEBUG_LEVEL 0

#define ABS(x)				(((x) < 0) ? -(x) : (x))
#define SWAP(x,y)           (((x) ^ (y)) && ((y) ^= (x) ^= (y), (x) ^= (y)))
#define ALIGN2(x)           ( ( ( x ) + 1 ) & ~1 )
#define ALIGN4(x)           ( ( ( x ) + 3 ) & ~3 )
#define ALIGN8(x)           ( ( ( x ) + 7 ) & ~7 )
#define ALIGN16(x)          ( ( ( x ) + 15 ) & ~15 )
#define ALIGN_WORD(x)       ( ( ( x ) + 3 ) & ~3 )
#define ALIGN_HALF(x)       ( ( ( x ) + 1 ) & ~1 )
#define ALIGN_BY(x,y)       ( ( ( x ) % ( y ) ) ? ( ( x ) + ( y ) - ( x ) % ( y ) ) : ( x ) )

#define LIST_COUNT(x)  util_list_count ( (t_list *)x )
#define LIST_END(x)    util_list_get_last ( (t_list *)x )
#define LIST_FFWD(x)   while(x&&x->next) x=x->next
#define LIST_REWND(x)  while(x&&x->prev) x=x->prev
#define LIST_ADD(x,y)  util_list_add ( (t_list *)x, (t_list *)y  )
#define LIST_NEW(x,y)  (y*)util_list_add_new (  (t_list *)x, sizeof(y), #y )
#define LIST_PREV(x)   x=x->prev
#define LIST_NEXT(x)   x=x->next
#define LIST_FREE(x)   util_list_free ( (t_list *)x ); x = NULL
#define LIST_MALLOC(x) (x*)util_smalloc ( sizeof(x), #x )
#define PRIV_MALLOC(x) (x*)util_smalloc ( sizeof(x), #x )
#define LIST_DEL_SIMPLE(item,head)	{\
									if ( item ) \
									{ \
										if ( item->prev )\
											item->prev->next = item->next;\
										else\
											head = item->next;\
										if ( item->next )\
											item->next->prev = item->prev;\
									}}
#define LIST_DEL(item,head,tail) 	{\
									if ( item ) \
									{ \
										if ( item->prev )\
											item->prev->next = item->next;\
										else\
											head = item->next;\
										if ( item->next )\
											item->next->prev = item->prev;\
										else if (tail != head)\
											tail = item->prev;\
									}}


int ui_printf_msg ( char *str, ... );
int ui_debug_msg ( int level, char *str, ... );
int ui_error_msg ( int level, char *str, ... );
#define R(x) {if ( x != E_OK ) return E_FAIL;}
#define CHECK_AND_FREE(x) if ( x ) { free ( x ); x = NULL; }

#ifdef TRIXPLUG_MODE
#define DBG    ft->debug_msg
#define ERR    ft->error_msg
#define printf ft->print_msg
//#define malloc ft->mem->malloc
//#define free   ft->mem->free
//#define ralloc ft->mem->realloc
#else
#define DBG ui_debug_msg
#define ERR ui_error_msg
#define printf ui_printf_msg
#endif



#ifdef QT_CORE_LIB
int __cdecl	qt_vprintf (const char *format, va_list args );

char * __cdecl	      qt_dlg_load_file ( const char *msg, const char *ext );
char * __cdecl	      qt_dlg_save_file ( const char *msg, const char *ext );
unsigned int __cdecl	qt_dlg_msg ( char *text, int type );
char * __cdecl	        qt_dlg_string ( char *def, char *text );
unsigned int __cdecl	qt_dlg_bool ( char *text );
unsigned int __cdecl    qt_dlg_box_create ( unsigned char *title );
unsigned int __cdecl    qt_dlg_box_process ( );
unsigned int __cdecl    qt_dlg_memviewer_process ();
unsigned int __cdecl    qt_dlg_workspace_process ();
unsigned int __cdecl	qt_dlg_box_clear ( unsigned int id );
unsigned int __cdecl	qt_dlg_box_msg ( unsigned int id, unsigned char *msg );
unsigned int __cdecl	qt_dlg_box_set_keypress_ptr ( unsigned int id, int *ptr );
unsigned int __cdecl	qt_dlg_box_release ( unsigned int id );
unsigned int __cdecl	qt_dlg_box_release_all ( );
unsigned int __cdecl	qt_dlg_box_release ( unsigned int id );
unsigned int __cdecl	qt_dlg_box_size ( unsigned int id, unsigned int width, unsigned int height );


#include "treenode.h"
#endif

#define M_SEGMENT      'SEGM'
#define M_STAGE        'STAG'
#define M_STAGE_INFO   'STGI'
#define M_FORMAT       'FORM'
#define M_UNDEF        'UNDF'

/* also in util.h for scripts */
#define DEBUG_FMT       0x0001
#define DEBUG_SEER      0x0002
#define DEBUG_ARM       0x0004
#define DEBUG_NOKIA     0x0008
#define DEBUG_LIST      0x0010
#define DEBUG_UNKNOWN   0x0020
#define DEBUG_FMT_ELF   0x0040
#define DEBUG_ELF       0x0080
#define DEBUG_FMT_EPOC  0x0100
#define DEBUG_PLUG      0x0200
#define DEBUG_NETRIX    0x0400
#define DEBUG_FMT_PE    0x0800

#define FLAGS_ENDIANESS    0x00030000
#define FLAGS_ENDIANESS_LE 0x00010000
#define FLAGS_ENDIANESS_BE 0x00020000
#define FLAGS_FREE_NAME    0x00040000
#define FLAGS_REPLACE      0x00080000
#define FLAGS_HIDE_IN_MEM  0x00100000
#define FLAGS_MAP_IN_MEM   0x00200000
#define FLAGS_MAP_ANY      0x00300000
#define FLAGS_SPARSE       0x00400000
#define FLAGS_SHADOW       0x00800000

// new macro for shift + and to get a byte from a word
// will make code more readable than all those ((variable>>24)&0xFF) | ((variable>>16)&0xFF) ...
#define BYTEPOS(val,pos) (((val)>>((pos)*8))&0xFF)

/*
	         JUST FOR TESTING
			------------------
    to be replaced with functions that respect the
    other segments and its start addresses
*/
#define GET_WORD(x,y) ((GET_HALF(x,(y))<<16)|GET_HALF(x,(y+2)))
#define GET_TRIP(x,y) ((GET_BYTE(x,(y))<<16)|GET_HALF(x,(y+1)))
#define GET_HALF(x,y) ((GET_BYTE(x,(y))<<8)|GET_BYTE(x,(y+1)))
#define GET_BYTE(x,y) (x->segments->data[y])
#define GET_PTR(x,y) &(x->segments->data[y])
#define GET_SIZE(x)   (x->segments->length)

#define SET_WORD(x,y,z) {SET_HALF(x,(y),((z)>>16)&0xFFFF);SET_HALF(x,(y)+2,(z)&0xFFFF)}
#define SET_TRIP(x,y,z) {SET_BYTE(x,(y),((z)>>16)&0xFF);SET_HALF(x,(y)+1,(z)&0xFFFF)}
#define SET_HALF(x,y,z) {SET_BYTE(x,(y),((z)>>8)&0xFF);SET_BYTE(x,(y)+1,(z)&0xFF)}
#define SET_BYTE(x,y,z) {x->segments->data[y]=z;}
#define SET_SIZE(x,y)   {x->segments->length=y;}

#define GET_WORD_RAW(buffer,pos) ((GET_HALF_RAW(buffer,(pos))<<16)|GET_HALF_RAW(buffer,(pos+2)))
#define GET_TRIP_RAW(buffer,pos) ((GET_BYTE_RAW(buffer,(pos))<<16)|GET_HALF_RAW(buffer,(pos+1)))
#define GET_HALF_RAW(buffer,pos) ((GET_BYTE_RAW(buffer,(pos))<<8)|GET_BYTE_RAW(buffer,(pos+1)))
#define GET_BYTE_RAW(buffer,pos) (((unsigned char*)(buffer))[pos])

#define SET_WORD_RAW(buffer,pos,data) {SET_HALF_RAW(buffer,(pos),((data)>>16)&0xFFFF);SET_HALF_RAW(buffer,(pos)+2,(data)&0xFFFF)}
#define SET_TRIP_RAW(buffer,pos,data) {SET_BYTE_RAW(buffer,(pos),((data)>>16)&0xFF);SET_HALF_RAW(buffer,(pos)+1,(data)&0xFFFF)}
#define SET_HALF_RAW(buffer,pos,data) {SET_BYTE_RAW(buffer,(pos),((data)>>8)&0xFF);SET_BYTE_RAW(buffer,(pos)+1,(data)&0xFF)}
#define SET_BYTE_RAW(buffer,pos,data) {((unsigned char*)(buffer))[pos]=data;}

#endif
