#ifndef __ARM_H__
#define __ARM_H__


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

    void *memint;
	unsigned char pc_changed;
} ARM7TDMI;

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

#define ZFLAG		((arm->cpsr & Z_BIT)?1:0)
#define NFLAG		((arm->cpsr & N_BIT)?1:0)
#define CFLAG		((arm->cpsr & C_BIT)?1:0)
#define VFLAG		((arm->cpsr & V_BIT)?1:0)

#define ZFLAG_SET(v) (arm->cpsr = (arm->cpsr & ~Z_BIT) | ((v)?Z_BIT:0))
#define NFLAG_SET(v) (arm->cpsr = (arm->cpsr & ~N_BIT) | ((v)?N_BIT:0))
#define CFLAG_SET(v) (arm->cpsr = (arm->cpsr & ~C_BIT) | ((v)?C_BIT:0))
#define VFLAG_SET(v) (arm->cpsr = (arm->cpsr & ~V_BIT) | ((v)?V_BIT:0))

#endif
