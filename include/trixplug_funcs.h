
/* trixplug function structs */

struct trix_functions
{
    int version;
    int (*debug_msg) ( int level, char *str, ... );
    int (*error_msg) ( int level, char *str, ... );
    int (*print_msg) ( char *str, ... );
#ifdef __TRIX_TRIXPLUG_TRIXPLUG_H__
#define TRIXPLUG_FUNC_TYPE struct trixplug_funcs *
#else
#define TRIXPLUG_FUNC_TYPE void *
#endif
	TRIXPLUG_FUNC_TYPE	 trixplug;
#ifdef __TRIX_TRIXPLUG_SEGMENT_H__
#define SEGMENT_FUNC_TYPE struct segment_funcs *
#else
#define SEGMENT_FUNC_TYPE void *
#endif
	SEGMENT_FUNC_TYPE	 segment;
#ifdef __TRIX_TRIXPLUG_STAGE_H__
#define STAGE_FUNC_TYPE struct stage_funcs *
#else
#define STAGE_FUNC_TYPE void *
#endif
	STAGE_FUNC_TYPE	 stage;
#ifdef __TRIX_TRIXPLUG_FILE_IO_H__
#define FILE_IO_FUNC_TYPE struct file_io_funcs *
#else
#define FILE_IO_FUNC_TYPE void *
#endif
	FILE_IO_FUNC_TYPE	 file_io;
#ifdef __TRIX_TRIXPLUG_FILE_H__
#define FILE_FUNC_TYPE struct file_funcs *
#else
#define FILE_FUNC_TYPE void *
#endif
	FILE_FUNC_TYPE	 file;
#ifdef __TRIX_TRIXPLUG_WORKSPACE_H__
#define WORKSPACE_FUNC_TYPE struct workspace_funcs *
#else
#define WORKSPACE_FUNC_TYPE void *
#endif
	WORKSPACE_FUNC_TYPE	 workspace;
#ifdef __TRIX_TRIXPLUG_UTIL_H__
#define UTIL_FUNC_TYPE struct util_funcs *
#else
#define UTIL_FUNC_TYPE void *
#endif
	UTIL_FUNC_TYPE	 util;
#ifdef __TRIX_TRIXPLUG_ELF_H__
#define ELF_FUNC_TYPE struct elf_funcs *
#else
#define ELF_FUNC_TYPE void *
#endif
	ELF_FUNC_TYPE	 elf;
#ifdef __TRIX_TRIXPLUG_CRYPTO_H__
#define CRYPTO_FUNC_TYPE struct crypto_funcs *
#else
#define CRYPTO_FUNC_TYPE void *
#endif
	CRYPTO_FUNC_TYPE	 crypto;
#ifdef __TRIX_TRIXPLUG_TRIXCRYPT_H__
#define TRIXCRYPT_FUNC_TYPE struct trixcrypt_funcs *
#else
#define TRIXCRYPT_FUNC_TYPE void *
#endif
	TRIXCRYPT_FUNC_TYPE	 trixcrypt;
#ifdef __TRIX_TRIXPLUG_NETRIX_H__
#define NETRIX_FUNC_TYPE struct netrix_funcs *
#else
#define NETRIX_FUNC_TYPE void *
#endif
	NETRIX_FUNC_TYPE	 netrix;
#ifdef __TRIX_TRIXPLUG_TREENODE_H__
#define TREENODE_FUNC_TYPE struct treenode_funcs *
#else
#define TREENODE_FUNC_TYPE void *
#endif
	TREENODE_FUNC_TYPE	 treenode;
#ifdef __TRIX_TRIXPLUG_UI_H__
#define UI_FUNC_TYPE struct ui_funcs *
#else
#define UI_FUNC_TYPE void *
#endif
	UI_FUNC_TYPE	 ui;
#ifdef __TRIX_TRIXPLUG_FMT_H__
#define FMT_FUNC_TYPE struct fmt_funcs *
#else
#define FMT_FUNC_TYPE void *
#endif
	FMT_FUNC_TYPE	 fmt;
#ifdef __TRIX_TRIXPLUG_SYMBOLS_H__
#define SYMBOLS_FUNC_TYPE struct symbols_funcs *
#else
#define SYMBOLS_FUNC_TYPE void *
#endif
	SYMBOLS_FUNC_TYPE	 symbols;
#ifdef __TRIX_TRIXPLUG_NOKIA_H__
#define NOKIA_FUNC_TYPE struct nokia_funcs *
#else
#define NOKIA_FUNC_TYPE void *
#endif
	NOKIA_FUNC_TYPE	 nokia;
#ifdef __TRIX_TRIXPLUG_OPTIONS_H__
#define OPTIONS_FUNC_TYPE struct options_funcs *
#else
#define OPTIONS_FUNC_TYPE void *
#endif
	OPTIONS_FUNC_TYPE	 options;
#ifdef __TRIX_TRIXPLUG_ARM_H__
#define ARM_FUNC_TYPE struct arm_funcs *
#else
#define ARM_FUNC_TYPE void *
#endif
	ARM_FUNC_TYPE	 arm;
#ifdef __TRIX_TRIXPLUG_SEER_H__
#define SEER_FUNC_TYPE struct seer_funcs *
#else
#define SEER_FUNC_TYPE void *
#endif
	SEER_FUNC_TYPE	 seer;
#ifdef __TRIX_TRIXPLUG_PROJECT_H__
#define PROJECT_FUNC_TYPE struct project_funcs *
#else
#define PROJECT_FUNC_TYPE void *
#endif
	PROJECT_FUNC_TYPE	 project;
#ifdef __TRIX_TRIXPLUG_MUTEX_H__
#define MUTEX_FUNC_TYPE struct mutex_funcs *
#else
#define MUTEX_FUNC_TYPE void *
#endif
	MUTEX_FUNC_TYPE	 mutex;
#ifdef __TRIX_TRIXPLUG_MEM_H__
#define MEM_FUNC_TYPE struct mem_funcs *
#else
#define MEM_FUNC_TYPE void *
#endif
	MEM_FUNC_TYPE	 mem;
};

#define TRIXPLUG_INIT_ALL \
	trixplug_plug_init ();\
	segment_plug_init ();\
	stage_plug_init ();\
	file_io_plug_init ();\
	file_plug_init ();\
	workspace_plug_init ();\
	util_plug_init ();\
	elf_plug_init ();\
	crypto_plug_init ();\
	trixcrypt_plug_init ();\
	netrix_plug_init ();\
	treenode_plug_init ();\
	ui_plug_init ();\
	fmt_plug_init ();\
	symbols_plug_init ();\
	nokia_plug_init ();\
	options_plug_init ();\
	arm_plug_init ();\
	seer_plug_init ();\
	project_plug_init ();\
	mutex_plug_init ();\
	mem_plug_init ();\



