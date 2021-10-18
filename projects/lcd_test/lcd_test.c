

#include "hw_defines.h"
#include "base.h"
#include "lcd.h"



// this one must set up stack before it changes ARM mode
int
main ( )
{
	
#ifdef OWN_STACK
	// use our own stack to make sure we have enough
	u32 old_stack = get_stack ();
	arm_mode_set ( 0x1F );
	set_stack ( OWN_STACK );
#endif

	code_main ( );
	
#ifdef OWN_STACK
	// restore old stack pointer
	set_stack ( old_stack );
#endif

	return 1;
}

u32
code_main ( )
{
	mcupll_set ( 3, 0 );

	// disable interrups
	arm_int_disable ( );
	int_init ( );
	
	// enable CBUS access for illumination
	cbus_unlock ();
	cbus_init ();
	uem_init ();
	
	// LCD stuff
	lcd_setup ();
	lcd_clear ();

//	lcd_pcf8833_draw_line ( 0, 0, 10, 0, 0xAA55 );

	lcd_write ( 10, 10, "Welcome", COLOR(0,0,0), 0 );

	return 1;
}

