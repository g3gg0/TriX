//*****************************************************************************
//
// Title		: IDE-ATA interface driver for hard disks
// Authors		: Angelo Bannack, Giordano Bruno Wolaniuk
// File Name	: 'ata.h'
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

#ifndef ATA_H
#define ATA_H

#include "global.h"
#include "ataconf.h"

// constants
#define DRIVE0		0

#define STANDBY		0
#define SLEEP		1
#define IDLE		2

// ATA status register bits
#define ATA_SR_BSY		0x80
#define ATA_SR_DRDY		0x40
#define ATA_SR_DF		0x20
#define ATA_SR_DSC		0x10
#define ATA_SR_DRQ		0x08
#define ATA_SR_CORR		0x04
#define ATA_SR_IDX		0x02
#define ATA_SR_ERR		0x01

// ATA error register bits
#define ATA_ER_UNC		0x40
#define ATA_ER_MC		0x20
#define ATA_ER_IDNF		0x10
#define ATA_ER_MCR		0x08
#define ATA_ER_ABRT		0x04
#define ATA_ER_TK0NF	0x02
#define ATA_ER_AMNF		0x01

// ATA error register bits
#define ATA_HEAD_USE_LBA	0x40

// ATA commands
#define ATA_CMD_READ			0x20
#define ATA_CMD_READNR			0x21
#define ATA_CMD_WRITE			0x30
#define ATA_CMD_WRITENR			0x31
#define ATA_CMD_IDENTIFY		0xEC
#define ATA_CMD_RECALIBRATE		0x10
#define ATA_CMD_SPINDOWN		0xE0	// spin down disk immediately
#define ATA_CMD_SPINUP			0xE1	// spin up disk immediately
#define ATA_CMD_STANDBY_5SU		0xE2	// spin down disk and set auto-power-down timer (sectorcount*5sec)
#define ATA_CMD_IDLE_5SU		0xE3	// keep disk spinning and set auto-power-down timer (sectorcount*5sec)
#define ATA_CMD_SLEEP			0xE6	// sleep disk (wakeup only on HW or SW reset)
#define ATA_CMD_STANDBY_01SU	0xF2	// spin down disk and set auto-power-down timer (sectorcount*0.1sec)
#define ATA_CMD_IDLE_01SU		0xF3	// keep disk spinning and set auto-power-down timer (sectorcount*0.1sec)


// ATA CHS disk parameters (examples, now we autodetect)
#define ATA_DISKPARM_CLYS		0x03A6
#define ATA_DISKPARM_HEADS		0x10
#define ATA_DISKPARM_SECTORS	0x11

// ATA Identity fields
// all offsets refer to word offset (2 unsigned char increments)
#define ATA_IDENT_DEVICETYPE	0		// specifies ATA/ATAPI, removable/non-removable
#define ATA_IDENT_CYLINDERS		1		// number of logical cylinders
#define ATA_IDENT_HEADS			3		// number of logical heads
#define ATA_IDENT_SECTORS		6		// number of sectors per track
#define ATA_IDENT_SERIAL		10		// drive model name (20 characters)
#define ATA_IDENT_MODEL			27		// drive model name (40 characters)
#define ATA_IDENT_FIELDVALID	53		// indicates field validity of higher words (bit0: words54-58, bit1: words 64-70)
#define ATA_IDENT_LBASECTORS	60		// number of sectors in LBA translation mode

// drive mode defines (for ataSetDrivePowerMode() )
#define ATA_DISKMODE_SPINDOWN	0
#define ATA_DISKMODE_SPINUP		1
#define ATA_DISKMODE_SETTIMEOUT	2
#define ATA_DISKMODE_SLEEP		3

// typedefs
// drive info structure
typedef struct
{
	unsigned int  cylinders;
	unsigned char heads;
	unsigned char sectors;
	unsigned long sizeinsectors;
	unsigned char LBAsupport;
	char model[41];
} typeDriveInfo;


// Prototypes
void delay(unsigned short);
unsigned char IDE_Wait_State(unsigned char);
void            ataInit                (void);
void            ataDriveSelect         (unsigned char DriveNo);
void            ataSetDrivePowerMode   (unsigned char DriveNo, unsigned char mode, unsigned char timeout);
unsigned char   ataReadByte            (unsigned char reg);
void            ataWriteByte           (unsigned char reg, unsigned char data);
void            ataReadDataBuffer      (unsigned char *Buffer, unsigned short numBytes);
////////////////////
#ifndef ATA_READ_ONLY
void            ataWriteDataBuffer     (unsigned char *Buffer, unsigned short numBytes);
#endif
////////////////////
unsigned char   ataStatusWait          (unsigned char mask, unsigned char waitStatus);
unsigned long   ataGetSizeInSectors    (void);
unsigned long   ataGetSize             (void);
char           *ataGetModel            (void);

// read and write routines for CHS based drives
unsigned char   ataReadSectorsCHS      (unsigned char Drive,
										unsigned char Head,
										unsigned int Track,
										unsigned char Sector,
										unsigned char *Buffer);

////////////////////
#ifndef ATA_READ_ONLY
unsigned char   ataWriteSectorsCHS     (unsigned char Drive,
										unsigned char Head,
										unsigned int Track,
										unsigned char Sector,
										unsigned char *Buffer);
#endif
////////////////////

// read and write routines for LBA based drives
unsigned char   ataReadSectorsLBA      (unsigned char Drive,
										unsigned long lba,
                            			unsigned char *Buffer);

////////////////////
#ifndef ATA_READ_ONLY
unsigned char   ataWriteSectorsLBA     (unsigned char Drive,
										unsigned long lba,
                            			unsigned char *Buffer);
#endif
////////////////////

// generic read and write routines using LBA
//   uses native or translated LBA addressing
//   given autodetected drive type
unsigned char   ataReadSectors         (unsigned char Drive,
										unsigned long lba,//sector number
                            			unsigned char *Buffer,
                            			unsigned long *SectorInCache); //actual sector
////////////////////
#ifndef ATA_READ_ONLY
unsigned char   ataWriteSectors        (unsigned char Drive,
										unsigned long lba,
	                           			unsigned char *Buffer);
#endif
////////////////////


#endif
