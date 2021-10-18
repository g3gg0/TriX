#ifndef __TRIX_SEGMENT_H__
#define __TRIX_SEGMENT_H__

typedef struct s_seg_priv t_seg_priv;
struct s_seg_priv
{
	STRUCT_HEADER;
};



/*
    the segment stuct contains info about one segment.
    these are - in case of more segments - linked together
    to a double linked list.
*/
typedef struct s_segment t_segment;
struct s_segment
{
	STRUCT_HEADER_LIST(t_segment);
    char *name;                  // like ".text" or "FLASH"
    unsigned int start;          // start address
    unsigned int end;            // end address
    unsigned int length;         // length of data
    unsigned char *data;         // the data block
	unsigned char *capture_plug; // symbol that handles memory accesses
	unsigned int capture_plug_active; // 
    t_seg_priv *priv;            // format parser private data
};

#include "trixplug_segment.h"


t_segment *segment_find_by_end_and_name ( t_segment * s, unsigned int addr, char *name, unsigned int flags );
t_segment *segment_find_by_end ( t_segment * s, unsigned int addr, unsigned int flags );
t_segment *segment_find_by_start ( t_segment * s, unsigned int addr, unsigned int flags );
t_segment *segment_find_by_name ( t_segment * s, char *name, unsigned int flags );
t_segment *segment_find_by_num ( t_segment * s, unsigned int num );
t_segment *segment_add ( t_segment * s );
t_segment *segment_create ( );
t_segment *segment_get_last ( t_segment * s );
t_segment *segment_duplicate ( t_segment * s );

unsigned int segment_is_mapped ( t_segment * s );
unsigned int segment_is_sparse ( t_segment * s );
unsigned int segment_dump ( t_segment * s );
unsigned int segment_count ( t_segment * s );
unsigned int segment_release ( t_segment * s );
unsigned int segment_release_all ( t_segment * s );

void segment_map_in_mem ( t_segment * s );
void segment_hide_in_mem ( t_segment * s );



#endif
