#ifndef __TRIX_FMT_C__
#define __TRIX_FMT_C__

#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

#include "defines.h"
#include "segment.h"
#include "stage.h"
#include "symbols.h"

#include "file_io.h"
#include "file.h"
#include "fmt.h"
#include "workspace.h"
#include "util.h"

#include "trixplug_funcs.h"

#include "mem.h"

t_fmt_table *fmt_table = NULL;

extern struct trix_functions *ft;
struct fmt_funcs fmt_functions;

FMT_PLUG_INIT;

/*
    t_fmt_handler *fmt_get_handler ( char *name )
   -----------------------------------------------------

    Internal:
		Yes

    Description:
        Get the format handler by its name

    Return value:
        returns handler on success NULL on error
*/
t_fmt_handler *
fmt_get_handler ( char *name )
{
    t_fmt_table *entry = fmt_table; // get the format parser table

    if ( !name )
        return NULL;

    while ( entry )             // scan for a format parser called *name
    {
        if ( !strcmp ( entry->handler->name, name ) )
            return entry->handler;  // found, return the handler routines struct
        entry = entry->next;    // else next one
    }
    return NULL;
}


/*
    int fmt_decode ( t_stage * s, char *type )
   -----------------------------------------------------

    Internal:
		Yes

    Description:
        Starts the decoding procedure starting from stage *s.

    Return value:
        returns E_OK on success E_FAIL on error
*/
unsigned int
fmt_decode ( t_stage * s, char *type )
{
    t_fmt_table *entry = NULL;
    t_stage *d = NULL;
    unsigned int next = 1;

    while ( next )
    {
        entry = fmt_table;      // get the format parser table
        d = stage_add ( s );	// add a blank stage
        if ( !d )
            return E_FAIL;

        next = 0;

        while ( entry && !next )    // run through all parsers
        { 
			// if we are forced to run just one type, check that
            if ( !type || !strcmp ( entry->handler->type, type ) )
            {
                if ( entry->handler->decode ( s, d ) == E_OK )
                {
                    s = d;
                    next = 1;
                }
            }
            entry = entry->next;    // else next one
        }
    }

    R ( stage_release ( d ) );

    return E_OK;
}

/*
    int fmt_encode ( t_stage * s, t_stage * d )
   -----------------------------------------------------

    Internal:
		Yes

    Description:
        Encodes stage *s into stage *d

    Return value:
        returns E_OK on success E_FAIL on error
*/
unsigned int
fmt_encode ( t_stage * s, t_stage * d )
{
    t_fmt_handler *h = NULL;

    if ( !s )
        return E_FAIL;

    h = fmt_get_handler ( s->parser );
    if ( !h )
        return E_FAIL;

	if ( h->encode )
		R ( h->encode ( s, d ) );

    return E_OK;
}

/*
    int fmt_encode_diff ( t_stage * s1, t_stage * s2, t_stage * d )
   -----------------------------------------------------

    Internal:
		Yes

    Description:
        Encodes differences between stage *s1 and s2 into stage *d

    Return value:
        returns E_OK on success E_FAIL on error
*/
unsigned int
fmt_encode_diff ( t_stage * s1, t_stage * s2, t_stage * d )
{
    t_fmt_handler *h = NULL;

    if ( !s1 || !s2 )
        return E_FAIL;

    h = fmt_get_handler ( s1->parser );
    if ( !h )
        return E_FAIL;

	if ( h->encode_diff )
	{
		R ( h->encode_diff ( s1, s2, d ) );
	}
	else
		return E_FAIL;

    return E_OK;
}

/*
    int fmt_free_priv ( t_stage * s )
   -----------------------------------------------------

    Internal:
		Yes

    Description:
        Free up the private data.

    Return value:
        returns E_OK on success E_FAIL on error
*/
unsigned int
fmt_free_priv ( t_stage * s )
{
    t_fmt_handler *h = NULL;

    if ( !s )
        return E_OK;

    h = fmt_get_handler ( s->parser );
    if ( !h )
	{
		/* special case... file_io_open has no handler but data needs to be freed */
		if ( s->segments )
			segment_release_all ( s->segments );
		s->segments = NULL;
        return E_OK;
	}

    if ( h->free_priv )
        R ( h->free_priv ( s ) );

    return E_OK;
}

/*
    int fmt_register_handler ( t_fmt_handler * handler )
   -----------------------------------------------------

    Internal:
		Yes

    Description:
        Register a handler.

    Return value:
        returns E_OK on success E_FAIL on error
*/
unsigned int
fmt_register_handler ( t_fmt_handler * handler )
{
    t_fmt_table *entry;

    if ( !handler || !handler->name )
        return E_FAIL;

    if ( fmt_get_handler ( handler->name ) )    // if it already exists
        return E_FAIL;

	entry = LIST_NEW(fmt_table, t_fmt_table);
    entry->handler = handler;
    entry->struct_magic = M_FORMAT;

	if ( !fmt_table )
        fmt_table = entry;

	fmt_update ( );

	return E_OK;
}

/*
    int fmt_add_seer_entry ( char *n, void *f, char **i, int p )
   -----------------------------------------------------

    Internal:
		Yes

    Description:
        Updates the import list at *n and registers the symbol
		of the given routine.

    Return value:
        returns E_OK on success E_FAIL on error
*/
unsigned int
fmt_add_seer_entry ( char *n, void *f, char **i, int p )
{
	char *imports = *i;
	char *template[] = { 
		"import int __%s ( );\n",
		"import int __%s ( t_stage * );\n",
		"import int __%s ( t_stage *, t_stage * );\n" };
	static char *buffer = NULL;
	static unsigned int buffer_length = 0;

	if ( !n || !f || !i || p > 2 )
		return E_FAIL;

	if ( !buffer )
	{
		buffer = malloc ( 1 );
		buffer[0] = '\000';
	}

	if ( !imports )
	{
		*i = malloc ( 1 );
		(*i)[0] = '\000';
		imports = *i;
	}

	if ( strlen ( n ) + strlen ( template[p] ) > buffer_length )
	{
		buffer_length = strlen ( n ) + strlen ( template[p] );
		buffer = realloc ( buffer, buffer_length );
	}
	sprintf ( buffer, template[p], n );
	imports = realloc ( imports, strlen ( imports ) + strlen ( buffer ) + 1 );
	*i = imports;
	strcat ( imports, buffer );

	scAdd_External_Symbol_Int ( n, f );

	return E_OK;
}

/*
    int fmt_add_seer ( t_fmt_handler *h, char **i )
   -----------------------------------------------------

    Internal:
		Yes

    Description:
        Registers the given handler in seer subsystem.

    Return value:
        returns E_OK on success E_FAIL on error
*/
unsigned int
fmt_add_seer ( t_fmt_handler *h, char **i )
{
	if ( !h || !i )
		return E_FAIL;

	if ( h->decode && h->decode_name )
		R(fmt_add_seer_entry ( h->decode_name, h->decode, i, 2 ));
	if ( h->encode && h->encode_name )
		R(fmt_add_seer_entry ( h->encode_name, h->encode, i, 2 ));
	if ( h->free_priv && h->free_priv_name )
		R(fmt_add_seer_entry ( h->free_priv_name, h->free_priv, i, 1 ));

	return E_OK;
}

/*
int fmt_plug_init (  )
{
	if ( !ft )
		return E_FAIL;
	ft->fmt = &fmt_functions;
	ft->fmt->decode = fmt_decode;
	ft->fmt->encode = fmt_encode;
	ft->fmt->get_handler = fmt_get_handler;
	ft->fmt->fregister = fmt_register_handler;
	ft->fmt->update = fmt_update;

	return E_OK;
}
*/

/* Update the SEER entries */
unsigned int fmt_update (  ) 
{
    t_fmt_table *entry = NULL;
	static char *imports = NULL;

	if ( imports )
		strcpy ( imports, "" );

	entry = fmt_table;
	while ( entry && entry->handler )
	{
		fmt_add_seer ( entry->handler, &imports );
		entry = entry->next;
	}
	scAdd_Internal_Header ( "fmt_parsers", imports );

    return E_OK;
}

/*
    int fmt_init (  )
   -----------------------------------------------------

    Internal:
		Yes

    Description:
        Initialize format subsystem.

    Return value:
        returns E_OK on success E_FAIL on error
*/
unsigned int
fmt_init (  )
{
	// call the "hardcoded" format handlers
	fmt_elf_init (  );
	fmt_bz2_init (  );
	fmt_epoc_init (  );
    fmt_dct3flash_init (  );
    fmt_wintesla_init (  );
    fmt_phoenix_init (  );
    fmt_dct4crypt_init (  );
	fmt_trixcrypt_init (  );
	fmt_pdb_init (  );
	
	return E_OK;
}

#endif
