//*****************************************************************************
//
// Title		: FAT time driver
// Authors		: Angelo Bannack, Giordano Bruno Wolaniuk
// File Name	: 'fattime.c'
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

#include "fattime.h"


////////////////////
#ifndef ATA_READ_ONLY
//*****************************************************************************
// Function: fatGetCurTime
// Parameters: The current Date and Time Address Struct
// Returns: none
//
// Description: Returns the current Time and Date
//
//              You will have to change this routine to read your local time,
//              and return it in this format.
//
//              Date and Time Format
//              Day:     valid range 1-31
//	            Month:   1= January, valid range 1-12
//              Year:    valid range 0-65535
//              Hour:    valid range 0-23
//              Minutes: valid range 0-59
//              Seconds: valid range 0-59
//*****************************************************************************
void fatGetCurTime(TTime *t)
{
	t->day     = 26;
	t->month   = 04;
	t->year    = 2004;
	t->hour    = 17;
	t->minutes = 30;
	t->seconds = 00;
}
#endif
////////////////////

