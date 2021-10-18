#ifndef __TRIX_WORKSPACE_C__
#define __TRIX_WORKSPACE_C__

#include <stdlib.h>
#include "defines.h"
#include "segment.h"
#include "stage.h"
#include "symbols.h"

#include "file_io.h"
#include "file.h"
#include "workspace.h"
#include "util.h"

#include "trixplug_funcs.h"
#include "mem.h"

WORKSPACE_PLUG_INIT;

int skip_error_msg = 0;

/*!
 * skip error message on file load, used for option loading
 * \param 1 to skip, 0 to print error message
 */
void
workspace_skip_error ( int skip )
{
	skip_error_msg = skip;
}

unsigned int
workspace_set_modified ( t_workspace *ws )
{
	if ( ws )
		ws->flags |= W_MODIFIED;
    return E_OK;
}

t_workspace *
workspace_startup ( char *filename )
{
    t_fileinfo *fi = NULL;
	t_workspace *ws = NULL;
	
	fi = file_open ( filename );
    if ( !fi )
    {
		if ( !skip_error_msg )
			ERR ( 1, "Could not open file '%s'\n", filename );
        return 0;
    }

	ws = workspace_create ();
	if ( !ws )
		return NULL;

	if ( workspace_add_file(ws, fi) == E_OK )
		workspace_update_memmap(ws);

	return ws;
}

t_workspace *
workspace_create ( )
{
	t_workspace *ws = NULL;

	ws = LIST_MALLOC(t_workspace);
	if ( !ws )
		return NULL;

	ws->memmap = NULL;
	ws->fileinfo = NULL;
	ws->flags = FLAGS_ENDIANESS_BE;

	return ws;
}

t_workspace *
workspace_create_file ( )
{
	t_workspace *ws = NULL;

	ws = LIST_MALLOC(t_workspace);
	if ( !ws )
		return NULL;

	ws->memmap = NULL;
	ws->fileinfo = file_io_create_empty ();
	ws->flags = FLAGS_ENDIANESS_BE;

	return ws;
}

t_workspace *
workspace_create_file_from_buffer ( unsigned char *buffer, unsigned int length )
{
	t_workspace *ws = workspace_create_file ();

	if ( !ws )
		return NULL;

	ws->fileinfo->stages->segments->data = buffer;
	ws->fileinfo->stages->segments->start = 0;
	ws->fileinfo->stages->segments->end = length;
	ws->fileinfo->stages->segments->length = length;

	workspace_update_memmap ( ws );

	return ws;
}


unsigned int
workspace_add_file ( t_workspace *ws, t_fileinfo *fileinfo )
{
	if ( !ws )
		return E_FAIL;
	
	if ( !ws->fileinfo )
		ws->fileinfo = fileinfo;
	else
		return LIST_ADD(ws->fileinfo, fileinfo);

	return E_OK;
}

unsigned int
workspace_memmap_add ( t_workspace *ws, t_stage *stage, t_segment *seg )
{
	t_memmap *mm = NULL;

	if ( !ws )
		return E_FAIL;

	mm = LIST_NEW(ws->memmap, t_memmap);
	mm->stage = stage;
	mm->segment = seg;
	mm->start = seg->start;
	mm->end = seg->end;
	mm->length = seg->length;

	if ( !ws->memmap )
		ws->memmap =  mm;

	return E_OK;
}

unsigned int 
workspace_release ( t_workspace *ws )
{

	if ( !ws )
		return E_FAIL;

	workspace_memmap_release ( ws );
	file_release_all ( ws->fileinfo );

	free ( ws );

	return E_OK;
}

unsigned int
workspace_memmap_release ( t_workspace *ws )
{
	return LIST_FREE(ws->memmap);
}

unsigned int
workspace_memmap_sort ( t_workspace *ws )
{
	int done = 0;
	t_memmap current;
	t_memmap last;
	t_memmap *mm = NULL;
	t_memmap *list = NULL;

	if ( !ws || !ws->memmap )
		return E_FAIL;

	last.start = E_FAIL;
	last.length = 0;
	list = ws->memmap;
	ws->memmap = NULL;
	while ( !done )
	{
		current.start = MEM_MAX;
		current.length = 0;
		mm = list;
		while ( mm )
		{
			/* find the lowest start address, then the biggest area (shorter areas will then not overlap bigger ones */
			if ( ((mm->start < current.start) || (mm->start == current.start && mm->length >= current.length) ) 
				&& ( (mm->start > last.start) || (last.start == E_FAIL) )
				&& (((mm->start + mm->length) >= (last.start + last.length)) || last.start == E_FAIL) )
				memcpy ( &current, mm, sizeof(t_memmap) );
			LIST_NEXT(mm);
		}
		if ( current.start == MEM_MAX )
			done = 1;
		else
		{
			memcpy ( &last, &current, sizeof(t_memmap) );
			workspace_memmap_add ( ws, current.stage, current.segment );
		}
	}

	LIST_FREE(list);

	return E_OK;
}

unsigned int
workspace_update_memmap ( t_workspace *ws )
{
	t_fileinfo *fi = NULL;
	t_stage *stage = NULL;
	t_segment *segment = NULL;

	if ( !ws )
		return E_FAIL;

	LIST_FREE(ws->memmap);

	fi = ws->fileinfo;
	while ( fi )
	{
		stage = stage_get_workspace ( fi->stages );
		if ( !stage )
			stage = stage_get_last ( fi->stages );

		if ( stage )
		{
			segment = stage->segments;
			while ( segment )
			{
				/* segments needs size and must not be an "invisible" segment */
				if ( segment->length && !(segment->flags & FLAGS_HIDE_IN_MEM))
					workspace_memmap_add ( ws, stage, segment );
				LIST_NEXT(segment);
			}
		}
		LIST_NEXT (fi);
	}

	workspace_memmap_sort ( ws );
	v_cache_flush ();

	return E_OK;
}

#endif

