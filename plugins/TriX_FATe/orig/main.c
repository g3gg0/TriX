//*****************************************************************************
//
// Title		: FAT16/32 file system driver for ATMEL AVR
// Authors		: Angelo Bannack, Giordano Bruno Wolaniuk
// File Name	: 'main.c'
// Date			: April 26, 2004
// Version		: 1.00
// Target MCU	: Atmel AVR Series
// Editor Tabs	: 4
//
// NOTE: This is a code example about how to use the "FAT16/32 file system driver",
//       from Angelo Bannack and Giordano Bruno Wolaniuk.
//       This example was compiled by WINAVR GCC 20040404 to an ATMega128, and uses the UART0
//       configured as 115.200 bps, 8 bits, 1 stop bit, to send debug informations.
//
//	     The authors in no way will be responsible for damages that you coul'd be using this code.
//       Use this code at your own risk.
//
//       This code is distributed under the GNU Public License
//       which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************
#include <avr/io.h>
#include <string.h>
#include <stdio.h>
#include "fat.h"
#include "ata.h"
#include "fattime.h"

void mcuInit		(void);
void dir_serial		(unsigned long cluster);
void print_hd_info	(void);
int  uart_putc		(char c);
int  uart_getc		(void);


//*****************************************************************************
// Function: main
// Parameters: none
// Returns: 0
//
// Description: Main function to test the FAT library
//*****************************************************************************
int main(void)
{
	TFILE *farq;
	struct direntry *de;
	char path[12];


	mcuInit();							// start ATMega128
	fdevopen(uart_putc, uart_getc, 0);	// configure printf to send data to serial port

	printf("FAT16/32 file system driver v1.00\r\n");
	printf("Writed by Angelo Bannack and Giordano Bruno Wolaniuk\r\n\r\n");

	ataInit();	// Start ATA
	fatInit();	// Start FAT

	print_hd_info();


	//***********************************************
	// fatDir usage example
	printf("\r\nC:\\>dir *.*\r\n");
	dir_serial(fatGetFirstDirCluster());
	//***********************************************

	////////////////////
	#ifndef ATA_READ_ONLY
	//***********************************************
	// fatMkdir usage example
	strcpy(path, "dir1");
	printf("\r\nC:\\>mkdir %s\r\n", path);
	if (fatMkdir(path))
		printf("\r\nDirectory %s created!\r\n", path);
	else
		printf("\r\nDirectory %s not created!\r\n", path);
	printf("\r\nC:\\>dir *.*\r\n");
	dir_serial(fatGetCurDirCluster());
	//***********************************************


	//***********************************************
	// fatCddir usage example
	printf ("\r\nC:\\>cd %s\r\n", path);
	if (fatCddir(path))
	{
		printf ("\r\nC:\\%s>dir *.*\r\n", path);
		dir_serial(fatGetCurDirCluster());
	}
	else
		printf ("\r\nError in cd %s", path);
	//***********************************************



	//***********************************************
	// fatFcreate usage example
	printf ("\r\nC:\\%s>make readme.txt\r\n", path);
	if (fatFcreate("readme.txt"))
	{
		printf ("\r\nFile readme.txt created suscefuly\r\n");
		printf ("\r\nC:\\%s>dir *.*\r\n", path);
		dir_serial(fatGetCurDirCluster());
	}
	else
		printf("\r\nError creating readme.txt\r\n");
	//***********************************************



	//***********************************************
	// fatFopen usage example
	// fatFputc usage example
	// fatFgetc usage example
	// fatFflush usage example
	// fatFseek usage example
	// fatFclose usage example
	printf ("\r\nC:\\%s>open readme.txt\r\n", path);
	if (farq= fatFopen("readme.txt"))
	{
		printf("\r\nfile %s, opened to write\r\n", path);
		printf ("\r\nC:\\%s>put \"Testing 123\" in  readme.txt\r\n", path);

		fatFputc(farq, 'T');
		fatFputc(farq, 'e');
		fatFputc(farq, 's');
		fatFputc(farq, 't');
		fatFputc(farq, 'i');
		fatFputc(farq, 'n');
		fatFputc(farq, 'g');
		fatFputc(farq, ' ');
		fatFputc(farq, '1');
		fatFputc(farq, '2');
		fatFputc(farq, '3');

		printf ("\r\nWriting file...\r\n");
		fatFflush(farq);
		fatFseek(farq, 0, SEEK_SET);

		printf ("\r\nReading file...\r\n");
		while(!fatFeof(farq))
			printf("%c", fatFgetc(farq));

		printf ("\r\nClosing file...\r\n");
		fatFclose(farq);
		printf ("\r\nFile readme.txt closed.\r\n");
	}
	else
		printf("\r\nError opening readme.txt\r\n");
	//***********************************************



	//***********************************************
	// fatRename usage example
	printf ("\r\nC:\\%s>rename readme.txt test.txt\r\n", path);
	if (fatRename("readme.txt", "test.txt"))
	{
		printf ("\r\nC:\\%s>dir *.*\r\n", path);
		dir_serial(fatGetCurDirCluster());
	}
	else
		printf("\r\nError renaming readme.txt\r\n");
	//***********************************************



	//***********************************************
	//fatRemoveAll usage example
	printf ("\r\nC:\\%s>del *.*\r\n", path);
	fatRemoveAll();
	printf ("\r\nC:\\%s>dir *.*\r\n", path);
	dir_serial(fatGetCurDirCluster());
	//***********************************************



	//***********************************************
	// fatCddir usage example
	printf ("\r\nC:\\%s>cd ..\r\n", path);
	if (fatCddir(".."))
	{
		printf ("\r\nC:\\>dir *.*\r\n");
		dir_serial(fatGetCurDirCluster());
	}
	else
		printf ("\r\nError in cd ..\r\n");
	//***********************************************



	//***********************************************
	//fatRemove usage example
	printf ("\r\nC:\\>remove %s\r\n", path);
	if (fatRemove(path))
	{
		printf ("\r\nC:\\>dir *.*\r\n");
		dir_serial(fatGetCurDirCluster());
	}
	else
		printf ("\r\nError removing %s\r\n", path);
	//***********************************************
	#endif
	////////////////////


	while(1){}

	return(0);
}



//*****************************************************************************
// Function: dir_serial
// Parameters: start cluster
// Returns: none
//
// Description: Print the directory started in cluster
//*****************************************************************************
void dir_serial(unsigned long cluster)
{
	unsigned long offset=0;
	struct direntry *de = 0;	// avoid compiler warning by initializing
	int index, aux;
	unsigned char day, month, year, hour, minutes, seconds;
	unsigned int time, date;

	printf("Vol Label: %s\r\n", fatGetVolLabel());
	for (offset=0; ; offset++)
	{
		de = (struct direntry *) fatDir(cluster, offset);
		if (de == NULL)
			return;
		for (index=0; index<16; index++) // 16 direntries
		{

			if (*de->deName == SLOT_EMPTY)
				return;		// there is no more direntries
			if((*de->deName != SLOT_DELETED) && (de->deAttributes != ATTR_LONG_FILENAME))
			{
				for (aux=0; aux<8; aux++)
					uart_putc(de->deName[aux]);
				printf(".");
				for (aux=8; aux<11; aux++)
					uart_putc(de->deName[aux]);
				printf(" atrib: ");
				printf("%02x", de->deAttributes);
				printf(" cluster: ");
				printf("%04X", de->deStartCluster);
				date= de->deCDate[0] + (de->deCDate[1] << 8);
				time= de->deCTime[0] + (de->deCTime[1] << 8);
				day=(date&DD_DAY_MASK)>>DD_DAY_SHIFT;
				month=(date&DD_MONTH_MASK)>>DD_MONTH_SHIFT;
				year=(date&DD_YEAR_MASK)>>DD_YEAR_SHIFT;
				hour=(time&DT_HOURS_MASK)>>DT_HOURS_SHIFT;
				minutes=(time&DT_MINUTES_MASK)>>DT_MINUTES_SHIFT;
				seconds=(time&DT_2SECONDS_MASK)<<DT_2SECONDS_SHIFT;
				printf("\t%02d/%02d/%04d", month, day, year+1980);
				printf(" %02d:%02d:%02d", hour, minutes, seconds);
				printf("\r\n");
			}
			de++;
		}	// end of sector
	}	// end of cluster
}


//*****************************************************************************
// Function: print_hd_info
// Parameters: none
// Returns: none
//
// Description: print informations about the ATA dispositive and the FAT File System
//*****************************************************************************
void print_hd_info(void)
{
	struct partrecord *PartInfo;

	printf("Drive 0: %dMB ", ataGetSize());
	printf("MODEL: %s\r\n", ataGetModel());

	PartInfo= fatGetPartInfo();
	switch (PartInfo->prPartType)
	{
		case PART_TYPE_DOSFAT16: 	printf("Found: DOSFAT 16\r\n"); break;
		case PART_TYPE_FAT16:		printf("Found: FAT16\r\n"); 	 break;
		case PART_TYPE_FAT16LBA:	printf("Found: FAT16 LBA\r\n"); break;
		case PART_TYPE_FAT32LBA:	printf("Found: FAT32 LBA\r\n"); break;
		case PART_TYPE_FAT32:		printf("Found: FAT32\r\n");  	 break;
		default:					printf("Found: No Partition!\r\n"); break;
	}
	printf("Cluster Size     : %d\r\n", fatClusterSize());
	printf("First sector     : %ld\r\n", PartInfo->prStartLBA);
	printf("Size             : %ld\r\n", PartInfo->prSize);
	printf("sectors/cluster  : %u\r\n", fatGetSecPerClust());
	printf("First Fat1 Sector: %ld\r\n", fatGetFirstFATSector());
	printf("First Fat2 Sector: %ld\r\n", fatGetFirstFAT2Sector());
	printf("First Data Sect  : %ld\r\n", fatGetFirstDataSector());
	printf("Number of Clust  : %ld\r\n", fatGetNumClusters());
	printf("Number of Sects  : %ld\r\n", ataGetSizeInSectors());
}



//*****************************************************************************
// Function: uart_putc
// Parameters: char to send
// Returns: 0
//
// Description: send a character to UART and return 0 for compatibility reasons
//*****************************************************************************
int uart_putc(char c)
{
    loop_until_bit_is_set(UCSR0A, UDRE0);
 	UDR0 = c;
    return 0;
}


//*****************************************************************************
// Function: uart_getc
// Parameters: none
// Returns: the character read from UART
//
// Description: get a character from UART and return it, return -1 if the UART
//              was empty
//*****************************************************************************
int uart_getc(void)
{
	if (bit_is_set(UCSR0A, RXC) == 0)
		return -1;
	return UDR0;
}


//*****************************************************************************
// Function: mcuInit
// Parameters: none
// Returns: none
//
// Description: Initialize the ATMega128
//*****************************************************************************
void mcuInit(void)
{
	// Input/Output Ports initialization
	PORTA=0xFF;// Port A initialization
	DDRA=0xDF;

	PORTB=0x00;// Port B initialization
	DDRB=0x00;

	PORTC=0x00;// Port C initialization
	DDRC=0xFF;

	PORTD=0x00;// Port D initialization
	DDRD=0x00;

	PORTE=0x00;// Port E initialization
	DDRE=0x00;

	PORTF=0x00;// Port F initialization
	DDRF=0x00;

	PORTG=0x00;// Port G initialization
	DDRG=0x00;

	// Timer/Counter 0 initialization
	// Clock source: System Clock
	// Clock value: Timer 0 Stopped
	// Mode: Normal top=FFh
	// OC0 output: Disconnected
	ASSR=0x00;
	TCCR0=0x00;
	TCNT0=0x00;
	OCR0=0x00;

	// Timer/Counter 1 initialization
	// Clock source: System Clock
	// Clock value: Timer 1 Stopped
	// Mode: Normal top=FFFFh
	// OC1A output: Discon.
	// OC1B output: Discon.
	// OC1C output: Discon.
	// Noise Canceler: Off
	// Input Capture on Falling Edge
	TCCR1A=0x00;
	TCCR1B=0x00;
	TCNT1H=0x00;
	TCNT1L=0x00;
	OCR1AH=0x00;
	OCR1AL=0x00;
	OCR1BH=0x00;
	OCR1BL=0x00;
	OCR1CH=0x00;
	OCR1CL=0x00;

	// Timer/Counter 2 initialization
	// Clock source: System Clock
	// Clock value: Timer 2 Stopped
	// Mode: Normal top=FFh
	// OC2 output: Disconnected
	TCCR2=0x00;
	TCNT2=0x00;
	OCR2=0x00;

	// Timer/Counter 3 initialization
	// Clock source: System Clock
	// Clock value: Timer 3 Stopped
	// Mode: Normal top=FFFFh
	// OC3A output: Discon.
	// OC3B output: Discon.
	// OC3C output: Discon.
	TCCR3A=0x00;
	TCCR3B=0x00;
	TCNT3H=0x00;
	TCNT3L=0x00;
	OCR3AH=0x00;
	OCR3AL=0x00;
	OCR3BH=0x00;
	OCR3BL=0x00;
	OCR3CH=0x00;
	OCR3CL=0x00;

	// External Interrupt(s) initialization
	// INT0: Off
	// INT1: Off
	// INT2: Off
	// INT3: Off
	// INT4: Off
	// INT5: Off
	// INT6: Off
	// INT7: Off
	EICRA=0x00;
	EICRB=0x00;
	EIMSK=0x00;

	// Timer(s)/Counter(s) Interrupt(s) initialization
	TIMSK=0x00;
	ETIMSK=0x00;

	// USART0 initialization
	// Communication Parameters: 8 Data, 1 Stop, No Parity
	// USART0 Receiver: Off
	// USART0 Transmitter: On
	// USART0 Mode: Asynchronous
	// USART0 Baud rate: 57600 (double speed = 115200)
	UCSR0A=0x02;
	UCSR0B=0x08;
	UCSR0C=0x06;
	UBRR0H=0x00;
	UBRR0L=0x10;


	// Analog Comparator initialization
	// Analog Comparator: Off
	// Analog Comparator Input Capture by Timer/Counter 1: Off
	// Analog Comparator Output: Off
	ACSR=0x80;
	SFIOR=0x00;
}
