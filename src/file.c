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

	if ( !filename )
		return NULL;

	if ( filename[0] == '"' && filename[strlen(filename)-1] == '"' )
	{
		filename[strlen(filename)-1] = '\000';
		filename++;
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
    int file_flush ( t_fileinfo *fi )
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
file_flush ( t_fileinfo *fi )
{
	t_stage *s = NULL;

	if ( !fi || !fi->stages )
		return E_FAIL;

	/* find a modified stage */
	s = stage_get_modified ( fi->stages );

	/* none found -> use the first */
	if ( !s )
        s = stage_find_by_num ( fi->stages, 0 );

	/* erh... this shouldnt happen of course :) */
	if ( !s )
		return E_FAIL;

	/* free all stages behind this one (they are non-modified) */
    if ( s->next )
	{
        stage_release_all ( s->next );
		s->next = NULL;
	}

	/* and go through all remaining from behind */
    while ( s && s->prev )
    {
		/* TODO: commented out? needed? useless? have to track the 'priv' field usage! */
//        R ( fmt_free_priv ( s->prev ) );
		R ( fmt_encode ( s, s->prev ) );
        s = s->prev;
        R ( stage_release ( s->next ) );

		/* if the flag S_SYNCBASE was set, stop here */
		if ( s->flags & S_SYNCBASE )
			s = NULL;
    }

    return E_OK;
}

/*
    int file_sync ( t_fileinfo *fi )
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
file_sync ( t_fileinfo *fi )
{
 	t_stage_info *info = NULL;

	/* get the sage information backup (flags) */
	info = stage_save_info ( fi->stages );

	/* 'commit' all changes down to the raw file (or to syncbase) first */
	R ( file_flush ( fi ) );

	/* then try to decode all again */
    R ( fmt_decode ( stage_get_last ( fi->stages ), NULL ) );

	/* restoring flags is non-critical and may safely fail if e.g. a parser was added or removed */
	stage_restore_info ( fi->stages, info );
	stage_release_info ( info );

	/* clear flag S_MODIFIED, since nothing is modified now anymore */
	R (	stage_clear_flags ( fi->stages, S_MODIFIED ) );

    return E_OK;
}

unsigned int 
file_format ( t_fileinfo *fi, char *format )
{
	t_stage *stage = NULL;
	t_stage *workstage = NULL;

    if ( !fi || !fi->stages || !format )
        return E_FAIL;

	/* first 'commit' all changes made */
	R ( file_flush ( fi ) );

	/* create an empty stage */
	stage = stage_create ( );

	/* get the *last* stage (so using S_SYNCBASE lets us select the stage to format) */
	workstage = stage_get_last ( fi->stages );

	/* this is tricky - the format string will get replaced by the parser */
	workstage->parser = format;
	
	/* set the new stage name */
	stage->name = "NOT SET";
	stage->flags &= ~FLAGS_FREE_NAME;

	/* let the encode encode the stage */
	if ( fmt_encode ( workstage, stage ) == E_FAIL )
	{
		free ( workstage->parser );
		stage_release ( stage );
		return E_FAIL;
	}

	/* release *all* stages in the current fileinfo */
	stage_release ( fi->stages );
	/* and set this as the new (and only) stage */
	fi->stages = stage;

	/* and re-parse everything */
	file_sync ( fi );

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
	file_sync ( fi );

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
		file_sync ( fi );
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
