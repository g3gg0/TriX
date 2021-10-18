
#ifndef __TRIX_TRIXPLUG_FILE_IO_H__
#define __TRIX_TRIXPLUG_FILE_IO_H__

/* trixplug struct */

struct file_io_funcs
{
	t_fileinfo *(*create_empty) ( );
	t_fileinfo *(*open) ( const char *path, const char *filename);
	unsigned int (*release) ( t_fileinfo * fi);
	unsigned int (*release_all) ( t_fileinfo * fi);
	unsigned int (*write) ( const char *filename, const t_fileinfo * fi);
 	int (*create_directory) ( char *dirname);
 	t_fileinfo *(*open_directory) ( char *path, unsigned int *ret_entries);
};

/* trixplug struct initializer */

#define FILE_IO_PLUG_INIT \
extern struct trix_functions *ft;\
struct file_io_funcs file_io_functions;\
unsigned int file_io_plug_init ( ) \
{\
	if ( !ft )\
		return E_FAIL;\
	ft->file_io = &file_io_functions;\
	ft->file_io->create_empty = file_io_create_empty;\
	ft->file_io->create_directory = file_io_create_directory;\
	ft->file_io->open_directory = file_io_open_directory;\
	ft->file_io->open = file_io_open;\
	ft->file_io->write = file_io_write;\
	ft->file_io->release = file_io_release;\
	ft->file_io->release_all = file_io_release_all;\
\
	return E_OK;\
}

#endif
