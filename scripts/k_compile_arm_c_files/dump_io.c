#define NULL (void*)0


void dump_io_range ( char *addr, int range )
{
    int i = 0;
    unsigned char c;
    at_printf ( "dumping char 0x%04X%04X, range 0x%04X", (int)addr >> 16, (int)addr & 0xFFFF, range );
    while ( i != range )
    {
        if ( !(i % 32) )
        {
            at_printf ( "\n" );
            at_send_success ( 0xfe, 0xe );
        }
        disable_int();
        //enable_access();
        c = *(addr+i);
        //disable_access();
        enable_int();
        at_printf ( "%02X ", c );
        i++;
    }
    at_printf ( "\ndump end\n" );
    at_send_success ( 0xfe, 0xe );
}

void dump_io_range_short ( short *addr, int range )
{
    int i = 0;
    unsigned short c;
    at_printf ( "dumping short 0x%04X%04X, range 0x%04X", (int)addr >> 16, (int)addr & 0xFFFF, range );
    while ( i != range )
    {
        if ( !(i % 16) )
        {
            at_printf ( "\n" );
            at_send_success ( 0xfe, 0xe );
        }
        disable_int();
        //enable_access();
        c = *(addr+i);
        //disable_access();
        enable_int();
        at_printf ( "%04X ", c );
        i++;
    }
    at_printf ( "\ndump end\n" );
    at_send_success ( 0xfe, 0xe );
}

void dump_io_range_int ( int *addr, int range )
{
    int i = 0;
    unsigned int c;
    at_printf ( "dumping int 0x%04X%04X, range 0x%04X", (int)addr >> 16, (int)addr & 0xFFFF, range );
    while ( i != range )
    {
        if ( !(i % 8) )
        {
            at_printf ( "\n" );
            at_send_success ( 0xfe, 0xe );
        }
        disable_int();
        //enable_access();
        c = *(addr+i);
        //disable_access();
        enable_int();
        at_printf ( "%04X%04X ", c >> 16, c & 0xFFFF );
        i++;
    }
    at_printf ( "\ndump end\n" );
    at_send_success ( 0xfe, 0xe );
}

void get_dump_range_char ( char *buffer, char *addr, int range )
{
    int i = 0;
    while ( i != range )
    {
        buffer[i] = addr[i];
        i++;
    }
}

void get_dump_range_short ( short *buffer, short *addr, int range )
{
    int i = 0;
    while ( i != range )
    {
        buffer[i] = addr[i];
        i++;
    }
}

void get_dump ( char *buffer )
{
    get_dump_range_short ( (short*) buffer          , (short*)0x6010200 , 0x20    );
    get_dump_range_short ( (short*)(buffer+0x40)    , (short*)0x6010400 , 0x20    );
    /*
    get_dump_range_short ( (short*)buffer           , (short*)0x600B000 , 0x80  );
    get_dump_range_short ( (short*)(buffer+0x100)   , (short*)0x600C000 , 0x80  );
    get_dump_range_short ( (short*)(buffer+0x200)   , (short*)0x6010000 , 0x80  );
    get_dump_range_short ( (short*)(buffer+0x300)   , (short*)0x6010400 , 0x80  );
    get_dump_range_short ( (short*)(buffer+0x400)   , (short*)0x6010B00 , 0x80  );
    get_dump_range_char  ( buffer+0x500             , (char*)0x6100000  , 0x100 );
    */
}

//moves buffer2 in buffer1 after compare!
/*void dump_cmp ( char *buffer1, char *buffer2, char *mask )
{
    int i;
    for ( i = 0; i < 0x600; i++ )
    {
        //if ( buffer1[i] != buffer2[i] )
        mask[i] |= buffer1[i] ^ buffer2[i];
        buffer1[i] = buffer2[i];
    }

}

int check ( char* cmp1, char* cmp2, int len )
{
    int i = 0;

    while ( i != len )
    {
        if ( cmp1[i] != cmp2[i] )
            return 0;
        i++;
    }

    return 1;
}

char *find_ownmalloc ( )
{
    char *addr = (char*)0x50000;
    int found = 0,i;

    while ( addr < (char*)0x90000 )
    {
        if ( check ( addr, "mymallocblock", 13 ) )
            return addr+13;
        addr++;
    }

    return NULL;
}

char *own_malloc ( int size )
{
    int i;
    char *buffer;

    buffer = (char*)malloc ( size + 13 );
    for ( i=0; i < 13; i++ )
        buffer[i] = "mymallocblock"[i];

    return buffer + 13;
}

void own_free ( char *buf )
{
    if ( !buf )
        return;
    buf -= 13;
    free ( buf );
}

void hook_sec_init ()
{
    char *buffer = (char*)0x5f65C;;

    //buffer = own_malloc ( 0x600*2 );
    get_dump ( buffer );
    sec_init();
    get_dump ( buffer + 0x600 );

}
*/
void hook_fchk ()
{
//    char *buffer = (char*)0x5f65C;;
//    int i;

    //buffer = own_malloc ( 0x600*2 );
    //get_dump ( buffer );

    branch_to_r1 ( 0x900006C, 0x840015 );
    saveregs();
    //saveregs();
//    buffer[0] = i >> 24 & 0xFF;
//    buffer[1] = i >> 16 & 0xFF;
//    buffer[2] = i >> 8 & 0xFF;
//    buffer[3] = i &0xFF;
    //get_dump ( buffer + 0x600 );
}

/* saveregs asm
.trix

.sub saveregs
push r0,r1,r2,r3,r4,r5,r6,r7,lr
push r1
ldr r1, @buf
str r0, r1, 0
pop r1
ldr r0, @buf
str r1, r0, 4
mov r1, #8
str r2, r0, r1
add r1, #4
str r3, r0, r1
add r1, #4
str r4, r0, r1
add r1, #4
str r5, r0, r1
add r1, #4
str r6, r0, r1
add r1, #4
str r7, r0, r1
add r1, #4
mov r2, h0
str r2, r0, r1
add r1, #4
mov r2, h1
str r2, r0, r1
add r1, #4
mov r2, h2
mov r3, h3
mov r4, h4
mov r5, h5
mov r6, h6
mov r7, h7

str r2, r0, r1
add r1, #4
str r3, r0, r1
add r1, #4
str r4, r0, r1
add r1, #4
str r5, r0, r1
add r1, #4
str r6, r0, r1
add r1, #4
str r7, r0, r1

pop r0,r1,r2,r3,r4,r5,r6,r7,lr
@buf:
dd 5f65c

.end

.end
*/


void dump_io ( int *obj, int method )
{

    int count, i;
    char *buffer = (char*)0x5f65C;
    //char *mask = NULL;
    //char *tmp = NULL;

    /* use *#0000# or *#06# ... to start */
    if ( *obj != 0x1497dac || method != 0x8743 )
        return;


    at_printf ( "\n" );

    //dump_io_range_short ( (char*)0x8001BA, 0x1000 );
    //i = branch_to_r1 ( 0x900006C, 0x840015 );
    /*for ( i = 0; i < 15*4; i++ )
        at_printf ( "%s%02X", i%4 ? " " : "\n", buffer[i] );*/

    for ( i = 0; i < 0x24; i++ )
    {
        count = get_uem ( i << 16 | 0xFFFF );
        at_printf ( "\n%02X: %04X%04X", i, count >> 16, count & 0xFFFF );
    }

    at_printf ("\ndump end\n" );
    at_send_success ( 0xfe, 0xe );


    /*
        //dump before and after fchk
    //buffer = find_ownmalloc ();
    buffer = (char*)0x5f65C;
    if ( !buffer )
    {
        at_printf ( "didn't find out malloc block!\n" );
        return;
    }

    at_printf ( "\n/* 2* io dump * /\n" );
    at_printf ( "\n/* dump before sec_init() * /\n" );

    for ( i = 0; i < 0x80/*0x600* /; i++ )
    {
        if ( !(i % 0x8) )
        {
            if ( !(i % 0x40) )
            {
                at_printf ( "\n" );
                at_send_success ( 0xfe, 0xe );
            }
            else
                at_printf ( " " );
        }
        at_printf ( "%02X", buffer[i] );
    }

    at_printf ( "\n/* dump after sec_init() * /\n" );
    for ( i = 0x600; i < 0x600+0x80/*0x600*2* /; i++ )
    {
        if ( !(i % 0x8) )
        {
            if ( !(i % 0x40) )
            {
                //if ( !(i % 0x100) )
                //    at_printf ( "\n/* --- new range --- * /" );
                at_printf ( "\n" );
                at_send_success ( 0xfe, 0xe );
            }
            else
                at_printf ( " " );
        }
        at_printf ( "%02X", buffer[i] );
    }

    at_printf ( "\n/* io dump end * /\n" );
    at_send_success ( 0xfe, 0xe );

    */
/*  //ownmalloc test

    buffer = own_malloc ( 0x1000 );
    buffer[0] = 'h';
    buffer[1] = 'i';
    buffer[2] = 0;

    at_printf ( "malloc: %04X%04X\n", (int)buffer >> 16, (int)buffer & 0xFFFF );
    buffer = find_ownmalloc();
    at_printf ( "found mymallocblock: %04X%04X : %s\n", (int)buffer >> 16, (int)buffer & 0xFFFF, buffer );
    at_send_success ( 0xfe, 0xe );

*/


    //malloc tests
    /*at_printf ( "\n" );
    for ( i = 1; i <= 131072; i = 2*i )
    {
        buffer = (char*)malloc ( i );
        at_printf ( "malloc(%04X%04X): 0x%04X%04X\n", i>>16, i&0xFFFF, (int)buffer >> 16, (int)buffer & 0xFFFF );
        at_send_success ( 0xfe, 0xe );
        free ( buffer );
    }*/

/*  //get 10 dumps, compare and generate a bitmask of changed values

    buffer = (char*) malloc ( 0x600 );
    mask = (char*) malloc ( 0x600 );
    tmp = (char*) malloc ( 0x600 );

    for ( i = 0; i < 0x600; i++ )
        mask[i] = 0x00;

    get_dump ( buffer );

    for ( i = 0; i < 10; i++ )
    {
        get_dump ( tmp );
        dump_cmp ( buffer, tmp, mask );
    }

    at_printf ( "\n/* io dump * /\n" );
    at_printf ( "\n/* bits changed somewhere in 10 loops are marked 1 * /\n" );

    for ( i = 0; i < 0x600; i++ )
    {
        if ( !(i % 0x8) )
        {
            if ( !(i % 0x40) )
            {
                at_printf ( "\n" );
                at_send_success ( 0xfe, 0xe );
            }
            else
                at_printf ( " " );
        }
        at_printf ( "%02X", mask[i] );
    }

    at_printf ( "\n/* io dump end * /\n" );
*/
/*  //dumps also / ldr/ldrh/ldrb tests
    at_printf ( "\n/*  io dump started  * /\n" );

    for ( count = 1; count <= 5; count++ )
    {
        at_printf ( "\n/*  %d. dump  * /\n", count );
        //dump_io_range ( (char*)0x1000000, 0x1000 ); //works
        //dump_io_range_int ( (int*)0x6000000, 0x40 ); //doesn't work with byte, short, int :-(
        dump_io_range_short ( (short*)0x600B000, 0x80 ); //work with short
        dump_io_range_short ( (short*)0x600C000, 0x80 ); //work with short
        dump_io_range_short ( (short*)0x6010000, 0x80 ); //work with short
        dump_io_range_short ( (short*)0x6010400, 0x80 ); //work with short
        dump_io_range_short ( (short*)0x6010B00, 0x80 ); //work with short
        dump_io_range ( (char*)0x6100000, 0x100 );  //works
    }

    at_printf ( "\n/*  io dump ended  * /\n" );
*/

    //at_send_success ( 0xfe, 0xe );

    //free ( buffer );
    //free ( mask );
    //free ( tmp );




}

