
#ifndef __TRIX_TRIXPLUG_ARM_H__
#define __TRIX_TRIXPLUG_ARM_H__

/* trixplug struct */

struct arm_funcs
{
	object *(*create_object) ( const char *makefile_path, const char *object_file);
	object *(*create_object_from_makefile) ( const char *makefile_path, const char *object_file);
	unsigned int (*create_func) ( t_workspace * ws, char *routine, unsigned int offset);
	unsigned int (*find) ( t_workspace * ws, unsigned int type, unsigned int start, unsigned int end, unsigned int direction);
	unsigned int (*find_src_of_dest) ( t_workspace * ws, int type, unsigned int dest, unsigned int start, unsigned int end, unsigned int direction);
	unsigned int (*get) ( t_workspace * ws, unsigned int offset);
	unsigned int (*get_bl) ( t_workspace * ws, unsigned int offset);
	unsigned int (*get_freespace) ( t_workspace * ws, unsigned int size, unsigned int offset);
	unsigned int (*init) ( );
	unsigned int (*run_makefile) ( const char *makefile_path);
	unsigned int (*set) ( t_workspace * ws, unsigned int type, unsigned int offset, unsigned int dest);
};

/* trixplug struct initializer */

#define ARM_PLUG_INIT \
extern struct trix_functions *ft;\
struct arm_funcs arm_functions;\
unsigned int arm_plug_init ( ) \
{\
	if ( !ft )\
		return E_FAIL;\
	ft->arm = &arm_functions;\
	ft->arm->init = arm_init;\
	ft->arm->set = arm_set;\
	ft->arm->get_bl = arm_get_bl;\
	ft->arm->get = arm_get;\
	ft->arm->find = arm_find;\
	ft->arm->get_freespace = arm_get_freespace;\
	ft->arm->find_src_of_dest = arm_find_src_of_dest;\
	ft->arm->create_func = arm_create_func;\
	ft->arm->create_object_from_makefile = arm_create_object_from_makefile;\
	ft->arm->run_makefile = arm_run_makefile;\
	ft->arm->create_object = arm_create_object;\
\
	return E_OK;\
}

#endif
