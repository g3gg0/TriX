#ifndef __TRIX_STAGE_H__
#define __TRIX_STAGE_H__


#define S_MODIFIED    0x0001  // flag set if stage is modified
#define S_WORKSPACE   0x0002  //                   is current workspace
#define S_SYNCBASE    0x0004  //                   sync will only sync down to this stage
#define S_SAVESTAGE   0x0008  //                   should get saved into file

typedef struct s_stage_priv t_stage_priv;
struct s_stage_priv
{
	STRUCT_HEADER;
};

typedef struct s_symbols t_symbols;
struct s_symbols
{
	STRUCT_HEADER_LIST(t_symbols);
    char *name;
    int type;
    char *data;
};


/*
    the stage represents one step in the decoding chain.
    the first stage is always the file that was opened.
    after that file was opened, each parser/decoder creates
    an extra stage each. so we are able to load a ZIP file
    containing a BZ2-compressed PHOENIX file and to patch it.
    the private data is handled by the format parser/decoder
    layer and also has to get free'd by calling the given
    routines with the stage they belong to.
*/
typedef struct s_stage t_stage;
struct s_stage
{
	STRUCT_HEADER_LIST(t_stage);
    char *name;                 // like "UNPACKED"
    char *type;                 // type of content, set by child - maybe parser handles more than one type
    char *parser;               // name of the parser that created this one, like "PHOENIX"
    unsigned int length;        // length of data (accumulated)
    t_segment *segments;        // the segments
    t_symbols *symbols;         // symbols for that stage, variable pool
    t_stage_priv *priv;         // format parser private data
};

typedef struct s_stage_info t_stage_info;
struct s_stage_info
{
	STRUCT_HEADER_LIST(t_stage_info);
};

#include "trixplug_stage.h"

t_stage *stage_add ( t_stage * s );
t_stage *stage_get ( t_stage * s, unsigned int flag );
t_stage *stage_get_modified ( t_stage * s );
t_stage *stage_get_savestage ( t_stage * s );
t_stage *stage_get_workspace ( t_stage * s );
t_stage *stage_find_by_num ( t_stage * s, unsigned int num );
t_stage *stage_create (  );
t_stage *stage_get_last ( t_stage * s );
t_stage *stage_duplicate ( t_stage * s );
t_stage *stage_get_current ( t_stage * s );
t_stage_info *stage_save_info ( t_stage * s );

unsigned int stage_release ( t_stage * s );
unsigned int stage_release_all ( t_stage * s );
unsigned int stage_replace ( t_stage * t, t_stage * s );
unsigned int stage_dump ( t_stage * s );
unsigned int stage_get_num ( t_stage * s );
unsigned int stage_get_segments ( t_stage * s );
unsigned int stage_restore_info ( t_stage * s, t_stage_info * i );
unsigned int stage_count ( t_stage * s );
unsigned int stage_set_modified ( t_stage * s );
unsigned int stage_set_savestage ( t_stage * s );
unsigned int stage_set_workspace ( t_stage * s );
unsigned int stage_clear_flags ( t_stage * s, unsigned int flag );
unsigned int stage_has_unmapped ( t_stage * s );
unsigned int stage_has_overlaps ( t_stage * s );
unsigned int stage_get_start ( t_stage * s  );
unsigned int stage_get_end ( t_stage * s );
unsigned int stage_merge ( t_stage * s, unsigned char *buffer, unsigned int length );

#endif
