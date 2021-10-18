#ifndef __TRIX_UTIL_C__
#define __TRIX_UTIL_C__

/*! \file util.c
 *
 */


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "defines.h"
#include "segment.h"
#include "stage.h"
#include "symbols.h"
#include "file_io.h"
#include "file.h"
#include "workspace.h"
#include "util.h"
#include "options.h"

#include "trixplug_funcs.h"

#include "mem.h"


int tabs_temp = 0;
int tabs = 0;
int create_segments = 1;
int debuglevel = DEFAULT_DEBUG_LEVEL;//DEBUG_PLUG|DEBUG_FMT_ELF|DEBUG_ELF;
/*
    note:
        maybe ERR () should be used to print errors, when functions fails
        in standard operation mode, it should print all errors ?
*/
int errorlevel = DEFAULT_ERROR_LEVEL;

/*
	caching saves the last used ws, segment and its data.
	the memory access routines will then be able to respond quicker
	with the needed data. just in case of an "cache miss", that means
	accessing a different segment, we will use the normal lookup.

	performance gain:  10%
	
	update 30.08.07:
		gain improved because many things were fixed and cleaned up
*/

int cache_enabled = 1;
int cache_start = -1;
int cache_end = -1;
int cache_dirty = 0;
int cache_read_hits = 0;
int cache_write_hits = 0;
int cache_read_misses = 0;
int cache_write_misses = 0;

t_workspace *cache_ws = NULL;
t_memmap *cache_mm = NULL;
char *cache_data = NULL;

UTIL_PLUG_INIT;


unsigned int util_init ()
{
	options_add_core_option ( OPT_INT, "util", create_segments, "Create segments upon memory write access to it using v_memcpy_put() ." );
	options_add_core_option ( OPT_INT, "util", cache_enabled, "Enable cached memory structs in memory management" );
	options_add_core_option ( OPT_INT, "util", debuglevel, "Verbosity level of functions. This is a bitmask!" );
	options_add_core_option ( OPT_INT, "util", errorlevel, "Error level of functions. This is a bitmask!" );

	return E_OK;
}

void util_set_debuglevel ( int lvl )
{
    debuglevel = lvl;
}

void util_set_errorlevel ( int lvl )
{
    errorlevel = lvl;
}

int util_script_is_aborted ()
{
	return seer_is_aborted ();
}

char *util_get_line ( char *text, unsigned int line )
{
	char          *ret = NULL;
	char          *tmp = NULL;
	unsigned int   len = 0;
	unsigned int   pos = 0;
	unsigned int   cur = 0;
	unsigned int start = 0;
	unsigned int  ende = 0;
	unsigned int  done = 0;

	static char          *prev_buf  = NULL;
	static char          *prev_text = NULL;
	static unsigned int   prev_line = 0;
	static unsigned int   prev_len  = 0;

	if ( !text )
	{
		prev_text = NULL;
		return NULL;
	}

	if ( prev_text != text || prev_line > line )
	{
		prev_buf  = text;
		prev_text = text;
		prev_line = 0;
		prev_len  = strlen (text);
	}
	else
		cur = prev_line;

	text = prev_buf;

	while ( cur <= line )
	{
		done  = 0;
		start = ende;
		if ( ret )
			free ( ret );
		ret   = NULL;
		
		while ( pos < prev_len && !done )
		{
			if ( text[pos] == '\n' )
			{
				if ( cur == line )
				{
					if ( pos - start < 1 )
						return strdup("");
					prev_line = line;
					prev_buf = (void*)((unsigned long)text + start);
					ret = malloc ( pos - start + 200 );
					memcpy ( ret, (void*)((unsigned long)text + start), pos - start );
					ret[pos-start] = '\000';
					if ( ret[pos-start-1] == '\r' || ret[pos-start-2] == '\n' )
						ret[pos-start-1] = '\000';
					if ( ret[pos-start-2] == '\r' || ret[pos-start-2] == '\n' )
						ret[pos-start-2] = '\000';
					tmp = ret;
					while ( *tmp == ' ' || *tmp == '\t' )
						tmp++;
					len = strlen ( tmp );
					tmp = malloc ( len + 200 );
					strcpy ( tmp, ret + strlen ( ret ) - len );
					free ( ret );
					ret = tmp;
					
				}
				ende = pos + 1;
				done = 1;
			}
			if ( text[pos] == '\000' )
			{
				prev_buf  = NULL;
				prev_text = NULL;
				prev_line = 0;
				prev_len  = 0;
			}
			pos++;
		}
		if ( pos > strlen ( text ) )
			return NULL;
		cur++;
	}

	return ret;
}


void *
util_smalloc_init ( void *ptr, int size, char *name )
{
	if ( size >= sizeof ( t_priv ) )
	{
		((t_priv*)ptr)->struct_magic = M_UNDEF;
		((t_priv*)ptr)->struct_name = name;
		((t_priv*)ptr)->struct_size = size;
		((t_priv*)ptr)->struct_refs = 0;
		((t_priv*)ptr)->flags = 0;
	}
	if ( size >= sizeof ( t_list ) )
	{
		((t_list*)ptr)->prev = NULL;
		((t_list*)ptr)->next = NULL;
	}

	return ptr;
}

void *
util_smalloc ( int size, char *name )
{
	void *ret = NULL;

	DBG(DEBUG_LIST, "%s: <%s>\n", __FUNCTION__, name );

	ret = tmalloc ( size, name );
	if ( !ret )
		return NULL;

	util_smalloc_init ( ret, size, name );

	return ret;
}

void util_dump_bytes ( unsigned char *data, int width, int bytes )
{
	int pos = 0;
	int col = 0;

	while ( pos < bytes )
	{
		printf ( "%08X: ", pos );
		col = 0;
		while ( col < width  )
		{
			if ( col+pos < bytes )
				printf ( "%02X ", data[pos+col] );
			else
				printf ( "   " );
			col++;
		}

		col = 0;
		while ( col < width )
		{
			if ( col+pos < bytes )
			{
				if ( data[pos+col] < 0x80 && data[pos+col] > 0x20 )
					printf ( "%c", data[pos+col] );
				else
					printf ( "." );
			}
			else
				printf ( " " );

			col++;
		}
		printf ( "\n" );
		pos += width;
	}

	return;
}


/*
	Inline functions for speedup
*/



INLINE_MODE unsigned int
v_call_plug ( t_workspace *ws, t_memmap *mm, unsigned int address, unsigned int value, unsigned char mode, unsigned char width  )
{
	unsigned int ret = E_FAIL;
	unsigned int (*handler) ( t_workspace *ws, unsigned int address, unsigned char type, unsigned int value );
	t_segment *seg = mm->segment;

	handler = (unsigned int) trixplug_get_global_plugin_symbol ( mm->segment->capture_plug );

	if ( handler )
	{
		seg->capture_plug_active = 1;
		ret = handler ( ws, address, mode | width, value );
		seg->capture_plug_active = 0;
	}
	else
	{
		printf ( "Capture plug <%s> for area 0x%08X-0x%08X failed, removed\n", mm->segment->capture_plug, mm->segment->start, mm->segment->end  );
		seg->capture_plug = NULL;
	}

	return ret;
}

unsigned int 
v_cache_update ( t_workspace *ws, t_memmap *mm )
{
	if ( !mm )
		return E_FAIL;

	cache_start = mm->start;
	cache_end = mm->end;
	cache_data = mm->segment->data;
	cache_ws = ws;
	cache_mm = mm;
	cache_dirty = 0;

	return E_OK;
}

unsigned int
v_cache_flush ( )
{
	cache_start = -1;
	cache_end = -1;
	cache_ws = NULL;
	cache_mm = NULL;
	cache_data = NULL;
	cache_dirty = 0;

	return E_OK;
}

// used by v_set_inline
static INLINE_MODE unsigned int
v_set_inline_raw ( t_workspace *ws, unsigned char *data, unsigned int offset, unsigned int value, unsigned char width )
{
	int pos_word_1 = 0;
	int pos_word_2 = 0;
	int pos_half = 0;
	int pos_byte = 0;

	if ( !ws || !data )
		return 0;

	switch ( width )
	{
		case WIDTH_WORD:
			if ( (ws->flags&FLAGS_ENDIANESS) == FLAGS_ENDIANESS_LE )
			{
				data[offset + 3] = (value >> 24) & 0xFF;
				data[offset + 2] = (value >> 16) & 0xFF;
				data[offset + 1] = (value >> 8) & 0xFF;
				data[offset + 0] = (value >> 0) & 0xFF;
			}
			else
			{
				data[offset + 3] = (value >> 0) & 0xFF;
				data[offset + 2] = (value >> 8) & 0xFF;
				data[offset + 1] = (value >> 16) & 0xFF;
				data[offset + 0] = (value >> 24) & 0xFF;
			}
			break;

		case WIDTH_HALF:
			if ( (ws->flags&FLAGS_ENDIANESS) == FLAGS_ENDIANESS_LE )
			{
				data[offset + 1] = (value >> 8) & 0xFF;
				data[offset + 0] = (value >> 0) & 0xFF;
			}
			else
			{
				data[offset + 1] = (value >> 0) & 0xFF;
				data[offset + 0] = (value >> 8) & 0xFF;
			}			
			break;

		case WIDTH_BYTE:
			data[offset + 0] = value & 0xFF;
	}
	return 0;
}

// used by v_get_inline
static INLINE_MODE unsigned int
v_get_inline_raw ( t_workspace *ws, unsigned char *data, unsigned int offset, unsigned char width )
{
	int pos_word_1 = 24;
	int pos_word_2 = 16;
	int pos_half = 8;
	int pos_byte = 0;

	unsigned int retval = 0;

	if ( !ws || !data )
		return 0;


	switch ( width )
	{
		case WIDTH_WORD:
			if ( (ws->flags&FLAGS_ENDIANESS) == FLAGS_ENDIANESS_LE )
			{
				retval |= (data[offset + 3] << 24);
				retval |= (data[offset + 2] << 16);
				retval |= (data[offset + 1] << 8);
				retval |= (data[offset + 0] << 0);
			}
			else
			{
				retval |= (data[offset + 3] << 0);
				retval |= (data[offset + 2] << 8);
				retval |= (data[offset + 1] << 16);
				retval |= (data[offset + 0] << 24);
			}
			break;

		case WIDTH_HALF:
			if ( (ws->flags&FLAGS_ENDIANESS) == FLAGS_ENDIANESS_LE )
			{
				retval |= (data[offset + 1] << 8);
				retval |= (data[offset + 0] << 0);
			}
			else
			{
				retval |= (data[offset + 1] << 0);
				retval |= (data[offset + 0] << 8);
			}
			break;

		case WIDTH_BYTE:
			retval |= data[offset + 0];
	}
	return retval;
}



INLINE_MODE unsigned int
v_set_inline ( t_workspace *ws, unsigned int address, unsigned int value, unsigned char width )
{
	t_memmap *mm = NULL;

	if ( cache_enabled && address >= cache_start && (address+width-1) < cache_end && ws == cache_ws )
	{
		cache_write_hits++;
		v_set_inline_raw ( ws, cache_data, address - cache_start, value, width );
		if ( !cache_dirty )
		{
			stage_set_modified ( cache_mm->stage );
			workspace_set_modified ( cache_ws );
			cache_dirty = 1;
		}

		return 0;
	}
	cache_write_misses++;

	if ( !ws || !ws->memmap )
		return MEM_FAIL;

	mm = ws->memmap;
	while ( mm )
	{
		if ( address >= mm->start && address < mm->end && (address+width-1) < mm->end && mm->segment )
		{
			if ( mm->segment->capture_plug && mm->segment->capture_plug_active == 0 )
			{
				return v_call_plug ( ws, mm, address, value, MODE_WRITE, width );
			}
			else
			{
				/* sparse memory has no storage, reads fail */
				if(mm->segment->flags & FLAGS_SPARSE)
				{
					return MEM_FAIL;
				}

				/* only update cache if there is no plugin that handles accesses */
				if ( mm->segment->capture_plug == NULL )
				{
					v_cache_update ( ws, mm );
				}

				v_set_inline_raw ( ws, mm->segment->data, address - mm->start, value, width );
				stage_set_modified (mm->stage);
				workspace_set_modified(ws);

				return 0;
			}
		}
		LIST_NEXT(mm);
	}
	return MEM_FAIL;
}



static INLINE_MODE unsigned int
v_get_inline ( t_workspace *ws, unsigned int address, unsigned char width )
{
	t_memmap *mm = NULL;

	// if segment cached
	if ( cache_enabled && address >= cache_start && (address+width-1) < cache_end && ws == cache_ws )
	{
		cache_read_hits++;
		return v_get_inline_raw ( ws, cache_data, address - cache_start, width );
	}
	cache_read_misses++;

	if ( !ws || !ws->memmap || address == E_FAIL )
		return MEM_FAIL;

	mm = ws->memmap;
	while ( mm )
	{
		if ( address >= mm->start && address < mm->end  && (address+width-1) < mm->end && mm->segment )
		{
			if ( mm->segment->capture_plug && mm->segment->capture_plug_active == 0 )
			{
				return v_call_plug ( ws, mm, address, 0, MODE_READ, width );
			}
			else
			{
				/* sparse memory has no storage, reads fail. also caching makes no sense */
				if(mm->segment->flags & FLAGS_SPARSE)
				{
					return MEM_FAIL;
				}

				/* only update cache if there is no plugin that handles accesses */
				if ( mm->segment->capture_plug == NULL )
				{
					v_cache_update ( ws, mm );
				}

				return v_get_inline_raw ( ws, mm->segment->data, address - mm->start, width );
			}
		}
		LIST_NEXT(mm);
	}
	return MEM_FAIL;
}


static INLINE_MODE unsigned int
v_get_b_inline ( t_workspace *ws, unsigned int address )
{
	return v_get_inline ( ws, address, WIDTH_BYTE );
}

INLINE_MODE unsigned int
v_get_h_inline ( t_workspace *ws, unsigned int address )
{
	return v_get_inline ( ws, address, WIDTH_HALF );
}

INLINE_MODE unsigned int
v_get_w_inline ( t_workspace *ws, unsigned int address )
{
	return v_get_inline ( ws, address, WIDTH_WORD );
}

INLINE_MODE int
v_set_b_inline ( t_workspace *ws, unsigned int address, unsigned int value )
{
	return v_set_inline ( ws, address, value, WIDTH_BYTE );
}

INLINE_MODE int
v_set_h_inline ( t_workspace *ws, unsigned int address, unsigned int value )
{
	return v_set_inline ( ws, address, value, WIDTH_HALF );
}

INLINE_MODE int
v_set_w_inline ( t_workspace *ws, unsigned int address, unsigned int value )
{
	return v_set_inline ( ws, address, value, WIDTH_WORD );
}





INLINE_MODE unsigned int
v_set_b_raw_inline ( t_priv *s, unsigned int address, unsigned int value )
{
	t_segment *segment = NULL;
	t_stage *stage = NULL;

	if ( !s )
		return MEM_FAIL;

	if ( is_type ( s, "t_stage" ) )
	{
		stage = (t_stage*)s;
		if ( !stage->segments )
			return MEM_FAIL;

		segment = stage->segments;
		while ( segment )
		{
			// for stage access, check if the segment is mapped into memory to prevent file sections to override real data
			if ( address >= segment->start && address < segment->start + segment->length && segment_is_mapped ( segment )  )
			{
				if(segment_is_sparse(segment))
				{
					return MEM_FAIL;
				}
				segment->data[address - segment->start] = value;
				return 0;
			}
			LIST_NEXT(segment);
		}
	}
	if ( is_type ( s, "t_segment" ) )
	{
		segment = (t_segment*)s;
		if ( address >= segment->start && address < segment->start + segment->length  )
		{
			if(segment_is_sparse(segment))
			{
				return MEM_FAIL;
			}
			segment->data[address - segment->start] = value;
			return 0;
		}
	}

	return MEM_FAIL;
}


INLINE_MODE unsigned int
v_set_h_raw_inline ( t_stage *s, unsigned int address, unsigned int value )
{
	if ( (s->flags&FLAGS_ENDIANESS) == FLAGS_ENDIANESS_BE )
	{
		v_set_b_raw_inline(s,address, value>>8);
		v_set_b_raw_inline(s,address+1, value&0xFF);
	}
	else
	{
		v_set_b_raw_inline(s,address+1, value>>8);
		v_set_b_raw_inline(s,address, value&0xFF);
	}
	return 0;
}

INLINE_MODE unsigned int
v_set_w_raw_inline ( t_stage *s, unsigned int address, unsigned int value )
{
	if ( (s->flags&FLAGS_ENDIANESS) == FLAGS_ENDIANESS_BE )
	{
		v_set_h_raw_inline(s,address, value>>16);
		v_set_h_raw_inline(s,address+2, value&0xFFFF);
	}
	else
	{
		v_set_h_raw_inline(s,address+2, value>>16);
		v_set_h_raw_inline(s,address, value&0xFFFF);
	}
	return 0;
}

INLINE_MODE unsigned int
v_get_b_raw_inline ( t_priv *s, unsigned int address )
{
	t_segment *segment = NULL;
	t_stage *stage = NULL;

	if ( !s )
		return MEM_FAIL;

	if ( is_type ( s, "t_stage" ) )
	{
		stage = (t_stage*)s;
		if ( !stage->segments )
			return MEM_FAIL;

		segment = stage->segments;
		while ( segment )
		{
			// for stage access, check if the segment is mapped into memory to prevent file sections to override real data
			if ( address >= segment->start && address < segment->start + segment->length && segment_is_mapped ( segment )  )
			{
			}
			LIST_NEXT(segment);
		}
	}
	if ( is_type ( s, "t_segment" ) )
	{
		segment = (t_segment*)s;
		if ( address >= segment->start && address < segment->start + segment->length )
		{
			if(segment_is_sparse(segment))
			{
				return MEM_FAIL;
			}
            if(!segment->data || address < segment->start || (address - segment->start) > segment->length)
            {
                return MEM_FAIL;
            }
			return segment->data[address - segment->start] & 0xFF;
		}
	}

	return MEM_FAIL;
}


INLINE_MODE unsigned int
v_get_h_raw_inline ( t_stage *s, unsigned int address )
{
	if ( (s->flags&FLAGS_ENDIANESS) == FLAGS_ENDIANESS_BE )
		return ((v_get_b_raw_inline(s,address)<<8)|v_get_b_raw_inline(s,address+1)) & 0xFFFF;
	else
		return ((v_get_b_raw_inline(s,address+1)<<8)|v_get_b_raw_inline(s,address)) & 0xFFFF;
}

INLINE_MODE unsigned int
v_get_w_raw_inline ( t_stage *s, unsigned int address )
{
	if ( (s->flags&FLAGS_ENDIANESS) == FLAGS_ENDIANESS_BE )
		return ((v_get_h_raw_inline(s,address)<<16)|v_get_h_raw_inline(s,address+2)) & 0xFFFFFFFF;
	else
		return ((v_get_h_raw_inline(s,address+2)<<16)|v_get_h_raw_inline(s,address)) & 0xFFFFFFFF;
}

// special case!!    v_valid returns   1 on success and   0 on fail
INLINE_MODE int
v_valid_inline ( t_workspace *ws, unsigned int address )
{
	t_memmap *mm = NULL;

	if ( cache_enabled && address >= cache_start && address < cache_end && ws == cache_ws )
		return 1;

	if ( !ws || !ws->memmap )
		return 0;

	mm = ws->memmap;
	while ( mm )
	{
		if ( address >= mm->start && address < mm->end && mm->segment )
		{
			if ( mm->segment->capture_plug == NULL )
				v_cache_update ( ws, mm );

			return 1;
		}
		LIST_NEXT(mm);
	}
	return 0;
}

/*
	Real functions, using inline code
*/
unsigned int
v_get_size ( t_workspace *ws )
{
	t_memmap *mm = NULL;
    int size = 0;

	if ( !ws || !ws->memmap )
		return E_FAIL;

	mm = ws->memmap;
	while ( mm )
	{
		if ( mm->segment )
			size += mm->segment->length;
		LIST_NEXT(mm);
	}
	return size;
}


/*
	Real functions, using inline code
*/
t_segment *
v_get_segment ( t_workspace *ws, unsigned int address )
{
	t_memmap *mm = NULL;

	if ( !ws || !ws->memmap )
		return NULL;

	mm = ws->memmap;
	while ( mm )
	{
		if ( address >= mm->start && address < mm->end && mm->segment )
			return mm->segment;

		LIST_NEXT(mm);
	}
	return NULL;
}

unsigned int
v_get_areas ( t_workspace *ws )
{
	t_memmap *mm = NULL;
    int size = 0;

	if ( !ws || !ws->memmap )
		return 0;

	mm = ws->memmap;
	while ( mm )
	{
		if ( mm->segment )
			size++;
		LIST_NEXT(mm);
	}
	return size;
}

unsigned int
v_get_start ( t_workspace *ws )
{
	if ( !ws || !ws->memmap )
		return E_FAIL;  //-1 should be better than 0

	return ws->memmap->start;
}

unsigned int
v_get_end ( t_workspace *ws )
{
    t_memmap *mm = NULL;

    if ( !ws || !ws->memmap )
        return E_FAIL;

    mm = ws->memmap;

	LIST_FFWD(mm);

	return mm->end;
}

unsigned int
v_set_b ( t_workspace *ws, unsigned int address, unsigned int value )
{
	return v_set_b_inline ( ws, address, value );
}

unsigned int
v_set_h ( t_workspace *ws, unsigned int address, unsigned int value )
{
	return v_set_h_inline ( ws, address, value );
}

unsigned int
v_set_w ( t_workspace *ws, unsigned int address, unsigned int value )
{
	return v_set_w_inline ( ws, address, value );
}

unsigned int
v_set_b_raw ( t_stage *s, unsigned int address, unsigned int value )
{
	return v_set_b_raw_inline(s,address,value);
}

unsigned int
v_set_h_raw ( t_stage *s, unsigned int address, unsigned int value )
{
	return v_set_h_raw_inline(s,address,value);
}

unsigned int
v_set_w_raw ( t_stage *s, unsigned int address, unsigned int value )
{
	return v_set_w_raw_inline(s,address,value);
}

unsigned int
v_get_b ( t_workspace *ws, unsigned int address )
{
	return v_get_b_inline(ws,address);
}

unsigned int
v_get_h ( t_workspace *ws, unsigned int address )
{
	return v_get_h_inline(ws,address);
}

unsigned int
v_get_w ( t_workspace *ws, unsigned int address )
{
	return v_get_w_inline(ws,address);
}

unsigned int
v_get_b_raw ( t_stage *s, unsigned int address )
{
	return v_get_b_raw_inline(s,address);
}

unsigned int
v_get_h_raw ( t_stage *s, unsigned int address )
{
	return v_get_h_raw_inline(s,address);
}

unsigned int
v_get_w_raw ( t_stage *s, unsigned int address )
{
	return v_get_w_raw_inline(s,address);
}


/*
	----------------------
*/

unsigned int
v_get_strlen ( t_workspace *ws, unsigned int address )
{
	unsigned int len = 0;

	while ( v_valid_inline(ws, address) && v_get_b_inline(ws, address++) )
		len++;

	return len;
}

unsigned char *
v_get_str ( t_workspace *ws, unsigned int address )
{
	static char *buffer = NULL;
	int len = v_get_strlen (ws, address);

	if ( !buffer )
		buffer = malloc ( len + 1 );
	else
		buffer = realloc ( buffer, len+1 );

	v_memcpy_get ( ws, buffer, address, len );
	buffer[len] = '\000';

	return buffer;
}

unsigned int
v_get_strlen_raw ( t_stage *s, unsigned int address )
{
	unsigned int len = 0;

	while ( v_get_b_raw_inline(s, address++) )
		len++;

	return len;
}


unsigned char *
v_get_str_raw ( t_stage *s, unsigned int address )
{
	static char *buffer = NULL;
	unsigned int len = v_get_strlen_raw (s, address);

	if ( !buffer )
		buffer = malloc ( len + 1 );
	else
		buffer = realloc ( buffer, len+1 );

	v_memcpy_get_raw ( s, buffer, address, len );
	buffer[len] = '\000';

	return buffer;
}

unsigned char *
v_get_ptr ( t_workspace *ws, unsigned int address )
{
	t_memmap *mm = NULL;

	if ( cache_enabled && address >= cache_start && address < cache_end && ws == cache_ws )
		return &(cache_data[address - cache_mm->start]);

	if ( !ws || !ws->memmap )
		return NULL;

	mm = ws->memmap;
	while ( mm )
	{
		if ( address >= mm->start && address < mm->end && mm->segment )
		{
			if ( mm->segment->capture_plug == NULL )
			{
				v_cache_update ( ws, mm );
			}

			if(segment_is_sparse(mm->segment))
			{
				return NULL;
			}

			return &(mm->segment->data[address - mm->start]);
		}

		LIST_NEXT(mm);
	}
	return NULL;
}

unsigned int
v_valid ( t_workspace *ws, unsigned int address )
{
	return v_valid_inline ( ws, address );
}


unsigned int
v_get_next_offset ( t_workspace *ws, unsigned int offset )
{
    t_memmap *mm = NULL;

    offset++;

	if ( cache_enabled && offset >= cache_start && offset < cache_end && ws == cache_ws )
		return offset;

	if ( offset > MEM_MAX || !ws )
        return E_FAIL;

    mm = ws->memmap;

    while ( mm )
    {
        if ( offset >= mm->start && offset < mm->end && mm->segment )
		{
			if ( mm->segment->capture_plug == NULL )
				v_cache_update ( ws, mm );

			return offset;
		}
        if ( offset < mm->start )
		{
			if ( mm->segment->capture_plug == NULL )
				v_cache_update ( ws, mm );

			return mm->start;
		}
        LIST_NEXT(mm);
    }

    return E_FAIL;
}

unsigned int
v_get_prev_offset ( t_workspace *ws, unsigned int offset )
{
    t_memmap *mm = NULL;

    offset--;

	if ( cache_enabled && offset > cache_start && offset < cache_end && ws == cache_ws )
		return offset;

    if ( offset < 1 || !ws )
        return E_FAIL;


    mm = ws->memmap;
    while ( mm )
    {
        if ( offset >= mm->start && offset < mm->end && mm->segment )
		{
			if ( mm->segment->capture_plug == NULL )
				v_cache_update ( ws, mm );

			return offset;
		}
        if ( mm->prev && offset < mm->prev->end )
		{
			if ( mm->segment->capture_plug == NULL )
				v_cache_update ( ws, mm );

			return mm->prev->end;
		}

        LIST_NEXT(mm);
    }

    return E_FAIL;
}

/*
	translate into the correct endianess
	for now just think, we operate on a LE system.
	if stage is NULL, assume source is BE
*/
unsigned int
v_arch_h ( t_stage *s, int data )
{
	if ( !s || (s->flags&FLAGS_ENDIANESS) == FLAGS_ENDIANESS_BE )
		return ((data&0xFF)<<8)|((data>>8)&0xFF) & 0xFFFF;
	else
		return data & 0xFFFF;
}

unsigned int
v_arch_w ( t_stage *s, int data )
{
	if ( !s || (s->flags&FLAGS_ENDIANESS) == FLAGS_ENDIANESS_BE )
		return ((v_arch_h(s,data&0xFFFF)<<16)|v_arch_h(s,(data>>16)&0xFFFF)) & 0xFFFFFFFF;
	else
		return data & 0xFFFFFFFF;
}


unsigned int
v_valid_raw ( t_priv *s, unsigned int address )
{
	t_segment *segment = NULL;
	t_stage *stage = NULL;

	if ( !s )
		return MEM_FAIL;

	if ( is_type ( s, "t_stage" ) )
	{
		stage = (t_stage*)s;
		if ( !stage->segments )
			return 0;

		segment = stage->segments;
		while ( segment )
		{
			// recursion here. but that is safe
			if ( segment_is_mapped (segment) && v_valid_raw ( segment, address ) )
				return 1;
			LIST_NEXT(segment);
		}
	}

	if ( is_type ( s, "t_segment" ) )
	{
		segment = (t_segment*)s;
		if ( address >= segment->start && address < segment->start + segment->length  )
			return 1;
	}

	return 0;
}
unsigned int
v_memcpy_get_raw ( t_priv *s, unsigned char *dest, unsigned int src, unsigned int length )
{
	unsigned int pos = 0;

	while ( length-- )
	{
		dest[pos++] = (v_get_b_raw_inline ( s, src++ ) & 0xFF);
	}

	return E_OK;
}


unsigned int
v_memcpy ( t_workspace *ws, unsigned int dest, unsigned int src, unsigned int length )
{
	unsigned int pos = 0;

	while ( length-- )
	{
		v_set_b_inline ( ws, dest++, v_get_b_inline ( ws, src++ ) );
	}

	return E_OK;
}

unsigned int
v_memcpy_get ( t_workspace *ws, unsigned char *dest, unsigned int src, unsigned int length )
{
	unsigned int pos = 0;

	if ( !dest )
	{
		return E_FAIL;
	}

	while ( length-- )
	{
		dest[pos++] = v_get_b_inline ( ws, src++ );
	}

	return E_OK;
}

unsigned int
v_memcpy_put ( t_workspace *ws, unsigned int dest, unsigned char *src, unsigned int length )
{
	unsigned int pos = 0;
	t_segment *current_seg = NULL;
	t_segment *existing_seg = NULL;
	t_stage *current_stage = NULL;

	/* quick solution, areas are contiguous and mapped correctly */
	current_seg = v_get_segment(ws, dest);
	if(current_seg)
	{
		/* its not copying beyond end? */
		if((current_seg->length - (dest - current_seg->start)) >= length)
		{
			/* mapped correctly in memmap? */
			if(current_seg->data == v_get_ptr(ws, current_seg->start))
			{
				memcpy(v_get_ptr(ws, dest), src, length);
				return E_OK;
			}
		}
	}

	while ( length )
	{
		if ( v_valid ( ws, dest ) )
		{
			// when initializing with NULL source dont overwrite existing memory areas
			if ( src )
			{
				v_set_b_inline ( ws, dest++, src[pos++] );
			}
			else
			{
				dest++;
			}

			current_seg = NULL;
		}
		else if ( ws->fileinfo && ws->fileinfo->stages )
		{
			unsigned int maxlen = 0;

			/* find the segment that can be extended */
			if ( !current_seg ) 
			{
				current_stage = stage_get_current ( ws->fileinfo->stages );
				if ( !current_stage )
					return E_FAIL;
                current_seg = segment_find_by_end ( current_stage->segments, dest, FLAGS_MAP_IN_MEM );

				/* no segment found, create a new one */
                if ( !current_seg )
				{
                    current_seg = segment_add ( current_stage->segments );
					if ( !current_seg )
						return E_FAIL;

					current_seg->data = malloc ( length );
					current_seg->start = dest;
					current_seg->end = dest;
					current_seg->length = 0;
					current_seg->name = "AUTO";
				}

				/* try to find the nearest segment to know how large we can make ours */
				for(maxlen = 1; maxlen < length; maxlen++)
				{
					if(segment_find_by_start ( current_stage->segments, dest + maxlen, FLAGS_MAP_IN_MEM ))
					{
						break;
					}
				}

				current_seg->data = realloc ( current_seg->data, current_seg->length + maxlen );
				current_seg->length += maxlen;
				current_seg->end += maxlen;

				/* need some locking!! would crash on simultanous access */
				/* we need this update since the memmap would not be correct */
				workspace_update_memmap ( ws );
			}

			if ( src )
				v_set_b_inline ( ws, dest++, src[pos++] );
			else
				v_set_b_inline ( ws, dest++, 0xFF );
		}

		length--;
	}

	return E_OK;
}



unsigned int
util_swap_half ( unsigned int v )
{
	return ( v>>8 ) | ((v&0xFF)<<8);
}

unsigned int
util_swap_word ( unsigned int v )
{
	return util_swap_half ( v>>16 ) | ((util_swap_half ( v&0xFFFF )<<16));
}

/*!
 * check if the given struct type is correct for a struct
 * \return 0 for FALSE and 1 for TRUE
 * \param data filled standard TriX structure
 * \param type name of the struct
 */
unsigned int
is_type ( t_priv *data, char *type )
{
	if ( !strncmp ( data->struct_name, type, strlen (type) ) )
		return 1;

	return 0;
}


/*
	unsigned int util_hex_get ( char *data, int *err)
   -----------------------------------------------------

    Description:
		converts the string given at *data into a hex number

    Return value:
		the value and in case of error a 1 in *err
*/
unsigned int util_hex_get ( char *data, int *err )
{
	int val = 0;
	int nibbles = 0;

	*err = 0;

	if ( !data )
	{
		*err = 1;
		return 0;
	}

	while ( *data == '0' || *data == '\\' || *data == 'x' || *data == 'X' || *data == ' ' )
		data++;

	while ( *data )
	{
		if ( nibbles == 8 )
		{
			// too many bytes to convert to integer
			*err = 1;
			return 0;
		}
		val *= 16;
		if ( *data >= '0' && *data <= '9' )
			val += *data - '0';
		else if ( *data >= 'a' && *data <= 'f' )
			val += 10 + (*data - 'a');
		else if ( *data >= 'A' && *data <= 'F' )
			val += 10 + (*data - 'A');
		else if ( *data != 'h' && *data != ' ' )
		{
			*err = 1;
			return 0;
		}
		else
			return val;

		data++;
		nibbles++;
	}
	return val;
}


/*
	unsigned int util_hex_get_buffer ( char *data, int *len)
   -----------------------------------------------------

    Description:
		converts the string given at *data into a binary buffer

    Return value:
		the buffer and the size or E_FAIL in len
*/
unsigned char *util_hex_get_buffer ( char *data, int *len )
{
	unsigned char *buffer = NULL;
	int val = 0;
	int nibbles = 0;

	*len = 0;

	if ( !data )
	{
		*len = E_FAIL;
		return NULL;
	}

	while ( *data == '\\' || *data == 'x' || *data == 'X' || *data == ' ' )
		data++;

	while ( 1 )
	{
		if ( nibbles == 2 )
		{
			if ( buffer )
				buffer = realloc ( buffer, (*len) + 1 );
			else
				buffer = malloc ( (*len) + 1 );

			if ( !buffer )
				return NULL;

			buffer[(*len)] = val;
			val = 0;
			nibbles = 0;
			(*len)++;
		}
		val *= 16;
		if ( *data >= '0' && *data <= '9' )
			val += *data - '0';
		else if ( *data >= 'a' && *data <= 'f' )
			val += 10 + (*data - 'a');
		else if ( *data >= 'A' && *data <= 'F' )
			val += 10 + (*data - 'A');
		else if ( *data != 'h' && *data != ' ' && *data != '\000' )
		{
			*len = E_FAIL;
			if ( buffer )
				free ( buffer );
			return NULL;
		}
		else
		{
			if ( nibbles )
			{
				/* broke after one nibble... we only allow bytes! */
				*len = E_FAIL;
				if ( buffer )
					free ( buffer );
				return NULL;
			}
			return buffer;
		}

		data++;
		nibbles++;
	}
	return buffer;
}

/*
	char *util_hexunpack ( char *data, int *length )
   -----------------------------------------------------

    Description:
		converts the string given at *data into a data buffer

    Return value:
		a buffer to the unpacked data or NULL in case of error
*/
char *util_hexunpack ( char *data, int *length )
{
	char *buff = NULL;
	char byte[3];
	int ipos = 0;
	int opos = 0;
	int err = 0;

	
	if ( !data || !length )
		return NULL;

	buff = malloc ( strlen ( data ) / 2 + 1);
	if ( !buff )
		return NULL;

	if ( strlen ( data ) == 0 )
	{
		*length = 0;
		return buff;
	}

	while ( data[ipos] )
	{
		while ( data[ipos] && ( data[ipos] == '\r' || data[ipos] == '\n' || data[ipos] == ' ' ) )
			ipos++;
		if ( data[ipos] )
		{
			byte[0] = data[ipos];
			ipos++;
		}
		while ( data[ipos] && ( data[ipos] == '\r' || data[ipos] == '\n' || data[ipos] == ' ' ) )
			ipos++;
		if ( data[ipos] )
		{
			byte[1] = data[ipos];
			ipos++;
	
			byte[2] = '\000';
			buff[opos++] = util_hex_get ( byte, &err );
			if ( err )
				return NULL;
		}
	}
	if ( length )
		*length = opos;

	return buff;
}


/*
	char *util_hexpack ( char *data, int bytes )
   -----------------------------------------------------

    Description:
		converts the data given at *data into a hexstring

    Return value:
		a buffer to the string or NULL in case of error
*/
char *util_hexpack ( unsigned char *data, int bytes )
{
	char *buff = NULL;
	char *buffptr = NULL;
	char byte[3];
	int pos = 0;

	if ( !data  )
		return NULL;

	buff = malloc ( 2*bytes + 1 );
	strcpy ( buff, "" );

	buffptr = buff;
	while ( pos < bytes )
	{
		sprintf ( &buffptr[2*pos], "%02X", (data[pos] & 0xFF) );
		pos++;
	}

	return buff;
}


t_list *
util_list_get_last ( t_list *l )
{
	if ( !l )
		return NULL;

	LIST_FFWD(l);

	return l;
}

unsigned int
util_list_add (  t_list *l1,  t_list *l2  )
{
	t_list *l = NULL;

	l = util_list_get_last ( l1 );
	if ( !l )
		return E_FAIL;

	l->next = l2;
	if ( l2 )
		l2->prev = l;

	return E_OK;
}


t_list *
util_list_add_new (  t_list *l1,  int size, char *name )
{
	t_list *l = NULL;

	DBG(DEBUG_LIST, "%s: <%s>\n", __FUNCTION__, name );
	l = util_smalloc ( size, name );
	if ( !l )
		return NULL;

	util_list_add ( l1, l );

	return l;
}

unsigned int
util_list_free ( t_list *l )
{
	if ( !l )
		return E_FAIL;

	LIST_FFWD(l);

	while ( l->prev )
	{
		LIST_PREV(l);
		if ( l->next->struct_refs )
			l->next->struct_refs--;
		else
			free ( l->next );
	}
	if ( l->struct_refs )
		l->struct_refs--;
	else
		free ( l );

	return E_OK;
}

unsigned int
util_list_count ( t_list *l )
{
	int count = 0;
	if ( !l )
		return E_FAIL;

	LIST_REWND(l);

	while ( l )
	{
		count++;
		LIST_NEXT(l);
	}

	return count;
}

INLINE_MODE unsigned int
util_check_pattern_inline ( t_workspace *ws, unsigned int offset, t_locator * loc )
{
	unsigned int pos = 0;

	/* g3gg0:
	       speedup tweak: removed get_ptr checking, instead do if we found the pattern
	       speedup tweak: just one XOR instead of two
	       speedup tweak: first check pattern (more often fails than pos check)
	*/
    while ( ((v_get_b_inline ( ws, offset + pos ) ^ loc->pattern[pos]) & loc->mask[pos]) == 0  &&
		pos < loc->length )
		pos++;

    if ( pos == loc->length )
	{
		// check if all bytes are in a "valid" area
		while ( pos-- )
		{
			if ( !v_valid_inline( ws, offset + pos ) )
				return E_FAIL;
		}
        return E_OK;
	}

    return E_FAIL;
}

unsigned int
util_check_pattern ( t_workspace *ws, unsigned int offset, t_locator * loc )
{
	/* needed for util_check_pattern_inline() */
	v_cache_flush ();

	return util_check_pattern_inline ( ws, offset, loc );
}


INLINE_MODE unsigned int
util_check_pattern_raw_inline ( t_stage *s, unsigned int offset, t_locator * loc )
{
    unsigned int pos = 0;

	if ( !s || !s->segments )
		return E_FAIL;

	if ( offset + loc->length > s->segments->length )
        return E_FAIL;

	if(segment_is_sparse(s->segments))
	{
		return E_FAIL;
	}

	while ( pos < loc->length && ( s->segments->data[offset + pos] & loc->mask[pos] ) == ( loc->pattern[pos] & loc->mask[pos] ) )
        pos++;

    if ( pos == loc->length )
        return E_OK;

    return E_FAIL;
}

unsigned int
util_check_pattern_raw ( t_stage *s, unsigned int offset, t_locator * loc )
{
	return util_check_pattern_raw_inline ( s, offset, loc );
}

unsigned int
util_quick_find_pattern ( t_workspace *ws, char *pattern, char *mask, unsigned int size, unsigned int start, unsigned int end, int options )
{
    t_locator loc;
	static int reserved = 100;
	static char *reserved_buf = NULL;
    int ret = -1;

    if ( !ws || !pattern )
        return E_FAIL;

	loc.func_name = "QuickFindPattern";

	if ( !reserved_buf )
	{
		reserved_buf = (char*) malloc ( reserved );
		memset ( reserved_buf, 0xff, reserved );
	}

    loc.pattern = pattern;
    loc.length = size;
    loc.options = 0;

    if ( !mask )
    {
		if ( reserved < size )
		{
			reserved = size;
			reserved_buf = (char*) realloc ( reserved_buf, reserved );
			memset ( reserved_buf, 0xff, reserved );
		}
		loc.mask = reserved_buf;
    }
    else
        loc.mask = mask;


    ret = util_find_pattern ( ws, &loc, start, end, options );


    return ret;
}

unsigned int
util_simple_find_pattern ( t_workspace *ws, char *pattern, char *mask, unsigned int size, int options )
{
	return util_quick_find_pattern (ws, pattern, mask, size, MEM_AUTO, MEM_AUTO, options );
}

unsigned int
util_find_pattern ( t_workspace *ws, t_locator * loc, unsigned int start, unsigned int end, int options )
{
	int found = 0;
	unsigned int ret_ofs = 0;
	unsigned int ofs = 0;
	unsigned int (*offset_func) (t_workspace *ws, unsigned int addr );

	if ( !ws || !loc )
        return E_FAIL;

	if ( start == MEM_AUTO )
		start = v_get_start ( ws );

    if ( end == MEM_AUTO )
        end = v_get_end ( ws );

    offset_func = (options&LOC_BACKWARD)?v_get_prev_offset:v_get_next_offset;

	 if ( options & LOC_BACKWARD )
		 ofs = end;
	 else
			ofs = start;

	/* needed for util_check_pattern_inline() */
	v_cache_flush ();

	while ( start <= ofs && ofs <= end && ofs != E_FAIL )
    {
        if ( util_check_pattern_inline ( ws, ofs, loc ) == E_OK )
        {
            if ( options & LOC_UNIQUE )
            {
                if ( found )
                    return E_FAIL;
            }
            else
                return ofs;

            found = 1;
            ret_ofs = ofs;
        }
        ofs = offset_func ( ws, ofs );
    }

    if ( found )
        return ret_ofs;

    return E_FAIL;
}


/* search a block ( ".text", ".bss", ... ) in a function struct and return size */
/* old function - should not be used! */
unsigned int
util_block_get_size ( t_function *func, const char *name )
{
    //int act_block = 0;
    struct s_function_block *act_block = NULL;

    if ( !func || !name || !func->blocks )
        return E_FAIL;

    //printf ( "%s", func->blocks->name );
    DBG ( DEBUG_UNKNOWN, "## %s: get size of %s\n", __FUNCTION__, func->blocks->name );

    act_block = func->blocks;

    while ( act_block && act_block->length && act_block->data )
    {
        if ( !strcmp ( name, act_block->name ) )
            return act_block->length;

        act_block += sizeof ( struct s_function_block );
    }

    return E_FAIL;
}

/* count all segments beginning with a dot, e.g. .text, .rodata */
/*!
 * count all segments in an object beginning with a dot, e.g. .text, .rodata
 * \return count or E_FAIL when it's not a legal object
 * \param o filled object structure
 */
unsigned int util_block_get_segment_count ( object *o )
{

    int count = 0, i = 0;
    if ( !o || !o[0] )
        return E_FAIL;

    while ( o[i] )
    {
        if ( !o[i+1] || !o[i+2] )
            return E_FAIL;
        if ( !strncmp ( o[i], ".", 1 ) )
            count++;
        i += 3;
    }

    return count;
}

/*!
 * get the occurence'th segment name ptr
 * \param o object
 * \param occurence the occurence'th occurence
 * \return occurence'th segment name ptr or NULL (error)
 */
char* util_block_get_segment_name_ptr ( object *o, int occurence )
{
    int i = 0;
    if ( !o || !o[0] )
        return NULL;

    while ( o[i] )
    {
        if ( !strncmp ( o[i], ".", 1 ) /*o[i][0] == '.'*/ )
            if ( !occurence-- )
                return o[i];
        if ( !o[i+1] || !o[i+2] )
            return NULL;
        i += 3;
    }

    return NULL;
}


/* converts hex and dezimal string to integer, else returns E_FAIL */
/*!
 * converts a hexadezimal or dezimal string to an integer
 * \return parsed int or E_FAIL
 * \param str a 0 terminated array
 */
unsigned int
util_str2int ( const char *str )
{
    int val = -1;

    if ( !str )
        return E_FAIL;

    if ( !strncmp ( str, "0x", 2 ) )
    {
        sscanf ( str+2, "%x", &val );
        return val;
    }
    if ( str[0] >= '0' && str[0] <= '9' )
    {
        sscanf ( str, "%d", &val );
        return val;
    }

    return E_FAIL;
}

/*
    returns offset of symbol or length of .text, .data, ...
    if !SYM_CALL is more than onced used and you want to get the second,
    let occurence be 1, for the third symbol 2 and so on
*/
unsigned int
util_block_get_number ( const t_function2 *func, const char *name, int occurence )
{
    //char *act_str = NULL;
    int i = 0;

    if ( !func || !name || !func[0] )
        return E_FAIL;

    //act_str = func[0];
    //printf ( "test" );


    while ( func[i] )
    {
        DBG ( DEBUG_UNKNOWN, "## %s: checking %s\n", __FUNCTION__, func[i] );
        if ( !func[i+1] )
            return E_FAIL;
        if ( !func[i+2] )
            return E_FAIL;
        if ( !strcmp ( name, func[i] ) )
            if ( !occurence-- )
                return util_str2int ( func[i+2] );
        i+=3;
    }



    return E_FAIL;
}

/*
    gets string pointer to data or symbol destination
    attention returns NULL on fail!
    occurence, see above.
*/
char *util_block_get_val_ptr ( const t_function2 *func, const char *name, int occurence )
{
    int i = 0;

    if ( !func || !name || !func[0] )
        return NULL;

    while ( func[i] )
    {
        if ( !func[i+1] )
            return NULL;
        if ( !func[i+2] )    //needed for jumping to next line
            return NULL;
        if ( !strcmp ( name, func[i] ) )
            if ( !occurence-- )
                return func[i+1];
        i+= 3;
    }

    return NULL;
}

/*!
 * get pointer to offset value in object symbols
 * \code
 * object example[] =
 * {
 *     "name", "example", "",
 *     ".text", "12345678901234567890", "20",
 *     "!SYM_CALL", "dest()", ".text+0x10",
 *     "!SYM_CALL", "dest2()", ".text",
 *     NULL
 * }
 * util_block_get_offset_ptr ( example, "!SYM_CALL", 0 ); // => returns ".text+0x10"
 * util_block_get_offset_ptr ( example, "!SYM_CALL", 1 ); // => returns ".text"
 * \endcode
 * \return pointer to offset data or NULL (error)
 * \param occurence the occurence'th occurence
 * \param o the object
 * \param name the destination or code to patch; generally: the 2nd parameter in a tripplet :)
 */
char *util_block_get_offset_ptr ( const object *o, const char *name, int occurence )
{
    int i = 0;

    if ( !o || !o[0] )
        return NULL;


    while ( o[i] )
    {
        DBG ( DEBUG_UNKNOWN, "## %s: checking %s\n", __FUNCTION__, o[i] );
        if ( !o[i+1] || !o[i+2] )
            return NULL;
        if ( !strcmp ( name, o[i] ) )
            if ( !occurence-- )
                return o[i+2];
        i+=3;
    }



    return NULL;
}

/* search a block and returns data pointer */
unsigned char *util_block_get_data_pointer ( const t_function *func, const char *name )
{
        struct s_function_block *act_block = NULL;

    if ( !func || !name || !func->blocks )
        return (unsigned char*)E_FAIL; // should return NULL!!

    DBG ( DEBUG_UNKNOWN, "## %s: get data pointer of %s\n", __FUNCTION__, func->blocks->name );

    act_block = func->blocks;

    while ( act_block && act_block->length && act_block->data )
    {
        if ( !strcmp ( name, act_block->name ) )
            return act_block->data;

        act_block += sizeof ( struct s_function_block );
    }

    return (unsigned char*)E_FAIL; // should return NULL!!
}


/*!
 * free's an allocated object and all strings in it
 * \param o an allocated object
 * \return E_OK in all cases
 */
unsigned int
util_free_object ( object *o )
{
    int i = 0;
    if ( !o )
        return E_OK;

    while ( o[i] )
        free ( o[i++] );

    free ( o );
    return E_OK;
}

/*!
 * dumps an object
 * \param o an object
 */
void
util_dump_object ( object *o )
{
    int i = -1;
    if ( !o )
        return;

    printf ( "object x[] =\n{" );

    //check for a segment beginning with . and print as readable string
    while ( o[++i] )
        printf ( "%s\"%s\",", !(i % 3) ? "\n\t" : " ", o[i] );

    printf ( "\n\tNULL\n};\n" );
}



char *
util_split ( char *string, unsigned char seperator, int which )
{
	static unsigned char *buffer = NULL;
	static unsigned int buffer_length = 1;
	unsigned char *ret = NULL;
	int pos = 0;

	if ( !buffer )
		buffer = malloc ( buffer_length );

	if ( !buffer )
		return NULL;

	if ( buffer_length < strlen (string) + 1 )
	{
		buffer_length = strlen ( string ) + 1;
		buffer = realloc ( buffer, buffer_length );
		if ( !buffer )
			return NULL;
	}

	strcpy ( buffer, string );

	/* search for "which"th string */
	while ( which )
	{
		while ( buffer[pos] && buffer[pos] != seperator )
			pos++;
		if ( buffer[pos] == seperator )
			pos++;
		which--;
	}

	if ( buffer[pos] )
	{
		/* remember its start */
		ret = &(buffer[pos]);

		/* search for its end */
		while ( buffer[pos] && buffer[pos] != seperator )
			pos++;
		/* if found, replace with NUL */
		if ( buffer[pos] == seperator )
			buffer[pos] = '\000';

		return ret;
	}

	return NULL;
}

void util_cache_enable ( )
{
	cache_enabled = 1;
}
void util_cache_disable ( )
{
	cache_enabled = 0;
}


#endif
