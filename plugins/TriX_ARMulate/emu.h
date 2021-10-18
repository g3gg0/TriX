#ifndef EMU_HEADER
#define EMU_HEADER

#include "arm.h"


#define INSTRUCTION_TRACE() \
	if ( instruction_trace ) \
	{\
		unsigned char buffer[128];\
		sprintf ( buffer, "<0x%08X  %s>\n", armulate_get_pc(), __FUNCTION__ );\
		armulate_warn ( buffer );\
	}\

#define OPCODE			arm->op
#define OPCODE_T		arm->op_t
#define CPSR			arm->cpsr
#define CONDITION_MASK	(OPCODE>>28)&0xF
#define OPCODE_MASK		((OPCODE&0x0FF00000)>>16)|((OPCODE&0xF0)>>4)
#define OPCODE_MASK_T	(OPCODE_T>>6)

#define N_FLAG_SET		(arm->cpsr&0x80000000)
#define Z_FLAG_SET		(arm->cpsr&0x40000000)
#define C_FLAG_SET		(arm->cpsr&0x20000000)
#define V_FLAG_SET		(arm->cpsr&0x10000000)
#define N_FLAG_CLEAR	(!(arm->cpsr&0x80000000))
#define Z_FLAG_CLEAR	(!(arm->cpsr&0x40000000))
#define C_FLAG_CLEAR	(!(arm->cpsr&0x20000000))
#define V_FLAG_CLEAR	(!(arm->cpsr&0x10000000))
#define N_EQU_V_FLAG	(!(N_FLAG_SET^(V_FLAG_SET<<3)))
#define N_NEQ_V_FLAG	(N_FLAG_SET^(V_FLAG_SET<<3))

#define LSL 0
#define LSR 1
#define ASR 2
#define ROR 3

#define POS(i) ( (~(i)) >> 31 )
#define NEG(i) ( (i) >> 31 )

ARM7TDMI *arm;
ARM7TDMI arm_backup;

//
// SEE ALSO armulate.h !!!!!
//
#define ARMULATE_EXECUTED        0x01
#define ARMULATE_BP_REACHED      0x02
#define ARMULATE_SKIPPED         0x03
#define ARMULATE_INV_INSTR       0x04
#define ARMULATE_STOP_REQUEST    0x05
#define ARMULATE_TIMED_OUT       0x06
#define ARMULATE_DEADLOCK        0x07
#define ARMULATE_EXCEPTION       0x08

#define ARMULATE_CB_NO_ACTION    0x1000
#define ARMULATE_CB_IRQ          0x1001
#define ARMULATE_CB_FIQ          0x1002

#define ARMULATE_ABORT_MEMORY_RD	 0x01
#define ARMULATE_ABORT_MEMORY_WR	 0x02
#define ARMULATE_ABORT_MEMORY_EX	 0x03
#define ARMULATE_ABORT_UNKNOWN_INSTR 0x04
#define ARMULATE_ABORT_INV_MODE      0x05


// MEMORY_RANGE *meminfo;

u32  (*code_execution   [0xF])(void);
u32  (*opcode_handles   [0x1000])(void);
u32  (*opcode_handles_special   [0x1000])(void);
u32  (*opcode_handles_t [0x400])(void);

void set_abort ( int mode );
void armulate_warn ( unsigned char *buffer );
int armulate_aborted();

char *cpu_mode_strings[0x20];  
char *dummy_mem; 
extern unsigned int instruction_trace;

void setup_tables (void);
u8   read_byte (u32 adress, ARM7TDMI *emu);
u16  read_hword (u32 adress, ARM7TDMI *emu);
u32  read_word (u32 adress, ARM7TDMI *emu);
u16  read_aligned_hword (u32 adress, ARM7TDMI *emu);
u32  read_aligned_word (u32 adress, ARM7TDMI *emu);
void write_byte (u32 adress, u8 data, ARM7TDMI *emu);
void write_hword (u32 adress, u16 data, ARM7TDMI *emu);
void write_word (u32 adress, u32 data, ARM7TDMI *emu);

void set_ARM_mode ( int mode );

void set_ARM_reg(int reg, u32 value);
void set_ARM_reg_mode(int reg, u32 value, u32 mode);
u32 get_ARM_reg_mode(int reg, u32 mode);
u32 get_ARM_reg(int reg);

void pc_changed ();



#endif
