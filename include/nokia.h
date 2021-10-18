#ifndef __TRIX_NOKIA_H__
#define __TRIX_NOKIA_H__

#include "trixplug_nokia.h"

unsigned int nokia_init ( );
unsigned int nokia_reserve_ram ( unsigned int size);
unsigned int nokia_release_ram ( unsigned int id);
unsigned int nokia_reserve_eeprom ( unsigned int size);
unsigned int nokia_release_eeprom ( unsigned int id);
unsigned int nokia_create_func ( t_workspace * ws, t_function * func, unsigned int offset);
unsigned int nokia_create_func2 ( t_workspace *ws, object *func, unsigned int offset, unsigned int base );
unsigned int nokia_is_section_name_relative ( unsigned char *name );
unsigned int nokia_get_section_dest ( t_workspace *ws, unsigned char *name );
unsigned int nokia_export_segments ( t_workspace *ws, object *func );
unsigned int nokia_fill_section_data ( t_workspace *ws, object *func, unsigned int offset, unsigned int base );
#endif
