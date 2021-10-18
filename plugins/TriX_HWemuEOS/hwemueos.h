#ifndef __TRIX_HWEMUEOS_H__
#define __TRIX_HWEMUEOS_H__

#include "trixplug.h"

TRIXPLUGIN_API unsigned int trixplugin_init ( struct trix_functions *callbacks );
TRIXPLUGIN_API char *trixplugin_name ( void );
TRIXPLUGIN_API unsigned int trixplugin_vers ( void );

unsigned int hwemueos_init ( );
unsigned int hwemueos_reset ( );
unsigned int hwemueos_initdialog ( );
unsigned int hwemueos_get_exec_callback ( );
unsigned int hwemueos_register ( t_workspace *ws, unsigned int address );
unsigned int hwemueos_handler ( t_workspace *ws, unsigned int address, unsigned char type, unsigned int value );
unsigned int hwemueos_default_handle ( t_workspace *ws, unsigned int address, unsigned char type, unsigned int value );
unsigned int hwemueos_print_trace ();
int *hwemueos_tio_get_rx_ptr ();

typedef struct 
{
	unsigned char *name;
	unsigned int start;
	unsigned int end;
	unsigned int (*handle) ( unsigned int parm, t_workspace *ws, unsigned int address, unsigned char type, unsigned int value );
	unsigned int parm;
} t_hwemueos_handler;

#ifndef __TRIX_HWEMUEOS_C__
extern int verbosity;
#endif

unsigned int hwemueos_handle_rom ( unsigned int parm, t_workspace *ws, unsigned int address, unsigned char type, unsigned int value );
unsigned int hwemueos_handle_flashctrl ( unsigned int parm, t_workspace *ws, unsigned int address, unsigned char type, unsigned int value );
unsigned int hwemueos_handle_dma ( unsigned int parm, t_workspace *ws, unsigned int address, unsigned char type, unsigned int value );
unsigned int hwemueos_handle_ram ( unsigned int parm, t_workspace *ws, unsigned int address, unsigned char type, unsigned int value );
unsigned int hwemueos_handle_sio ( unsigned int parm, t_workspace *ws, unsigned int address, unsigned char type, unsigned int value );
unsigned int hwemueos_handle_cartridge ( unsigned int parm, t_workspace *ws, unsigned int address, unsigned char type, unsigned int value );
unsigned int hwemueos_handle_tio ( unsigned int parm, t_workspace *ws, unsigned int address, unsigned char type, unsigned int value );
unsigned int hwemueos_handle_timers ( unsigned int parm, t_workspace *ws, unsigned int address, unsigned char type, unsigned int value );
unsigned int hwemueos_handle_timers_ ( unsigned int parm, t_workspace *ws, unsigned int address, unsigned char type, unsigned int value );
unsigned int hwemueos_handle_intengine ( unsigned int parm, t_workspace *ws, unsigned int address, unsigned char type, unsigned int value );
unsigned int hwemueos_handle_basic ( unsigned int parm, t_workspace *ws, unsigned int address, unsigned char type, unsigned int value );
unsigned int hwemueos_handle_unk ( unsigned int parm, t_workspace *ws, unsigned int address, unsigned char type, unsigned int value );
unsigned int hwemueos_handle_gpio ( unsigned int parm, t_workspace *ws, unsigned int address, unsigned char type, unsigned int value );
unsigned int hwemueos_handle_sdio ( unsigned int parm, t_workspace *ws, unsigned int address, unsigned char type, unsigned int value );

#define HWEMUEOS_HEADERS \
	"import unsigned int __hwemueos_register ( t_workspace *ws, unsigned int address );\n" \
	"import unsigned int __hwemueos_get_exec_callback ();\n" \
	"import unsigned int __hwemueos_print_trace ();\n" \
	"import unsigned int __hwemueos_reset ();\n" \
	"import unsigned int __hwemueos_initdialog ();\n" \
	"import int *__hwemueos_tio_get_rx_ptr ();\n" \


#define HWEMUEOS_SYMBOLS \
	REGISTER_SYMBOL ( hwemueos_register );\
	REGISTER_SYMBOL ( hwemueos_handler );\
	REGISTER_SYMBOL ( hwemueos_get_exec_callback );\
	REGISTER_SYMBOL ( hwemueos_print_trace );\
	REGISTER_SYMBOL ( hwemueos_reset );\
	REGISTER_SYMBOL ( hwemueos_tio_get_rx_ptr );\
	REGISTER_SYMBOL ( hwemueos_initdialog );\


#define PC_TRACE_ENTRIES 8192
#define INT_ENTRIES 0x100

#define ROM_START 0xff810000
#define ROM_SIZE 0x7f0000
#define KERNEL_LOAD_ADDR 0x00010000

#define IO_MEM_START 0xc0000000
#define IO_MEM_LEN 0xa00000

#define ARMULATE_CB_NO_ACTION    0x1000
#define ARMULATE_CB_IRQ          0x1001
#define ARMULATE_CB_FIQ          0x1002


// iomem: 0xc0000000 
//               0xc0200000
//               0xc0400000
//               0xc0500000
//        0xc0800000
//        0xc0900000
// 64MB
// #define RAM_SIZE = 0x4000000
//
#endif

