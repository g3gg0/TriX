#ifndef __TRIX_FATE_C__
#define __TRIX_FATE_C__



#include "defines.h"
#include "seer.h"
#include "segment.h"
#include "stage.h"
#include "file_io.h"
#include "file.h"
#include "workspace.h"
#include "util.h"
#include "options.h"

#include "mem.h"

#include "treenode.h"

#include "trixplug.h"

#include "fat.h"
#include "fattime.h"

#include "fate.h"

// include all the TriXPluG function/init stuff
TRIXPLUG_STUBS
#include "trixplug_wrapper.h"

int sectorsize = 512;
t_workspace *workspace = NULL;


OPT_BEGIN
//	OPTION( OPT_STR,  "String Test Variable", "teststring", &teststring )
OPT_FINISH

PLUGIN_INFO_BEGIN
	PLUGIN_NAME ( "FATe" )
	PLUGIN_INIT ( fate_init )
	PLUGIN_OPTIONS
PLUGIN_INFO_FINISH




unsigned char ataReadSectors(	t_fat_info *info, unsigned char Drive,
								unsigned long lba,//sector number
                            	unsigned char *Buffer,
                            	unsigned long *SectorInCache //actual sector
                            )
{
	if ( !info )
		return 0;

	v_memcpy_get ( info->ws, Buffer, lba * sectorsize, sectorsize );
	return 0;
}

unsigned char ataWriteSectors(	t_fat_info *info, unsigned char Drive,
								unsigned long lba,
                            	unsigned char *Buffer)
{
	if ( !info )
		return 0;

	v_memcpy_put ( info->ws, lba * sectorsize, Buffer, sectorsize );
	return 0;
}

unsigned long ataGetSizeInSectors(t_fat_info *info )
{
	if ( !info )
		return 0;

	return v_get_size ( info->ws ) / sectorsize;
}

// Parameters: start cluster
// Returns: none
//
// Description: Print the directory started in cluster
//*****************************************************************************
void dir_serial( t_fat_info *info, unsigned long cluster)
{
	unsigned long offset=0;
	struct direntry *de = 0;	// avoid compiler warning by initializing
	struct direntry lfn_de;
	int index, aux;
	//unsigned char day, month, year, hour, minutes, seconds;
	//unsigned int time, date;

	lfn_de.deLFNstate = 0;

	printf ( "Directory of: %s\r\n", fatGetVolLabel(info));
	printf ( "\r\n" );
	for (offset=0; ; offset++)
	{
		de = (struct direntry *) fatDir(info, cluster, offset);
		if (de == NULL)
		{	
			printf ( "\r\n" );
			return;
		}
		for (index=0; index<FAT_DIRENTRIES_PER_SECTOR; index++)
		{

			if (*de->deName == SLOT_EMPTY)
			{	
				printf ( "\r\n" );
				return;
			}

			fatProcessLFN ( &lfn_de, de );

			if((*de->deName != SLOT_DELETED) && (de->deAttributes != ATTR_LONG_FILENAME))
			{
				if ( de->deAttributes & ATTR_DIRECTORY )
					printf ( "[DIR]           " );
				else
					printf ( "[FILE] %8i ", de->deFileSize );

				if ( de->deLFNValid )
					printf ( "%s", de->deLFNName );
				else
				{
					for (aux=0; aux<8; aux++)
						printf ( "%c", de->deName[aux]); 
					printf(" ");
					for (aux=0; aux<3; aux++)
						printf ( "%c", de->deExtension[aux]);
				}
				lfn_de.deLFNstate = 0;
				/*
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
				printf(" %02d:%02d:%02d", hour, minutes, seconds);*/
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
void print_hd_info ( t_fat_info *info )
{
	switch (info->prPartType)
	{
		case PART_TYPE_DOSFAT16: 	printf("Found: DOSFAT 16\r\n"); break;
		case PART_TYPE_FAT16:		printf("Found: FAT16\r\n"); 	 break;
		case PART_TYPE_FAT16LBA:	printf("Found: FAT16 LBA\r\n"); break;
		case PART_TYPE_FAT32LBA:	printf("Found: FAT32 LBA\r\n"); break;
		case PART_TYPE_FAT32:		printf("Found: FAT32\r\n");  	 break;
		default:					printf("Found: No Partition!\r\n"); break;
	}
	printf("Cluster Size     : %d\r\n", fatClusterSize(info));
	printf("First sector     : %ld\r\n", 0);
	printf("Size in Sectors  : %ld\r\n", ataGetSizeInSectors(info));
	printf("sectors/cluster  : %u\r\n", fatGetSecPerClust(info));
	printf("First Fat1 Sector: %ld\r\n", fatGetFirstFATSector(info));
	printf("First Fat2 Sector: %ld\r\n", fatGetFirstFAT2Sector(info));
	printf("First Data Sect  : %ld\r\n", fatGetFirstDataSector(info));
	printf("Number of Clust  : %ld\r\n", fatGetNumClusters(info));
	printf("Number of Sects  : %ld\r\n", ataGetSizeInSectors(info));
}



unsigned int fate_init ( )
{
    REGISTER_HEADER ( "trix_fate", FATE_HEADERS );
    FATE_SYMBOLS

	printf ( "FATe v0.1 Plugin Loaded" );

    return E_OK;
}

t_fat_info *fate_open ( t_workspace *ws, unsigned char fat_type )
{
	return fatInit ( ws, fat_type );
}

TFILE *fate_fcreate ( t_fat_info *info, unsigned char *name )
{
	return fatFcreate ( info, name );
}

TFILE *fate_fopen ( t_fat_info *info, unsigned char *name )
{
	return fatFopen ( info, name );
}

unsigned int fate_unlink ( t_fat_info *info, unsigned char *name )
{
	return fatRemove ( info, name );
}


unsigned int fate_feof ( TFILE *file )
{
	return fatFeof ( file );
}

unsigned int fate_fclose ( TFILE *file )
{
	return fatFclose ( file );
}

unsigned int fate_fputc ( TFILE *file, unsigned char c)
{
	return fatFputc ( file, c );
}

unsigned char fate_fgetc ( TFILE *file )
{
	return fatFgetc ( file );
}

unsigned int fate_fseek ( TFILE *fp, unsigned int offset, unsigned char mode )
{
	return fatFseek ( fp, offset, mode );
}

unsigned int fate_cd (  t_fat_info *info, unsigned char *name )
{
	return fatCddir( info, name );
}

unsigned int fate_rename ( t_fat_info *info, char *old_name, char *new_name )
{
	return fatRename( info, old_name, new_name );
}

unsigned int fate_fread ( unsigned char * ptr, unsigned int size, unsigned int count, TFILE *fp )
{
	unsigned int block = 0;
	unsigned int pos = 0;
	long current = 0;
	long length = 0;

	// get current position
	current = fatFtell ( fp );
	if ( current < 0 )
		PLUGIN_ERROR ( "strange position?!\n", 0 );

	// go to the end
	fatFseek ( fp, 0, SEEK_END );

	if ( fatFtell ( fp ) < 0 )
	{
		fatFseek ( fp, current, SEEK_SET );
		PLUGIN_ERROR ( "strange end position?!\n", 0 );
	}

	// and calc the number of bytes we have
	length = fatFtell ( fp ) - current;

	if ( length < 0 )
	{
		fatFseek ( fp, current, SEEK_SET );
		PLUGIN_ERROR ( "position beyond end?!\n", 0 );
	}

	// then back to the current pos
	fatFseek ( fp, current, SEEK_SET );

	block = 0;
	while ( block < count && length >= size )
	{
		pos = 0;
		while ( pos < size )
		{
			if ( fatFeof ( fp ) )
				PLUGIN_ERROR ( "feof when we shouldnt get it?!\n", 0 );
			ptr[block*size + pos] = fatFgetc ( fp );
			pos++;
		}
		length -= size;
		block++;
	}

	return block;
}

unsigned int fate_fwrite ( unsigned char * ptr, unsigned int size, unsigned int count, TFILE *fp )
{
	unsigned int pos = 0;

	while ( pos < size*count )
		fatFputc ( fp, ptr[pos++] );

	fatFflush ( fp );

	return count;
}

unsigned int fate_test ( t_workspace *ws )
{
	TFILE *farq;
	t_fat_info *info;
	char path[12];

	workspace = ws;
	
	info = fatInit ( ws, PART_TYPE_FAT16LBA );
	
	print_hd_info ( info );
	//***********************************************
	// fatDir usage example
	printf("\r\nC:\\>dir *.*\r\n");
	dir_serial(info, fatGetFirstDirCluster(info));
	//***********************************************
/*
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
*/
	strcpy(path, "");


	//***********************************************
	// fatFcreate usage example
	printf ("\r\nC:\\%s>make readme.txt\r\n", path);
	if (fatFcreate(info, "readme.txt"))
	{
		printf ("\r\nFile readme.txt created suscefuly\r\n");
		printf ("\r\nC:\\%s>dir *.*\r\n", path);
		dir_serial(info, fatGetCurDirCluster(info ));
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
	if (farq= fatFopen(info, "readme.txt"))
	{
		printf("\r\nfile %s, opened to write\r\n", path);
		printf ("\r\nC:\\%s>put \"Testing 123\" in  readme.txt\r\n", path);

		fatFputc(farq, 'T');
		fatFputc(farq, 'e');
		fatFputc( farq, 's');
		fatFputc( farq, 't');
		fatFputc( farq, 'i');
		fatFputc( farq, 'n');
		fatFputc( farq, 'g');
		fatFputc( farq, ' ');
		fatFputc( farq, '1');
		fatFputc( farq, '2');
		fatFputc( farq, '3');

		printf ("\r\nWriting file...\r\n");
		fatFflush( farq);
		fatFseek( farq, 0, SEEK_SET);

		printf ("\r\nReading file...\r\n");
		while(!fatFeof( farq))
			printf("%c", fatFgetc( farq));

		printf ("\r\nClosing file...\r\n");
		fatFclose( farq);
		printf ("\r\nFile readme.txt closed.\r\n");
	}
	else
		printf("\r\nError opening readme.txt\r\n");
	//***********************************************


	printf ( "READ TEST\r\n" );
	printf ( "READ TEST\r\n" );
	printf ( "READ TEST\r\n" );

	//***********************************************
	// fatFopen usage example
	// fatFputc usage example
	// fatFgetc usage example
	// fatFflush usage example
	// fatFseek usage example
	// fatFclose usage example
	printf ("\r\nC:\\%s>open readme.txt\r\n", path);
	if (farq= fatFopen(info, "readme.txt"))
	{
		printf ("\r\nReading file...\r\n");
		while(!fatFeof( farq))
			printf("%c", fatFgetc( farq));

		printf ("\r\nClosing file...\r\n");
		fatFclose( farq);
		printf ("\r\nFile readme.txt closed.\r\n");
	}
	else
		printf("\r\nError opening readme.txt\r\n");
	//***********************************************

	return E_OK;
}


unsigned int fate_dir ( t_fat_info *info )
{
	dir_serial(info, info->currentDirCluster /*fatGetFirstDirCluster(info)*/);
	return E_OK;
}



#endif
