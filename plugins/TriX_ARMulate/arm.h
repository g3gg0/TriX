#ifndef __ARM_H__
#define __ARM_H__

typedef unsigned char  byte;
typedef unsigned short hword;
typedef unsigned long  word;

typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned long  u32;

typedef signed char  s8;
typedef signed short s16;
typedef signed long  s32;

typedef struct tARM7TDMI {
	u32 gp_reg_USER[16];
	u32 gp_reg_FIQ[8];
	u32 gp_reg_IRQ[2];
	u32 gp_reg_SUPERVISOR[2];
	u32 gp_reg_ABORT[2];
	u32 gp_reg_UNDEFINED[2];
	u32 gp_reg_SYSTEM[2];
	u32 gp_reg_SAVE[16];
	u32 cpsr;
	u32 cpsr_SAVE;
	u32 op;
	u16 op_t;
	u32 spsr[0x20];
//	u32 state;

	u32 z_flag;
	u32 n_flag;
	u32 c_flag;
	u32 v_flag;

	unsigned char pc_changed;
} ARM7TDMI;


typedef struct {
	ARM7TDMI *arm;
	void *pDoc;
	void *memint;
} t_emustate;

#define USER_MODE		0x10
#define FIQ_MODE		0x11
#define IRQ_MODE		0x12
#define SUPERVISOR_MODE	0x13
#define ABORT_MODE		0x17
#define UNDEFINED_MODE	0x1B
#define SYSTEM_MODE		0x1F

#define THUMB_BIT		0x20
#define FIQ_BIT			0x40
#define IRQ_BIT			0x80

#define THUMB_MODE		1
#define ARM_MODE		0

#define N_BIT		0x80000000
#define Z_BIT		0x40000000
#define C_BIT		0x20000000
#define V_BIT		0x10000000
#define T_BIT		0x00000020

#define ZFLAG		arm->z_flag
#define NFLAG		arm->n_flag
#define CFLAG		arm->c_flag
#define VFLAG		arm->v_flag

#endif