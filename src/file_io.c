#ifndef __TRIX_FILE_IO_C__
#define __TRIX_FILE_IO_C__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "defines.h"
#include "segment.h"
#include "stage.h"
#include "symbols.h"

#include "file_io.h"
#include "file.h"

#include "trixplug_funcs.h"
#include "mem.h"

#ifdef WIN32
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#endif

FILE_IO_PLUG_INIT;
t_fileinfo *
file_io_create_empty (  )
{
    t_fileinfo *fi = NULL;

    fi = ( t_fileinfo * ) calloc ( 1, sizeof ( t_fileinfo ) );  //calloc else file_io_release will fail when free'ing before prev set
    if ( !fi )
        return NULL;


    fi->filename = strdup ( "(none)" );
    fi->path = NULL;


    fi->stages = stage_create (  );
    fi->stages->segments = segment_create (  );

    fi->stages->name = "RAW";
    fi->stages->type = "RAW";
    fi->stages->parser = "FILE_IO";
    fi->stages->flags = 0;
    fi->stages->segments->name = "DATA";
    fi->stages->segments->start = 0;
    fi->stages->segments->end = 0;
    fi->stages->segments->length = 0;
    fi->stages->segments->data = NULL;
	fi->prev = NULL;
    fi->next = NULL;
    fi->priv = NULL;
    fi->stages_org = NULL;
    fi->stages_mod = NULL;
    fi->options = file_get_options();

    return fi;

}

/*
     int file_io_create_directory ( char *dirname )
   -----------------------------------------------------

    Internal:
		Yes

    Description:
        Creates the specified directory

    Return value:
        returns E_OK on success E_FAIL on error
*/
int
file_io_create_directory ( char *dirname )
{
#ifdef WIN32
	if(!CreateDirectory(dirname, NULL))
	{
		return E_FAIL;
	}

	return E_OK;
#else
	/* TODO */
	return E_FAIL;
#endif
}

/*
     t_fileinfo *file_io_open_directory ( char *filename, unsigned int *entries )
   -----------------------------------------------------

    Internal:
		Yes

    Description:
        Opens a directory and generates a list of files (but doesn't load them)

    Return value:
        returns a array of t_fileinfo on success, NULL on error.
		User has to manually free all filename struct members of the returned
		struct array. User has to free the returned pointer too.
*/

t_fileinfo *
file_io_open_directory ( char *path, unsigned int *ret_entries )
{
#ifdef WIN32
	int done = 0;
	int entries = 0;
	int entries_allocated = 32;
	int entries_blocksize = 32;
	t_fileinfo *file_infos = NULL;
	WIN32_FIND_DATA file_data;
	HANDLE hFind;

	hFind = FindFirstFile(path, &file_data);
	if (hFind == INVALID_HANDLE_VALUE)
	{	
		*ret_entries = 0;
		return NULL;
	}

	// allocate first block
	entries_allocated += entries_blocksize;
	file_infos = malloc ( entries_allocated * sizeof(t_fileinfo) );

	while (!done)
	{
		if ( strcmp ( file_data.cFileName, "." ) && strcmp ( file_data.cFileName, ".." ) )
		{
			// not enough space, realloc block
			if(entries >= entries_allocated)
			{
				entries_allocated += entries_blocksize;
				file_infos = realloc ( file_infos, entries_allocated * sizeof(t_fileinfo) );

				if(!file_infos)
				{
					entries = 0;
					ERR(0, "Failed to realloc 'file_infos'");
					break;
				}
			}

			memset ( &file_infos[entries], 0x00, sizeof(t_fileinfo) );
			util_smalloc_init ( &file_infos[entries], sizeof(t_fileinfo), "t_fileinfo");
			file_infos[entries].filename = strdup ( file_data.cFileName );

			if ( file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
			{
				file_infos[entries].options = FILE_DIRECTORY;
			}
			else
			{
				file_infos[entries].options = FILE_NORMAL;
			}
			entries++;
		}


		if (!FindNextFile(hFind, &file_data))
		{
			done = 1;
			/*
			if (GetLastError() == ERROR_NO_MORE_FILES)
			{
				done = 1;
			}
			else
			{
				done = 1;
			}
			*/
		}
	}

	if ( !entries )
	{
		CHECK_AND_FREE(file_infos);
	}

	FindClose(hFind);

	*ret_entries = entries;
	return file_infos;
#else
	return NULL;
#endif
}


/*
    t_fileinfo *fi file_io_open ( const char *path, const char *filename )
   -----------------------------------------------------

    Internal:
		Yes

	Description:
        Opens a file and fills it into the fileinfo.
        Does not keep open any handlers.

    Return value:
        returns fileinfo on success, NULL on error
*/

t_fileinfo *
file_io_open ( const char *path, const char *filename )
{
    char *tmp_c = NULL;
    FILE *file_in = NULL;
    t_fileinfo *fi = NULL;

    // hmmm then without const in signature?
    if ( !filename )
    {
        filename = path;
        path = NULL;
    }
    if ( !filename )
        return NULL;


    fi = ( t_fileinfo * ) calloc ( 1, sizeof ( t_fileinfo ) );  //calloc else file_io_release will fail when free'ing before prev set
    if ( !fi )
        return NULL;


    fi->filename = strdup ( filename );

    if ( path )
        fi->path = strdup ( path );
    else
        fi->path = NULL;


    fi->stages = stage_create (  );
    fi->stages->segments = segment_create (  );

    if ( path )
    {
        tmp_c = ( char * ) malloc ( strlen ( path ) + strlen ( filename ) + 2 );
        sprintf ( tmp_c, "%s%s%s", path, path[strlen ( path ) - 1] == DIR_SEP ? "" : DIR_SEP_STR, filename );
    }
    else
        tmp_c = strdup ( filename );

    file_in = fopen ( tmp_c, "rb" );
    free ( tmp_c );

    if ( !file_in )
    {
        file_io_release ( fi );
        return NULL;
    }


    fseek ( file_in, 0, SEEK_END );
    fi->stages->length = ftell ( file_in );
    fseek ( file_in, 0, SEEK_SET );

    fi->stages->name = "RAW";
    fi->stages->type = "RAW";
    fi->stages->parser = "FILE_IO";
    fi->stages->flags = 0;
    fi->stages->segments->name = "DATA";
    fi->stages->segments->start = 0;
    fi->stages->segments->end = fi->stages->length;
    fi->stages->segments->length = fi->stages->length;
    fi->stages->segments->data = ( char * ) malloc ( fi->stages->length + 1 );
	fi->prev = NULL;
    fi->next = NULL;
    fi->priv = NULL;
    fi->stages_org = NULL;
    fi->stages_mod = NULL;
    fi->options = file_get_options ();

    if ( fi->stages->length > 0 && fread ( fi->stages->segments->data, fi->stages->length, 1, file_in ) != 1 )
    {
        file_io_release ( fi );
        return NULL;
    }
	
    fclose ( file_in );

	// in case of text files, make sure it will end with an NULL byte
	fi->stages->segments->data[fi->stages->length] = '\000';

    return fi;

}




/*
    int file_io_write ( const char *filename, const t_fileinfo *fi )
   -----------------------------------------------------

    Internal:
		Yes

    Description:
        Writes the contents of specified stage to a file, using data in fileinfo.
	
    Return value:
        returns E_OK on success E_FAIL on error
*/
//maybe also add a path ?
unsigned int
file_io_write ( const char *filename, const t_fileinfo * fi )
{
	unsigned int stage = 0;
    t_stage *s;
    FILE *file_out;

    if ( !filename || !fi )
        return E_FAIL;


    //TODO:
    // think about how to output stages not in RAW format
    //  just concat the segments ?

	s = stage_get_savestage ( fi->stages );
	if ( !s )
		s = fi->stages;

	// empty files should be valid
	if ( !s || (segment_count (s->segments) != 1) || !s->segments->data /*|| (s->segments->length < 1) */ )
        return E_FAIL;

	/*
    if ( strcmp ( s->name, "RAW" ) )
        return E_FAIL;          //until it's supported
    if ( strcmp ( s->segments->name, "DATA" ) )
        return E_FAIL;          //until supported
	*/

	if ( !(fi->flags & FILE_DRYMODE) )
	{
		file_out = fopen ( filename, "wb" );
		if ( !file_out )
			return E_FAIL;

		if ( fwrite ( s->segments->data, s->segments->length, 1, file_out ) != 1 )
		{
			fclose ( file_out );
			return E_FAIL;
		}

		fclose ( file_out );
	}
    return E_OK;
}



/*
    int file_io_release ( t_fileinfo *fi )
   -----------------------------------------------------

    Internal:
		Yes

    Description:
        Free's up the allocated buffers in the fileinfo.

    Return value:
        returns E_OK on success E_FAIL on error
*/
unsigned int
file_io_release ( t_fileinfo * fi )
{

    if ( !fi )
        return E_FAIL;

    CHECK_AND_FREE ( fi->filename );
    CHECK_AND_FREE ( fi->path );

    if ( fi->prev && fi->prev->next == fi )
        fi->prev->next = fi->next;
    if ( fi->next && fi->next->prev == fi )
        fi->next->prev = fi->prev;

    if ( fi->stages )
        R ( stage_release_all ( fi->stages ) );
    if ( fi->stages_org )
        R ( stage_release_all ( fi->stages_org ) );
    if ( fi->stages_mod )
        R ( stage_release_all ( fi->stages_mod ) );

    free ( fi );

    return E_OK;
}

unsigned int
file_io_release_all ( t_fileinfo * fi )
{
	if ( !fi )
		return E_FAIL;

	while ( fi->next )
	{
		fi = fi->next;
		file_io_release ( fi->prev );
	}
	file_io_release ( fi );

    return E_OK;
}





#endif
