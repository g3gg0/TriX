
#include "hw_defines.h"
#include "base.h"
#include "lcd.h"


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

#define MRC_DEFINE(name,a,b,c,d,e) \
u32 name () \
{ \
	u32 value; \
	asm volatile (  \
		"adr     r0, .ta_"#name";" \
		"bx	     r0;" \
		".code 32;" \
		".ta_"#name":;" \
		"mrc     "#a","#b",%0,"#c","#d","#e";" \
		"adr     r0, .tt_"#name"+1;" \
		"bx	     r0;" \
		".code 16;" \
		".tt_"#name":;" \
		: "=r"(value) : : "cc", "r0" \
		); \
	return value; \
} \


#define MCR_DEFINE(name,a,b,c,d,e) \
void name (u32 value) \
{ \
	asm volatile (  \
		"adr     r0, .ta_"#name";" \
		"bx	     r0;" \
		".code 32;" \
		".ta_"#name":;" \
		"mcr     "#a","#b",%0,"#c","#d","#e";" \
		"adr     r0, .tt_"#name"+1;" \
		"bx	     r0;" \
		".code 16;" \
		".tt_"#name":;" \
		: : "r"(value) : "cc", "r0" \
		); \
} \

//		"mrc     p15,0,r0,c15,c0,0;" 

MRC_DEFINE(read_idcode, p15, 0, c0, c0, 0);
MRC_DEFINE(read_cache_type, p15, 0, c0, c0, 1);

MRC_DEFINE(read_cache_ctrl, p15, 0, c1, c0, 0);
MCR_DEFINE(write_cache_ctrl, p15, 0, c1, c0, 0);



MRC_DEFINE(read_test_state_regs, p15, 0, c15, c0, 0);
MRC_DEFINE(read_C15CI, p15, 3, c15, c1, 0);
MRC_DEFINE(read_C15CD, p15, 3, c15, c2, 0);


MCR_DEFINE(write_dcache_invalidate, p15, 0, c7, c6, 0);
MCR_DEFINE(write_dcache_victim_lockdown, p15, 0, c9, c0, 0);
MCR_DEFINE(write_dcache_cam, p15, 2, c15, c6, 6);
MCR_DEFINE(write_dcache_ram, p15, 2, c15, c10, 6);
MCR_DEFINE(write_dcache_ram_read, p15, 2, c15, c10, 2);

MCR_DEFINE(write_icache_invalidate, p15, 0, c7, c5, 0);
MCR_DEFINE(write_icache_victim_lockdown, p15, 0, c9, c0, 1);
MCR_DEFINE(write_icache_cam, p15, 2, c15, c5, 6);
MCR_DEFINE(write_icache_ram, p15, 2, c15, c9, 6);
MCR_DEFINE(write_icache_ram_read, p15, 2, c15, c9, 2);
MCR_DEFINE(write_icache_cam_match_read, p15, 2, c15, c5, 5);
MCR_DEFINE(write_icache_prefetch, p15, 0, c7, c13, 1);

MCR_DEFINE(write_C15CI, p15, 3, c15, c1, 0);
MCR_DEFINE(write_C15CD, p15, 3, c15, c2, 0);
MCR_DEFINE(write_C15CID, p15, 3, c15, c3, 0);
MCR_DEFINE(write_cam_match_d_read, p15, 2, c15, c6, 5);

#define MVA_GET_TAG(address)  (((u32)address)>>8)
#define MVA_GET_SEG(address)  ((((u32)address)>>5) & 3)
#define MVA_GET_WORD(address) ((((u32)address)>>2) & 3)

u32 read_address ( u32 address )
{
	write_icache_prefetch(address & ~3);
	write_icache_cam_match_read ( address & ~3 );

	return read_C15CI();
}


int 
main ( )
{

	buffer[0] = FL_CMD_INIT;

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

			write_dcache_invalidate(0);
			write_icache_invalidate(0);
			
			// enable caches
			write_cache_ctrl ( read_cache_ctrl() | (1<<2) | (1<<12) );
			write_C15CID ( 0x12345678 );
			/*
			// dcache victim and lockdown base with 0
			write_dcache_victim_lockdown ( 0 );
			
			// dcache CAM write to seg 0, index 0
			write_dcache_cam ( (MVA_GET_TAG(buffer)<<8) | (MVA_GET_SEG(buffer)<<5) | (1<<4) | (0<<3) | (0<<2) | (0<<1) );
			
			// DCache RAM write to seg 0, index 0, word 0
			write_C15CD ( 0x89ABCDEF );
			//write_C15CI ( 0x12345678 );
			
			write_icache_victim_lockdown ( 0 );
			write_icache_ram ( (0<<5) | (0<<2) );
			
			write_dcache_victim_lockdown ( 0 );
			write_dcache_ram ( (MVA_GET_SEG(buffer)<<5) | (MVA_GET_WORD(buffer)<<2) );

			// clear C15.C.D
			write_C15CD(0x11111111);
			//write_C15CI(0x22222222);
			
			// do a ram read to C15.C.D
			
			write_icache_victim_lockdown ( 0 );
			write_icache_ram_read ( (0<<5) | (0<<2) );
			
			write_dcache_victim_lockdown ( 0 );
			write_dcache_ram_read ( (0<<5) | (0<<2) );
			*/

			// load the flash info into buffer in case script needs it
			buffer[2] = 0xDEADBEEF; // magic
			buffer[3] = 0x00000000;
			buffer[4] = read_cache_type();
			buffer[5] = read_idcode();
			buffer[6] = 0x11111111;
			buffer[7] = 0x11111111;
			buffer[8] = read_C15CI();
			buffer[9] = read_C15CD();
			buffer[10] = read_address(0x0A002000);
			buffer[11] = 0x11111111;
			buffer[12] = read_cache_ctrl();
			buffer[13] = 0x00000000;
			
			{
				int pos = 0;
				
				for ( pos=0; pos <64; pos++)
				{
					buffer[16 + pos] = read_address(0x0A004000 + pos);
				}
			}
				
			// enable LCD illumination
			uem_write ( 0x0F, 0x00, 8 ); // set LCD PWM duty cycle
			uem_write ( 0x10, 0xFF, 8 ); // enable LCD PWM
			
			// default command
			buffer[0] = FL_CMD_READ;
			break;
			
		case FL_CMD_SETADDR:
		
			// default command
			buffer[0] = FL_CMD_READ;
			break;

		case FL_CMD_READ:
			break;

	}

	// Reset WDTs
	strh ( CTSI_BASE+0x0006, 0xFFFF );
	uem_write ( 0x12, 0x20, 8 );
	
	return 1;
}
