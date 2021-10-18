
#ifndef __TRIX_TRIXPLUG_ELF_H__
#define __TRIX_TRIXPLUG_ELF_H__

/* trixplug struct */

struct elf_funcs
{
	Elf32_Sym *(*get_symbol) ( t_workspace * ws, int symbol_id);
	char *(*get_section_name) ( t_workspace * ws, int section);
	char *(*get_string) ( t_workspace * ws, int string);
	char *(*get_symbol_name) ( t_workspace * ws, int symbol_id);
	object *(*create_object) ( t_workspace * ws);
	t_segment *(*get_reloc) ( t_workspace * ws, int num);
	t_segment *(*get_section) ( t_workspace * ws, int section);
	t_segment *(*get_strtab) ( t_workspace * ws, int num);
	t_segment *(*get_symtab) ( t_workspace * ws, int num);
	t_segment *(*get_tab) ( t_workspace * ws, int type, int num);
	unsigned int (*byteswap_rel) ( t_priv * source, Elf32_Rel * header);
	unsigned int (*byteswap_sym) ( t_priv * source, Elf32_Sym * header);
	unsigned int (*dump_object) ( t_workspace * ws);
	unsigned int (*dump_relocs) ( t_workspace * ws);
	unsigned int (*dump_symtabs) ( t_workspace * ws);
	unsigned int (*fmt_elf_cify) ( char *name);
	unsigned int (*free_object) ( t_elf_object * obj);
	unsigned int (*parse_reloc) ( t_workspace * ws, t_elf_object * obj, t_segment * rel);
	unsigned int (*parse_symtab) ( t_workspace * ws, t_elf_object * obj, t_segment * tab);
	unsigned int (*section_used) ( t_workspace * ws, t_elf_object * obj, t_segment * seg);
};

/* trixplug struct initializer */

#define ELF_PLUG_INIT \
extern struct trix_functions *ft;\
struct elf_funcs elf_functions;\
unsigned int elf_plug_init ( ) \
{\
	if ( !ft )\
		return E_FAIL;\
	ft->elf = &elf_functions;\
	ft->elf->free_object = elf_free_object;\
	ft->elf->get_tab = elf_get_tab;\
	ft->elf->get_symtab = elf_get_symtab;\
	ft->elf->get_reloc = elf_get_reloc;\
	ft->elf->get_strtab = elf_get_strtab;\
	ft->elf->byteswap_rel = elf_byteswap_rel;\
	ft->elf->byteswap_sym = elf_byteswap_sym;\
	ft->elf->get_string = elf_get_string;\
	ft->elf->get_section = elf_get_section;\
	ft->elf->fmt_elf_cify = fmt_elf_cify;\
	ft->elf->get_section_name = elf_get_section_name;\
	ft->elf->parse_symtab = elf_parse_symtab;\
	ft->elf->dump_symtabs = elf_dump_symtabs;\
	ft->elf->get_symbol = elf_get_symbol;\
	ft->elf->get_symbol_name = elf_get_symbol_name;\
	ft->elf->parse_reloc = elf_parse_reloc;\
	ft->elf->dump_relocs = elf_dump_relocs;\
	ft->elf->section_used = elf_section_used;\
	ft->elf->create_object = elf_create_object;\
	ft->elf->dump_object = elf_dump_object;\
\
	return E_OK;\
}

#endif
