#ifndef __TRIX_FATE_H__
#define __TRIX_FATE_H__

#include "trixplug.h"

TRIXPLUGIN_API unsigned int trixplugin_init ( struct trix_functions *callbacks );
TRIXPLUGIN_API char *trixplugin_name ( void );
TRIXPLUGIN_API unsigned int trixplugin_vers ( void );

#define TYPE_DIR  0
#define TYPE_FILE 1


#define FATE_HEADERS \
	"#define TYPE_DIR  0\n"\
	"#define TYPE_FILE 1\n"\
	"\n"\
	"#define PART_TYPE_UNKNOWN		0x00\n"\
	"#define PART_TYPE_FAT12		0x01\n"\
	"#define PART_TYPE_XENIX		0x02\n"\
	"#define PART_TYPE_DOSFAT16		0x04\n"\
	"#define PART_TYPE_EXTDOS		0x05\n"\
	"#define PART_TYPE_FAT16		0x06\n"\
	"#define PART_TYPE_NTFS			0x07\n"\
	"#define PART_TYPE_FAT32		0x0B\n"\
	"#define PART_TYPE_FAT32LBA		0x0C\n"\
	"#define PART_TYPE_FAT16LBA		0x0E\n"\
	"#define PART_TYPE_EXTDOSLBA	0x0F\n"\
	"#define PART_TYPE_ONTRACK		0x33\n"\
	"#define PART_TYPE_NOVELL		0x40\n"\
	"#define PART_TYPE_PCIX			0x4B\n"\
	"#define PART_TYPE_PHOENIXSAVE	0xA0\n"\
	"#define PART_TYPE_CPM			0xDB\n"\
	"#define PART_TYPE_DBFS			0xE0\n"\
	"#define PART_TYPE_BBT			0xFF\n"\
	"#define SEEK_SET				0x00\n"\
	"#define SEEK_CUR				0x01\n"\
	"#define SEEK_END				0x02\n"\
	"\n"\
	"typedef void TFILE;\n"\
	"typedef void t_fat_info;\n"\
	"\n"\
	"import unsigned int __fate_test ( t_workspace *ws );\n"\
	"import unsigned int __fate_dir ( t_fat_info *info );\n"\
	"import t_fat_info *__fate_open ( t_workspace *ws, unsigned int start_address );\n"\
	"import TFILE *__fate_fcreate ( t_fat_info *info, unsigned char *name );\n"\
	"import TFILE *__fate_fopen ( t_fat_info *info, unsigned char *name );\n"\
	"import unsigned int __fate_unlink ( t_fat_info *info, unsigned char *name );\n"\
	"import unsigned int __fate_feof ( TFILE *file );\n"\
	"import unsigned int __fate_fclose ( TFILE *file );\n"\
	"import unsigned int __fate_fputc ( TFILE *file, unsigned char c );\n"\
	"import unsigned char __fate_fgetc ( TFILE *file );\n"\
	"import unsigned int __fate_fseek ( TFILE *fp, unsigned int offset, unsigned char mode );\n"\
	"import unsigned int __fate_fread ( unsigned char * ptr, unsigned int size, unsigned int count, TFILE *fp );\n"\
	"import unsigned int __fate_fwrite ( unsigned char * ptr, unsigned int size, unsigned int count, TFILE *fp );\n"\
	"import unsigned int __fate_cd (  t_fat_info *info, unsigned char *name );\n"\
	"import unsigned int __fate_rename ( t_fat_info *info, char *old_name, char *new_name );\n"\
	"import unsigned char *__fate_get_name( t_fat_info *info, unsigned char type, unsigned int entry_index );\n"\
	"import void __fate_free_name( unsigned char *name );\n"\


#define FATE_SYMBOLS \
	REGISTER_SYMBOL ( fate_open );\
	REGISTER_SYMBOL ( fate_fcreate );\
	REGISTER_SYMBOL ( fate_fopen );\
	REGISTER_SYMBOL ( fate_feof );\
	REGISTER_SYMBOL ( fate_fclose );\
	REGISTER_SYMBOL ( fate_fputc );\
	REGISTER_SYMBOL ( fate_fgetc );\
	REGISTER_SYMBOL ( fate_fseek );\
	REGISTER_SYMBOL ( fate_fread );\
	REGISTER_SYMBOL ( fate_fwrite );\
	REGISTER_SYMBOL ( fate_dir );\
	REGISTER_SYMBOL ( fate_cd );\
	REGISTER_SYMBOL ( fate_rename );\
	REGISTER_SYMBOL ( fate_unlink );\
	REGISTER_SYMBOL ( fate_get_name );\
	REGISTER_SYMBOL ( fate_free_name );\


unsigned int fate_test ( t_workspace *ws );
unsigned int fate_init ( );
t_fat_info *fate_open ( t_workspace *ws, unsigned int start_address );
TFILE *fate_fcreate ( t_fat_info *info, unsigned char *name );
TFILE *fate_fopen ( t_fat_info *info, unsigned char *name );
unsigned int fate_unlink ( t_fat_info *info, unsigned char *name );
unsigned int fate_feof ( TFILE *file );
unsigned int fate_fclose ( TFILE *file );
unsigned int fate_fputc ( TFILE *file, unsigned char c );
unsigned char fate_fgetc ( TFILE *file );
unsigned int fate_fseek ( TFILE *fp, unsigned int offset, unsigned char mode );
unsigned int fate_fread ( unsigned char * ptr, unsigned int size, unsigned int count, TFILE *fp );
unsigned int fate_fwrite ( unsigned char * ptr, unsigned int size, unsigned int count, TFILE *fp );
unsigned int fate_dir ( t_fat_info *info );
unsigned int fate_cd (  t_fat_info *info, unsigned char *name );
unsigned int fate_rename ( t_fat_info *info, char *old_name, char *new_name );
unsigned char *fate_get_name( t_fat_info *info, unsigned char type, unsigned int entry_index );
void fate_free_name( unsigned char *name );


#endif

