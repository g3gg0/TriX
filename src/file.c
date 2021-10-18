#ifndef __TRIX_FILE_C__
#define __TRIX_FILE_C__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "defines.h"
#include "segment.h"
#include "stage.h"
#include "symbols.h"

#include "file_io.h"
#include "file.h"
#include "crypto.h"
#include "netrix.h"

#include "trixplug_funcs.h"
#include "mem.h"


unsigned int file_options = FILE_NORMAL;

FILE_PLUG_INIT;

unsigned int file_set_options ( unsigned int options )
{
	int old_options = file_options;

	file_options = options;

	return old_options;
}

unsigned int file_get_options (  )
{
	return file_options;
}

/*
     t_fileinfo *file_open ( const char *filename )
   -----------------------------------------------------

    Internal:
		Yes

    Description:
        Opens a file and fills it into the fileinfo.
		Also runs through the format parsers to unpack/decode
		the file.	

    Return value:
        returns valid t_fileinfo on success, NULL  on error
*/

t_fileinfo *
file_open ( char *filename )
{
	char *url_path = NULL;
    t_fileinfo *f = NULL;

	if ( filename )
	{
		if ( filename[0] == '"' &&  filename[strlen(filename)-1] == '"' )
		{
			filename[strlen(filename)-1] = '\000';
			filename++;
		}
	}

	if ( netrix_check ( filename ) == E_OK )
	{	
		f = netrix_open ( filename );
		if ( !f )
			return NULL;
	}
	else
	{
		if ( !strncmp ( filename, "file:///", 8 ) )
			f = file_io_open ( NULL, filename + 8 );
		else
			f = file_io_open ( NULL, filename );
		if ( !f )
			return NULL;
	}

    if ( fmt_decode ( f->stages, NULL ) != E_OK )
    {
        file_release ( f );
        return NULL;
    }

    return f;
}

/*
    int file_flush_diff ( t_stage * s, t_stage * d )
   -----------------------------------------------------

    Internal:
		Yes

    Description:
        Clear unchanged stages, crunch down to the fist 
		stage. Tries to do differential saving (e.g. partial)

    Return value:
        returns E_OK on success E_FAIL on error
*/
unsigned int
file_flush_diff ( t_stage * s, t_stage * d )
{

	if ( !s || !d || (stage_count ( s ) != stage_count ( d )) )
		return E_FAIL;

	LIST_FFWD ( s );
	LIST_FFWD ( d );

    while ( s && s->prev )
    {
		if ( fmt_encode_diff ( s, d, s->prev ) != E_OK )
		{
			fmt_encode ( s, s->prev );
			fmt_encode ( d, d->prev );
		}
        s = s->prev;
        d = d->prev;
        R ( stage_release ( s->next ) );
        R ( stage_release ( d->next ) );
    }

    return E_OK;
}

/*
    int file_flush ( t_stage * s )
   -----------------------------------------------------

    Internal:
		Yes

    Description:
        Clear unchanged stages, crunch down to the fist 
		stage.

    Return value:
        returns E_OK on success E_FAIL on error
*/
unsigned int
file_flush ( t_stage * s )
{
	if ( !s )
		return E_FAIL;

	if ( !stage_get_modified ( s ) )
        s = stage_find_by_num ( s, 0 );
    else
        s = stage_get_modified ( s );

	if ( !s )
		return E_FAIL;

    if ( s->next )
        stage_release_all ( s->next );
	s->next = NULL;

    while ( s && s->prev )
    {
//        R ( fmt_free_priv ( s->prev ) );
		R ( fmt_encode ( s, s->prev ) );
        s = s->prev;
        R ( stage_release ( s->next ) );

		// if the flag S_SAVEBASE was set, 
		// prevent any further sync operation.
		if ( s->flags & S_SYNCBASE )
			s = NULL;
    }

    return E_OK;
}

/*
    int file_sync ( t_stage * s )
   -----------------------------------------------------

    Internal:
		Yes

    Description:
        Clear unchanged stages, crunch down to the fist 
		stage and re-parse all stages again.

    Return value:
        returns E_OK on success E_FAIL on error
*/
unsigned int
file_sync ( t_stage * s )
{
	t_stage_info *info = NULL;

	info = stage_save_info ( s );

	R ( file_flush ( s ) );
    R ( fmt_decode ( stage_get_last ( s ), NULL ) );

	R ( stage_restore_info ( s, info ) );
	R (	stage_clear_flags ( s, S_MODIFIED ) );

    return E_OK;
}

unsigned int 
file_format ( t_fileinfo *fi, char *format )
{
	t_stage *stage = NULL;

    if ( !fi || !fi->stages || !format )
        return E_FAIL;

	R ( file_sync ( fi->stages ) );

	stage = stage_create ( );

	fi->stages->parser = strdup (format);
	
	stage->name = "NOT SET";
	stage->flags &= ~FLAGS_FREE_NAME;

	if ( fmt_encode ( fi->stages, stage ) == E_FAIL )
	{
		stage_release ( stage );
		return E_FAIL;
	}

	stage_release ( fi->stages );
	fi->stages = stage;
	file_sync ( fi->stages );

	return E_OK;
}

/*
    int file_write ( char *filename, t_fileinfo *fi )
   -----------------------------------------------------

    Internal:
		Yes

    Description:
        Writes the contents of specified stage to a file, using data in fileinfo.
		Per default stage 0 is written when no stage was set as S_SAVESTAGE.
	
    Return value:
        returns E_OK on success E_FAIL on error
*/
unsigned int
file_write ( char *filename, t_fileinfo * fi )
{
	unsigned int ret = 0;
	t_stage *modified = NULL;
	t_stage *savestage = NULL;

    if ( !fi || !fi->stages )
        return E_FAIL;

	modified = stage_get_modified ( fi->stages );
	savestage = stage_get_savestage ( fi->stages );

	// if both flags set somewhere
	if ( modified && savestage )
	{
		// check if we should save a dirty (unsynced) stage
		if ( stage_get_num ( modified ) > stage_get_num ( savestage ) )
		{
			printf ( "%s: ERROR:  (savestage < modified)  would save unsynced data!\n", __FUNCTION__ );
			return E_FAIL;
		}
	}

	/* 
		in case of differential saving, save the original content
		so we know what has changed. its enough to save the RAW stage.
		only has to be done once.
	*/
	if ( !fi->stages_org )
		fi->stages_org = stage_duplicate ( fi->stages );

	/*
		in any case, first sync the changes
	*/
	file_sync ( fi->stages );

	/*
		then save the modified version, since we now create a diff
	*/
	if ( fi->options & FILE_DIFF )
	{
		// save modified version
		fi->stages_mod = stage_duplicate ( fi->stages );
		// decode all stages in the "original" tree
		R ( fmt_decode ( stage_get_last ( fi->stages_org ), NULL ) );
		// create a diff stage from it
		R ( file_flush_diff ( fi->stages, fi->stages_org ) );
		// normally dont need this.. just for testing
		R ( fmt_decode ( stage_get_last ( fi->stages ), NULL ) );
	}

	/*
		save the modified/diff file
	*/
	ret = file_io_write ( filename, fi );

	/*
		if it was differential saving, we have just the differences
		in our normal stages. purge them and resync from normal modified version.
	*/
	if ( fi->options & FILE_DIFF )
	{
		// clear the diff stuff from fi->stages
		stage_release_all ( fi->stages );
		// get the modified version again
		fi->stages = stage_duplicate ( fi->stages_mod );
		// free up the "modified version scratchpad"
		stage_release_all ( fi->stages_mod );
		fi->stages_mod = NULL;
		// and rebuild the stages
		file_sync ( fi->stages );
		// stages_org is kept saved
	}

	return ret;
}



/*
    int file_release ( t_fileinfo *fi )
   -----------------------------------------------------

    Internal:
		Yes

    Description:
        Free's up the allocated buffers in the fileinfo.

    Return value:
        returns E_OK on success E_FAIL on error
*/
unsigned int
file_release ( t_fileinfo * fi )
{
    return file_io_release ( fi );
}


/*
    int file_release_all ( t_fileinfo *fi )
   -----------------------------------------------------

    Internal:
		Yes

    Description:
        Free's up the allocated buffers in the fileinfo
		and its children.

    Return value:
        returns E_OK on success E_FAIL on error
*/
unsigned int
file_release_all ( t_fileinfo * fi )
{
    return file_io_release_all ( fi );
}


#endif
