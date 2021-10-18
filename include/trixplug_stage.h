
#ifndef __TRIX_TRIXPLUG_STAGE_H__
#define __TRIX_TRIXPLUG_STAGE_H__

/* trixplug struct */

struct stage_funcs
{
	t_stage *(*add) ( t_stage * s);
	t_stage *(*create) ( );
	t_stage *(*duplicate) ( t_stage * s);
	t_stage *(*find_by_num) ( t_stage * s, unsigned int num);
	t_stage *(*get) ( t_stage * s, unsigned int flag);
	t_stage *(*get_current) ( t_stage * s);
	t_stage *(*get_last) ( t_stage * s);
	t_stage *(*get_modified) ( t_stage * s);
	t_stage *(*get_savestage) ( t_stage * s);
	t_stage *(*get_workspace) ( t_stage * s);
	t_stage_info *(*save_info) ( t_stage * s);
	unsigned int (*clear_flags) ( t_stage * s, unsigned int flag);
	unsigned int (*count) ( t_stage * s);
	unsigned int (*dump) ( t_stage * s);
	unsigned int (*get_end) ( t_stage * s);
	unsigned int (*get_num) ( t_stage * s);
	unsigned int (*get_segments) ( t_stage * s);
	unsigned int (*get_start) ( t_stage * s);
	unsigned int (*has_overlaps) ( t_stage * s);
	unsigned int (*has_unmapped) ( t_stage * s);
	unsigned int (*merge) ( t_stage * s, unsigned char *buffer, unsigned int length);
	unsigned int (*release) ( t_stage * s);
	unsigned int (*release_all) ( t_stage * s);
	unsigned int (*replace) ( t_stage * t, t_stage * s);
	unsigned int (*restore_info) ( t_stage * s, t_stage_info * i);
	unsigned int (*set_modified) ( t_stage * s);
	unsigned int (*set_savestage) ( t_stage * s);
	unsigned int (*set_workspace) ( t_stage * s);
	unsigned int (*release_info) ( t_stage_info * info);
};

/* trixplug struct initializer */

#define STAGE_PLUG_INIT \
extern struct trix_functions *ft;\
struct stage_funcs stage_functions;\
unsigned int stage_plug_init ( ) \
{\
	if ( !ft )\
		return E_FAIL;\
	ft->stage = &stage_functions;\
	ft->stage->merge = stage_merge;\
	ft->stage->get_start = stage_get_start;\
	ft->stage->get_end = stage_get_end;\
	ft->stage->has_unmapped = stage_has_unmapped;\
	ft->stage->has_overlaps = stage_has_overlaps;\
	ft->stage->replace = stage_replace;\
	ft->stage->duplicate = stage_duplicate;\
	ft->stage->add = stage_add;\
	ft->stage->create = stage_create;\
	ft->stage->find_by_num = stage_find_by_num;\
	ft->stage->get_last = stage_get_last;\
	ft->stage->restore_info = stage_restore_info;\
	ft->stage->release_info = stage_release_info;\
	ft->stage->save_info = stage_save_info;\
	ft->stage->count = stage_count;\
	ft->stage->get_num = stage_get_num;\
	ft->stage->get_segments = stage_get_segments;\
	ft->stage->dump = stage_dump;\
	ft->stage->get = stage_get;\
	ft->stage->clear_flags = stage_clear_flags;\
	ft->stage->get_modified = stage_get_modified;\
	ft->stage->get_savestage = stage_get_savestage;\
	ft->stage->get_workspace = stage_get_workspace;\
	ft->stage->set_modified = stage_set_modified;\
	ft->stage->set_savestage = stage_set_savestage;\
	ft->stage->set_workspace = stage_set_workspace;\
	ft->stage->get_current = stage_get_current;\
	ft->stage->release = stage_release;\
	ft->stage->release_all = stage_release_all;\
\
	return E_OK;\
}

#endif
