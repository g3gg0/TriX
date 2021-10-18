//*****************************************************************************
//
// Title		: FAT16/32 file system library configuration
// Authors		: Angelo Bannack, Giordano Bruno Wolaniuk
// File Name	: 'fatconf.h'
// Date			: April 26, 2004
// Version		: 1.00
// Target MCU	: Atmel AVR Series
// Editor Tabs	: 4
//
// NOTE: This code is based on original library "FAT16/32 file system library configuration"
//       from Pascal Stang.
//
//	     The authors in no way will be responsible for damages that you coul'd be using this code.
//       Use this code at your own risk.
//
//       This code is distributed under the GNU Public License
//       which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#ifndef FATCONF_H
#define FATCONF_H


// This Address is defined in ataconf.h, and is used, in ata.c, only to init the drive,
// so we use this here again to optimize the code and the memory usage
// This buffer is used to put de data read from hard disk
#define SECTOR_BUFFER1_ADDR SECTOR_BUFFER_ADDR
//#define SECTOR_BUFFER1_SIZE			0x0200

// This buffer is used to put the fat data read from hard disk
// It's possible to optimize the memory usage using the same buffer as defined above
// but we have to adjust the code and the performance will be decreased.
unsigned char FAT_CACHE_ADDR[512];
//#define FAT_CACHE_SIZE				0x0200

#endif
