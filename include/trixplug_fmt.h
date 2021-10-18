
#ifndef __TRIX_TRIXPLUG_FMT_H__
#define __TRIX_TRIXPLUG_FMT_H__

/* trixplug struct */

struct fmt_funcs
{
	t_fmt_handler *(*get_handler) ( char *name);
	unsigned int (*add_seer) ( t_fmt_handler * h, char **i);
	unsigned int (*add_seer_entry) ( char *n, void *f, char **i, int p);
	unsigned int (*decode) ( t_stage * s, char *type);
	unsigned int (*encode) ( t_stage * s, t_stage * d);
	unsigned int (*encode_diff) ( t_stage * s1, t_stage * s2, t_stage * d);
	unsigned int (*free_priv) ( t_stage * s);
	unsigned int (*init) ( );
	unsigned int (*register_handler) ( t_fmt_handler * handler);
	unsigned int (*update) ( );
};

/* trixplug struct initializer */

#define FMT_PLUG_INIT \
extern struct trix_functions *ft;\
struct fmt_funcs fmt_functions;\
unsigned int fmt_plug_init ( ) \
{\
	if ( !ft )\
		return E_FAIL;\
	ft->fmt = &fmt_functions;\
	ft->fmt->get_handler = fmt_get_handler;\
	ft->fmt->decode = fmt_decode;\
	ft->fmt->encode = fmt_encode;\
	ft->fmt->encode_diff = fmt_encode_diff;\
	ft->fmt->free_priv = fmt_free_priv;\
	ft->fmt->register_handler = fmt_register_handler;\
	ft->fmt->add_seer_entry = fmt_add_seer_entry;\
	ft->fmt->add_seer = fmt_add_seer;\
	ft->fmt->update = fmt_update;\
	ft->fmt->init = fmt_init;\
\
	return E_OK;\
}

#endif
