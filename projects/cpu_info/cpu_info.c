
#include "hw_defines.h"
#include "base.h"
#include "coprocessor.h"
#include "lcd.h"


/* this value will get overwritten by the loader that loads us into phone memory */
u32 CONTROL_MEMORY_BASE = 0x0A002000;
u32* buffer;



#define SCAN_COPROCESSOR_DEFINE(name,a,b,c,d) \
u32 name () \
{ \
	u32 value; \
	asm volatile (  \
		"adr     r0, .ta_"#name";" \
		"bx	     r0;" \
		".code 32;" \
		".ta_"#name":;" \
		"mrc     "#a","#b",r1,"#c","#d",0;" \
		"mov     %0, r1;" \
		"adr     r0, .tt_"#name"+1;" \
		"bx	     r0;" \
		".code 16;" \
		".tt_"#name":;" \
		: "=r"(value) : : "cc", "r0", "r1" \
		); \
	return value; \
} \

SCAN_COPROCESSOR_DEFINE(check_cp0,p0,0,c0,c0);
SCAN_COPROCESSOR_DEFINE(check_cp1,p1,0,c0,c0);
SCAN_COPROCESSOR_DEFINE(check_cp2,p2,0,c0,c0);
SCAN_COPROCESSOR_DEFINE(check_cp3,p3,0,c0,c0);
SCAN_COPROCESSOR_DEFINE(check_cp4,p4,0,c0,c0);
SCAN_COPROCESSOR_DEFINE(check_cp5,p5,0,c0,c0);
SCAN_COPROCESSOR_DEFINE(check_cp6,p6,0,c0,c0);
SCAN_COPROCESSOR_DEFINE(check_cp7,p7,0,c0,c0);
SCAN_COPROCESSOR_DEFINE(check_cp8,p8,0,c0,c0);
SCAN_COPROCESSOR_DEFINE(check_cp9,p9,0,c0,c0);
SCAN_COPROCESSOR_DEFINE(check_cp10,p10,0,c0,c0);
SCAN_COPROCESSOR_DEFINE(check_cp11,p11,0,c0,c0);
SCAN_COPROCESSOR_DEFINE(check_cp12,p12,0,c0,c0);
SCAN_COPROCESSOR_DEFINE(check_cp13,p13,0,c0,c0);
SCAN_COPROCESSOR_DEFINE(check_cp14,p14,0,c0,c0);
SCAN_COPROCESSOR_DEFINE(check_cp15,p15,0,c0,c0);

/* this exception handler will set R1 to some magic value */
void exception_vector (void)
{
	asm volatile (  
		".code 32;" 
		"ldr      r1, .dummy_word;"
		"movs	  pc, r14;" 
		".dummy_word:;"
		".word 0xDEADBEEF;"
		".code 16;" ); 
}



int 
main ( )
{
	int pos = 2;
	buffer = (u32*)CONTROL_MEMORY_BASE;

	if(!TIKU_DEVICE)
	{
		/* setting interrupt vectors is not supported for TIKU devices */
		intvec_init( (u32)exception_vector);
	}
	
	buffer[pos++] = 0xDEADBEEF; // magic
	buffer[pos++] = 0x00000000;
	buffer[pos++] = read_cache_type();
	buffer[pos++] = read_cache_ctrl();
	buffer[pos++] = read_idcode();
	buffer[pos++] = 0x11111111;
	
	if(!TIKU_DEVICE)
	{
		/* scan all coprocessors on non-TIKU devices */
		buffer[pos++] = check_cp0();
		buffer[pos++] = check_cp1();
		buffer[pos++] = check_cp2();
		buffer[pos++] = check_cp3();
		buffer[pos++] = check_cp4();
		buffer[pos++] = check_cp5();
		buffer[pos++] = check_cp6();
		buffer[pos++] = check_cp7();
		buffer[pos++] = check_cp8();
		buffer[pos++] = check_cp9();
		buffer[pos++] = check_cp10();
		buffer[pos++] = check_cp11();
		buffer[pos++] = check_cp12();
		buffer[pos++] = check_cp13();
		buffer[pos++] = check_cp14();
		buffer[pos++] = check_cp15();
	}	
	
	buffer[pos++] = 0x22222222;
	
	return 1;
}
