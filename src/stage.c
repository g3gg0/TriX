#ifndef __TRIX_STAGE_C__
#define __TRIX_STAGE_C__

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

STAGE_PLUG_INIT;

/*!
 * Merges the segments in this stage into the given buffer.
 * \param s stage to get merged
 * \param buffer buffer that should get filled with data
 * \param length size of the buffer's size
 * \return E_FAIL if error occured, else the memory address
 */
unsigned int stage_merge ( t_stage * s, unsigned char *buffer, unsigned int length )
{
	unsigned int start_address = 0;
	unsigned int end_address = 0;
	t_segment *seg = NULL;

	if ( !s || !buffer || !length )
		return E_FAIL;

	/* retrieve stage size */
	start_address = stage_get_start ( s );
	end_address = stage_get_end ( s );

	if ( start_address == E_FAIL || end_address == E_FAIL )
		return E_FAIL;

	/* buffer big enough? */
	if ( length < end_address - start_address )
		return E_FAIL;

	/* now merge all the segments */
	seg = s->segments;
	while ( seg )
	{
		/* just merge if the segment is mapped into memory */
		if ( segment_is_mapped ( seg ) )
			memcpy ( buffer + (seg->start - start_address), seg->data, seg->length );
		LIST_NEXT(seg);
	}

	return E_OK;

}

/*!
 * Return the lowest start address.
 * \param s stage to get scanned
 * \return E_FAIL if error occured, else the memory address
 */
unsigned int stage_get_start ( t_stage * s )
{
	t_segment *seg = NULL;
	unsigned int address = E_FAIL;

	if ( !s )
		return E_FAIL;

	seg = s->segments;
	while ( seg )
	{
		if ( (address == E_FAIL || seg->start < address) && segment_is_mapped ( seg ) )
			address = seg->start;
		LIST_NEXT(seg);
	}

	return address;
}
/*!
 * Return the highest end address.
 * \param s stage to get scanned
 * \return E_FAIL if error occured, else the memory address
 */
unsigned int stage_get_end ( t_stage * s )
{
	t_segment *seg = NULL;
	unsigned int address = E_FAIL;

	if ( !s )
		return E_FAIL;

	seg = s->segments;
	while ( seg )
	{
		if ( (address == E_FAIL || seg->end > address) && segment_is_mapped ( seg ) )
			address = seg->end;
		LIST_NEXT(seg);
	}

	return address;
}

/*!
 * Checks if there is one unmapped segment in a stage.
 * \param s stage to get checked
 * \return 0 if no unmapped found, 1 if one or more were found
 */
unsigned int stage_has_unmapped ( t_stage * s )
{
	unsigned int pos = 0;
	unsigned int segments = 0;
	t_segment *seg = NULL;

	if ( !s )
		return 0;

	segments = segment_count ( s->segments );

	for ( pos = 0; pos < segments; pos++ )
	{
		seg = segment_find_by_num ( s->segments, pos );

		if ( !segment_is_mapped ( seg ) )
			return 1;
	}
	return 0;
}

/*!
 * Checks if the segments in a stage overlap in memory.
 * \param s stage to get checked
 * \return 0 if no overlaps found, 1 if one or more were found
 */
unsigned int stage_has_overlaps ( t_stage * s )
{
	unsigned int pos1 = 0;
	unsigned int pos2 = 0;
	unsigned int segments = 0;
	t_segment *seg1 = NULL;
	t_segment *seg2 = NULL;


	if ( !s )
		return 0;

	segments = segment_count ( s->segments );

	for ( pos1 = 0; pos1 < segments; pos1++ )
	{
		seg1 = segment_find_by_num ( s->segments, pos1 );

		for ( pos2 = pos1+1; pos2 < segments; pos2++ )
		{
			seg2 = segment_find_by_num ( s->segments, pos2 );

			/* 
				just check, when segment hast start and end 
				so we will ignore e.g. BT segments
			*/
			if ( segment_is_mapped ( seg1 ) )
			{

				/*
					 1111111111 
					222222222222
				*/
				if ( seg1->start >= seg2->start && seg1->end <= seg2->end )
					return 1;
				/*
					1111111111 
						22222222222
				*/
				if ( seg1->start <= seg2->start && seg1->end >= seg2->start )
					return 1;
				/*
						  1111111111 
					222222222222
				*/
				if ( seg1->start <= seg2->end && seg1->end >= seg2->end )
					return 1;
			}
		}
	}

	return 0;

}


unsigned int
stage_replace ( t_stage * t, t_stage * s )
{
	if ( !s || !t )
		return E_FAIL;

	s->next = t->next;
	s->prev = t->prev;

	t->flags |= FLAGS_REPLACE;
	stage_release ( t );

	memcpy ( t, s, sizeof ( t_stage ) );
	CHECK_AND_FREE ( s );

	return E_OK;
}

t_stage *
stage_duplicate ( t_stage * s )
{
	t_stage *t = NULL;
	t_segment *segment = NULL;
	int segments = 0;
	int pos = 0;

	if ( !s )
		return NULL;

	t = stage_create ( );
	if ( !t )
		return NULL;

	segments = segment_count ( s->segments );
	while ( pos < segments )
	{
		if ( !pos )
		{
			t->segments = segment_duplicate ( segment_find_by_num ( s->segments, pos ) );
			segment = t->segments;
		}
		else
		{
			segment->next = segment_duplicate ( segment_find_by_num ( s->segments, pos ) );
			segment->next->prev = segment;
			segment = segment->next;
		}
		if ( !segment )
			return NULL;
		pos++;
	}
	t->flags = s->flags;
	t->length = s->length;
	t->struct_magic = s->struct_magic;

	if ( s->flags & FLAGS_FREE_NAME )
		t->name = strdup ( s->name );
	else
		t->name = s->name;

	t->parser = s->parser;
	t->symbols = s->symbols;
	t->type = s->type;
	t->next = NULL;
	t->prev = NULL;
	
	if ( s->priv )
		s->priv->struct_refs++;
	t->priv = s->priv;

	return t;
}

t_stage *
stage_add ( t_stage * s )
{
    if ( s->next )
        return NULL;
    s->next = stage_create ( );
    if ( !s->next )
        return NULL;
    s->next->prev = s;

    return s->next;
}


t_stage *
stage_create ( )
{
    t_stage *s = NULL;

    s = LIST_MALLOC(t_stage);
    if ( !s )
        return NULL;
	s->struct_magic = M_STAGE;
    s->name = "EMPTY";
    s->type = "EMPTY";
    s->parser = "EMPTY";
    s->next = NULL;
    s->prev = NULL;
    s->priv = NULL;
    s->flags = FLAGS_ENDIANESS_BE;
    s->segments = NULL;
    s->length = 0;
    s->symbols = NULL;

    return s;
}

t_stage *
stage_find_by_num ( t_stage * s, unsigned int num )
{
    if ( !s )
        return NULL;

    while ( s->prev )
        s = s->prev;

    while ( s )
    {
        if ( !num-- )
            return s;
        s = s->next;
    }

    return NULL;
}

t_stage *
stage_get_last ( t_stage * s )
{
    if ( !s )
        return NULL;

    while ( s->next )
        s = s->next;

    return s;
}

unsigned int
stage_restore_info ( t_stage * s, t_stage_info * i )
{
	if ( !s || !i )
		return E_FAIL;

	if ( LIST_COUNT ( s ) != LIST_COUNT ( i )  )
		return E_FAIL;

	while ( s && i )
	{
		s->flags = i->flags;
		s = s->next;
		i = i->next;
	}

	return E_OK;
}

unsigned int
stage_release_info ( t_stage_info *info )
{
	LIST_FFWD(info);

	while ( info )
	{
		t_stage_info *entry = info;
		LIST_PREV(info);
		free ( entry );
	}

	return E_OK;
}

t_stage_info *
stage_save_info ( t_stage * s )
{
	t_stage_info *i = NULL;

	if ( !s )
		return NULL;

	i = LIST_MALLOC(t_stage_info);
	i->flags = 0;
	i->struct_magic = M_STAGE_INFO;
	i->prev = NULL;
	i->next = NULL;

	while ( s )
	{
		i->flags = s->flags;
		s = s->next;
		if ( s )
		{
			i->next = LIST_MALLOC(t_stage_info);
			i->next->flags = 0;
			i->next->struct_magic = M_STAGE_INFO;
			i->next->prev = i;
			i->next->next = NULL;
			i = i->next;
		}
	}
	while ( i->prev )
		i = i->prev;

	return i;
}


unsigned int
stage_count ( t_stage * s )
{
	return LIST_COUNT(s);
}


unsigned int
stage_get_num ( t_stage * s )
{
    unsigned int pos = 0;

    if ( !s )
        return E_FAIL;

    while ( s->prev )
    {
        s = s->prev;
        pos++;
    }

    return pos;
}

unsigned int
stage_get_segments ( t_stage * s )
{
	if ( !s )
		return E_FAIL;
	return segment_count ( s->segments );
}

unsigned int
stage_dump ( t_stage * s )
{
    while ( s )
    {
        TAB;
        printf ( "-----------------------------\n" );
        TAB;
        printf ( "Dump of stage <%s>\n", s->name );
        TAB;
        printf ( "-----------------------------\n" );
        TAB;
        printf ( " Name:     <%s>\n", s->name );
        TAB;
        printf ( " Parser:   <%s>\n", s->parser );
        TAB;
        printf ( " Type:     <%s>\n", s->type );
        TAB;
        printf ( " Length:   <0x%08X>\n", s->length );
        TAB;
		printf ( " Flags:    <0x%02X>\n", s->flags );
        TABINC;
        segment_dump ( s->segments );
        TABDEC;
        s = s->next;
    }
    return E_OK;
}

t_stage *
stage_get ( t_stage * s, unsigned int flag )
{

    while ( s )
    {
        if ( s->flags & flag )
            return s;
        s = s->next;
    }

    return NULL;
}

unsigned int
stage_clear_flags ( t_stage * s, unsigned int flag )
{
	if ( !s )
		return E_FAIL;

	while ( s->prev )
		s = s->prev;

	while ( s )
	{
		if ( s->flags & flag )
			s->flags &= ~flag;
		s = s->next;
	}

	return E_OK;
}

t_stage *
stage_get_modified ( t_stage * s )
{
    return stage_get ( s, S_MODIFIED );
}

t_stage *
stage_get_savestage ( t_stage * s )
{
    return stage_get ( s, S_SAVESTAGE );
}

t_stage *
stage_get_workspace ( t_stage * s )
{
    return stage_get ( s, S_WORKSPACE );
}

unsigned int
stage_set_modified ( t_stage * s )
{
	R (	stage_clear_flags ( s, S_MODIFIED ) );
	if ( s )
		s->flags |= S_MODIFIED;
    return E_OK;
}

unsigned int
stage_set_savestage ( t_stage * s )
{
	R (	stage_clear_flags ( s, S_SAVESTAGE ) );

	if ( s )
		s->flags |= S_SAVESTAGE;

    return E_OK;
}

unsigned int
stage_set_workspace ( t_stage * s )
{
	R (	stage_clear_flags ( s, S_WORKSPACE ) );

	if ( s )
		s->flags |= S_WORKSPACE;

    return E_OK;
}

t_stage *
stage_get_current ( t_stage * s )
{
	t_stage *d = NULL;

    d = stage_get ( s, S_WORKSPACE );
	if ( !d )
		return stage_get_last ( s );

	return d;
}

/*
    release a stage and corrects previous and next pointer

*/
unsigned int
stage_release ( t_stage * s )
{
    int failed = E_OK;

    if ( !s )
        return E_FAIL;

	R ( fmt_free_priv ( s ) );

    if ( s->symbols )
		if ( symbols_free_all ( s ) == E_FAIL )
			failed = E_FAIL;

    if ( s->segments )
        if ( segment_release_all ( s->segments ) == E_FAIL )
            failed = E_FAIL;

	if ( s->flags & FLAGS_FREE_NAME )
		CHECK_AND_FREE ( s->name );

	/* 
	when the flag for replacing was set, dont update pointers since the
	entry will not get deleted, but just replaced
	*/
	if ( !(s->flags&FLAGS_REPLACE) )
	{
		if ( s->prev && (s->prev->next == s) )
			s->prev->next = s->next;
		if ( s->next && (s->next->prev == s) )
			s->next->prev = s->prev;

		CHECK_AND_FREE ( s );
	}
    return failed;
}

/*
    frees this stage and all next, corrects pointer of previous
*/
unsigned int
stage_release_all ( t_stage * s )
{
    unsigned int failed = E_OK;

    if ( !s )
        return E_FAIL;

	if ( s->prev )
		s->prev->next = NULL;

    while ( s->next )
    {
        s = s->next;
        if ( stage_release ( s->prev ) != E_OK )
            failed = E_FAIL;
    }
    R ( stage_release ( s ) );

    return failed;

}

#endif
