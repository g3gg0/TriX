

#define INT_VECTOR_RESET      0
#define INT_VECTOR_UNDEF      1
#define INT_VECTOR_SWI        2
#define INT_VECTOR_ABORT_INST 3
#define INT_VECTOR_ABORT_DATA 4
#define INT_VECTOR_IRQ        5
#define INT_VECTOR_FIQ        6

//#define TIKU

#ifdef TIKU
#define LITTLE_ENDIAN     1
#define INT_VECTOR_BASE   (0x00000020)
#define FAID_BASE         (0x0000012C)
#define CBUSC_BASE        (0x0C305300)
#define STI_BASE          (0x0C305400)
#define CTSI_BASE         (0x0C308000)
#define PUP_BASE          (0x0C308200)
#define UIF_BASE          (0x0C308400)
#define MCUPLL_BASE       (0)
#define EBUSC_BASE        (0)
#define IRQ_STACK         (0x0A000800)
//#define OWN_STACK         (0x0A001000)
#define OWN_BUFFER        (0x0A002000)
#define RAM_BASE          (0x0A000000)
#else
#define LITTLE_ENDIAN     0
#define INT_VECTOR_BASE   (0x00000020)
#define FAID_BASE         (0x0100002C)
#define CBUSC_BASE        (0x0600C800)
#define STI_BASE          (0x0600D000)
#define CTSI_BASE         (0x06010000)
#define PUP_BASE          (0x06010200)
#define UIF_BASE          (0x06010400)
#define MCUPLL_BASE       (0x06009000)
#define EBUSC_BASE        (0x0600C000)
#define IRQ_STACK         (0x00003F00)
#define RAM_BASE          (0x00000000)
#define OWN_STACK         (0x00003E00)
#define OWN_BUFFER        (0x00002000)
#endif


#define CBUS_CLK_SET      (CTSI_BASE + 0x0034)
#define CBUS_CLK_RESET    (CTSI_BASE + 0x0036)

#define CBUS_BITCOUNT     (UIF_BASE + 0x0020)
#define CBUS_CTRL         (UIF_BASE + 0x0022)
#define CBUS_DATA_HIGH    (UIF_BASE + 0x0024)
#define CBUS_DATA_LOW     (UIF_BASE + 0x0026)
#define CBUS_CLK_SPEED    (UIF_BASE + 0x0030)

#define CBUS_CTRL_READY   (0x0400)
#define CBUS_CTRL_WRITE   (0x0200)
#define CBUS_CTRL_READ    (0x0100)
#define CBUS_BITS_WRITE   (0xE0)
#define CBUS_INIT         (0x40)

#define CLK_SPEED         (0x66)

#define CBUSC_INIT        (CBUSC_BASE + 0x0000)
#define CBUSC_FAID        (CBUSC_BASE + 0x001A)
#define CBUSC_BYTE        (CBUSC_BASE + 0x0036)

#define UEM_ADDRESS       (1)

// evil macros to convert defined constants to strings
#define STR(x) #x
#define ADDR_STR(x) STR(x)


#define INT_COUNT			(*freeram)
#define STATE				(*state)
#define SAVED				(*saved)

/* current system/user mode registers */
#define R(x)				(registers[x])
#define CPSR				(R(0x10))

/* saved (for state restore) system/user mode registers */
#define S(x)				(R(0x11+x))
#define SPSR				(S(0x10))

/* backup (for displaying state) system/user mode registers */
#define B(x)				(storage[x])
#define BPSR				(B(0x10))

typedef unsigned long  u32;
typedef unsigned short u16;
typedef unsigned char  u8;


u32 *freeram    = RAM_BASE + 0x0004B5A0;
u32 *state      = RAM_BASE + 0x0004B5A4;
u32 *saved      = RAM_BASE + 0x0004B5A8;

u32 *registers  = RAM_BASE + 0x0004B5B0;
u32 *storage    = RAM_BASE + 0x0004B6B0;
u32 *config     = RAM_BASE + 0x00003000;

u32 cpy_area    = RAM_BASE + 0x00007000;
u32 buffer      = RAM_BASE + 0x00006000;

u32 calib_value = 0;
u32 calib_systimer_diff = 0;
u32 calib_systimer = 0;

u32 romcall_addr = 0x00000000;
u32 romcall_r0   = 0x55555555;
u32 romcall_r1   = 0x55555555;
u32 romcall_r2   = 0x55555555;
u32 romcall_r3   = 0x55555555;


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

static inline void
arm_mode_set ( u32 mode )
{
    asm volatile ( 
		"adr     R0, mode_set_arm;"
		"bx	     R0;"

		".code 32;"
		"mode_set_arm:;"
		"mrs   r0, CPSR;"
		"bic   r0, r0, #0x01F;"
		"orr   r0, r0, %0;"
		"msr   CPSR_c, r0;"
		
		"adr     R0, .mode_set_thumb+1;"
		"bx	     R0;"

		".code 16;"
		".mode_set_thumb:;":: "r" (mode) );
		
}

void 
memset_32 ( u32 *dst, u32 value, u32 blocks )
{
	while ( blocks-- )
	{
		*dst = value;
		dst++;
	}
}


void 
memcpy_32 ( u32 *dst, u32 *src, u32 blocks )
{
	while ( blocks-- )
	{
		*dst = *src;
		dst++;
		src++;
	}
}

void 
memcpy_16 ( u16 *dst, u16 *src, u32 blocks )
{
	while ( blocks-- )
	{
		*dst = *src;
		dst++;
		src++;
	}
}

void 
memcpy_8 ( u8 *dst, u8 *src, u32 blocks )
{
	while ( blocks-- )
	{
		*dst = *src;
		dst++;
		src++;
	}
}

void 
bx_r7 ()
{
    asm volatile ( "bx r7;" );
}



/* this is the ARM FIQ/IRQ handler */
void
arm_irq_handler ( )
{
	asm volatile ( 
		".code 32;"
		"ldr     r13, .irq_stack;"
		"stmfd   SP!, {r0-r1, lr};"
		
		/* acknowledge timer interrupt */
		"ldr     r0, .ctsi_base;"
		"add     r0, r0, #0xff;"
		"add     r0, r0, #0x01;"
		"mov     r1, #0x80;"
		"strh    r1, [r0];"

		"ldmfd   SP!, {R0-R1, LR};"      // get "original" R0 R1 and LR
		"subs    pc, lr, #4;"

		".align 4;"
		
		".irq_stack:;"
		"	.word "ADDR_STR(IRQ_STACK)";"
		".ctsi_base:;"
		"	.word "ADDR_STR(CTSI_BASE)";"
		);
}


static void
int_enable ( )
{
	asm volatile ( 
		"adr     R0, switch_to_arm1;"
		"bx	     R0;"

		/* now in ARM mode, save all registers to our buffer */
		".code 32;"
		"switch_to_arm1:;"

		"mrs r0, CPSR;"
		"bic r0, r0, #0xC0;" 
		"msr CPSR_cxsf, r0;" 
		
		"adr     R0, .switch_thumb1+1;"
		"bx	     R0;"

		/* after saving regs to stack, DISABLED (check if timer int occured) */
		".code 16;"
		".switch_thumb1:;"
		
		);
}

static void
int_disable ( )
{
	asm volatile ( 
		"adr     R0, switch_to_arm2;"
		"bx	     R0;"

		/* now in ARM mode, save all registers to our buffer */
		".code 32;"
		"switch_to_arm2:;"

		"mrs r0, CPSR;"
		"orr r0, r0, #0xC0;" 
		"msr CPSR_cxsf, r0;" 
		
		"adr     R0, .switch_thumb2+1;"
		"bx	     R0;"

		/* after saving regs to stack, DISABLED (check if timer int occured) */
		".code 16;"
		".switch_thumb2:;"
		
		);
}

static void
mcupll_set ( u32 multiplier, u32 divisor )
{
	volatile int loops = 0x2040;
	
	strh ( MCUPLL_BASE + 0x0000, 0x2000 );
	while ( loops-- );
	strh ( MCUPLL_BASE + 0x0000, 0x2010 | ((multiplier&0x1F)<<7) | ((divisor&0x03)<<5) );
}


int 
intvec_init ()
{
	volatile u32 *vec_table = (u32 *)INT_VECTOR_BASE;
	
	vec_table[INT_VECTOR_UNDEF] = &arm_irq_handler;
	vec_table[INT_VECTOR_SWI] = &arm_irq_handler;
	vec_table[INT_VECTOR_ABORT_INST] = &arm_irq_handler;
	vec_table[INT_VECTOR_ABORT_DATA] = &arm_irq_handler;
	vec_table[INT_VECTOR_IRQ] = &arm_irq_handler;
	vec_table[INT_VECTOR_FIQ] = &arm_irq_handler;
	
	return 0;
}


int 
int_init ()
{
	/* activate interrupt controller */
	strh ( CTSI_BASE + 0x0024, 0x0010 ); // disable IRQs
	strh ( CTSI_BASE + 0x0024, 0x0001 ); // generally enable IRQs
	strh ( CTSI_BASE + 0x0024, 0x0004 ); // activate IRQs
	
	/* disable ALL device interrupts */
	strh ( CTSI_BASE + 0x0008, 0xFFFF ); // Disable IRQs
	strh ( CTSI_BASE + 0x000C, 0xFFFF ); // Disable IRQs
	strh ( CTSI_BASE + 0x0010, 0xFFFF ); // Disable IRQs
	strh ( CTSI_BASE + 0x0014, 0xFFFF ); // Disable IRQs
	strh ( CTSI_BASE + 0x0018, 0xFFFF ); // Disable IRQs
	strh ( CTSI_BASE + 0x001C, 0xFFFF ); // Disable IRQs
	strh ( CTSI_BASE + 0x0020, 0xFFFF ); // Disable IRQs

	/* just enable Timer 2 */
	strh ( CTSI_BASE + 0x0012, 0x0080 ); // Enable Timer IRQ
	
	return 0;
}

int
timer_prepare ( )
{
	u32 value = 0;
	
	strh ( CTSI_BASE + 0x0114, 0xFF );
	value = ldrh ( CTSI_BASE + 0x0116 );
	value += 0x0010;
	strh ( CTSI_BASE + 0x0118, value );
	
	return 0;
}


// set the CBUS clock speed 
static void
cbus_clock ( u8 speed )
{
	static u16 set_values[] = { 0x0000, 0x0024, 0x000C };
	static u16 rst_values[] = { 0x002C, 0x0008, 0x0020 };
	
	if ( speed < 3 )
	{
		if ( rst_values[speed] )
			strh ( CBUS_CLK_RESET, rst_values[speed] );
		if ( set_values[speed] )
			strh ( CBUS_CLK_SET, set_values[speed] );
	}
}

// initialize CBUS clock and state
static void
cbus_init ()
{
	cbus_clock ( 0 );
	strh ( CBUS_CLK_SPEED, CLK_SPEED );
	strh ( CBUS_CLK_SET, CBUS_INIT );
	cbus_clock ( 1 );
}

// unlock CBUS by copying FAID into CBUSC registers
int
cbus_unlock ( )
{
#ifndef TIKU
	asm volatile ( 
		"push  {r0-r7,lr};"
		"adr   r0, faidbuf;"
		"ldrb  r1, [r0, #0x0D];"
		"mov   r2, #0x0B;"
		"mov   r3, #0x00;"
		"ldr   r4, upload_faid;"
		"mov   lr, pc;"
		"bx    r4;"
		"pop   {r0-r7,pc};"
		
		".align 4;"
		"upload_faid:;"
		".word 0x0084001F;"
		"faidbuf:;"
		".word 0xCE9CB10E;"
		".word 0x58312707;"
		".word 0x7246C0C2;"
		".word 0x0001a4bf;"
		 );
#else
   int pos = 0;
   
   strh ( CBUSC_INIT, 0x1F00 );
   strh ( CBUSC_BYTE, ldrb ( FAID_BASE + 12 ) );
   
   for ( pos = 0; pos < 12; pos+=2 )
   {
      u16 value = ldrh_rel ( FAID_BASE, pos );
      
      strh_rel ( CBUSC_FAID, pos, value );
   }
   
   strh ( CBUSC_INIT, 0x000B );
#endif
}


// wait until CBUS is ready
static void
cbus_wait ()
{
	u32 timeout = 0xF000;
	
	while ( !(ldrh ( CBUS_CTRL ) & CBUS_CTRL_READY) && (timeout-- > 0) );
	
	if ( !timeout )
		buffer[0] = 0xEEEE;
}

// read register [reg] from device [dev]
static u32
cbus_read ( u32 dev, u32 reg )
{
	cbus_wait ();
	strh ( CBUS_BITCOUNT, 0x0F );
	strh ( CBUS_CTRL, CBUS_CTRL_READ | ((dev & 0x07) << 5) | (reg & 0x1F) );
	cbus_wait ();
	
	return (ldrh ( CBUS_DATA_HIGH ) << 16) | ldrh ( CBUS_DATA_LOW );
}

// read register [reg] from UEM
static u32
uem_read ( u32 reg )
{
	return cbus_read ( UEM_ADDRESS, reg );
}



// dump UEM registers
int dump_uem ()
{
	int pos = 0;
	
	// to make sure that this routine was run
	buffer[buffer_pos++] = 0x5555;

	while ( pos < 0x1F )
		buffer[buffer_pos++] = uem_read ( pos++ );
	
	// to make sure that this routine was run
	buffer[buffer_pos++] = 0xAAAA;

	return 0;
}






int
spi_ready ()
{
	while ( !(ldrh ( 0x0601042A ) & 0x01) ); // -> 0x0007 (UIF+0x002A Graph Ready States)
	
	return 1;
}
int
spi_send_cmd ( u32 data )
{
	strh ( 0x0601041A, data ); // (UIF+0x001A Graph Write CMD)
}

int
spi_send_data ( u32 data, u32 len )
{
	strh ( 0x0601041C, data ); // (UIF+0x001A Graph Write CMD)
}

int
lcd_init ()
{
	int pos = 0;
	static const unsigned short lcd_inittab[] = {
		0x0011,
		0x00DA,
		0x00AF,
		0x00CA, 0x800C, 0x8020, 0x800C,
		0x00BB, 0x8001,
		0x0094,
		0x00D1,
		0x0081, 0x801F, 0x8003,
		0x0082, 0x8001, 0x8000, 0x8000, 0x8000,	0x8000, 0x8001, 0x8015, 0x8017, 0x8022,	0x8024,	0x8026,	0x8027,	0x8028,	0x8000,
		0x0020, 0x8008,
		0x00A9,
		0x00AA, 0x8002, 0x8022, 0x8022, 0x8003,
		0x00AB, 0x8000,
		0x00BC, 0x8000, 0x8000, 0x8002,
		0x0075, 0x8000, 0x8082,
		0x0015, 0x8000, 0x8082,
		0x0020, 0x8008,
		0x0020, 0x800F,
		0
	};
	
	// reset LCD (?)
	strh ( 0x0601020E, 0xFFFF ); // (PUP+0x000E IO A Direction)
	strh ( 0x06010204, 0x0010 ); // (PUP+0x0004 IO A Output Reset)
	strh ( 0x06010202, 0x0010 ); // (PUP+0x0002 IO A Output Set)


	while ( lcd_inittab[pos] )
	{
		spi_ready ();
		if ( lcd_inittab[pos] & 0x8000 )
			spi_send_data ( lcd_inittab[pos]&0xFF, 2 );
		else
			spi_send_cmd ( lcd_inittab[pos]&0xFF );
		pos++;
	}

}


int 
main (  )
{
	int i = 0;
#ifdef OWN_STACK
	// use our own stack to make sure we have enough
	u32 old_stack = get_stack ();
	set_stack ( OWN_STACK );
#endif


	strh ( 0x06010034, 0x0003 ); // (CTSI+0x0034 LCD/Audio)
	strh ( 0x0601042E, 0x0009 ); // (UIF+0x002E Graph Interface Setup)
	strh ( 0x0601042C, 0x1000 ); // (UIF+0x002C )

	lcd_init ();
	
	lcd_write ( 10, 10, "CBUS", 0, 0 );
	cbus_unlock ();
	cbus_init ();
	lcd_write ( 10, 30, "DONE", 0, 0 );
	
	

	
#ifdef OWN_STACK
	// restore old stack pointer
	set_stack ( old_stack );
#endif

	return 0;
}
