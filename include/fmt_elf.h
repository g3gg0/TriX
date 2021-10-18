#ifndef __TRIX_FMT_ELF_H__
#define __TRIX_FMT_ELF_H__

typedef struct s_elf_priv t_elf_priv;
struct s_elf_priv
{
	STRUCT_HEADER;
	int hdr_str_offset;
	Elf32_Ehdr header;
};

typedef struct s_elf_sect_priv t_elf_sect_priv;
struct s_elf_sect_priv
{
	STRUCT_HEADER;
	Elf32_Shdr header;
};

#endif
