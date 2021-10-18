//*****************************************************************************
//
// Title		: IDE-ATA interface driver for hard disks
// Authors		: Angelo Bannack, Giordano Bruno Wolaniuk
// File Name	: 'ata.c'
// Date			: April 16, 2004
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

#ifndef WIN32
	#include <avr/io.h>
	#include <avr/interrupt.h>
	#include <avr/signal.h>
	#include <avr/pgmspace.h>
#endif
#include "global.h"
#include "ata.h"

// global variables
typeDriveInfo ataDriveInfo;		// drive information


//*****************************************************************************
// Function: ataInit
// Parameters: none
// Returns: none
//
// Description: Gives a software reset into the ata dispositive
//*****************************************************************************
void ataInit(void)
{
	unsigned char i;
	unsigned char* buffer = (unsigned char*) SECTOR_BUFFER_ADDR;


	DDR_DATAL= 0x00;			// Use DATAL as the input from the hardrive
	DDR_DATAH= 0x00;			// Use DATAH as the input from the hardrive
	PORT_DATAL= 0xFF;			// Enable pullup on DATAL
	PORT_DATAH= 0xFF;			// Enable pullup on DATAH

	DDR_ADDR = 0xDF;			// Use ADDR PORT as the output for the hardrive
  								// But use PORT_ADDR.5 as host interrupt (INPUT)
    PORT_ADDR= 0xc0;            // Initialize PORT_ADDR to c0.


    ataWriteByte(ATA_REG_ACTSTATUS, 0x06);  	 	// Assert Software reboot.
    delay(100);
    ataWriteByte(ATA_REG_ACTSTATUS, 0x00);
    delay(100);

    // Wait until drive clear BUSY Bit in Alternate Status Register
    while(IDE_Wait_State(ATA_SR_BSY) == 1);

	// issue identify command
	ataWriteByte(ATA_REG_HDDEVSEL, 0);
	ataWriteByte(ATA_REG_ACTSTATUS, 0x02);

    while(IDE_Wait_State(ATA_SR_DRDY) == 0);

	ataWriteByte(ATA_REG_CMDSTATUS1, 0xEC);
	// wait for drive to request data transfer

	while(IDE_Wait_State(ATA_SR_DRQ) == 0);

	delay(200); // wait 200 us
	// read in the data
	ataReadDataBuffer(buffer, 512);

	// set local drive info parameters
	ataDriveInfo.cylinders =		*( ((unsigned int*) buffer) + ATA_IDENT_CYLINDERS );
	ataDriveInfo.heads =			*( ((unsigned int*) buffer) + ATA_IDENT_HEADS );
	ataDriveInfo.sectors =			*( ((unsigned int*) buffer) + ATA_IDENT_SECTORS );
	ataDriveInfo.LBAsupport =		*( ((unsigned int*) buffer) + ATA_IDENT_FIELDVALID );
	ataDriveInfo.sizeinsectors =	*( (unsigned long*) (buffer + ATA_IDENT_LBASECTORS*2) );
	// copy model string
	for(i=0; i<40; i+=2)
	{
		// correct for byte order
		ataDriveInfo.model[i  ] = buffer[(ATA_IDENT_MODEL*2) + i + 1];
		ataDriveInfo.model[i+1] = buffer[(ATA_IDENT_MODEL*2) + i    ];
	}
	// terminate string
	ataDriveInfo.model[40] = 0;

	// process and print info
	if(!ataDriveInfo.LBAsupport) {
		// CHS, no LBA support
		// calculate drive size
		ataDriveInfo.sizeinsectors = (unsigned long) ataDriveInfo.cylinders*ataDriveInfo.heads*ataDriveInfo.sectors;
	}
}


//*****************************************************************************
// Function: ataSetDrivePowerMode
// Parameters: DriveNo, mode, timeout
// Returns: none
//
// Description: Sets the Power Mode
//	            Possible modes: ATA_DISKMODE_SPINDOWN
//                              ATA_DISKMODE_SPINUP
//                              ATA_DISKMODE_SETTIMEOUT
//                              ATA_DISKMODE_SLEEP
//*****************************************************************************
void ataSetDrivePowerMode(unsigned char DriveNo, unsigned char mode, unsigned char timeout)
{
	// select drive
	ataDriveSelect(DriveNo);
	// Wait for drive to be ready
	ataStatusWait(ATA_SR_BSY, ATA_SR_BSY);

	// set mode
	switch(mode)
	{
	case ATA_DISKMODE_SPINDOWN:		ataWriteByte(ATA_REG_CMDSTATUS1, ATA_CMD_SPINDOWN); break;
	case ATA_DISKMODE_SPINUP:		ataWriteByte(ATA_REG_CMDSTATUS1, ATA_CMD_SPINUP); break;
	case ATA_DISKMODE_SETTIMEOUT:
		ataWriteByte(ATA_REG_SECCOUNT, timeout);
		ataWriteByte(ATA_REG_CMDSTATUS1, ATA_CMD_IDLE_5SU);
		break;
	case ATA_DISKMODE_SLEEP:		ataWriteByte(ATA_REG_CMDSTATUS1, ATA_CMD_SLEEP); break;
	default:
		break;
	}
}



//*****************************************************************************
// Function: ataReadDataBuffer
// Parameters: Buffer, numBytes
// Returns: none
//
// Description: The Buffer will be filled with the readed data
//*****************************************************************************
void ataReadDataBuffer(unsigned char *Buffer, unsigned short numBytes)
{
	register unsigned int i asm ("r16");

	DDR_DATAL = 0x00; 		      	// Use the PORT DATAH as an input
	DDR_DATAH = 0x00; 		      	// Use the PORT DATAL as an input
	PORT_DATAL= 0xFF;				// habilita pull-ups
	PORT_DATAH= 0xFF;				// habilita pull-ups
	PORT_ADDR = PORT_ADDR & 0xe0; 		// Clear the lower 5 bits of the address line
	PORT_ADDR = PORT_ADDR | (ATA_REG_DATA & 0x1f); 	// Assert the address Line

	for (i=0; i< numBytes; i+=2)
	{
		cbi(PORT_IDE_RD, PIN_IDE_RD);
	  	__asm volatile ("NOP");
	  	__asm volatile ("NOP");
	  	*(Buffer++)= PIN_DATAL;
	  	*(Buffer++)= PIN_DATAH;
		sbi(PORT_IDE_RD, PIN_IDE_RD);
	}
}


////////////////////
#ifndef ATA_READ_ONLY
//*****************************************************************************
// Function: ataWriteDataBuffer
// Parameters: Buffer, numBytes
// Returns: none
//
// Description: The entire Buffer will be pulled out to the ata dispositive
//*****************************************************************************
void ataWriteDataBuffer(unsigned char *Buffer, unsigned short numBytes)
{
	register unsigned int i asm ("r16");

	DDR_DATAL = 0xFF; 		      	// Use the PORT DATAH as an output
	DDR_DATAH = 0xFF; 		      	// Use the PORT DATAL as an output
	PORT_ADDR = PORT_ADDR & 0xe0; 		// Clear the lower 5 bits of the address line
	PORT_ADDR = PORT_ADDR | (ATA_REG_DATA & 0x1f); 	// Assert the address Line

	for (i=0; i< numBytes; i+=2)
	{
	  	PORT_DATAL= *(Buffer++);
	  	PORT_DATAH= *(Buffer++);
	  	cbi(PORT_IDE_WR, PIN_IDE_WR);
	  	__asm volatile ("NOP");
	  	__asm volatile ("NOP");
		sbi(PORT_IDE_WR, PIN_IDE_WR);
	}
}
#endif
////////////////////


//*****************************************************************************
// Function: ataStatusWait
// Parameters: mask, waitStatus
// Returns: the wait status
//
// Description: Read the Status form the hard disk until the result be equal waitStatus
//*****************************************************************************
unsigned char ataStatusWait(unsigned char mask, unsigned char waitStatus)
{
	register unsigned char status;

	// wait for desired status
	while( ((status = ataReadByte(ATA_REG_ACTSTATUS)) & mask) == waitStatus );
	return status;
}

//*****************************************************************************
// Function: ataReadSectorCHS
// Parameters: Driver, Head, Track, Sector, Buffer
// Returns: on Suscefull returns 0, the result errors otherwise.
//
// Description: Read one Sector (512 Bytes) from the ata dispositive
//*****************************************************************************
unsigned char ataReadSectorsCHS(	unsigned char Drive,
											unsigned char Head,
											unsigned int Track,
											unsigned char Sector,
											unsigned char *Buffer)
{
  	unsigned char temp;

	// Wait for drive to be ready
	temp = ataStatusWait(ATA_SR_BSY, ATA_SR_BSY);

  	// Prepare parameters...
	ataWriteByte(ATA_REG_HDDEVSEL, 0xA0+(Drive ? 0x10:00)+Head); // CHS mode/Drive/Head
	ataWriteByte(ATA_REG_CYLHI, Track>>8);  		// MSB of track
	ataWriteByte(ATA_REG_CYLLO, Track);     		// LSB of track
  	ataWriteByte(ATA_REG_STARTSEC, Sector);    	// sector
	ataWriteByte(ATA_REG_SECCOUNT, 1);			// # of sectors

	// Issue read sector command...
  	ataWriteByte(ATA_REG_CMDSTATUS1, 0x21);

	// Wait for drive to be ready
	temp = ataStatusWait(ATA_SR_BSY, ATA_SR_BSY);

	if (temp & ATA_SR_ERR)
		return 1;

	// Wait for drive to request data transfer
	ataStatusWait(ATA_SR_DRQ, 0);

	// read data from drive
	ataReadDataBuffer(Buffer, 512);

	// Return the error bit from the status register...
	temp = ataReadByte(ATA_REG_CMDSTATUS1);	// read status register

	return (temp & ATA_SR_ERR) ? 1:0;
}


////////////////////
#ifndef ATA_READ_ONLY
//*****************************************************************************
// Function: ataWriteSectorsCHS
// Parameters: Driver, Head, Track, Sector, Buffer
// Returns: on Suscefull returns 0, the result errors otherwise.
//
// Description: Write one Sector (512 Bytes) to the ata dispositive
//*****************************************************************************
unsigned char ataWriteSectorsCHS(unsigned char Drive,
											unsigned char Head,
											unsigned int Track,
											unsigned char Sector,
											unsigned char *Buffer)
{
  	unsigned char temp;

	// Wait for drive to be ready
	temp = ataStatusWait(ATA_SR_BSY, ATA_SR_BSY);

  	// Prepare parameters...
	ataWriteByte(ATA_REG_HDDEVSEL, 0xA0+(Drive ? 0x10:00)+Head); // CHS mode/Drive/Head
	ataWriteByte(ATA_REG_CYLHI, Track>>8);  		// MSB of track
	ataWriteByte(ATA_REG_CYLLO, Track);     		// LSB of track
  	ataWriteByte(ATA_REG_STARTSEC, Sector);    	// sector
	ataWriteByte(ATA_REG_SECCOUNT, 1);			// # of sectors

	// Issue write sector command
  	ataWriteByte(ATA_REG_CMDSTATUS1, 0x31);

	// Wait for drive to request data transfer
	ataStatusWait(ATA_SR_DRQ, 0);

	// write data to drive
	ataWriteDataBuffer(Buffer, 512);

	// Wait for drive to finish write
	temp = ataStatusWait(ATA_SR_BSY, ATA_SR_BSY);

	// Return the error bit from the status register...
	return (temp & ATA_SR_ERR) ? 1:0;
}
#endif
////////////////////


//*****************************************************************************
// Function: ataReadSectorLBA
// Parameters: Driver, lba address, Buffer
// Returns: on Suscefull returns 0, the result errors otherwise.
//
// Description: Read one Sector (512 Bytes) from the ata dispositive
//*****************************************************************************
unsigned char ataReadSectorsLBA(	unsigned char Drive,
									unsigned long lba,
                            		unsigned char *Buffer)
{
  	unsigned int cyl, head, sect;
  	unsigned char temp;


	sect = (int) ( lba & 0x000000ffL );
	lba = lba >> 8;
	cyl = (int) ( lba & 0x0000ffff );
	lba = lba >> 16;
	head = ( (int) ( lba & 0x0fL ) ) | ATA_HEAD_USE_LBA;

	temp = ataReadSectorsCHS( Drive, head, cyl, sect, Buffer );
	return temp;
}


////////////////////
#ifndef ATA_READ_ONLY
//*****************************************************************************
// Function: ataWriteSectorsLBA
// Parameters: Driver, lba address, Buffer
// Returns: on Suscefull returns 0, the result errors otherwise.
//
// Description: Write one Sector (512 Bytes) to the ata dispositive
//*****************************************************************************
unsigned char ataWriteSectorsLBA(	unsigned char Drive,
									unsigned long lba,
									unsigned char *Buffer)
{
	unsigned int cyl, head, sect;
	unsigned char temp;

	sect = (int) ( lba & 0x000000ffL );
	lba = lba >> 8;
	cyl = (int) ( lba & 0x0000ffff );
	lba = lba >> 16;
	head = ( (int) ( lba & 0x0fL ) ) | ATA_HEAD_USE_LBA;

	temp = ataWriteSectorsCHS( Drive, head, cyl, sect, Buffer );
	return temp;
}
#endif
////////////////////


//*****************************************************************************
// Function: ataReadSectors
// Parameters: Driver, lba, Buffer, SectorInCache
// Returns: on Suscefull returns 0, the result errors otherwise.
//
// Description: Read one Sector (512 Bytes) from the ata dispositive
//*****************************************************************************
unsigned char ataReadSectors(	unsigned char Drive,
								unsigned long lba,//sector number
                            	unsigned char *Buffer,
                            	unsigned long *SectorInCache //actual sector
                            )
{
  	unsigned int cyl, head, sect;
  	unsigned char temp;


	// if the Sector is already in the memory buffer, we don't need to read the sector again
 	if(*SectorInCache==lba)
  		return 0;

	*SectorInCache=lba;

	// check if drive supports native LBA mode
	if(ataDriveInfo.LBAsupport)
	{
		// drive supports using native LBA
		temp = ataReadSectorsLBA(Drive, lba, Buffer);
	}
	else
	{
		// drive required CHS access
		// convert LBA to pseudo CHS
		// remember to offset the sector count by one
		sect = (unsigned char) (lba % ataDriveInfo.sectors)+1;
		lba = lba / ataDriveInfo.sectors;
		head = (unsigned char) (lba % ataDriveInfo.heads);
		lba = lba / ataDriveInfo.heads;
		cyl = (unsigned short) lba;

		temp = ataReadSectorsCHS( Drive, head, cyl, sect, Buffer );
	}
	return temp;
}


////////////////////
#ifndef ATA_READ_ONLY
//*****************************************************************************
// Function: ataWriteSectors
// Parameters: Driver, lba, Buffer
// Returns: on Suscefull returns 0, the result errors otherwise.
//
// Description: Write one Sector (512 Bytes) to the ata dispositive
//*****************************************************************************
unsigned char ataWriteSectors(	unsigned char Drive,
								unsigned long lba,
                            	unsigned char *Buffer)
{
  	unsigned int cyl, head, sect;
  	unsigned char temp;

	// check if drive supports native LBA mode
	if(ataDriveInfo.LBAsupport)
	{
		// drive supports using native LBA
		temp = ataWriteSectorsLBA(Drive, lba, Buffer);
	}
	else
	{
		// drive required CHS access
		// convert LBA to pseudo CHS
		// remember to offset the sector count by one
		sect = (unsigned char) (lba % ataDriveInfo.sectors)+1;
		lba = lba / ataDriveInfo.sectors;
		head = (unsigned char) (lba % ataDriveInfo.heads);
		lba = lba / ataDriveInfo.heads;
		cyl = (unsigned short) lba;

		temp = ataWriteSectorsCHS( Drive, head, cyl, sect, Buffer );
	}
	return temp;
}
#endif
////////////////////


//*****************************************************************************
// Function: ataDriveSelect
// Parameters: Drive Number
// Returns: none.
//
// Description: Change the drive, Master or Slave
//*****************************************************************************
void ataDriveSelect(unsigned char DriveNo)
{
	ataWriteByte(ATA_REG_HDDEVSEL, 0xA0+(DriveNo ? 0x10:00)); // Drive selection
}


//*****************************************************************************
// Function: ataReadByte
// Parameters: reg
// Returns: The Readed Data from the ata dispositive.
//
// Description: Read one Byte from the ata dispositive
//*****************************************************************************
unsigned char ataReadByte(unsigned char reg)
{
	register unsigned char ret;

  	PORT_DATAL= 0xFF;				// habilita pull-ups
  	DDR_DATAL  = 0x00; 		      	// Use the DATAH as an input
  	PORT_ADDR = PORT_ADDR & 0xe0; 		// Clear the lower 5 bits of the address line
  	PORT_ADDR = PORT_ADDR | (reg & 0x1f); 	// Assert the address Line

  	cbi(PORT_IDE_RD, PIN_IDE_RD);                  	// Assert DIOR
  	__asm volatile ("NOP");
  	__asm volatile ("NOP");
  	__asm volatile ("NOP");

  	ret = PIN_DATAL;
  	sbi(PORT_IDE_RD, PIN_IDE_RD);			      	// Negate DIOR

  	return (ret);
}


//*****************************************************************************
// Function: ataWriteByte
// Parameters: reg, data
// Returns: none.
//
// Description: Write one Byte to the ata dispositive
//*****************************************************************************
void ataWriteByte(unsigned char reg, unsigned char data)
{
  	DDR_DATAL = 0xff;			   	// Use DATAL an an output
  	PORT_ADDR = PORT_ADDR & 0xe0;	       		// Clear the Lower 5 bits of the Address Line
  	PORT_ADDR = PORT_ADDR | (reg & 0x1f); 	// Assert the Address line

  	PORT_DATAL = data;				// Output the data

  	cbi(PORT_IDE_WR,PIN_IDE_WR);				// Assert DIOW
  	__asm volatile ("NOP");
  	__asm volatile ("NOP");
  	__asm volatile ("NOP");
  	sbi(PORT_IDE_WR,PIN_IDE_WR);				// Negate DIOW
}



//*****************************************************************************
// Function: IDE_Wait_State
// Parameters: test_bit
// Returns: on suscefull returns 1, otherwise returns 0.
//
// Description: Test a bit in the status register
//*****************************************************************************
unsigned char IDE_Wait_State(unsigned char test_bit)
{
	if ((ataReadByte(ATA_REG_ACTSTATUS) & test_bit) == test_bit)
		return 1;
	return 0;
}



//*****************************************************************************
// Function: ataGetSizeInSectors
// Parameters: none
// Returns: Size in sectors from the ata dispositive
//
// Description: Returns the number of sectors from the ata dispositive
//*****************************************************************************
unsigned long ataGetSizeInSectors(void)
{
	return(ataDriveInfo.sizeinsectors);
}


//*****************************************************************************
// Function: ataGetSize
// Parameters: none
// Returns: Size in bytes from the ata dispositive
//
// Description: Returns the size in bytes from the ata dispositive
//*****************************************************************************
unsigned long ataGetSize(void)
{
	return(ataDriveInfo.sizeinsectors/(1000000/512));
}



//*****************************************************************************
// Function: ataGetModel
// Parameters: none
// Returns: Model string from the ata dispositive
//
// Description: Returns the model string from the ata dispositive
//*****************************************************************************
char *ataGetModel(void)
{
	return(ataDriveInfo.model);
}


//*****************************************************************************
// Function: delay
// Parameters: time in us
// Returns: none
//
// Description: delay for a minimum of <us> microseconds
// 			    the time resolution is dependent on the time the loop takes
// 	            e.g. with 4Mhz and 5 cycles per loop, the resolution is 1.25 us
//*****************************************************************************
void delay(unsigned short us)
{
	unsigned short delay_loops;
	register unsigned short i;

	delay_loops = (us+3)/5*CYCLES_PER_US; // +3 for rounding up (dirty)

	// one loop takes 5 cpu cycles
	for (i=0; i < delay_loops; i++) {};
}
