
#include "hw_defines.h"
#include "base.h"
#include "lcd.h"

#define CTR_CMD_NONE   0xDEAD
#define CTR_CMD_UPLOAD 1


u32 CONTROL_MEMORY_BASE = 0x00002000;
u32 *control;
void (*romcall_fsig) (u32 *fsig_data) = 0x00840015;

struct fsig_block {
  u32	long1;			// wird nach *0600c8a4.l geschrieben
  u32	long2;			// wird nach *0600c8a0.l geschrieben
  u32	numblocks1;		// zB 0001ffe0 *8=0x000fff00 bytes 
  u32	start1;			// zB 01000100 fuer 01000100-01100000
  u32	numblocks2;		// wird addiert mit numblocks nach *0600c82c.l geschrieben und enthaelt hier 0, fuer zweiten Block!
  u32	start2;			// Adresse zweiter Block, hier 0
};

static struct fsig_block fsig_data = {
	0xF3CC2AC9,
	0x934551F3,
	0x00040000,
	0x01000100,
	0x00000000,
	0x00000000
};


int 
main ( )
{
	control = (u32*)CONTROL_MEMORY_BASE;	
	control[0] = CTR_CMD_UPLOAD;

	switch ( control[0] )
	{
		case CTR_CMD_UPLOAD:
			arm_int_disable ();  // disable ARM CPSR IRQs
			int_init ();        // disable CTSI IRQ enabled flags
			
			sti_init ();
			cbus_unlock ();  // init CBUS and FlashCrypt
			pup_init ();
			cbus_init ();
			uem_init ();

			romcall_fsig(0x0900006C);
			//romcall_fsig(&fsig_data);
			
	
			// enable LCD illumination
			uem_write ( 0x0F, 0x00, 8 ); // set LCD PWM duty cycle
			uem_write ( 0x10, 0xFF, 8 ); // enable LCD PWM
			
			// default command
			control[0] = CTR_CMD_NONE;
			break;
			
		case CTR_CMD_NONE:
			break;
	}

	// Reset WDTs
	strh ( CTSI_BASE+0x0006, 0xFFFF );
	uem_write ( 0x12, 0x20, 8 );
	
	return 1;
}
