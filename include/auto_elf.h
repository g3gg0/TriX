

/* seer headers for elf.c */

#define ELF_AUTO_IMPORTS \
	"import unsigned int __elf_free_object ( t_elf_object * obj);\n"\
	"import t_segment *__elf_get_tab ( t_workspace * ws, int type, int num);\n"\
	"import t_segment *__elf_get_symtab ( t_workspace * ws, int num);\n"\
	"import t_segment *__elf_get_reloc ( t_workspace * ws, int num);\n"\
	"import t_segment *__elf_get_strtab ( t_workspace * ws, int num);\n"\
	"import unsigned int __elf_byteswap_rel ( t_priv * source, Elf32_Rel * header);\n"\
	"import unsigned int __elf_byteswap_sym ( t_priv * source, Elf32_Sym * header);\n"\
	"import char *__elf_get_string ( t_workspace * ws, int string);\n"\
	"import t_segment *__elf_get_section ( t_workspace * ws, int section);\n"\
	"import unsigned int __fmt_elf_cify ( char *name);\n"\
	"import char *__elf_get_section_name ( t_workspace * ws, int section);\n"\
	"import unsigned int __elf_parse_symtab ( t_workspace * ws, t_elf_object * obj, t_segment * tab);\n"\
	"import unsigned int __elf_dump_symtabs ( t_workspace * ws);\n"\
	"import Elf32_Sym *__elf_get_symbol ( t_workspace * ws, int symbol_id);\n"\
	"import char *__elf_get_symbol_name ( t_workspace * ws, int symbol_id);\n"\
	"import unsigned int __elf_parse_reloc ( t_workspace * ws, t_elf_object * obj, t_segment * rel);\n"\
	"import unsigned int __elf_dump_relocs ( t_workspace * ws);\n"\
	"import unsigned int __elf_section_used ( t_workspace * ws, t_elf_object * obj, t_segment * seg);\n"\
	"import object *__elf_create_object ( t_workspace * ws);\n"\
	"import unsigned int __elf_dump_object ( t_workspace * ws);\n"\



#define ELF_AUTO_IMPORTS_HTML \
	"		<font face=\"Monospace\">"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__elf_free_object</font>&nbsp;(&nbsp;<font color=\"#000000\">t_elf_object</font>&nbsp;*&nbsp;<font color=\"#000000\">obj</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#000000\">t_segment</font>&nbsp;*<font color=\"#000000\">__elf_get_tab</font>&nbsp;(&nbsp;<font color=\"#000000\">t_workspace</font>&nbsp;*&nbsp;<font color=\"#000000\">ws</font>,&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">type</font>,&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">num</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#000000\">t_segment</font>&nbsp;*<font color=\"#000000\">__elf_get_symtab</font>&nbsp;(&nbsp;<font color=\"#000000\">t_workspace</font>&nbsp;*&nbsp;<font color=\"#000000\">ws</font>,&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">num</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#000000\">t_segment</font>&nbsp;*<font color=\"#000000\">__elf_get_reloc</font>&nbsp;(&nbsp;<font color=\"#000000\">t_workspace</font>&nbsp;*&nbsp;<font color=\"#000000\">ws</font>,&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">num</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#000000\">t_segment</font>&nbsp;*<font color=\"#000000\">__elf_get_strtab</font>&nbsp;(&nbsp;<font color=\"#000000\">t_workspace</font>&nbsp;*&nbsp;<font color=\"#000000\">ws</font>,&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">num</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__elf_byteswap_rel</font>&nbsp;(&nbsp;<font color=\"#000000\">t_priv</font>&nbsp;*&nbsp;<font color=\"#000000\">source</font>,&nbsp;<font color=\"#000000\">Elf32_Rel</font>&nbsp;*&nbsp;<font color=\"#000000\">header</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__elf_byteswap_sym</font>&nbsp;(&nbsp;<font color=\"#000000\">t_priv</font>&nbsp;*&nbsp;<font color=\"#000000\">source</font>,&nbsp;<font color=\"#000000\">Elf32_Sym</font>&nbsp;*&nbsp;<font color=\"#000000\">header</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">__elf_get_string</font>&nbsp;(&nbsp;<font color=\"#000000\">t_workspace</font>&nbsp;*&nbsp;<font color=\"#000000\">ws</font>,&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">string</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#000000\">t_segment</font>&nbsp;*<font color=\"#000000\">__elf_get_section</font>&nbsp;(&nbsp;<font color=\"#000000\">t_workspace</font>&nbsp;*&nbsp;<font color=\"#000000\">ws</font>,&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">section</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__fmt_elf_cify</font>&nbsp;(&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">name</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">__elf_get_section_name</font>&nbsp;(&nbsp;<font color=\"#000000\">t_workspace</font>&nbsp;*&nbsp;<font color=\"#000000\">ws</font>,&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">section</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__elf_parse_symtab</font>&nbsp;(&nbsp;<font color=\"#000000\">t_workspace</font>&nbsp;*&nbsp;<font color=\"#000000\">ws</font>,&nbsp;<font color=\"#000000\">t_elf_object</font>&nbsp;*&nbsp;<font color=\"#000000\">obj</font>,&nbsp;<font color=\"#000000\">t_segment</font>&nbsp;*&nbsp;<font color=\"#000000\">tab</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__elf_dump_symtabs</font>&nbsp;(&nbsp;<font color=\"#000000\">t_workspace</font>&nbsp;*&nbsp;<font color=\"#000000\">ws</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#000000\">Elf32_Sym</font>&nbsp;*<font color=\"#000000\">__elf_get_symbol</font>&nbsp;(&nbsp;<font color=\"#000000\">t_workspace</font>&nbsp;*&nbsp;<font color=\"#000000\">ws</font>,&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">symbol_id</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">__elf_get_symbol_name</font>&nbsp;(&nbsp;<font color=\"#000000\">t_workspace</font>&nbsp;*&nbsp;<font color=\"#000000\">ws</font>,&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">symbol_id</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__elf_parse_reloc</font>&nbsp;(&nbsp;<font color=\"#000000\">t_workspace</font>&nbsp;*&nbsp;<font color=\"#000000\">ws</font>,&nbsp;<font color=\"#000000\">t_elf_object</font>&nbsp;*&nbsp;<font color=\"#000000\">obj</font>,&nbsp;<font color=\"#000000\">t_segment</font>&nbsp;*&nbsp;<font color=\"#000000\">rel</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__elf_dump_relocs</font>&nbsp;(&nbsp;<font color=\"#000000\">t_workspace</font>&nbsp;*&nbsp;<font color=\"#000000\">ws</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__elf_section_used</font>&nbsp;(&nbsp;<font color=\"#000000\">t_workspace</font>&nbsp;*&nbsp;<font color=\"#000000\">ws</font>,&nbsp;<font color=\"#000000\">t_elf_object</font>&nbsp;*&nbsp;<font color=\"#000000\">obj</font>,&nbsp;<font color=\"#000000\">t_segment</font>&nbsp;*&nbsp;<font color=\"#000000\">seg</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#000000\">object</font>&nbsp;*<font color=\"#000000\">__elf_create_object</font>&nbsp;(&nbsp;<font color=\"#000000\">t_workspace</font>&nbsp;*&nbsp;<font color=\"#000000\">ws</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__elf_dump_object</font>&nbsp;(&nbsp;<font color=\"#000000\">t_workspace</font>&nbsp;*&nbsp;<font color=\"#000000\">ws</font>);<br>"\
	"<br>"\
	"		</font>"\

/* seer function registration for elf.c */

#define ELF_AUTO_REGISTER \
	scAddExtSymInt ( elf_free_object );\
	scAddExtSymInt ( elf_get_tab );\
	scAddExtSymInt ( elf_get_symtab );\
	scAddExtSymInt ( elf_get_reloc );\
	scAddExtSymInt ( elf_get_strtab );\
	scAddExtSymInt ( elf_byteswap_rel );\
	scAddExtSymInt ( elf_byteswap_sym );\
	scAddExtSymInt ( elf_get_string );\
	scAddExtSymInt ( elf_get_section );\
	scAddExtSymInt ( fmt_elf_cify );\
	scAddExtSymInt ( elf_get_section_name );\
	scAddExtSymInt ( elf_parse_symtab );\
	scAddExtSymInt ( elf_dump_symtabs );\
	scAddExtSymInt ( elf_get_symbol );\
	scAddExtSymInt ( elf_get_symbol_name );\
	scAddExtSymInt ( elf_parse_reloc );\
	scAddExtSymInt ( elf_dump_relocs );\
	scAddExtSymInt ( elf_section_used );\
	scAddExtSymInt ( elf_create_object );\
	scAddExtSymInt ( elf_dump_object );\


/* seer function declaration for elf.c */
#ifndef HEADER_SKIP_DECLARATION
void elf_free_object ();
void elf_get_tab ();
void elf_get_symtab ();
void elf_get_reloc ();
void elf_get_strtab ();
void elf_byteswap_rel ();
void elf_byteswap_sym ();
void elf_get_string ();
void elf_get_section ();
void fmt_elf_cify ();
void elf_get_section_name ();
void elf_parse_symtab ();
void elf_dump_symtabs ();
void elf_get_symbol ();
void elf_get_symbol_name ();
void elf_parse_reloc ();
void elf_dump_relocs ();
void elf_section_used ();
void elf_create_object ();
void elf_dump_object ();

#endif
/* additional seer includes from elf_seer.h */

#define ELF_AUTO_MISC_PRE \
	"#define ELF32_R_SYM(i) ((i)>>8)\n"\
	"#define ELF32_R_TYPE(i) ((unsigned char)(i))\n"\
	"#define ELF32_R_INFO(s,t) (((s)<<8)+(unsigned char)(t))\n"\
	"\n"\
	"#define ELF32_ST_BIND(i) ((i)>>4)\n"\
	"#define ELF32_ST_TYPE(i) ((i)&0xf)\n"\
	"#define ELF32_ST_INFO(b,t) (((b)<<4)+((t)&0xf))\n"\
	"\n"\
	"typedef unsigned int Elf32_Addr;\n"\
	"typedef unsigned short Elf32_Half;\n"\
	"typedef unsigned int Elf32_Off;\n"\
	"typedef int Elf32_Sword;\n"\
	"typedef unsigned int Elf32_Word;\n"\
	"\n"\
	"#define ELFARCH_ARM 0x28\n"\
	"\n"\
	"#define SHN_UNDEF 0\n"\
	"#define SHN_LORESERVE 0xff00\n"\
	"#define SHN_LOPROC 0xff00\n"\
	"#define SHN_HIPROC 0xff1f\n"\
	"#define SHN_ABS 0xfff1\n"\
	"#define SHN_COMMON 0xfff2\n"\
	"#define SHN_HIRESERVE 0xffff\n"\
	"\n"\
	"#define STT_NOTYPE 0\n"\
	"#define STT_OBJECT 1\n"\
	"#define STT_FUNC 2\n"\
	"#define STT_SECTION 3\n"\
	"#define STT_FILE 4\n"\
	"#define STT_LOPROC 13\n"\
	"#define STT_HIPROC 15\n"\
	"\n"\
	"#define STB_LOCAL 0\n"\
	"#define STB_GLOBAL 1\n"\
	"#define STB_WEAK 2\n"\
	"#define STB_LOPROC 13\n"\
	"#define STB_HIPROC 15\n"\
	"\n"\
	"#define EI_MAG0 0\n"\
	"#define EI_MAG1 1\n"\
	"#define EI_MAG2 2\n"\
	"#define EI_MAG3 3\n"\
	"#define EI_CLASS 4\n"\
	"#define EI_DATA 5\n"\
	"#define EI_VERSION 6\n"\
	"#define EI_PAD 7\n"\
	"#define EI_NIDENT 16\n"\
	"\n"\
	"#define ELFMAG0 0x7f\n"\
	"#define ELFMAG1 'E'\n"\
	"#define ELFMAG2 'L'\n"\
	"#define ELFMAG3 'F'\n"\
	"\n"\
	"#define ELFCLASSNONE 0\n"\
	"#define ELFCLASS32 1\n"\
	"#define ELFCLASS64 2\n"\
	"\n"\
	"#define ELFDATANONE 0\n"\
	"#define ELFDATA2LSB 1\n"\
	"#define ELFDATA2MSB 2\n"\
	"\n"\
	"#define SHT_NULL 0\n"\
	"#define SHT_PROGBITS 1\n"\
	"#define SHT_SYMTAB 2\n"\
	"#define SHT_STRTAB 3\n"\
	"#define SHT_RELA 4\n"\
	"#define SHT_HASH 5\n"\
	"#define SHT_DYNAMIC 6\n"\
	"#define SHT_NOTE 7\n"\
	"#define SHT_NOBITS 8\n"\
	"#define SHT_REL 9\n"\
	"#define SHT_SHLIB 10\n"\
	"#define SHT_DYNSYM 11\n"\
	"#define SHT_LOPROC 0x70000000\n"\
	"#define SHT_HIPROC 0x7fffffff\n"\
	"#define SHT_LOUSER 0x80000000\n"\
	"#define SHT_HIUSER 0xffffffff\n"\
	"\n"\
	"#define SHF_WRITE 0x1\n"\
	"#define SHF_ALLOC 0x2\n"\
	"#define SHF_EXECINSTR 0x4\n"\
	"/*\n"\
	" * ELF header at the beginning of the executable.\n"\
	" */\n"\
	"typedef struct _Elf32_Ehdr {\n"\
	"	unsigned char e_ident[EI_NIDENT];\n"\
	"	Elf32_Half e_type;\n"\
	"	Elf32_Half e_machine;\n"\
	"	Elf32_Word e_version;\n"\
	"	Elf32_Addr e_entry;\n"\
	"	Elf32_Off  e_phoff;\n"\
	"	Elf32_Off  e_shoff;\n"\
	"	Elf32_Word e_flags;\n"\
	"	Elf32_Half e_ehsize;\n"\
	"	Elf32_Half e_phentsize;\n"\
	"	Elf32_Half e_phnum;\n"\
	"	Elf32_Half e_shentsize;\n"\
	"	Elf32_Half e_shnum;\n"\
	"	Elf32_Half e_shstrndx;\n"\
	"} Elf32_Ehdr;\n"\
	"\n"\
	"typedef struct s_Elf32_Shdr {\n"\
	"	Elf32_Word sh_name;\n"\
	"	Elf32_Word sh_type;\n"\
	"	Elf32_Word sh_flags;\n"\
	"	Elf32_Addr sh_addr;\n"\
	"	Elf32_Off sh_offset;\n"\
	"	Elf32_Word sh_size;\n"\
	"	Elf32_Word sh_link;\n"\
	"	Elf32_Word sh_info;\n"\
	"	Elf32_Word sh_addralign;\n"\
	"	Elf32_Word sh_entsize;\n"\
	"} Elf32_Shdr;\n"\
	"\n"\
	"typedef struct s_Elf32_Sym {\n"\
	"	Elf32_Word st_name;\n"\
	"	Elf32_Addr st_value;\n"\
	"	Elf32_Word st_size;\n"\
	"	unsigned char st_info;\n"\
	"	unsigned char st_other;\n"\
	"	Elf32_Half st_shndx;\n"\
	"} Elf32_Sym;\n"\
	"\n"\
	"typedef struct s_Elf32_Rel {\n"\
	"	Elf32_Addr r_offset;\n"\
	"	Elf32_Word r_info;\n"\
	"} Elf32_Rel;\n"\
	"\n"\
	"typedef struct s_Elf32_Rela {\n"\
	"	Elf32_Addr r_offset;\n"\
	"	Elf32_Word r_info;\n"\
	"	Elf32_Sword r_addend;\n"\
	"} Elf32_Rela;\n"\
	"\n"\
	"\n"\
	"typedef struct s_elf_exports\n"\
	"{\n"\
	"	STRUCT_HEADER_LIST(t_elf_exports);\n"\
	"	char *name;			// e.g. \"screenshot_capture\"\n"\
	"	char *position;		// e.g. \".text+0x14\"\n"\
	"} t_elf_exports;\n"\
	"\n"\
	"typedef struct s_elf_imports\n"\
	"{\n"\
	"	STRUCT_HEADER_LIST(t_elf_imports);\n"\
	"	char *name;			// e.g. \"fs_fopen\"		or \"(internal)\"\n"\
	"	char *type;			// e.g. \"CALL\"			or \"PTR\"\n"\
	"	char *position;		// e.g. \".test+0x18\"	or \".test+0x18\"\n"\
	"	char *destination;	// e.g. \"fs_fopen\"		or \".bss+0x10\"\n"\
	"} t_elf_imports;\n"\
	"\n"\
	"typedef struct s_elf_object\n"\
	"{\n"\
	"	STRUCT_HEADER;\n"\
	"	char *name;\n"\
	"	t_elf_exports *exports;\n"\
	"	t_elf_imports *imports;\n"\
	"} t_elf_object;\n"\
	"\n"\


#define ELF_AUTO_MISC_POST \


#define ELF_AUTO_MISC_PRE_HTML \
	"		<font face=\"Monospace\">"\
	"<font color=\"#008000\">#define&nbsp;ELF32_R_SYM(i)&nbsp;((i)&gt;&gt;8)<br></font>"\
	"<font color=\"#008000\">#define&nbsp;ELF32_R_TYPE(i)&nbsp;((unsigned&nbsp;char)(i))<br></font>"\
	"<font color=\"#008000\">#define&nbsp;ELF32_R_INFO(s,t)&nbsp;(((s)&lt;&lt;8)+(unsigned&nbsp;char)(t))<br></font>"\
	"<br>"\
	"<font color=\"#008000\">#define&nbsp;ELF32_ST_BIND(i)&nbsp;((i)&gt;&gt;4)<br></font>"\
	"<font color=\"#008000\">#define&nbsp;ELF32_ST_TYPE(i)&nbsp;((i)&amp;0xf)<br></font>"\
	"<font color=\"#008000\">#define&nbsp;ELF32_ST_INFO(b,t)&nbsp;(((b)&lt;&lt;4)+((t)&amp;0xf))<br></font>"\
	"<br>"\
	"<font color=\"#000080\"><b>typedef</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">Elf32_Addr</font>;<br>"\
	"<font color=\"#000080\"><b>typedef</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">short</font>&nbsp;<font color=\"#000000\">Elf32_Half</font>;<br>"\
	"<font color=\"#000080\"><b>typedef</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">Elf32_Off</font>;<br>"\
	"<font color=\"#000080\"><b>typedef</b></font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">Elf32_Sword</font>;<br>"\
	"<font color=\"#000080\"><b>typedef</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">Elf32_Word</font>;<br>"\
	"<br>"\
	"<font color=\"#008000\">#define&nbsp;ELFARCH_ARM&nbsp;0x28<br></font>"\
	"<br>"\
	"<font color=\"#008000\">#define&nbsp;SHN_UNDEF&nbsp;0<br></font>"\
	"<font color=\"#008000\">#define&nbsp;SHN_LORESERVE&nbsp;0xff00<br></font>"\
	"<font color=\"#008000\">#define&nbsp;SHN_LOPROC&nbsp;0xff00<br></font>"\
	"<font color=\"#008000\">#define&nbsp;SHN_HIPROC&nbsp;0xff1f<br></font>"\
	"<font color=\"#008000\">#define&nbsp;SHN_ABS&nbsp;0xfff1<br></font>"\
	"<font color=\"#008000\">#define&nbsp;SHN_COMMON&nbsp;0xfff2<br></font>"\
	"<font color=\"#008000\">#define&nbsp;SHN_HIRESERVE&nbsp;0xffff<br></font>"\
	"<br>"\
	"<font color=\"#008000\">#define&nbsp;STT_NOTYPE&nbsp;0<br></font>"\
	"<font color=\"#008000\">#define&nbsp;STT_OBJECT&nbsp;1<br></font>"\
	"<font color=\"#008000\">#define&nbsp;STT_FUNC&nbsp;2<br></font>"\
	"<font color=\"#008000\">#define&nbsp;STT_SECTION&nbsp;3<br></font>"\
	"<font color=\"#008000\">#define&nbsp;STT_FILE&nbsp;4<br></font>"\
	"<font color=\"#008000\">#define&nbsp;STT_LOPROC&nbsp;13<br></font>"\
	"<font color=\"#008000\">#define&nbsp;STT_HIPROC&nbsp;15<br></font>"\
	"<br>"\
	"<font color=\"#008000\">#define&nbsp;STB_LOCAL&nbsp;0<br></font>"\
	"<font color=\"#008000\">#define&nbsp;STB_GLOBAL&nbsp;1<br></font>"\
	"<font color=\"#008000\">#define&nbsp;STB_WEAK&nbsp;2<br></font>"\
	"<font color=\"#008000\">#define&nbsp;STB_LOPROC&nbsp;13<br></font>"\
	"<font color=\"#008000\">#define&nbsp;STB_HIPROC&nbsp;15<br></font>"\
	"<br>"\
	"<font color=\"#008000\">#define&nbsp;EI_MAG0&nbsp;0<br></font>"\
	"<font color=\"#008000\">#define&nbsp;EI_MAG1&nbsp;1<br></font>"\
	"<font color=\"#008000\">#define&nbsp;EI_MAG2&nbsp;2<br></font>"\
	"<font color=\"#008000\">#define&nbsp;EI_MAG3&nbsp;3<br></font>"\
	"<font color=\"#008000\">#define&nbsp;EI_CLASS&nbsp;4<br></font>"\
	"<font color=\"#008000\">#define&nbsp;EI_DATA&nbsp;5<br></font>"\
	"<font color=\"#008000\">#define&nbsp;EI_VERSION&nbsp;6<br></font>"\
	"<font color=\"#008000\">#define&nbsp;EI_PAD&nbsp;7<br></font>"\
	"<font color=\"#008000\">#define&nbsp;EI_NIDENT&nbsp;16<br></font>"\
	"<br>"\
	"<font color=\"#008000\">#define&nbsp;ELFMAG0&nbsp;0x7f<br></font>"\
	"<font color=\"#008000\">#define&nbsp;ELFMAG1&nbsp;'E'<br></font>"\
	"<font color=\"#008000\">#define&nbsp;ELFMAG2&nbsp;'L'<br></font>"\
	"<font color=\"#008000\">#define&nbsp;ELFMAG3&nbsp;'F'<br></font>"\
	"<br>"\
	"<font color=\"#008000\">#define&nbsp;ELFCLASSNONE&nbsp;0<br></font>"\
	"<font color=\"#008000\">#define&nbsp;ELFCLASS32&nbsp;1<br></font>"\
	"<font color=\"#008000\">#define&nbsp;ELFCLASS64&nbsp;2<br></font>"\
	"<br>"\
	"<font color=\"#008000\">#define&nbsp;ELFDATANONE&nbsp;0<br></font>"\
	"<font color=\"#008000\">#define&nbsp;ELFDATA2LSB&nbsp;1<br></font>"\
	"<font color=\"#008000\">#define&nbsp;ELFDATA2MSB&nbsp;2<br></font>"\
	"<br>"\
	"<font color=\"#008000\">#define&nbsp;SHT_NULL&nbsp;0<br></font>"\
	"<font color=\"#008000\">#define&nbsp;SHT_PROGBITS&nbsp;1<br></font>"\
	"<font color=\"#008000\">#define&nbsp;SHT_SYMTAB&nbsp;2<br></font>"\
	"<font color=\"#008000\">#define&nbsp;SHT_STRTAB&nbsp;3<br></font>"\
	"<font color=\"#008000\">#define&nbsp;SHT_RELA&nbsp;4<br></font>"\
	"<font color=\"#008000\">#define&nbsp;SHT_HASH&nbsp;5<br></font>"\
	"<font color=\"#008000\">#define&nbsp;SHT_DYNAMIC&nbsp;6<br></font>"\
	"<font color=\"#008000\">#define&nbsp;SHT_NOTE&nbsp;7<br></font>"\
	"<font color=\"#008000\">#define&nbsp;SHT_NOBITS&nbsp;8<br></font>"\
	"<font color=\"#008000\">#define&nbsp;SHT_REL&nbsp;9<br></font>"\
	"<font color=\"#008000\">#define&nbsp;SHT_SHLIB&nbsp;10<br></font>"\
	"<font color=\"#008000\">#define&nbsp;SHT_DYNSYM&nbsp;11<br></font>"\
	"<font color=\"#008000\">#define&nbsp;SHT_LOPROC&nbsp;0x70000000<br></font>"\
	"<font color=\"#008000\">#define&nbsp;SHT_HIPROC&nbsp;0x7fffffff<br></font>"\
	"<font color=\"#008000\">#define&nbsp;SHT_LOUSER&nbsp;0x80000000<br></font>"\
	"<font color=\"#008000\">#define&nbsp;SHT_HIUSER&nbsp;0xffffffff<br></font>"\
	"<br>"\
	"<font color=\"#008000\">#define&nbsp;SHF_WRITE&nbsp;0x1<br></font>"\
	"<font color=\"#008000\">#define&nbsp;SHF_ALLOC&nbsp;0x2<br></font>"\
	"<font color=\"#008000\">#define&nbsp;SHF_EXECINSTR&nbsp;0x4<br></font>"\
	"<font color=\"#808080\"><i>/*<br>"\
	"&nbsp;*&nbsp;ELF&nbsp;header&nbsp;at&nbsp;the&nbsp;beginning&nbsp;of&nbsp;the&nbsp;executable.<br>"\
	"&nbsp;*/</i></font><br>"\
	"<font color=\"#000080\"><b>typedef</b></font>&nbsp;<font color=\"#000080\"><b>struct</b></font>&nbsp;<font color=\"#000000\">_Elf32_Ehdr</font>&nbsp;{<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">char</font>&nbsp;<font color=\"#000000\">e_ident</font>[<font color=\"#000000\">EI_NIDENT</font>];<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#000000\">Elf32_Half</font>&nbsp;<font color=\"#000000\">e_type</font>;<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#000000\">Elf32_Half</font>&nbsp;<font color=\"#000000\">e_machine</font>;<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#000000\">Elf32_Word</font>&nbsp;<font color=\"#000000\">e_version</font>;<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#000000\">Elf32_Addr</font>&nbsp;<font color=\"#000000\">e_entry</font>;<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#000000\">Elf32_Off</font>&nbsp;&nbsp;<font color=\"#000000\">e_phoff</font>;<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#000000\">Elf32_Off</font>&nbsp;&nbsp;<font color=\"#000000\">e_shoff</font>;<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#000000\">Elf32_Word</font>&nbsp;<font color=\"#000000\">e_flags</font>;<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#000000\">Elf32_Half</font>&nbsp;<font color=\"#000000\">e_ehsize</font>;<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#000000\">Elf32_Half</font>&nbsp;<font color=\"#000000\">e_phentsize</font>;<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#000000\">Elf32_Half</font>&nbsp;<font color=\"#000000\">e_phnum</font>;<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#000000\">Elf32_Half</font>&nbsp;<font color=\"#000000\">e_shentsize</font>;<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#000000\">Elf32_Half</font>&nbsp;<font color=\"#000000\">e_shnum</font>;<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#000000\">Elf32_Half</font>&nbsp;<font color=\"#000000\">e_shstrndx</font>;<br>"\
	"}&nbsp;<font color=\"#000000\">Elf32_Ehdr</font>;<br>"\
	"<br>"\
	"<font color=\"#000080\"><b>typedef</b></font>&nbsp;<font color=\"#000080\"><b>struct</b></font>&nbsp;<font color=\"#000000\">s_Elf32_Shdr</font>&nbsp;{<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#000000\">Elf32_Word</font>&nbsp;<font color=\"#000000\">sh_name</font>;<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#000000\">Elf32_Word</font>&nbsp;<font color=\"#000000\">sh_type</font>;<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#000000\">Elf32_Word</font>&nbsp;<font color=\"#000000\">sh_flags</font>;<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#000000\">Elf32_Addr</font>&nbsp;<font color=\"#000000\">sh_addr</font>;<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#000000\">Elf32_Off</font>&nbsp;<font color=\"#000000\">sh_offset</font>;<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#000000\">Elf32_Word</font>&nbsp;<font color=\"#000000\">sh_size</font>;<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#000000\">Elf32_Word</font>&nbsp;<font color=\"#000000\">sh_link</font>;<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#000000\">Elf32_Word</font>&nbsp;<font color=\"#000000\">sh_info</font>;<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#000000\">Elf32_Word</font>&nbsp;<font color=\"#000000\">sh_addralign</font>;<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#000000\">Elf32_Word</font>&nbsp;<font color=\"#000000\">sh_entsize</font>;<br>"\
	"}&nbsp;<font color=\"#000000\">Elf32_Shdr</font>;<br>"\
	"<br>"\
	"<font color=\"#000080\"><b>typedef</b></font>&nbsp;<font color=\"#000080\"><b>struct</b></font>&nbsp;<font color=\"#000000\">s_Elf32_Sym</font>&nbsp;{<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#000000\">Elf32_Word</font>&nbsp;<font color=\"#000000\">st_name</font>;<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#000000\">Elf32_Addr</font>&nbsp;<font color=\"#000000\">st_value</font>;<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#000000\">Elf32_Word</font>&nbsp;<font color=\"#000000\">st_size</font>;<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">char</font>&nbsp;<font color=\"#000000\">st_info</font>;<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">char</font>&nbsp;<font color=\"#000000\">st_other</font>;<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#000000\">Elf32_Half</font>&nbsp;<font color=\"#000000\">st_shndx</font>;<br>"\
	"}&nbsp;<font color=\"#000000\">Elf32_Sym</font>;<br>"\
	"<br>"\
	"<font color=\"#000080\"><b>typedef</b></font>&nbsp;<font color=\"#000080\"><b>struct</b></font>&nbsp;<font color=\"#000000\">s_Elf32_Rel</font>&nbsp;{<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#000000\">Elf32_Addr</font>&nbsp;<font color=\"#000000\">r_offset</font>;<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#000000\">Elf32_Word</font>&nbsp;<font color=\"#000000\">r_info</font>;<br>"\
	"}&nbsp;<font color=\"#000000\">Elf32_Rel</font>;<br>"\
	"<br>"\
	"<font color=\"#000080\"><b>typedef</b></font>&nbsp;<font color=\"#000080\"><b>struct</b></font>&nbsp;<font color=\"#000000\">s_Elf32_Rela</font>&nbsp;{<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#000000\">Elf32_Addr</font>&nbsp;<font color=\"#000000\">r_offset</font>;<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#000000\">Elf32_Word</font>&nbsp;<font color=\"#000000\">r_info</font>;<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#000000\">Elf32_Sword</font>&nbsp;<font color=\"#000000\">r_addend</font>;<br>"\
	"}&nbsp;<font color=\"#000000\">Elf32_Rela</font>;<br>"\
	"<br>"\
	"<br>"\
	"<font color=\"#000080\"><b>typedef</b></font>&nbsp;<font color=\"#000080\"><b>struct</b></font>&nbsp;<font color=\"#000000\">s_elf_exports</font><br>"\
	"{<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#000000\">STRUCT_HEADER_LIST</font>(<font color=\"#000000\">t_elf_exports</font>);<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">name</font>;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#808080\"><i>//&nbsp;e.g.&nbsp;&quot;screenshot_capture&quot;<br></i></font>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">position</font>;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#808080\"><i>//&nbsp;e.g.&nbsp;&quot;.text+0x14&quot;<br></i></font>"\
	"}&nbsp;<font color=\"#000000\">t_elf_exports</font>;<br>"\
	"<br>"\
	"<font color=\"#000080\"><b>typedef</b></font>&nbsp;<font color=\"#000080\"><b>struct</b></font>&nbsp;<font color=\"#000000\">s_elf_imports</font><br>"\
	"{<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#000000\">STRUCT_HEADER_LIST</font>(<font color=\"#000000\">t_elf_imports</font>);<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">name</font>;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#808080\"><i>//&nbsp;e.g.&nbsp;&quot;fs_fopen&quot;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;or&nbsp;&quot;(internal)&quot;<br></i></font>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">type</font>;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#808080\"><i>//&nbsp;e.g.&nbsp;&quot;CALL&quot;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;or&nbsp;&quot;PTR&quot;<br></i></font>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">position</font>;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#808080\"><i>//&nbsp;e.g.&nbsp;&quot;.test+0x18&quot;&nbsp;&nbsp;&nbsp;&nbsp;or&nbsp;&quot;.test+0x18&quot;<br></i></font>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">destination</font>;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#808080\"><i>//&nbsp;e.g.&nbsp;&quot;fs_fopen&quot;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;or&nbsp;&quot;.bss+0x10&quot;<br></i></font>"\
	"}&nbsp;<font color=\"#000000\">t_elf_imports</font>;<br>"\
	"<br>"\
	"<font color=\"#000080\"><b>typedef</b></font>&nbsp;<font color=\"#000080\"><b>struct</b></font>&nbsp;<font color=\"#000000\">s_elf_object</font><br>"\
	"{<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#000000\">STRUCT_HEADER</font>;<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">name</font>;<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#000000\">t_elf_exports</font>&nbsp;*<font color=\"#000000\">exports</font>;<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#000000\">t_elf_imports</font>&nbsp;*<font color=\"#000000\">imports</font>;<br>"\
	"}&nbsp;<font color=\"#000000\">t_elf_object</font>;<br>"\
	"<br>"\
	"		</font>"\


#define ELF_AUTO_MISC_POST_HTML \
	"		<font face=\"Monospace\">"\
	"		</font>"\


