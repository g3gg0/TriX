

#define ARMULATE_BP_COUNT  32
#define ARMULATE_BP_EXEC   1
#define ARMULATE_BP_READ   2
#define ARMULATE_BP_WRITE  4

typedef struct
{
    unsigned int address;
    unsigned int flags;
} breakpoint_t;


unsigned int armulate_init ( );
unsigned int armulate_cleanup ( );

ARM7TDMI *init_emu (void);
void free_emu ();
int exec_step (void);
int exec_next (void);
int run_breakpoint (unsigned long);
void set_reg (unsigned long index, unsigned long value);
unsigned long get_reg (unsigned long index);
int is_thumb_state();
u32 get_cpsr();
void set_cpsr (u32 value);


unsigned int armulate_setup ( t_workspace *ws, unsigned int start_address );
unsigned int armulate_run_until ( unsigned int address );

unsigned int armulate_get_reg ( unsigned int reg );
void armulate_set_reg ( unsigned int reg, unsigned int value );
unsigned int armulate_get_cpsr ( void );
void armulate_set_cpsr ( unsigned int value );
unsigned int armulate_exec_step ( void );
unsigned int armulate_exec_next ( void );
unsigned int armulate_get_pc ( void );
unsigned int armulate_is_thumb ( void );
unsigned int armulate_gdb_listen ( void );
unsigned int armulate_add_bp ( unsigned int address );
unsigned int armulate_set_bps ( unsigned int *bps );
unsigned int armulate_is_bp_flag ( unsigned int address, unsigned int flags );
unsigned int armulate_is_bp ( unsigned int address );
const char *armulate_reason ( int reason );
unsigned int armulate_set_pc ( unsigned int address );

unsigned int armulate_get_abort_address ( );
unsigned int armulate_get_abort_memory ( );
unsigned int armulate_get_abort_value ( );
unsigned int armulate_get_abort_width ( );
unsigned int armulate_get_abort_override ( );
unsigned int armulate_get_abort_situation ( );
void armulate_set_abort_address ( unsigned int val );
void armulate_set_abort_memory ( unsigned int val );
void armulate_set_abort_value ( unsigned int val );
void armulate_set_abort_width ( unsigned int val );
void armulate_set_abort_override ( unsigned int val );
void armulate_set_abort_situation ( unsigned int val );
void armulate_set_exec_callback (unsigned int (*cb)(unsigned int) );
void armulate_lr_dump(unsigned int count);

//
// SEE ALSO emu.h !!!!!
//
#define ARMULATE_EXECUTED        0x01
#define ARMULATE_BP_REACHED      0x02
#define ARMULATE_SKIPPED         0x03
#define ARMULATE_INV_INSTR       0x04
#define ARMULATE_STOP_REQUEST    0x05
#define ARMULATE_TIMED_OUT       0x06
#define ARMULATE_DEADLOCK        0x07
#define ARMULATE_EXCEPTION       0x08

#define ARMULATE_ABORT_MEMORY_RD	 0x01
#define ARMULATE_ABORT_MEMORY_WR	 0x02
#define ARMULATE_ABORT_MEMORY_EX	 0x03
#define ARMULATE_ABORT_UNKNOWN_INSTR 0x04
#define ARMULATE_ABORT_INV_MODE      0x05

#define ARMULATE_VERB_ALWAYS  2
#define ARMULATE_VERB_ONERR   1
#define ARMULATE_VERB_NONE    0

#define ARMULATE_HEADERS "import unsigned int __armulate_setup ( t_workspace *ws, unsigned int start_address );\n"\
	"import unsigned int __armulate_run_until ( unsigned int address );\n"\
	"import unsigned int __armulate_get_reg ( unsigned int reg );\n"\
	"import void __armulate_set_reg ( unsigned int reg, unsigned int value );\n"\
	"import unsigned int __armulate_get_cpsr (  );\n"\
	"import void __armulate_set_cpsr ( unsigned int value );\n"\
	"import unsigned int __armulate_exec_step (  );\n"\
	"import unsigned int __armulate_exec_next (  );\n"\
	"import unsigned int __armulate_get_pc (  );\n"\
	"import unsigned int __armulate_set_pc ( unsigned int address );\n"\
	"import unsigned int __armulate_is_thumb ( );\n"\
	"import unsigned int __armulate_add_bp ( unsigned int address );\n"\
	"import unsigned int __armulate_set_bps ( unsigned int *bps );\n"\
	"import unsigned int __armulate_is_bp ( unsigned int address );\n"\
	"import unsigned int __armulate_gdb_listen (  );\n"\
	"import const char *__armulate_reason ( int reason );\n"\
	"import unsigned int __armulate_get_abort_address ( );\n"\
	"import unsigned int __armulate_get_abort_memory ( );\n"\
	"import unsigned int __armulate_get_abort_value ( );\n"\
	"import unsigned int __armulate_get_abort_width ( );\n"\
	"import unsigned int __armulate_get_abort_override ( );\n"\
	"import unsigned int __armulate_get_abort_situation ( );\n"\
	"import void __armulate_set_abort_address ( unsigned int val );\n"\
	"import void __armulate_set_abort_memory ( unsigned int val );\n"\
	"import void __armulate_set_abort_value ( unsigned int val );\n"\
	"import void __armulate_set_abort_width ( unsigned int val );\n"\
	"import void __armulate_set_abort_override ( unsigned int val );\n"\
	"import void __armulate_set_abort_situation ( unsigned int val );\n"\
	"import void __armulate_set_exec_callback (unsigned int cb);\n"\
	"import void __armulate_lr_dump(unsigned int count);\n"\
	"#define ARMULATE_EXECUTED        0x01\n"\
	"#define ARMULATE_BP_REACHED      0x02\n"\
	"#define ARMULATE_SKIPPED         0x03\n"\
	"#define ARMULATE_INV_INSTR       0x04\n"\
	"#define ARMULATE_STOP_REQUEST    0x05\n"\
	"#define ARMULATE_TIMED_OUT       0x06\n"\
	"#define ARMULATE_DEADLOCK        0x07\n"\
	"#define ARMULATE_EXCEPTION       0x08\n"\
	"#define ARMULATE_ABORT_MEMORY_RD 0x01\n"\
	"#define ARMULATE_ABORT_MEMORY_WR 0x02\n"\
	"#define ARMULATE_ABORT_MEMORY_EX 0x03\n"\
	"#define ARMULATE_ABORT_UNKNOWN_INSTR 0x04\n"\
	"#define ARMULATE_ABORT_INV_MODE      0x05\n"\


#define ARMULATE_SYMBOLS REGISTER_SYMBOL ( armulate_setup );\
	REGISTER_SYMBOL ( armulate_run_until );\
	REGISTER_SYMBOL ( armulate_get_reg );\
	REGISTER_SYMBOL ( armulate_set_reg );\
	REGISTER_SYMBOL ( armulate_get_cpsr );\
	REGISTER_SYMBOL ( armulate_set_cpsr );\
	REGISTER_SYMBOL ( armulate_exec_step );\
	REGISTER_SYMBOL ( armulate_exec_next );\
	REGISTER_SYMBOL ( armulate_get_pc );\
	REGISTER_SYMBOL ( armulate_set_pc );\
	REGISTER_SYMBOL ( armulate_is_thumb );\
	REGISTER_SYMBOL ( armulate_add_bp );\
	REGISTER_SYMBOL ( armulate_set_bps );\
	REGISTER_SYMBOL ( armulate_is_bp );\
	REGISTER_SYMBOL ( armulate_gdb_listen );\
	REGISTER_SYMBOL ( armulate_reason );\
	REGISTER_SYMBOL ( armulate_get_abort_address );\
	REGISTER_SYMBOL ( armulate_get_abort_memory );\
	REGISTER_SYMBOL ( armulate_get_abort_value );\
	REGISTER_SYMBOL ( armulate_get_abort_width );\
	REGISTER_SYMBOL ( armulate_get_abort_override );\
	REGISTER_SYMBOL ( armulate_get_abort_situation );\
	REGISTER_SYMBOL ( armulate_set_abort_address );\
	REGISTER_SYMBOL ( armulate_set_abort_memory );\
	REGISTER_SYMBOL ( armulate_set_abort_value );\
	REGISTER_SYMBOL ( armulate_set_abort_width );\
	REGISTER_SYMBOL ( armulate_set_abort_override );\
	REGISTER_SYMBOL ( armulate_set_abort_situation );\
	REGISTER_SYMBOL ( armulate_set_exec_callback );\
	REGISTER_SYMBOL ( armulate_lr_dump );\


