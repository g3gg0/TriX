
#include "hw_defines.h"
#include "base.h"

unsigned int HW_DEVICE_TYPE;

int hw_flash_bases[] = { 0x00000000, 0x01000000 };
int hw_faid_bases[] = { 0x0000012C, 0x0100002C };

int hw_lead3mmi_bases[] = { 0xFFFF0000, 0x06002000 };
int hw_dsppll_bases[] = { 0xFFFF0000, 0x06008000 };
int hw_mcupll_bases[] = { 0xFFFF0000, 0x06009000 };
int hw_accpll_bases[] = { 0xFFFF0000, 0x0600A000 };
int hw_dspmif_bases[] = { 0x0C305500, 0x0600B000 };
int hw_ebusc_bases[] = { 0xFFFF0000, 0x0600C000 };
int hw_fsig_bases[] = { 0xFFFF0000, 0x0600C800 };
int hw_testif_bases[] = { 0xFFFF0000, 0x0600F000 };
int hw_cbusc_bases[] = { 0x0C305300, 0xFFFF0000 };
int hw_sti_bases[] = { 0x0C305400, 0x0600D000 };
int hw_ctsi_bases[] = { 0x0C308000, 0x06010000 };
int hw_pup_bases[] = { 0x0C308200, 0x06010200 };
int hw_uif_bases[] = { 0x0C308400, 0x06010400 };
int hw_accif_bases[] = { 0xFFFF0000, 0x06010500 };
int hw_simif_bases[] = { 0xFFFF0000, 0x06010800 };
int hw_mfi_bases[] = { 0xFFFF0000, 0x06010900 };
int hw_brain_bases[] = { 0xFFFF0000, 0x06100000 };
int hw_osmodat_bases[] = { 0xFFFF0000, 0x04000000 };
int hw_osmocmd_bases[] = { 0xFFFF0000, 0x04FD0000 };
int hw_ram_bases[] = { 0x0A000000, 0x00000000 };
int hw_intvec_bases[] = { 0x00000020, 0x00000020 };
int hw_endianess_little[] = { 1, 0 };




void
usleep ( u32 us )
{
	int innerloop = 0;

	us &= 0xFFFFFFFC;
	while ( us )
	{
		us-=4;
		innerloop = 22;
		while ( innerloop )
		{
			asm ( "nop" );
			innerloop--;
		}
	}
}

void
arm_int_enable ( )
{
	asm volatile ( 
		"adr     r0, .switch_to_arm1;"
		"bx	     r0;"

		".code 32;"
		".switch_to_arm1:;"

		"mrs     r0, CPSR;"
		"bic     r0, r0, #0xC0;" 
		"msr     CPSR_cxsf, r0;" 
		
		"adr     r0, .switch_thumb1+1;"
		"bx	     r0;"

		".code 16;"
		".switch_thumb1:;"
		: : : "cc", "r0"
		);
}

void
arm_int_disable ( )
{
	asm volatile ( 
		"adr     r0, .switch_to_arm2;"
		"bx	     r0;"

		".code 32;"
		".switch_to_arm2:;"

		"mrs     r0, CPSR;"
		"orr     r0, r0, #0xC0;" 
		"msr     CPSR_cxsf, r0;" 
		
		"adr     r0, .switch_thumb2+1;"
		"bx	     r0;"

		".code 16;"
		".switch_thumb2:;"
		: : : "cc", "r0"
		);
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

void 
bx_r6 ()
{
    asm volatile ( "bx r6;" );
}

void 
bx_r5 ()
{
    asm volatile ( "bx r5;" );
}

void 
bx_r4 ()
{
    asm volatile ( "bx r4;" );
}

void 
bx_r3 ()
{
    asm volatile ( "bx r3;" );
}

void 
bx_r2 ()
{
    asm volatile ( "bx r2;" );
}

void 
bx_r1 ()
{
    asm volatile ( "bx r1;" );
}

void 
bx_r0  ()
{
    asm volatile ( "bx r0;" );
}



u32
mcupll_set ( u32 multiplier, u32 divisor )
{
	volatile int loops = 0x2040;
	
	strh ( MCUPLL_BASE + 0x0000, 0x2000 );
	while ( loops-- );
	strh ( MCUPLL_BASE + 0x0000, 0x2010 | ((multiplier&0x1F)<<7) | ((divisor&0x03)<<5) );
	
	return 1;
}


u32 
intvec_init ( u32 arm_irq_handler )
{
	if(!TIKU_DEVICE)
	{
		volatile u32 *vec_table = (u32 *)INT_VECTOR_BASE;
		
		vec_table[INT_VECTOR_UNDEF] = arm_irq_handler;
		vec_table[INT_VECTOR_SWI] = arm_irq_handler;
		vec_table[INT_VECTOR_ABORT_INST] = arm_irq_handler;
		vec_table[INT_VECTOR_ABORT_DATA] = arm_irq_handler;
		vec_table[INT_VECTOR_IRQ] = arm_irq_handler;
		vec_table[INT_VECTOR_FIQ] = arm_irq_handler;
	}

	return 1;
}


// set up unknown STI registers
u32
sti_init ( )
{
	strh_rel ( STI_BASE, 0, 5 );
	strh_rel ( STI_BASE, 4, 0x0200 );
	strh_rel ( STI_BASE, 2, 0x0100 );

	if(TIKU_DEVICE)
	{
		if ( ldrh_rel ( CTSI_BASE, 0x0002 ) & 0x04 )
			strh_rel ( CTSI_BASE, 0x0034, 0x40 );
		else
			strh_rel ( CTSI_BASE, 0x0036, 0x40 );
	}
	
	return 1;
}

// set up unknown PUP registers
u32
pup_init ( )
{
	strh_rel ( PUP_BASE, 0x001C, 0x0200 );
	
	return 1;
}



u32 
int_init ()
{
	/* activate interrupt controller */
	strh ( CTSI_BASE + 0x0024, 0x0010 ); // disable IRQs
	
	/* disable ALL device interrupts */
	strh ( CTSI_BASE + 0x0008, 0xFFFF ); // Disable IRQs
	strh ( CTSI_BASE + 0x000C, 0xFFFF ); // Disable IRQs
	strh ( CTSI_BASE + 0x0010, 0xFFFF ); // Disable IRQs
	strh ( CTSI_BASE + 0x0014, 0xFFFF ); // Disable IRQs
	strh ( CTSI_BASE + 0x0018, 0xFFFF ); // Disable IRQs
	strh ( CTSI_BASE + 0x001C, 0xFFFF ); // Disable IRQs
	strh ( CTSI_BASE + 0x0020, 0xFFFF ); // Disable IRQs

	return 1;
}


// set the CBUS clock speed 
u32
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

	
	return 1;
}

// initialize CBUS clock and state
u32
cbus_init ()
{
	cbus_clock ( 0 );
	strh ( CBUS_CLK_SPEED, CLK_SPEED );
	strh ( CBUS_CLK_SET, CBUS_INIT );
	cbus_clock ( 1 );

	return 1;
}

// unlock CBUS by copying FAID into CBUSC registers
u32
cbus_unlock ( )
{
	if(ASIC2_DEVICE)
	{
		asm volatile ( 
			"adr   r0, faid_data_2;"
			"ldrb  r1, [r0, #0x0D];"
			"mov   r2, #0x0B;"
			"mov   r3, #0x00;"
			"ldr   r4, upload_faid_2;"
			"mov   lr, pc;"
			"bx    r4;"
			"b     .cbus_unlock_ret_2;"
			
			".align 4;"
			"upload_faid_2:;"
			".word 0x0084001F;"

			// NHL-4U
			"faid_data_2:;"
			".word 0xCE9CB10E;"
			".word 0x58312707;"
			".word 0x7246C0C2;"
			".word 0x0001a4bf;"
			
			".cbus_unlock_ret_2:;"
			: : : "r0", "r1", "r2", "r3", "r4", "cc"
			 );
	}
	else if(ASIC11_DEVICE)
	{
		asm volatile ( 
			"adr   r0, faid_data_11;"
			"ldrb  r1, [r0, #0x0D];"
			"mov   r2, #0x0B;"
			"mov   r3, #0x00;"
			"ldr   r4, upload_faid_11;"
			"mov   lr, pc;"
			"bx    r4;"
			"b     .cbus_unlock_ret_11;"
			
			".align 4;"
			"upload_faid_11:;"
			".word 0x0084001F;"
			
			// RH-87
			"faid_data_11:;"
			".word 0xC5B5FFF8;"
			".word 0x120A5E1D;"
			".word 0xD0FF4F52;"
			".word 0x00001D2A;"
			
			".cbus_unlock_ret_11:;"
			: : : "r0", "r1", "r2", "r3", "r4", "cc"
			 );			 
	}
	else if(TIKU_DEVICE)
	{
		u32 pos = 0;

		strh ( CBUSC_INIT, 0x1F00 );
		strh ( CBUSC_BYTE, ldrb ( FAID_BASE + 12 ) );

		for ( pos = 0; pos < 12; pos+=2 )
		{
			u16 value = ldrh_rel ( FAID_BASE, pos );
			strh_rel ( CBUSC_FAID, pos, value );
		}

		strh ( CBUSC_INIT, 0x000B );
	}

	return 1;
}

// wait until CBUS is ready
u32
cbus_wait ()
{
	u32 timeout = 0xF000;
	
	while ( !(ldrh ( CBUS_CTRL ) & CBUS_CTRL_READY) && (timeout-- > 0) );
	
	if ( !timeout )
		return 0;
	
	return 1;
}

// read register [reg] from device [dev]
u32
cbus_read ( u32 dev, u32 reg, u32 bits )
{
	bits--;
	cbus_wait ();
	
	strh ( CBUS_BITCOUNT, (bits & 0x1F ) );
	strh ( CBUS_CTRL, CBUS_CTRL_READ | ((dev & 0x07) << 5) | (reg & 0x1F) );
	
	cbus_wait ();
	
	return (ldrh ( CBUS_DATA_HIGH ) << 16) | ldrh ( CBUS_DATA_LOW );
}

// write [val] into register [reg] of device [dev]
u32
cbus_write ( u32 dev, u32 reg, u32 val, u32 bits )
{
	bits--;
	cbus_wait ();
	
	strh ( CBUS_BITCOUNT, CBUS_BITS_WRITE | (bits & 0x1F ) );
	strh ( CBUS_CTRL, CBUS_CTRL_WRITE | ((dev & 0x07) << 5) | (reg & 0x1F) );
	
	if ( bits >= 0x10 )
		strh ( CBUS_DATA_HIGH, (val >> 16) );
	strh ( CBUS_DATA_LOW, val );
		
	cbus_wait ();
	
	return 1;
}

// read register [reg] from UEM
u32
uem_read ( u32 reg, u32 bits )
{
	return cbus_read ( UEM_ADDRESS, reg, bits );
}

// write [val] into register [reg] to UEM
u32
uem_write ( u32 reg, u32 val, u32 bits )
{
	return cbus_write ( UEM_ADDRESS, reg, val, bits );
}

u32
uem_init ( )
{
	uem_write ( 0x12, 0x3F, 8 );  // WDT
	uem_write ( 0x19, 0x80, 16 ); // 
	uem_write ( 0x0B, 0x04, 8 );  // voltages

	if(TIKU_DEVICE)
	{
		// enable device 0x06 ?
		strh ( PUP_BASE + 0x001C, 0x0200 );

		cbus_read ( 6, 0x02, 19 );
		cbus_write ( 6, 0x02, 0x0007FFFF, 19 );

		cbus_read ( 6, 0x08, 16 );
		cbus_write ( 6, 0x08, 0x9AFD, 16 );
	}

	return 1;
}

u32
spi_received ()
{
	while ( !(ldrh ( UIF_BASE + 0x002A ) & 0x04) );
	
	return 1;
}

u32
spi_ready ()
{
	while ( !(ldrh ( UIF_BASE + 0x002A ) & 0x01) ); // -> 0x0007 (UIF+0x002A Graph Ready States)
	
	return 1;
}

u32
spi_read ( )
{
	spi_received ();
	return ldrh ( UIF_BASE + 0x0028 ); // (UIF+0x001A Graph Write CMD)
}

u32
spi_send_cmd ( u32 data )
{
	spi_ready ();
	strh ( UIF_BASE + 0x001A, data ); // (UIF+0x001A Graph Write CMD)
	
	return 1;
}

u32
spi_send_data ( u32 data )
{
	spi_ready ();
	strh ( UIF_BASE + 0x001C, data ); // (UIF+0x001C Graph Write DATA)
	
	return 1;
}
