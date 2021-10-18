#define SWAP_HALF(value) (((value>>8) | (value<<8)) & 0xFFFF)


#define FAID_BASE         (0x0000012C)
#define CBUSC_BASE        (0x0C305300)
#define STI_BASE          (0x0C305400)
#define CTSI_BASE         (0x0C308000)
#define PUP_BASE          (0x0C308200)
#define UIF_BASE          (0x0C308400)


#define CBUS_CLK_SET      (CTSI_BASE + 0x0034)
#define CBUS_CLK_RESET    (CTSI_BASE + 0x0036)

#define CBUS_BITCOUNT     (UIF_BASE + 0x0020)
#define CBUS_CTRL         (UIF_BASE + 0x0022)
#define CBUS_DATA_HIGH    (UIF_BASE + 0x0024)
#define CBUS_DATA_LOW     (UIF_BASE + 0x0026)
#define CBUS_CLK_SPEED    (UIF_BASE + 0x0030)

#define CBUS_CTRL_READY   (0x0400)
#define CBUS_CTRL_WRITE   (0x0200)
#define CBUS_CTRL_READ    (0x0100)
#define CBUS_BITS_WRITE   (0xE0)
#define CBUS_INIT         (0x40)

#define CLK_SPEED         (0x66)

#define CBUSC_INIT        (CBUSC_BASE + 0x0000)
#define CBUSC_FAID        (CBUSC_BASE + 0x001A)
#define CBUSC_BYTE        (CBUSC_BASE + 0x0036)

#define UEM_ADDRESS       (1)


typedef unsigned long u32;
typedef unsigned short u16;
typedef unsigned char u8;


u16* buffer = (u16*)0x0A002000;
u32 buffer_pos = 0;



static inline void
set_stack ( u32 addr )
{
    asm volatile ( "mov sp, %0"::"r" ( addr ) );
}

static inline u32
get_stack (  )
{
   u32 addr = 0;
   
    asm volatile ( "mov %0, sp":"=r" ( addr ) );
   return addr;
}

static inline u32
ldr ( u32 addr )
{
   u32 val = 0;
   
    asm volatile ( "ldr  %0, [%1,#0x00]" : "=r" ( val ) : "r" ( addr ) );
    return val;
}

static inline u32
ldrh ( u32 addr )
{
   u32 val = 0;
   
    asm volatile ( "ldrh  %0, [%1,#0x00]" : "=r" ( val ) : "r" ( addr ) );
    return val;
}

static inline u32
ldrh_rel ( u32 addr, u32 offs )
{
   u32 val = 0;
   
    asm volatile ( "ldrh  %0, [%1,%2]" : "=r" ( val ) : "r" ( addr ), "r" ( offs ) );
    return val;
}

static inline u32
ldrb ( u32 addr )
{
   u32 val = 0;
   
    asm volatile ( "ldrb  %0, [%1,#0x00]" : "=r" ( val ) : "r" ( addr ) );
    return val;
}

static inline void
str ( u32 addr, u32 val )
{
    asm volatile ( "str  %0, [%1,#0x00]"::"r" ( val ), "r" ( addr ) );
    return;
}

static inline void
strh_rel ( u32 addr, u32 offs, u32 val )
{
    asm volatile ( "strh  %0, [%1,%2]"::"r" ( val ), "r" ( addr ), "r" ( offs ) );
    return;
}

static inline void
strh ( u32 addr, u32 val )
{
    asm volatile ( "strh  %0, [%1,#0x00]"::"r" ( val ), "r" ( addr ) );
    return;
}

static inline void
strb ( u32 addr, u32 val )
{
    asm volatile ( "strb  %0, [%1,#0x00]"::"r" ( val ), "r" ( addr ) );
    return;
}


// set up unknown STI registers
static void
sti_init ( )
{
   strh_rel ( STI_BASE, 0, 5 );
   strh_rel ( STI_BASE, 4, 0x0200 );
   strh_rel ( STI_BASE, 2, 0x0100 );
}

// set the CBUS clock speed
static void
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
}

// initialize CBUS clock and state
static void
cbus_init ()
{
   cbus_clock ( 0 );
   strh ( CBUS_CLK_SPEED, CLK_SPEED );
   strh ( CBUS_CLK_SET, CBUS_INIT );
   cbus_clock ( 1 );
}

// unlock CBUS by copying FAID into CBUSC registers
static void
cbus_unlock ()
{
   int pos = 0;
   
   strh ( CBUSC_INIT, 0x1F00 );
   strh ( CBUSC_BYTE, ldrb ( FAID_BASE + 12 ) );
   
   for ( pos = 0; pos < 12; pos+=2 )
   {
      u16 value = ldrh_rel ( FAID_BASE, pos );
      
      strh_rel ( CBUSC_FAID, pos, value );
   }
   
   strh ( CBUSC_INIT, 0x000B );
}

// wait until CBUS is ready
static u32
cbus_wait ()
{
   u32 timeout = 0xF000;
   
   while ( !(ldrh ( CBUS_CTRL ) & CBUS_CTRL_READY) && (timeout-- > 0) );
   
   if ( !timeout )
   {
      buffer[0] = 0xEEEE;
      return 0;
   }
   
   return 1;
}

// read register [reg] from device [dev]
static u32
cbus_read ( u32 dev, u32 reg, u32 bits )
{
   cbus_wait ();
   strh ( CBUS_BITCOUNT, (bits & 0x1F ) );
   strh ( CBUS_CTRL, CBUS_CTRL_READ | ((dev & 0x07) << 5) | (reg & 0x1F) );
   cbus_wait ();
   
   return (ldrh ( CBUS_DATA_HIGH ) << 16) | ldrh ( CBUS_DATA_LOW );
}

// write [val] into register [reg] of device [dev]
static u32
cbus_write ( u32 dev, u32 reg, u32 val, u32 bits )
{
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
static u32
uem_read ( u32 reg, u32 bits )
{
   return cbus_read ( UEM_ADDRESS, reg, bits );
}

// write [val] into register [reg] to UEM
static u32
uem_write ( u32 reg, u32 val, u32 bits )
{
   return cbus_write ( UEM_ADDRESS, reg, val, bits );
}

static u32
uem_init ( )
{
   uem_write ( 0x12, 0x20, 0x07 );
   uem_write ( 0x19, 0x80, 0x0F );
   uem_write ( 0x0B, 0x04, 0x0F );
   
   return 1;
}


// dump UEM registers
int dump_uem ()
{
   int pos = 0;
   
   // to make sure that this routine was run
   buffer[buffer_pos++] = 0x5555;

   while ( pos < 0x20 )
      buffer[buffer_pos++] = uem_read ( pos++, 0x0F );
   
   // to make sure that this routine was run
   buffer[buffer_pos++] = 0xAAAA;

   return 0;
}

// initialize CBUS device 0x06 - no idea whats that
int init_other ()
{
   int pos = 0;
   u32 value = 0;
   
   // enable device 0x06 ?
   strh ( PUP_BASE + 0x001C, 0x0200 );

   // read, but discard
   value = cbus_read ( 6, 0x02, 0x12 );
   value = 0x0007FFFF;
   cbus_write ( 6, 0x02, value, 0x12 );

   // read, but discard
   value = cbus_read ( 6, 0x08, 0x0F );
   value = 0x9AFD;
   cbus_write ( 6, 0x08, value, 0x0F );
   
}

int main (  )
{

   // use our own stack to make sure we have enough of it
   u32 old_stack = get_stack ();

   set_stack ( 0x0A001000 );

   sti_init ();
   cbus_unlock ();
   cbus_init ();
   uem_init ();
   init_other ();
   
   uem_write ( 0x0B, 0x0004, 0x0F );
   
   // disable WDTs (if not already done)
   strh ( CTSI_BASE+0x0006, 0xFFFF );
   uem_write ( 0x12, 0x3F, 0x07 );

   
   dump_uem ();
   
   // restore old stack pointer
   set_stack ( old_stack );

} 