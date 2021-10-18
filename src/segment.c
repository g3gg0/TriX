#ifndef __TRIX_SEGMENT_C__
#define __TRIX_SEGMENT_C__


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

SEGMENT_PLUG_INIT;

t_segment *
segment_duplicate ( t_segment * s )
{
	t_segment *t = NULL;

    if ( !s )
        return NULL;

	t = segment_create ();
    if ( !t )
        return NULL;

	/* sparse segments do not have any memory */
	if(!(s->flags & FLAGS_SPARSE))
	{
		t->data = malloc ( s->length );
		if ( !t->data )
		{
			CHECK_AND_FREE ( t );
			return NULL;
		}
		memcpy ( t->data, s->data, s->length );
	}
	else
	{
		t->data = NULL;
	}

	t->length = s->length;
	t->start = s->start;
	t->end = s->end;
	t->struct_magic = s->struct_magic;
	t->capture_plug = s->capture_plug;
	t->capture_plug_active = s->capture_plug_active;

	if ( s->flags & FLAGS_FREE_NAME )
		t->name = strdup ( s->name );
	else
		t->name = s->name;

	t->next = NULL;
	t->prev = NULL;
	t->flags = s->flags;
	if ( s->priv )
		s->priv->struct_refs++;
	t->priv = s->priv;

	return t;
}


t_segment *
segment_find_by_num ( t_segment * s, unsigned int num )
{
    if ( !s )
        return NULL;

	LIST_REWND(s);

    while ( s )
    {
        if ( !num-- )
            return s;
		LIST_NEXT(s);
    }

    return NULL;
}


t_segment *
segment_find_by_end_and_name ( t_segment * s, unsigned int addr, char *name, unsigned int flags )
{
    if ( !s )
        return NULL;

	LIST_REWND(s);

	while ( s )
	{
		if ( (s->flags & flags) && (s->end == addr && s->start != s->end) && (name && !strcmp ( s->name, name )) )
			return s;
		LIST_NEXT(s);
	}

    return NULL;
}

t_segment *
segment_find_by_end ( t_segment * s, unsigned int addr, unsigned int flags )
{
    if ( !s )
        return NULL;

	LIST_REWND(s);

	while ( s )
	{
		if ( (s->flags & flags) && (s->end == addr) && (s->start != s->end) )
			return s;
		LIST_NEXT(s);
	}

    return NULL;
}

t_segment *
segment_find_by_start ( t_segment * s, unsigned int addr, unsigned int flags )
{
	LIST_REWND(s);

	while ( s )
	{
		if ( (s->flags & flags) && (s->start == addr) && (s->start != (s->start + s->length)) )
			return s;
		LIST_NEXT(s);
	}

    return NULL;
}

t_segment *
segment_find_by_name ( t_segment * s, char *name, unsigned int flags )
{
    if ( !s )
        return NULL;

	LIST_REWND(s);

	while ( s )
	{
		if ( (s->flags & flags) && (name && !strcmp ( s->name, name )) )
			return s;
		LIST_NEXT(s);
	}
    return NULL;
}

unsigned int
segment_count ( t_segment * s )
{
	return LIST_COUNT(s);
}

unsigned int
segment_is_mapped ( t_segment * s )
{
    if ( !s )
        return 0;

	return (s->flags & FLAGS_MAP_IN_MEM);
}

unsigned int
segment_is_sparse ( t_segment * s )
{
    if ( !s )
        return 0;

	return (s->flags & FLAGS_SPARSE);
}

unsigned int
segment_is_shadow ( t_segment * s )
{
    if ( !s )
        return 0;

	return (s->flags & FLAGS_SHADOW);
}

void
segment_map_in_mem ( t_segment * s )
{
	if ( !s )
		return;
	s->flags &= ~FLAGS_MAP_ANY;
	s->flags |= FLAGS_MAP_IN_MEM;
}

void
segment_hide_in_mem ( t_segment * s )
{
	if ( !s )
		return;
	s->flags &= ~FLAGS_MAP_ANY;
	s->flags |= FLAGS_HIDE_IN_MEM;
}


t_segment *
segment_get_last ( t_segment * s )
{
    if ( !s )
        return NULL;

	if ( is_type ( s, "t_stage" ) )
		s = ((t_stage*)s)->segments;

	LIST_FFWD(s);

    return s;
}

t_segment *
segment_add ( t_segment * s )
{
	s = segment_get_last ( s );

    if ( !s || s->next )
        return NULL;

	s->next = segment_create ( );
    if ( !s->next )
        return NULL;

    s->next->prev = s;

    return s->next;
}

t_segment *
segment_create ( )
{
    t_segment *s = NULL;

    s = LIST_MALLOC(t_segment);
    if ( !s )
        return NULL;
	s->struct_magic = M_SEGMENT;
    s->name = "EMPTY";
    s->data = NULL;
    s->next = NULL;
    s->prev = NULL;
    s->priv = NULL;
	s->capture_plug = NULL;
	s->capture_plug_active = 0;
    s->length = 0;
    s->start = 0;
    s->end = 0;
	s->flags = 0;

	segment_map_in_mem ( s );

    return s;
}

unsigned int
segment_dump ( t_segment * s )
{
    while ( s )
    {
        TAB;
        printf ( "-----------------------------\n" );
        TAB;
        printf ( "Dump of segment <%s>\n", s->name );
        TAB;
        printf ( "-----------------------------\n" );
        TAB;
        printf ( " Name:    <%s>\n", s->name );
        TAB;
        printf ( " Start:   <0x%08X>\n", s->start );
        TAB;
        printf ( " End:     <0x%08X>\n", s->end );
        TAB;
        printf ( " Length:  <0x%08X>\n", s->length );
        TAB;
        printf ( " Pointer: <0x%08X>\n", s->data );
        s = s->next;
    }

    return E_OK;
}


/*
    release a segment and corrects previous and next pointer
*/
unsigned int
segment_release ( t_segment * s )
{
    if ( !s )
        return E_FAIL;

	if(!segment_is_sparse(s) && !segment_is_shadow(s))
	{
		CHECK_AND_FREE ( s->data );
	}

	if ( s->priv )
	{
		s->priv->struct_refs--;
		if (s->priv->struct_refs == 0 )
			CHECK_AND_FREE ( s->priv );
	}

    if ( s->prev && s->prev->next == s )
        s->prev->next = s->next;
    if ( s->next && s->next->prev == s )
        s->next->prev = s->prev;

	if ( s->flags & FLAGS_FREE_NAME )
		CHECK_AND_FREE ( s->name );

    CHECK_AND_FREE ( s );

    return E_OK;
}

/*
    release this stage and all next, corrects pointer of previous
*/
unsigned int
segment_release_all ( t_segment * s )
{
    unsigned int ret = E_OK;

    if ( !s )
        return E_FAIL;

    while ( s->next )
    {
		LIST_NEXT(s);
        if ( segment_release ( s->prev ) == E_FAIL )
            ret = E_FAIL;
    }
    R ( segment_release ( s ) );

    return ret;
}


#endif
