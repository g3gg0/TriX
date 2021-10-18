
#ifndef __TRIX_TRIXPLUG_MEM_H__
#define __TRIX_TRIXPLUG_MEM_H__

/* trixplug struct */

struct mem_funcs
{
	t_mem_list *(*get_entry) ( void *ptr);
	unsigned int (*check) ( t_mem_list * entry, unsigned int final);
	unsigned int (*check_all) ( );
	unsigned int (*check_overwrite) ( void *ptr);
	unsigned int (*check_overwrites) ( );
	unsigned int (*delete_old_entry) ( unsigned int address);
	unsigned int (*get_hash) ( unsigned int hash);
	unsigned int (*get_state_mark) ( unsigned int *mark, unsigned char *prefix);
	unsigned int (*init) ( );
	unsigned int (*is_valid) ( void *ptr);
	unsigned int (*release_tagged) ( );
	unsigned int (*set_state_mark) ( unsigned int mark, unsigned char *prefix);
	void (*release) ( void *ptr, const char *func, int line);
	void (*release_passthru) ( char *buf);
	void (*release_seer) ( char *buf);
	void *(*allocate) ( unsigned int bytes, const char *func, int line);
	void *(*allocate_passthru) ( unsigned int bytes);
	void *(*allocate_seer) ( unsigned int bytes);
	void *(*allocate_type) ( unsigned int bytes, const char *func, int line, char *type);
	void *(*calloc) ( unsigned int count, unsigned int size, const char *func, int line);
	void *(*calloc_seer) ( unsigned int count, unsigned int size);
	void *(*reallocate) ( void *ptr, unsigned int bytes, const char *func, int line);
	void *(*reallocate_seer) ( void *ptr, unsigned int bytes);
	void *(*strduplicate) ( char *text, const char *func, int line);
	void *(*strduplicate_seer) ( char *text);
};

/* trixplug struct initializer */

#define MEM_PLUG_INIT \
extern struct trix_functions *ft;\
struct mem_funcs mem_functions;\
unsigned int mem_plug_init ( ) \
{\
	if ( !ft )\
		return E_FAIL;\
	ft->mem = &mem_functions;\
	ft->mem->init = mem_init;\
	ft->mem->get_hash = mem_get_hash;\
	ft->mem->allocate = mem_allocate;\
	ft->mem->get_entry = mem_get_entry;\
	ft->mem->delete_old_entry = mem_delete_old_entry;\
	ft->mem->allocate_type = mem_allocate_type;\
	ft->mem->strduplicate = mem_strduplicate;\
	ft->mem->calloc = mem_calloc;\
	ft->mem->check = mem_check;\
	ft->mem->check_overwrite = mem_check_overwrite;\
	ft->mem->check_overwrites = mem_check_overwrites;\
	ft->mem->check_all = mem_check_all;\
	ft->mem->set_state_mark = mem_set_state_mark;\
	ft->mem->get_state_mark = mem_get_state_mark;\
	ft->mem->release_tagged = mem_release_tagged;\
	ft->mem->reallocate = mem_reallocate;\
	ft->mem->release = mem_release;\
	ft->mem->is_valid = mem_is_valid;\
	ft->mem->reallocate_seer = mem_reallocate_seer;\
	ft->mem->allocate_seer = mem_allocate_seer;\
	ft->mem->calloc_seer = mem_calloc_seer;\
	ft->mem->strduplicate_seer = mem_strduplicate_seer;\
	ft->mem->release_seer = mem_release_seer;\
	ft->mem->allocate_passthru = mem_allocate_passthru;\
	ft->mem->release_passthru = mem_release_passthru;\
\
	return E_OK;\
}

#endif
