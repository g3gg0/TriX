//*****************************************************************************
//
// Title		: FAT16/32 file system driver for ATMEL AVR
// Authors		: Angelo Bannack, Giordano Bruno Wolaniuk
// File Name	: 'fat.h'
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

#ifndef FAT_H
#define FAT_H

#include "global.h"


// Some useful cluster numbers
#define MSDOSFSROOT     0               // cluster 0 means the root dir
#define CLUST_FREE      0               // cluster 0 also means a free cluster
#define MSDOSFSFREE     CLUST_FREE
#define CLUST_FIRST     2               // first legal cluster number
#define CLUST_RSRVD     0xfffffff6      // reserved cluster range
#define CLUST_BAD       0xfffffff7      // a cluster with a defect
#define CLUST_EOFS      0xfffffff8      // start of eof cluster range
#define CLUST_EOFE      0xffffffff      // end of eof cluster range

#define FAT16_MASK      0x0000ffff      // mask for 16 bit cluster numbers
#define FAT32_MASK      0x0fffffff      // mask for FAT32 cluster numbers


// Partition Type used in the partition record
#define PART_TYPE_UNKNOWN		0x00
#define PART_TYPE_FAT12			0x01
#define PART_TYPE_XENIX			0x02
#define PART_TYPE_DOSFAT16		0x04
#define PART_TYPE_EXTDOS		0x05
#define PART_TYPE_FAT16			0x06
#define PART_TYPE_NTFS			0x07
#define PART_TYPE_FAT32			0x0B
#define PART_TYPE_FAT32LBA		0x0C
#define PART_TYPE_FAT16LBA		0x0E
#define PART_TYPE_EXTDOSLBA		0x0F
#define PART_TYPE_ONTRACK		0x33
#define PART_TYPE_NOVELL		0x40
#define PART_TYPE_PCIX			0x4B
#define PART_TYPE_PHOENIXSAVE	0xA0
#define PART_TYPE_CPM			0xDB
#define PART_TYPE_DBFS			0xE0
#define PART_TYPE_BBT			0xFF


#define BYTES_PER_SECTOR			512 //bytes
#define DIRENTRY_SIZE				32	//bytes
#define FAT_DIRENTRIES_PER_SECTOR	BYTES_PER_SECTOR/DIRENTRY_SIZE
#define FAT32_STRUCTS_PER_SECTOR	128
#define FAT16_STRUCTS_PER_SECTOR	256

#define SEEK_SET					0x00
#define SEEK_CUR					0x01
#define SEEK_END					0x02

struct partrecord // length 16 bytes
{
	unsigned char	prIsActive;					// 0x80 indicates active partition
	unsigned char	prStartHead;				// starting head for partition
	unsigned int	prStartCylSect;				// starting cylinder and sector
	unsigned char	prPartType;					// partition type (see above)
	unsigned char	prEndHead;					// ending head for this partition
	unsigned int	prEndCylSect;				// ending cylinder and sector
	unsigned long	prStartLBA;					// first LBA sector for this partition
	unsigned long	prSize;						// size of this partition (bytes or sectors ?)
};


struct partsector
{
	char			psPartCode[512-64-2];		// pad so struct is 512b
	unsigned char	psPart[64];					// four partition records (64 bytes)
	unsigned char	psBootSectSig0;				// two signature bytes (2 bytes)
	unsigned char	psBootSectSig1;
	#define 		BOOTSIG0        0x55
	#define 		BOOTSIG1        0xaa
};



// Format of a boot sector.  This is the first sector on a DOS floppy disk
// or the first sector of a partition on a hard disk.  But, it is not the
// first sector of a partitioned hard disk.
struct bootsector710 {
	unsigned char	bsJump[3];					// jump inst E9xxxx or EBxx90
	char			bsOEMName[8];				// OEM name and version
	char			bsBPB[53];					// BIOS parameter block
	char			bsExt[26];					// Bootsector Extension
	char			bsBootCode[418];			// pad so structure is 512b
	unsigned char	bsBootSectSig2;				// 2 & 3 are only defined for FAT32?
	unsigned char	bsBootSectSig3;
	unsigned char	bsBootSectSig0;				// boot sector signature byte 0x55
	unsigned char	bsBootSectSig1;				// boot sector signature byte 0xAA
	#define 		BOOTSIG0        0x55
	#define 		BOOTSIG1        0xaa
	#define 		BOOTSIG2        0
	#define 		BOOTSIG3        0
};


/***************************************************************/
/***************************************************************/
// BPB for DOS 7.10 (FAT32)
// This one has a few extensions to bpb50.
struct bpb710 {
	unsigned int	bpbBytesPerSec;	// bytes per sector
	unsigned char	bpbSecPerClust;	// sectors per cluster
	unsigned int	bpbResSectors;	// number of reserved sectors
	unsigned char	bpbFATs;		// number of FATs
	unsigned int	bpbRootDirEnts;	// number of root directory entries
	unsigned int	bpbSectors;		// total number of sectors
	unsigned char	bpbMedia;		// media descriptor
	unsigned int	bpbFATsecs;		// number of sectors per FAT
	unsigned int	bpbSecPerTrack;	// sectors per track
	unsigned int	bpbHeads;		// number of heads
	unsigned long	bpbHiddenSecs;	// # of hidden sectors
	// 3.3 compat ends here
	unsigned long	bpbHugeSectors;	// # of sectors if bpbSectors == 0
	// 5.0 compat ends here
	unsigned long	bpbBigFATsecs;	// like bpbFATsecs for FAT32
	unsigned int	bpbExtFlags;	// extended flags:
	#define FATNUM    0xf			// mask for numbering active FAT
	#define FATMIRROR 0x80			// FAT is mirrored (like it always was)
	unsigned int	bpbFSVers;		// filesystem version
	#define			FSVERS    0		// currently only 0 is understood
	unsigned long	bpbRootClust;	// start cluster for root directory
	unsigned int	bpbFSInfo;		// filesystem info structure sector
	unsigned int	bpbBackup;		// backup boot sector
	unsigned char	bpbReserved[12];
	unsigned char	bpbDriveNum;
	unsigned char	bpbReserved1;
	unsigned char	bpbBootSig;
	unsigned char	bpbVolID[4];
	unsigned char	bpbVolLabel[11];
	unsigned char	bpbFileSystemType[8];
};



// ***************************************************************
// * byte versions of the above structs                          *
// ***************************************************************

// BPB for DOS 7.10 (FAT32).
// This one has a few extensions to bpb50.
struct byte_bpb710 {
	unsigned char bpbBytesPerSec[2];     // bytes per sector
	unsigned char bpbSecPerClust;        // sectors per cluster
	unsigned char bpbResSectors[2];      // number of reserved sectors
	unsigned char bpbFATs;               // number of FATs
	unsigned char bpbRootDirEnts[2];     // number of root directory entries
	unsigned char bpbSectors[2];         // total number of sectors
	unsigned char bpbMedia;              // media descriptor
	unsigned char bpbFATsecs[2];         // number of sectors per FAT
	unsigned char bpbSecPerTrack[2];     // sectors per track
	unsigned char bpbHeads[2];           // number of heads
	unsigned char bpbHiddenSecs[4];      // # of hidden sectors
	unsigned char bpbHugeSectors[4];     // # of sectors if bpbSectors == 0
	unsigned char bpbBigFATsecs[4];      // like bpbFATsecs for FAT32
	unsigned char bpbExtFlags[2];        // extended flags:
	unsigned char bpbFSVers[2];          // filesystem version
	unsigned char bpbRootClust[4];       // start cluster for root directory
	unsigned char bpbFSInfo[2];          // filesystem info structure sector
	unsigned char bpbBackup[2];          // backup boot sector
	// There is a 12 byte filler here, but we ignore it
};

// FAT32 FSInfo block.
struct fsinfo {
	unsigned char fsisig1[4];
	unsigned char fsifill1[480];
	unsigned char fsisig2[4];
	unsigned char fsinfree[4];
	unsigned char fsinxtfree[4];
	unsigned char fsifill2[12];
	unsigned char fsisig3[4];
	unsigned char fsifill3[508];
	unsigned char fsisig4[4];
};


/***************************************************************/
/***************************************************************/


// Structure of a dos directory entry.
struct direntry {
	unsigned char	deName[8];      			// filename, blank filled
	#define 		SLOT_EMPTY      	0x00	// slot has never been used
	#define 		SLOT_E5         	0x05	// the real value is 0xe5
	#define 		SLOT_DELETED    	0xe5	// file in this slot deleted
	unsigned char	deExtension[3]; 			// extension, blank filled
	unsigned char	deAttributes;   			// file attributes
	#define 		ATTR_NORMAL     	0x00	// normal file
	#define 		ATTR_READONLY   	0x01	// file is readonly
	#define 		ATTR_HIDDEN     	0x02	// file is hidden
	#define 		ATTR_SYSTEM     	0x04	// file is a system file
	#define 		ATTR_VOLUME     	0x08	// entry is a volume label
	#define 		ATTR_LONG_FILENAME	0x0f	// this is a long filename entry
	#define 		ATTR_DIRECTORY  	0x10	// entry is a directory name
	#define 		ATTR_ARCHIVE    	0x20	// file is new or modified
	unsigned char	deLowerCase;    			// NT VFAT lower case flags
	#define 		LCASE_BASE      	0x08	// filename base in lower case
	#define 		LCASE_EXT       	0x10	// filename extension in lower case
	unsigned char	deCHundredth;				// hundredth of seconds in CTime
	unsigned char	deCTime[2];					// create time
	unsigned char	deCDate[2];					// create date
	unsigned char	deADate[2];					// access date
	unsigned int 	deHighClust;				// high bytes of cluster number
	unsigned char	deMTime[2];					// last update time
	unsigned char	deMDate[2];					// last update date
	unsigned int 	deStartCluster;				// starting cluster of file
	unsigned long	deFileSize;					// size of file in bytes
};


// Internal structure of a FILE, used in the program, not writed in FAT
typedef struct{
	struct direntry	de;					// Information about the file opened
	unsigned int	currentSector;  	// Actual sector address in memory
	unsigned char  *buffer;				// buffer pointer to memory (cache sector)
	unsigned long	bytePointer;		// byte pointer to the actual byte (divide by 512 to find the current buffer position)
	unsigned char	sectorHasChanged;	// TRUE if the sector in memory has changed and needs to be write before a close file or change in sector
}TFILE;


// number of directory entries in one sector
#define DIRENTRIES_PER_SECTOR	0x10

// Structure of a Win95 long name directory entry
struct winentry {
	unsigned char		weCnt;
	#define WIN_LAST	0x40
	#define WIN_CNT		0x3f
	unsigned char		wePart1[10];
	unsigned char		weAttributes;
	#define ATTR_WIN95	0x0f
	unsigned char		weReserved1;
	unsigned char		weChksum;
	unsigned char		wePart2[12];
	unsigned int 		weReserved2;
	unsigned char		wePart3[4];
};

#define WIN_CHARS	13      // Number of chars per winentry

// Maximum filename length in Win95
// Note: Must be < sizeof(dirent.d_name)
#define WIN_MAXLEN      255


// Prototypes
unsigned char      fatInit               (void);
unsigned int       fatClusterSize        (void);
unsigned long      fatNextCluster        (unsigned long cluster);
unsigned long      fatGetFirstDirCluster (void);
unsigned long      fatClustToSect        (unsigned long clust);
unsigned long      fatSectToClust        (unsigned long sect);
unsigned char     *fatDir                (unsigned long cluster, unsigned long offset);
struct direntry   *fatGetFileInfo        (struct direntry *rde, char *shortName);
char              *fatGetVolLabel        (void);
struct partrecord *fatGetPartInfo        (void);
unsigned int       fatGetSecPerClust     (void);
unsigned long      fatGetFirstFATSector  (void);
unsigned long      fatGetFirstFAT2Sector (void);
unsigned long      fatGetFirstDataSector (void);
unsigned long      fatGetNumClusters     (void);
unsigned char      fatCddir              (char *path);
TFILE             *fatFopen              (char *shortName);
char               fatFgetc              (TFILE *fp);
unsigned int       fatFseek              (TFILE *fp, unsigned long offSet, unsigned char mode);
unsigned char      fatFeof               (TFILE *fp);
void               fatNormalize          (char *string);
unsigned long      fatGetCurDirCluster   (void);
////////////////////
#ifndef ATA_READ_ONLY
unsigned char      fatMkdir              (char *path);
unsigned char      fatRename             (char *oldShortName, char *newShortName);
unsigned char      fatRemove             (char *shortName);
TFILE             *fatFcreate            (char *shortName);
unsigned char      fatFclose             (TFILE *fp);
unsigned char      fatFflush             (TFILE *fp);
unsigned char      fatFputc              (TFILE *fp, char c);
struct direntry   *fatNextFreeDirEntry   (unsigned long cluster);
unsigned long      fatNextFreeCluster    (unsigned long startSector);
void               fatWriteEOC           (unsigned long cluster);
void               fatWrite              (unsigned long cluster, unsigned long data);
unsigned long      fatTableClustToSect   (unsigned long cluster);
unsigned long      fatLastCluster        (unsigned long cluster);
unsigned char      fatDirectoryIsEmpty   (unsigned long DirCluster);
void               fatRemoveAll          (void);
#endif
////////////////////
#endif

