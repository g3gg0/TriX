#ifndef _LCD_C_
#define _LCD_C_


#include "fonts.h"

extern struct FONT_DEF fonts[];

//#define FIXED_WIDTH 6


#include "hw_defines.h"
#include "base.h"
#include "lcd.h"


u32
lcd_init ()
{
	u32 pos = 0;
	static const u16 lcd_inittab[] = {
		0x0011,
		0x00DA,
		0x00AF,
		0x00CA, 0x800C, 0x8020, 0x800C,
		0x00BB, 0x8001,
		0x0094,
		0x00D1,
		0x0081, 0x801F, 0x8003,
		0x0082, 0x8001, 0x8000, 0x8000, 0x8000,	0x8000, 0x8001, 0x8015, 0x8017, 0x8022,	0x8024,	0x8026,	0x8027,	0x8028,	0x8000,
		0x0020, 0x8008,
		0x00A9,
		0x00AA, 0x8002, 0x8022, 0x8022, 0x8003,
		0x00AB, 0x8000,
		0x00BC, 0x8000, 0x8000, 0x8002,
		0x0075, 0x8000, 0x8082,
		0x0015, 0x8000, 0x8082,
		0x0020, 0x8008,
		0x0020, 0x800F,
		0
	};

	while ( lcd_inittab[pos] )
	{
		if ( lcd_inittab[pos] & 0x8000 )
			spi_send_data ( lcd_inittab[pos]&0xFF );
		else
			spi_send_cmd ( lcd_inittab[pos]&0xFF );
		pos++;
	}

	return 1;
}

u32
lcd_illumination ( u32 enabled )
{
	int uem_type = 0;
	
	if ( enabled )
	{
		if ( uem_type == 0 ) // standard
		{
			uem_write ( 0x0F, 0x00, 8 ); // set LCD PWM duty cycle
			uem_write ( 0x10, 0xFF, 8 ); // enable LCD PWM
		}
		else if ( uem_type == 1 ) // RH-87
		{
			int dcr = 0x1f;
			uem_write ( 0x13, 0x80 | 0x40 | (dcr & 0x3F), 8 ); // 0x80 -> inverted PWM
			uem_write ( 0x18, 0x4080, 16 ); // enable LEDs
		}
	}
	else
	{
		uem_write ( 0x0F, 0x00, 8 ); // set LCD PWM duty cycle
		uem_write ( 0x10, 0x00, 8 ); // disable LCD PWM
	}
	
	return 1;
}


u32
lcd_setup ()
{
	// set up SPI ?
	strh ( CTSI_BASE + 0x0034, 0x0003 ); // (CTSI+0x0034 LCD/Audio)
	strh ( UIF_BASE + 0x002E, 0x0009 ); // (UIF+0x002E Graph Interface Setup)
	strh ( UIF_BASE + 0x002C, 0x1000 ); // (UIF+0x002C )
	
	// reset LCD module (RH-87)
	strh ( PUP_BASE + 0x0010, 0xFFFF ); // (PUP+0x0010 IO B Direction)
	strh ( PUP_BASE + 0x0008, 0x0400 ); // (PUP+0x0008 IO B Output Reset)
	usleep ( 50 msec );
	strh ( PUP_BASE + 0x0006, 0x0400 ); // (PUP+0x0006 IO B Output Set)
	
	// reset LCD module (NHL-4U)
	strh ( PUP_BASE + 0x000E, 0xFFFF ); // (PUP+0x000E IO A Direction)
	strh ( PUP_BASE + 0x0004, 0x0010 ); // (PUP+0x0004 IO A Output Reset)
	usleep ( 50 msec );
	strh ( PUP_BASE + 0x0002, 0x0010 ); // (PUP+0x0002 IO A Output Set)

	// init lcd module
	lcd_init ();

	//lcd_pcf8833_init ();
	//lcd_pcf8833_clear ();
	
	// enable LCD illumination
	lcd_illumination ( 1 );
	
	
	return 1;
}


u32
lcd_font_height ( u32 font )
{
	return fonts[font].glyph_height;
}

u32
lcd_put_char ( u32 x_offset, u32 y_offset, u32 font, u8 character, u32 color )
{
	u32 ypos = 0;
	u32 xpos = 0;
	u32 table_glyph = 0;
	u8 value = 0;
	u32 pixels = 0;
	u32 combo = 0;
	u32 width = 0;


	if ( character < fonts[font].glyph_beg || character > fonts[font].glyph_end )
		character = fonts[font].glyph_def;

	character -= fonts[font].glyph_beg;

	x_offset &= 0xFFFFFFFE;

	if ( fonts[font].fixed_width )
		width = fonts[font].fixed_width;
	else
		width = fonts[font].width_table[character];

#ifdef FIXED_WIDTH
	width = FIXED_WIDTH;
#endif

	table_glyph = fonts[font].store_width * fonts[font].glyph_height * character;
	while ( ypos < fonts[font].glyph_height )
	{
		xpos = 0;
		while ( xpos < width )
		{
			value = fonts[font].glyph_table[table_glyph + fonts[font].store_width * ypos + xpos/8];

			pixels = (value>>( 7 - (xpos&0x07) )) & 3;

			combo = 0;
			if ( pixels & 1 )
				combo |= color;
			if ( pixels & 2 )
				combo |= (color<<0x0C);

			lcd_set_pixel ( x_offset+xpos, y_offset+ypos, combo );

			xpos += 2;
		}
		ypos++;
	}

	return width+1;
}

u32
lcd_write ( u32 x, u32 y, u8 *text, u32 color, u32 font )
{
	u32 pos = 0;

	while ( text[pos] )
	{
#ifndef FIXED_WIDTH
		x += lcd_put_char ( x, y, font, text[pos], color );
#else
		lcd_put_char ( x, y, font, text[pos], color );
		x += FIXED_WIDTH;
#endif
		pos++;
	}
	return x;
}

void
lcd_draw_rect ( u32 x1, u32 x2, u32 y1, u32 y2, u32 value )
{
    u32 i;
    while ( y1 < y2 )
    {
        i = x1;
        while ( i < x2 )
        {
            lcd_set_pixel ( i, y1, value | value<<0x0C );
            i += 2;
        }
        y1++;

    }

}


u32
lcd_set_pixel ( u32 x, u32 y, u32 value )
{
	spi_send_cmd ( 0x15 );
		spi_send_data ( x );
		spi_send_data ( x+1 );
	spi_send_cmd ( 0x75 );
		spi_send_data ( y+1 );
		spi_send_data ( y+2 );


	spi_send_cmd ( 0x5C );
	spi_send_data ( (value>>0x10)&0xFF );
	spi_send_data ( (value>>0x08)&0xFF );
	spi_send_data ( (value>>0x00)&0xFF );
	
	return 1;
}


u32
lcd_clear ()
{
	u32 x = 0;
	u32 y = 0;
	
	spi_send_cmd ( 0x15 );
		spi_send_data ( x );
		spi_send_data ( x+1 );
	spi_send_cmd ( 0x75 );
		spi_send_data ( y+1 );
		spi_send_data ( y+2 );

	while ( y < 132 )
	{
		x = 0;
		while ( x < 132 )
		{
			lcd_set_pixel ( x, y, 0x000000 );
			x += 2;
		}
		y++;
	}

	return 1;
}

#endif
