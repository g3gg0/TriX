
#ifndef __TRIX_TRIXPLUG_SEGMENT_H__
#define __TRIX_TRIXPLUG_SEGMENT_H__

/* trixplug struct */

struct segment_funcs
{
	t_segment *(*add) ( t_segment * s);
	t_segment *(*create) ( );
	t_segment *(*duplicate) ( t_segment * s);
	t_segment *(*find_by_end) ( t_segment * s, unsigned int addr, unsigned int flags);
	t_segment *(*find_by_end_and_name) ( t_segment * s, unsigned int addr, char *name, unsigned int flags);
	t_segment *(*find_by_name) ( t_segment * s, char *name, unsigned int flags);
	t_segment *(*find_by_num) ( t_segment * s, unsigned int num);
	t_segment *(*find_by_start) ( t_segment * s, unsigned int addr, unsigned int flags);
	t_segment *(*get_last) ( t_segment * s);
	unsigned int (*count) ( t_segment * s);
	unsigned int (*dump) ( t_segment * s);
	unsigned int (*is_mapped) ( t_segment * s);
	unsigned int (*release) ( t_segment * s);
	unsigned int (*release_all) ( t_segment * s);
	void (*hide_in_mem) ( t_segment * s);
	void (*map_in_mem) ( t_segment * s);
 	unsigned int (*is_sparse) ( t_segment * s);
};

/* trixplug struct initializer */

#define SEGMENT_PLUG_INIT \
extern struct trix_functions *ft;\
struct segment_funcs segment_functions;\
unsigned int segment_plug_init ( ) \
{\
	if ( !ft )\
		return E_FAIL;\
	ft->segment = &segment_functions;\
	ft->segment->duplicate = segment_duplicate;\
	ft->segment->find_by_num = segment_find_by_num;\
	ft->segment->find_by_end_and_name = segment_find_by_end_and_name;\
	ft->segment->find_by_end = segment_find_by_end;\
	ft->segment->find_by_start = segment_find_by_start;\
	ft->segment->find_by_name = segment_find_by_name;\
	ft->segment->count = segment_count;\
	ft->segment->is_mapped = segment_is_mapped;\
	ft->segment->is_sparse = segment_is_sparse;\
	ft->segment->map_in_mem = segment_map_in_mem;\
	ft->segment->hide_in_mem = segment_hide_in_mem;\
	ft->segment->get_last = segment_get_last;\
	ft->segment->add = segment_add;\
	ft->segment->create = segment_create;\
	ft->segment->dump = segment_dump;\
	ft->segment->release = segment_release;\
	ft->segment->release_all = segment_release_all;\
\
	return E_OK;\
}

#endif
