#ifndef __TRIX_UTIL_H__
#define __TRIX_UTIL_H__

/*! \file util.h
 */

typedef struct s_locator t_locator;
struct s_locator
{

    unsigned char *func_name;   //   draw_signal_strength          name for displaying in log window
    unsigned char *pattern;     //   b5 f0 20 00 21 00 22 00 ...   the bytes to search
    unsigned char *mask;        //   ff ff ff ff ff 00 ff 00 ...   bitmask for the pattern
    unsigned int length;        //   20                            what do u think?!?!
    unsigned int options;                // normally the engine searches back for a B5 XX (PUSH) 1 to disable this
        //double options definition, see util_find_pattern
};


typedef struct s_list t_list;
struct s_list
{
	STRUCT_HEADER_LIST(t_list);
};
typedef struct s_priv t_priv;
struct s_priv
{
	STRUCT_HEADER;
};

#define LOC_UNIQUE    1
#define LOC_FORWARD   0
#define LOC_BACKWARD  2

#define TAB     { tabs_temp=tabs; while ( tabs_temp-- ) printf ( "   " ); }
#define TABINC  tabs++
#define TABDEC  tabs--

#ifndef __TRIX_UTIL_C__
extern int tabs;
extern int tabs_temp;
#endif

// defines for memory access through plugins
#define MODE_READ  0x10
#define MODE_WRITE 0x20

#define WIDTH_BYTE 0x01
#define WIDTH_HALF 0x02
#define WIDTH_WORD 0x04
//

#define SYM_PTR 1
#define SYM_CALL 2
#define SYM_DATA 3

struct s_function_block
{
    char *name;
    unsigned char *data;
    unsigned int length;
};

struct s_function_relative_data
{
    unsigned int offset;
    int type;
    char *dest;
};

typedef struct s_function t_function;
struct s_function
{
    char *name;
    struct s_function_block *blocks;
    struct s_function_relative_data *rel_data;
};

typedef char* t_function2;
typedef char* func;
typedef char* object;

/*! a object to patch\n
 * \code
 * object MyObject[] =
 * {
 *     "name", "MyObject", "",
 *     ".text", "\xFF\x00\xFE\xCCTriX", "8",
 *     "!SYM_CALL", "becool", "0x00",
 *     "!SYM_PTR", "ILoveTrixPtr", "0x4",
 *     "!SYM_EXP", "MyObjectPart2", ".text+4",
 *     NULL
 * };
 * \endcode
 */

/*
    define a function like this:
    t_function func_test =
        {
            "new_function",
            {
                { ".text",    "\xB5\x00\x20\x00\xF0\x00\xF8\x00\xBC\x00", 10 },
                { ".data",    "\x00\x00\x00\x00\x00\x00\x00\x00", 8 },
                { ".bss",     NULL, 20 },
                { NULL,       NULL, 0 }
            },
            {
                { 0x04, SYM_CALL, "test_callee" },
                { 0x00, SYM_PTR,  ".bss+0x14" },
                { 0x08, SYM_PTR,  ".data+0x04" },
                { 0x0A, SYM_PTR,  ".data+0x08" },
                { 0, 0, NULL }
            }
        };
*/

#include "trixplug_util.h"


char *util_get_line ( char *text, unsigned int line );
void util_set_debuglevel ( int lvl );
void util_set_errorlevel ( int lvl );
unsigned char* util_block_get_data_pointer ( const t_function *func, const char *name );
char *util_hexunpack ( char *data, int *length );
char *util_hexpack ( unsigned char *data, int bytes );
unsigned int util_check_pattern_raw ( t_stage *s, unsigned int offset, t_locator * loc );
unsigned int util_check_pattern ( t_workspace *ws, unsigned int offset, t_locator * loc );
unsigned int util_find_pattern ( t_workspace *ws, t_locator * loc, unsigned int start, unsigned int end, int options );
unsigned int util_quick_find_pattern ( t_workspace *ws, char *pattern, char *mask, unsigned int size, unsigned int start, unsigned int end, int options );
unsigned int util_simple_find_pattern ( t_workspace *ws, char *pattern, char *mask, unsigned int size, int options );
unsigned int util_list_count ( t_list *l );

unsigned int util_str2int ( const char *str );
unsigned int util_block_get_segment_count ( object *o );
char* util_block_get_segment_name_ptr ( object *o, int occurence );
char *util_block_get_offset_ptr ( const object *o, const char *name, int occurence );
unsigned int util_block_get_size ( t_function *func, const char *name );
unsigned int util_block_get_number ( const t_function2 *func, const char *name, int occurence );
char *util_block_get_val_ptr ( const t_function2 *func, const char *name, int occurence );
unsigned int v_get_b ( t_workspace *ws, unsigned int address );
unsigned int v_get_h ( t_workspace *ws, unsigned int address );
unsigned int v_get_w ( t_workspace *ws, unsigned int address );
unsigned int v_get_b_raw ( t_stage *s, unsigned int address );
unsigned int v_get_h_raw ( t_stage *s, unsigned int address );
unsigned int v_get_w_raw ( t_stage *s, unsigned int address );
unsigned int v_set_b ( t_workspace *ws, unsigned int address, unsigned int value );
unsigned int v_set_h ( t_workspace *ws, unsigned int address, unsigned int value );
unsigned int v_set_w ( t_workspace *ws, unsigned int address, unsigned int value );
unsigned int v_memcpy_get_raw ( t_priv *s, unsigned char *dest, unsigned int src, unsigned int length );
unsigned int v_memcpy ( t_workspace *ws, unsigned int dest, unsigned int src, unsigned int length );
unsigned int v_memcpy_get ( t_workspace *ws, unsigned char *dest, unsigned int src, unsigned int length );
unsigned int v_memcpy_put ( t_workspace *ws, unsigned int dest, unsigned char *src, unsigned int length );
unsigned int v_get_end ( t_workspace *ws );
t_segment *v_get_segment ( t_workspace *ws, unsigned int address );
unsigned int v_arch_h ( t_stage *s, int data );
unsigned int v_arch_w ( t_stage *s, int data );
unsigned char *v_get_ptr ( t_workspace *ws, unsigned int address );
unsigned int v_valid_raw ( t_priv *segment, unsigned int address );
unsigned int v_valid ( t_workspace *ws, unsigned int address );
unsigned char * v_get_str_raw ( t_stage *s, unsigned int address );
unsigned int v_get_strlen_raw ( t_stage *s, unsigned int address );
unsigned int v_get_strlen ( t_workspace *ws, unsigned int address );
unsigned char *v_get_str ( t_workspace *ws, unsigned int address );
unsigned int v_get_next_offset ( t_workspace *ws, unsigned int offset );
unsigned int v_get_prev_offset ( t_workspace *ws, unsigned int offset );
unsigned int v_get_size ( t_workspace *ws );
unsigned int v_get_start ( t_workspace *ws );
unsigned int v_get_areas ( t_workspace *ws );
unsigned int v_cache_update ( t_workspace *ws, t_memmap *mm );
unsigned int v_cache_flush (  );
void *util_smalloc ( int size, char *name );
t_list *util_list_get_last ( t_list *l );
unsigned int util_list_add (  t_list *l1,  t_list *l2  );
t_list *util_list_add_new (  t_list *l1, int size, char *name  );
unsigned int util_list_free ( t_list *l );
unsigned int util_swap_half ( unsigned int v );
unsigned int util_swap_word ( unsigned int v );
unsigned int util_free_object ( object *o );
void util_dump_object ( object *o );
char *util_split ( char *string, unsigned char seperator, int which );
void util_dump_bytes ( unsigned char *data, int width, int bytes );
void util_cache_enable ( );
void util_cache_disable ( );
unsigned int is_type ( t_priv *data, char *type );
unsigned int util_hex_get ( char *data, int *err);
unsigned char *util_hex_get_buffer ( char *data, int *len );
unsigned int util_init ();

#endif
