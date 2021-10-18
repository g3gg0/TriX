#ifndef __TRIX_WORKSPACE_H__
#define __TRIX_WORKSPACE_H__

typedef struct s_memmap t_memmap;
struct s_memmap
{
	STRUCT_HEADER_LIST(t_memmap);
	unsigned int start;
	unsigned int end;
	unsigned int length;
	t_segment *segment;
	t_stage *stage;
};

typedef struct s_workspace t_workspace;
struct s_workspace
{
	STRUCT_HEADER_LIST(t_workspace);
	t_memmap *memmap;
	t_fileinfo *fileinfo;
};


#include "trixplug_workspace.h"


void workspace_skip_error ( int skip );
unsigned int workspace_set_modified ( t_workspace *ws );
unsigned int workspace_update_memmap ( t_workspace *ws );
unsigned int workspace_memmap_add ( t_workspace *ws, t_stage *stage, t_segment *seg );
unsigned int workspace_add_file ( t_workspace *ws, t_fileinfo *fileinfo );
t_workspace *workspace_create ( );
t_workspace *workspace_startup ( char *filename );
unsigned int workspace_memmap_sort ( t_workspace *ws );
unsigned int workspace_memmap_release ( t_workspace *ws );
t_workspace *workspace_create_file ( );
t_workspace *workspace_create_file_from_buffer ( unsigned char *buffer, unsigned int length );
unsigned int workspace_release ( t_workspace *ws );
unsigned int workspace_memmap_reverse ( t_workspace *ws );

#define W_MODIFIED    0x0001  // flag set if workspace is modified



#endif
