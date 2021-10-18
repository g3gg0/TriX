
#ifndef __TRIX_TRIXPLUG_FILE_H__
#define __TRIX_TRIXPLUG_FILE_H__

/* trixplug struct */

struct file_funcs
{
	t_fileinfo *(*open) ( char *filename);
	unsigned int (*flush) ( t_stage * s);
	unsigned int (*flush_diff) ( t_stage * s, t_stage * d);
	unsigned int (*format) ( t_fileinfo * fi, char *format);
	unsigned int (*get_options) ( );
	unsigned int (*release) ( t_fileinfo * fi);
	unsigned int (*release_all) ( t_fileinfo * fi);
	unsigned int (*set_options) ( unsigned int options);
	unsigned int (*sync) ( t_stage * s);
	unsigned int (*write) ( char *filename, t_fileinfo * fi);
};

/* trixplug struct initializer */

#define FILE_PLUG_INIT \
extern struct trix_functions *ft;\
struct file_funcs file_functions;\
unsigned int file_plug_init ( ) \
{\
	if ( !ft )\
		return E_FAIL;\
	ft->file = &file_functions;\
	ft->file->set_options = file_set_options;\
	ft->file->get_options = file_get_options;\
	ft->file->open = file_open;\
	ft->file->flush_diff = file_flush_diff;\
	ft->file->flush = file_flush;\
	ft->file->sync = file_sync;\
	ft->file->format = file_format;\
	ft->file->write = file_write;\
	ft->file->release = file_release;\
	ft->file->release_all = file_release_all;\
\
	return E_OK;\
}

#endif
