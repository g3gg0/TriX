#ifndef __TRIX_SYMBOLS_H__
#define __TRIX_SYMBOLS_H__


#include "trixplug_symbols.h"


unsigned int symbols_add ( t_stage *stage, const char *name, const int type, const char *data );
unsigned int symbols_count ( t_stage *stage, const int type );
unsigned int symbols_free_all ( t_stage *stage );
t_symbols *symbols_get ( t_stage *stage, const char *name );
unsigned int symbols_remove ( t_stage *stage, const char* name );
void symbols_dump ( t_stage *stage );
unsigned int symbols_get_address ( t_stage *stage, const char *name );


#endif

