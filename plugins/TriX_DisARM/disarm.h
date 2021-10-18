
unsigned int disarm_init ( );
unsigned int print_instruction ( unsigned char *data, unsigned int offset, void *btree, unsigned int pc, int is_thumb, void *i );
unsigned int disarm_address ( t_workspace *ws, unsigned char *dest, unsigned int address, int is_thumb );

#define DISARM_HEADERS "import unsigned int __disarm_address ( t_workspace *ws, unsigned char *dest, unsigned int address, int is_thumb );\n"\

#define DISARM_SYMBOLS \
	REGISTER_SYMBOL ( disarm_address );\


typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned long  u32;

typedef signed char  s8;
typedef signed short s16;
typedef signed long  s32;


typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned long  u32;

typedef signed char  s8;
typedef signed short s16;
typedef signed long  s32;

