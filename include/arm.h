#ifndef __TRIX_ARM_H__
#define __TRIX_ARM_H__


#define MIN_FREESPACE 0x100  //if searched for less free space, it  will search for this value

// THUMB opcodes
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

// ARM opcodes
#define ARM_B   19
#define ARM_BL  20
#define ARM_BLX 21

#define ADDRESS 0xFF


#include "trixplug_arm.h"

unsigned int arm_init ();
unsigned int arm_set ( t_workspace *ws, unsigned int type, unsigned int offset, unsigned int dest );
unsigned int arm_set_32 ( t_workspace *ws, unsigned int type, unsigned int offset, unsigned int dest, unsigned int cond_field );
unsigned int arm_get ( t_workspace *ws, unsigned int offset );
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
