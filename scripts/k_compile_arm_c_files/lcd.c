
#define msec *1000
#define sec *1000 msec
#define COLOR(r,g,b) ((~r&0x0F)<<8|(~g&0x0F)<<4|(~b&0x0F))

#include "fonts.h"

extern struct FONT_DEF fonts[];

#define FIXED_WIDTH 6

void usleep ( int us )
{
	int val = 0;

	us &= 0xFFFFFFFC;
	while ( us )
	{
		us-=4;
		val = 22;
		while ( val )
		{
			asm ( "nop" );
			val--;
		}
	}
}

int  lcd_font_height ( int font )
{
	return fonts[font].glyph_height;
}
int  lcd_put_char ( int x_offset, int y_offset, int font, unsigned char character, int color )
{
	int ypos = 0;
	int xpos = 0;
	int table_glyph = 0;
	unsigned char value = 0;
	int pixels = 0;
	int combo = 0;
	int width = 0;


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

int  lcd_write ( int x, int y, unsigned char *text, int color, int font )
{
	int pos = 0;

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

int  lcd_set_pixel ( int x, int y, unsigned int value )
{
	spi_send_cmd ( 0x15 );
		spi_send_data ( x, 2 );
		spi_send_data ( x+1, 2 );
	spi_send_cmd ( 0x75 );
		spi_send_data ( y+1, 2 );
		spi_send_data ( y+2, 2 );


	spi_send_cmd ( 0x5C );
	spi_send_data ( (value>>0x10)&0xFF, 2 );
	spi_send_data ( (value>>0x08)&0xFF, 2 );
	spi_send_data ( (value>>0x00)&0xFF, 2 );
}

void lcd_draw_rect ( int x1, int x2, int y1, int y2, unsigned int value )
{
    int i;
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
void lcd_init ( )
{

	int pos = 0;
	static const unsigned short lcd_inittab[] = {
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
		0
	};

	while ( lcd_inittab[pos] )
	{
		if ( lcd_inittab[pos] & 0x8000 )
			spi_send_data ( lcd_inittab[pos]&0xFF, 2 );
		else
			spi_send_cmd ( lcd_inittab[pos]&0xFF );
		pos++;
	}

	spi_send_cmd ( 0x20 );
		spi_send_data ( 0x08, 2 );
	usleep ( 50 msec );

	spi_send_cmd ( 0x20 );
		spi_send_data ( 0x0F, 2 );
	usleep ( 50 msec );

	lcd_clear ();

}

int  lcd_clear ()
{
	int x = 0;
	int y = 0;
	spi_send_cmd ( 0x15 );
		spi_send_data ( x, 2 );
		spi_send_data ( x+1, 2 );
	spi_send_cmd ( 0x75 );
		spi_send_data ( y+1, 2 );
		spi_send_data ( y+2, 2 );

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

}

