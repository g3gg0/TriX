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

//int sectorsize = 512;
t_workspace *workspace = NULL;
unsigned int force_fat16 = 1;
unsigned int fate_silent = 0;


OPT_BEGIN
   OPTION( OPT_BOOL, "Force FAT16", "force_fat16", &force_fat16 )
   OPTION( OPT_BOOL, "Silent", "fate_silent", &fate_silent )
OPT_FINISH

PLUGIN_INFO_BEGIN
	PLUGIN_NAME ( "FATe" )
	PLUGIN_INIT ( fate_init )
	PLUGIN_OPTIONS
PLUGIN_INFO_FINISH



// Parameters: start cluster
// Returns: none
//
// Description: Print the directory started in cluster
//*****************************************************************************

unsigned int fate_dir ( t_fat_info *info )
{
	unsigned long cluster = info->currentDirCluster;
	unsigned long offset = 0;
	struct direntry *de = 0;	// avoid compiler warning by initializing
	struct direntry lfn_de;
	int index, aux;
	//unsigned char day, month, year, hour, minutes, seconds;
	//unsigned int time, date;

	lfn_de.deLFNstate = 0;
	lfn_de.deLFNstate = 0;

	printf ( "Directory of: %s\r\n", fatGetVolLabel(info));
	printf ( "\r\n" );
	for (offset=0; ; offset++)
	{
		de = (struct direntry *) fatDir(info, cluster, offset);
		if (de == NULL)
		{	
			printf ( "\r\n" );
			return E_FAIL;
		}
		for (index=0; index<FAT_DIRENTRIES_PER_SECTOR; index++)
		{
			if (*de->deName == SLOT_EMPTY)
			{	
				printf ( "\r\n" );
				return E_OK;
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

	return E_OK;
}

void fate_free_name( unsigned char *name )
{
	free ( name );
}

// Parameters: info, file number
// Returns: filename
//
// Description: return the name of a file or dir identified by its index. 
//              caller must free that allocated string by calling fate_free_name
//*****************************************************************************
unsigned char *fate_get_name( t_fat_info *info, unsigned char type, unsigned int entry_index )
{
	unsigned long cluster = info->currentDirCluster;
	unsigned long offset = 0;
	struct direntry *de = 0;	// avoid compiler warning by initializing
	struct direntry lfn_de;
	int index, aux;

	int file_entry = 0;
	int dir_entry = 0;
	int found = 0;

	lfn_de.deLFNstate = 0;

	for (offset=0; ; offset++)
	{
		de = (struct direntry *) fatDir(info, cluster, offset);
		if (de == NULL)
			return NULL;

		for (index=0; index<FAT_DIRENTRIES_PER_SECTOR; index++)
		{

			if (*de->deName == SLOT_EMPTY)
				return NULL;

			fatProcessLFN ( &lfn_de, de );

			if((*de->deName != SLOT_DELETED) && (de->deAttributes != ATTR_LONG_FILENAME))
			{
				if ( de->deAttributes & ATTR_DIRECTORY )
				{
					if ( type == TYPE_DIR && dir_entry == entry_index )
						found = 1;
					dir_entry++;
				}
				else
				{
					if ( type == TYPE_FILE && file_entry == entry_index )
						found = 1;
					file_entry++;
				}

				if ( found )
				{
					HEAP_CHECK;
					if ( de->deLFNValid )
						return strdup(de->deLFNName);
					else
						return strdup(de->deNameExt);
				}
				lfn_de.deLFNstate = 0;
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
		case PART_TYPE_FAT12:		printf("Found             : FAT12\r\n"); 	 break;
		case PART_TYPE_DOSFAT16: 	printf("Found             : DOS-FAT16\r\n"); break;
		case PART_TYPE_FAT16:		printf("Found             : FAT16\r\n"); 	 break;
		case PART_TYPE_FAT16LBA:	printf("Found             : FAT16 LBA\r\n"); break;
		case PART_TYPE_FAT32LBA:	printf("Found             : FAT32 LBA\r\n"); break;
		case PART_TYPE_FAT32:		printf("Found             : FAT32\r\n");  	 break;
		default:					printf("Found             : No Partition!\r\n"); return; 
	}
	printf("Label             : %s\r\n", fatGetVolLabel(info));
	printf("\r\n");
	printf("First sector      : %ld\r\n", 0);
	printf("First FAT1 Sector : %ld\r\n", fatGetFirstFATSector(info));
	printf("First FAT2 Sector : %ld\r\n", fatGetFirstFAT2Sector(info));
	printf("First Data Sect   : %ld\r\n", fatGetFirstDataSector(info));
	printf("\r\n");
	printf("Sectors/Cluster   : %u\r\n", fatGetSecPerClust(info));
	printf("Cluster Size      : %u\r\n", fatClusterSize(info));
	printf("Sector Size       : %u\r\n", fatSectorSize(info));
	printf("Number of Clusters: %u\r\n", fatGetNumClusters(info));
	printf("\r\n");
	printf("Total Size        : %ld\r\n", fatGetNumClusters(info) * fatGetSecPerClust(info) * fatSectorSize(info));
	printf("Image Size        : %ld\r\n", v_get_size ( info->ws ));
}



unsigned int fate_init ( )
{
    REGISTER_HEADER ( "trix_fate", FATE_HEADERS );
    FATE_SYMBOLS

	printf ( "FATe v0.1 Plugin Loaded" );

    return E_OK;
}

t_fat_info *fate_open ( t_workspace *ws, unsigned int start_address )
{
	t_fat_info *info = NULL;

	info = fatInit ( ws, start_address );

	if ( info != NULL && !fate_silent )
		print_hd_info ( info );

	return info;
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
		PLUGIN_ERROR ( "Could not get position in file. Maybe the FAT is corrupt.\n", 0 );

	// go to the end
	fatFseek ( fp, 0, SEEK_END );

	if ( fatFtell ( fp ) < 0 )
	{
		fatFseek ( fp, current, SEEK_SET );
		PLUGIN_ERROR ( "Could not get file end position. Maybe the FAT is corrupt.\n", 0 );
	}

	// and calc the number of bytes we have
	length = fatFtell ( fp ) - current;

	if ( length < 0 )
	{
		fatFseek ( fp, current, SEEK_SET );
		PLUGIN_ERROR ( "Could not get file end position. Maybe the FAT is corrupt.\n", 0 );
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


#endif
