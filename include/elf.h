#ifndef __TRIX_ELF_H__
#define __TRIX_ELF_H__

#define ELF32_R_SYM(i) ((i)>>8)
#define ELF32_R_TYPE(i) ((unsigned char)(i))
#define ELF32_R_INFO(s,t) (((s)<<8)+(unsigned char)(t))

#define ELF32_ST_BIND(i) ((i)>>4)
#define ELF32_ST_TYPE(i) ((i)&0xf)
#define ELF32_ST_INFO(b,t) (((b)<<4)+((t)&0xf))

typedef unsigned int Elf32_Addr;
typedef unsigned short Elf32_Half;
typedef unsigned int Elf32_Off;
typedef int Elf32_Sword;
typedef unsigned int Elf32_Word;

#define ELFARCH_ARM 0x28

#define SHN_UNDEF 0
#define SHN_LORESERVE 0xff00
#define SHN_LOPROC 0xff00
#define SHN_HIPROC 0xff1f
#define SHN_ABS 0xfff1
#define SHN_COMMON 0xfff2
#define SHN_HIRESERVE 0xffff

#define STT_NOTYPE 0
#define STT_OBJECT 1
#define STT_FUNC 2
#define STT_SECTION 3
#define STT_FILE 4
#define STT_LOPROC 13
#define STT_HIPROC 15

#define STB_LOCAL 0
#define STB_GLOBAL 1
#define STB_WEAK 2
#define STB_LOPROC 13
#define STB_HIPROC 15

#define EI_MAG0 0
#define EI_MAG1 1
#define EI_MAG2 2
#define EI_MAG3 3
#define EI_CLASS 4
#define EI_DATA 5
#define EI_VERSION 6
#define EI_PAD 7
#define EI_NIDENT 16

#define ELFMAG0 0x7f
#define ELFMAG1 'E'
#define ELFMAG2 'L'
#define ELFMAG3 'F'

#define ELFCLASSNONE 0
#define ELFCLASS32 1
#define ELFCLASS64 2

#define ELFDATANONE 0
#define ELFDATA2LSB 1
#define ELFDATA2MSB 2

#define SHT_NULL 0
#define SHT_PROGBITS 1
#define SHT_SYMTAB 2
#define SHT_STRTAB 3
#define SHT_RELA 4
#define SHT_HASH 5
#define SHT_DYNAMIC 6
#define SHT_NOTE 7
#define SHT_NOBITS 8
#define SHT_REL 9
#define SHT_SHLIB 10
#define SHT_DYNSYM 11
#define SHT_LOPROC 0x70000000
#define SHT_HIPROC 0x7fffffff
#define SHT_LOUSER 0x80000000
#define SHT_HIUSER 0xffffffff

#define SHF_WRITE 0x1
#define SHF_ALLOC 0x2
#define SHF_EXECINSTR 0x4
/*
 * ELF header at the beginning of the executable.
 */
typedef struct _Elf32_Ehdr {
	unsigned char e_ident[EI_NIDENT];
	Elf32_Half e_type;
	Elf32_Half e_machine;
	Elf32_Word e_version;
	Elf32_Addr e_entry;
	Elf32_Off  e_phoff;
	Elf32_Off  e_shoff;
	Elf32_Word e_flags;
	Elf32_Half e_ehsize;
	Elf32_Half e_phentsize;
	Elf32_Half e_phnum;
	Elf32_Half e_shentsize;
	Elf32_Half e_shnum;
	Elf32_Half e_shstrndx;
} Elf32_Ehdr;

typedef struct {
	Elf32_Word sh_name;
	Elf32_Word sh_type;
	Elf32_Word sh_flags;
	Elf32_Addr sh_addr;
	Elf32_Off sh_offset;
	Elf32_Word sh_size;
	Elf32_Word sh_link;
	Elf32_Word sh_info;
	Elf32_Word sh_addralign;
	Elf32_Word sh_entsize;
} Elf32_Shdr;

typedef struct {
	Elf32_Word st_name;
	Elf32_Addr st_value;
	Elf32_Word st_size;
	unsigned char st_info;
	unsigned char st_other;
	Elf32_Half st_shndx;
} Elf32_Sym;

typedef struct {
	Elf32_Addr r_offset;
	Elf32_Word r_info;
} Elf32_Rel;

typedef struct {
	Elf32_Addr r_offset;
	Elf32_Word r_info;
	Elf32_Sword r_addend;
} Elf32_Rela;


unsigned int elf_dump_symtabs ( t_workspace *ws );
object *elf_create_object ( t_workspace *ws );
unsigned int elf_dump_object ( t_workspace *ws );


typedef struct s_elf_exports t_elf_exports;
struct s_elf_exports
{
	STRUCT_HEADER_LIST(t_elf_exports);
	char *name;			// e.g. "screenshot_capture"
	char *position;		// e.g. ".text+0x14"
};

typedef struct s_elf_imports t_elf_imports;
struct s_elf_imports
{
	STRUCT_HEADER_LIST(t_elf_imports);
	char *name;			// e.g. "fs_fopen"		or "(internal)"
	char *type;			// e.g. "CALL"			or "PTR"
	char *position;		// e.g. ".test+0x18"	or ".test+0x18"
	char *destination;	// e.g. "fs_fopen"		or ".bss+0x10"
} ;

typedef struct s_elf_object t_elf_object;
struct s_elf_object
{
	STRUCT_HEADER;
	char *name;
	t_elf_exports *exports;
	t_elf_imports *imports;
} ;

#include "trixplug_elf.h"

unsigned int elf_free_object ( t_elf_object * obj);
t_segment *elf_get_tab ( t_workspace * ws, int type, int num);
t_segment *elf_get_symtab ( t_workspace * ws, int num);
t_segment *elf_get_reloc ( t_workspace * ws, int num);
t_segment *elf_get_strtab ( t_workspace * ws, int num);
unsigned int elf_byteswap_rel ( t_priv * source, Elf32_Rel * header);
unsigned int elf_byteswap_sym ( t_priv * source, Elf32_Sym * header);
char *elf_get_string ( t_workspace * ws, int string);
t_segment *elf_get_section ( t_workspace * ws, int section);
unsigned int fmt_elf_cify ( char *name);
char *elf_get_section_name ( t_workspace * ws, int section);
unsigned int elf_parse_symtab ( t_workspace * ws, t_elf_object * obj, t_segment * tab);
unsigned int elf_dump_symtabs ( t_workspace * ws);
Elf32_Sym *elf_get_symbol ( t_workspace * ws, int symbol_id);
char *elf_get_symbol_name ( t_workspace * ws, int symbol_id);
unsigned int elf_parse_reloc ( t_workspace * ws, t_elf_object * obj, t_segment * rel);
unsigned int elf_dump_relocs ( t_workspace * ws);
unsigned int elf_section_used ( t_workspace * ws, t_elf_object * obj, t_segment * seg);
object *elf_create_object ( t_workspace * ws);
unsigned int elf_dump_object ( t_workspace * ws);



#endif
