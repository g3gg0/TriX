#ifndef __TRIX_NETRIX_C__
#define __TRIX_NETRIX_C__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defines.h"
#include "segment.h"
#include "stage.h"
#include "symbols.h"

#include "file_io.h"
#include "file.h"
#include "seer.h"
#include "workspace.h"
#include "util.h"
#include "md5.h"
#include "http_lib.h"
#include "netrix.h"

#include "trixplug_funcs.h"
#include "mem.h"

NETRIX_PLUG_INIT;

unsigned int
netrix_check ( char *url )
{
	if ( http_parse_url ( url, NULL ) != E_OK )
		return E_FAIL;
	return E_OK;
}


t_fileinfo *
netrix_open ( const char *url )
{
	unsigned int length = 0;
	char *file = NULL;
	char *data = NULL;
    FILE *file_in = NULL;
    t_fileinfo *fi = NULL;


	if ( http_parse_url ( url, &file ) != E_OK )
		return NULL;

	fi = file_io_create_empty ();

	fi->path = strdup ( url );

	if ( file )
        fi->filename = strdup ( file );
    else
        fi->filename = NULL;

	if ( http_get ( file, &data, &length, NULL ) != E_OK )
	{
		free ( fi );
		return NULL;
	}

    fi->stages->length = length;

    fi->stages->name = "RAW";
    fi->stages->type = "RAW";
    fi->stages->parser = "NETRIX";
    fi->stages->flags = 0;
    fi->stages->segments->name = "DATA";
    fi->stages->segments->start = 0;
    fi->stages->segments->end = fi->stages->length;
    fi->stages->segments->length = fi->stages->length;
    fi->stages->segments->data = realloc ( data, length+1 );


	// in case of text files, make sure it will end with an NULL byte
	fi->stages->segments->data[fi->stages->length] = '\000';

    return fi;
}



unsigned int
netrix_execute ( char *url )
{
	char *data = NULL;
	char *path = NULL;
	unsigned int length = 0;
	unsigned int ret = E_OK;


	printf ( "\n[NeTriX] Retrieving '%s'...", url );

	if ( http_parse_url ( url, &path ) != E_OK )
	{
		printf  ( "   [FAILED]\n" );
		return E_FAIL;
	}

	if ( http_get ( path, &data, &length, NULL ) != E_OK )
	{
		printf  ( "   [FAILED]\n" );
		return E_FAIL;
	}

	printf  ( "   [OK]\n" );

	ret = seer_run ( data );
	free ( data );

	return ret;
}



char *
netrix_get ( char *url )
{
	char *data = NULL;
	char *path = NULL;
	unsigned int length = 0;
	unsigned int ret = E_OK;

	printf ( "\n[NeTriX] Retrieving '%s'...", url );

	if (   ( http_parse_url ( url, &path ) != E_OK ) 
		|| ( http_get ( path, &data, &length, NULL ) != E_OK ) )
	{
		printf  ( "   [FAILED]\n" );
		return NULL;
	}

	printf  ( "   [OK]\n" );

	return data;
}


#endif
