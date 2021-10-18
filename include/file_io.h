#ifndef __TRIX_FILE_IO_H__
#define __TRIX_FILE_IO_H__


//typedef, so we can miss the struct while programming
typedef struct s_fileinfo t_fileinfo;
struct s_fileinfo
{
	STRUCT_HEADER_LIST(t_fileinfo);
    char *filename;
    char *path;
    t_stage *stages;
    t_stage *stages_org;
    t_stage *stages_mod;
    void *priv;
    unsigned int options;
};


#include "trixplug_file_io.h"

t_fileinfo *file_io_open ( const char *path, const char *filename );
unsigned int file_io_write ( const char *filename, const t_fileinfo * fi );
unsigned int file_io_release ( t_fileinfo * fi );
t_fileinfo *file_io_create_empty (  );
unsigned int file_io_release ( t_fileinfo * fi );
unsigned int file_io_release_all ( t_fileinfo * fi );
t_fileinfo *file_io_open_directory ( char *path, unsigned int *ret_entries );
int file_io_create_directory ( char *dirname );

#endif
