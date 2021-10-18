

#include "hw_defines.h"
#include "base.h"
#include "lcd.h"



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


u32 CONTROL_MEMORY_BASE = 0x00002000;

u32 *freeram;
u32 *state;
u32 *saved;

u32 *registers;
u32 *storage;
u32 *config;

u32 cpy_area;
u32 buffer;


u32 irq_stack_addr;
u32 ctsi_base_addr;
u32 timer_base_addr;


u32 calib_value = 0;
u32 calib_systimer_diff = 0;
u32 calib_systimer = 0;

/* the address to trace and the registers before startup */
u32 romcall_addr = 0x0080020F;
u32 romcall_r0   = 0;
u32 romcall_r1   = 0;
u32 romcall_r2   = 0;
u32 romcall_r3   = 0;
u32 romcall_r4   = 0;
u32 romcall_r5   = 0;
u32 romcall_r6   = 0;
u32 romcall_r7   = 0;


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
		calib_systimer_diff = calib_systimer;
		
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
		/*
		memcpy_8 ( cpy_area, buffer, 0x29C );
		memcpy_8 ( cpy_area+0x400, 0x00005000, 0x29C );
		*/
		memcpy_16 ( cpy_area, buffer, 0x40 );
		memcpy_16 ( cpy_area+0x80, 0x0600C800, 0x40 );

		/* tell controller where the registers are */
		if ( R(6) == 0xC01DFACE )
			config[1] = 3; // set info "overrun"
		else if ( calib_systimer != calib_systimer_diff )
			config[1] = 4; // set info "timer jitter"
		else
			config[1] = 2; // set info "captured"
			
		i = 5;
		
		config[i++] = storage;
		config[i++] = 17*4;

		config[i++] = cpy_area;
		config[i++] = 0x100;
		
		/*
		config[i++] = cpy_area;
		config[i++] = 0x29C;
		config[i++] = cpy_area+0x400;
		config[i++] = 0x10;
		*/

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
		"ldr     r13, [r13, #0];"
		"stmfd   SP!, {r0-r1, lr};"    // SP-3
		
		"adr     R0, .switch_thumb_here+1;"
		"bx	     R0;"

		/* after saving regs to stack, DISABLED (check if timer int occured) */
		".code 16;"
		".switch_thumb_here:;"

		/* if so, check if we should "sample" the registers */
		"ldr     R0, .ctsi_base;"
		"ldr     R0, [R0, #0];"
		"add     R0, R0, #0xFF;"
		"add     R0, R0, #0x01;"
		"ldrh    R1, [R0, #0x16];"
		"ldrh    R0, [R0, #0x18];"
		"cmp     R0, R1;"
		"bne     .no_timer_int_occured;"
		
		/* save timer values to config[2] and config[3] for debugging reasons */
		"push    {R2-R4};"
		"ldr     R0, .ctsi_base;"
		"ldr     R0, [R0, #0];"
		"add     R0, R0, #0xFF;"
		"add     R0, R0, #0x01;"
		"ldrh    R2, [R0, #0x12];"
		"ldrh    R3, [R0, #0x16];"
		
		"ldr     R0, .config;"
		"ldr     R0, [R0];"
		"str     R2, [R0, #0x08];"
		"str     R3, [R0, #0x0C];"
		
		/* calc difference of timer1 and save to config[4]*/
		"ldr     r3, .ctsi_base;"
		"ldr     r3, [R3, #0];"
		"ldrh    r2, [r3, #0x38];"
		"ldrh    r4, [r3, #0x3A];"
		"lsl     r2, r2, #0x04;"
		"orr     r2, r2, r4;"
		"ldr     r0, .systimer_val;"
		"ldr     r3, [r0];"
		"sub     r4, r2, r3;"
		"str     r4, [r0];"

		"pop     {R2-R4};"
		
		
		"ldr     R0, .freeram;"
		"ldr     R0, [R0];"
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
		"ldr     R1, [R1];"
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
		"ldr     R3, [R3];"
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
		"ldr     r0, [r0, #0];"
		"add     r0, r0, #0xff;"
		"add     r0, r0, #0x01;"
		"mov     r1, #0x80;"
		"strh    r1, [r0];"

		"ldmfd   SP!, {R0-R1, LR};"      // get "original" R0 R1 and LR
		"subs    pc, lr, #4;"

		".align 4;"
		".config:;"
		"	.word config;"
		".freeram:;"
		"	.word freeram;"
		".registers:;"
		"	.word registers;"


		".irq_stack:;"
		"	.word irq_stack_addr;"
		".ctsi_base:;"
		"	.word ctsi_base_addr;"
		);
}


void 
timing_step ( unsigned int delay )
{
	asm volatile ( 
		".code 16;"

		"push   { r0-r7, lr };"
		"mov    r5, %0;"
		"ldr    r0, .state;"
		"ldr    r0, [r0];"
		"ldr    r1, .timer_base;"
		"ldr    r1, [r1, #0];"
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
		"ldr    r6, .savestate;"
		"str    r4, [r0, #0];"
		
		/* and wait for timer int that saves our state */
		"timing_step_savestate:;"
		"cmp    r6, #0;"
		"bne    timing_step_savestate;"
		
		/* if we got 'restored' (R5 == 0), then immediately quit */
		"cmp    r5, #0;"
		"beq    timing_step_quit;"
		
		/* else signal to ignore next interrupt (normally not needed... but cleaner) */
		"ldr    r6, .skiprestore;"
		
		
		/*       now prepare stepping     */
		
		
		/* wait until timer advances */
		"timing_step_wait:;"
		"ldrh   r3, [r1, #0x12];"
		"cmp    r3, #0x00;"
		"bne    timing_step_wait;"
		
		/* let the handler know that timing is active */
		"str    r4, [r0, #0];"
		
		/* load register for timer 1 */
		"ldr    r0, .systimer_val;"
		"sub    r3, r1, #0x01;"
		"sub    r3, #0xff;"

		/* no more ignore next interrupt (R6 used as timer base now, not some magic number anymore) */
		"ldr    r6, .timer_base;"
		"ldr    r6, [r6, #0];"
		
		/* then set trigger time */
		"ldrh   r4, [r6, #0x16];"
		"add    r4, r4, #0x04;"
		"strh   r4, [r6, #0x18];"
		
		"ldrh   r2, [r3, #0x38];"
		"ldrh   r4, [r3, #0x3A];"
		"lsl    r2, r2, #0x04;"
		"orr    r2, r2, r4;"
		"str    r2, [r0];"
		
		/* since we have many cpu cycles per tick, first burn <delay>*8? cycles */
		"timing_step_loop:"
		"sub    r5, r5, #1;"
		"bne    timing_step_loop;"
		
		/* then call the ROM handler we want to debug */
		"ldr    r0, .romcall_r0;"
		"ldr    r0, [r0];"
		"ldr    r1, .romcall_r1;"
		"ldr    r1, [r1];"
		"ldr    r2, .romcall_r2;"
		"ldr    r2, [r2];"
		"ldr    r3, .romcall_r3;"
		"ldr    r3, [r3];"
		"ldr    r4, .romcall_r4;"
		"ldr    r4, [r4];"
		"ldr    r5, .romcall_r5;"
		"ldr    r5, [r5];"
		"ldr    r6, .romcall_r6;"
		"ldr    r6, [r6];"
		"ldr    r7, .romcall;"
		"ldr    r7, [r7];"
	
	
		"bl     bx_r7;"
		
		/* endless loop until the timer triggers */
		"ldr    r6, .overrun;"
		"timing_step_lock:"
		"b      timing_step_lock;"
		
		
		"timing_step_quit:;"
		/* clear saved state register */
		"ldr    r0, .state;"
		"ldr    r0, [r0];"
		"add    r0, r0, #0x08;"
		"mov    r1, #0x00;"
		"str    r1, [r0, #0];"
		
		"pop    { r0-r7, pc };"
		
				
	::"r" ( delay ) );

}

void 
timing_calib ( )
{
	asm volatile ( 
		".code 16;"
		
		"push   { r0-r7, lr };"
		"mov    r5, #0;"
		"ldr    r0, .state;"
		"ldr    r0, [r0];"
		"str    r5, [r0, #0];"
		"ldr    r1, .timer_base;"
		"ldr    r1, [r1, #0];"
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
		"ldr    r6, .signalvalue;"
		"str    r4, [r0, #0];"
		"ldr    r0, .systimer_val;"
		
		"ldrh   r2, [r1, #0x16];"
		"add    r2, r2, #0x04;"
		"strh   r2, [r1, #0x18];"
		
		"ldrh   r2, [r3, #0x38];"
		"ldrh   r4, [r3, #0x3A];"
		"lsl    r2, r2, #0x04;"
		"orr    r2, r2, r4;"
		"str    r2, [r0];"
		
		/* load registers so we have this delay also calibrate */
		"ldr    r0, .romcall_r0;"
		"ldr    r0, [r0];"
		"ldr    r1, .romcall_r1;"
		"ldr    r1, [r1];"
		"ldr    r2, .romcall_r2;"
		"ldr    r2, [r2];"
		"ldr    r3, .romcall_r3;"
		"ldr    r3, [r3];"
		"ldr    r4, .romcall_r4;"
		"ldr    r4, [r4];"
		"ldr    r4, .romcall_r5;" /* changed to R4 since R5 is used! */
		"ldr    r4, [r4];"
		"ldr    r4, .romcall_r6;" /* changed to R4 since R6 is used! */
		"ldr    r4, [r4];"
		"ldr    r7, .romcall;"
		"ldr    r7, [r7];"
		
		/* count up until int handler will set R5 to a negative value */
		"timing_calib_loop:"
		"add    r5, r5, #1;"
		"bpl    timing_calib_loop;"
		
		"pop    { r0-r7, pc };"
		
		
		".align 4;"
		".systimer_val:;"
		"	.word calib_systimer;"
		".signalvalue:;"
		"	.word 0xDEADBEEF;"
		".savestate:;"
		"	.word 0xDEADBABE;"
		".skiprestore:;"
		"	.word 0xBABEFACE;"
		".overrun:;"
		"	.word 0xC01DFACE;"
		".state:;"
		"	.word state;"
		".timer_base:;"
		"	.word timer_base_addr;"

		".romcall:;"
		"	.word romcall_addr;"
		".romcall_r0:;"
		"	.word romcall_r0;"
		".romcall_r1:;"
		"	.word romcall_r1;"
		".romcall_r2:;"
		"	.word romcall_r2;"
		".romcall_r3:;"
		"	.word romcall_r3;"
		".romcall_r4:;"
		"	.word romcall_r4;"
		".romcall_r5:;"
		"	.word romcall_r5;"
		".romcall_r6:;"
		"	.word romcall_r6;"
		".romcall_r7:;"
		"	.word romcall_r7;"
		
	 );

}


int 
int_enable_timer ()
{
	/* activate interrupt controller */
	strh ( CTSI_BASE + 0x0024, 0x0010 ); // disable IRQs
	strh ( CTSI_BASE + 0x0024, 0x0001 ); // generally enable IRQs
	strh ( CTSI_BASE + 0x0024, 0x0004 ); // activate IRQs
	
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



// this one must set up stack before it changes ARM mode
int
main ( )
{
	
	// use our own stack to make sure we have enough
	u32 old_stack = get_stack ();
	arm_mode_set ( 0x1F );
	set_stack ( OWN_STACK );

	code_main ( );
	
	// restore old stack pointer
	set_stack ( old_stack );

	return 1;
}


int 
code_main (  )
{
	freeram    = RAM_BASE + 0x0004B5A0;
	state      = RAM_BASE + 0x0004B5A4;
	saved      = RAM_BASE + 0x0004B5A8;

	registers  = RAM_BASE + 0x0004B5B0;
	storage    = RAM_BASE + 0x0004B6B0;
	
	config     = CONTROL_MEMORY_BASE;
	cpy_area   = CONTROL_MEMORY_BASE + 0x00001000;
	buffer     = CONTROL_MEMORY_BASE + 0x00001800;
	/* at +0x2000 this code will start */
	
	irq_stack_addr = IRQ_STACK;
	ctsi_base_addr = CTSI_BASE;
	timer_base_addr = CTSI_BASE + 0x0100;
	
	switch ( config[0] )
	{
	
		/* command 1 - say hello */
		case 1:
			arm_int_disable ( );
			
			intvec_init ( arm_irq_handler );
			int_init ( );
			int_enable_timer ( );
			cbus_unlock ( );
			
			timer_prepare ( );
			arm_int_enable ( );

			/* overclock little ;) */
			mcupll_set ( 3, 0 );
			
			config[1] = 1; // set "ok" flag
			break;
			
			
		/* command 2 - calibrate */
		case 2:
			/* clock down a little bit */
			mcupll_set ( 0, 3 );
			
			timing_calib ( );
			
			/* overclock little ;) */
			mcupll_set ( 3, 0 );
			
			config[1] = calib_value;
			config[4] = calib_systimer_diff;
			break;
		
		
		/* command 3 - execute code */
		case 3:
			/* clear buffers */
			//memset_32 ( 0x00005000, 0xffffffff, 0x29C/4 );
			memset_32 ( buffer, 0, 0x29C/4 );
			
			/* clock down a little bit */
			mcupll_set ( 0, 3 );
			
			config[1] = 1;
			calib_value = config[2];
			
			// try a few times
			{
				int loops = 50;
				
				// when int handler said that timer1 difference was not as expected, retry
				while ( loops-- && (config[1] != 0x02) )
					timing_step ( calib_value );
			}
			config[4] = calib_systimer;
			
			/* overclock little ;) */
			mcupll_set ( 3, 0 );
			break;
		
		
		/* unknown command */
		default:
			config[1] = 0xFF; // set error flag
			break;
	}
	
	return 0;
}
