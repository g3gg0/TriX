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

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <string.h>


#include "ata.h"
#include "fat.h"
#include "fatconf.h"
#include "fattime.h"

// globals
unsigned char *SectorBuffer  =		(unsigned char *) SECTOR_BUFFER_ADDR;	// Sector Buffer
unsigned char *FatCache =			(unsigned char *) FAT_CACHE_ADDR;		// Fat Buffer
char VolLabel[12];							// Volume Label

struct partrecord PartInfo;					// Partition Information
TFILE File;									// File Information
unsigned long FSInfo;						// File System Information
unsigned char Fat32Enabled;					// Indicates if is FAT32 or FAT16
unsigned long FirstDataSector;				// First Data Sector Address
unsigned int  SectorsPerCluster;			// Number of Sectors per Cluster
unsigned long FirstFATSector;				// First FAT Sector Address
unsigned long FirstFAT2Sector;				// First FAT2 Sector Address
unsigned long FirstDirCluster;				// First Directory (Data) Cluster Address
unsigned long FatInCache = 0xFFFFFFFF;		// Address of the FAT Cluster in FatCache
unsigned long FatSectors;					// Number of FAT Sectors
unsigned long currentDirCluster;			// Actual Dir Cluster Number
unsigned long NumClusters;					// ATA Dispositive Cluster Numbers
unsigned long SectorInCache = 0xFFFFFFFF;	// Address of the Sector Cluster in SectorBuffer



//*****************************************************************************
// Function: fatClustToSect
// Parameters: cluster
// Returns: sector
//
// Description: Converts Sectors in Clusters
//*****************************************************************************
unsigned long fatClustToSect(unsigned long clust)
{
	if (clust == 0)
		clust=2;
	return ((clust-2) * SectorsPerCluster) + FirstDataSector;
}


//*****************************************************************************
// Function: fatSectToClust
// Parameters: sector
// Returns: cluster
//
// Description: Converts Clusters in Sectors
//*****************************************************************************
unsigned long fatSectToClust(unsigned long sect)
{
	return (((sect - FirstDataSector) / SectorsPerCluster) + 2);
}


//*****************************************************************************
// Function: fatInit
// Parameters: none
// Returns: TRUE if a FAT drive was found, FALSE otherwise
//
// Description: Get FAT info from ATA dispositive and initialize internal variables
//*****************************************************************************
unsigned char fatInit(void)
{
	struct bpb710 *bpb;

	// read partition table
	ataReadSectors(DRIVE0, 0, SectorBuffer, &SectorInCache);

	// map first partition record
	// save partition information to global PartInfo
	PartInfo = *((struct partrecord *) ((struct partsector *) SectorBuffer)->psPart);

	// Read the Partition BootSector
	// **first sector of partition in PartInfo.prStartLBA
	ataReadSectors( DRIVE0, PartInfo.prStartLBA, SectorBuffer, &SectorInCache );

	bpb = (struct bpb710 *) ((struct bootsector710 *) SectorBuffer)->bsBPB;

	// setup global disk constants
	FirstDataSector	= PartInfo.prStartLBA;
	if(bpb->bpbFATsecs)
	{
		// bpbFATsecs is non-zero and is therefore valid
		FirstDataSector	+= bpb->bpbResSectors + bpb->bpbFATs * bpb->bpbFATsecs;
	}
	else
	{
		// bpbFATsecs is zero, real value is in bpbBigFATsecs
		FirstDataSector	+= bpb->bpbResSectors + bpb->bpbFATs * bpb->bpbBigFATsecs;
	}
	SectorsPerCluster	= bpb->bpbSecPerClust;
	FirstFATSector		= bpb->bpbResSectors + PartInfo.prStartLBA;
	FatSectors			= bpb->bpbBigFATsecs;
	NumClusters			= ataGetSizeInSectors()/(bpb->bpbSecPerClust);

	// initialize Volume Label
	memcpy(&VolLabel, bpb->bpbVolLabel, 11);
	VolLabel[11]='\0';

	switch (PartInfo.prPartType)
	{
		case PART_TYPE_DOSFAT16:
		case PART_TYPE_FAT16:
		case PART_TYPE_FAT16LBA:
			// first directory cluster is 2 by default (clusters range 2->big)
			FirstDirCluster	= CLUST_FIRST;
			// push data sector pointer to end of root directory area
			//FirstDataSector += (bpb->bpbRootDirEnts)/DIRENTRIES_PER_SECTOR;
			Fat32Enabled = FALSE;
			break;
		case PART_TYPE_FAT32LBA:
		case PART_TYPE_FAT32:
			// bpbRootClust field exists in FAT32 bpb710, but not in lesser bpb's
			FirstDirCluster = bpb->bpbRootClust;
			// push data sector pointer to end of root directory area
			// need this? FirstDataSector += (bpb->bpbRootDirEnts)/DIRENTRIES_PER_SECTOR;
			Fat32Enabled = TRUE;
			break;
		default:
			//Found: No Partition!
			return FALSE;
			break;
	}

	if (Fat32Enabled)
		FirstFAT2Sector=FirstFATSector+bpb->bpbBigFATsecs;
	else
		FirstFAT2Sector=FirstFATSector+bpb->bpbFATsecs;
	FSInfo=bpb->bpbFSInfo+PartInfo.prStartLBA;

	currentDirCluster= FirstDirCluster;

	return TRUE;
}


//*****************************************************************************
// Function: fatNextCluster
// Parameters: cluster
// Returns: the next cluster
//
// Description: Find next cluster in the FAT chain
//*****************************************************************************
unsigned long fatNextCluster(unsigned long cluster)
{
	unsigned long nextCluster;
	unsigned long fatMask;
	unsigned long fatOffset;
	unsigned long sector;
	unsigned int offset;

	// get fat offset in bytes
	if(Fat32Enabled)
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
	sector = FirstFATSector + (fatOffset / BYTES_PER_SECTOR);
	// calculate offset of the our entry within that FAT sector
	offset = fatOffset % BYTES_PER_SECTOR;

	// if we don't already have this FAT chunk loaded, go get it
	ataReadSectors( DRIVE0, sector, (unsigned char*)FAT_CACHE_ADDR, &FatInCache);


	// read the nextCluster value
	nextCluster = (*((unsigned long*) &((char*)FAT_CACHE_ADDR)[offset])) & fatMask;

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
unsigned int fatClusterSize(void)
{
	return SectorsPerCluster;
}


//*****************************************************************************
// Function: fatGetFirstDirCluster
// Parameters: none
// Returns: the cluster number of the first dir entry in FAT
//
// Description: return the first dir entry cluster in FAT
//*****************************************************************************
unsigned long fatGetFirstDirCluster(void)
{
	return(FirstDirCluster);
}


//*****************************************************************************
// Function: fatGetPartInfo
// Parameters: none
// Returns: FAT partition information
//
// Description: return the partition information read in fatInit routine
//*****************************************************************************
struct partrecord *fatGetPartInfo(void)
{
	return (&PartInfo);
}



//*****************************************************************************
// Function: fatGetSecPerClust
// Parameters: none
// Returns: the number of Sectors per Cluster
//
// Description: return the number of Sectors per Cluster read in fatInit
//*****************************************************************************
unsigned int fatGetSecPerClust(void)
{
	return SectorsPerCluster;
}


//*****************************************************************************
// Function: fatGetFirstFATSector
// Parameters: none
// Returns: the address of the first FAT Sector
//
// Description: return the sector address of the first FAT
//*****************************************************************************
unsigned long fatGetFirstFATSector(void)
{
	return(FirstFATSector);
}


//*****************************************************************************
// Function: fatGetFirstFAT2Sector
// Parameters: none
// Returns: the address of the first second FAT Sector
//
// Description: return the sector address of the second FAT
//*****************************************************************************
unsigned long fatGetFirstFAT2Sector(void)
{
	return(FirstFAT2Sector);
}


//*****************************************************************************
// Function: fatGetFirstDataSector
// Parameters: none
// Returns: the address of the first data Sector
//
// Description: return the sector address of the data in FAT
//*****************************************************************************
unsigned long fatGetFirstDataSector(void)
{
	return(FirstDataSector);
}


//*****************************************************************************
// Function: fatGetNumClusters
// Parameters: none
// Returns: the total number of clusters
//
// Description: return the total number of clusters in the ata dispositive
//*****************************************************************************
unsigned long fatGetNumClusters(void)
{
	return(NumClusters);
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
unsigned char *fatDir(unsigned long cluster, unsigned long offset)
{
	unsigned long index;

	for (index=0; index < offset/SectorsPerCluster; index++)
		cluster=fatNextCluster(cluster);

	if (cluster == 0)
		return NULL;

	ataReadSectors(DRIVE0, fatClustToSect(cluster)+ offset%SectorsPerCluster, SectorBuffer, &SectorInCache);
	return SectorBuffer;
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
struct direntry *fatGetFileInfo(struct direntry *rde, char *shortName)
{
	unsigned long sector=0, cluster;
	struct direntry *de;
	int index, isetor;
	char Name[12];

	strncpy(Name, shortName,12);
	Name[11]='\0';
	fatNormalize(Name); // adjust the name to the FAT format

	cluster= currentDirCluster;	// start the search in the current cluster
	do
	{
		for (isetor=0; isetor< SectorsPerCluster; isetor++)		// clusters
		{
			// read dir data
			sector = fatClustToSect(cluster) + (unsigned long)isetor;
			ataReadSectors( DRIVE0, sector, SectorBuffer, &SectorInCache);
			de = (struct direntry *) SectorBuffer;

			for (index=1; index<=FAT_DIRENTRIES_PER_SECTOR; index++)
			{
				if (*de->deName == 0x00)
				{
					return NULL;		// there is no more direntries
				}
				if((*de->deName != SLOT_DELETED) && (de->deAttributes != ATTR_LONG_FILENAME))
				{
					if (strncmp(de->deName, Name, 11) == 0)
					{
						memcpy(rde, de, DIRENTRY_SIZE);
						return(de);
					}
				}
				de++;
			}	// end of sector
		}	// end of cluster
		cluster= fatNextCluster(cluster);
	}while  (sector < FirstFATSector+FatSectors); // end of FatDirentries

	return NULL;
}


//*****************************************************************************
// Function: fatGetVolLavel
// Parameters: none
// Returns: the FAT Volume Name
//
// Description: Return the FAT Volume Name read in fatInit
//*****************************************************************************
char* fatGetVolLabel(void)
{
	return VolLabel;
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
unsigned char fatCddir(char *path)
{
	struct direntry de;

	// if the path doesn't exist return FALSE
	if (fatGetFileInfo(&de, path) == NULL)
		return FALSE;

	// if the path is a directory
	if ((de.deAttributes & ATTR_DIRECTORY) == ATTR_DIRECTORY)
	{
		// change the current dir cluster to the path information
		currentDirCluster= (de.deStartCluster) + ((unsigned long)de.deHighClust<<16);
		if (currentDirCluster == 0)
			currentDirCluster = 2;
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
unsigned char fatMkdir(char *path)
{
	struct direntry *de, rde;
	unsigned long freeCluster, newDirEntrySector, lastCluster;
	unsigned int i, date, time;
	char string[12];
	TTime t;

	// if the file already exist
	if (fatGetFileInfo(&rde, path) != NULL)
	{
		return FALSE;
	}

	// Find a free direntry, returns a pointer to the direntry struct inside SectorBuffer,
	de=fatNextFreeDirEntry(currentDirCluster);

	// Saves the sector address of this new DirEntry to write it back to the ATA dispositive
	newDirEntrySector= SectorInCache;


	// if goes until the end of the cluster and don't find a free direntry
	// needs to create a new direntry in a new cluster to put this new directory.
	if (de == NULL)
	{
		freeCluster=fatNextFreeCluster(0); 	// find a new free cluster
		if (freeCluster == 0)				// is the disk is full
			return (FALSE);					//		return ERROR

		lastCluster= fatLastCluster(currentDirCluster);	// find the last cluster of the current dir
		fatWrite(lastCluster, freeCluster);				// creates a connection between the last cluster of the current directory and the new cluster created
		fatWriteEOC(freeCluster);						// mark the new cluster with an END OF CLUSTER mark
		memset (SectorBuffer, '\0', BYTES_PER_SECTOR); 	// fill the SectorBuffer with ZEROS to informate to the FAT that don't have more direntries after this one that we are creating
		de= (struct direntry *)SectorBuffer;			// de points to the first address in the SectorBuffer
		newDirEntrySector= fatClustToSect(freeCluster); // Calculate the start sector address of this new cluster
		SectorInCache= newDirEntrySector;				// the sector in memory is this new direntry allocated
		for (unsigned int i=1; i< SectorsPerCluster; i++)	// write zeros in all the new cluster allocated
			ataWriteSectors( DRIVE0, newDirEntrySector+i, SectorBuffer);
	}

	// in this point we have a direntry in SectorBuffer, and de point to the address of the informations about the new directory


	// find a FAT free cluster to write the directory content
	freeCluster=fatNextFreeCluster(0);

	// fill the direntry
	strcpy(string, path);
	string[11]='\0';
	fatNormalize(string);

	fatGetCurTime(&t);
	time= (unsigned int)((t.hour)<<DT_HOURS_SHIFT) + ((t.minutes)<<DT_MINUTES_SHIFT) + ((t.seconds)>>DT_2SECONDS_SHIFT);		// create time
	date= (unsigned int)((t.year-1980)<<DD_YEAR_SHIFT) + (t.month<<DD_MONTH_SHIFT) + t.day;     						// create date

	memcpy(de->deName, string, 11);		// name of the directory
	de->deAttributes= ATTR_DIRECTORY;
	de->deLowerCase=0;   	 			// lowercase Names
	de->deCHundredth= 0x00;	 			// hundredth of seconds in CTime

	de->deCTime[0]=(unsigned char)time&0x00FF;
	de->deCTime[1]=(unsigned char)((time&0xFF00)>>8);
	de->deCDate[0]=(unsigned char)date&0x00FF;
	de->deCDate[1]=(unsigned char)((date&0xFF00)>>8);
	de->deADate[0]=de->deCDate[0];     	// access date
	de->deADate[1]=de->deCDate[1];     	// access date
	de->deHighClust= (freeCluster>>16); // high bytes of cluster number
	de->deMTime[0]=de->deCTime[0];     	// last update time
	de->deMTime[1]=de->deCTime[1];     	// last update time
	de->deMDate[0]=de->deCDate[0];     	// last update date
	de->deMDate[1]=de->deCDate[1];     	// last update date
	de->deStartCluster= freeCluster; 	// starting cluster of file
	de->deFileSize=0;  					// size of file in bytes, is a directory, so the size is 0


	// write SectorBuffer filled with the new directory information
	ataWriteSectors( DRIVE0, newDirEntrySector, SectorBuffer);


	// create the files "." and ".."
	de= (struct direntry *)SectorBuffer;
	strncpy(de->deName, ".          ", 11);
	de->deAttributes= ATTR_DIRECTORY;
	de->deLowerCase=0;   				// lowercase Names
	de->deCHundredth= 0x00;	 			// hundredth of seconds in CTime
	de->deCTime[0]=(unsigned char)time&0x00FF;
	de->deCTime[1]=(unsigned char)((time&0xFF00)>>8);
	de->deCDate[0]=(unsigned char)date&0x00FF;
	de->deCDate[1]=(unsigned char)((date&0xFF00)>>8);	de->deADate[0]=de->deCDate[0];     	// access date
	de->deADate[1]=de->deCDate[1];     	// access date
	de->deHighClust= (freeCluster>>16); // high bytes of cluster number
	de->deMTime[0]=de->deCTime[0];     	// last update time
	de->deMTime[1]=de->deCTime[1];     	// last update time
	de->deMDate[0]=de->deCDate[0];     	// last update date
	de->deMDate[1]=de->deCDate[1];     	// last update date
	de->deStartCluster= freeCluster; // starting cluster of file
	de->deFileSize=0;  					// size of file in bytes

	de++;
	strncpy(de->deName, "..         ", 11);
	de->deAttributes= ATTR_DIRECTORY;
	de->deLowerCase=0;   				// lowercase Names
	de->deCHundredth= 0x00;	 			// hundredth of seconds in CTime
	de->deCTime[0]=(unsigned char)time&0x00FF;
	de->deCTime[1]=(unsigned char)((time&0xFF00)>>8);
	de->deCDate[0]=(unsigned char)date&0x00FF;
	de->deCDate[1]=(unsigned char)((date&0xFF00)>>8);
	de->deADate[0]=de->deCDate[0];     	// access date
	de->deADate[1]=de->deCDate[1];     	// access date
	de->deHighClust= (currentDirCluster>>16); // high bytes of cluster number
	de->deMTime[0]=de->deCTime[0];     	// last update time
	de->deMTime[1]=de->deCTime[1];     	// last update time
	de->deMDate[0]=de->deCDate[0];     	// last update date
	de->deMDate[1]=de->deCDate[1];     	// last update date
	de->deStartCluster= currentDirCluster; // starting cluster of file
	de->deFileSize=0;  					// size of file in bytes
	de++;

	// fill the rest with zeros
	for (i= (unsigned int)(((unsigned char *)de) - SectorBuffer); i < 512; i++)
		SectorBuffer[i]= 0;

	// write the content of the new directory created, including "." and ".." files
	ataWriteSectors( DRIVE0, fatClustToSect(freeCluster), SectorBuffer);


	// fill the entire sector buffer with zeros
	memset (SectorBuffer, '\0', BYTES_PER_SECTOR);

	// write all the others sectors of this new cluster.
	for (i=1; i< SectorsPerCluster; i++)
		ataWriteSectors( DRIVE0, fatClustToSect(freeCluster)+i, SectorBuffer);

	// actualize the sector address in cache
	SectorInCache= fatClustToSect(freeCluster)+SectorsPerCluster-1;


	// mark the cluster with the content off the new directory created with an END OF CLUSTER mark
	fatWriteEOC(freeCluster);
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
TFILE* fatFcreate(char *shortName)
{
	struct direntry *de, rde;
	unsigned long freeCluster, newDirEntrySector, lastCluster;
	unsigned int date, time;
	char string[12];
	TTime t;

	// if the file already exist
	if (fatGetFileInfo(&rde, shortName) != NULL)
		return NULL;


	// Find a free direntry, returns a pointer to the direntry struct inside SectorBuffer,
	de=fatNextFreeDirEntry(currentDirCluster);

	// Saves the sector address of this new DirEntry to write it back to the ATA dispositive
	newDirEntrySector= SectorInCache;


	// if goes until the end of the cluster and don't find a free direntry
	// needs to create a new direntry in a new cluster to put this new file.
	if (de == NULL)
	{
		freeCluster=fatNextFreeCluster(0);
		if (freeCluster == 0)	// if disk is full
			return (FALSE);		// returns ERROR


		lastCluster= fatLastCluster(currentDirCluster);	// find the last cluster of the current dir
		fatWrite(lastCluster, freeCluster);				// creates a connection between the last cluster of the current directory and the new cluster created
		fatWriteEOC(freeCluster);						// mark the new cluster with an END OF CLUSTER mark
		memset (SectorBuffer, '\0', BYTES_PER_SECTOR); 	// fill the SectorBuffer with ZEROS to informate to the FAT that don't have more direntries after this one that we are creating
		de= (struct direntry *)SectorBuffer;			// de points to the first address in the SectorBuffer
		newDirEntrySector= fatClustToSect(freeCluster); // Calculate the start sector address of this new cluster
		SectorInCache= newDirEntrySector;				// the sector in memory is this new direntry allocated
	}

	// find a free fat cluster to put the content of this new file
	freeCluster=fatNextFreeCluster(0);

	// fill with file data information
	strcpy(string, shortName);
	string[11]='\0';
	fatNormalize(string);


	fatGetCurTime(&t);
	time= (unsigned int)((t.hour)<<DT_HOURS_SHIFT) + ((t.minutes)<<DT_MINUTES_SHIFT) + ((t.seconds)>>DT_2SECONDS_SHIFT);		// create time
	date= (unsigned int)((t.year-1980)<<DD_YEAR_SHIFT) + (t.month<<DD_MONTH_SHIFT) + t.day;     						// create date

	memcpy(de->deName, string, 11);
	de->deAttributes= ATTR_ARCHIVE;
	de->deLowerCase=0;   				// lowercase Names
	de->deCHundredth= 0x00;				// hundredth of seconds in CTime

	de->deCTime[0]=(unsigned char)time&0x00FF;
	de->deCTime[1]=(unsigned char)((time&0xFF00)>>8);
	de->deCDate[0]=(unsigned char)date&0x00FF;
	de->deCDate[1]=(unsigned char)((date&0xFF00)>>8);
	de->deADate[0]=de->deCDate[0];     	// access date
	de->deADate[1]=de->deCDate[1];     	// access date
	de->deHighClust= (freeCluster>>16); // high bytes of cluster number
	de->deMTime[0]=de->deCTime[0];     	// last update time
	de->deMTime[1]=de->deCTime[1];     	// last update time
	de->deMDate[0]=de->deCDate[0];     	// last update date
	de->deMDate[1]=de->deCDate[1];     	// last update date
	de->deStartCluster= freeCluster; 	// starting cluster of file
	de->deFileSize=0;  					// size of file in bytes

	// write direntry with the file information
	ataWriteSectors( DRIVE0, newDirEntrySector, SectorBuffer);

	// mark the cluster with the content off the new directory created with an END OF CLUSTER mark
	fatWriteEOC(freeCluster);

	// open the file to read and write
	return(fatFopen(shortName));
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
unsigned char fatRename(char *oldShortName, char *newShortName)
{
	struct direntry *de, rde;
	char string[12];

	// if the new file name already exist
	de=fatGetFileInfo(&rde, newShortName);
	if (de != NULL)
		return FALSE;

	// if the file don't exist
	de=fatGetFileInfo(&rde, oldShortName);
	if (de == NULL)
		return FALSE;


	strcpy(string, newShortName);
	string[11]='\0';
	fatNormalize(string);
	memcpy(de->deName, string, 11);
	ataWriteSectors( DRIVE0, SectorInCache, SectorBuffer);
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
unsigned char fatRemove(char *shortName)
{
	struct direntry *de, rde;
	unsigned long CurrentDirCluster, NextCluster, fileSector;


	de=fatGetFileInfo(&rde, shortName);	// get file information
	if (de == NULL)						// if the file doesn't exist
		return FALSE;					//	 	return ERROR

	fileSector=SectorInCache;			// save the File sector address

	// calculate the cluster address of this file
	CurrentDirCluster = ((unsigned long)de->deHighClust << 16) + de->deStartCluster;

	// if the file is a directory
	if ( de->deAttributes == ATTR_DIRECTORY )
	{
		if (!fatDirectoryIsEmpty(CurrentDirCluster))	// if there are files in the directory
			return FALSE;								//		return ERROR
	}

	// Read the direntry information about the file
	ataReadSectors( DRIVE0, fileSector, SectorBuffer, &SectorInCache);

	// mark the file name as deleted
	*de->deName = SLOT_DELETED;

	// write the direntry back to the ATA dispositive
	ataWriteSectors( DRIVE0, SectorInCache, SectorBuffer);

	// erase fat table
	do{
		NextCluster=fatNextCluster(CurrentDirCluster);
		fatWrite(CurrentDirCluster,CLUST_FREE);	// free the current dir cluster
		CurrentDirCluster=NextCluster;
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
TFILE* fatFopen(char *shortName)
{
	// if the file don't exist
	if (fatGetFileInfo(&File.de, shortName) == NULL)
		return NULL;

	File.currentSector = fatClustToSect(((unsigned long)File.de.deHighClust << 16) + File.de.deStartCluster);
	File.buffer=SectorBuffer;
	File.bytePointer=0;
	File.sectorHasChanged=FALSE;
	ataReadSectors( DRIVE0, File.currentSector, File.buffer, &SectorInCache); // read the first file sector

	return (&File);
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
unsigned char fatFclose(TFILE *fp)
{
	struct direntry *de, rde;
	unsigned long fileSize;
	unsigned int date, time;
	TTime t;

	fileSize= fp->de.deFileSize;
	fp->bytePointer=0;
	if (fatFflush(fp))
	{
		if ((de=fatGetFileInfo(&rde, fp->de.deName)) == NULL) // if the file don't exist
			return FALSE;
		de->deFileSize= fileSize; // refresh the file size
		fatGetCurTime(&t);
		time= (unsigned int)((t.hour)<<DT_HOURS_SHIFT) + ((t.minutes)<<DT_MINUTES_SHIFT) + ((t.seconds)>>DT_2SECONDS_SHIFT);		// create time
		date= (unsigned int)((t.year-1980)<<DD_YEAR_SHIFT) + (t.month<<DD_MONTH_SHIFT) + t.day;     						// create date
		de->deMTime[0]=(unsigned char)time&0x00FF;			// last update time
		de->deMTime[1]=(unsigned char)((time&0xFF00)>>8);	// last update time
		de->deMDate[0]=(unsigned char)date&0x00FF;			// last update date
		de->deMDate[1]=(unsigned char)((date&0xFF00)>>8);	// last update date
		de->deADate[0]=de->deMDate[0];     					// access date
		de->deADate[1]=de->deMDate[1];     					// access date


		ataWriteSectors( DRIVE0, SectorInCache, SectorBuffer);


		ataReadSectors( DRIVE0, FSInfo, SectorBuffer, &SectorInCache); // Read FSInfo
		((struct fsinfo *)SectorBuffer)->fsinfree[0]=0xFF; // change to Operation System don't to try to correct this field and the FAT
		((struct fsinfo *)SectorBuffer)->fsinfree[1]=0xFF;
		((struct fsinfo *)SectorBuffer)->fsinfree[2]=0xFF;
		((struct fsinfo *)SectorBuffer)->fsinfree[3]=0xFF;

		((struct fsinfo *)SectorBuffer)->fsinxtfree[0]=0xFF;
		((struct fsinfo *)SectorBuffer)->fsinxtfree[1]=0xFF;
		((struct fsinfo *)SectorBuffer)->fsinxtfree[2]=0xFF;
		((struct fsinfo *)SectorBuffer)->fsinxtfree[3]=0xFF;
		ataWriteSectors( DRIVE0, SectorInCache, SectorBuffer);
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
unsigned char fatFflush(TFILE *fp)
{
	if (fp->sectorHasChanged)
	{
		ataWriteSectors( DRIVE0, fp->currentSector, fp->buffer);
		fp->sectorHasChanged=FALSE;
	}
	return TRUE;
}
#endif
////////////////////


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
unsigned int fatFseek(TFILE *fp, unsigned long offSet, unsigned char mode)
{

	unsigned long numClusters, curCluster, numSector;

	// calculate the new byte pointer
	switch (mode)
	{
		case SEEK_END: fp->bytePointer= fp->de.deFileSize - offSet;	break;
		case SEEK_SET: fp->bytePointer= offSet;                     break;
		case SEEK_CUR: fp->bytePointer= fp->bytePointer + offSet;   break;
		default: return FALSE;
	}

	// calculate the current file cluster
	curCluster=((unsigned long)fp->de.deHighClust<<16)+(fp->de.deStartCluster);
	numClusters=fp->bytePointer/(BYTES_PER_SECTOR*SectorsPerCluster);
	numSector=(fp->bytePointer/BYTES_PER_SECTOR)%SectorsPerCluster;

	// calculate the cluster address of the new byte pointer
	while (numClusters > 0)
	{
		curCluster=fatNextCluster(curCluster);
		numClusters--;
	}

	// calculate the Sector address of the new byte pointer
	fp->currentSector=fatClustToSect(curCluster) + numSector;

	// read that sector
	ataReadSectors( DRIVE0, fp->currentSector, fp->buffer, &SectorInCache);

	return TRUE;
}


//*****************************************************************************
// Function: fatFgetc
// Parameters: TFILE struct of the file opened
// Returns: On SUSCEFULL returns the next character from file, and 0 otherwise
//
// Description: Get the next character from file, and actualize the byte pointer
//*****************************************************************************
char fatFgetc(TFILE *fp)
{
	unsigned int bufferPointer;

	if (fatFeof(fp))	// if is the end of file
		return 0;


	bufferPointer= fp->bytePointer & 0x001FF; // equal (fp->bytePointer % 512)
	fp->bytePointer++;


	if ((fp->bytePointer == 1) || (bufferPointer != 0))
	{
		return(fp->buffer[bufferPointer]);
	}
	else
	{
		// Next Sector is in current Cluster
		if (fatSectToClust(fp->currentSector) == fatSectToClust(fp->currentSector+1))
		{
			fp->currentSector++;
		}
		else // Next Sector is in next Cluster
		{
			unsigned long cluster;

			cluster= fatNextCluster(fatSectToClust(fp->currentSector));
			fp->currentSector= fatClustToSect(cluster);
		}
		ataReadSectors( DRIVE0, fp->currentSector, fp->buffer, &SectorInCache);
		return(fp->buffer[bufferPointer]);
	}
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
unsigned char fatFputc(TFILE *fp, char c)
{
	unsigned int bufferPointer;
	unsigned long freeCluster;

	bufferPointer= fp->bytePointer & 0x001FF; // equal (fp->bytePointer % 512)
	fp->bytePointer++;


	if ((fp->bytePointer != 1) && (bufferPointer == 0))	// if we need to allocate a new sector to the file
	{
		ataWriteSectors(DRIVE0, fp->currentSector, fp->buffer);

		if(fp->de.deFileSize>fp->bytePointer)	// sector already exist
		{
			// Next Sector is in current Cluster
			if (fatSectToClust(fp->currentSector) == fatSectToClust(fp->currentSector+1))
			{
				fp->currentSector++;
			}
			else // Next Sector is in next Cluster
			{
				unsigned long cluster;

				cluster= fatNextCluster(fatSectToClust(fp->currentSector));
				fp->currentSector= fatClustToSect(cluster);
			}
			ataReadSectors( DRIVE0, fp->currentSector, fp->buffer, &SectorInCache);
		}
		else 	// sector isn´t used or the cluster isn´t allocated
		{

			if (fatSectToClust(fp->currentSector) == fatSectToClust(fp->currentSector+1))//the next sector is in current cluster
			{
				fp->currentSector++;
			}
			else
			{
				freeCluster=fatNextFreeCluster(0);
				if (freeCluster == 0)		// if disk is full
					return (FALSE);
				fatWrite(fatSectToClust(fp->currentSector), freeCluster);
				fatWriteEOC(freeCluster);
				fp->currentSector=fatClustToSect(freeCluster);
			}
		}
	}
	fp->buffer[bufferPointer]=c;
	fp->sectorHasChanged=TRUE;
	if(fp->bytePointer>fp->de.deFileSize)
		fp->de.deFileSize=fp->bytePointer;

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
unsigned char fatFeof(TFILE *fp)
{
	if ((fp->bytePointer) >= (fp->de.deFileSize))
		return TRUE;
	return FALSE;
}


//*****************************************************************************
// Function: fatNormalize
// Parameters: file name to be normalized
// Returns: the normalized file name
//
// Description: normalize the file name. Insert spaces betwen the name and the extension.
//              The file name will be 11 caracters
//*****************************************************************************
void fatNormalize(char *str_dest)
{
	unsigned char o=0, d=0;
	char str_ori[12];

	strncpy(str_ori, str_dest, 12);

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
unsigned long fatGetCurDirCluster(void)
{
	return currentDirCluster;
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
struct direntry *fatNextFreeDirEntry(unsigned long cluster)
{
	unsigned int s,index=0;
	unsigned long sector;
	struct direntry *de;

	do
	{
		sector = fatClustToSect(cluster);
		for(s=0; s< SectorsPerCluster ; s++)
		{
			ataReadSectors(DRIVE0, sector+s, SectorBuffer, &SectorInCache);
			de = (struct direntry *) SectorBuffer;
			for (index=0; index<FAT_DIRENTRIES_PER_SECTOR; index++)
			{
				if((*de->deName == SLOT_DELETED) || (*de->deName == 0x00))
					return(de);
				de++;
			}
		}
		cluster=fatNextCluster(cluster);
		if (cluster == 0)
			return NULL;
    }while  (cluster <  NumClusters ); // end of FatDirentries
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
unsigned long fatNextFreeCluster(unsigned long cluster)
{
	unsigned int index;
	unsigned long sector;
	unsigned long *fat32Buffer;
	unsigned int *fat16Buffer;

	fat32Buffer= (unsigned long *)FatCache;
	fat16Buffer= (unsigned int *)FatCache;

	if (Fat32Enabled)
		sector= (cluster>>7) + FirstFATSector;// 128 structures of 4 bytes each as a pointer to a cluster
	else
		sector= (cluster>>8) + FirstFATSector;// 256 structures of 2 bytes each as a pointer to a cluster

	index=0;
	do
	{
		ataReadSectors( DRIVE0, sector, FatCache, &FatInCache);
		if (Fat32Enabled)
		{
			for (index=0; index < FAT32_STRUCTS_PER_SECTOR; index++) //Read all Cluster
			{
				if ((fat32Buffer[index]&FAT32_MASK) == 0x00000000)
					return (index + ((sector - FirstFATSector)*FAT32_STRUCTS_PER_SECTOR));
			}
		}
		else //FAT16
		{
			for (index=0; index < FAT16_STRUCTS_PER_SECTOR; index++) //Read all Cluster
			{
				if ((fat16Buffer[index]&FAT16_MASK) == 0x0000)
					return (index + ((sector - FirstFATSector)*FAT16_STRUCTS_PER_SECTOR));
			}
		}
		sector++;
    }
	while  (sector < FirstFATSector+FatSectors); // end of FatDirentries
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
void fatWriteEOC(unsigned long cluster)
{
	fatWrite(cluster, FAT32_MASK & CLUST_EOFE);
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
void fatWrite(unsigned long cluster, unsigned long data)
{
	unsigned int offset;
	unsigned long *Fat32CacheLong, sector;
	unsigned int  *Fat16CacheInt;

	// calculate offset of the our entry within that FAT sector
	if(Fat32Enabled)
		offset = cluster % FAT32_STRUCTS_PER_SECTOR;
	else
		offset = cluster % FAT16_STRUCTS_PER_SECTOR;

	// read the FAT sector, with has information abou the cluster that we are interested in.
	sector= fatTableClustToSect(cluster);

	ataReadSectors( DRIVE0,	sector, FatCache, &FatInCache);

	// write the data to Fat Cache
	if (Fat32Enabled)
	{
		Fat32CacheLong= (unsigned long *)FatCache;
		Fat32CacheLong[offset]= (Fat32CacheLong[offset]&(~FAT32_MASK)) | (data&FAT32_MASK);
	}
	else
	{
		Fat16CacheInt= (unsigned int *)FatCache;
		Fat16CacheInt[offset]= (Fat16CacheInt[offset]&(~FAT16_MASK)) | (data&FAT16_MASK);
	}
	// write Fat Cache in the HardDisk
	ataWriteSectors( DRIVE0, sector, FatCache);	// FAT1
	ataWriteSectors( DRIVE0, sector+(FirstFAT2Sector-FirstFATSector), FatCache); // FAT2
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
unsigned long fatTableClustToSect(unsigned long cluster)
{
	unsigned long sector, fatOffset;

	// get fat offset in bytes
	if(Fat32Enabled)
		fatOffset = cluster << 2;
	else
		fatOffset = cluster << 1;

	// calculate the FAT sector that we're interested in
	sector = FirstFATSector + (fatOffset / BYTES_PER_SECTOR);
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
unsigned long fatLastCluster(unsigned long cluster)
{
	unsigned long lastCluster;

	do
	{
		lastCluster=cluster;
		cluster = fatNextCluster(lastCluster);
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
unsigned char fatDirectoryIsEmpty(unsigned long DirCluster)
{
	struct direntry *de;
	unsigned long sector;

	do
	{
		sector=fatClustToSect(DirCluster);	//r ead directory clusters
		for (unsigned long isector=0; isector < SectorsPerCluster ;isector++)	// read one cluster
		{
			ataReadSectors( DRIVE0, sector + isector, SectorBuffer, &SectorInCache);
			de= (struct direntry *)SectorBuffer;
			for (int index=0; index<FAT_DIRENTRIES_PER_SECTOR; index++)	// read one sector
			{
				if	(*de->deName != '.' )
				{
					if ( *de->deName == SLOT_EMPTY )
						return TRUE;
					if ( *de->deName != SLOT_DELETED)
						return FALSE;
				}
				// If goes until here it's because de->Name is equal '.' or is equal SLOT_DELETED
				de++;
			}
		}
		DirCluster=fatNextCluster(DirCluster);
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
void fatRemoveAll(void)
{
	unsigned long offset=0;
	struct direntry *de = 0;	// avoid compiler warning by initializing
	int index;

	for (offset=0; ; offset++)
	{
		de = (struct direntry *) fatDir(currentDirCluster, offset);
		if (de == NULL)
			return;
		for (index=0; index<FAT_DIRENTRIES_PER_SECTOR; index++)
		{

			if (*de->deName == SLOT_EMPTY)
				return;		// there is no more direntries

			if (( de->deAttributes & ATTR_VOLUME ) != ATTR_VOLUME)
				if ((*de->deName != SLOT_DELETED) && (*de->deName != '.'))
					fatRemove(de->deName);
			de++;
		}	// end of sector
	}	// end of cluster
}
#endif
////////////////////
