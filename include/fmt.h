#ifndef __TRIX_FMT_H__
#define __TRIX_FMT_H__

typedef struct s_fmt_handler t_fmt_handler;
struct s_fmt_handler
{
    char *name;
    char *type;
	char *decode_name;
    int ( *decode ) ( t_stage * source, t_stage * target );
	char *encode_name;
    int ( *encode ) ( t_stage * source, t_stage * target );
	char *free_priv_name;
    int ( *free_priv ) ( t_stage * stage );
	char *encode_diff_name;
    int ( *encode_diff ) ( t_stage * source, t_stage * modified, t_stage * target );
};

typedef struct s_fmt_table t_fmt_table;
struct s_fmt_table
{
	STRUCT_HEADER_LIST(t_fmt_table);
    t_fmt_handler *handler;
};

#include "trixplug_fmt.h"

t_fmt_handler *fmt_get_handler ( char *name );
unsigned int fmt_decode ( t_stage * s, char *type );
unsigned int fmt_encode ( t_stage * s, t_stage * d );
unsigned int fmt_encode_diff ( t_stage * s1, t_stage * s2, t_stage * d );
unsigned int fmt_free_priv ( t_stage * s );
unsigned int fmt_register_handler ( t_fmt_handler * handler );
unsigned int fmt_add_seer_entry ( char *n, void *f, char **i, int p );
unsigned int fmt_add_seer ( t_fmt_handler * h, char **i );
unsigned int fmt_plug_init ( void );
unsigned int fmt_update ( void );
unsigned int fmt_init ( void );


#endif
