
#ifndef __TRIX_TRIXPLUG_WORKSPACE_H__
#define __TRIX_TRIXPLUG_WORKSPACE_H__

/* trixplug struct */

struct workspace_funcs
{
	t_workspace *(*create) ( );
	t_workspace *(*create_file) ( );
	t_workspace *(*create_file_from_buffer) ( unsigned char *buffer, unsigned int length);
	t_workspace *(*startup) ( char *filename);
	unsigned int (*add_file) ( t_workspace * ws, t_fileinfo * fileinfo);
	unsigned int (*memmap_add) ( t_workspace * ws, t_stage * stage, t_segment * seg);
	unsigned int (*memmap_release) ( t_workspace * ws);
	unsigned int (*memmap_sort) ( t_workspace * ws);
	unsigned int (*release) ( t_workspace * ws);
	unsigned int (*set_modified) ( t_workspace * ws);
	unsigned int (*update_memmap) ( t_workspace * ws);
	void (*skip_error) ( int skip);
 	unsigned int (*memmap_reverse) ( t_workspace * ws);
};

/* trixplug struct initializer */

#define WORKSPACE_PLUG_INIT \
extern struct trix_functions *ft;\
struct workspace_funcs workspace_functions;\
unsigned int workspace_plug_init ( ) \
{\
	if ( !ft )\
		return E_FAIL;\
	ft->workspace = &workspace_functions;\
	ft->workspace->skip_error = workspace_skip_error;\
	ft->workspace->set_modified = workspace_set_modified;\
	ft->workspace->startup = workspace_startup;\
	ft->workspace->create = workspace_create;\
	ft->workspace->create_file = workspace_create_file;\
	ft->workspace->create_file_from_buffer = workspace_create_file_from_buffer;\
	ft->workspace->add_file = workspace_add_file;\
	ft->workspace->memmap_add = workspace_memmap_add;\
	ft->workspace->release = workspace_release;\
	ft->workspace->memmap_release = workspace_memmap_release;\
	ft->workspace->memmap_sort = workspace_memmap_sort;\
	ft->workspace->memmap_reverse = workspace_memmap_reverse;\
	ft->workspace->update_memmap = workspace_update_memmap;\
\
	return E_OK;\
}

#endif
