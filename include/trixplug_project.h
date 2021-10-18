
#ifndef __TRIX_TRIXPLUG_PROJECT_H__
#define __TRIX_TRIXPLUG_PROJECT_H__

/* trixplug struct */

struct project_funcs
{
	t_project *(*load) ( char *file);
	t_project_files *(*get_file) ( t_project * p, unsigned int pos);
	unsigned int (*add_file) ( t_project * p, char *file);
	unsigned int (*del_file) ( t_project * p, unsigned int pos);
	unsigned int (*init) ( );
	unsigned int (*save) ( t_project * p, char *file);
 	t_project *(*create) ( );
 	unsigned int (*free) ( t_project * p);
};

/* trixplug struct initializer */

#define PROJECT_PLUG_INIT \
extern struct trix_functions *ft;\
struct project_funcs project_functions;\
unsigned int project_plug_init ( ) \
{\
	if ( !ft )\
		return E_FAIL;\
	ft->project = &project_functions;\
	ft->project->free = project_free;\
	ft->project->create = project_create;\
	ft->project->init = project_init;\
	ft->project->add_file = project_add_file;\
	ft->project->get_file = project_get_file;\
	ft->project->del_file = project_del_file;\
	ft->project->load = project_load;\
	ft->project->save = project_save;\
\
	return E_OK;\
}

#endif
