#ifndef __TRIX_FILE_H__
#define __TRIX_FILE_H__

#define FILE_NORMAL    0x00
#define FILE_DIFF      0x01
#define FILE_DRYMODE   0x02
#define FILE_DIRECTORY 0x04

#include "trixplug_file.h"

t_fileinfo *file_open ( char *filename );
unsigned int file_write ( char *filename, t_fileinfo * fi );
unsigned int file_release ( t_fileinfo * fi );
unsigned int file_flush ( t_fileinfo *fi );
unsigned int file_sync ( t_fileinfo *fi );
unsigned int file_format ( t_fileinfo *fi, char *format );
unsigned int file_set_options ( unsigned int options );
unsigned int file_get_options (  );
unsigned int file_flush_diff ( t_stage * s, t_stage * d );
unsigned int file_release_all ( t_fileinfo * fi );

#endif
