#ifndef _PCF8833_C_
#define _PCF8833_C_


#define msec *1000
#define sec *1000 msec

#include "pcf8833.h"

//  ***************************************************************************** 
//          InitLcd.c 
//  
//     Initializes the Philips PCF8833 LCD Controller 
//      
//     Inputs:  none 
// 
//    Author:  James P Lynch     July 7, 2007 
//  *****************************************************************************  
void 
lcd_pcf8833_init (void) 
{ 
	// Sleep out  (command 0x11) 
	spi_send_cmd(SLEEPOUT); 

	// Inversion on  (command 0x20) 
	spi_send_cmd(INVON);    // seems to be required for this controller 

	// Color Interface Pixel Format  (command 0x3A) 
	spi_send_cmd(COLMOD); 
	spi_send_data(0x03);      // 0x03 = 12 bits-per-pixel 

	// Memory access controler  (command 0x36) 
	spi_send_cmd(MADCTL); 
	spi_send_data(0xC8);      // 0xC0 = mirror x and y, reverse rgb 

	// Write contrast  (command 0x25) 
	spi_send_cmd(SETCON); 
	spi_send_data(0x30);      // contrast 0x30  
	
	usleep(20 msec); 

	// Display On  (command 0x29) 
	spi_send_cmd(DISPON); 
}

//  ***************************************************************************** 
//          LCDClearScreen.c 
//  
//     Clears the LCD screen to single color (WHITE) 
//      
//     Inputs:  none 
// 
//    Author:  James P Lynch     July 7, 2007 
//  *****************************************************************************  
void 
lcd_pcf8833_clear (void) 
{ 

	long  i;    // loop counter 

	// Row address set  (command 0x2B) 
	spi_send_cmd(PASET); 
	spi_send_data(0); 
	spi_send_data(131); 

	// Column address set  (command 0x2A) 
	spi_send_cmd(CASET); 
	spi_send_data(0); 
	spi_send_data(131); 

	// set the display memory to BLACK 
	spi_send_cmd(RAMWR); 
	for (i = 0; i < ((131 * 131) / 2); i++) { 
		spi_send_data((WHITE >> 4) & 0xFF); 
		spi_send_data(((WHITE & 0xF) << 4) | ((WHITE >> 8) & 0xF)); 
		spi_send_data(WHITE & 0xFF);  } 
}



//  ***************************************************************************** 
//          LCDSetXY.c 
//  
//     Sets the Row and Column addresses 
//      
//     Inputs:    x  =   row address (0 .. 131) 
//        y  =   column address  (0 .. 131) 
// 
// 
//     Returns:   nothing   
// 
//    Author:  James P Lynch     July 7, 2007 
//  *****************************************************************************  
void 
lcd_pcf8833_set_xy (int x, int y) 
{ 
	// Row address set  (command 0x2B) 
	spi_send_cmd(PASET); 
	spi_send_data(x); 
	spi_send_data(x); 

	// Column address set  (command 0x2A) 
	spi_send_cmd(CASET); 
	spi_send_data(y); 
	spi_send_data(y); 
}



//  ************************************************************************************* 
//          lcd_pcf8833_set_xy.c 
//  
//  Lights a single pixel in the specified color at the specified x and y addresses 
//      
// Inputs:    x     =   row address (0 .. 131) 
//      y     =   column address  (0 .. 131) 
//      color =   12-bit color value  rrrrggggbbbb 
//     rrrr = 1111 full red 
//          : 
//      0000 red is off 
// 
//     gggg = 1111 full green 
//          : 
//      0000 green is off 
// 
//     bbbb = 1111 full blue 
//          : 
//      0000 blue is off 
// 
//     Returns:   nothing  
// 
//    Note: see lcd.h for some sample color settings  
// 
//    Author:  James P Lynch     July 7, 2007 
//  *************************************************************************************  
void 
lcd_pcf8833_set_pixel (int x, int y, int color) 
{ 
	lcd_pcf8833_set_xy(x, y); 
	spi_send_cmd(RAMWR); 
	spi_send_data((unsigned char)((color >> 4) & 0xFFFF)); 
	spi_send_data((unsigned char)(((color & 0x0F) << 4) | 0x00)); 
	spi_send_cmd(NOP); 
} 

//  ************************************************************************************************* 
//          lcd_pcf8833_draw_line.c 
//  
//  Draws a line in the specified color from (x0,y0) to (x1,y1)  
//      
// Inputs:    x     =   row address (0 .. 131) 
//      y     =   column address  (0 .. 131) 
//      color =   12-bit color value  rrrrggggbbbb 
//     rrrr = 1111 full red 
//          : 
//      0000 red is off 
// 
//     gggg = 1111 full green 
//          : 
//      0000 green is off 
// 
//     bbbb = 1111 full blue 
//          : 
//      0000 blue is off 
// 
//  Returns:   nothing  
// 
//  Note:  good write-up on this algorithm in Wikipedia (search for Bresenham's line algorithm) 
//    see lcd.h for some sample color settings   
// 
//  Authors:   Dr. Leonard McMillan, Associate Professor UNC 
//      Jack Bresenham IBM, Winthrop University (Father of this algorithm, 1962) 
// 
//      Note: taken verbatim from Professor McMillan's presentation:  
//            http://www.cs.unc.edu/~mcmillan/comp136/Lecture6/Lines.html 
// 
//  *************************************************************************************************  
void
lcd_pcf8833_draw_line(int x0, int y0, int x1, int y1, int color) { 

	int dy = y1 - y0; 
	int dx = x1 - x0; 
	int stepx = 1;
	int stepy = 1;


	if (dy < 0) 
	{ 
		dy = -dy;  
		stepy = -1; 
	} 
	
	if (dx < 0) 
	{ 
		dx = -dx;  
		stepx = -1; 
	} 
	
	dy <<= 1;        // dy is now 2*dy 
	dx <<= 1;        // dx is now 2*dx 

	lcd_pcf8833_set_pixel (x0, y0, color); 

	if (dx > dy) 
	{ 
		int fraction = dy - (dx >> 1);  // same as 2*dy - dx 
		while (x0 != x1) 
		{ 
			if (fraction >= 0) 
			{ 
				y0 += stepy; 
				fraction -= dx;    // same as fraction -= 2*dx 
			} 
			x0 += stepx; 
			fraction += dy;    // same as fraction -= 2*dy 
			lcd_pcf8833_set_pixel(x0, y0, color); 
		} 
	} 
	else 
	{ 
		int fraction = dx - (dy >> 1); 
		while (y0 != y1) 
		{ 
			if (fraction >= 0) 
			{ 
				x0 += stepx; 
				fraction -= dy; 
			} 
			y0 += stepy; 
			fraction += dx; 
			lcd_pcf8833_set_pixel(x0, y0, color); 
		} 
	} 
} 

//  ***************************************************************************************** 
//          lcd_pcf8833_draw_rect.c 
//  
//  Draws a rectangle in the specified color from (x1,y1) to (x2,y2) 
//  Rectangle can be filled with a color if desired  
//      
// Inputs:    x     =   row address (0 .. 131) 
//      y     =   column address  (0 .. 131) 
//      fill  =   0=no fill, 1-fill entire rectangle  
//      color =   12-bit color value for lines  rrrrggggbbbb 
//     rrrr = 1111 full red 
//      : 
//     0000 red is off 
// 
//     gggg = 1111 full green 
//      : 
//     0000 green is off 
// 
//     bbbb = 1111 full blue 
//      : 
//     0000 blue is off 
//   Returns:   nothing  
// 
//   Notes: 
// 
//    The best way to fill a rectangle is to take advantage of the "wrap-around" featute 
//    built into the Philips PCF8833 controller. By defining a drawing box, the memory can 
//    be simply filled by successive memory writes until all pixels have been illuminated. 
// 
//      1.  Given the coordinates of two opposing corners (x0, y0) (x1, y1) 
//          calculate the minimums and maximums of the coordinates 
// 
//        xmin = (x0 <= x1) ? x0 : x1; 
//        xmax = (x0 > x1) ? x0 : x1; 
//        ymin = (y0 <= y1) ? y0 : y1; 
//        ymax = (y0 > y1) ? y0 : y1; 
// 
//      2. Now set up the drawing box to be the desired rectangle 
// 
//     spi_send_cmd(PASET);   // set the row boundaries 
//     spi_send_data(xmin); 
//     spi_send_data(xmax); 
//     spi_send_cmd(CASET);   // set the column boundaries 
//     spi_send_data(ymin); 
//     spi_send_data(ymax); 
// 
//      3. Calculate the number of pixels to be written divided by 2 
// 
//        NumPixels = ((((xmax - xmin + 1) * (ymax - ymin + 1)) / 2) + 1) 
// 
//        You may notice that I added one pixel to the formula.  
//        This covers the case where the number of pixels is odd and we  
//        would lose one pixel due to rounding error. In the case of 
//        odd pixels, the number of pixels is exact.  
//        in the case of even pixels, we have one more pixel than 
//        needed, but it cannot be displayed because it is outside 
//     the drawing box. 
// 
//        We divide by 2 because two pixels are represented by three bytes. 
//        So we work through the rectangle two pixels at a time. 
// 
//      4.  Now a simple memory write loop will fill the rectangle 
// 
//        for (i = 0; i < ((((xmax - xmin + 1) * (ymax - ymin + 1)) / 2) + 1); i++) { 
//      spi_send_data((color >> 4) & 0xFF); 
//          spi_send_data(((color & 0xF) << 4) | ((color >> 8) & 0xF)); 
//      spi_send_data(color & 0xFF); 
//     } 
// 
//    In the case of an unfilled rectangle, drawing four lines with the Bresenham line 
//    drawing algorithm is reasonably efficient. 
// 
//    Author:  James P Lynch      July 7, 2007 
//  *****************************************************************************************  

void
lcd_pcf8833_draw_rect ( int x0, int y0, int x1, int y1, unsigned char fill, int color) { 
	int   xmin, xmax, ymin, ymax; 
	int   i; 

	// check if the rectangle is to be filled 
	if (fill == FILL) 
	{ 

		// best way to create a filled rectangle is to define a drawing box 
		// and loop two pixels at a time 

		// calculate the min and max for x and y directions 
		xmin = (x0 <= x1) ? x0 : x1; 
		xmax = (x0 > x1) ? x0 : x1; 
		ymin = (y0 <= y1) ? y0 : y1; 
		ymax = (y0 > y1) ? y0 : y1; 

		// specify the controller drawing box according to those limits 
		// Row address set  (command 0x2B) 
		spi_send_cmd(PASET); 
		spi_send_data(xmin); 
		spi_send_data(xmax); 
		
		// Column address set  (command 0x2A) 
		spi_send_cmd(CASET); 
		spi_send_data(ymin); 
		spi_send_data(ymax); 

		// WRITE MEMORY 
		spi_send_cmd(RAMWR); 

		// loop on total number of pixels / 2 
		for (i = 0; i < ((((xmax - xmin + 1) * (ymax - ymin + 1)) / 2) + 1); i++)
		{
			// use the color value to output three data bytes covering two pixels 
			spi_send_data((color >> 4) & 0xFF); 
			spi_send_data(((color & 0xF) << 4) | ((color >> 8) & 0xF)); 
			spi_send_data(color & 0xFF); 
		}
	} 
	else 
	{ 
		// best way to draw un unfilled rectangle is to draw four lines 
		lcd_pcf8833_draw_line(x0, y0, x1, y0, color); 
		lcd_pcf8833_draw_line(x0, y1, x1, y1, color); 
		lcd_pcf8833_draw_line(x0, y0, x0, y1, color); 
		lcd_pcf8833_draw_line(x1, y0, x1, y1, color); 
	} 
}

#endif