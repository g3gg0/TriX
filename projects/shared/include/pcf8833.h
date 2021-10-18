#ifndef _PCF8833_H_
#define _PCF8833_H_

//  ************************************************************************************* 
//      LCD Include File for Philips PCF8833 STN RGB- 132x132x3 Driver 
// 
//        Taken from Philips data sheet Feb 14, 2003 
//  ************************************************************************************* 
 
// Philips PCF8833 LCD controller command codes 
#define NOP        0x00  // nop 
#define SWRESET    0x01  // software reset 
#define BSTROFF    0x02  // booster voltage OFF 
#define BSTRON     0x03  // booster voltage ON 
#define RDDIDIF    0x04  // read display identification 
#define RDDST      0x09  // read display status 
#define SLEEPIN    0x10  // sleep in 
#define SLEEPOUT   0x11  // sleep out 
#define PTLON      0x12  // partial display mode 
#define NORON      0x13  // display normal mode 
#define INVOFF     0x20  // inversion OFF 
#define INVON      0x21  // inversion ON 
#define DALO       0x22  // all pixel OFF 
#define DAL        0x23  // all pixel ON 
#define SETCON     0x25  // write contrast 
#define DISPOFF    0x28  // display OFF 
#define DISPON     0x29  // display ON 
#define CASET      0x2A  // column address set 
#define PASET      0x2B  // page address set 
#define RAMWR      0x2C  // memory write 
#define RGBSET     0x2D  // colour set 
#define PTLAR      0x30  // partial area 
#define VSCRDEF    0x33  // vertical scrolling definition 
#define TEOFF      0x34  // test mode 
#define TEON       0x35  // test mode 
#define MADCTL     0x36  // memory access control 
#define SEP        0x37  // vertical scrolling start address 
#define IDMOFF     0x38  // idle mode OFF 
#define IDMON      0x39  // idle mode ON 
#define COLMOD     0x3A  // interface pixel format 
#define SETVOP     0xB0  // set Vop   
#define BRS        0xB4  // bottom row swap 
#define TRS        0xB6  // top row swap 
#define DISCTR     0xB9  // display control 
#define DOR        0xBA  // data order 
#define TCDFE      0xBD  // enable/disable DF temperature compensation 
#define TCVOPE     0xBF  // enable/disable Vop temp comp 
#define EC         0xC0  // internal or external  oscillator 
#define SETMUL     0xC2  // set multiplication factor 
#define TCVOPAB    0xC3  // set TCVOP slopes A and B 
#define TCVOPCD    0xC4  // set TCVOP slopes c and d 
#define TCDF       0xC5  // set divider frequency 
#define DF8COLOR   0xC6  // set divider frequency 8-color mode 
#define SETBS      0xC7  // set bias system 
#define RDTEMP     0xC8  // temperature read back 
#define NLI        0xC9  // n-line inversion 
#define RDID1      0xDA  // read ID1 
#define RDID2      0xDB  // read ID2 
#define RDID3      0xDC  // read ID3 
 
 
// backlight control   
#define BKLGHT_LCD_ON  1 
#define BKLGHT_LCD_OFF  2 
 
// Booleans 
#define NOFILL 0 
#define FILL    1 
 
// 12-bit color definitions 
#define WHITE    0xFFF 
#define BLACK    0x000 
#define RED      0xF00 
#define GREEN    0x0F0 
#define BLUE     0x00F 
#define CYAN     0x0FF 
#define MAGENTA  0xF0F 
#define YELLOW   0xFF0 
#define BROWN    0xB22 
#define ORANGE   0xFA0 
#define PINK     0xF6A    
 
// Font sizes 
#define SMALL  0 
#define MEDIUM 1 
#define LARGE  2 

// mask definitions 
#define   BIT0        0x00000001 
#define   BIT1        0x00000002 
#define   BIT2        0x00000004 
#define   BIT3        0x00000008 
#define   BIT4        0x00000010 
#define   BIT5        0x00000020 
#define   BIT6        0x00000040 
#define   BIT7        0x00000080 
#define   BIT8        0x00000100 
#define   BIT9        0x00000200 
#define   BIT10       0x00000400 
#define   BIT11       0x00000800 
#define   BIT12       0x00001000 
#define   BIT13       0x00002000 
#define   BIT14       0x00004000 
#define   BIT15       0x00008000 
#define   BIT16       0x00010000 
#define   BIT17       0x00020000 
#define   BIT18       0x00040000 
#define   BIT19       0x00080000 
#define   BIT20       0x00100000 
#define   BIT21       0x00200000 
#define   BIT22       0x00400000 
#define   BIT23       0x00800000 
#define   BIT24       0x01000000 
#define   BIT25       0x02000000 
#define   BIT26       0x04000000 
#define   BIT27       0x08000000 
#define   BIT28       0x10000000 
#define   BIT29       0x20000000 
#define   BIT30       0x40000000 
#define   BIT31       0x80000000 

#endif