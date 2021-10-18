
int testfunc ( int val);

#define TAG_LSB    0x08
#define SEG_LSB    0x05
#define VLD_LSB    0x04
#define DE_LSB     0x03
#define DO_LSB     0x02
#define WB_LSB     0x01
#define WORD_LSB   0x02
#define LOCK_LSB   0x1A


	unsigned char *tt = "111111";
	unsigned char *tt2 = "22222";
	unsigned char *tt3 = "33333";
	unsigned char *tt4 = "4444";
	unsigned char *tt5 = "555555";

unsigned int buffer = 0x0C0B0500;

void
set_dword ( unsigned long addr, unsigned int val )
{
    asm volatile ( "str  %0, [%1,#0x00]"::"r" ( val ), "r" ( addr ) );
    return;
}



unsigned int get_i_entry ( unsigned int dest )
{
	unsigned int entry = 0xFECA1DC0;
/*
	asm volatile (
		"adr     R2, switch_to_arm2;"
		"bx	     R2;"
		".code 32;"
		"switch_to_arm2:;"


//		"MCR   P15, 2, %1, C15, C9, 2;"
		"MCR   P15, 2, %1, C15, C5, 2;"
		"MRC   P15, 3, %0, C15, C1, 0;"


		"adr   R2, .switch_thumb2+1;"
		"bx	   R2;"
		".code 16;"
		".switch_thumb2:;"
		:"=r" (entry):"r" (dest) : "memory", "r2"
	);
*/
	return entry;
}

void do_code ()
{
	int i = 0;

	for ( i=0; i<40; i++ )
	{
//		set_dword ( buffer+(i<<2), 0);
	}

	for ( i=0; i<64; i++ )
	{
//		set_dword ( buffer+(i<<2), get_i_entry ( i<<2 ));
	}

}

int main (  )
{
	

	asm volatile (
		"push    { R0-R7, LR };"
		"bl      do_code;"
		"pop     { R0-R7, PC };"
	);


}

