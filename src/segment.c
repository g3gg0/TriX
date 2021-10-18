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

	t->data = malloc ( s->length );
	if ( !t->data )
	{
		free ( t );
		return NULL;
	}
	memcpy ( t->data, s->data, s->length );
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


t_segment *
segment_find_by_end_and_name ( t_segment * s, unsigned int addr, char *name )
{
    while ( s )
    {
        if ( (s->end == addr && s->start != s->end) && (!strcmp ( s->name, name )) )
            return s;
        s = s->next;
    }
    return NULL;
}

t_segment *
segment_find_by_end ( t_segment * s, unsigned int addr )
{
    while ( s )
    {
        if ( s->end == addr && s->start != s->end )
            return s;
        s = s->next;
    }
    return NULL;
}

t_segment *
segment_find_by_start ( t_segment * s, unsigned int addr )
{
    while ( s )
    {
        if ( s->start == addr && s->start != s->end )
            return s;
        s = s->next;
    }
    return NULL;
}

t_segment *
segment_find_by_name ( t_segment * s, char *name )
{
    while ( s )
    {
        if ( !strcmp ( s->name, name ) )
            return s;
        s = s->next;
    }
    return NULL;
}

unsigned int
segment_count ( t_segment * s )
{
	return LIST_COUNT(s);
}

t_segment *
segment_get_last ( t_segment * s )
{
    if ( !s )
        return NULL;

	if ( is_type ( s, "t_stage" ) )
		s = ((t_stage*)s)->segments;

    while ( s && s->next )
        s = s->next;

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

    CHECK_AND_FREE ( s->data );
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
    unsigned int failed = E_OK;

    if ( !s )
        return E_FAIL;

    while ( s->next )
    {
        s = s->next;
        if ( segment_release ( s->prev ) == E_FAIL )
            failed = E_FAIL;
    }
    R ( segment_release ( s ) );

    return failed;
}


#endif
