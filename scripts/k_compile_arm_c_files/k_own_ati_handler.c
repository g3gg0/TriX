#define AM_BYTE 1
#define AM_HALF 2
#define AM_WORD 4

#define err { at_printf ( "err\n" ); free ( buffer ); return 0x320; }

/*void dump_buffer ( int x, int y, unsigned char *addr, int bytes )
{
	int i = 0;
	char buf[3];

	while ( i < bytes )
	{
		sprintf ( buf, "%02X", addr[i] );
		/*x = lcd_write ( x, y, buf, COLOR(0,0,0), 0);
		if ( i && (i&3) == 3 )
			x += 4;* /
		i++;
		at_printf ( buf );
	}
	while ( i < bytes )
	{
		sprintf ( buf, "%02X", addr[i]^addr[i-bytes/2] );
		/*x = lcd_write ( x, y, buf, COLOR(0,0,0), 0);
		if ( i && (i&3) == 3 )
			x += 4;* /
		i++;
		at_printf ( buf );
	}
	return;
}

int decrypt_test ( int pos, const unsigned char *value )
{
	int algo = 0x00;
	int algoffset = 0;
	int i = 0;
	int x = 0;
	int y = 0;
	unsigned char buffer[64];

	while ( algo < 0xC0 )
	{

		sprintf ( buffer, "Val %i, Algo 0x%02X\n", pos+1, algo  );
		//lcd_write ( 20, 10, buffer, COLOR(15,0,0), 0);
		at_printf ( buffer );
		y = 30;
		algoffset = 0;
		while ( algoffset < 8 )
		{
			sprintf ( buffer, "0x%02X ", algo+algoffset  );
			//lcd_write ( 0, y, buffer, COLOR(0,15,0), 0);
			at_printf ( buffer );

			for ( i=0; i<8; i++ )
				buffer[i] = value[i];

			decrypt ( buffer, buffer, 8, algo+algoffset );
			dump_buffer ( 26, y, buffer, 8 );
			at_printf ( "\n" );
			y+=10;
			algoffset++;
		}
		//usleep ( 5 sec );
		algo+=8;

		if ( algo == 0x40 )
			algo = 0x80;
	}

	return 0;

}
*/

// from strcmp.de
int strcmp( const char *a, const char *b)
{
    while( *a == *b && *a )
        a++,++b;

    return *(const unsigned char*)a-*(const unsigned char*)b;
}

// original from Linus Thorvalds
int strncmp(const char * cs,const char * ct,int count)
{
	register signed char __res = 0;

	while (count) {
		if ((__res = *cs - *ct++) != 0 || !*cs++)
			break;
		count--;
	}

	return __res;
}

inline int char2int ( const char c )
{
    if ( c >= 'a' && c <= 'f' )
        return c - 'a' + 0xa;
    if ( c >= 'A' && c <= 'F' )
        return c - 'A' + 0xa;
    if ( c >= '0' && c <= '9' )
        return c - '0';
    return -1;
}


int hex2int ( const char* str )
{
    int len = strlen ( str );
    int i = 0;
    int tmp;
    int ret = 0;

    while ( i < len )
    {
        tmp = char2int ( str[i] );
        if ( tmp == -1 )
            return ret;
        ret <<= 4;
        ret |= tmp;
        i++;
    }

    return ret;
}

//lazy lazy *g
/*
    cmds:
        ATI0|get|1|1A|1234ABCD
            ATI0|get|accessmode|bytes|addr
*/

// store bytes content of addr as string
void dump ( void *addr, int bytes, char *destbuf, int accessmode )
{
    int i = 0;

    while ( i < bytes )
    {
        //disable_int();
        switch ( accessmode )
        {
            case AM_BYTE:
                if ( !(i%0x20) )
                    sprintf ( destbuf, "\n%08X: %02X", (int)addr+i, *(char*)(addr+i) & 0xFF);
                else
                    sprintf ( destbuf, "%s%02X", i % 4 ? "" : " ", *(char*)(addr+i) & 0xFF);
                break;
            case AM_HALF:
                if ( !(i%0x20) )
                    sprintf ( destbuf, "\n%08X: %04X", (int)addr+i, *(short*)(addr+i) & 0xFFFF);
                else
                    sprintf ( destbuf, "%s%04X", i % 4 ? "" : " ", *(short*)(addr+i) & 0xFFFF );
                break;
            case AM_WORD:
                if ( !(i%0x20) )
                    sprintf ( destbuf, "\n%08X: %08X ", (int)addr+i, *(int*)(addr+i) );
                else
                    sprintf ( destbuf, "%08X ", *(int*)(addr+i) );
                break;
        }
        //enable_int();
        destbuf += strlen ( destbuf );
        i += accessmode;
    }
}

void find ( void *start, void *end, char *search, char *destbuf, int accessmode )
{
    int i;

    i = hex2int ( search );

    while ( start < end )
    {
        switch ( accessmode )
        {
            case AM_BYTE:
                if ( *(char*)start == i & 0xFF )
                    sprintf ( destbuf, "found at: 0x%08X\n", (int)start );
                break;
            case AM_HALF:
                if ( *(short*)start == i & 0xFFFF )
                    sprintf ( destbuf, "found at: 0x%08X\n", (int)start );
                break;
            case AM_WORD:
                if ( *(int*)start == i )
                    sprintf ( destbuf, "found at: 0x%08X\n", (int)start );
                break;
        }
        destbuf += strlen ( destbuf );
        start += accessmode;
    }

}

//unsigned int cool();

void irq_main_handler_hook ( int func, int irq_tbl_offset )
{
    unsigned int *buffer = (int*)0x5f65C;

    buffer[irq_tbl_offset/8]++;
    asm volatile (  "mov r0, %0;"
                    "mov r1, %1;"
                    "bl bx_r0;" :: "r" ( func ), "r" ( irq_tbl_offset ) );

}

unsigned int ati_print_handler ( int unused1, char *cmdend, short *error_addr )
{
    //???????????????????????????
    //asm ( "push {r1-r7}" );
    short *new_addr = (short*)*(int*)0x145CC;   //*some_addr;

    char *buffer;
    char *tmp;
    char accessmode = AM_BYTE;
    //char run = 1;
/*
    static const unsigned char *values[] =
	{
		"\xE7\x4E\x3C\x1E\xDC\xB2\xCA\x4F",
		"\xE3\xC1\x9A\xB6\x74\x94\x11\x82",
		"\x00\xAC\xAF\x52\xC0\xCB\x0C\x7C",
		0
	};
*/
    unsigned short ati_val = new_addr[0x2d]; //*((*some_addr)+0x2d);
    int i, start = ati_val * 0x200;

    int bytes = 0, r0, r1, r2;

    //asm volatile ( "ldr  %0, [%1,#0x00]":"=r" ( new_addr ):"r" ( some_addr ) );

//    if ( ati_val == 0x0 )
//    {
//        i = 0;
//        at_printf ( "can't read from 0x0...\n" );
//        //at_printf ( "dumping decrypt test for g3gg0 :-)\n" );
//       /* while ( values[i] )
//        {
//            decrypt_test ( i, values[i] );
//            //usleep ( 2 sec );
//            i++;
//        }*/
//    }
//    else
//    {

        buffer = (char*)malloc ( 1024 );
        if ( !buffer )
            err;
        //sprintf ( buffer, "\nr0: %08X\nr1: %08X\nr2: %08X(%08X)\nr4: %08X\n", unused1, unused, (int)error_addr, *(int*)error_addr, r4 );
        while ( *(char*)cmdend != 'A' && *(char*)(cmdend+1) != 'T' )
            cmdend--;
        //cmdend--;
//        at_printf ( "call string: " );
//        at_printf ( (char*)(cmdend) );
//        at_printf ( "\n" );

        tmp = strchr ( cmdend, '|' );

        if ( !tmp )
            err;
        //at_printf ( tmp );
        if ( !strncmp ( tmp+1, "get", 3 ) )
        {
            //at_printf ( tmp+1 );
            at_printf ( "\nget\n" );
            if ( !(tmp = strchr ( tmp+1, '|' )) )
                err;
//            at_printf ( "noerr" );

            if ( !strchr ( tmp+1, '|' ) || !strchr ( strchr ( tmp+1, '|' ) + 1, '|' ) )
                err;

            bytes = hex2int ( strchr ( tmp+1, '|' ) + 1 );
            start = hex2int ( strchr ( strchr ( tmp+1, '|' ) + 1, '|' ) + 1 );

            dump ( (void*)start, bytes, buffer, tmp[1] - 0x30 );
            //sprintf ( buffer, "am: %02X, addr: %08X, bytes: %08X\n", tmp[1], start, bytes );
//            switch ( tmp[1] )
//            {
//                case '1':
//                    sprintf ( buffer, "get %08X: %02X\n", i, *(char*)i );
//                    break;
//                case '2':
//                    sprintf ( buffer, "get %08X: %04X\n", i, *(short*)i );
//                    break;
//                case '4':
//                    sprintf ( buffer, "get %08X: %08X\n", i, *(int*)i );
//                    break;
//                default:
//                    sprintf ( buffer, "get: unknown accessmode\n" );
//            }
            at_printf ( buffer );
        }
        else
            if ( !strncmp ( tmp+1, "set", 3 ) )
            {
                at_printf ( "\nset\n" );
                tmp = strchr ( tmp+1, '|' );
                if ( !tmp )
                    err;

                if ( !strchr ( tmp+1, '|' ) || !strchr ( strchr ( tmp+1, '|' ) + 1, '|' ) )
                    err;

                start = hex2int ( strchr ( tmp+1, '|' ) + 1 );  //addr
                bytes = hex2int ( strchr ( strchr ( tmp+1, '|' ) + 1, '|' ) + 1 );  //value

                sprintf ( buffer, "set %08X to ", start );
                at_printf ( buffer );
                buffer[0] = 0;
                disable_int();
                switch ( tmp[1] )
                {
                    case '1':
                        *(char*)start = bytes & 0xFF;
                        sprintf ( buffer, "%02X\n", bytes & 0xFF );     //don't like the sprintf with disabled ints... but for now it's ok
                        break;
                    case '2':
                        *(short*)start = bytes & 0xFFFF;
                        sprintf ( buffer, "%04X\n", bytes & 0xFFFF );
                        break;
                    case '4':
                        *(int*)start = bytes;
                        sprintf ( buffer, "%08X\n", bytes );
                        break;
                }
                enable_int();
                at_printf ( buffer );
            }
            else
                if ( !strncmp ( tmp+1, "call", 4 ) )
                {
                    at_printf ( "\ncall\n" );
                    tmp = strchr ( tmp+1, '|' );
                    if ( !tmp )
                        err;

                    start = hex2int ( tmp+1 ) + 1;  //func, add 1 - thumb
                    r0 = 0;
                    r1 = 0;
                    r2 = 0;
                    if ( tmp = strchr ( tmp+1, '|' ) )
                    {
                        tmp++;
                        r0 = hex2int ( tmp );
                        if ( tmp = strchr ( tmp, '|' ) )
                        {
                            tmp++;
                            r1 = hex2int ( tmp );
                            if ( tmp = strchr ( tmp, '|' ) );
                            {
                                tmp++;
                                r2 = hex2int ( tmp );
                            }
                        }
                    }

                    //replace with asm code
//                    asm volatile ( "mov r0, %0"::"r" ( r0 ) );
//                    asm volatile ( "mov r1, %0"::"r" ( r1 ) );
//                    asm volatile ( "mov r2, %0"::"r" ( r2 ) );
//                    asm volatile ( "mov r4, %0"::"r" ( start ) );
//                    asm volatile ( "bl bx_r4" );
//                    asm volatile ( "mov %0, r0":"=r" ( r0 ): );
                    asm volatile ("mov r0, %1; mov r1, %2; mov r2, %3; mov r4, %4; BL bx_r4; mov %0, r0"
                        : "=r" (r0) /* Outputs */
                        : "r" (r0), "r" (r1), "r" (r2), "r" (start) /* Inputs */
                        : "r0", "r1", "r2", "r3", "ip", "lr", "memory"); /* possible modified */
                    //r0 = bx_r2 ( r0, r1, start );
                    sprintf ( buffer, "\n%08X returned %08X\n", start, r0 );
                    at_printf ( buffer );
                }
                else
                    if ( !strncmp ( tmp+1, "find", 4 ) )
                    {
                        at_printf ( "\nfind\n" );

                        tmp = strchr ( tmp+1, '|' );
                        if ( !tmp )
                            err;
                        tmp++;
                        accessmode = tmp[0] - 0x30;

                        tmp = strchr ( tmp, '|' );
                        if ( !tmp )
                            err;
                        tmp++;
                        start = hex2int ( tmp );    //search start addr

                        tmp = strchr ( tmp, '|' );
                        if ( !tmp )
                            err;
                        tmp++;
                        r0 = hex2int ( tmp );       //search end addr

                        tmp = strchr ( tmp, '|' );
                        if ( !tmp )
                            err;
                        tmp++;
                        buffer[0] = 0;
                        find ( (int*)start, (int*)r0, tmp, buffer, accessmode );
                        at_printf ( buffer );
                    }
                    else
                        if ( !strncmp ( tmp+1, "clear", 5 ) )
                        {
                            for ( i = 0; i < 4*0x1f; i += 4 )
                                *(int*)(0x5f65C+i) = 0;
                            at_printf ( "\ncleared\n" );
                        }
                        else
                        {
                            at_printf ( "unknow cmd\n" );
                            //call n* ati handler ...
                        }


    //cool ();




/*
        sprintf ( buffer, "\ndumping 0x%X - 0x%X:\n", start, start+0x200 );
        at_printf ( buffer );
        for ( i = 0; i < 0x200; i += 4 )
        {
            if ( !(i%0x20) )
                sprintf ( buffer, "\n%08X: %08X ", start+i, *(int*)(start+i) );
            else
                sprintf ( buffer, "%08X ", *(int*)(start+i) );
            at_printf ( buffer ); //"you called ATI%04X\n" );
        }
        at_printf("\n\n");
*/
        free ( buffer );
//    }

    //asm ( "pop {r1-r7}" );
    return 0x320;

}
