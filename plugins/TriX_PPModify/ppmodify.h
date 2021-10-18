#ifndef __TRIX_PPMODIFY_H__
#define __TRIX_PPMODIFY_H__

#include "trixplug.h"

TRIXPLUGIN_API unsigned int trixplugin_init ( struct trix_functions *callbacks );
TRIXPLUGIN_API char *trixplugin_name ( void );
TRIXPLUGIN_API unsigned int trixplugin_vers ( void );

typedef struct {
    unsigned int   id;
    unsigned int   length;
    unsigned char  name[5];
    unsigned char flags1;
    unsigned char flags2;
    unsigned char flags3;
    unsigned char flags4;
} subchunk_hdr;

typedef struct {
    unsigned int   tone_nr;
    unsigned int   length;
    unsigned char  name[5];
    unsigned short unused;
} tone_hdr;

typedef struct {
    unsigned int chksum;
    unsigned int length;
    unsigned char name[5];
    unsigned char version[9];
} chunk_hdr;

typedef struct {
    unsigned char ppm_string[5];
    unsigned char version[37];
    unsigned char type[5];
} ppm_hdr;

typedef struct {
    unsigned short *buffer;
	int length;
	int allocated;
	unsigned int priv_data;
} t_stringbuffer;

//
//
//  Private structs
//  
//
//


#define TOKEN_TYPE_PLAIN  0
#define TOKEN_TYPE_ESCAPE 1
#define TOKEN_TYPE_TOKEN  2

typedef struct
{
	unsigned char done;
	unsigned char type;
	unsigned char value;
} t_token_entry;

typedef struct
{
	unsigned int entries;
	unsigned int allocated;

	unsigned char type;
	unsigned char token_1;
	unsigned char token_2;

	t_token_entry *data;

	unsigned int priv_data;

} t_compress_buffer;



typedef struct
{
	unsigned int dict_entries;
	unsigned int dict_allocated;
	unsigned int data_entries;
	unsigned int data_allocated;

	unsigned int max_entries;

	t_compress_buffer *dict;
	t_compress_buffer *data;

	unsigned char priv_data;
} t_comp_dict;


#define PPMODIFY_HEADERS \
	"import unsigned int __ppmodify_dump ( t_workspace *ws, unsigned int start );\n"\
	"import t_treenode * __ppmodify_dump_xml ( t_workspace *ws, unsigned int start );\n"\
	"import unsigned int __ppmodify_build_ppm ( t_workspace *ws, unsigned int start, unsigned int length, t_treenode *node );\n"\
	"import unsigned int __ppmodify_merge_ppm ( t_workspace *ws );\n"\


#define PPMODIFY_SYMBOLS \
	REGISTER_SYMBOL ( ppmodify_dump );\
	REGISTER_SYMBOL ( ppmodify_dump_xml );\
	REGISTER_SYMBOL ( ppmodify_build_ppm );\
	REGISTER_SYMBOL ( ppmodify_merge_ppm );\


unsigned int ppmodify_dump ( t_workspace *ws, unsigned int start );
t_treenode * ppmodify_dump_xml ( t_workspace *ws, unsigned int start );
unsigned int ppmodify_init ( );
unsigned int ppmodify_merge_ppm ( t_treenode *node );



unsigned char *utf16_to_utf8 ( t_stringbuffer *string );
unsigned int utf16_strlen ( t_stringbuffer *string );
unsigned int utf16_stradd ( t_stringbuffer *string, unsigned short c );
unsigned int ppmodify_text_subchunk_dump_lang_comp_token ( unsigned char *inbuffer, unsigned int length, unsigned char *tokens, t_stringbuffer *buffer, unsigned short flags1, int recursive );
unsigned int ppmodify_text_subchunk_dump_lang_comp_entry ( t_workspace *ws, unsigned int start, unsigned int length, t_treenode *node, unsigned short flags1, unsigned short flags2, unsigned char *tokens );
unsigned int ppmodify_text_subchunk_dump_lang_comp_strings ( t_workspace *ws, unsigned int start, unsigned int length, t_treenode *node, unsigned short flags1, unsigned short flags2, unsigned char *tokens, unsigned char *lengths, int entries );
unsigned int ppmodify_text_subchunk_dump_lang_comp ( t_workspace *ws, unsigned int start, unsigned int length, t_treenode *node, unsigned short flags1, unsigned short flags2 );
unsigned int ppmodify_text_subchunk_dump_lang_plain ( t_workspace *ws, unsigned int start, unsigned int length, t_treenode *node, unsigned short flags1, unsigned short flags2 );
unsigned int ppmodify_text_subchunk_dump_lang ( t_workspace *ws, unsigned int start, unsigned int length, t_treenode *node, unsigned short flags1, unsigned short flags2 );
unsigned int ppmodify_text_subchunk_dump ( t_workspace *ws, unsigned int start, unsigned int length, t_treenode *node );
unsigned int ppmodify_build_ppm ( t_workspace *ws, unsigned int start, unsigned int length, t_treenode *node );
unsigned int ppmodify_string_unescape ( unsigned char *string );
unsigned int ppmodify_build_text_subchunk ( t_treenode *node );

unsigned int ppmodify_text_compress_init ( t_compress_buffer *buffer );
unsigned int ppmodify_text_comp_compress ( unsigned char *inbuffer, unsigned int length, unsigned char *tokens, t_compress_buffer *buffer, unsigned short flags1, int recursive );
unsigned int ppmodify_text_compress_add ( t_compress_buffer *buffer, unsigned char type, unsigned char data );
t_compress_buffer *ppmodify_text_comp_build_table ( unsigned char *tokentable, unsigned short flags1, unsigned short flags2 );

unsigned int ppmodify_text_comp_dict_add_token ( t_comp_dict *dict, unsigned char type, unsigned char token_1, unsigned char token_2 );
unsigned int ppmodify_text_comp_dict_add_data ( t_comp_dict *dict, unsigned char *buffer, unsigned int length );
unsigned int ppmodify_text_comp_dict_init ( t_comp_dict *dict );
unsigned int ppmodify_text_comp_dict_release ( t_comp_dict *dict );
unsigned int ppmodify_text_compress_release ( t_compress_buffer *buffer );
unsigned char *ppmodify_text_comp_retrieve_tokenbuf ( t_comp_dict *dict, unsigned int entry, unsigned int *length );
unsigned int ppmodify_text_comp_dict_finish ( t_comp_dict *dict, unsigned char *tokenbuffer, unsigned short flags1 );

unsigned int ppmodify_text_comp_dict_add_all_chars ( t_comp_dict *dict );
unsigned int ppmodify_text_comp_dict_get_highfreq ( t_comp_dict *dict, t_compress_buffer *highest, unsigned short flags1 );
unsigned int ppmodify_text_comp_dict_subst_token ( t_comp_dict *dict, t_compress_buffer *highest, unsigned short flags1 );
unsigned int ppmodify_text_comp_token_align ( t_comp_dict *dict );
unsigned int ppmodify_text_comp_decompress ( t_compress_buffer *buffer, unsigned char *tokens, unsigned short flags1 );
unsigned int ppmodify_text_comp_token_escape ( t_comp_dict *dict );

#endif

