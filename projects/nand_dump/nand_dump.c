
#include "hw_defines.h"
#include "base.h"
#include "lcd.h"

/*
Flash notes:
00EC0036:  K5E1212ACM

K5E1212-ACB MCP(64MB NAND flash / 64MB DDR)
*/

#define FL_CMD_INIT    0xABAC0000
#define FL_CMD_SETADDR 0xABAC0001
#define FL_CMD_READ    0xABAC0002

#define FL_TYPE_ONENAND 0x00000001
#define FL_TYPE_EIJA    0x00000002


u32* buffer = (u32*)0x0A002000;
u32  flash_addr;
u32  flash_type;
u32  block_count;

u32  current_block;
u32  current_page;

void
eija_set_address ( u32 block, u32 page )
{
	// clear ready flag
	strh ( flash_addr + 0x1442, ldrh ( flash_addr + 0x1442 ) & 0x7FFF);

	// set addresses
	strh ( flash_addr + 0x1402, block );                // block
	strh ( flash_addr + 0x1404, ((page*8)<<8) | 0x08 ); // sector & sector count
	strh ( flash_addr + 0x1406, 0x0000 );               // destination buffer
	strh ( flash_addr + 0x1422, 0x0002 );               // no idea :)
	
	current_block = block;
	current_page = page;
}


int 
main ( )
{
	switch ( buffer[0] )
	{
		case FL_CMD_INIT:
			arm_int_disable ();  // disable ARM CPSR IRQs
			int_init ();        // disable CTSI IRQ enabled flags
			
			sti_init ();
			cbus_unlock ();  // init CBUS and FlashCrypt
			pup_init ();
			cbus_init ();
			uem_init ();
			
			// flash base address has to be at this address
			flash_type = buffer[1];
			flash_addr = buffer[2];			
			block_count = buffer[3];
			
			switch ( flash_type )
			{
				case FL_TYPE_ONENAND:
					// load the flash info into buffer in case script needs it
					buffer[2] = 0xDEADBEEF; // magic
					buffer[3] = 0x00000000;
					buffer[4] = ldrh ( flash_addr + (0xF000<<1) ); // Manufacturer ID
					buffer[5] = ldrh ( flash_addr + (0xF001<<1) ); // Device ID
					buffer[6] = ldrh ( flash_addr + (0xF002<<1) ); // Version ID
					buffer[7] = ldrh ( flash_addr + (0xF003<<1) ); // Data Buffer Size
					buffer[8] = ldrh ( flash_addr + (0xF004<<1) ); // Boot Buffer Size
					buffer[9] = ldrh ( flash_addr + (0xF005<<1) ); // Amount of Buffers
					buffer[10] = ldrh ( flash_addr + (0xF006<<1) ); // Technology
					buffer[11] = 0x00000000;
					break;
				
				case FL_TYPE_EIJA:
					// memory controller setup?
					if ( flash_addr )
						str ( 0x08000014, (ldr ( 0x08000014 ) & 0xFFEFFFFF) | 0x00200000 );
					else
						str ( 0x08000010, (ldr ( 0x08000010 ) & 0xFFEFFFFF) | 0x00200000 );
					
					// init NAND controller
					strh ( flash_addr + 0x1420, 0x00F3 ); // reset
					strh ( flash_addr + 0x1422, 0x0000 );
					
					strh ( flash_addr + 0x144C, 0x0000 );
					strh ( flash_addr + 0x144E, block_count - 1 ); // max block address 
					strh ( flash_addr + 0x144A, 0x0004 ); // seems to make no difference if 2 or 4
				
					buffer[2] = 0xDEADBEEF; // magic
					buffer[3] = 0x00000000;
					buffer[4] = 0x00000000;
					buffer[5] = 0x00000000;
					buffer[6] = 0x00000000;
					buffer[7] = 0x00000000;
					buffer[8] = 0x00000000;
					buffer[9] = 0x00000000;
					buffer[10] = 0x00000000;
					buffer[11] = 0x00000000;
					break;
				
				default:
					buffer[2] = 0xDEADDEAD; // error magic
					break;
			}
			
			// enable LCD illumination
			uem_write ( 0x0F, 0x00, 8 ); // set LCD PWM duty cycle
			uem_write ( 0x10, 0xFF, 8 ); // enable LCD PWM
			
			// default command
			buffer[0] = FL_CMD_READ;
			break;
			
		case FL_CMD_SETADDR:
		
			switch ( flash_type )
			{
				case FL_TYPE_ONENAND:
					// Set Block/Page addresses
					strh ( flash_addr + (0xF100<<1), (buffer[1] & 0xFFFF)      ); // FBA (Block)
					strh ( flash_addr + (0xF107<<1), (buffer[2] & 0x00FF) << 2 ); // FPA
					break;
				
				case FL_TYPE_EIJA:
					eija_set_address ( buffer[1], buffer[2] );
					break;

				default:
					break;
			}
			
			// default command
			buffer[0] = FL_CMD_READ;
			break;

		case FL_CMD_READ:
			switch ( flash_type )
			{
				case FL_TYPE_ONENAND:
					// Load Data into Buffer
					strh ( flash_addr, 0x00E0 ); // cycle 1
					strh ( flash_addr, 0x0000 ); // cycle 2
					break;
				
				case FL_TYPE_EIJA:
					// read command
					strh ( flash_addr + 0x1420, 0x0000 );
					
					// wait for reading done
					while ( (ldrh ( flash_addr + 0x1442 ) & 0x8000) != 0x8000 );
					
					// controller has no next-page-mechanism, so do it manually
					eija_set_address ( current_block, current_page+1 );
					break;

				default:
					break;
			}
			
			break;

	}

	// Reset WDTs
	strh ( CTSI_BASE+0x0006, 0xFFFF );
	uem_write ( 0x12, 0x20, 8 );
	
	return 1;
}
