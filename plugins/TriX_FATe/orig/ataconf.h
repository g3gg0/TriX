//*****************************************************************************
//
// Title		: IDE-ATA interface driver for hard disks
// Authors		: Angelo Bannack, Giordano Bruno Wolaniuk
// File Name	: 'ataconf.h'
// Date			: April 26, 2004
// Version		: 1.00
// Target MCU	: Atmel AVR Series
// Editor Tabs	: 4
//
// NOTE: This code is based on original library "IDE-ATA interface driver for hard disks"
//       from Pascal Stang.
//
//	     The authors in no way will be responsible for damages that you coul'd be using this code.
//       Use this code at your own risk.
//
//       This code is distributed under the GNU Public License
//       which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#ifndef ATA_IFCONF_H
#define ATA_IFCONF_H


//#define ATA_READ_ONLY // only uncomment this line if you don't need to use read routines


// constants
unsigned char SECTOR_BUFFER_ADDR[512];


//*****************************************************************************
//*****************************************************************************
//*****************************************************************************
// If you change the hardware, you will have to change this definitions.
// from here ...
#define DDR_DATAL		DDRC		// Define the Direction PORT to DATA High
#define DDR_DATAH		DDRB		// Define the Direction PORT to DATA Low
#define PORT_DATAL		PORTC		// Define the PORT to DATA High
#define PORT_DATAH		PORTB		// Define the PORT to DATA Low
#define PIN_DATAL		PINC		// Define the PIN to DATA High
#define PIN_DATAH		PINB		// Define the PIN to DATA Low

#define PORT_ADDR		PORTA		// Define the PORT to Address
#define DDR_ADDR		DDRA		// Define the PORT to Address

#define PORT_IDE_RD		PORTA		// Define the PORT to IDE_RD
#define PIN_IDE_RD 		7			// Define the PIN number to IDE_RD

#define PORT_IDE_WR		PORTA		// Define the PORT to IDE_WR
#define PIN_IDE_WR 		6			// Define the PIN number to IDE_WR
// ... until here
//*****************************************************************************
//*****************************************************************************
//*****************************************************************************

// ATA register offset
#define ATA_REG_DATA		0x08
#define ATA_REG_ERROR		0x09
#define ATA_REG_SECCOUNT	0x0a
#define ATA_REG_STARTSEC	0x0b
#define ATA_REG_CYLLO		0x0c
#define ATA_REG_CYLHI		0x0d
#define ATA_REG_HDDEVSEL	0x0e
#define ATA_REG_CMDSTATUS1	0x0f
#define ATA_REG_ACTSTATUS	0x16


#endif


