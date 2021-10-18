//*****************************************************************************
//
// Title		: FAT time driver
// Authors		: Angelo Bannack, Giordano Bruno Wolaniuk
// File Name	: 'fattime.h'
// Date			: April 26, 2004
// Version		: 1.00
// Target MCU	: Atmel AVR Series
// Editor Tabs	: 4
//
// NOTE: The authors in no way will be responsible for damages that you coul'd be using this code.
//       Use this code at your own risk.
//
//       This code is distributed under the GNU Public License
//       which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************


#ifndef FATTIME_H
#define FATTIME_H


// This is the format of the contents of the deTime field in the direntry
// structure.
// We don't use bitfields because we don't know how compilers for
// arbitrary machines will lay them out.
#define DT_2SECONDS_MASK        0x1F    // seconds divided by 2
#define DT_2SECONDS_SHIFT       1		// SHIFT to right, ALL the others are to left
#define DT_MINUTES_MASK         0x7E0   // minutes
#define DT_MINUTES_SHIFT        5
#define DT_HOURS_MASK           0xF800  // hours
#define DT_HOURS_SHIFT          11

// This is the format of the contents of the deDate field in the direntry
// structure.
#define DD_DAY_MASK             0x1F    // day of month
#define DD_DAY_SHIFT            0
#define DD_MONTH_MASK           0x1E0   // month
#define DD_MONTH_SHIFT          5
#define DD_YEAR_MASK            0xFE00  // year - 1980
#define DD_YEAR_SHIFT           9


////////////////////
#ifndef ATA_READ_ONLY
typedef struct
{
	unsigned char day;
	unsigned char month;
	unsigned int  year;
	unsigned char hour;
	unsigned char minutes;
	unsigned char seconds;
}TTime;


void fatGetCurTime(TTime *t);

#endif
////////////////////

#endif
