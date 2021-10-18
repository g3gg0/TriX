#ifndef __TRIX_SYMBOLS_C__
#define __TRIX_SYMBOLS_C__

#include "defines.h"
#include "seer.h"
#include "segment.h"
#include "stage.h"
#include "symbols.h"

#include <stdlib.h>
#include <string.h>

#include "trixplug_funcs.h"
#include "mem.h"


SYMBOLS_PLUG_INIT;

t_symbols *symbol_first = NULL;

/*
        TODO: work together with workspace
            create destination parsing funcs in symbols
               for 0xHEX, decimal, .text, .data, .bss + (offset)values
*/

void
symbols_dump ( t_stage *stage )
{
    t_symbols *tmp = symbol_first;

    printf ( ":: symbols::dump       ::\n" );
    while ( tmp )
    {
		printf ( ":    data: %s, type: %d, name: %s\n", tmp->data, tmp->type, tmp->name );
        tmp = tmp->next;
    }
    printf ( ":: end of symbols dump ::\n" );
}

/* add a symbol to the var pool */
unsigned int
symbols_add ( t_stage *stage, const char *name, const int type, const char *data )
{
    t_symbols *new_sym = NULL;

    if ( !name || !data )
        return E_FAIL;

    // find place for our new symbol
    if ( !symbol_first )
    {
        symbol_first = new_sym = (t_symbols *) malloc ( sizeof ( t_symbols ) );
        new_sym->prev = NULL;
        new_sym->next = NULL;
    }
    else
        if ( (new_sym = symbols_get ( stage, name )) )    //symbol already exists - overwritting
        {
            free ( new_sym->name );
            free ( new_sym->data );
        }
        else
        {
            new_sym = symbol_first;
            while ( new_sym->next )
                new_sym = new_sym->next;
            new_sym->next = (t_symbols *) malloc ( sizeof ( t_symbols ) );
            new_sym->next->prev = new_sym;
            new_sym = new_sym->next;

            new_sym->next = NULL;
        }

    new_sym->name = (char*)strdup ( name );
    new_sym->type = type;
    new_sym->data = (char*)strdup ( data );



    /*  TODO:
            set magic, size and refs
    */

    return E_FAIL;
}

unsigned int
symbols_remove ( t_stage *stage, const char *name )
{
    t_symbols *tmp_sym = symbol_first;

    if ( !name )
        return E_FAIL;

    while ( tmp_sym )
    {
        if ( !strcmp ( name, tmp_sym->name ) )
        {
            free ( tmp_sym->name );
            free ( tmp_sym->data );

            if ( tmp_sym->prev )
                tmp_sym->prev->next = tmp_sym->next;

            if ( tmp_sym->next )
                tmp_sym->next->prev = tmp_sym->prev;

            free ( tmp_sym );
            tmp_sym = NULL;
            return E_OK;
        }
        tmp_sym = tmp_sym->next;
    }

    return E_FAIL;
}

unsigned int
symbols_free_all ( t_stage *stage  )
{
    //t_symbols *tmp = NULL;

    /*if ( !symbol_first )
        return E_OK;*/

    while ( symbol_first )
    {
        if ( symbol_first->next )
        {
            symbol_first = symbol_first->next;
            free ( symbol_first->prev->name );
            free ( symbol_first->prev->data );
            free ( symbol_first->prev );
        }
        else
        {
            free ( symbol_first->name );
            free ( symbol_first->data );
            free ( symbol_first );
            symbol_first = NULL;
        }
    }

    return E_OK;
}

t_symbols *
symbols_get ( t_stage *stage, const char *name )
{
    t_symbols *tmp = symbol_first;

    if ( !name )
        return tmp;

    while ( tmp )
    {
        if ( !strcmp ( name, tmp->name ) )
            return tmp;
		LIST_NEXT(tmp);
    }


    return NULL;
}

unsigned int
symbols_get_address ( t_stage *stage, const char *name )
{
    t_symbols *tmp = symbol_first;
	unsigned int address;

    if ( !name )
        return E_FAIL;

    while ( tmp )
    {
        if ( !strcmp ( name, tmp->name ) )		
            return util_str2int ( tmp->data );

		LIST_NEXT(tmp);
    }


    return E_FAIL;
}

unsigned int
symbols_count ( t_stage *stage, const int type )
{
    int count = 0;
    t_symbols *tmp = symbol_first;

    while ( tmp )
    {
        if ( tmp->type == type )
            ++count;
        tmp = tmp->next;
    }

    return count;
}


#endif

