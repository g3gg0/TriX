
#define msec *1000
#define sec *1000 msec
#define COLOR(r,g,b) ((~r&0x0F)<<8|(~g&0x0F)<<4|(~b&0x0F))
#define FIXED_WIDTH 6

#define g3gg0
//#define OSTRICH

/*
const char *int2asc[] =
{
    "--zero-",
    "--one--",
    "--two--",
    "-three-",
    "-four--",
    "--five-",
    "--six--",
    "-seven-",
    "-eight-",
    "--nine-",
    "--ten--",
    "eleven-",
    "-twelve",
    NULL
}
*/

int _call_via_r3 () 
{ 
    asm volatile (  ".code 16;" 
                    "mov pc, r3;" ); 
}
int _call_via_r4 () 
{ 
    asm volatile (  ".code 16;" 
                    "mov pc, r4;" ); 
}

void memset ( unsigned char *dst, unsigned char val, unsigned int bytes )
{
	while ( bytes-- )
	{
		*dst = val;
		dst++;
	}
}

#ifdef g3gg0
void dump_buffer ( int x, int y, unsigned char *addr, int bytes )
{
	int i = 0;
	char buf[3];

	while ( i < bytes )
	{
		sprintf ( buf, "%02X", addr[i] );
		x = lcd_write ( x, y, buf, COLOR(0,0,0), 0);
		if ( i && (i&3) == 3 )
			x += 4;
		i++;
	}
	while ( i < bytes )
	{
		sprintf ( buf, "%02X", addr[i]/*^addr[i-bytes/2]*/ );
		x = lcd_write ( x, y, buf, COLOR(0,0,0), 0);
		if ( i && (i&3) == 3 )
			x += 4;
		i++;
	}
	return;
}

unsigned short timer_get ( void )
{
	return *((short*)0x06010038);
}

void pseudo_abort_handler ( void )
{
	unsigned short *cstart = (unsigned short*)((int)0x40);
	unsigned short *cend = (unsigned short*)((int)0x42);
	char buf[11];
	int x = 0;
	int y = 50;

	*cend = timer_get();
	lcd_init ();
	sprintf ( buf, "%04X - ", (int)(*cend) );
	x = lcd_write ( x, y, buf, COLOR(0,0,15), 0);

	sprintf ( buf, "%04X = ", (int)(*cstart));
	x = lcd_write ( x, y, buf, COLOR(0,0,15), 0);

	sprintf ( buf, "%04X", (int)(*cend) - (int)(*cstart) );
	x = lcd_write ( x, y, buf, COLOR(0,0,15), 0);
	
//	set_light ( 0 );
	usleep ( 10 sec );
}

int  scan_memory ( unsigned int start, unsigned int end, unsigned int values[] )
{
	int pos = start;
	char buf[32];
	int x = 0;
	int y = 10;
	int value = 0;
	int vpos = 0;

	while ( pos < end )
	{
		x = 0;
		sprintf ( buf, "%08X: ", pos );
		x = lcd_write ( x, y, buf, COLOR(15,0,0), 0);

		vpos = 0;
		while ( (value = values[vpos++]) != 0x00000000 )
		{
			if ( *((unsigned int*)pos) == value )
			{
				sprintf ( buf, "= %08X", *((unsigned int*)pos) );
				lcd_write ( x, y, buf, COLOR(0,0,15), 0);
				y += 9;
			}
		}
		pos += 4;
	}
}

int own_decrypt ( unsigned char *dst, unsigned char *src, int bytes, int algo )
{
	unsigned char *buffer = ((unsigned char*)0x4B5A0);
	void (*ROMCALL)( unsigned char *algo_dst, int algo_bytes, unsigned char *algo_buffer, int algo_id );

	ROMCALL = 0x00084000F;

	memset ( buffer, 0xFF, 0x200 );

	hw_abort_prepare ( 5 );
	dump_buffer ( 0, 0, 0x008001BA, 0x20 );
	usleep ( 10 sec );
	ROMCALL ( dst, bytes, buffer, algo );
	hw_abort_finish_1 ( 5 );
	hw_abort_finish_2 ( 5 );

	return 1;
}


int own_flashchk_call ( unsigned char *data, unsigned int value, unsigned int unk, int type )
{
	void (*ROMCALL)( unsigned char *algo_data, unsigned int algo_value, unsigned int algo_unk, int algo_type );

	ROMCALL = 0x000840021;

	hw_abort_prepare ( 5 );
	ROMCALL ( data, value, unk, type );
	hw_abort_finish_1 ( 5 );
	hw_abort_finish_2 ( 5 );

	return 1;
}

int  decrypt_test ( int pos, const unsigned char *value )
{
	int algo = 0x00;
	int algoffset = 0;
	int i = 0;
	int x = 0;
	int y = 0;
	unsigned char buffer[64];

	while ( algo < 0xC0 )
	{

		sprintf ( buffer, "Val %i, Algo 0x%02X", pos+1, algo  );
		lcd_write ( 20, 10, buffer, COLOR(15,0,0), 0);
		y = 30;
		algoffset = 0;
		while ( algoffset < 8 )
		{
			sprintf ( buffer, "0x%02X", algo+algoffset  );
			lcd_write ( 0, y, buffer, COLOR(0,15,0), 0);

			for ( i=0; i<8; i++ )
				buffer[i] = value[i];

			decrypt ( buffer, buffer, 8, algo+algoffset );
			dump_buffer ( 26, y, buffer, 8 );
			y+=10;
			algoffset++;
		}
		usleep ( 5 sec );
		algo+=8;

		if ( algo == 0x40 )
			algo = 0x80;
	}

	return 0;

}

int decrypt_claudia ( const unsigned char *value )
{
	int algo = 0x20;
	int pos = 0;
	int i = 0;
	int x = 0;
	int y = 0;
	unsigned char buffer[0x40];

	for ( i=0; i<0x40; i++ )
		buffer[i] = value[i];

	if ( !own_decrypt ( buffer, buffer, 0x40, 0x20 ) )
		return 0;


	own_flashchk_call ( buffer, 0x2396, 0x01060031, 5 );

	while ( pos < 0x40 )
	{
		dump_buffer ( 0, y, buffer+pos, 0x08 );
		pos += 0x08;
		y+=10;
	}
	usleep ( 60 sec );

	return 1;
}




#else
void dump_reg_short ( int x, int y, unsigned short *addr )
{
	char buf[32];
/*
	unsigned short val = addr[0];
	int bits = 16;
	while ( bits )
	{
		if ( val & 0x8000 )
			x = lcd_write ( x, y, "1", COLOR(15,0,0), 0);
		else
			x = lcd_write ( x, y, "0", COLOR(15,0,0), 0);
		val <<= 1;
		bits--;
	}
*/

	sprintf ( buf, "%08X: ", addr );
	x = lcd_write ( x, y, buf, COLOR(15,0,0), 0);

	sprintf ( buf, "%04X", addr[0] );
	x = lcd_write ( x, y, buf, COLOR(0,0,15), 0);

	sprintf ( buf, "%04X   ", addr[1] );
	x = lcd_write ( x, y, buf, COLOR(0,0,15), 0);
	return;
}

void dump_4reg_short ( int x, int y, unsigned short *addr )
{
    int i;
    unsigned short val = *addr;
    char *buffer = (char*)0x5f65C;
    buffer[0] = 0;

    for ( i = 0; i < 4; i++ )
        sprintf ( buffer+strlen ( buffer ), "%04X ", *(addr+i) );

    lcd_write ( x, y, buffer, COLOR(15,0,0), 0 );
    return;
}

void dump_genio ( int x, int y, int pin )
{
    int val = hw_io_get_pin ( pin );
    if ( val )
        lcd_write ( x, y, "GENIO PIN SET", COLOR(0,0,15), 0 );
    else
        lcd_write ( x, y, "GENIO PIN NOT SET", COLOR(0,0,15), 0 );
}
int  timer_is_timer ( unsigned short *addr )
{
    unsigned int i = 1;
    unsigned short tmp_val2, tmp_val = *addr;

    while ( tmp_val == tmp_val2 && i < 1 << 21  )   //max ~3 sek
    {
        usleep ( i );
        tmp_val2 = *addr;
        i *= 2;
    }

    if ( tmp_val == tmp_val2 )
        return 0;

    if ( tmp_val2 - tmp_val > 8 )
        return 0;

    return tmp_val2 - tmp_val;
}


void _1000nops ()
{
        asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");
        asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");
        asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");
        asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");
        asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");
        asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");
        asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");
        asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");
        asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");
        asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");
        // 100
        asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");
        asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");
        asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");
        asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");
        asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");
        asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");
        asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");
        asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");
        asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");
        asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");
        // 200
        asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");
        asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");
        asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");
        asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");
        asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");
        asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");
        asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");
        asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");
        asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");
        asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");
        // 300
        asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");
        asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");
        asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");
        asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");
        asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");
        asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");
        asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");
        asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");
        asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");
        asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");
        // 400
        asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");
        asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");
        asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");
        asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");
        asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");
        asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");
        asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");
        asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");
        asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");
        asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");
        // 500

        asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");
        asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");
        asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");
        asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");
        asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");
        asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");
        asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");
        asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");
        asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");
        asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");
        // 100
        asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");
        asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");
        asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");
        asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");
        asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");
        asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");
        asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");
        asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");
        asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");
        asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");
        // 200
        asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");
        asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");
        asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");
        asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");
        asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");
        asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");
        asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");
        asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");
        asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");
        asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");
        // 300
        asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");
        asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");
        asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");
        asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");
        asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");
        asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");
        asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");
        asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");
        asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");
        asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");
        // 400
        asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");
        asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");
        asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");
        asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");
        asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");
        asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");
        asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");
        asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");
        asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");
        asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");asm ("nop");
        // 500
};
#endif

int  set_light ( int val )
{
	hw_light_set_ratio ( (val>0)?100:1 );
	hw_light_set_pwm ( (val>0)?100:1 );
}

void toggle_pins ( unsigned int table, unsigned int call )
{
	static const unsigned int own_table[6] = { 0xF3C19AB6, 0x74941182, 0x1FFE0, 0x01000100, 0, 0 };

//	int i = 3;
//	unsigned short *address = 0x06010476;
/*
	hw_set_pinstate ( 0x0F, 0x22 );

	usleep ( MS(1000) );

	while ( i-- )
	{
		hw_set_pin ( 0x0F, 1 );
		usleep ( MS(1000) );
		hw_set_pin ( 0x0F, 0 );
		usleep ( MS(1000) );
	}
	usleep ( MS(1000) );
	hw_set_pin ( 0x0F, 1 );
*/
/*
	usleep ( 1 sec );
	set_light ( 1 );
	usleep ( 100 msec );
	set_light ( 0 );
	usleep ( 100 msec );
	set_light ( 1 );
	usleep ( 100 msec );
	set_light ( 0 );
	usleep ( 100 msec );
	set_light ( 1 );
	usleep ( 100 msec );
	set_light ( 0 );
	usleep ( 1 sec );
*/
	set_light ( 1 );

	bx_r1 ( table, call );

	set_light ( 0 );
	usleep ( 500 msec );
	set_light ( 1 );
	lcd_test ();
	usleep ( 60 sec );
/*
	hw_set_pin ( 0x0F, 0 );

	usleep ( MS(1000) );
	usleep ( MS(1000) );
	hw_set_pin ( 0x0F, 1 );
	usleep ( MS(10000000) );
*/
//	address[0] = 0x0000;

}

/*
E74E3C1EDCB2CA4F
E3C19AB674941182
00ACAF52C0CB0C7C

Val 1, Algo 0x00
0x00 D0F923D6381C4618
0x01 39893AF90F7BC2BD
0x02 636FE1EC8256B926
0x03 2C6EC65CBE200223
0x04 1B5C8A6F075DD1C5
0x05 EC0405A71E5FFB85
0x06 5A4BDAF9AD4C09F2
0x07 B7FD30DCA58767DF
Val 1, Algo 0x08
0x08 2B69E73AB2EC5743
0x09 1A5E7C2450F359C4
0x0A A9104EE9D53DCC7E
0x0B 7D0FAE90B02955E5
0x0C BEE1A38B52FF4E81
0x0D 2A340F0DA4648960
0x0E B87D771B73D54560
0x0F 5D5769EEC8BB1866
Val 1, Algo 0x10
0x10 FDF9960752917F84
0x11 C0EF7040D9F902D5
0x12 0D3C4CA67AE48672
0x13 93D966704001E9BA
0x14 1F2F800B2826FEF6
0x15 D0DCC4C99617B243
0x16 40BD3D47ED6E64ED
0x17 9152A9078CB31200
Val  1, Algo 0x18
0x18 AB06E1E1DBBADB9A
0x19 839821071563F60D
0x1A 1D74F80B82659ADE
0x1B 4EB8739F3BB7387D
0x1C 05CE79ED95261FCB
0x1D 9FB68BA3A487A2B6
0x1E 01866410C4C14DBB
0x1F 85A25E0872706033
Val  1, Algo 0x20
0x20 FED832327BD28CFA
0x21 D9431F5DEC72503E
0x22 3C448A1D859299B3
0x23 5647F08994C6EC5A
0x24 86BC13DDE939CE48
0x25 5A0CD9C2F8B14663
0x26 8F6A05CE6C3CEAD2
0x27 019D6BD77C6C5E97
Val  1, Algo 0x28
0x28 6A308CD4B8CD4CB7
0x29 C52D9AE24973EE86
0x93 EE5BB1CA73035EEA
0x94 E7C3825D1B8487A8
0x95 68AFFFB68C664635
0x96 C49700E304A44A88
0x97 188408F704C8FE07
Val  2, Algo 0x98
0x98 495B071B1789470D
0x99 8583FBDAE0A27BED
0x9A CC2DC44C5637F776
0x9B E61A1581902781EE
0x9C BE877591E647B7EC
0x9D 34601A442A6ACB9A
0x9E 18F22BE747EC797E
0x9F 67D2FE73AACA91A1
Val  2, Algo 0xA0
0xA0 EF3554D55AB75FB8
0xA1 B45A376606509304
0xA2 0D628CECBC7A1D7E
0xA3 7E93E1790B08E06E
0xA4 602B01E15AC7EE24
0xA5 9307A736A7EBBE22
0xA6 534C6835DAF19D8A
0xA7 AAD0BD580536F751
Val  2, Algo 0xA8
0xA8 4CAA869992C09E68
0xA9 2F2810C72C93A8A8
0xAA 1B640226D32615E0
0xAB 23227A5823A0002B
0xAC 07D6261B2BD0A530
0xAD 8285388A3A7D68B4
0xAE 8C760F0E0FEC372D
0xAF B13CC74F9BFE8DF0
Val  2, Algo 0xB0
0xB0 A2554F966BFEC6A3
0xB1 EC26BD24E56CDAA9
0xB2 3F87954ACC433B00
0xB3 0BCAF8C92DB9BE25
0xB4 A305875D7950A13B
0xB5 DBB3DE37D9A9E0BC
0xB6 39EF85ECC17729C5
0xB7 864B173CA0451644
Val  2, Algo 0xB8
0xB8 EAD6410755D71211
0xB9 5CD1F92238318D53
0xBA 8A72F7CFC6EF7744
0xBB 85EB68359890045B
0xBC D167F93F80B8BE60
0xBD D248476AC0FB8D81
0xBE EC8A48DAC3A25957
0xBF DA5E098A1F8B5550
Val  3, Algo 0x00
0x00 269377638FE21E53
0x01 40AE409F992C6811
0x02 04ED1DA27F14DEBE
0x03 EAAE88DE6A14F148
0x04 5366BBB984F769EF
0x05 07083A66D5CBB942
0x06 8CFC0F52BB5D91F8
0x07 3E807E6680B2C952
Val  3, Algo 0x08
0x08 D1A8BABB061D5E00
0x09 3B2D81825D70FB36
0x0A 9E4247D6536A0695
0x0B A928B3D7F673D602
0x0C 11D6552ED1DFB76C
0x0D 092E5107C4892B80
0x0E 7F53A48F25FCB910
0x0F 70628BB6386A37CD
Val  3, Algo 0x10
0x10 5440E4F50C014275
0x11  0A85A75A1C4F0591
0x12 649D7A078EA2AFD1
0x13 97C399F5DC5B7D0A
0x14 DE74B9809F500D96
0x15 CE797DB4C7E75E70
0x16 683F3AB632CFB158
0x17 02C15581B345BF12
Val  3, Algo 0x18
0x18 BAF7A983C9019AF9
0x19 30261C8CEE6DED45
0x1A 492E4CADEB7C4642
0x1B AB8646B62BB81EC6
0x1C 9A7C90F449CE7392
0x1D B2FEFCB801670CAF
0x1E D5E4724933BE11F2
0x1F FAF98BDB772FED2F
Val  3, Algo 0x20
0x20 0272346259FD655C
0x21 52478D7591C8EB7B
0x22 16CF7216ED2F1F6F
0x23 8A3EED1B80B966B3
0x24 3545D29DC83B3138
0x25 123F39110E99FBEC
0x26 4E33E36D58FFBE97
0x27 FC634FED2E4F3150
Val  3, Algo 0x28
0x28 C0A20669B5C46817
0x29 810295BE2A9315B4
0x2A 7BE21B014B261D07
0x2B C228C5B76442EE12
0x2C A254A26E9EBFE3C4
0x2D F24AE25A95757FAA
0x2E 17C3AE696BBAC701
0x2F 8836BE3DB4777618
Val  3, Algo 0x30
0x30 C523347EDE8234C6
0x31 144937FE76153571
0x32 89DD3BBBC4F62D23
0x33 F9456BCC7F3F264F
0x34 85E03F3044FD6547
0x35 4AD7805BFFFD680F
0x36 A0EAA712D8530D54
0x37 49567FD9F6156D15
Val  3, Algo 0x38
0x38 204B92DF7A02AECF
0x39 DC05D8392B348F5E
0x3A 2C4DE3A440597735
0x3B 52443610F826BF6D
0x3C 185D16B2A5EA029F
0x3D 2AD035740C265D9C
0x3E 57BF9B7A883FFEF4
0x3F 4B5426D01A8B47FF
Val  3, Algo 0x80
0x80 F8FCA67E596EAA7F
0x81 26A46EB69C104811
0x82 F62FC2E588A042C5
0x83 35A561DE86A163C3
0x84 B0D440622562D37B
0x85 CBC472B7911EA89C
0x86 147064D009F13368
0x87 5867D054155DE47C
Val  3, Algo 0x88
0x88 E1A3EF00C75B4AB7
0x89 EA25A80E90CEF086
0x8A 2A384D0C411B746E
0x8B 3B9D7AD4502A4A17
0x8C E7E14CEF616BCFE6
0x8D 2F2CEF1C4F5522B7
0x8E 0E438018C700143B
0x8F 6758932DD4603D0B
Val  3, Algo 0x90
0x90 7C5AEFE3FFA735AE
0x91 22B4F5DFC5B24BEF
0x92 4E9BDFF6FB2DADED
0x93 D827EAE3ACA4F681
0x94 712E51BD0D19BB67
0x95 B07BC7CB23E399FF
0x96 FD8D6FFB8978DF69
0x97 39D0BA2F9290282D
Val  3, Algo 0x98
0x98 A8C13917B9BAAD21
0x99 900B6C4C92DF2D6F
0x9A 207AA81CA4322B8A
0x9B 3B23B7135BF7A93B
0x9C A72720287442F591
0x9D 73595F2742E7A0A5
0x9E EFF1CE6C327A9D53
0x9F CE1094D00278255B
Val  3, Algo 0xA0
0xA0 8B343F0FD8F7B4DD
0xA1 7235A83892A3B99B
0xA2 E9E6956B2EEAA69B
0xA3 B5D5F435D8289EF4
0xA4 BB1F69415EDFACE8
0xA5 AC819EFFC2A07907
0xA6 F87F6CE16E0606B6
0xA7 8AF2A403E32E084D
Val  3, Algo 0xA8
0xA8 BC0AF16C99763E86
0xA9 1644FBE154A833F8
0xAA 834DBDD15C0119FA
0xAB FB7E8F0B32E8A9DE
0xAC 70CB9682504BD860
0xAD 1371567690194A77
0xAE 911726BB356A66A3
0xAF C6D4A4E908EEFE95
Val 3 , Algo 0xB0
0xB0 0E60D4E692F1E49B
0xB1 1D42125AAF2DF955
0xB2 A753A021D6677B76
0xB3 0FE96D5DF56DAF9C
0xB4 50CF76C95D330198
0xB5 299D19B47CD6EA2C
0xB6 F4DAF81C80F46D00
0xB7 75EFF5EBCA7CC7D6
Val  3, Algo 0xB8
0xB8 F92964A5B17C2505
0xB9 201207396EBA3D51
0xBA 556D8921F1CC9F47
0xBB 3E120BCE44C072EE
0xBC BD507CE0DCA96480
0xBD F5F8BAEB35C497B3
0xBE 0B979FB53A518C95
0xBF BF4354A52DE5C80F
*/ 
 
int  lcd_test ( void )
{
	int pos = 0;
	int y = 0;
	int font = 0;
//=======begin
	int page = 0;
	int i = 0;

	const int base_addr = 0x6010000;

	unsigned short tmp_val;
	unsigned short tmp_val2;

	char *buffer = (char*)0x5f65C;
//=======end
	int counter = 0;
	int address = 0;
	char buf[32];

	unsigned short timer_before, timer_after;
	static const unsigned char *values[] = 
	{
		"\x0c\x32\x88\xa2\xc3\x92\xbb\x01\xf2\x61\x55\x66\xf3\xe3\xe3\x4b\xc8\xd8\x83\xb8\x1e\x20\x9e\x55\x16\xb1\x9e\xdb\x9d\x55\xc0\x60\x62\x30\x62\x61\xde\xd0\xfc\x17\x08\xbc\x96\x18\x78\x2b\x63\xd6\x1f\x46\xa3\x90\x0a\x8e\xd9\xd5\xf5\x1e\x27\xe3\x75\x4d\xf2\x43", // claudia
		"\x38\xE9\xBA\xE3\x00\xDE\xF4\xF1\xA9\x00\x00\x04",
/*		"\xA2\x63\xD9\x04\xE7\xA3\xDA\xB7", //8310 albert
		"\xE7\xA3\xDA\xB7\xA2\x63\xD9\x04", // swapped
		"\xDC\xB2\xCA\x4F\xE7\x4E\x3C\x1E", // NHL-4JX
		"\x58\x18\x90\x81\x4C\x96\xE5\xC6", // NHL-4
		"\xCC\x96\xC0\xC0\xBB\x73\xC8\x5C", // NHL-4
		"\x72\xB8\x32\xF0\x26\x94\x47\x49", // NHL-4 */
		0
	};

#ifdef g3gg0
	lcd_init ();

	while ( values[i] )
	{
		memset ( 0x4c3b0, 0xFF, 0x400 );
		decrypt_claudia ( values[1] );
		//decrypt_test ( i, values[i] );
		usleep ( 2 sec );
		i++;
	}
#else


	lcd_init ();

//=================get system speed ? or something else speed ?
    lcd_clear();
    sprintf ( buffer, "%d", system_get_type_freq () );
    lcd_write ( 0,0, buffer, COLOR ( 15,0,0 ), 0 );
    usleep ( 4 sec );

//=================timer is timer test
    lcd_clear();
    sprintf ( buffer, "timer: %u", timer_is_timer ( (short*)0x6010038 ) );
    lcd_write ( 0,0, buffer, COLOR(0,0,0), 0 );
    usleep ( 4 sec );

//=================timer clock test
	lcd_clear();

	for ( i = 0; i < 10; i++ )
	{

        //lcd_write ( 0, 0, "1000 Noobs", COLOR(0,0,0), 0 );

        timer_before = *(short*)0x6010038;

        _1000nops();
        _1000nops();
        _1000nops();
        _1000nops();
        _1000nops();
        _1000nops();
        _1000nops();
        _1000nops();
        _1000nops();
        _1000nops();
        timer_after = *(short*)0x6010038;

        sprintf ( buffer, "%d: %08X ", i, timer_before );
        sprintf ( buffer+strlen(buffer), "%08X", timer_after );
        lcd_write ( 0, i*10, buffer, COLOR(0,0,0), 0 );
//        lcd_write ( 0, i*10, "test", COLOR(0,0,0), 0 );
        //usleep ( 2 sec );
	}

	while ( 1 );
// ============================ timer clock test end
/*
	while ( font < 6 )

	{
		lcd_clear ();
		y = 0;
		lcd_write ( 0, y, " Hello World! ", COLOR(15,0,0), font );
		y += lcd_font_height( font );
		lcd_write ( 0, y, "--------------", COLOR(0,15,0), font );
		y += lcd_font_height( font );
		lcd_write ( 0, y, "this is a test", COLOR(0,0,15), font );

		font++;
		usleep ( 2 sec );
	}

	lcd_clear ();
	font = 0;
	y = 0;
	lcd_write ( 0, y, " Now going to ", COLOR(15,0,0), font );
	y += lcd_font_height( font );
	lcd_write ( 0, y, "   dump some  ", COLOR(0,15,0), font );
	y += lcd_font_height( font );
	lcd_write ( 0, y, "  funny regs  ", COLOR(0,0,15), font );

	usleep ( 2 sec );
*/

//=======================
    i = 0;
    y = 0;
	lcd_clear ();
	address = 0x06010000;
//	while ( address < 0x06010800 )
    while ( 1 )
	{
	    y = 0;
//	    lcd_draw_rect ( 0, 120, y, y+10, COLOR(15,15,15) );
	    dump_4reg_short ( 0, 0, (short*)0x6010038 );
	    y+=10;

//	    lcd_draw_rect ( 0, 120, y, y+10, COLOR(15,15,15) );
	    dump_4reg_short ( 0, 10, (short*)0x601003a );
	    y+=10;

//	    lcd_draw_rect ( 0, 120, y, y+10, COLOR(15,15,15) );
	    dump_4reg_short ( 0, 20, (short*)0x6010112 );
	    y+=10;

//	    lcd_draw_rect ( 0, 120, y, y+10, COLOR(15,15,15) );
	    dump_4reg_short ( 0, 30, (short*)0x6010116 );
	    y+=10;

//	    lcd_draw_rect ( 0, 120, y, y+10, COLOR(15,15,15) );
	    dump_4reg_short ( 0, 40, (short*)0x6010436 );
	    y+=10;

//	    lcd_draw_rect ( 0, 120, y, y+10, COLOR(15,15,15) );
	    dump_4reg_short ( 0, 50, (short*)0x601080c );
	    y+=10;

//	    lcd_draw_rect ( 0, 120, y, y+10, COLOR(15,15,15) );
	    dump_4reg_short ( 0, 60, (short*)0x6010910 );
	    y+=10;

//	    lcd_draw_rect ( 0, 120, y, y+10, COLOR(15,15,15) );
	    dump_4reg_short ( 0, 70, (short*)0x6010928 );
	    y+=10;

//	    lcd_draw_rect ( 0, 120, y, y+10, COLOR(15,15,15) );
	    dump_4reg_short ( 0, 80, (short*)0x601092a );

	    usleep ( 100 msec );

//==========================

//	    usleep ( 100 msec );
//	    if ( !hw_io_get_pin ( 2 ) )
//	    {
//	        usleep ( 50 msec );
//	        if ( !hw_io_get_pin ( 2 ) )
//                page++;
//	    }
//	    sprintf ( buffer, "r %08X-%08X", base_addr+page*64, base_addr+(page+1)*64 );
//	    lcd_write ( 0, 0, buffer, COLOR (0,0,0), 0 );
//
//	    for ( y = 0; y < 8; y++ )
//            dump_4reg_short ( 0, y*10+10, (short*)(base_addr+page*64+8*y) );
//
//		//dump_reg_short ( 0, 0,  (short*)0x6010400 );
//		//dump_4reg_short ( 0, 0, (short*)0x6010400 );
////		dump_reg_short ( 0, 10, (short*)0x6010402 );
////		dump_reg_short ( 0, 20, (short*)0x6010404 );
////		dump_reg_short ( 0, 30, (short*)0x6010406 );
////		dump_reg_short ( 0, 40, (short*)0x6010408 );
////		dump_reg_short ( 0, 50, (short*)0x601040A );
////		dump_reg_short ( 0, 60, (short*)0x601040C );
////		dump_reg_short ( 0, 70, (short*)0x601040E );
//
//        //buffer = "nokia tries to make live harder :-)";
//        sprintf ( buffer, "- page %d -", page );
//		lcd_write ( 0, 100, buffer, COLOR(15,0,0), 0 );
//		//dump_genio     ( 0, 80, 2 );
//		wdt_reset();

//============================
//        if ( !(y % 0x100) )
//        {
//            lcd_draw_rect ( 0, 120, 0, 10, COLOR(0,0,0) );
//            sprintf ( buffer, "scanning %08X", base_addr+y );
//            lcd_write ( 0, 0, buffer, COLOR(0,0,0), 0 );
//        }
//        tmp_val = *(short*)(base_addr+y);
//        usleep ( 2 msec *2 );
//        if ( *(short*)(base_addr+y) != tmp_val )
//        {
//            usleep ( 1 msec *2 );
//            if ( (tmp_val2 = *(short*)(base_addr+y) ) != tmp_val )
//            {
//                //lcd_clear();
//                sprintf ( buffer, "%08X: ", base_addr+y );
//                sprintf ( buffer+strlen(buffer), "%04X ", tmp_val & 0xFFFF );
//                sprintf ( buffer+strlen(buffer), "%04X", tmp_val2 & 0xFFFF );
//                lcd_write ( 0, 10+10*i, buffer, COLOR(15,0,0), 0 );
//                i++;
//                usleep ( 1 sec );
//            }
//            //usleep ( 2 sec );
//        }
//        if ( y < 0xb00 )
//            y += 2;

//============================
//		counter = 5;
//		while ( counter )
//		{
//			counter--;
//
////			lcd_clear ();
//			dump_reg_short ( 0, 10,  (short*)address );
//			dump_reg_short ( 0, 20, (short*)address+0x04 );
//			dump_reg_short ( 0, 30, (short*)address+0x08 );
//			dump_reg_short ( 0, 40, (short*)address+0x0C );
//			dump_reg_short ( 0, 50, (short*)address+0x10 );
//			dump_reg_short ( 0, 60, (short*)address+0x14 );
//			dump_reg_short ( 0, 70, (short*)address+0x18 );
//			dump_reg_short ( 0, 80, (short*)address+0x1C );
//			sprintf ( buf, "time:  %02i    ", counter );
//			lcd_write ( 20, 100, buf, COLOR((counter%16),(counter/2)%16,(counter/4)%16), 0);
//
//			usleep ( 1 msec );
//		}
//		address += 0x08;

	}
#endif
}



