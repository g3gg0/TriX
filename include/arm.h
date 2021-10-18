#ifndef __TRIX_ARM_H__
#define __TRIX_ARM_H__


#define MIN_FREESPACE 0x100  //if searched for less free space, it  will search for this value

#define ARM_DEST_VAL  0x8000
#define ARM_REG_RD    0x4000

#define THUMB_OPCODE  0x1000
#define ARM_OPCODE    0x2000
#define ARM_THUMB_MASK (THUMB_OPCODE | ARM_OPCODE)

// THUMB opcodes
#define THUMB_BL  (THUMB_OPCODE | 0)
#define THUMB_B   (THUMB_OPCODE | 1)
#define THUMB_BEQ (THUMB_OPCODE | 2)
#define THUMB_BNE (THUMB_OPCODE | 3)
#define THUMB_BCS (THUMB_OPCODE | 4)
#define THUMB_BCC (THUMB_OPCODE | 5)
#define THUMB_BMI (THUMB_OPCODE | 6)
#define THUMB_BPL (THUMB_OPCODE | 7)
#define THUMB_BVS (THUMB_OPCODE | 8)
#define THUMB_BVC (THUMB_OPCODE | 9)
#define THUMB_BHI (THUMB_OPCODE | 10)
#define THUMB_BLS (THUMB_OPCODE | 11)
#define THUMB_BGE (THUMB_OPCODE | 12)
#define THUMB_BLT (THUMB_OPCODE | 13)
#define THUMB_BGT (THUMB_OPCODE | 14)
#define THUMB_BLE (THUMB_OPCODE | 15)
#define THUMB_LDR (THUMB_OPCODE | 16)
#define THUMB_ADR (THUMB_OPCODE | 17)
#define THUMB_BLX (THUMB_OPCODE | 18)

// ARM opcodes
#define ARM_B    (ARM_OPCODE | 19)
#define ARM_BL   (ARM_OPCODE | 20)
#define ARM_BLX  (ARM_OPCODE | 21)
#define ARM_BLX1 (ARM_OPCODE | 22)
#define ARM_BLX2 (ARM_OPCODE | 23)
#define ARM_LDR  (ARM_OPCODE | 24)
#define ARM_ADR  (ARM_OPCODE | 25)
#define ARM_MOV  (ARM_OPCODE | 26)

// THUMB opcodes for backward compatibility
#define BL  0
#define B   1
#define BEQ 2
#define BNE 3
#define BCS 4
#define BCC 5
#define BMI 6
#define BPL 7
#define BVS 8
#define BVC 9
#define BHI 10
#define BLS 11
#define BGE 12
#define BLT 13
#define BGT 14
#define BLE 15
#define LDR 16
#define ADR 17
#define BLX 18

#define ADDRESS 0xFF


#include "trixplug_arm.h"

unsigned int arm_init ();
unsigned int arm_set ( t_workspace *ws, unsigned int type, unsigned int offset, unsigned int dest );
unsigned int arm_set_32 ( t_workspace *ws, unsigned int type, unsigned int offset, unsigned int dest, unsigned int cond_field );
unsigned int arm_get ( t_workspace *ws, unsigned int offset );
unsigned int arm_get_32 ( t_workspace *ws, unsigned int offset );
unsigned int arm_get_32_adv ( t_workspace *ws, unsigned int offset, unsigned int type );
unsigned int arm_get_adv ( t_workspace *ws, unsigned int offset, unsigned int type );
unsigned int arm_find ( t_workspace *ws, unsigned int type, unsigned int start, unsigned int end, unsigned int direction );
unsigned int arm_get_freespace ( t_workspace *ws, unsigned int size, unsigned int offset );
unsigned int arm_find_src_of_dest ( t_workspace *ws, int type, unsigned int dest, unsigned int start, unsigned int end, unsigned int direction );
unsigned int arm_find ( t_workspace *ws, unsigned int type, unsigned int start, unsigned int end, unsigned int direction );
object *arm_create_object_from_makefile ( const char *makefile_path, const char *object_file );
object *arm_create_object ( const char *makefile_path, const char *object_file );
unsigned int arm_run_makefile ( const char *makefile_path );
unsigned int arm_get_bl ( t_workspace *ws, unsigned int offset );
unsigned int arm_create_func ( t_workspace *ws, char *routine, unsigned int offset );

INLINE_MODE int arm_convert_to_int ( int arm_value, unsigned char bits );
INLINE_MODE int arm_convert_to_x_bit ( int val, unsigned char bits );


#endif
