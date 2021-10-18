
#ifndef __TRIX_TRIXPLUG_NOKIA_H__
#define __TRIX_TRIXPLUG_NOKIA_H__

/* trixplug struct */

struct nokia_funcs
{
	unsigned int (*create_func) ( t_workspace * ws, t_function * func, unsigned int offset);
	unsigned int (*create_func2) ( t_workspace * ws, object * func, unsigned int offset, unsigned int base);
	unsigned int (*export_segments) ( t_workspace * ws, object * func);
	unsigned int (*fill_section_data) ( t_workspace * ws, object * func, unsigned int offset, unsigned int base);
	unsigned int (*get_section_dest) ( t_workspace * ws, unsigned char *name);
	unsigned int (*init) ( );
	unsigned int (*is_section_name_relative) ( unsigned char *name);
	unsigned int (*release_eeprom) ( unsigned int id);
	unsigned int (*release_ram) ( unsigned int id);
	unsigned int (*reserve_eeprom) ( unsigned int size);
	unsigned int (*reserve_ram) ( unsigned int size);
};

/* trixplug struct initializer */

#define NOKIA_PLUG_INIT \
extern struct trix_functions *ft;\
struct nokia_funcs nokia_functions;\
unsigned int nokia_plug_init ( ) \
{\
	if ( !ft )\
		return E_FAIL;\
	ft->nokia = &nokia_functions;\
	ft->nokia->init = nokia_init;\
	ft->nokia->reserve_ram = nokia_reserve_ram;\
	ft->nokia->release_ram = nokia_release_ram;\
	ft->nokia->reserve_eeprom = nokia_reserve_eeprom;\
	ft->nokia->release_eeprom = nokia_release_eeprom;\
	ft->nokia->is_section_name_relative = nokia_is_section_name_relative;\
	ft->nokia->get_section_dest = nokia_get_section_dest;\
	ft->nokia->export_segments = nokia_export_segments;\
	ft->nokia->fill_section_data = nokia_fill_section_data;\
	ft->nokia->create_func2 = nokia_create_func2;\
	ft->nokia->create_func = nokia_create_func;\
\
	return E_OK;\
}

#endif
