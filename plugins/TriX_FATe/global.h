//*****************************************************************************
//
// Title		: AVRlib project global include
// Authors		: Angelo Bannack, Giordano Bruno Wolaniuk
// File Name	: 'global.h'
// Date			: April 26, 2004
// Version		: 1.00
// Target MCU	: Atmel AVR Series
// Editor Tabs	: 4
//
// NOTE: This code is based on original library "AVRlib project global include"
//       from Pascal Stang.
//
//	     The authors in no way will be responsible for damages that you coul'd be using this code.
//       Use this code at your own risk.
//
//       This code is distributed under the GNU Public License
//       which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#ifndef GLOBAL_H
#define GLOBAL_H


#define FALSE	 0
#define TRUE	-1

// project/system dependent defines

// CPU clock speed
  #define F_CPU       16000000               		// 16MHz processor
//#define F_CPU       14745000               		// 14.745MHz processor
//#define F_CPU        8000000               		// 8MHz processor
//#define F_CPU        7370000               		// 7.37MHz processor
//#define F_CPU        4000000               		// 4MHz processor
//#define F_CPU        3686400               		// 3.69MHz processor
#define CYCLES_PER_US ((F_CPU+500000)/1000000) 		// cpu cycles per microsecond

#endif
