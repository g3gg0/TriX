#define IO_FSIG_CHECK1 *(unsigned short*)0x600C82A
#define IO_FSIG_CHECK2 *(unsigned short*)0x600C828
#define IO_FSIG_CHECK3 *(unsigned short*)0x600C826

#define IO_FSIG_FSIG1 *(unsigned int*)0x600C8A0
#define IO_FSIG_FSIG2 *(unsigned int*)0x600C8A4

#define IO_FSIG_DATA1 *(unsigned int*)0x600C860
#define IO_FSIG_DATA2 *(unsigned int*)0x600C864

#define IO_FSIG_COUNT *(unsigned int*)0x600C82C

#define IO_FSIG_SOMETHING *(unsigned short*)0x600C800

unsigned int modified[100];

void irqfiq_disable ( )
{
	asm volatile (
	".code 16;"
	"adr     R0, .arm_mode_1;"
	"bx	     R0;"
	".code 32;"
	".arm_mode_1:;"

	"mrs r0, CPSR;"
	"orr r0, r0, #0xc0;"
	"msr CPSR_cxsf, r0;"
	
	"adr     R0, .thumb_mode_1+1;"
	"bx	     R0;"
	".code 16;"
	".thumb_mode_1:;"
	:
	:
	: "r0"
	);

}

void irqfiq_enable ( )
{
	asm volatile (
	".code 16;"
	"adr     R0, .arm_mode_2;"
	"bx	     R0;"
	".code 32;"
	".arm_mode_2:;"
	
	".code 32;"
	"mrs r0, CPSR;"
	"bic r0, r0, #0xc0;"
	"msr CPSR_cxsf, r0;"
	
	"adr     R0, .thumb_mode_2+1;"
	"bx	     R0;"
	".code 16;"
	".thumb_mode_2:;"
	:
	:
	: "r0"
	);

}

void fsig_clone ( unsigned int *fsig_rom )
{

	unsigned int *start;
	unsigned int blocks;
	unsigned int next_modified = 0;
	unsigned int data;
	unsigned char c = 0;
	
	unsigned int *fsig_data;
	
	/*
	if ( ((unsigned int)fsig_rom & 0xFF ) == 0x6C )
		return;
		
	while ( 1 );*/
	
	
	//disable fiq/irq
	irqfiq_disable ();
	
	fsig_data = (unsigned int*)0x600C860;
	
	
	//what is it?
	if ( IO_FSIG_CHECK1 )
	{
	
		//while ( 1 );
		return;
	}
	//what is it? timer maybe?
	if ( !(IO_FSIG_CHECK2 + IO_FSIG_CHECK3) )
	{
		//while ( 1 );
		return;
	}
	
	
	//IO_FSIG_FSIG1 = 0x6C895CF6; //*(fsig_rom+1);
	IO_FSIG_FSIG1 = *(fsig_rom+1);
	//return;
	//IO_FSIG_FSIG2 = 0xA02C8454; //*(fsig_rom);
	IO_FSIG_FSIG2 = *(fsig_rom);
	//return;
	//IO_FSIG_COUNT = 0x000FFF00/8; //*(fsig_rom+2);
	IO_FSIG_COUNT = *(fsig_rom+2);
	//return;
//	start = (unsigned int*)0x01000100; //(unsigned int*)*(fsig_rom+3);
	start = (unsigned int*)*(fsig_rom+3);
	
	
	IO_FSIG_DATA1 = *start;
	IO_FSIG_DATA2 = *(start+1);
	
	//??
	IO_FSIG_SOMETHING = 3;
	//return;
	
	start += 2;
	//blocks = 0x000FFF00/8; //*(fsig_rom+2);
	blocks = *(fsig_rom+2);
	blocks--;
	blocks *= 2;
	
	/*
	next_modified_addr = modified[0];
	next_modified_data = modified[1];
	*/
	do
	{
	
		if ( start == (unsigned int*)modified[next_modified] )
		{
			data = modified[next_modified+1];
			next_modified += 2;
		}
		else
			data = *start;
	
		*fsig_data = data;
			
		//c ^= 1;
		fsig_data = (unsigned int*)((unsigned int)fsig_data ^ 4);
		
		start++;
		
	
	} while (--blocks);
	//while ( 1 );
	
	//enable fiq/irq
	irqfiq_enable ();
	
}

int _call_via_sp ()
{
	asm volatile (
	"mov r2, sp;"
	"add r2, r2, #1;"
	"bx r2;"
	);
}

int _call_via_r3 ()
{
	asm volatile (
		"bx r3;"
	);
}

void copy_to_ram_and_run ( unsigned int *fsig_rom )
{
	void (*ram_func)(unsigned int *) = 0; 
	unsigned char run_ram[400];
	unsigned int i;
	
	for ( i = 0; i < 400; i++ )
		run_ram[i] = *(fsig_clone+i);
		
	if ( (unsigned int)run_ram % 1 )
	{
		ram_func = run_ram - 1;
		ram_func(fsig_rom);
	}
	else
	{
		ram_func = run_ram - 1;
		ram_func(fsig_rom);
	}
}


