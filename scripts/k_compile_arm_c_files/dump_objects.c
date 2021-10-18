
#define NULL (void*)0

//#define BUFFERING

/* object classes */
#define EventHandler 0x1491ca8
    #define StandardNumKH 0x148efa8
    #define InfKEventHandler 0x14909e0

#define MessageMemory 0x1463f64
    #define PMMMessageMemory 0x14636a0
        #define PMMTemplateMemory 0x14636b4
    #define SIMMessageMemory 0x1463bfc
#define TerminalAdapter 0x1522a84
#define CommPep 0x15228a0

/*
    at_printf restrictions:
    %c not known
    %08X not know, use %04X%04X
*/


typedef struct s_objects
{
    int addr;
    char *name;
} t_objects;

const t_objects objects[] =
{
    MessageMemory       , "MessageMemory     ",
    PMMMessageMemory    , "PMMMessageMemory  ",
    PMMTemplateMemory   , "PMMTemplateMemory ",
    SIMMessageMemory    , "SIMMessageMemory  ",
    StandardNumKH       , "StandardNumKH     ",
    InfKEventHandler    , "InfKEventHandler  ",
    0, NULL
};

typedef struct s_methods
{
    unsigned short id;
    char *name;
} t_methods;

const t_methods methods[] =
{
    0xc000, "ISOFCL",
    0xc001, "GETMHA",
    0xc002, "GETCLA",
    0xc003, "M_NEW ",
    0xc004, "CONSTR",
    0xc005, "DELETE",
    0xc006, "DESTRU",
    0, NULL
};

//dump if it's the current object
#define this_object 1
//dump if it that is the super object
#define super_object 2
#define object_pointer 3

typedef struct s_objecttodump
{
    char type;
    unsigned int object;
} t_objecttodump;

const t_objecttodump objectstodump[] =
{
    this_object, InfKEventHandler,
    0, 0
};


at_draw_string ( char *uni )
{
    int i;
    uni = (char*)get_string ( uni, *(char*)0x23124 );
    for ( i = 0; uni[i+1]; i+=2 )
        at_printf ( "%s", &uni[i+1] );
}

int scan_args_for_string ( int *ptr_stack )
{
    switch ( *(ptr_stack+4) )
    {
        case 0x12aa3d1:
            return 0;
            break;
        case 0x12aa3b3:
            if ( *(ptr_stack+2) > 0x1550000 )
                return 1;
            break;
        case 0x12AA391:
            //in r7 & r6
            //at_printf ( "2P" );
            if ( *(ptr_stack+3) > 0x1550000 )
                return 1;
            if ( *(ptr_stack+2) > 0x1550000 )
                return 1;
            break;
        case 0x12aa36d:
            //in r7, r6, sp
            //at_printf ( "3P" );
            if ( *(ptr_stack+3) > 0x1550000 )
                return 1;
            if ( *(ptr_stack+2) > 0x1550000 )
                return 1;
            if ( *(ptr_stack+5) > 0x1550000 )
                return 1;
            break;
        case 0x12AA3F7:
            //at_printf ( "s4P %04X%04X %04X%04X %04X%04X %04X%04X", *(ptr_stack+3) >> 16, *(ptr_stack+3) & 0xFFFF, *(ptr_stack+2) >> 16, *(ptr_stack+2) & 0xFFFF, *(ptr_stack+5) >> 16, *(ptr_stack+5) & 0xFFFF, *(ptr_stack+1) >> 16, *(ptr_stack+1) & 0xFFFF );
            //at_printf ( " it's super" );
            //it's the super object! it's called from somewhere/child requesting the super object method!
            if ( *(ptr_stack+2) > 0x1550000 )
                return 1;
            if ( *(ptr_stack+3) > 0x1550000 )
                return 1;
            if ( *(ptr_stack+5) > 0x1550000 )
                return 1;
            if ( *(ptr_stack+1) > 0x1550000 )
                return 1;
            break;
    }

    return 0;

}

void draw_param ( int p )
{
    int i;
    short *unicode;
    char *test;

#ifdef BUFFERING
    char *buffer = (char*)0x5f65C;
    unsigned short *buf_len = (short*)0x5f65D;

    if ( *buffer == 0xAB )
    {
        sprintf ( buffer+*buf_len+5, " %04X%04X", p >> 16, p & 0xFFFF );
        *buf_len += strlen ( buffer+*buf_len+5 );

        if ( p > 0x10000 && p < 0x100000 )
        {
            //at_printf ( "<" );
            sprintf ( buffer+*buf_len+5, "<" );
            *buf_len += 1;
            for ( i = 0; i < 8; i++ )
            {
                sprintf ( buffer+*buf_len+5, " %04X%04X", p >> 16, p & 0xFFFF );
                *buf_len += strlen ( buffer+*buf_len+5 );
                //at_printf ( "%02X%s", *(char*)(p+i), i == 3 ? " " : "" );
            }
            sprintf ( buffer+*buf_len+5, ">", p >> 16, p & 0xFFFF );
            *buf_len += 1;
            //at_printf ( ">" );
        }
        /* //don't include - overloaded :)
        if ( p > 0x1550000 && p < 0x1553000 )
        {
            //p = 0x15505AC + 0x56;
            at_printf ( "<%d,", *(char*)0x23124 );
            //unicode = (short*)get_string ( p, *(char*)0x23124 );
            test = (char*)get_string ( p, *(char*)0x23124 );
            for ( i = 0; test[i+1]; i+=2 )
                at_printf ( "%s", &test[i+1] ); //can't handle %c
            at_printf ( ">" );
        }
        */
    }
    else
    {
#endif
        at_printf ( " %04X%04X", p >> 16, p & 0xFFFF );

        if ( p > 0x10000 && p < 0x100000 )
        {
            at_printf ( "<" );
            for ( i = 0; i < 8; i++ )
                at_printf ( "%02X%s", *(char*)(p+i), i == 3 ? " " : "" );
            at_printf ( ">" );
        }
        if ( p > 0x1550000 && p < 0x1553000 )
        {
            //p = 0x15505AC + 0x56;
            at_printf ( "<%d,", *(char*)0x23124 );
            //unicode = (short*)get_string ( p, *(char*)0x23124 );
            /*test = (char*)get_string ( p, *(char*)0x23124 );
            for ( i = 0; test[i+1]; i+=2 )
                at_printf ( "%s", &test[i+1] ); //can't handle %c*/
            at_draw_string ( (char*)p );
            at_printf ( ">" );
        }
#ifdef BUFFERING
    }
#endif
}

/*
int strncmp ( const char *cmp1, const char *cmp2, int n )
{
    int i = 0;
    while ( i != n )
        if ( cmp1[i] != cmp2[i] )
            return 0;
    return 1;
}


/*
int init( int i)
{
    switch ( i )
    {
        case 1:
        at_printf("init");
        break;
        case 2:
        at_printf("init2");
        break;
        default:
        at_printf("none");
    }
    at_printf ( "%d %d", i+10, i*10 );
    at_printf ( "the loooongest text ever or not ! :)" );
    at_send_success( 0xfe, 0xe );
    return i-1;
};
*/

const char *krisha = "\0k\0r\0i\0s\0h\0a\0";

void dump_objects( int *obj, unsigned short method, int *ptr_stack )
{
    /* ptr_stack:
       +0 r4
       +1 r5
       +2 r6
       +3 r7
       +4 backjump addr after get_methodpointer_of_object/lr
       +5 SP/r3
       +6 r4
       +7 r5
       +8 r6
       +9 r7
       +10 lr
    */
    char *oname, *mname;
    char *buffer = (char*)0x5f65C;
    unsigned short *buf_len = (short*)0x5f65D;
    int i = 0;
    char *unicode;


    /* the terminal object, will cause endless loop */
    if ( *obj == TerminalAdapter || *obj == CommPep )
        return;


        i = 0;
        while ( objectstodump[i].type )
        {
            if ( objectstodump[i].type == this_object && objectstodump[i].object == *obj )
                break;
            if ( objectstodump[i].type == super_object && objectstodump[i].object == *(obj+1) )
                break;
            if ( objectstodump[i].type == object_pointer && objectstodump[i].object == (int)obj )
                break;
            i++;
        }
        if ( objectstodump[i].type == 0 && !scan_args_for_string ( ptr_stack ) )
            return;

    oname = "                  ";

    for ( i = 0; objects[i].addr; i++ )
        if ( *obj == objects[i].addr )
        {
            oname = objects[i].name;
            break;
        }


    mname = "      ";
    for ( i = 0; methods[i].id; i++ )
        if ( method == methods[i].id )
        {
            mname = methods[i].name;
            break;
        }
        at_printf ( "%04X%04X: ", *(ptr_stack+10) >> 16, *(ptr_stack+10) & 0xFFFF );
        at_printf ( "%04X%04X(%s)|%04X(%s)(", *obj >> 16, *obj & 0xFFFF, oname, method, mname );

    //draw params
    switch ( *(ptr_stack+4) )
    {
        case 0x12aa3d1:
            //at_printf ( "0P" );
            break;
        case 0x12aa3b3:
            //param 1 in r6 - stack
            //at_printf ( "1P" );
            draw_param ( *(ptr_stack+2) );
            break;
        case 0x12AA391:
            //in r7 & r6
            //at_printf ( "2P" );
            draw_param ( *(ptr_stack+3) );
            draw_param ( *(ptr_stack+2) );
            break;
        case 0x12aa36d:
            //in r7, r6, sp
            //at_printf ( "3P" );
            draw_param ( *(ptr_stack+3) );
            draw_param ( *(ptr_stack+2) );
            draw_param ( *(ptr_stack+5) );
            break;
        case 0x12AA3F7:
            //at_printf ( "s4P %04X%04X %04X%04X %04X%04X %04X%04X", *(ptr_stack+3) >> 16, *(ptr_stack+3) & 0xFFFF, *(ptr_stack+2) >> 16, *(ptr_stack+2) & 0xFFFF, *(ptr_stack+5) >> 16, *(ptr_stack+5) & 0xFFFF, *(ptr_stack+1) >> 16, *(ptr_stack+1) & 0xFFFF );
            //at_printf ( " it's super" );
            //it's the super object! it's called from somewhere/child requesting the super object method!
            draw_param ( *(ptr_stack+2) );
            draw_param ( *(ptr_stack+3) );
            draw_param ( *(ptr_stack+5) );
            draw_param ( *(ptr_stack+1) );
            break;
        default:
                at_printf ( "?unknown?" );
    }

        at_printf ( ")\n" );
        //at_send_success_forwarder ( 0xfe, 0xe );
        //at_send_success
        bx_r2 ( 0xfe, 0xe, 0x1061220+1 ); //check if funktion pointer works!

    return;

}


