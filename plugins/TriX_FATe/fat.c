//*****************************************************************************
//
// Title		: FAT16/32 file system driver for ATMEL AVR
// Authors		: Angelo Bannack, Giordano Bruno Wolaniuk
// File Name	: 'fat.c'
// Date			: April 26, 2004
// Version		: 1.00
// Target MCU	: Atmel AVR Series
// Editor Tabs	: 4
//
// NOTE: This code is based on original library "FAT16/32 file system driver"
//       from Pascal Stang.
//
//	     The authors in no way will be responsible for damages that you coul'd be using this code.
//       Use this code at your own risk.
//
//       This code is distributed under the GNU Public License
//       which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#include <string.h>

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

#include "trixplug.h"

extern int dump_chunks;
extern int recompress;
extern int bitmask_depth;
extern int endianess_be;
extern struct trix_functions *ft;

#include "trixplug_wrapper.h"

#include "fat.h"
#include "fatconf.h"
#include "fattime.h"
#include "fate.h"

// globals
u8 SectorBuffer[8192];	// Sector Buffer
u8 FatCache[8192];		// Fat Buffer

//struct partrecord PartInfo;					// Partition Information
TFILE File;									// File Information

#define DRIVE0    0

//*****************************************************************************
// Function: fatClustToSect
// Parameters: cluster
// Returns: sector
//
// Description: Converts Sectors in Clusters
//*****************************************************************************
u32 fatClustToSect ( t_fat_info *info, u32 clust)
{
	// requested root dir?
	if (clust < 2)
		return (info->FirstDataSector - (info->bpb->bpbRootDirEnts)/DIRENTRIES_PER_SECTOR);

	return ((clust-2) * info->SectorsPerCluster) + info->FirstDataSector;
}


//*****************************************************************************
// Function: fatSectToClust
// Parameters: sector
// Returns: cluster
//
// Description: Converts Clusters in Sectors
//*****************************************************************************
u32 fatSectToClust( t_fat_info *info, u32 sect)
{
	// dir cluster requested
	if ( sect < info->FirstDataSector )
		return 0;
	return (((sect - info->FirstDataSector) / info->SectorsPerCluster) + 2);
}


//*****************************************************************************
// Function: fatInit
// Parameters: none
// Returns: TRUE if a FAT drive was found, FALSE otherwise
//
// Description: Get FAT info from ATA dispositive and initialize internal variables
//*****************************************************************************
t_fat_info *fatInit ( t_workspace *workspace, unsigned char prPartType )
{
	t_fat_info *info = malloc ( sizeof ( t_fat_info ) );
	struct bpb710 *bpb = malloc ( sizeof ( struct bpb710 ) );

	info->bpb = bpb;
	info->ws = workspace;

	bpb->bpbBytesPerSec = v_get_h ( workspace, 0x0B );
	bpb->bpbSecPerClust  = v_get_b ( workspace, 0x0D );
	bpb->bpbResSectors  = v_get_h ( workspace, 0x0E );
	bpb->bpbFATs  = v_get_b ( workspace, 0x10 );
	bpb->bpbRootDirEnts  = v_get_h ( workspace, 0x11 );
	bpb->bpbSectors  = v_get_h ( workspace, 0x13 );
	bpb->bpbMedia  = v_get_b ( workspace, 0x15 );
	bpb->bpbFATsecs  = v_get_h ( workspace, 0x16 );
	bpb->bpbSecPerTrack  = v_get_h ( workspace, 0x18 );
	bpb->bpbHeads  = v_get_h ( workspace, 0x1A );
	bpb->bpbHiddenSecs  = v_get_w ( workspace, 0x1C );
	bpb->bpbHugeSectors  = v_get_w ( workspace, 0x20 );

	bpb->bpbBigFATsecs  = v_get_w ( workspace, 0x24 );
	bpb->bpbExtFlags  = v_get_h ( workspace, 0x28 );

	info->prPartType = prPartType;

	// setup global disk constants
	info->FirstDataSector	= 0;
	if(bpb->bpbFATsecs)
	{
		// bpbFATsecs is non-zero and is therefore valid
		info->FirstDataSector	+= bpb->bpbResSectors + bpb->bpbFATs * bpb->bpbFATsecs;
	}
	else
	{
		// bpbFATsecs is zero, real value is in bpbBigFATsecs
		info->FirstDataSector	+= bpb->bpbResSectors + bpb->bpbFATs * bpb->bpbBigFATsecs;
	}
	info->SectorsPerCluster		= bpb->bpbSecPerClust;
	info->FirstFATSector		= bpb->bpbResSectors + 0;
	info->FatSectors			= bpb->bpbBigFATsecs;
	info->NumClusters			= (v_get_size ( workspace ) /BYTES_PER_SECTOR )/(bpb->bpbSecPerClust);

	// initialize Volume Label

	switch ( prPartType )
	{
		case PART_TYPE_DOSFAT16:
		case PART_TYPE_FAT16:
		case PART_TYPE_FAT16LBA:
			// first directory cluster is 2 by default (clusters range 2->big)
			info->FirstDirCluster	= CLUST_FIRST;
			// push data sector pointer to end of root directory area
			info->FirstDataSector += (bpb->bpbRootDirEnts)/DIRENTRIES_PER_SECTOR;
			info->Fat32Enabled = FALSE;
			v_memcpy_get ( workspace, info->VolLabel, 0x2B, 11 );
			info->VolLabel[11]='\0';
			break;
		case PART_TYPE_FAT32LBA:
		case PART_TYPE_FAT32:
			// bpbRootClust field exists in FAT32 bpb710, but not in lesser bpb's
			info->FirstDirCluster = bpb->bpbRootClust;
			// push data sector pointer to end of root directory area
			// need this? FirstDataSector += (bpb->bpbRootDirEnts)/DIRENTRIES_PER_SECTOR;
			info->Fat32Enabled = TRUE;
			v_memcpy_get ( workspace, info->VolLabel, 0x47, 11 );
			info->VolLabel[11]='\0';
			break;
		default:
			//Found: No Partition!
			return NULL;
			break;
	}

	if (info->Fat32Enabled)
		info->FirstFAT2Sector = info->FirstFATSector + bpb->bpbBigFATsecs;
	else
		info->FirstFAT2Sector = info->FirstFATSector + bpb->bpbFATsecs;
	info->FSInfo = bpb->bpbFSInfo + 0;

	info->currentDirCluster = info->FirstDirCluster;

	return info;
}


//*****************************************************************************
// Function: fatNextCluster
// Parameters: cluster
// Returns: the next cluster
//
// Description: Find next cluster in the FAT chain
//*****************************************************************************
u32 fatNextCluster (  t_fat_info *info, u32 cluster )
{
	u32 nextCluster;
	u32 fatMask;
	u32 fatOffset;
	u32 sector;
	u16 offset;

	// get fat offset in bytes
	if(info->Fat32Enabled)
	{
		// four FAT bytes (32 bits) for every cluster
		fatOffset = cluster << 2;
		// set the FAT bit mask
		fatMask = FAT32_MASK;
	}
	else
	{
		// two FAT bytes (16 bits) for every cluster
		fatOffset = cluster << 1;
		// set the FAT bit mask
		fatMask = FAT16_MASK;
	}

	// calculate the FAT sector that we're interested in
	sector = info->FirstFATSector + (fatOffset / BYTES_PER_SECTOR);
	// calculate offset of the our entry within that FAT sector
	offset = fatOffset % BYTES_PER_SECTOR;

	// if we don't already have this FAT chunk loaded, go get it
	v_memcpy_put ( info->ws, sector * BYTES_PER_SECTOR, NULL, BYTES_PER_SECTOR );
	v_memcpy_get ( info->ws, (u8*)FAT_CACHE_ADDR, sector * BYTES_PER_SECTOR, BYTES_PER_SECTOR );
//	ataReadSectors( info, DRIVE0, sector, (u8*)FAT_CACHE_ADDR, &(info->FatInCache));


	// read the nextCluster value
	nextCluster = (*((u32*) &((char*)FAT_CACHE_ADDR)[offset])) & fatMask;

	// check to see if we're at the end of the chain
	if (nextCluster == (CLUST_EOFE & fatMask))
		nextCluster = 0;

	return nextCluster;
}



//*****************************************************************************
// Function: fatClusterSize
// Parameters: none
// Returns: the size of a cluster in sectors
//
// Description: return the number of sectors in a disk cluster
//*****************************************************************************
u16 fatClusterSize( t_fat_info *info)
{
	return info->SectorsPerCluster;
}


//*****************************************************************************
// Function: fatGetFirstDirCluster
// Parameters: none
// Returns: the cluster number of the first dir entry in FAT
//
// Description: return the first dir entry cluster in FAT
//*****************************************************************************
u32 fatGetFirstDirCluster( t_fat_info *info)
{
	return(info->FirstDirCluster);
}



//*****************************************************************************
// Function: fatGetSecPerClust
// Parameters: none
// Returns: the number of Sectors per Cluster
//
// Description: return the number of Sectors per Cluster read in fatInit
//*****************************************************************************
u16 fatGetSecPerClust( t_fat_info *info)
{
	return info->SectorsPerCluster;
}


//*****************************************************************************
// Function: fatGetFirstFATSector
// Parameters: none
// Returns: the address of the first FAT Sector
//
// Description: return the sector address of the first FAT
//*****************************************************************************
u32 fatGetFirstFATSector( t_fat_info *info)
{
	return(info->FirstFATSector);
}


//*****************************************************************************
// Function: fatGetFirstFAT2Sector
// Parameters: none
// Returns: the address of the first second FAT Sector
//
// Description: return the sector address of the second FAT
//*****************************************************************************
u32 fatGetFirstFAT2Sector( t_fat_info *info)
{
	return(info->FirstFAT2Sector);
}


//*****************************************************************************
// Function: fatGetFirstDataSector
// Parameters: none
// Returns: the address of the first data Sector
//
// Description: return the sector address of the data in FAT
//*****************************************************************************
u32 fatGetFirstDataSector( t_fat_info *info)
{
	return(info->FirstDataSector);
}


//*****************************************************************************
// Function: fatGetNumClusters
// Parameters: none
// Returns: the total number of clusters
//
// Description: return the total number of clusters in the ata dispositive
//*****************************************************************************
u32 fatGetNumClusters( t_fat_info *info)
{
	return(info->NumClusters);
}

int
fatWriteDirentry (  t_fat_info *info, struct direntry *de )
{
	if ( !info || !de || !de->info )
		return 0;

	de->srcaddr = (de->deSector*BYTES_PER_SECTOR) + (de->deEntry*DIRENTRY_SIZE);

	// initialize memory
	v_memcpy_put ( de->info->ws, de->srcaddr, NULL, 32 );

	if ( de->deWriteRaw )
	{
		// copy raw direntry content
		v_memcpy_put ( de->info->ws, de->srcaddr + 0, de->deRaw, 0x20 );
	}
	else
	{
		v_memcpy_put ( de->info->ws, de->srcaddr + 0, de->deName, 8 );
		v_memcpy_put ( de->info->ws, de->srcaddr + 8, de->deExtension, 3 );

		v_set_b ( de->info->ws, de->srcaddr + 11, de->deAttributes );
		v_set_b ( de->info->ws, de->srcaddr + 12, de->deLowerCase );
		v_set_b ( de->info->ws, de->srcaddr + 13, de->deCHundredth );

		v_memcpy_put ( de->info->ws, de->srcaddr + 14, de->deCTime, 2 );
		v_memcpy_put ( de->info->ws, de->srcaddr + 16, de->deCDate, 2 );
		v_memcpy_put ( de->info->ws, de->srcaddr + 18, de->deADate, 2 );

		v_set_h ( de->info->ws, de->srcaddr + 20, de->deHighClust );

		v_memcpy_put ( de->info->ws, de->srcaddr + 22, de->deCDate, 2 );
		v_memcpy_put ( de->info->ws, de->srcaddr + 24, de->deADate, 2 );

		v_set_h ( de->info->ws, de->srcaddr + 26, de->deStartCluster );
		v_set_w ( de->info->ws, de->srcaddr + 28, de->deFileSize );
	}
	return 0;
}

int
fatGetDirentry ( t_fat_info *info, struct direntry *de, unsigned int sector, unsigned int entry )
{
	if ( !info || !de )
		return 0;

	de->deSector = sector;
	de->deEntry = entry;

	de->srcaddr = (sector*BYTES_PER_SECTOR) + (entry*DIRENTRY_SIZE);
	de->info = info;

	// copy raw direntry content
	v_memcpy_get ( info->ws, de->deRaw, (sector*BYTES_PER_SECTOR) + (entry*DIRENTRY_SIZE) + 0, 0x20 );
	de->deWriteRaw = 0;

	v_memcpy_get ( info->ws, de->deName, (sector*BYTES_PER_SECTOR) + (entry*DIRENTRY_SIZE) + 0, 8 );
	v_memcpy_get ( info->ws, de->deExtension, (sector*BYTES_PER_SECTOR) + (entry*DIRENTRY_SIZE) + 8, 3 );
	de->deName[8] = '\000';
	de->deExtension[3] = '\000';

	de->deAttributes = v_get_b ( info->ws, (sector*BYTES_PER_SECTOR) + (entry*DIRENTRY_SIZE) + 11 );
	de->deLowerCase = v_get_b ( info->ws, (sector*BYTES_PER_SECTOR) + (entry*DIRENTRY_SIZE) + 12 );
	de->deCHundredth = v_get_b ( info->ws, (sector*BYTES_PER_SECTOR) + (entry*DIRENTRY_SIZE) + 13 );

	v_memcpy_get ( info->ws, de->deCTime, (sector*BYTES_PER_SECTOR) + (entry*DIRENTRY_SIZE) + 14, 2 );
	v_memcpy_get ( info->ws, de->deCDate, (sector*BYTES_PER_SECTOR) + (entry*DIRENTRY_SIZE) + 16, 2 );
	v_memcpy_get ( info->ws, de->deADate, (sector*BYTES_PER_SECTOR) + (entry*DIRENTRY_SIZE) + 18, 2 );

	de->deHighClust = v_get_h ( info->ws, (sector*BYTES_PER_SECTOR) + (entry*DIRENTRY_SIZE) + 20 );

	v_memcpy_get ( info->ws, de->deCDate, (sector*BYTES_PER_SECTOR) + (entry*DIRENTRY_SIZE) + 22, 2 );
	v_memcpy_get ( info->ws, de->deADate, (sector*BYTES_PER_SECTOR) + (entry*DIRENTRY_SIZE) + 24, 2 );

	de->deStartCluster = v_get_h ( info->ws, (sector*BYTES_PER_SECTOR) + (entry*DIRENTRY_SIZE) + 26 );
	de->deFileSize = v_get_w ( info->ws, (sector*BYTES_PER_SECTOR) + (entry*DIRENTRY_SIZE) + 28 );

	de->deLFNValid = 0;

	return 1;
}


//*****************************************************************************
// Function: fatDir
// Parameters: initial cluster, offset
// Returns: On SUSCEFULL returns the sector with direntries info, otherwise
//          returns NULL
//
// Description: return the sector with direntries info starting in the cluster,
//              and with offset sectors from the begining cluster sector
//*****************************************************************************
struct direntry *fatDir( t_fat_info *info, u32 cluster, u32 offset)
{
	static struct direntry de[FAT_DIRENTRIES_PER_SECTOR];
	int pos = 0;
	u32 index;

	if ( cluster > 0 )
	{
		for ( index=0; index < offset/info->SectorsPerCluster; index++ )
		{
			cluster = fatNextCluster ( info, cluster );
			if ( !cluster )
				return NULL;
		}

		offset %= (info->SectorsPerCluster);
	}
	else
	{
		if ( offset > (info->bpb->bpbRootDirEnts/FAT_DIRENTRIES_PER_SECTOR) )
			return NULL;
	}

	while ( pos < FAT_DIRENTRIES_PER_SECTOR )
	{
		fatGetDirentry ( info, &de[pos], fatClustToSect(info,cluster) + offset, pos );
		pos++;
	}

	return de;
}


unsigned char fatLFNchecksum ( struct direntry *de )
{
	unsigned char c = 0;
	unsigned char i = 0;

	for (i = 0; i < 11; i++) 
	{
		// Rotate c to the right
		c = ((c & 0x01) ? 0x80 : 0) + (c >> 1);
		// Add ASCII character from name
		if ( i < 8 )
			c = c + de->deName[i];
		else
			c = c + de->deExtension[i-8];
	}

	return c;
}
int fatProcessLFN ( struct direntry *lfn_de, struct direntry *de )
{
LFNrestart:
	if ( !lfn_de->deLFNstate )
	{
		memset ( lfn_de->deLFNName, 0x00, 257 );
		lfn_de->deLFNValid = 0;
	}

	if ( de->deAttributes == ATTR_LONG_FILENAME )
	{
		if ( lfn_de->deLFNstate == 0 && (*de->deName & 0x40 ) )
		{
			lfn_de->deLFNstate = 1;
			lfn_de->deLFNFirstSector = de->deSector;
			lfn_de->deLFNFirstEntry = de->deEntry;
			lfn_de->deLFNcount = (*de->deName & 0x3F);
			if ( lfn_de->deLFNcount == 0 )
			{
				lfn_de->deLFNstate = 0;
				goto LFNrestart;
			}
			else
			{
				int lfn_offset = (lfn_de->deLFNcount - lfn_de->deLFNstate) * 13;

				lfn_de->deLFNchecksum = de->deRaw[13];

				lfn_de->deLFNName[lfn_offset+0] = de->deRaw[1];
				lfn_de->deLFNName[lfn_offset+1] = de->deRaw[3];
				lfn_de->deLFNName[lfn_offset+2] = de->deRaw[5];
				lfn_de->deLFNName[lfn_offset+3] = de->deRaw[7];
				lfn_de->deLFNName[lfn_offset+4] = de->deRaw[9];
				lfn_de->deLFNName[lfn_offset+5] = de->deRaw[14];
				lfn_de->deLFNName[lfn_offset+6] = de->deRaw[16];
				lfn_de->deLFNName[lfn_offset+7] = de->deRaw[18];
				lfn_de->deLFNName[lfn_offset+8] = de->deRaw[20];
				lfn_de->deLFNName[lfn_offset+9] = de->deRaw[22];
				lfn_de->deLFNName[lfn_offset+10] = de->deRaw[24];
				lfn_de->deLFNName[lfn_offset+11] = de->deRaw[28];
				lfn_de->deLFNName[lfn_offset+12] = de->deRaw[30];

			}
		}
		else if ( lfn_de->deLFNstate < lfn_de->deLFNcount )
		{

			if ( (*de->deName & 0x3F) != (lfn_de->deLFNcount - lfn_de->deLFNstate) )
			{
				lfn_de->deLFNstate = 0;
				goto LFNrestart;
			}
			else
			{
				int lfn_offset = (lfn_de->deLFNcount - ++lfn_de->deLFNstate) * 13;

				lfn_de->deLFNName[lfn_offset+0] = de->deRaw[1];
				lfn_de->deLFNName[lfn_offset+1] = de->deRaw[3];
				lfn_de->deLFNName[lfn_offset+2] = de->deRaw[5];
				lfn_de->deLFNName[lfn_offset+3] = de->deRaw[7];
				lfn_de->deLFNName[lfn_offset+4] = de->deRaw[9];
				lfn_de->deLFNName[lfn_offset+5] = de->deRaw[14];
				lfn_de->deLFNName[lfn_offset+6] = de->deRaw[16];
				lfn_de->deLFNName[lfn_offset+7] = de->deRaw[18];
				lfn_de->deLFNName[lfn_offset+8] = de->deRaw[20];
				lfn_de->deLFNName[lfn_offset+9] = de->deRaw[22];
				lfn_de->deLFNName[lfn_offset+10] = de->deRaw[24];
				lfn_de->deLFNName[lfn_offset+11] = de->deRaw[28];
				lfn_de->deLFNName[lfn_offset+12] = de->deRaw[30];
			}
		}
		else 
		{
			lfn_de->deLFNstate = 0;
			goto LFNrestart;
		}
	}
	else
	{
		if ( lfn_de->deLFNstate == lfn_de->deLFNcount )
		{
			if ( fatLFNchecksum ( de ) == lfn_de->deLFNchecksum )
			{
				de->deLFNValid = 1;
				de->deLFNFirstSector = lfn_de->deLFNFirstSector;
				de->deLFNFirstEntry = lfn_de->deLFNFirstEntry;
				memcpy ( de->deLFNName, lfn_de->deLFNName, 257 );
			}
		}
		lfn_de->deLFNstate = 0;
	}

	return 0;
}


//*****************************************************************************
// Function: fatGetFileInfo
// Parameters: address of direntry struct, short name of the file
// Returns: On SUSCEFULL, return the direntry struct filled out with the file info,
//          otherwise returns NULL;
//
// Description: return the direntry struct of a short name file given
//              this function will search for a filename only in the current
//              directory, given by currentDirCluster
//*****************************************************************************
struct direntry *fatGetFileInfo( t_fat_info *info, struct direntry *rde, char *shortName)
{
	static struct direntry de;
	static struct direntry lfn_de;
	
	u32 sector = 0;
	u32 cluster = 0;
	int offset = 0;
	int entry = 0;
	int direntries = 0;
	char Name[13];

	strncpy ( Name, shortName, 12);
	Name[12]='\0';
	fatNormalize ( info, Name ); // adjust the name to the FAT format

	lfn_de.deLFNstate = 0;

	cluster = info->currentDirCluster;	// start the search in the current cluster
	sector = fatClustToSect(info,cluster);

	while ( 1 )
	{
		// for each sector per cluster
		for ( offset = 0; offset < info->SectorsPerCluster; offset++ )
		{
			// read every dir entry
			for ( entry = 0; entry < FAT_DIRENTRIES_PER_SECTOR; entry++ )
			{
				direntries++;
				fatGetDirentry ( info, &de, sector + offset, entry );

				if (*de.deName == 0x00)
					return NULL;		// there are no more direntries

				// process LFN state machine
				fatProcessLFN ( &lfn_de, &de );
				
				if ( (*de.deName != SLOT_DELETED) && (de.deAttributes != ATTR_LONG_FILENAME ) )
				{
					// when we got a 8.3 name, then check these fields
					// else just compare the long filename
					if ( (strlen (shortName) == 11 && (!strncmp(de.deName, Name, 8) && !strncmp(de.deExtension, Name+8, 3))) 
						|| (de.deLFNValid && !strcasecmp ( shortName, de.deLFNName )  ) )
					{
						memcpy ( rde, &de, sizeof (struct direntry) );
						return &de;
					}
					else
						de.deLFNstate = 0;
				}
			}	// end of sector
		}	// end of cluster

		if ( cluster > 0 )
		{
			cluster = fatNextCluster(info,cluster);
			if ( !cluster )
				return NULL;
			sector = fatClustToSect(info,cluster);
		}
		else
		{
			if ( direntries >= info->bpb->bpbRootDirEnts )
				return NULL;
			sector += info->SectorsPerCluster;
		}
		if ( sector >= info->FirstFATSector + info->FatSectors ) 
			return NULL;

	} 

	return NULL;
}


//*****************************************************************************
// Function: fatGetVolLavel
// Parameters: none
// Returns: the FAT Volume Name
//
// Description: Return the FAT Volume Name read in fatInit
//*****************************************************************************
char* fatGetVolLabel( t_fat_info *info)
{
	return info->VolLabel;
}


//*****************************************************************************
// Function: fatCddir
// Parameters: path
// Returns: On SUSCEFULL returns TRUE, otherwise returns FALSE
//
// Description: Change the current directory. Only one level path
//              ex: cd \test
//              cd \test\test2  IS INVALID, use cd \test cd\test2 instead
//*****************************************************************************
u8 fatCddir( t_fat_info *info, char *path)
{
	struct direntry de;

	// if the path doesn't exist return FALSE
	if (fatGetFileInfo(info, &de, path) == NULL)
		return FALSE;

	// if the path is a directory
	if ((de.deAttributes & ATTR_DIRECTORY) == ATTR_DIRECTORY)
	{
		// change the current dir cluster to the path information
		info->currentDirCluster= (de.deStartCluster) + ((u32)de.deHighClust<<16);
		return TRUE;
	}
	return FALSE;
}


////////////////////
#ifndef ATA_READ_ONLY
//*****************************************************************************
// Function: fatMkdir
// Parameters: path
// Returns: TRUE if the path was created, otherwise FALSE
//
// Description: Create a new directory on the current directory
//*****************************************************************************
u8 fatMkdir( t_fat_info *info, char *path)
{
	struct direntry *de, rde;
	u32 freeCluster;
	u16 i, date, time;
	char string[13];
	TTime t;

	// if the file already exist
	if ( fatGetFileInfo ( info, &rde, path ) != NULL )
		return FALSE;

	// Find a free direntry, returns a pointer to the direntry struct inside SectorBuffer,
	de = fatNextFreeDirEntryCount ( info, info->currentDirCluster, 2 + (strlen ( path ) / 13) );
	if ( !de )
	{
		fatExtendCurrentDir ( info );
		de = fatNextFreeDirEntryCount ( info, info->currentDirCluster, 2 + (strlen ( path ) / 13) );
		if ( !de )
			return FALSE;
	}

	// find a FAT free cluster to write the directory content
	freeCluster = fatNextFreeCluster ( info, 0 );
	if ( !freeCluster )
		return FALSE;

	// fill the direntry
	strncpy ( string, path, 12 );
	string[12] = '\0';
	fatNormalize ( info, string );
	fatFindUnique ( info, string );

	fatGetCurTime ( &t );
	time = (u16)((t.hour)<<DT_HOURS_SHIFT) + ((t.minutes)<<DT_MINUTES_SHIFT) + ((t.seconds)>>DT_2SECONDS_SHIFT);		// create time
	date = (u16)((t.year-1980)<<DD_YEAR_SHIFT) + (t.month<<DD_MONTH_SHIFT) + t.day;     						// create date


	// fill structs
	memcpy(de->deName, string, 11);		    // name of the directory
	de->deName[8] = '\000';
	strncpy ( de->deLFNName, path, 256 );
	de->deLFNName[256] = '\000';

	de->deAttributes = ATTR_DIRECTORY;
	de->deLowerCase = 0;   	 			    // lowercase Names
	de->deCHundredth = 0x00;	 			// hundredth of seconds in CTime
	de->deCTime[0] = (u8)time&0x00FF;
	de->deCTime[1] = (u8)((time&0xFF00)>>8);
	de->deCDate[0] = (u8)date&0x00FF;
	de->deCDate[1] = (u8)((date&0xFF00)>>8);
	de->deADate[0] = de->deCDate[0];     	// access date
	de->deADate[1] = de->deCDate[1];     	// access date
	de->deHighClust = (freeCluster>>16); // high bytes of cluster number
	de->deMTime[0] = de->deCTime[0];     	// last update time
	de->deMTime[1] = de->deCTime[1];     	// last update time
	de->deMDate[0] = de->deCDate[0];     	// last update date
	de->deMDate[1] = de->deCDate[1];     	// last update date
	de->deStartCluster = freeCluster; 	// starting cluster of file
	de->deFileSize = 0;  					// size of file in bytes, is a directory, so the size is 0
	de->info = info;

	fatWriteLFN ( info, de, de );


	// fill the entire sector buffer with zeros and
	// write all the sectors of this new cluster.
	memset (SectorBuffer, '\000', BYTES_PER_SECTOR);
	for (i = 0; i< info->SectorsPerCluster; i++)
		v_memcpy_put ( info->ws, (fatClustToSect(info, freeCluster)+i) * BYTES_PER_SECTOR, SectorBuffer, BYTES_PER_SECTOR );

	// create the files "." and ".."
	de = (struct direntry *)SectorBuffer;
	strncpy(de->deName, ".          ", 11);
	de->deAttributes = ATTR_DIRECTORY;
	de->deLowerCase = 0;   				// lowercase Names
	de->deCHundredth = 0x00;	 			// hundredth of seconds in CTime
	de->deCTime[0] = (u8)time&0x00FF;
	de->deCTime[1] = (u8)((time&0xFF00)>>8);
	de->deCDate[0] = (u8)date&0x00FF;
	de->deCDate[1] = (u8)((date&0xFF00)>>8);	
	de->deADate[0] = de->deCDate[0];     	// access date
	de->deADate[1] = de->deCDate[1];     	// access date
	de->deHighClust = (freeCluster>>16); // high bytes of cluster number
	de->deMTime[0] = de->deCTime[0];     	// last update time
	de->deMTime[1] = de->deCTime[1];     	// last update time
	de->deMDate[0] = de->deCDate[0];     	// last update date
	de->deMDate[1] = de->deCDate[1];     	// last update date
	de->deStartCluster = freeCluster; // starting cluster of file
	de->deFileSize = 0;  					// size of file in bytes
	de->info = info;
	de->deSector = fatClustToSect(info, freeCluster);
	de->deEntry = 0;
	fatWriteDirentry ( info, de );


	// create ".."
	strncpy(de->deName, "..         ", 11);
	de->deAttributes = ATTR_DIRECTORY;
	de->deLowerCase =0;   				// lowercase Names
	de->deCHundredth = 0x00;	 			// hundredth of seconds in CTime
	de->deCTime[0] = (u8)time&0x00FF;
	de->deCTime[1] = (u8)((time&0xFF00)>>8);
	de->deCDate[0] = (u8)date&0x00FF;
	de->deCDate[1] = (u8)((date&0xFF00)>>8);
	de->deADate[0] = de->deCDate[0];     	// access date
	de->deADate[1] = de->deCDate[1];     	// access date
	de->deHighClust = (info->currentDirCluster>>16); // high bytes of cluster number
	de->deMTime[0] = de->deCTime[0];     	// last update time
	de->deMTime[1] = de->deCTime[1];     	// last update time
	de->deMDate[0] = de->deCDate[0];     	// last update date
	de->deMDate[1] = de->deCDate[1];     	// last update date
	de->deStartCluster = info->currentDirCluster; // starting cluster of file
	de->deFileSize = 0;  					// size of file in bytes
	de->info = info;
	de->deSector = fatClustToSect(info, freeCluster);
	de->deEntry = 1;

	fatWriteDirentry ( info, de );


	// mark the cluster with the content off the new directory created with an END OF CLUSTER mark
	fatWriteEOC(info, freeCluster);

	return TRUE;
}
#endif
////////////////////



////////////////////
#ifndef ATA_READ_ONLY
//*****************************************************************************
// Function: fatFcreate
// Parameters: File short name
// Returns: A TFILE struct filled with the created file information, or NULL
//          if an error has ocurred
//
// Description: Create a file and open it in the current directory
//*****************************************************************************
TFILE* fatFcreate( t_fat_info *info, char *shortName)
{
	int entries = 0;
	struct direntry *de, rde;
	u32 freeCluster;
	u16 date, time;
	char string[13];
	TTime t;

	// if the file already exist
	if ( fatGetFileInfo(info, &rde, shortName) != NULL )
		return NULL;

	entries = 2 + (strlen ( shortName ) / 13);

	// Find a free direntry, returns a pointer to the direntry struct inside SectorBuffer,
	de = fatNextFreeDirEntryCount ( info, info->currentDirCluster, entries );

	if ( !de )
		fatExtendCurrentDir ( info );

	de = fatNextFreeDirEntryCount ( info, info->currentDirCluster, entries );
	if ( !de )
		return NULL;

	// find a free fat cluster to put the content of this new file
	freeCluster = fatNextFreeCluster(info, 0);

	// fill with file data information
	strncpy ( string, shortName, 12 );
	string[12]='\0';
	fatNormalize ( info, string );
	fatFindUnique ( info, string );


	fatGetCurTime(&t);
	time= (u16)((t.hour)<<DT_HOURS_SHIFT) + ((t.minutes)<<DT_MINUTES_SHIFT) + ((t.seconds)>>DT_2SECONDS_SHIFT);		// create time
	date= (u16)((t.year-1980)<<DD_YEAR_SHIFT) + (t.month<<DD_MONTH_SHIFT) + t.day;     						// create date

	memcpy ( de->deName, string, 8 );
	de->deName[8] = '\000';
	memcpy ( de->deExtension, string+8, 3 );
	de->deExtension[3] = '\000';
	strncpy ( de->deLFNName, shortName, 256 );
	de->deLFNName[256] = '\000';

	de->deAttributes = ATTR_ARCHIVE;
	de->deLowerCase = 0;   				// lowercase Names
	de->deCHundredth = 0x00;				// hundredth of seconds in CTime

	de->deCTime[0]=(u8)time&0x00FF;
	de->deCTime[1]=(u8)((time&0xFF00)>>8);
	de->deCDate[0]=(u8)date&0x00FF;
	de->deCDate[1]=(u8)((date&0xFF00)>>8);
	de->deADate[0]=de->deCDate[0];     	// access date
	de->deADate[1]=de->deCDate[1];     	// access date
	de->deHighClust= (freeCluster>>16); // high bytes of cluster number
	de->deMTime[0]=de->deCTime[0];     	// last update time
	de->deMTime[1]=de->deCTime[1];     	// last update time
	de->deMDate[0]=de->deCDate[0];     	// last update date
	de->deMDate[1]=de->deCDate[1];     	// last update date
	de->deStartCluster= freeCluster; 	// starting cluster of file
	de->deFileSize=0;  					// size of file in bytes

	fatWriteLFN ( info, de, de );

	// mark the cluster with the content off the new directory created with an END OF CLUSTER mark
	fatWriteEOC(info, freeCluster);

	// open the file to read and write
	return(fatFopen(info, shortName));
}
#endif
////////////////////


////////////////////
#ifndef ATA_READ_ONLY
//*****************************************************************************
// Function: fatRename
// Parameters: old name, new name
// Returns: TRUE if the name was chanched, otherwise FALSE
//
// Description: Change the name of a directory or file
//*****************************************************************************
u8 fatRename( t_fat_info *info, char *oldShortName, char *newShortName)
{
	int done = 0;
	struct direntry *new_de, rde, lfn_de;
	struct direntry source_de;
	char string[13];
	int delete_entry = 0;
	int old_entries = 0;
	int new_entries = 0;

	// if the new file name already exists
	if ( fatGetFileInfo(info, &rde, newShortName) != NULL)
		return FALSE;

	// if the file doesn't exist
	if ( fatGetFileInfo(info, &source_de, oldShortName) == NULL)
		return FALSE;

	strncpy ( string, newShortName, 12);
	string[12]='\0';
	fatNormalize ( info, string );
	fatFindUnique ( info, string );

	// find free direntries, mark the file name as deleted

	// first allocate N direntries for writing LFN plus file
	new_entries = 2 + (strlen ( newShortName ) / 13);
	old_entries = 2 + (strlen ( oldShortName ) / 13);

	// needed entry count changed, reallocate
	if ( ( source_de.deLFNValid && new_entries != old_entries ) || !source_de.deLFNValid )
		delete_entry = 1;

	// we have to reallocate
	if ( delete_entry )
	{
		// get new dir entry
		new_de = fatNextFreeDirEntryCount ( info, fatSectToClust(info, source_de.deSector), new_entries );

		if ( !new_de )
		{
			fatExtendCurrentDir ( info );
			new_de = fatNextFreeDirEntryCount ( info, fatSectToClust(info, source_de.deSector), new_entries );
			if ( !new_de )
				return FALSE;
		}

		// free entries found, delete old entries
		lfn_de.deEntry = source_de.deLFNFirstEntry;
		lfn_de.deSector = source_de.deLFNFirstSector;

		while ( !done )
		{
			fatGetDirentry ( info, &lfn_de, lfn_de.deSector, lfn_de.deEntry );
			lfn_de.deName[0] = SLOT_DELETED;
			fatWriteDirentry ( info, &lfn_de );

			if ( lfn_de.deEntry >= source_de.deEntry && lfn_de.deSector >= source_de.deSector )
				done = 1;
			else
			{
				lfn_de.deEntry++;
				if ( lfn_de.deEntry >= DIRENTRIES_PER_SECTOR )
				{
					lfn_de.deEntry = 0;
					lfn_de.deSector++;
				}
			}
		}
	}
	else
	{
		fatGetDirentry ( info, &rde, source_de.deLFNFirstSector, source_de.deLFNFirstEntry );
		new_de = &rde;
	}

	memcpy ( source_de.deName, string, 8 );
	memcpy ( source_de.deExtension, string+8, 3 );
	strncpy ( source_de.deLFNName, newShortName, 256 );
	source_de.deLFNName[256] = '\000';

	fatWriteLFN ( info, &source_de, new_de );

	return TRUE;

}

int fatWriteLFN ( t_fat_info *info, struct direntry *src_de, struct direntry *dst_de )
{
	int entries = 0;
	int entry_pos = 0;
	int done = 0;
	struct direntry de;

	if ( !info || !src_de || !dst_de )
		return 0;

	entries = 1 + (strlen ( src_de->deLFNName ) / 13);

	while ( !done )
	{
		fatGetDirentry ( info, &de, dst_de->deSector, dst_de->deEntry );

		de.deWriteRaw = 1;
		memset ( de.deRaw, 0x00, 0x20 );

		if ( !entry_pos )
			de.deRaw[0] = 0x40 | (entries);
		else
			de.deRaw[0] = (entries-entry_pos);

		de.deRaw[0x0b] = 0x0f;

		de.deRaw[0x0d] = fatLFNchecksum ( src_de );

		de.deRaw[1] = src_de->deLFNName[(entries-entry_pos-1)*13 + 0];
		de.deRaw[3] = src_de->deLFNName[(entries-entry_pos-1)*13 + 1];
		de.deRaw[5] = src_de->deLFNName[(entries-entry_pos-1)*13 + 2];
		de.deRaw[7] = src_de->deLFNName[(entries-entry_pos-1)*13 + 3];
		de.deRaw[9] = src_de->deLFNName[(entries-entry_pos-1)*13 + 4];
		de.deRaw[14] = src_de->deLFNName[(entries-entry_pos-1)*13 + 5];
		de.deRaw[16] = src_de->deLFNName[(entries-entry_pos-1)*13 + 6];
		de.deRaw[18] = src_de->deLFNName[(entries-entry_pos-1)*13 + 7];
		de.deRaw[20] = src_de->deLFNName[(entries-entry_pos-1)*13 + 8];
		de.deRaw[22] = src_de->deLFNName[(entries-entry_pos-1)*13 + 9];
		de.deRaw[24] = src_de->deLFNName[(entries-entry_pos-1)*13 + 10];
		de.deRaw[28] = src_de->deLFNName[(entries-entry_pos-1)*13 + 11];
		de.deRaw[30] = src_de->deLFNName[(entries-entry_pos-1)*13 + 12];

		fatWriteDirentry ( info, &de );

		dst_de->deEntry++;
		if ( dst_de->deEntry >= DIRENTRIES_PER_SECTOR )
		{
			dst_de->deEntry = 0;
			dst_de->deSector++;
		}
		if ( entries == ++entry_pos )
			done = 1;
	}

	src_de->deEntry = dst_de->deEntry;
	src_de->deSector = dst_de->deSector;

	fatWriteDirentry ( info, src_de );

	return 0;
}

int fatExtendCurrentDir ( t_fat_info *info )
{
	struct direntry *de = NULL;

	int i = 0;
	u32 lastCluster = 0;
	u32 newDirEntrySector = 0;
	u32 freeCluster = fatNextFreeCluster(info, 0); 	// find a new free cluster

	if ( info->currentDirCluster == 0 )
		return FALSE;
	if (freeCluster == 0)				// is the disk is full
		return FALSE;					//		return ERROR

	lastCluster = fatLastCluster(info, info->currentDirCluster);	// find the last cluster of the current dir
	fatWrite ( info, lastCluster, freeCluster);						// creates a connection between the last cluster of the current directory and the new cluster created
	fatWriteEOC ( info, freeCluster);								// mark the new cluster with an END OF CLUSTER mark
	memset (SectorBuffer, '\0', BYTES_PER_SECTOR);					// fill the SectorBuffer with ZEROS to informate to the FAT that don't have more direntries after this one that we are creating
	de = (struct direntry *)SectorBuffer;							// de points to the first address in the SectorBuffer
	newDirEntrySector = fatClustToSect(info, freeCluster);			// Calculate the start sector address of this new cluster
	info->SectorInCache = newDirEntrySector;						// the sector in memory is this new direntry allocated
	for ( i=1; i< info->SectorsPerCluster; i++)						// write zeros in all the new cluster allocated
		v_memcpy_put ( info->ws, (newDirEntrySector+i) * BYTES_PER_SECTOR, SectorBuffer, BYTES_PER_SECTOR );

	return TRUE;
}

#endif
////////////////////


////////////////////
#ifndef ATA_READ_ONLY
//*****************************************************************************
// Function: fatRemove
// Parameters: File or Directory short name
// Returns: TRUE if the file was removed, otherwise FALSE
//
// Description: remove a file or directory
//*****************************************************************************
u8 fatRemove( t_fat_info *info, char *shortName)
{
	int done = 0;
	struct direntry *de, rde, lfn_de;
	u32 CurrentDirCluster, NextCluster, fileSector;


	de = fatGetFileInfo(info, &rde, shortName);	// get file information
	if (de == NULL)						// if the file doesn't exist
		return FALSE;					//	 	return ERROR

	fileSector=info->SectorInCache;			// save the File sector address

	// calculate the cluster address of this file
	CurrentDirCluster = ((u32)de->deHighClust << 16) + de->deStartCluster;

	// if the file is a directory
	if ( de->deAttributes == ATTR_DIRECTORY )
	{
		if (!fatDirectoryIsEmpty(info, CurrentDirCluster))	// if there are files in the directory
			return FALSE;								//		return ERROR
	}

	// mark the file name as deleted
	if ( de->deLFNValid )
	{
		lfn_de.deEntry = de->deLFNFirstEntry;
		lfn_de.deSector = de->deLFNFirstSector;
	}
	else
	{
		lfn_de.deEntry = de->deEntry;
		lfn_de.deSector = de->deSector;
	}

	while ( !done )
	{
		fatGetDirentry ( info, &lfn_de, lfn_de.deSector, lfn_de.deEntry );
		*lfn_de.deName = SLOT_DELETED;
		fatWriteDirentry ( info, &lfn_de );

		if ( lfn_de.deEntry >= de->deEntry && lfn_de.deSector >= de->deSector )
			done = 1;
		else
		{
			lfn_de.deEntry++;
			if ( lfn_de.deEntry >= DIRENTRIES_PER_SECTOR )
			{
				lfn_de.deEntry = 0;
				lfn_de.deSector++;
			}
		}
	}

	// erase fat table
	do{
		NextCluster = fatNextCluster ( info, CurrentDirCluster);
		fatWrite(info, CurrentDirCluster, CLUST_FREE);	// free the current dir cluster
		CurrentDirCluster = NextCluster;
	}while( NextCluster != 0 );

	//file erased
	return TRUE;
}
#endif
////////////////////



//*****************************************************************************
// Function: fatFopen
// Parameters: File or Directory short name
// Returns: A TFILE struct filled with the opened file information, or NULL
//          if an error has ocurred
//
// Description: Open a file to read and write. The File struct is filled and
//              the Sector Buffer in memory is filled with the first sector
//              of the file opened
//*****************************************************************************
TFILE* fatFopen( t_fat_info *info, char *shortName)
{
	TFILE *file = NULL;

	file = malloc ( sizeof ( TFILE ) );

	// if the file don't exist
	if ( fatGetFileInfo(info, &file->de, shortName ) == NULL )
	{
		free ( file );
		return fatFcreate ( info, shortName );
	}

	file->currentSector = fatClustToSect(info, ((u32)file->de.deHighClust << 16) + file->de.deStartCluster);
	file->buffer = malloc ( BYTES_PER_SECTOR );
	file->bytePointer = 0;
	file->sectorHasChanged = FALSE;
	file->info = info;

	// initialize if not existing
	v_memcpy_put ( info->ws, file->currentSector*BYTES_PER_SECTOR, NULL, BYTES_PER_SECTOR );
	v_memcpy_get ( info->ws, file->buffer, file->currentSector*BYTES_PER_SECTOR, BYTES_PER_SECTOR );

	return file;
}


////////////////////
#ifndef ATA_READ_ONLY
//*****************************************************************************
// Function: fatFclose
// Parameters: TFILE struct of the file to be closed
// Returns: TRUE if the file was corrected closed, and FALSE otherwise
//
// Description: write current file to the FAT file system
//              refresh the file size
//*****************************************************************************
u8 fatFclose( TFILE *fp)
{
	struct direntry *de, rde;
	u32 fileSize;
	u16 date, time;
	TTime t;

	fileSize= fp->de.deFileSize;
	fp->bytePointer=0;
	if (fatFflush ( fp))
	{
		if ((de=fatGetFileInfo(fp->info, &rde, fp->de.deName)) == NULL) // if the file don't exist
			return FALSE;
		de->deFileSize= fileSize; // refresh the file size
		fatGetCurTime(&t);
		time= (u16)((t.hour)<<DT_HOURS_SHIFT) + ((t.minutes)<<DT_MINUTES_SHIFT) + ((t.seconds)>>DT_2SECONDS_SHIFT);		// create time
		date= (u16)((t.year-1980)<<DD_YEAR_SHIFT) + (t.month<<DD_MONTH_SHIFT) + t.day;     						// create date
		de->deMTime[0]=(u8)time&0x00FF;			// last update time
		de->deMTime[1]=(u8)((time&0xFF00)>>8);	// last update time
		de->deMDate[0]=(u8)date&0x00FF;			// last update date
		de->deMDate[1]=(u8)((date&0xFF00)>>8);	// last update date
		de->deADate[0]=de->deMDate[0];     					// access date
		de->deADate[1]=de->deMDate[1];     					// access date

		fatWriteDirentry ( fp->info, de );

		free ( fp->buffer );
		free ( fp );
		return(TRUE);
	}
	return (FALSE);
}
#endif
////////////////////



////////////////////
#ifndef ATA_READ_ONLY
//*****************************************************************************
// Function: fatFflush
// Parameters: TFILE struct of the file to be flushed
// Returns: TRUE if the file was corrected flushed, and FALSE otherwise
//
// Description: write current sector file to hard disk if necessary
//*****************************************************************************
u8 fatFflush( TFILE *fp )
{
	if ( fp->sectorHasChanged )
	{
		v_memcpy_put ( fp->info->ws, fp->currentSector * BYTES_PER_SECTOR, fp->buffer, BYTES_PER_SECTOR );
		fp->sectorHasChanged=FALSE;
	}
	return TRUE;
}
#endif
////////////////////


long fatFtell ( TFILE *fp )
{
	if ( !fp )
		return -1;

	return fp->bytePointer;
}


//*****************************************************************************
// Function: fatFseek
// Parameters: TFILE struct of the file opened, offSet, seek mode
// Returns: On SUSCEFULL returns TRUE, and FALSE otherwise
//
// Description: find a byte position in the file and load the corresponded sector in buffer
//              Modes: SEEK_CUR: from the current position of the file pointer;
//                     SEEK_SET: from the beggining of the file
//                     SEEK_END: from the end of file to back.
//*****************************************************************************
u16 fatFseek( TFILE *fp, u32 offSet, u8 mode)
{

	u32 numClusters, curCluster, numSector;

	// calculate the new byte pointer
	switch (mode)
	{
		case SEEK_END: fp->bytePointer= fp->de.deFileSize - offSet;	break;
		case SEEK_SET: fp->bytePointer= offSet;                     break;
		case SEEK_CUR: fp->bytePointer= fp->bytePointer + offSet;   break;
		default: return FALSE;
	}

	// calculate the current file cluster
	curCluster=((u32)fp->de.deHighClust<<16)+(fp->de.deStartCluster);
	numClusters=fp->bytePointer/(BYTES_PER_SECTOR * (fp->info->SectorsPerCluster));
	numSector=(fp->bytePointer/BYTES_PER_SECTOR) % (fp->info->SectorsPerCluster);

	// calculate the cluster address of the new byte pointer
	while (numClusters > 0)
	{
		curCluster = fatNextCluster(fp->info, curCluster);
		if ( !curCluster )
			return (FALSE);
		numClusters--;
	}

	// calculate the Sector address of the new byte pointer
	fp->currentSector = fatClustToSect(fp->info, curCluster) + numSector;

	// initialize if not existing
	v_memcpy_put ( fp->info->ws, fp->currentSector*BYTES_PER_SECTOR, NULL, BYTES_PER_SECTOR );
	// read that sector
	v_memcpy_get ( fp->info->ws, fp->buffer, fp->currentSector*BYTES_PER_SECTOR, BYTES_PER_SECTOR );

	return TRUE;
}


//*****************************************************************************
// Function: fatFgetc
// Parameters: TFILE struct of the file opened
// Returns: On SUSCEFULL returns the next character from file, and 0 otherwise
//
// Description: Get the next character from file, and actualize the byte pointer
//*****************************************************************************
unsigned char fatFgetc( TFILE *fp )
{
	unsigned char retval = 0;
	u16 bufferPointer;

	if (fatFeof(fp))	// if is the end of file
		return 0;

	// first get the char to return
	bufferPointer = fp->bytePointer % BYTES_PER_SECTOR;

	// increase position
	fp->bytePointer++;
	retval = fp->buffer[bufferPointer];

	// then check for sector end
	bufferPointer = fp->bytePointer % BYTES_PER_SECTOR;
	if ( !bufferPointer )
	{
		// Next Sector is in current Cluster
		if (fatSectToClust ( fp->info, fp->currentSector ) == fatSectToClust ( fp->info, fp->currentSector + 1 ) )
		{
			fp->currentSector++;
		}
		else // Next Sector is in next Cluster
		{
			u32 cluster;

			cluster= fatNextCluster(fp->info, fatSectToClust(fp->info, fp->currentSector));
			if ( !cluster )
				return 0;
			fp->currentSector= fatClustToSect(fp->info, cluster);
		}
		// initialize if not existing
		v_memcpy_put ( fp->info->ws, fp->currentSector*BYTES_PER_SECTOR, NULL, BYTES_PER_SECTOR );
		// read that sector
		v_memcpy_get ( fp->info->ws, fp->buffer, fp->currentSector * BYTES_PER_SECTOR, BYTES_PER_SECTOR );
	}

	return retval;
}



////////////////////
#ifndef ATA_READ_ONLY
//*****************************************************************************
// Function: fatFputc
// Parameters: TFILE struct of the file opened, character to be writed
// Returns: On SUSCEFULL returns TRUE, and FALSE otherwise
//
// Description: Write a character to the file. Return false if the disk is full
//*****************************************************************************
u8 fatFputc( TFILE *fp, unsigned char c )
{
	u16 bufferPointer;
	u32 freeCluster;

	bufferPointer= fp->bytePointer % 512;
	fp->bytePointer++;

	fp->buffer[bufferPointer] = c;
	fp->sectorHasChanged = TRUE;

	if ( fp->bytePointer > fp->de.deFileSize )
		fp->de.deFileSize = fp->bytePointer;

	bufferPointer= fp->bytePointer % 512;

	if ( !bufferPointer )	// if we need to allocate a new sector to the file
	{
		v_memcpy_put ( fp->info->ws, fp->currentSector * BYTES_PER_SECTOR, fp->buffer, BYTES_PER_SECTOR );

		if( fp->de.deFileSize > fp->bytePointer )	// sector already exist
		{
			// Next Sector is in current Cluster
			if (fatSectToClust(fp->info, fp->currentSector) == fatSectToClust(fp->info, fp->currentSector+1))
			{
				fp->currentSector++;
			}
			else // Next Sector is in next Cluster
			{
				u32 cluster;

				cluster = fatNextCluster(fp->info, fatSectToClust(fp->info, fp->currentSector) );
				if ( !cluster )
					return (FALSE);
				fp->currentSector= fatClustToSect(fp->info, cluster);
			}
			// initialize if not existing
			v_memcpy_put ( fp->info->ws, fp->currentSector*BYTES_PER_SECTOR, NULL, BYTES_PER_SECTOR );
			v_memcpy_get ( fp->info->ws, fp->buffer, fp->currentSector * BYTES_PER_SECTOR, BYTES_PER_SECTOR );
		}
		else 	// sector isn´t used or the cluster isn´t allocated
		{
			if (fatSectToClust(fp->info, fp->currentSector) == fatSectToClust(fp->info, fp->currentSector+1))//the next sector is in current cluster
			{
				fp->currentSector++;
			}
			else
			{
				freeCluster = fatNextFreeCluster ( fp->info, 0 );
				if (freeCluster == 0)		// if disk is full
					return (FALSE);

				fatWrite ( fp->info, fatSectToClust(fp->info, fp->currentSector), freeCluster );
				fatWriteEOC ( fp->info, freeCluster );
				fp->currentSector = fatClustToSect ( fp->info, freeCluster );

				// initialize the buffer with FF's
				memset ( fp->buffer, 0xFF, BYTES_PER_SECTOR );
			}
		}
	}

	return (TRUE);
}
#endif
////////////////////


//*****************************************************************************
// Function: fatFeof
// Parameters: TFILE struct of the file opened
// Returns: On END OF FILE returns TRUE, and FALSE otherwise
//
// Description: return TRUE if the byte pointer points to the end of the file
//*****************************************************************************
u8 fatFeof( TFILE *fp)
{
	if ((fp->bytePointer) >= (fp->de.deFileSize))
		return TRUE;
	return FALSE;
}

int fatFindUnique ( t_fat_info *info, char *str_dest )
{
	unsigned char tmp_buf[16];
	struct direntry rde;
	int done = 0;
	int loop = 1;

	while ( !done )
	{
		if ( fatGetFileInfo ( info, &rde, str_dest ) == NULL )
			done = 1;
		else
		{
			sprintf ( tmp_buf, "~%03i", loop++ );
			memcpy ( &str_dest[4], tmp_buf, 4 );
			str_dest[8] = ' ';
			str_dest[9] = ' ';
			str_dest[10] = ' ';
		}
		if ( loop > 999 )
			return FALSE;
	}
	return TRUE;
}

//*****************************************************************************
// Function: fatNormalize
// Parameters: file name to be normalized
// Returns: the normalized file name
//
// Description: normalize the file name. Insert spaces betwen the name and the extension.
//              The file name will be 11 caracters
//*****************************************************************************
void fatNormalize(t_fat_info *info, char *str_dest)
{
	u8 o=0, d=0;
	char str_ori[12];

	strncpy ( str_ori, str_dest, 12 );

	if (str_ori[0] == '.') 		// if the file name is "." or ".."
	{
		d=1;
		if (str_ori[1] == '.')
			d=2;
		for (;d<11; d++)
			str_dest[d]=' ';	// complete with spaces
		str_dest[11]='\0';
		return;					// finish
	}


	for (d=0; d<8; d++)			// for all the other file name
	{
		if (str_ori[o] == '.')	// if we found the '.'
		{
			o++;
			break;				//		break the loop
		}
		if (str_ori[o] == '\0')	// if we find the file name end
			break;				//		break the loop
		str_dest[d]= str_ori[o++];
	}

	for (; d<8; d++)			// complete the eight caracters file name with spaces
		str_dest[d]= ' ';

	if (str_ori[o]=='.')		// eliminate the '.'
		o++;

	for (; d<11; d++)			// copy the extension
	{
		if (str_ori[o] == '\0')
			break;
		str_dest[d]= str_ori[o++];
	}

	for (; d<11; d++)			// complete the three caracters extension file name with spaces
		str_dest[d]= ' ';

	strupr(str_dest);
	str_dest[11]='\0';

}


//*****************************************************************************
// Function: fatGetCurDirCluster
// Parameters: none
// Returns: The current dir cluster
//
// Description: Return the current directory cluster number
//*****************************************************************************
u32 fatGetCurDirCluster( t_fat_info *info)
{
	return info->currentDirCluster;
}


////////////////////
#ifndef ATA_READ_ONLY
//*****************************************************************************
// Function: fatNextFreeDirEntry
// Parameters: start cluster
// Returns: On SUSCEFULL returns the next free dir entry found, NULL otherwise
//
// Description:   Find a free dir entry in the directory started in cluster address
//                If found a free dir entry, returns the address of this dir entry
//                The SectorInCache variable will be filled with the sector address
//              of the free dir entry found.
//                If goes until the end of the clusters and don't found a free dir
//              entry, returns NULL. Will be necessary alocate a new cluster
//*****************************************************************************
struct direntry *fatNextFreeDirEntry( t_fat_info *info, u32 cluster)
{
	u16 s,index=0;
	u32 sector;
	unsigned int entry = 0;
	static struct direntry de;

	do
	{
		sector = fatClustToSect(info, cluster);
		for(s=0; s< info->SectorsPerCluster ; s++)
		{
			for (index=0; index < FAT_DIRENTRIES_PER_SECTOR; index++)
			{
				fatGetDirentry ( info, &de, sector + s, entry );

				if((*de.deName == SLOT_DELETED) || (*de.deName == 0x00))
					return(&de);

				entry++;
			}
		}
		cluster=fatNextCluster(info, cluster);
		if (cluster == 0)
			return NULL;
    } while  (cluster <  info->NumClusters ); // end of FatDirentries
 	return NULL;
}

//*****************************************************************************
// Function: fatNextFreeDirEntry
// Parameters: start cluster
// Returns: On SUSCEFULL returns the next free dir entry found, NULL otherwise
//
// Description:   Find a free dir entry in the directory started in cluster address
//                If found a free dir entry, returns the address of this dir entry
//                The SectorInCache variable will be filled with the sector address
//              of the free dir entry found.
//                If goes until the end of the clusters and don't found a free dir
//              entry, returns NULL. Will be necessary alocate a new cluster
//*****************************************************************************
struct direntry *fatNextFreeDirEntryCount( t_fat_info *info, u32 cluster, u32 min_entries )
{
	u16 s,index=0;
	u32 sector;
	int direntries = 0;
	unsigned int entry = 0;
	unsigned int free_entries = 0;
	struct direntry de;
	static struct direntry ret_de;

	sector = fatClustToSect ( info, cluster );
	while ( 1 )
	{
		for ( s=0; s< info->SectorsPerCluster ; s++)
		{
			for (entry=0; entry < FAT_DIRENTRIES_PER_SECTOR; entry++)
			{
				direntries++;
				fatGetDirentry ( info, &de, sector + s, entry );

				if((*de.deName == SLOT_DELETED) || (*de.deName == 0x00))
				{
					if ( !free_entries )
						ret_de = de;
					free_entries++;
				}
				else 
					free_entries = 0;

				if ( free_entries >= min_entries )
					return &ret_de;
			}
		}
		if ( cluster > 0 )
		{
			cluster = fatNextCluster(info, cluster);
			if (cluster == 0)
				return NULL;
			sector = fatClustToSect ( info, cluster );
		}
		else
		{
			if ( direntries >= info->bpb->bpbRootDirEnts )
				return NULL;
			sector++;
		}
		if ( sector > info->FirstDataSector + info->FatSectors )
			return NULL;

    } 
 	return NULL;
}
#endif
////////////////////


////////////////////
#ifndef ATA_READ_ONLY
//*****************************************************************************
// Function: fatNextFreeCluster
// Parameters: start cluster
// Returns: On SUSCEFULL returns the next free cluster found, 0 otherwise
//
// Description: Returns the first free cluster found in Fat table.
//              If goes until the end of the FAT table and don't found a free cluster.
//              return 0.
//*****************************************************************************
u32 fatNextFreeCluster( t_fat_info *info, u32 cluster)
{
	u16 index;
	u32 sector;
	u32 *fat32Buffer;
	u16 *fat16Buffer;

	fat32Buffer = (u32 *)FatCache;
	fat16Buffer = (u16 *)FatCache;

	if (info->Fat32Enabled)
		sector= (cluster>>7) + info->FirstFATSector;// 128 structures of 4 bytes each as a pointer to a cluster
	else
		sector= (cluster>>8) + info->FirstFATSector;// 256 structures of 2 bytes each as a pointer to a cluster

	index=0;
	do
	{
		v_memcpy_get ( info->ws, FatCache, sector * BYTES_PER_SECTOR, BYTES_PER_SECTOR );

		if (info->Fat32Enabled)
		{
			for (index=0; index < FAT32_STRUCTS_PER_SECTOR; index++) //Read all Cluster
			{
				if ((fat32Buffer[index]&FAT32_MASK) == 0x00000000)
					return (index + ((sector - info->FirstFATSector)*FAT32_STRUCTS_PER_SECTOR));
			}
		}
		else //FAT16
		{
			for (index=0; index < FAT16_STRUCTS_PER_SECTOR; index++) //Read all Cluster
			{
				if ((fat16Buffer[index]&FAT16_MASK) == 0x0000)
					return (index + ((sector - info->FirstFATSector)*FAT16_STRUCTS_PER_SECTOR));
			}
		}
		sector++;
    }
	while  (sector < info->FirstFATSector + info->FatSectors ); // end of FatDirentries
 	return 0;
}
#endif
////////////////////



////////////////////
#ifndef ATA_READ_ONLY
//*****************************************************************************
// Function: fatWriteEOC
// Parameters: cluster
// Returns: none
//
// Description: Write in FAT in cluster position, the END OF CLUSTER mark
//*****************************************************************************
void fatWriteEOC( t_fat_info *info, u32 cluster)
{
	fatWrite(info, cluster, FAT32_MASK & CLUST_EOFE);
}
#endif
////////////////////


////////////////////
#ifndef ATA_READ_ONLY
//*****************************************************************************
// Function: fatWrite
// Parameters: cluster, data to write
// Returns: none
//
// Description: Write the data in FAT in cluster position
//*****************************************************************************
void fatWrite( t_fat_info *info, u32 cluster, u32 data)
{
	u16 offset;
	u32 *Fat32CacheLong, sector;
	u16  *Fat16CacheInt;

	// calculate offset of the our entry within that FAT sector
	if(info->Fat32Enabled)
		offset = cluster % FAT32_STRUCTS_PER_SECTOR;
	else
		offset = cluster % FAT16_STRUCTS_PER_SECTOR;

	// read the FAT sector, with has information abou the cluster that we are interested in.
	sector= fatTableClustToSect(info, cluster);

	// initialize if not existing
	v_memcpy_put ( info->ws, sector*BYTES_PER_SECTOR, NULL, BYTES_PER_SECTOR );
	v_memcpy_get ( info->ws, FatCache, sector * BYTES_PER_SECTOR, BYTES_PER_SECTOR );

	// write the data to Fat Cache
	if (info->Fat32Enabled)
	{
		Fat32CacheLong= (u32 *)FatCache;
		Fat32CacheLong[offset]= (Fat32CacheLong[offset]&(~FAT32_MASK)) | (data&FAT32_MASK);
	}
	else
	{
		Fat16CacheInt= (u16 *)FatCache;
		Fat16CacheInt[offset]= (Fat16CacheInt[offset]&(~FAT16_MASK)) | (data&FAT16_MASK);
	}
	// write Fat Cache in the HardDisk
	v_memcpy_put ( info->ws, sector * BYTES_PER_SECTOR, FatCache, BYTES_PER_SECTOR );
	v_memcpy_put ( info->ws, (sector+(info->FirstFAT2Sector-info->FirstFATSector)) * BYTES_PER_SECTOR, FatCache, BYTES_PER_SECTOR );
}
#endif
////////////////////


////////////////////
#ifndef ATA_READ_ONLY
//*****************************************************************************
// Function: fatTableClustToSect
// Parameters: cluster
// Returns: the FAT sector address with has cluster information
//
// Description: Calculate the FAT sector address with has cluster information
//*****************************************************************************
u32 fatTableClustToSect( t_fat_info *info, u32 cluster)
{
	u32 sector, fatOffset;

	// get fat offset in bytes
	if(info->Fat32Enabled)
		fatOffset = cluster << 2;
	else
		fatOffset = cluster << 1;

	// calculate the FAT sector that we're interested in
	sector = info->FirstFATSector + (fatOffset / BYTES_PER_SECTOR);
	return (sector);
}
#endif
////////////////////


////////////////////
#ifndef ATA_READ_ONLY
//*****************************************************************************
// Function: fatLastCluster
// Parameters: cluster
// Returns: last cluster of the file started in cluster
//
// Description: Find the last cluster of a file started in "cluster" in fat table
//*****************************************************************************
u32 fatLastCluster( t_fat_info *info, u32 cluster)
{
	u32 lastCluster;

	do
	{
		lastCluster=cluster;
		cluster = fatNextCluster(info, lastCluster);
    }
	while  (cluster != 0);
	return lastCluster;
}
#endif
////////////////////



////////////////////
#ifndef ATA_READ_ONLY
//*****************************************************************************
// Function: fatDirectoryIsEmpty
// Parameters: Start Directory Cluster
// Returns: TRUE if the directory is empty, FALSE otherwise
//
// Description: check to the existance of files in the directory started in DirCluster
//*****************************************************************************
u8 fatDirectoryIsEmpty( t_fat_info *info, u32 DirCluster)
{
	struct direntry de;
	u32 sector;
	u32 isector;
	int index;
	unsigned int entry = 0;

	do
	{
		sector=fatClustToSect(info, DirCluster);	// read directory clusters
		for (isector=0; isector < info->SectorsPerCluster ;isector++)	// read one cluster
		{
			for ( index=0; index<FAT_DIRENTRIES_PER_SECTOR; index++)	// read one sector
			{
				fatGetDirentry ( info, &de, sector + isector, entry );
				if	(*de.deName != '.' )
				{
					if ( *de.deName == SLOT_EMPTY )
						return TRUE;
					if ( *de.deName != SLOT_DELETED)
						return FALSE;
				}
				// If goes until here it's because de->Name is equal '.' or is equal SLOT_DELETED
				entry++;
			}
		}
		DirCluster=fatNextCluster(info, DirCluster);
	}while ( DirCluster != 0 );
	// if goes until here it's because the all file (directory) is filled with SLOT_DELETED
	return  TRUE;
}
#endif
////////////////////


////////////////////
#ifndef ATA_READ_ONLY
//*****************************************************************************
// Function: fatRemoveAll
// Parameters: none
// Returns: none
//
// Description: Remove all files in currentDirCluster
//*****************************************************************************
void fatRemoveAll( t_fat_info *info )
{
	u32 offset=0;
	struct direntry *de = 0;	// avoid compiler warning by initializing
	int index;

	for (offset=0; ; offset++)
	{
		de = (struct direntry *) fatDir(info, info->currentDirCluster, offset);
		if (de == NULL)
			return;
		for (index=0; index<FAT_DIRENTRIES_PER_SECTOR; index++)
		{
			if (*de->deName == SLOT_EMPTY)
				return;		// there is no more direntries

			if (( de->deAttributes & ATTR_VOLUME ) != ATTR_VOLUME)
				if ((*de->deName != SLOT_DELETED) && (*de->deName != '.'))
					fatRemove(info, de->deName);
			de++;
		}	// end of sector
	}	// end of cluster
}
#endif
////////////////////
