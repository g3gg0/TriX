#define msec *1000
#define sec *1000 msec
#define COLOR(r,g,b) ((~r&0x0F)<<8|(~g&0x0F)<<4|(~b&0x0F))


#define data_buffer  (0x4C000)


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
#define B(x)				(S(0x11+x))
#define BPSR				(B(0x10))

#define FCHK_AREA_BAK		(storage+0x00)
#define FCHK_AREA_SNAP		(storage+0x40)

#define FCHK_AREA			((unsigned int*)0x0600C800)
#define FCHK_BLOCKPOS		((unsigned int*)0x0600C82C)
#define FCHK_DATADEST		((unsigned int*)0x0600C860)
#define FCHK_FCHKDEST		((unsigned int*)0x0600C8A0)

#define TIMER_DEST			(*(unsigned short*)0x6010118)
#define TIMER_ACT			(*(unsigned short*)0x6010116)
#define TIMER_SPEED			(*(unsigned short*)0x6010114)
#define TIMER_IRQ_ENABLE    (*(unsigned short*)0x6010012)

#define NOP					asm volatile ( "nop" )

#ifdef INT_STUFF

int _call_via_r9 ()
{
    asm volatile (  ".code 16;"
                    "mov pc, r9;" );
}

int _call_via_r3 ()
{
    asm volatile (  ".code 16;"
                    "mov pc, r3;" );
}


void memcpy_32 ( unsigned int *dst, unsigned int *src, unsigned int blocks )
{
	while ( blocks-- )
	{
		*dst = *src;
		dst++;
		src++;
	}
}

void memcpy_16 ( unsigned short *dst, unsigned short *src, unsigned int blocks )
{
	while ( blocks-- )
	{
		*dst = *src;
		dst++;
		src++;
	}
}

void memcpy_8 ( unsigned char *dst, unsigned char *src, unsigned int blocks )
{
	while ( blocks-- )
	{
		*dst = *src;
		dst++;
		src++;
	}
}
void irq_handler_replacement ()
{
    asm volatile (
					/*
					   totally weird thumb<->arm32 switching since our elf parser does not
					   work correctly for ARM branches yet :( so branches have to be done
					   in thumb mode.
				    */
					".code 32;"
					"stmfd   SP!, {R0-R1,LR};"       // save R0 and R1 since these get modified here
					"adr     R0, .switch_thumb_here+1;"
					"bx	     R0;"

					/* after saving regs to stack, DISABLED (check if timer int occured) */
					".code 16;"
					".switch_thumb_here:;"

					/* if so, check if we should "sample" the registers */
					"ldr     R0, .freeram;"
					"ldr     R1, [R0, #4];"
					"cmp     R1, #1;"             // start sample condition
					"bne     .no_timer_int_occured;"

					/* check if we are using timer int for our purposes - else give control to back to nokia */
/*					"ldr     R1, [R0, #8];"
					"cmp     R1, #1;"
					"bne     .no_timer_int_occured;"
*/
					/* if we should sample, set the sample trigger value to 2, so we do that just once */
					"mov     R1, #2;"             // already sampled condition
					"str     R1, [R0, #4];"
					"adr     R0, switch_to_arm;"
					"bx	     R0;"

					/* now in ARM mode, save all registers to our buffer */
					".code 32;"
					"switch_to_arm:;"
					"ldr     R1, .registers;"
					"add     R0, R1, #0x08;"
					"stmia   R0, { R2-R14 }^;"   // first save R2-R14 which are still unchanged
					"ldmfd   SP, { R2-R3, R4 };" // now get the LR (old PC) into R4 and old R0/R1 into R2/R3
					"add     R0, R1, #0x00;"
					"stmia   R0, { R2-R3 };"     // and save old R0/R1
					"add     R0, R1, #0x3C;"
					"stmia   R0, { R4 };"        // then save our LR (old PC)
					"mrs     R4, SPSR;"
					"add     R0, R1, #0x40;"
					"stmia   R0, { R4 };"        // then save our CPSR
<<<<<<< k_lcd_irq_count.c
/*
					"ldr     R4, .fchkregs;"     //
                    "add     R0, R1, #72;"
                    "ldmia   R4, { R2-R3, R5-R14 };"    //read 48 bytes
                    "stmia   R0, { R2-R3, R5-R14 };"    //store them
                    "ldmia   R4, { R2-R3, R5-R14 };"    //read 48 bytes
                    "stmia   R0, { R2-R3, R5-R14 };"    //store them*/
=======
//
//					"ldr     R4, .fchkregs;"     //
//                    "add     R0, R1, #72;"
//                    "ldmia   R4, { R2-R3, R5-R14 };"    //read 48 bytes
//                    "stmia   R0, { R2-R3, R5-R14 };"    //store them
//                    "ldmia   R4, { R2-R3, R5-R14 };"    //read 48 bytes
//                    "stmia   R0, { R2-R3, R5-R14 };"    //store them
>>>>>>> 1.15
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
					"ldr     R3, .registers;"
					"add     R0, R3, #0x40;"
					"ldmia   R0, { R1 };"            // restore our CPSR
					"msr     SPSR, R1;"
					"ldmfd   SP!, { R0-R1, R2 };"    // now get the original R0, R1 and LR
					"add     R4, R3, #0x00;"         // the start of our registers R0 and R1
					"ldmia   R4, { R0-R1 };"         // get the new registers R0 and R1
					"add     R4, R3, #0x3C;"         // the start of our register LR
					"ldmia   R4, { R2 };"            // get the new LR
					"stmfd   SP!, { R0-R1, R2 };"    // and immediately push them on stack
					"add     R0, R3, #0x08;"         // the start of our registers R2-R15
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
					"ldmfd   SP!, {R0-R1, LR};"      // get "original" R0 R1 and LR
					"stmfd   SP!, {R0-R3,R12,LR};"   // and save them on stack
					"ldr     R0, .original_handler;"
					"bx      R0;"

					".align 4;"
					/* this is the original interrupt handler PLUS 4 !!! */
					".original_handler:;"
					/* 6610 */
//					"	.word 0x012E55C4;"
                    /* 3100 */
                    "   .word 0x0130E2E4;"
					".freeram:;"
					"	.word 0x0004B5A0;"
					".registers:;"
					"	.word 0x0004B5B0;"
					".timer_dst:;"
					"	.word 0x6010118;"
					".timer_act:;"
					"	.word 0x6010116;"
					".fchkregs:;"
					"   .word 0x600c800;"
					:: );
}

static __inline void timer_int_restorestate ( )
{
	int i = 0;

	for ( i = 0; i<16; i++ )
		R(i) = S(i);

	CPSR = SPSR;
}

static __inline void timer_int_savestate ( )
{
	int i = 0;

	for ( i = 0; i<16; i++ )
		S(i) = R(i);

	SPSR = CPSR;
}

static __inline void timer_int_backup ( )
{
	int i = 0;

	/* save current FCHK area for further analysis */
	memcpy_32 ( FCHK_AREA_SNAP, FCHK_AREA, 0x40 );

	for ( i = 0; i<16; i++ )
		B(i) = R(i);

	BPSR = CPSR;
}

void timer_int_reg_mod ( )
{
	/* here check and modify the saved registers */


	if ( R(6) == 0xDEADBEEF )
	{
		/* the timer calibration routine requested signal */
		timer_int_backup ();
		R(6) = 0;
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
		timer_int_restorestate ();
		R(5) = 0;
	}
}

void timing_step ( unsigned int delay )
{
	asm volatile ( ";\
			.code 16;\
			\
			push   { r0-r7 };\
			mov    r5, %0;\
			ldr    r0, state;\
			ldr    r1, timer_dst;\
			ldr    r2, timer_act;\
			mov    r3, #0;\
			mov    r4, #1;\
			mov    r6, #0;\
			mov    r7, #0;\
			\
			\
			/*       start saving the current state     */\
			\
			/* get current timer value */\
			ldrh   r7, [r2, #0];\
			\
			/* wait until it advances */\
			timing_step_savestate_wait:;\
			ldrh   r3, [r2, #0];\
			cmp    r3, r7;\
			beq    timing_step_savestate_wait;\
			\
			/* then set trigger time */\
			add    r3, r3, #1;\
			strh   r3, [r1, #0];\
			\
			/* request state save */\
			ldr    r6, savestate;\
			str    r4, [r0, #0];\
			\
			/* and wait for timer int that saves our state */\
			timing_step_savestate:;\
			cmp    r6, #0;\
			bne    timing_step_savestate;\
			\
			/* if we got 'restored' (R5 == 0), then immediately quit */\
			cmp    r5, #0;\
			beq    timing_step_quit;\
			\
			/* else signal to ignore next interrupt (normally not needed... but cleaner) */\
			ldr    r6, skiprestore;\
			\
			\
			/*       now prepare stepping     */\
			\
			\
			/* get current timer value */\
			ldrh   r7, [r2, #0];\
			\
			/* wait until it advances */\
			timing_step_wait:;\
			ldrh   r3, [r2, #0];\
			cmp    r3, r7;\
			beq    timing_step_wait;\
			\
			/* then set trigger time */\
			add    r3, r3, #10;\
			strh   r3, [r1, #0];\
			str    r4, [r0, #0];\
			\
			/* no more ignore next interrupt */\
			mov    r6, #0x00;\
			\
			/* since we have many cpu cycles per tick, first burn <delay>*8? cycles */\
			timing_step_loop:\
			sub    r5, r5, #1;\
			cmp    R5, #0;\
			bne    timing_step_loop;\
			\
			/* then call the ROM handler we want to debug */\
			ldr    r0, table;\
			ldr    r1, romcall;\
			\
			/* set the signalling register values so we know, we're behind the call */\
			mov    r2, #0x55;\
			mov    r3, #0x55;\
			mov    r4, #0x55;\
			mov    r5, #0x55;\
			mov    r6, #0x55;\
			mov    r7, #0x55;\
			bl     bx_r1;\
			ldr    r0, signalvalue;\
			ldr    r1, signalvalue;\
			ldr    r2, signalvalue;\
			ldr    r3, signalvalue;\
			ldr    r4, signalvalue;\
			ldr    r5, signalvalue;\
			ldr    r6, signalvalue;\
			ldr    r7, signalvalue;\
			bl     neverhappen;\
			\
			timing_step_quit:;\
			/* clear saved state register */\
			ldr    r0, state;\
			add    r0, r0, #0x08;\
			mov    r1, #0x00;\
			str    r1, [r0, #0];\
			\
			pop    { r0-r7 };\
			" ::"r" ( delay ): "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7" , "memory" );

}


void timing_calib ( )
{
	asm volatile ( ";\
			.code 16;\
			\
			push   { r0-r7 };\
			mov    r5, #0;\
			ldr    r0, state;\
			str    r5, [r0, #0];\
			ldr    r1, timer_dst;\
			ldr    r2, timer_act;\
			mov    r6, #0;\
			mov    r7, #0;\
			\
			/* get current timer value */\
			mov    r4, #1;\
			ldrh   r7, [r2, #0];\
			\
			/* wait until it advances */\
			timing_calib_wait:;\
			ldrh   r6, [r2, #0];\
			cmp    r6, r7;\
			beq    timing_calib_wait;\
			\
			/* then set trigger value */\
			add    r6, r6, #10;\
			strh   r6, [r1, #0];\
			str    r4, [r0, #0];\
			ldr    r6, signalvalue;\
			\
			/* count up until we reach break condition */\
			timing_calib_loop:\
			add    r5, r5, #1;\
			cmp    R6, #0x00;\
			bne    timing_calib_loop;\
			b      timing_calib_quit;\
			\
			.align 4;\
			signalvalue:;\
			.word 0xDEADBEEF;\
			savestate:;\
			.word 0xDEADBABE;\
			skiprestore:;\
			.word 0xBABEFACE;\
			state:;\
			.word 0x0004B5A4;\
			timer_dst:;\
			.word 0x06010118;\
			timer_act:;\
			.word 0x06010116;\
			romcall:;\
			.word 0x0084001D;\
			table:;\
			.word 0x0900006C;\
			\
			timing_calib_quit:;\
			pop    { r0-r7 };\
			" ::: "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7" , "memory" );

}

void pseudo_abort_handler (  )
{
	set_light ( 0 );
	usleep ( 1 sec );
	set_light ( 1 );
	usleep ( 1 sec );
}


/*
void timer_handler ()
{

    asm volatile (  ".code 32;"
					"ldr     R0, __freeram__;"
					"stmia   R0!, { R0-R14 }^;"

					"cmp r0,r1;"
					"beq test;"
					"nop;"
					"bne test;"
					"test:;"

					"mov     R1, LR;"
					"stmia   R0!, { R1 };"
					"adr     R0, timer_handler_thumb+1;"
					"bx	     R0;"
					"__freeram__:;"
					".word 0x0005F668;"
					:: );
}
*/
void timer_handler_thumb ()
{
	// increase int counter
	INT_COUNT++;
/*
	// if we already got a start condition, now fix the cycle time
	if ( freeram[1] > 2 )
		freeram[1] |= 0x80000000;
	// start the measurement
	if ( freeram[1] == 1 )
		freeram[1] = 2;
		*/
	TIMER_DEST = TIMER_ACT + 0x10;
	TIMER_SPEED = 0x0f;  //speed up :)

}

void lcd_test();

void neverhappen ( )
{

	lcd_write ( 0, 0,  "CALLEE RETURNED", COLOR(15,0,0), 0);
	lcd_write ( 0, 10, "MUST NOT HAPPEN", COLOR(15,0,0), 0);

	usleep ( 1000 sec );
}

void init()
{
    int i;
    for ( i = 0; i < 4*0x20; i += 4 )
        *(int*)(freeram+i) = i/4;
}

void nop ( )
{
    asm volatile ( "nOp" );
}

void lcd_test_f ()
{
    lcd_write ( 5, 5, "works", COLOR (0,0,0), 0 );
}

int already_buffered ()
{
    int *start = (int*)(data_buffer+12);
    int *end = (int*)(*(int*)(data_buffer+8));

    while ( start < end )
    {
        if ( start[15] == B(15) )
            return 1;
        start = &start[16 + 1 + 0x30 + 0x10 + 1];
    }

    return 0;
}

void init_databuffer ()
{
    if ( *(int*)data_buffer != 0x0BADC0DE )
    {
        *(int*)data_buffer = 0xBADC0DE;
<<<<<<< k_lcd_irq_count.c
        *(int*)(data_buffer+4) = 0xCC8C*8;   //delay
=======
        *(int*)(data_buffer+4) = 0x280*8;   //delay
>>>>>>> 1.15
        *(int*)(data_buffer+8) = data_buffer+12;    //data_pointer
    }
}

void save_regs ()
{
    int i;
    int total = 0;
    int *buf = (int*)(*(int*)(data_buffer+8));

    if ( already_buffered () )
        return;

    //save register
    for ( i = 0; i < 16; i++ )
        buf[total+i] = B(i);
    total += 16;

    buf[total] = B(16);
    total++;

    for ( i = 0; i < 0x30; i++ )    //0xc0 bytes
        buf[total+i] = *(int*)(0x600c800 + i * 4);
    total += 0x30;

    for ( i = 0; i < 0x10; i++ )
        buf[total+i] = *(short*)(0x6100000 + i*2);
    total += 0x10;

    buf[total] = 0x55AA55AA;
    total++;

    *(int*)(data_buffer+8) += total * 4;

}

void save_delay ( unsigned int delay )
{
    *(int*)(data_buffer+4) = delay;
}

unsigned int get_delay ()
{
    return *(int*)(data_buffer+4);
}

int buffering_is_active ()
{
    if ( *(int*)data_buffer == 0xBADC0DE && get_delay() > 0x240*8 )
        return 1;
    else
        return 0;
}

int key_pressed ( )
{
	if ( get_uem ( 0x1F << 16 | 0xFFFF ) & 0x02 )
		return 1;
	return 0;
}

void lcd_test ()
{
    unsigned int max = 0x00000000;
    unsigned int min = 0xFFFFFFFF;
    unsigned int i;
	unsigned int enabled = 0;
	unsigned int locked = 0;

<<<<<<< k_lcd_irq_count.c
	unsigned int delay; // = 0xCC8B * 0x08;
	//unsigned int delay = 0x229 * 0x08;  //3100
=======
	//unsigned int delay = 0x216 * 0x08;
	//define it in init_buffer()
	unsigned int delay = 0x229 * 0x08;  //3100
>>>>>>> 1.15
	unsigned int startdelay;

    char buffer[32];
    int y = 0;

	int just_calib = 0;

	INT_COUNT = 0;
	SAVED = 0;
	STATE = 0;


	//set irq and fiq stack pointer
	set_irqfiq_SP();

    //register timer irq
    register_irq ( 0xb, timer_handler_thumb + 1, 0x408 );

    //set timer startup values
    TIMER_SPEED = 0x9f;
    TIMER_DEST = 0x200;
    TIMER_IRQ_ENABLE = 0x80;

    //enable IRQ and FIQ in CPSR
    enable_int_CPSR();

	lcd_init ();

	init_databuffer();
	startdelay = delay = get_delay();

//      phone checks POWER_KEY after fchk! if noit pressed phone will turn off
//    disable_int();
//    for ( i = 0; i < 0x1f; i++ )
//    {
//        *(int*)(0x25ACC + i*8) = 0;
//        *(int*)(0x25ACC + i*8 + 4) = 0;
//    }
//    enable_int();
//    return;

    //enable IRQs
    enable_int();

    //set MCU speed (multiplicator and div)
    pll_set ( 0x6009000, 8, 0 );

<<<<<<< k_lcd_irq_count.c
    *(short *)0x6010006 = 0xff;
/*
	while ( 1 )
	{
	    for (i = 0; i < 0xc; i++ )
	    {
            sprintf ( buffer, "%02X %04X %04X %04X", 3*i, get_uem ( 3*i << 16 | 0xFFFF ), get_uem ( 3*i+1 << 16 | 0xFFFF ), get_uem ( 3*i+2 << 16 | 0xFFFF ));
            lcd_write ( 2, i*10, buffer, COLOR ( 15,0,0 ), 0 );
	    }
	    sprintf ( buffer, "%02X %04X", 0x24, get_uem ( 0x24 << 16 | 0xFFFF ) );
        lcd_write ( 2, 0xc *10, buffer, COLOR ( 15,0,0 ), 0 );
        set_uem ( 0x0900FF, 0x000008 );
	    //wdt_reset(2);  //wdt reset
	}
*/
=======

//    *(short *)0x6010006 = 0xff;
//	while ( 1 )
//	{
//	    for (i = 0; i < 0xc; i++ )
//	    {
//            sprintf ( buffer, "%02X %04X %04X %04X", 3*i, get_uem ( 3*i << 16 | 0xFFFF ), get_uem ( 3*i+1 << 16 | 0xFFFF ), get_uem ( 3*i+2 << 16 | 0xFFFF ));
//            lcd_write ( 2, i*10, buffer, COLOR ( 15,0,0 ), 0 );
//	    }
//	    sprintf ( buffer, "%02X %04X", 0x24, get_uem ( 0x24 << 16 | 0xFFFF ) );
//        lcd_write ( 2, 0xc *10, buffer, COLOR ( 15,0,0 ), 0 );
//        set_uem ( 0x0900FF, 0x000008 );
//	    //wdt_reset(2);  //wdt reset
//	}

>>>>>>> 1.15

	//at_printf ( "test" );
	//at_send_success ( 0xfe, 0xe );

    //test_func();

    while ( 1 )
    {


		if ( !just_calib )
		{
			// dump 2 variables and 16 registers
			if ( (B(15) & 0xFFF00000) != 0x01400000   )
			{
				sprintf ( buffer, "%08X %08X", INT_COUNT, STATE);
			    save_regs();
				//for ( i = 0; i < 0x09; i++ )

				lcd_write ( 0, 0, buffer, COLOR(15,0,0), 0);
				for ( i = 0; i < 0x08; i++ )
				{
					sprintf ( buffer, "%08X %08X", B(2*i), B(2*i+1) );
					lcd_write ( 0, 12+i*10, buffer, COLOR(15,0,0), 0);
				}
			}
<<<<<<< k_lcd_irq_count.c
			/*
			for ( i = 0; i < 0x40; i++ )
			{
				if ( FCHK_AREA_BAK[i] != FCHK_AREA_SNAP[i] )
				{
					sprintf ( buffer, "%02X:%08X %08X", i*4, FCHK_AREA_BAK[i], FCHK_AREA_SNAP[i] );
					lcd_write ( 0, 120, buffer, COLOR(0,0,0), 0);
					usleep ( 1 sec );
					lcd_write ( 0, 120, "                                        ", COLOR(0,0,0), 0);
					usleep ( 1 sec );
					if ( i*4 != 0x44 && i*4 != 0x48 )
						delay++;
				}
			}
			*/
=======
//			for ( i = 0; i < 0x40; i++ )
//			{
//				if ( FCHK_AREA_BAK[i] != FCHK_AREA_SNAP[i] )
//				{
//					sprintf ( buffer, "%02X:%08X %08X", i*4, FCHK_AREA_BAK[i], FCHK_AREA_SNAP[i] );
//					lcd_write ( 0, 120, buffer, COLOR(0,0,0), 0);
//					usleep ( 1 sec );
//					lcd_write ( 0, 120, "                                        ", COLOR(0,0,0), 0);
//					usleep ( 1 sec );
//					if ( i*4 != 0x44 && i*4 != 0x48 )
//						delay++;
//				}
//			}
>>>>>>> 1.15
		}
		else
		{
			sprintf ( buffer, "%08X %08X", B(15), B(0));
			lcd_write ( 0, 20, buffer, COLOR(0,0,0), 0);

			sprintf ( buffer, "  lag: %X        ", R(5) );
			lcd_write ( 0, 30, buffer, COLOR(0,0,0), 0);
			sprintf ( buffer, "   (%X-%X)            ", min, max );
			lcd_write ( 0, 40, buffer, COLOR(0,0,0), 0);

			if ( B(5) )
			{
				if ( B(5) > max )
					max = B(5);
				if ( B(5) < min )
					min = B(5);
			}
		}


		// show the current timer delay/8
        sprintf ( buffer, "delay %08X", delay / 0x08 );
        lcd_write ( 10, 100, buffer, COLOR ( 0,0,15 ), 0 );

//        sprintf ( buffer, "cpsr %08X", hw_arm_cpsr_get() );
//        lcd_write ( 2, 100, buffer, COLOR ( 0,0,0 ), 0 );

        //draw timer values
		sprintf ( buffer, "timer: %04X %04X", TIMER_ACT & 0xFFFF, TIMER_DEST & 0xFFFF );
        lcd_write ( 4, 110, buffer, COLOR ( 0, 15, 0 ), 0 );

		if ( key_pressed() )
		{
			if ( !enabled )
				enabled = 1;
			else 
			{
				enabled = 0;
				delay-=4;
			}

		}

		do
		{
			sprintf ( buffer, "screen %08X  %s", y++, enabled?"EN":"  " );
			lcd_write ( 0, 90, buffer, COLOR ( 0,0,15 ), 0 );
		} while ( key_pressed () );

		// wait 1 int, then start sampling using delay/8 (so every 8 cycles the delay is decreased by 1)
		if ( INT_COUNT && enabled && (STATE == 0) )
		{
//			memcpy_32 ( FCHK_AREA, FCHK_AREA_BAK, 0x40 );
            //don't use it, if nokia should continue later ..
			if ( !just_calib )
				timing_step ( delay / 0x08 );
			else
				timing_calib ();
		}


		// after 3 screens, rerun the timer measurement
		if ( (y & 0x03) == 0 && STATE )
		{
			STATE = 0;
//			delay--;
			save_delay ( delay );
			if ( startdelay - delay > 24 )
			{
			    //asm volatile ( "mov r0, #0; bx r0;" );  //reset
//			    *(short*)0x6010000 = 8; //reset - 4 will only start 4 times again... 8 as often as we wish
			    // also has todo something with WD ? 
			    // load n* firmware
			    // reset with at cmd
			    // press 5 at startup
			    // go on
			}
		}

//        if ( y > 0x25 )
//        {
//            asm volatile ( "mov r0, #0; bx r0;" );  //reset
//        }

//		//stop and load nokia firmware
//		if ( delay < 0x200*8 )
//		{
////                disable_int();
////                /* clear all irq handler */
////                for ( i = 0; i < 0x1f; i++ )
////                {
////                    *(int*)(0x25ACC + i*8) = 0;
////                    *(int*)(0x25ACC + i*8 + 4) = 0;
////                }
//////                TIMER_IRQ_ENABLE = 0;
//////                TIMER_DEST = 0;
//            *(short*)0x6010000 = 8;
////                enable_int();
////                TIMER_SPEED = 0x9f;
////                TIMER_DEST = 0x500;
////                register_irq ( 0xb, 0, 0x408 );
//
//                // !!!! PRESS POWER KEY !!!!!
//                break;
//		}
//		freeram[1] = 0;
		//test_func ( 0x12003f, 0 );
		//wdt_reset(2);
		/*
            1 : MCU
            2 : UEM
            3 : both
        */
    }
}


void irq_main_handler_hook ( int func, int irq_tbl_offset )
{
    unsigned int *buffer = (int*)freeram;
//    unsigned int cpsr_saved;


    //increase irq counter
    buffer[irq_tbl_offset/8]++;
    //lcd_test_string(func);
//    return;
    asm volatile (  "mov r0, %0;"
                    "mov r1, %1;"
                    "bl bx_r0;" :: "r" ( func ), "r" ( irq_tbl_offset ) );

//    cpsr_saved = hw_arm_cpsr_get ();
//    hw_arm_cpsr_set ( cpsr_saved );

}

void show_fchk_area ()
{
    char buffer[32];

	lcd_init ();

    while ( 1 )
    {
		sprintf ( buffer, "blk %08X", FCHK_BLOCKPOS[0] );
		lcd_write ( 0, 20, buffer, COLOR(0,0,0), 0);

		sprintf ( buffer, "%08X %08X", FCHK_DATADEST[0], FCHK_DATADEST[1] );
		lcd_write ( 0, 30, buffer, COLOR(0,0,0), 0);

		sprintf ( buffer, "%08X %08X", FCHK_FCHKDEST[0], FCHK_FCHKDEST[1] );
		lcd_write ( 0, 40, buffer, COLOR(0,0,0), 0);
    }


}


void fchk_emulate ( )
{

	asm volatile (
		"\
		push    { R0-R7 };\
		\
		ldr     R0, fchk_data;\
		ldr     R1, fchk_datadst;\
		ldr     R2, fchk_blkdest;\
		ldr     R3, fchk_chkaddr;\
		ldr     R4, fchk_chkdest;\
		ldr     R5, fchk_blocks;\
		\
		/* copy the FCHK value */\
		ldr     R6, [R3, #0];\
		str     R6, [R4, #0];\
		ldr     R6, [R3, #4];\
		str     R6, [R4, #4];\
		\
		mov     R6, #0;\
		fchk_emulate_copyloop:;\
		ldrh    R3, [R0, #0];\
		lsl     R3, R3, #0x10;\
		ldrh    R4, [R0, #2];\
		orr     R3, R3, R4;\
		str     R3, [R1, #0];\
		\
		ldrh    R3, [R0, #4];\
		lsl     R3, R3, #0x10;\
		ldrh    R4, [R0, #6];\
		orr     R3, R3, R4;\
		str     R3, [R1, #4];\
		\
		add     R0, R0, #8;\
		add     R6, R6, #1;\
		str     R6, [R2, #0];\
		cmp     R6, R5;\
		bne     fchk_emulate_copyloop;\
		\
		pop     { R0-R7 };\
		b       fchk_emulate_quit;\
		\
		.align 4;\
		fchk_data:;\
		.word 0x01000100;\
		fchk_datadst:;\
		.word 0x0600C860;\
		fchk_blkdest:;\
		.word 0x0600C82C;\
		fchk_chkaddr:;\
		.word 0x0900006C;\
		fchk_chkdest:;\
		.word 0x0600C8A0;\
		fchk_blocks:;\
		.word 0x0001FFE0;\
		\
		fchk_emulate_quit:;\
		" );
}

int  set_light ( int val )
{
	hw_light_set_ratio ( (val>0)?100:1 );
	hw_light_set_pwm ( (val>0)?100:1 );
}

void uem_send_hook ( int r0, int r1 )
{
    asm volatile ( "push {r0-r7};" );
//    asm volatile ( "mov %0, r5; " : "=r"(r0) : );
//
//    if ( *(int*)freeram == 0 )
//    {
//        *(int*)(*(int*)((char*)freeram+4)) = r0;
//        *(int*)((*(int*)((char*)freeram+4))+4) = r1;
//        *(int*)((char*)freeram+4) += 8;
//    }
    asm volatile ( "pop {r0-r7};" );
}


void fchk_hook ( unsigned int table, unsigned int call )
{
    int i;

    set_light ( 1 );
/*
	for ( i = 0; i < 0x40; i++ )
		FCHK_AREA[i] = 0xDEADDEAD;
	for ( i = 0; i < 0x40; i++ )
		freeram[i] = 0;
<<<<<<< k_lcd_irq_count.c

	memcpy_32 ( FCHK_AREA_BAK, FCHK_AREA, 0x40 );
	memcpy_32 ( FCHK_AREA_SNAP, FCHK_AREA, 0x40 );
=======
*/
//	memcpy_32 ( FCHK_AREA_BAK, FCHK_AREA, 0x40 );
//	memcpy_32 ( FCHK_AREA_SNAP, FCHK_AREA, 0x40 );
>>>>>>> 1.15

    *(int*)freeram = 0;
    *(int*)((char*)freeram+4) = (int)freeram+8;

*/
        lcd_test ();    //if key 5 pressed load nokia firmware

//	fchk_emulate ();
	bx_r1 ( table, call );
//	show_fchk_area ();
//    bx_r1 ( table, call );
<<<<<<< k_lcd_irq_count.c
/*
=======

/*
    to start fsig rom sniffing press key 5 (3100)
    if you press nothing nokia FW is loaded
    key 5 not gen_io 2 ? -> comment it
*/

>>>>>>> 1.15
    if ( !hw_io_get_pin ( 2 ) || buffering_is_active() )
    {
        freeram[-1] = 1;
        lcd_test ();    //if key 5 pressed load nokia firmware
    }
    else
    {
        freeram[-1] = 0;
        bx_r1 ( table, call );

    }
	*/
}
#else

int  set_light ( int val )
{
	hw_light_set_ratio ( (val>0)?100:1 );
	hw_light_set_pwm ( (val>0)?100:1 );
}

// Generated by BreakPoint Software's Hex Workshop v4.20
//   http://www.hexworkshop.com
//   http://www.bpsoft.com
//
//  Source File: NokiaEmu_patched.bin
//         Time: 04.05.2007 22:10
// Orig. Offset: 300 / 0x00000000
//       Length: 1008 / 0x000003F0 (bytes)
unsigned char rawData[1008] =
{
    0x16, 0x18, 0x97, 0x17, 0x6E, 0x62, 0x43, 0xDC, 0x8E, 0x7F, 0x45, 0x77, 0x1C, 0x38, 0xB8, 0xAB, 
    0x1B, 0xC0, 0x7C, 0xB0, 0x74, 0xE1, 0xF6, 0x46, 0x44, 0xA7, 0x1F, 0xEB, 0xAD, 0x26, 0x90, 0x0F, 
    0x23, 0x3C, 0x74, 0x77, 0xCD, 0x63, 0xA0, 0x59, 0x7A, 0x75, 0xF3, 0x7F, 0x31, 0xC9, 0x6A, 0x80, 
    0x23, 0x49, 0xD3, 0x80, 0x4D, 0xBA, 0x94, 0x52, 0xA4, 0x95, 0x3E, 0x06, 0x60, 0x2B, 0x39, 0xE4, 
    0xD7, 0xAE, 0x18, 0xFD, 0xB5, 0x00, 0xD7, 0x54, 0xA8, 0x47, 0x0A, 0x90, 0x50, 0x4F, 0x1B, 0x22, 
    0xBB, 0xB9, 0xB1, 0xE3, 0x9A, 0x09, 0x72, 0x0E, 0xBD, 0x40, 0x2E, 0x3B, 0x6F, 0xC7, 0x5D, 0x83, 
    0x66, 0x3F, 0x05, 0x04, 0x1D, 0x6A, 0x38, 0x97, 0x31, 0x2D, 0xE6, 0xF8, 0xDB, 0xB1, 0x7E, 0x96, 
    0x09, 0xD6, 0x61, 0x45, 0x31, 0x45, 0xA9, 0xC3, 0x1E, 0xAC, 0xCF, 0x9C, 0x11, 0x86, 0x7E, 0x79, 
    0x83, 0xC0, 0x57, 0x81, 0x5D, 0xBB, 0xDC, 0x5F, 0x2B, 0x55, 0xF4, 0x0B, 0x0B, 0xBA, 0x10, 0x56, 
    0xDB, 0xBF, 0xBC, 0x11, 0x56, 0xAF, 0x83, 0x3F, 0xCE, 0xD2, 0x49, 0xFD, 0xA7, 0x67, 0xBA, 0x5D, 
    0xB0, 0x32, 0x5B, 0x37, 0x42, 0x88, 0xEE, 0x26, 0x8A, 0x67, 0x9C, 0xF3, 0x6A, 0x1E, 0xBD, 0x65, 
    0xFC, 0x84, 0x18, 0x1D, 0xA9, 0x6E, 0xC6, 0x38, 0x38, 0xA5, 0xF4, 0xE3, 0x3F, 0x1F, 0x12, 0x08, 
    0x3D, 0x27, 0x89, 0x5A, 0x0D, 0x5C, 0x6C, 0x7B, 0x33, 0x2B, 0xCE, 0x03, 0x73, 0xF7, 0xC8, 0xCA, 
    0x05, 0xCF, 0xF8, 0x26, 0x05, 0xE7, 0x9D, 0xFD, 0x60, 0x87, 0x6D, 0xBD, 0x34, 0x84, 0xE0, 0xF5, 
    0x5F, 0x98, 0x1C, 0xB3, 0xE9, 0xCA, 0xD5, 0x60, 0x07, 0x05, 0x80, 0x26, 0x63, 0xB8, 0x78, 0xBD, 
    0x1C, 0x0F, 0x0F, 0x24, 0xF3, 0x5F, 0x67, 0x69, 0x1B, 0x4C, 0xCF, 0x65, 0x94, 0xC2, 0x42, 0x3C, 
    0x03, 0xB5, 0xD3, 0x8C, 0x2F, 0x8E, 0xC0, 0x4A, 0x6C, 0x31, 0x97, 0xE8, 0xDE, 0x8F, 0x6E, 0x09, 
    0xDE, 0x20, 0xE0, 0xB9, 0x60, 0xF9, 0x11, 0x60, 0x34, 0xF2, 0xDE, 0x56, 0xFD, 0x2C, 0x35, 0x63, 
    0x51, 0x7C, 0x84, 0x71, 0xBB, 0x26, 0xFF, 0x95, 0x36, 0xD3, 0xB3, 0xC2, 0xC9, 0x6A, 0x9C, 0xC1, 
    0xA8, 0xB6, 0xF1, 0xD8, 0x6D, 0x97, 0x1D, 0x0C, 0xF0, 0x08, 0x3E, 0xF7, 0xF9, 0xB5, 0xC5, 0x2C, 
    0x84, 0x2F, 0x9D, 0xE0, 0x3E, 0x42, 0x5A, 0xA7, 0x76, 0x36, 0x8C, 0x1D, 0x82, 0x91, 0xD5, 0x87, 
    0x0C, 0xED, 0x93, 0x3D, 0x3F, 0x10, 0x32, 0x88, 0x4E, 0xBB, 0x29, 0x56, 0xF8, 0xA8, 0x52, 0xC5, 
    0x2E, 0x03, 0x43, 0xDC, 0xE4, 0x77, 0x40, 0x1D, 0x5A, 0xB9, 0x31, 0x3F, 0xBF, 0x98, 0xB0, 0x90, 
    0xE8, 0x96, 0xA6, 0x21, 0xE5, 0x1E, 0xE4, 0x84, 0x2B, 0xA4, 0x73, 0x3F, 0x1C, 0x9D, 0x28, 0xF1, 
    0x9F, 0xB8, 0x5D, 0x28, 0xC9, 0x92, 0x93, 0x3D, 0x31, 0xB9, 0x70, 0x7B, 0x32, 0xC5, 0xFC, 0x73, 
    0x59, 0xBA, 0x28, 0x85, 0xA5, 0x94, 0x89, 0x95, 0x45, 0x53, 0x1E, 0x7E, 0x41, 0xC5, 0xBC, 0x96, 
    0x9A, 0x6C, 0x31, 0x09, 0xB5, 0x66, 0x4F, 0x16, 0x06, 0xED, 0x37, 0xDA, 0x70, 0x4B, 0x50, 0x79, 
    0xAD, 0xDA, 0xE7, 0xC1, 0xD9, 0xF1, 0xC4, 0x2C, 0x2D, 0x21, 0x1F, 0x67, 0xC8, 0x6C, 0x83, 0x0C, 
    0x86, 0xC8, 0xE0, 0x70, 0x01, 0x22, 0x35, 0x04, 0x1F, 0x88, 0x4E, 0x60, 0xA0, 0xD3, 0x61, 0x33, 
    0xB9, 0xFA, 0x0F, 0x87, 0xE0, 0x47, 0xEA, 0x2C, 0xB7, 0x95, 0x2C, 0xFB, 0x9E, 0x31, 0x3A, 0x3F, 
    0x87, 0xD0, 0xD8, 0xCB, 0x03, 0xDC, 0x6C, 0xD1, 0x20, 0xED, 0x27, 0xAF, 0xE0, 0x47, 0xB9, 0xED, 
    0x75, 0x28, 0x51, 0x2B, 0x09, 0xFC, 0xA0, 0xBE, 0x14, 0xA9, 0x50, 0xCC, 0x8E, 0x3C, 0x06, 0xCB, 
    0xFD, 0xC6, 0x5E, 0xEB, 0xFE, 0x50, 0x02, 0x98, 0x0F, 0xBA, 0xF6, 0x82, 0xA1, 0x03, 0x85, 0x7F, 
    0xC6, 0x03, 0x25, 0xFC, 0x51, 0xBA, 0x5E, 0xD9, 0x97, 0x0E, 0xF6, 0x7B, 0x38, 0x40, 0xA5, 0x48, 
    0xA4, 0x2F, 0x0F, 0x01, 0x4D, 0xDD, 0xD4, 0xE4, 0xC5, 0xA7, 0x9E, 0xA1, 0x8C, 0x94, 0x83, 0x74, 
    0xEC, 0x38, 0x83, 0xBA, 0x53, 0x91, 0x33, 0x8B, 0x18, 0x69, 0x5B, 0xC9, 0x35, 0x71, 0xB9, 0xDA, 
    0x61, 0x47, 0x88, 0xA3, 0x0B, 0xE4, 0x57, 0xD3, 0x15, 0x60, 0xAF, 0xF4, 0xE1, 0x7C, 0x3A, 0x96, 
    0x4A, 0x9E, 0xEA, 0xA7, 0xC9, 0xCB, 0x6A, 0x6C, 0xEF, 0xE7, 0x80, 0xE9, 0x90, 0x10, 0x76, 0x35, 
    0xCD, 0x47, 0x5A, 0xF3, 0xDA, 0x56, 0x00, 0xEC, 0x48, 0xC6, 0x97, 0x85, 0x95, 0x81, 0xC6, 0xBD, 
    0x26, 0x9E, 0x30, 0xEF, 0xDB, 0x7A, 0x3A, 0x01, 0x16, 0x50, 0x99, 0x02, 0x40, 0x8B, 0xE3, 0x50, 
    0x72, 0x30, 0x6B, 0xF3, 0xB5, 0x58, 0xDF, 0x58, 0x8D, 0xA1, 0xFE, 0x7B, 0x0D, 0x72, 0x40, 0xCA, 
    0x1C, 0x28, 0xE9, 0x9B, 0xF3, 0x5A, 0xDF, 0x3F, 0x96, 0xFF, 0x5D, 0xEF, 0x57, 0x63, 0x8D, 0x02, 
    0x3F, 0xBD, 0xF8, 0xE3, 0x19, 0xB3, 0x4A, 0x01, 0xA3, 0x78, 0x94, 0xC0, 0xEC, 0xAD, 0x96, 0x7D, 
    0x73, 0xD8, 0xE2, 0x2B, 0x22, 0x04, 0x4B, 0x2B, 0x3D, 0x8E, 0x52, 0xDB, 0x40, 0xBD, 0xE2, 0x76, 
    0xC2, 0xC5, 0x68, 0x2C, 0xCE, 0x31, 0x78, 0x88, 0xD3, 0xFF, 0xA2, 0xE8, 0x2D, 0x7C, 0x79, 0x76, 
    0x74, 0x0F, 0x5D, 0x5E, 0x1B, 0x00, 0x35, 0x59, 0xE0, 0xBE, 0xA3, 0x1A, 0x94, 0x23, 0x70, 0xCF, 
    0x69, 0x25, 0xDA, 0x88, 0xD7, 0x42, 0x49, 0x37, 0x33, 0x61, 0x16, 0x8B, 0x20, 0x68, 0x56, 0x31, 
    0xF6, 0x99, 0x64, 0xB4, 0xBA, 0xA4, 0x1C, 0xC1, 0x1B, 0xF7, 0x3F, 0xF6, 0xFD, 0x18, 0x63, 0xCF, 
    0x16, 0x70, 0x61, 0xBB, 0x11, 0xF6, 0x56, 0xA6, 0x9F, 0xFF, 0x19, 0x00, 0x3E, 0xDC, 0xDA, 0x41, 
    0x1D, 0x43, 0x40, 0xF5, 0x08, 0x3B, 0x2A, 0xC2, 0x9E, 0x0B, 0xAF, 0xCF, 0x79, 0xFE, 0x19, 0x89, 
    0x96, 0x2A, 0xF0, 0xED, 0x46, 0xD9, 0x98, 0x6F, 0x8E, 0xB2, 0xDE, 0x8E, 0xF1, 0xFA, 0xEB, 0x13, 
    0xF4, 0x52, 0xCB, 0x1C, 0xA9, 0xD4, 0xD4, 0xE0, 0x30, 0x5C, 0x13, 0xE5, 0xAE, 0x11, 0x0D, 0x6B, 
    0x45, 0xD2, 0x5F, 0xAB, 0x30, 0xA9, 0xAD, 0x5A, 0xD4, 0xA6, 0x10, 0x7E, 0xAB, 0x1B, 0x3E, 0xE7, 
    0x51, 0xB8, 0xF1, 0x54, 0xBD, 0xE3, 0x2F, 0x8E, 0xA9, 0x4C, 0xA3, 0x79, 0x8A, 0x73, 0x42, 0x6E, 
    0x0D, 0xB8, 0x22, 0x61, 0x37, 0x24, 0xF8, 0x22, 0x08, 0x7E, 0xF2, 0x73, 0x2E, 0x3D, 0xA8, 0x8A, 
    0xA8, 0x1B, 0x76, 0x83, 0xCD, 0xD6, 0x93, 0x09, 0x7E, 0x22, 0xAF, 0x02, 0x15, 0xA1, 0x6A, 0xF0, 
    0xEC, 0x17, 0xFE, 0xA9, 0x56, 0xF1, 0xBD, 0x7C, 0xD2, 0x74, 0x7F, 0x72, 0x83, 0x20, 0x3E, 0xD6, 
    0x4E, 0x47, 0xF6, 0x19, 0x71, 0xE0, 0xA3, 0x1E, 0xCE, 0xF2, 0x99, 0xD7, 0x59, 0xA8, 0x59, 0xAC, 
    0xAF, 0xB5, 0x5E, 0x68, 0xC1, 0x4A, 0xE7, 0xB3, 0x1C, 0x26, 0xBD, 0xC9, 0xD5, 0x50, 0xEB, 0xBC, 
    0x56, 0xC4, 0x0C, 0x80, 0xD8, 0xB4, 0xD8, 0x7C, 0xF5, 0x89, 0xB1, 0x98, 0x35, 0xDE, 0x6E, 0x89, 
    0xD7, 0xF8, 0x75, 0x9D, 0xDE, 0x1B, 0x35, 0x3B, 0xFD, 0x9C, 0xC4, 0x0A, 0x63, 0x8B, 0x57, 0xE3, 
    0x83, 0xC9, 0xF3, 0x32, 0x3F, 0x02, 0xB5, 0x04, 0xEA, 0x10, 0x0D, 0x3C, 0x9B, 0x27, 0x52, 0xCA, 
    0xD5, 0x13, 0x47, 0x36, 0xCD, 0x39, 0xF1, 0x13, 0x42, 0xA1, 0xB4, 0xA2, 0x97, 0xFD, 0xEF, 0xAF, 
} ;

void memcpy_8 ( unsigned char *dst, unsigned char *src, unsigned int blocks )
{
	while ( blocks-- )
	{
		*dst = *src;
		dst++;
		src++;
	}
}

void fchk_hook ( unsigned int table, unsigned int call )
{
    char *buffer = 0x03000000;
	char textbuf[32];


	set_light ( 1 );

	lcd_init ();

	sprintf ( textbuf, "memcpy" );
    lcd_write ( 10, 100, textbuf, COLOR ( 0,0,15 ), 0 );

	memcpy_8 ( buffer, rawData, 1008 );

	sprintf ( textbuf, "before: %02X %02X %02X %02X", buffer[0], buffer[1], buffer[2], buffer[3] );
    lcd_write ( 10, 80, textbuf, COLOR ( 0,0,15 ), 0 );

	sprintf ( textbuf, "decode" );
    lcd_write ( 10, 100, textbuf, COLOR ( 0,0,15 ), 0 );

	asm volatile (  "mov r0, %0;"
                    "mov r1, %1;"
                    "mov r2, %2;"
                    "mov r3, %3;"
					"ldr     R5, .decrypter;"
					"nop;"
					"mov lr, pc;"
					"bx      R5;"
					"b       decr_ret;"
					".align 4;"
					".decrypter:;"
					"	.word 0x84000F;" 
					"decr_ret:;"
					:: "r" ( buffer ), "r" ( 0x168 ), "r" ( &buffer[0x168] ), "r" ( 0x20 ) : "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7" , "memory" );

	sprintf ( textbuf, "after : %02X %02X %02X %02X", buffer[0], buffer[1], buffer[2], buffer[3] );
    lcd_write ( 10, 100, textbuf, COLOR ( 0,0,15 ), 0 );


	usleep ( 500 sec );

	bx_r1 ( table, call );
}
#endif