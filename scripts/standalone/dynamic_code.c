

#define INT_VECTOR_RESET      0
#define INT_VECTOR_UNDEF      1
#define INT_VECTOR_SWI        2
#define INT_VECTOR_ABORT_INST 3
#define INT_VECTOR_ABORT_DATA 4
#define INT_VECTOR_IRQ        5
#define INT_VECTOR_FIQ        6

#define INT_VECTOR_BASE   (0x00000020)
#define FAID_BASE         (0x0100002C)

#define STI_BASE          (0x0600D000)
#define CTSI_BASE         (0x06010000)
#define PUP_BASE          (0x06010200)
#define UIF_BASE          (0x06010400)
#define MCUPLL_BASE       (0x06009000)
#define EBUSC_BASE        (0x0600C000)

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

#define OWN_STACK 0x000003E00
#define IRQ_STACK 0x000003F00

// evil macros to convert defined constants to strings
#define STR(x) #x
#define ADDR_STR(x) STR(x)


#define freeram		((unsigned int*)0x4B5A0)
#define registers	((unsigned int*)0x4B5B0)
#define storage		((unsigned int*)0x4B6B0)

#define INT_COUNT			(freeram[0])
#define STATE				(freeram[1])
#define SAVED				(freeram[2])

/* current system/user mode registers */
#define R(x)				(registers[x])
#define CPSR				(R(0x10))

/* saved (for state restore) system/user mode registers */
#define S(x)				(R(0x11+x))
#define SPSR				(S(0x10))

/* backup (for displaying state) system/user mode registers */
#define B(x)				(storage[x])
#define BPSR				(B(0x10))

typedef unsigned long u32;
typedef unsigned short u16;
typedef unsigned char u8;


u32* config = (u32*)0x00003000;
u32 cpy_area = 0x00007000;
u32 buffer = 0x00006000;
u32 calib_value = 0;
u32 calib_timerdiff = 0;


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




static __inline void 
timer_int_restorestate ( )
{
	int i = 0;

	for ( i = 0; i<16; i++ )
		R(i) = S(i);

	CPSR = SPSR;
}

static __inline void 
timer_int_savestate ( )
{
	int i = 0;

	for ( i = 0; i<16; i++ )
		S(i) = R(i);

	SPSR = CPSR;
}

static __inline void 
timer_int_backup ( )
{
	int i = 0;

	for ( i = 0; i<16; i++ )
		B(i) = R(i);

	BPSR = CPSR;
}

void 
timer_int_reg_mod ( )
{
	/* here check and modify the saved registers */
	int i = 0;

	if ( R(6) == 0xDEADBEEF )
	{
		/* the timer calibration routine requested signal */
		timer_int_backup ();
		calib_value = R(5);
		calib_timerdiff = config[4];
		
		R(5) = 0x80000000;
	}

	if ( R(6) == 0xDEADBABE )
	{
		/* requested to save the current state */
		R(6) = 0;
		timer_int_savestate ();
		SAVED = 1;
	}
	else if ( R(6) != 0xBABEFACE && SAVED == 1 )
	{
		/* trigger condition, restore CPU state */
		timer_int_backup ();
		
		/* copy buffer to saved area */
		memcpy_8 ( cpy_area, buffer, 0x29C );
		memcpy_8 ( cpy_area+0x400, 0x00005000, 0x29C );

		/* tell controller where the registers are */
		if ( R(6) == 0xC01DFACE )
			config[1] = 3; // set info "overrun"
		else if ( config[4] != calib_timerdiff )
			config[1] = 4; // set info "timer jitter"
		else
			config[1] = 2; // set info "captured"
			
		i = 5;
		
		config[i++] = storage;
		config[i++] = 17*4;
		
		config[i++] = cpy_area;
		config[i++] = 0x29C;
		config[i++] = cpy_area+0x400;
		config[i++] = 0x10;
		

		config[i++] = 0;
		config[i++] = 0;
			
		timer_int_restorestate ();
		R(5) = 0;
	}
}

/* this is the ARM FIQ/IRQ handler */
void
arm_irq_handler ( )
{
	asm volatile ( 
		/* jumping in ARM code is still buggy :( */
		".code 32;"
		"ldr     r13, .irq_stack;"
		"stmfd   SP!, {r0-r1, lr};"    // SP-3
		
		"adr     R0, .switch_thumb_here+1;"
		"bx	     R0;"

		/* after saving regs to stack, DISABLED (check if timer int occured) */
		".code 16;"
		".switch_thumb_here:;"

		/* if so, check if we should "sample" the registers */
		"ldr     R0, .ctsi_base;"
		"add     R0, R0, #0xFF;"
		"add     R0, R0, #0x01;"
		"ldrh    R1, [R0, #0x16];"
		"ldrh    R0, [R0, #0x18];"
		"cmp     R0, R1;"
		"bne     .no_timer_int_occured;"
		
		/* save timer values to config[2] and config[3] for debugging reasons */
		"push    {R2-R4};"
		"ldr     R0, .ctsi_base;"
		"add     R0, R0, #0xFF;"
		"add     R0, R0, #0x01;"
		"ldrh    R2, [R0, #0x12];"
		"ldrh    R3, [R0, #0x16];"
		"ldr     R0, .config;"
		"str     R2, [R0, #0x08];"
		"str     R3, [R0, #0x0C];"
		
		/* calc difference of timer1 */
		"ldr     r3, .ctsi_base;"
		"ldrh    r2, [r3, #0x38];"
		"ldrh    r4, [r3, #0x3A];"
		"lsl     r2, r2, #0x04;"
		"orr     r2, r2, r4;"
		"ldr     r3, [r0, #0x10];"
		"sub     r3, r2, r3;"
		"str     r3, [r0, #0x10];"
					
		"pop     {R2-R4};"
		
		
		"ldr     R0, .freeram;"
		"ldr     R1, [R0, #4];"
		"cmp     R1, #1;"             // start sample condition
		"bne     .no_timer_int_occured;"
		

		/* if we should sample, set the sample trigger value to 2, so we do that just once */
		"mov     R1, #2;"             // already sampled condition
		"str     R1, [R0, #4];"
		"adr     R0, switch_to_arm;"
		"bx	     R0;"

		/* now in ARM mode, save all registers to our buffer */
		".align 4;"
		".code 32;"
		"switch_to_arm:;"

		/* save all registers to our buffer */
		"ldr     R1, .registers;"
		"add     R0, R1, #0x08;"
		"stmia   R0, { R2-R14 }^;"   //             first save R2-R14 which are still unchanged
		"ldmfd   SP, { R2-R3, R4 };" // SP-0  now get the LR (old PC) into R4 and old R0/R1 into R2/R3
		"add     R0, R1, #0x00;"
		"stmia   R0, { R2-R3 };"     //       and save old R0/R1
		"add     R0, R1, #0x3C;"
		"stmia   R0, { R4 };"        //       hen save our LR (old PC)
		"mrs     R4, SPSR;"
		"add     R0, R1, #0x40;"
		"stmia   R0, { R4 };"        //          then save original CPSR


		"adr     R0, .do_modding_stuff+1;"
		"bx	     R0;"

		/* and then call our "modding" routine */
		".code 16;"
		".do_modding_stuff:;"
		"mov     R0, LR;"
		"push    { R0 };"
		"bl      timer_int_reg_mod;"
		"pop     { R0 };"
		"mov     LR, R0;"
		"adr     R0, switch_to_arm_2;"
		"bx	     R0;"

		/* after the "modding" routine was called, restore all (modded) userspace regs again */
		".code 32;"
		"switch_to_arm_2:;"

		/* after the "modding" routine was called, restore all (modded) userspace regs again */
		"ldr     R3, .registers;"
		"add     R0, R3, #0x40;"
		"ldmia   R0, { R1 };"            //  restore original/modded CPSR
		"msr     SPSR, R1;"
		"ldmfd   SP!, { R0-R1, R2 };"    // SP-0   now get the original R0, R1 and LR (we will discard them)
		"add     R4, R3, #0x00;"         //             the start of our registers R0 and R1
		"ldmia   R4, { R0-R1 };"         //             get the new registers R0 and R1
		"add     R4, R3, #0x3C;"         //             the start of our register LR
		"ldmia   R4, { R2 };"            //             get the new LR
		"stmfd   SP!, { R0-R1, R2 };"    // SP-3   and immediately push them on stack again
		"add     R0, R3, #0x08;"         //            the start of our registers R2-R15
		"ldmia   R0, { R2-R14 }^;"       // get them
		
		"adr     R0, .no_timer_int_occured+1;"
		"bx	     R0;"

		/* and continue with normal execution flow */
		".code 16;"
		".no_timer_int_occured:;"
		"adr     R0, back_to_arm;"
		"bx	     R0;"

		".code 32;"
		"back_to_arm:;"
		
		/* acknowledge timer interrupt in any case */
		"ldr     r0, .ctsi_base;"
		"add     r0, r0, #0xff;"
		"add     r0, r0, #0x01;"
		"mov     r1, #0x80;"
		"strh    r1, [r0];"

		"ldmfd   SP!, {R0-R1, LR};"      // get "original" R0 R1 and LR
		"subs    pc, lr, #4;"

		".align 4;"
		".config:;"
		"	.word 0x00003000;"
		".freeram:;"
		"	.word 0x0004B5A0;"
		".registers:;"
		"	.word 0x0004B5B0;"


		".irq_stack:;"
		"	.word "ADDR_STR(IRQ_STACK)";"
		".ctsi_base:;"
		"	.word "ADDR_STR(CTSI_BASE)";"
		);
}


void 
timing_step ( unsigned int delay )
{
	asm volatile ( 
		".code 16;"

		"push   { r0-r7, lr };"
		"mov    r5, %0;"
		"ldr    r0, state;"
		"ldr    r1, timer_base;"
		"mov    r3, #0;"
		"mov    r4, #1;"
		"mov    r6, #0;"
		"mov    r7, #0;"
		
		/*       start saving the current state     */
		
		/* wait until timer advances */
		"timing_step_savestate_wait:;"
		"ldrh   r3, [r1, #0x12];"
		"cmp    r3, #0x00;"
		"bne    timing_step_savestate_wait;"
		
		/* then set trigger time */
		"ldrh   r3, [r1, #0x16];"
		"add    r3, r3, #1;"
		"strh   r3, [r1, #0x18];"
		
		/* request state save */
		"ldr    r6, savestate;"
		"str    r4, [r0, #0];"
		
		/* and wait for timer int that saves our state */
		"timing_step_savestate:;"
		"cmp    r6, #0;"
		"bne    timing_step_savestate;"
		
		/* if we got 'restored' (R5 == 0), then immediately quit */
		"cmp    r5, #0;"
		"beq    timing_step_quit;"
		
		/* else signal to ignore next interrupt (normally not needed... but cleaner) */
		"ldr    r6, skiprestore;"
		
		
		/*       now prepare stepping     */
		
		
		/* wait until timer advances */
		"timing_step_wait:;"
		"ldrh   r3, [r1, #0x12];"
		"cmp    r3, #0x00;"
		"bne    timing_step_wait;"
		
		/* let the handler know that timing is active */
		"str    r4, [r0, #0];"
		
		/* load register for timer 1 */
		"ldr    r0, timer1diff;"
		"sub    r3, r1, #0x01;"
		"sub    r3, #0xff;"

		/* no more ignore next interrupt (R6 used as timer base now, not some magic number anymore) */
		"ldr    r6, timer_base;"
		
		/* then set trigger time */
		"ldrh   r4, [r6, #0x16];"
		"add    r4, r4, #0x04;"
		"strh   r4, [r6, #0x18];"
		
		"ldrh   r2, [r3, #0x38];"
		"ldrh   r4, [r3, #0x3A];"
		"lsl    r2, r2, #0x04;"
		"orr    r2, r2, r4;"
		"str    r2, [r0, #0x00];"
		"mov    r4, #0x00;"
		
		/* since we have many cpu cycles per tick, first burn <delay>*8? cycles */
		"timing_step_loop:"
		"sub    r5, r5, #1;"
		"bne    timing_step_loop;"
		
		/* then call the ROM handler we want to debug */
		"ldr    r0, romcall_r0;"
		"ldr    r1, romcall_r1;"
		"ldr    r2, romcall_r2;"
		"ldr    r3, romcall_r3;"
		"ldr    r7, romcall;"
		
		"bl     bx_r7;"
		
		/* endless loop until the timer triggers */
		"ldr    r6, overrun;"
		"timing_step_lock:"
		"b      timing_step_lock;"
		
		
		"timing_step_quit:;"
		/* clear saved state register */
		"ldr    r0, state;"
		"add    r0, r0, #0x08;"
		"mov    r1, #0x00;"
		"str    r1, [r0, #0];"
		
		"pop    { r0-r7, pc };"
		
		".align 4;"
		"romcall:;"
		".word 0x0084001D;"
		"romcall_r0:;"
		".word 0xAA55AA55;"
		"romcall_r1:;"
		".word 0xAA55AA55;"
		"romcall_r2:;"
		".word 0xAA55AA55;"
		"romcall_r3:;"
		".word 0xAA55AA55;"
				
	::"r" ( delay ) );

}

void 
timing_calib ( )
{
	asm volatile ( 
		".code 16;"
		
		"push   { r0-r7, lr };"
		"mov    r5, #0;"
		"ldr    r0, state;"
		"str    r5, [r0, #0];"
		"ldr    r1, timer_base;"
		"sub    r3, r1, #0x01;"
		"sub    r3, #0xff;"
		"mov    r6, #0;"
		"mov    r7, #0;"
		"mov    r4, #1;"
		
		/* wait until timer advances */
		"timing_calib_wait:;"
		"ldrh   r6, [r1, #0x12];"
		"cmp    r6, #0x00;"
		"bne    timing_calib_wait;"
		
		/* then set trigger values */
		"ldr    r6, signalvalue;"
		"str    r4, [r0, #0];"
		"ldr    r0, timer1diff;"
		
		"ldrh   r2, [r1, #0x16];"
		"add    r2, r2, #0x04;"
		"strh   r2, [r1, #0x18];"
		
		"ldrh   r2, [r3, #0x38];"
		"ldrh   r4, [r3, #0x3A];"
		"lsl    r2, r2, #0x04;"
		"orr    r2, r2, r4;"
		"str    r2, [r0, #0x00];"
		
		/* count up until int handler will set R5 to a negative value */
		"timing_calib_loop:"
		"add    r5, r5, #1;"
		"bpl    timing_calib_loop;"
		
		"pop    { r0-r7, pc };"
		
		
		".align 4;"
		"timer1diff:;"
		".word 0x00003010;"
		"signalvalue:;"
		".word 0xDEADBEEF;"
		"savestate:;"
		".word 0xDEADBABE;"
		"skiprestore:;"
		".word 0xBABEFACE;"
		"overrun:;"
		".word 0xC01DFACE;"
		"state:;"
		".word 0x0004B5A4;"
		"timer_base:;"
		".word 0x06010100;"
		
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
	
	strh ( 0x06009000, 0x2000 );
	while ( loops-- );
	strh ( 0x06009000, 0x2010 | ((multiplier&0x1F)<<7) | ((divisor&0x03)<<5) );
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

int
cbus_unlock ( )
{
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
   			
}

int 
main (  )
{

#ifdef OWN_STACK
	// use our own stack to make sure we have enough
	u32 old_stack = get_stack ();
	set_stack ( OWN_STACK );
#endif


	switch ( config[0] )
	{
	
		/* command 1 - say hello */
		case 1:
			int_disable ( );
			
			intvec_init ( );
			int_init ( );
			cbus_unlock ( );
			
			timer_prepare ( );
			int_enable ( );
			arm_mode_set ( 0x1F );

			/* overclock little ;) */
			mcupll_set ( 3, 0 );
			
			config[1] = 1; // set "ok" flag
			break;
			
			
		/* command 2 - calibrate */
		case 2:
			/* clock down a little bit */
			mcupll_set ( 1, 0 );
			
			timing_calib ( );
			
			/* overclock little ;) */
			mcupll_set ( 3, 0 );
			
			config[1] = calib_value;
			break;
		
		
		/* command 3 - execute code */
		case 3:
			/* clear buffers */
			memset_32 ( 0x00005000, 0xffffffff, 0x29C/4 );
			memset_32 ( buffer, 0, 0x29C/4 );
			
			/* clock down a little bit */
			mcupll_set ( 1, 0 );
			
			config[1] = 1;
			calib_value = config[2];
			
			// try a few times
			{
				int loops = 50;
				
				// when int handler said that timer1 difference was not as expected, retry
				while ( loops-- && config[1] != 0x02 )
					timing_step ( calib_value );
			}
			
			/* overclock little ;) */
			mcupll_set ( 3, 0 );
			break;
		
		
		/* unknown command */
		default:
			config[1] = 0xFF; // set error flag
			break;
	}
	
#ifdef OWN_STACK
	// restore old stack pointer
	set_stack ( old_stack );
#endif

	return 0;
}
