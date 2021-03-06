#include "auto_segment.h"
#include "auto_stage.h"
#include "auto_file_io.h"
#include "auto_file.h"
#include "auto_workspace.h"
#include "auto_util.h"
#include "auto_elf.h"
#include "auto_crypto.h"
#include "auto_trixcrypt.h"
#include "auto_netrix.h"
#include "auto_treenode.h"
#include "auto_ui.h"
#include "auto_fmt.h"
#include "auto_symbols.h"
#include "auto_nokia.h"
#include "auto_options.h"
#include "auto_arm.h"
#include "auto_seer.h"
#include "auto_project.h"
#include "auto_mutex.h"
#include "auto_mem.h"

#define TRIX_AUTO_IMPORT \
	SEGMENT_AUTO_MISC_PRE \
	SEGMENT_AUTO_IMPORTS \
	SEGMENT_AUTO_MISC_POST \
	STAGE_AUTO_MISC_PRE \
	STAGE_AUTO_IMPORTS \
	STAGE_AUTO_MISC_POST \
	FILE_IO_AUTO_MISC_PRE \
	FILE_IO_AUTO_IMPORTS \
	FILE_IO_AUTO_MISC_POST \
	FILE_AUTO_MISC_PRE \
	FILE_AUTO_IMPORTS \
	FILE_AUTO_MISC_POST \
	WORKSPACE_AUTO_MISC_PRE \
	WORKSPACE_AUTO_IMPORTS \
	WORKSPACE_AUTO_MISC_POST \
	UTIL_AUTO_MISC_PRE \
	UTIL_AUTO_IMPORTS \
	UTIL_AUTO_MISC_POST \
	ELF_AUTO_MISC_PRE \
	ELF_AUTO_IMPORTS \
	ELF_AUTO_MISC_POST \
	CRYPTO_AUTO_MISC_PRE \
	CRYPTO_AUTO_IMPORTS \
	CRYPTO_AUTO_MISC_POST \
	TRIXCRYPT_AUTO_MISC_PRE \
	TRIXCRYPT_AUTO_IMPORTS \
	TRIXCRYPT_AUTO_MISC_POST \
	NETRIX_AUTO_MISC_PRE \
	NETRIX_AUTO_IMPORTS \
	NETRIX_AUTO_MISC_POST \
	TREENODE_AUTO_MISC_PRE \
	TREENODE_AUTO_IMPORTS \
	TREENODE_AUTO_MISC_POST \
	UI_AUTO_MISC_PRE \
	UI_AUTO_IMPORTS \
	UI_AUTO_MISC_POST \
	FMT_AUTO_MISC_PRE \
	FMT_AUTO_IMPORTS \
	FMT_AUTO_MISC_POST \
	SYMBOLS_AUTO_MISC_PRE \
	SYMBOLS_AUTO_IMPORTS \
	SYMBOLS_AUTO_MISC_POST \
	NOKIA_AUTO_MISC_PRE \
	NOKIA_AUTO_IMPORTS \
	NOKIA_AUTO_MISC_POST \
	OPTIONS_AUTO_MISC_PRE \
	OPTIONS_AUTO_IMPORTS \
	OPTIONS_AUTO_MISC_POST \
	ARM_AUTO_MISC_PRE \
	ARM_AUTO_IMPORTS \
	ARM_AUTO_MISC_POST \
	SEER_AUTO_MISC_PRE \
	SEER_AUTO_IMPORTS \
	SEER_AUTO_MISC_POST \
	PROJECT_AUTO_MISC_PRE \
	PROJECT_AUTO_IMPORTS \
	PROJECT_AUTO_MISC_POST \
	MUTEX_AUTO_MISC_PRE \
	MUTEX_AUTO_IMPORTS \
	MUTEX_AUTO_MISC_POST \
	MEM_AUTO_MISC_PRE \
	MEM_AUTO_IMPORTS \
	MEM_AUTO_MISC_POST \


#define TRIX_AUTO_REGISTER \
	SEGMENT_AUTO_REGISTER \
	STAGE_AUTO_REGISTER \
	FILE_IO_AUTO_REGISTER \
	FILE_AUTO_REGISTER \
	WORKSPACE_AUTO_REGISTER \
	UTIL_AUTO_REGISTER \
	ELF_AUTO_REGISTER \
	CRYPTO_AUTO_REGISTER \
	TRIXCRYPT_AUTO_REGISTER \
	NETRIX_AUTO_REGISTER \
	TREENODE_AUTO_REGISTER \
	UI_AUTO_REGISTER \
	FMT_AUTO_REGISTER \
	SYMBOLS_AUTO_REGISTER \
	NOKIA_AUTO_REGISTER \
	OPTIONS_AUTO_REGISTER \
	ARM_AUTO_REGISTER \
	SEER_AUTO_REGISTER \
	PROJECT_AUTO_REGISTER \
	MUTEX_AUTO_REGISTER \
	MEM_AUTO_REGISTER \



#define HEADER_DEFINE(x) char *x[] = { "segment.h", "Definitions:\n" SEGMENT_AUTO_MISC_PRE "\n\nDeclarations:\n" SEGMENT_AUTO_IMPORTS "\n\nImplementations:\n" SEGMENT_AUTO_MISC_POST "\n\n", "<h1>Definitions:</h1><br>" SEGMENT_AUTO_MISC_PRE_HTML "<br><hr><h1>Declarations:</h1><br>" SEGMENT_AUTO_IMPORTS_HTML "<br><hr><h1>Implementations:</h1><br>" SEGMENT_AUTO_MISC_POST_HTML "<br><hr>", "stage.h", "Definitions:\n" STAGE_AUTO_MISC_PRE "\n\nDeclarations:\n" STAGE_AUTO_IMPORTS "\n\nImplementations:\n" STAGE_AUTO_MISC_POST "\n\n", "<h1>Definitions:</h1><br>" STAGE_AUTO_MISC_PRE_HTML "<br><hr><h1>Declarations:</h1><br>" STAGE_AUTO_IMPORTS_HTML "<br><hr><h1>Implementations:</h1><br>" STAGE_AUTO_MISC_POST_HTML "<br><hr>", "file_io.h", "Definitions:\n" FILE_IO_AUTO_MISC_PRE "\n\nDeclarations:\n" FILE_IO_AUTO_IMPORTS "\n\nImplementations:\n" FILE_IO_AUTO_MISC_POST "\n\n", "<h1>Definitions:</h1><br>" FILE_IO_AUTO_MISC_PRE_HTML "<br><hr><h1>Declarations:</h1><br>" FILE_IO_AUTO_IMPORTS_HTML "<br><hr><h1>Implementations:</h1><br>" FILE_IO_AUTO_MISC_POST_HTML "<br><hr>", "file.h", "Definitions:\n" FILE_AUTO_MISC_PRE "\n\nDeclarations:\n" FILE_AUTO_IMPORTS "\n\nImplementations:\n" FILE_AUTO_MISC_POST "\n\n", "<h1>Definitions:</h1><br>" FILE_AUTO_MISC_PRE_HTML "<br><hr><h1>Declarations:</h1><br>" FILE_AUTO_IMPORTS_HTML "<br><hr><h1>Implementations:</h1><br>" FILE_AUTO_MISC_POST_HTML "<br><hr>", "workspace.h", "Definitions:\n" WORKSPACE_AUTO_MISC_PRE "\n\nDeclarations:\n" WORKSPACE_AUTO_IMPORTS "\n\nImplementations:\n" WORKSPACE_AUTO_MISC_POST "\n\n", "<h1>Definitions:</h1><br>" WORKSPACE_AUTO_MISC_PRE_HTML "<br><hr><h1>Declarations:</h1><br>" WORKSPACE_AUTO_IMPORTS_HTML "<br><hr><h1>Implementations:</h1><br>" WORKSPACE_AUTO_MISC_POST_HTML "<br><hr>", "util.h", "Definitions:\n" UTIL_AUTO_MISC_PRE "\n\nDeclarations:\n" UTIL_AUTO_IMPORTS "\n\nImplementations:\n" UTIL_AUTO_MISC_POST "\n\n", "<h1>Definitions:</h1><br>" UTIL_AUTO_MISC_PRE_HTML "<br><hr><h1>Declarations:</h1><br>" UTIL_AUTO_IMPORTS_HTML "<br><hr><h1>Implementations:</h1><br>" UTIL_AUTO_MISC_POST_HTML "<br><hr>", "elf.h", "Definitions:\n" ELF_AUTO_MISC_PRE "\n\nDeclarations:\n" ELF_AUTO_IMPORTS "\n\nImplementations:\n" ELF_AUTO_MISC_POST "\n\n", "<h1>Definitions:</h1><br>" ELF_AUTO_MISC_PRE_HTML "<br><hr><h1>Declarations:</h1><br>" ELF_AUTO_IMPORTS_HTML "<br><hr><h1>Implementations:</h1><br>" ELF_AUTO_MISC_POST_HTML "<br><hr>", "crypto.h", "Definitions:\n" CRYPTO_AUTO_MISC_PRE "\n\nDeclarations:\n" CRYPTO_AUTO_IMPORTS "\n\nImplementations:\n" CRYPTO_AUTO_MISC_POST "\n\n", "<h1>Definitions:</h1><br>" CRYPTO_AUTO_MISC_PRE_HTML "<br><hr><h1>Declarations:</h1><br>" CRYPTO_AUTO_IMPORTS_HTML "<br><hr><h1>Implementations:</h1><br>" CRYPTO_AUTO_MISC_POST_HTML "<br><hr>", "trixcrypt.h", "Definitions:\n" TRIXCRYPT_AUTO_MISC_PRE "\n\nDeclarations:\n" TRIXCRYPT_AUTO_IMPORTS "\n\nImplementations:\n" TRIXCRYPT_AUTO_MISC_POST "\n\n", "<h1>Definitions:</h1><br>" TRIXCRYPT_AUTO_MISC_PRE_HTML "<br><hr><h1>Declarations:</h1><br>" TRIXCRYPT_AUTO_IMPORTS_HTML "<br><hr><h1>Implementations:</h1><br>" TRIXCRYPT_AUTO_MISC_POST_HTML "<br><hr>", "netrix.h", "Definitions:\n" NETRIX_AUTO_MISC_PRE "\n\nDeclarations:\n" NETRIX_AUTO_IMPORTS "\n\nImplementations:\n" NETRIX_AUTO_MISC_POST "\n\n", "<h1>Definitions:</h1><br>" NETRIX_AUTO_MISC_PRE_HTML "<br><hr><h1>Declarations:</h1><br>" NETRIX_AUTO_IMPORTS_HTML "<br><hr><h1>Implementations:</h1><br>" NETRIX_AUTO_MISC_POST_HTML "<br><hr>", "treenode.h", "Definitions:\n" TREENODE_AUTO_MISC_PRE "\n\nDeclarations:\n" TREENODE_AUTO_IMPORTS "\n\nImplementations:\n" TREENODE_AUTO_MISC_POST "\n\n", "<h1>Definitions:</h1><br>" TREENODE_AUTO_MISC_PRE_HTML "<br><hr><h1>Declarations:</h1><br>" TREENODE_AUTO_IMPORTS_HTML "<br><hr><h1>Implementations:</h1><br>" TREENODE_AUTO_MISC_POST_HTML "<br><hr>", "ui.h", "Definitions:\n" UI_AUTO_MISC_PRE "\n\nDeclarations:\n" UI_AUTO_IMPORTS "\n\nImplementations:\n" UI_AUTO_MISC_POST "\n\n", "<h1>Definitions:</h1><br>" UI_AUTO_MISC_PRE_HTML "<br><hr><h1>Declarations:</h1><br>" UI_AUTO_IMPORTS_HTML "<br><hr><h1>Implementations:</h1><br>" UI_AUTO_MISC_POST_HTML "<br><hr>", "fmt.h", "Definitions:\n" FMT_AUTO_MISC_PRE "\n\nDeclarations:\n" FMT_AUTO_IMPORTS "\n\nImplementations:\n" FMT_AUTO_MISC_POST "\n\n", "<h1>Definitions:</h1><br>" FMT_AUTO_MISC_PRE_HTML "<br><hr><h1>Declarations:</h1><br>" FMT_AUTO_IMPORTS_HTML "<br><hr><h1>Implementations:</h1><br>" FMT_AUTO_MISC_POST_HTML "<br><hr>", "symbols.h", "Definitions:\n" SYMBOLS_AUTO_MISC_PRE "\n\nDeclarations:\n" SYMBOLS_AUTO_IMPORTS "\n\nImplementations:\n" SYMBOLS_AUTO_MISC_POST "\n\n", "<h1>Definitions:</h1><br>" SYMBOLS_AUTO_MISC_PRE_HTML "<br><hr><h1>Declarations:</h1><br>" SYMBOLS_AUTO_IMPORTS_HTML "<br><hr><h1>Implementations:</h1><br>" SYMBOLS_AUTO_MISC_POST_HTML "<br><hr>", "nokia.h", "Definitions:\n" NOKIA_AUTO_MISC_PRE "\n\nDeclarations:\n" NOKIA_AUTO_IMPORTS "\n\nImplementations:\n" NOKIA_AUTO_MISC_POST "\n\n", "<h1>Definitions:</h1><br>" NOKIA_AUTO_MISC_PRE_HTML "<br><hr><h1>Declarations:</h1><br>" NOKIA_AUTO_IMPORTS_HTML "<br><hr><h1>Implementations:</h1><br>" NOKIA_AUTO_MISC_POST_HTML "<br><hr>", "options.h", "Definitions:\n" OPTIONS_AUTO_MISC_PRE "\n\nDeclarations:\n" OPTIONS_AUTO_IMPORTS "\n\nImplementations:\n" OPTIONS_AUTO_MISC_POST "\n\n", "<h1>Definitions:</h1><br>" OPTIONS_AUTO_MISC_PRE_HTML "<br><hr><h1>Declarations:</h1><br>" OPTIONS_AUTO_IMPORTS_HTML "<br><hr><h1>Implementations:</h1><br>" OPTIONS_AUTO_MISC_POST_HTML "<br><hr>", "arm.h", "Definitions:\n" ARM_AUTO_MISC_PRE "\n\nDeclarations:\n" ARM_AUTO_IMPORTS "\n\nImplementations:\n" ARM_AUTO_MISC_POST "\n\n", "<h1>Definitions:</h1><br>" ARM_AUTO_MISC_PRE_HTML "<br><hr><h1>Declarations:</h1><br>" ARM_AUTO_IMPORTS_HTML "<br><hr><h1>Implementations:</h1><br>" ARM_AUTO_MISC_POST_HTML "<br><hr>", "seer.h", "Definitions:\n" SEER_AUTO_MISC_PRE "\n\nDeclarations:\n" SEER_AUTO_IMPORTS "\n\nImplementations:\n" SEER_AUTO_MISC_POST "\n\n", "<h1>Definitions:</h1><br>" SEER_AUTO_MISC_PRE_HTML "<br><hr><h1>Declarations:</h1><br>" SEER_AUTO_IMPORTS_HTML "<br><hr><h1>Implementations:</h1><br>" SEER_AUTO_MISC_POST_HTML "<br><hr>", "project.h", "Definitions:\n" PROJECT_AUTO_MISC_PRE "\n\nDeclarations:\n" PROJECT_AUTO_IMPORTS "\n\nImplementations:\n" PROJECT_AUTO_MISC_POST "\n\n", "<h1>Definitions:</h1><br>" PROJECT_AUTO_MISC_PRE_HTML "<br><hr><h1>Declarations:</h1><br>" PROJECT_AUTO_IMPORTS_HTML "<br><hr><h1>Implementations:</h1><br>" PROJECT_AUTO_MISC_POST_HTML "<br><hr>", "mutex.h", "Definitions:\n" MUTEX_AUTO_MISC_PRE "\n\nDeclarations:\n" MUTEX_AUTO_IMPORTS "\n\nImplementations:\n" MUTEX_AUTO_MISC_POST "\n\n", "<h1>Definitions:</h1><br>" MUTEX_AUTO_MISC_PRE_HTML "<br><hr><h1>Declarations:</h1><br>" MUTEX_AUTO_IMPORTS_HTML "<br><hr><h1>Implementations:</h1><br>" MUTEX_AUTO_MISC_POST_HTML "<br><hr>", "mem.h", "Definitions:\n" MEM_AUTO_MISC_PRE "\n\nDeclarations:\n" MEM_AUTO_IMPORTS "\n\nImplementations:\n" MEM_AUTO_MISC_POST "\n\n", "<h1>Definitions:</h1><br>" MEM_AUTO_MISC_PRE_HTML "<br><hr><h1>Declarations:</h1><br>" MEM_AUTO_IMPORTS_HTML "<br><hr><h1>Implementations:</h1><br>" MEM_AUTO_MISC_POST_HTML "<br><hr>", NULL } 

