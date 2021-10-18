#ifndef _LCD_H_
#define _LCD_H_

#define COLOR(r,g,b) ((~r&0xFF)<<16|(~g&0xFF)<<8|(~b&0xFF))

void usleep ( u32 us );
u32  lcd_font_height ( u32 font );
u32  lcd_put_char ( u32 x_offset, u32 y_offset, u32 font, u8 character, u32 color );
u32  lcd_write ( u32 x, u32 y, u8 *text, u32 color, u32 font );
u32  lcd_set_pixel ( u32 x, u32 y, u32 value );
void lcd_draw_rect ( u32 x1, u32 x2, u32 y1, u32 y2, u32 value );
u32  lcd_clear ();

u32 lcd_illumination ( u32 enabled );
u32 lcd_setup ();

#endif
