#ifndef __TRIX_MEM_H__
#define __TRIX_MEM_H__


// MEM_SEC_ZONE MUST BE WORD-ALIGNED (4 byte steps)
#define MEM_SEC_ZONE 0x0008
#define MEM_DEBUG
//#define MEM_KEEP_MALLOCED
#define MEM_HASH_MAX      0xF00

#define MEM_MARK_DEFAULT  0xAC01D1CE

// the primitive lock mechanisms
#define MEM_LOCK_RD { \
	if ( !mem_mutex )\
		mem_mutex = mutex_create();\
	mutex_lock ( mem_mutex );\
	}
#define MEM_UNLOCK_RD { \
	mutex_unlock ( mem_mutex );\
	}

#define MEM_LOCK_WR { \
	if ( !mem_mutex )\
		mem_mutex = mutex_create();\
	mutex_lock ( mem_mutex );\
	}
#define MEM_UNLOCK_WR { \
	mutex_unlock ( mem_mutex );\
	}



typedef struct s_mem_list t_mem_list;
struct s_mem_list
{
	STRUCT_HEADER_LIST(t_mem_list);
	unsigned int bytes;
	unsigned int address;
	char caller[32];
	char *type;
	int line;
	int freed;
	unsigned int state_mark;
};

#include "trixplug_mem.h"



#ifndef __TRIX_MEM_C__
#ifdef MEM_DEBUG
#define calloc(x,y)      mem_calloc(x,y,__FUNCTION__,__LINE__)
#define malloc(x)        mem_allocate(x,__FUNCTION__,__LINE__)
#define tmalloc(x,type)  mem_allocate_type(x,__FUNCTION__,__LINE__,type)
#define realloc(x,y)     mem_reallocate(x,y,__FUNCTION__,__LINE__)
#define free(x)          mem_release(x,__FUNCTION__,__LINE__)
#define strdup(x)        mem_strduplicate(x,__FUNCTION__,__LINE__)
unsigned int mem_is_valid ( void *ptr );
#else
#define tmalloc(x,type)  malloc(x)
#endif
#endif


unsigned int mem_get_hash ( unsigned int hash );
void * mem_allocate ( unsigned int bytes, const char *func, int line );
t_mem_list * mem_get_entry ( void *ptr );
unsigned int mem_delete_old_entry ( unsigned int address );
void *mem_allocate_type ( unsigned int bytes, const char *func, int line, char *type );
void *mem_strduplicate ( char *text, const char *func, int line );
unsigned int mem_check ( t_mem_list *entry, unsigned int final );
unsigned int mem_check_overwrite ( void *ptr  );
unsigned int mem_check_overwrites (  );
unsigned int mem_check_all (  );
unsigned int mem_release_tagged (  );
unsigned int mem_set_state_mark ( unsigned int mark, unsigned char *prefix );
unsigned int mem_get_state_mark ( unsigned int *mark, unsigned char *prefix );
void *mem_reallocate ( void *ptr, unsigned int bytes, const char *func, int line );
void mem_release ( void *ptr, const char *func, int line );
unsigned int mem_is_valid ( void *ptr );
void *mem_reallocate_seer ( void *ptr, unsigned int bytes );
void *mem_allocate_seer ( unsigned int bytes );
void *mem_calloc_seer ( unsigned int count, unsigned int size );
void *mem_strduplicate_seer ( char *text );
void mem_release_seer ( char *buf );
void *mem_allocate_passthru ( unsigned int bytes );
void mem_release_passthru ( char *buf );
void *mem_calloc ( unsigned int count, unsigned int size, const char *func, int line );
unsigned int mem_init ( );
#endif
