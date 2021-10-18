#ifndef _BASE_H_
#define _BASE_H_


#define msec *1000
#define sec *1000 msec

typedef unsigned long  u32;
typedef unsigned short u16;
typedef unsigned char  u8;

void usleep ( u32 us );
void memset_32 ( u32 *dst, u32 value, u32 blocks );
void memcpy_32 ( u32 *dst, u32 *src, u32 blocks );
void memcpy_16 ( u16 *dst, u16 *src, u32 blocks );
void memcpy_8 ( u8 *dst, u8 *src, u32 blocks );
void bx_r7 ( );
void bx_r6 ( );
void bx_r5 ( );
void bx_r4 ( );
void arm_int_disable ( );
void arm_int_enable ( );


u32 mcupll_set ( u32 multiplier, u32 divisor );

u32 intvec_init ( u32 arm_irq_handler );
u32 int_init ( );

u32 sti_init ( );
u32 pup_init ( );

u32 cbus_clock ( u8 speed );
u32 cbus_init ( );
u32 cbus_unlock ( );
u32 cbus_wait ( );
u32 cbus_read ( u32 dev, u32 reg, u32 bits );
u32 cbus_write ( u32 dev, u32 reg, u32 val, u32 bits );
u32 uem_read ( u32 reg, u32 bits );
u32 uem_write ( u32 reg, u32 val, u32 bits );
u32 uem_init ( );


u32 spi_received ( );
u32 spi_ready ( );
u32 spi_read ( );
u32 spi_send_cmd ( u32 data );
u32 spi_send_data ( u32 data );


// must be static inline and called  before stack setup
static inline void
arm_mode_set ( u32 mode )
{
    asm volatile ( 
		"adr     r1, .arm_mode_set32;"
		"bx	     r1;"

		".code 32;"
		".arm_mode_set32:;"
		
		"mrs     r1, CPSR;"
		"bic     r1, r1, #0x1F;"
		"orr     r1, r1, %0;"
		"msr     CPSR_cxsf, r1;"
		
		"adr     r1, .arm_mode_set16+1;"
		"bx	     r1;"

		".code 16;"
		".arm_mode_set16:;"
		: : "r" (mode) : "cc", "r1" );
		
}

static inline void
set_stack ( u32 addr )
{
    asm volatile ( "mov sp, %0"::"r" ( addr ) );
}

static inline u32
get_stack (  )
{
	u32 addr = 0;
	
    asm volatile ( "mov %0, sp":"=r" ( addr ) );
	return addr;
}

static inline u32
ldr ( u32 addr )
{
	u32 val = 0;
	
    asm volatile ( "ldr  %0, [%1,#0x00]" : "=r" ( val ) : "r" ( addr ) );
    return val;
}

static inline u32
ldrh ( u32 addr )
{
	u32 val = 0;
	
    asm volatile ( "ldrh  %0, [%1,#0x00]" : "=r" ( val ) : "r" ( addr ) );
    return val;
}

static inline u32
ldrh_rel ( u32 addr, u32 offs )
{
	u32 val = 0;
	
    asm volatile ( "ldrh  %0, [%1,%2]" : "=r" ( val ) : "r" ( addr ), "r" ( offs ) );
    return val;
}

static inline u32
ldrb ( u32 addr )
{
	u32 val = 0;
	
    asm volatile ( "ldrb  %0, [%1,#0x00]" : "=r" ( val ) : "r" ( addr ) );
    return val;
}

static inline void
str ( u32 addr, u32 val )
{
    asm volatile ( "str  %0, [%1,#0x00]"::"r" ( val ), "r" ( addr ) );
}

static inline void
strh_rel ( u32 addr, u32 offs, u32 val )
{
    asm volatile ( "strh  %0, [%1,%2]"::"r" ( val ), "r" ( addr ), "r" ( offs ) );
}

static inline void
strh ( u32 addr, u32 val )
{
    asm volatile ( "strh  %0, [%1,#0x00]"::"r" ( val ), "r" ( addr ) );
}

static inline void
strb ( u32 addr, u32 val )
{
    asm volatile ( "strb  %0, [%1,#0x00]"::"r" ( val ), "r" ( addr ) );
}

static void
_call_via_r4 ()
{
    asm volatile ( "bx r4;" );
}

static void
_call_via_r3 ()
{
    asm volatile ( "bx r3;" );
}

static void
_call_via_r2 ()
{
    asm volatile ( "bx r2;" );
}

static void
_call_via_r1 ()
{
    asm volatile ( "bx r1;" );
}

static void
_call_via_r0 ()
{
    asm volatile ( "bx r0;" );
}


#endif
