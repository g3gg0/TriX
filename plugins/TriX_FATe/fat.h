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
#define CLUST_FIRST     0               // first legal cluster number
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

typedef unsigned char   u8;
typedef unsigned short  u16;
typedef unsigned int    u32;

struct partrecord // length 16 bytes
{
	u8	prIsActive;					// 0x80 indicates active partition
	u8	prStartHead;				// starting head for partition
	u16	prStartCylSect;				// starting cylinder and sector
	u8	prPartType;					// partition type (see above)
	u8	prEndHead;					// ending head for this partition
	u16	prEndCylSect;				// ending cylinder and sector
	u32	prStartLBA;					// first LBA sector for this partition
	u32	prSize;						// size of this partition (bytes or sectors ?)
};


struct partsector
{
	char			psPartCode[512-64-2];		// pad so struct is 512b
	u8	psPart[64];					// four partition records (64 bytes)
	u8	psBootSectSig0;				// two signature bytes (2 bytes)
	u8	psBootSectSig1;
	#define 		BOOTSIG0        0x55
	#define 		BOOTSIG1        0xaa
};



// Format of a boot sector.  This is the first sector on a DOS floppy disk
// or the first sector of a partition on a hard disk.  But, it is not the
// first sector of a partitioned hard disk.
struct bootsector710 {
	u8	bsJump[3];					// jump inst E9xxxx or EBxx90
	char			bsOEMName[8];				// OEM name and version
	char			bsBPB[53];					// BIOS parameter block
	char			bsExt[26];					// Bootsector Extension
	char			bsBootCode[418];			// pad so structure is 512b
	u8	bsBootSectSig2;				// 2 & 3 are only defined for FAT32?
	u8	bsBootSectSig3;
	u8	bsBootSectSig0;				// boot sector signature byte 0x55
	u8	bsBootSectSig1;				// boot sector signature byte 0xAA
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
	u16	bpbBytesPerSec;	// bytes per sector
	u8	bpbSecPerClust;	// sectors per cluster
	u16	bpbResSectors;	// number of reserved sectors
	u8	bpbFATs;		// number of FATs
	u16	bpbRootDirEnts;	// number of root directory entries
	u16	bpbSectors;		// total number of sectors
	u8	bpbMedia;		// media descriptor
	u16	bpbFATsecs;		// number of sectors per FAT
	u16	bpbSecPerTrack;	// sectors per track
	u16	bpbHeads;		// number of heads
	u32	bpbHiddenSecs;	// # of hidden sectors
	// 3.3 compat ends here
	u32	bpbHugeSectors;	// # of sectors if bpbSectors == 0
	// 5.0 compat ends here
	u32	bpbBigFATsecs;	// like bpbFATsecs for FAT32
	u16	bpbExtFlags;	// extended flags:
	#define FATNUM    0xf			// mask for numbering active FAT
	#define FATMIRROR 0x80			// FAT is mirrored (like it always was)
	u16	bpbFSVers;		// filesystem version
	#define			FSVERS    0		// currently only 0 is understood
	u32	bpbRootClust;	// start cluster for root directory
	u16	bpbFSInfo;		// filesystem info structure sector
	u16	bpbBackup;		// backup boot sector
	u8	bpbReserved[12];
	u8	bpbDriveNum;
	u8	bpbReserved1;
	u8	bpbBootSig;
	u8	bpbVolID[4];
	u8	bpbVolLabel[11];
	u8	bpbFileSystemType[8];
};

typedef struct
{
	struct bpb710 *bpb;
	t_workspace *ws;
	u8 VolLabel[12];							// Volume Label
	u32 FSInfo;						// File System Information
	u8 Fat32Enabled;					// Indicates if is FAT32 or FAT16
	u32 FirstDataSector;				// First Data Sector Address
	u16  SectorsPerCluster;			// Number of Sectors per Cluster
	u32 FirstFATSector;				// First FAT Sector Address
	u32 FirstFAT2Sector;				// First FAT2 Sector Address
	u32 FirstDirCluster;				// First Directory (Data) Cluster Address
	u32 FatInCache;		// Address of the FAT Cluster in FatCache
	u32 FatSectors;					// Number of FAT Sectors
	u32 currentDirCluster;			// Actual Dir Cluster Number
	u32 NumClusters;					// ATA Dispositive Cluster Numbers
	u32 SectorInCache;	// Address of the Sector Cluster in SectorBuffer
	u8 prPartType;

} t_fat_info;


// ***************************************************************
// * byte versions of the above structs                          *
// ***************************************************************

// BPB for DOS 7.10 (FAT32).
// This one has a few extensions to bpb50.
struct byte_bpb710 {
	u8 bpbBytesPerSec[2];     // bytes per sector
	u8 bpbSecPerClust;        // sectors per cluster
	u8 bpbResSectors[2];      // number of reserved sectors
	u8 bpbFATs;               // number of FATs
	u8 bpbRootDirEnts[2];     // number of root directory entries
	u8 bpbSectors[2];         // total number of sectors
	u8 bpbMedia;              // media descriptor
	u8 bpbFATsecs[2];         // number of sectors per FAT
	u8 bpbSecPerTrack[2];     // sectors per track
	u8 bpbHeads[2];           // number of heads
	u8 bpbHiddenSecs[4];      // # of hidden sectors
	u8 bpbHugeSectors[4];     // # of sectors if bpbSectors == 0
	u8 bpbBigFATsecs[4];      // like bpbFATsecs for FAT32
	u8 bpbExtFlags[2];        // extended flags:
	u8 bpbFSVers[2];          // filesystem version
	u8 bpbRootClust[4];       // start cluster for root directory
	u8 bpbFSInfo[2];          // filesystem info structure sector
	u8 bpbBackup[2];          // backup boot sector
	// There is a 12 byte filler here, but we ignore it
};

// FAT32 FSInfo block.
struct fsinfo {
	u8 fsisig1[4];
	u8 fsifill1[480];
	u8 fsisig2[4];
	u8 fsinfree[4];
	u8 fsinxtfree[4];
	u8 fsifill2[12];
	u8 fsisig3[4];
	u8 fsifill3[508];
	u8 fsisig4[4];
};


/***************************************************************/
/***************************************************************/


// Structure of a dos directory entry.
struct direntry {
	u32 deLFNFirstSector;
	u32 deLFNFirstEntry;
	u8  deLFNstate;
	u8  deLFNcount;
	u8  deLFNValid;				// correct LFN entry?
	u8  deWriteRaw;				// write raw direntry content?
	u8  deRaw[0x20];				// raw direntry content
	u8  deLFNName[257];				// long filename name
	u8  deLFNchecksum;
	u8	deName[9];      			// filename, blank filled
	#define 		SLOT_EMPTY      	0x00	// slot has never been used
	#define 		SLOT_E5         	0x05	// the real value is 0xe5
	#define 		SLOT_DELETED    	0xe5	// file in this slot deleted
	u8	deExtension[4]; 			// extension, blank filled
	u8	deAttributes;   			// file attributes
	#define 		ATTR_NORMAL     	0x00	// normal file
	#define 		ATTR_READONLY   	0x01	// file is readonly
	#define 		ATTR_HIDDEN     	0x02	// file is hidden
	#define 		ATTR_SYSTEM     	0x04	// file is a system file
	#define 		ATTR_VOLUME     	0x08	// entry is a volume label
	#define 		ATTR_LONG_FILENAME	0x0f	// this is a long filename entry
	#define 		ATTR_DIRECTORY  	0x10	// entry is a directory name
	#define 		ATTR_ARCHIVE    	0x20	// file is new or modified
	u8	deLowerCase;    			// NT VFAT lower case flags
	#define 		LCASE_BASE      	0x08	// filename base in lower case
	#define 		LCASE_EXT       	0x10	// filename extension in lower case
	u8	deCHundredth;				// hundredth of seconds in CTime
	u8	deCTime[2];					// create time
	u8	deCDate[2];					// create date
	u8	deADate[2];					// access date
	u16	deHighClust;				// high bytes of cluster number
	u8	deMTime[2];					// last update time
	u8	deMDate[2];					// last update date
	u16	deStartCluster;				// starting cluster of file
	u32	deFileSize;					// size of file in bytes

	t_fat_info *info;
	u32 srcaddr;
	u32 deSector;
	u32 deEntry;
};


// Internal structure of a FILE, used in the program, not writed in FAT
typedef struct{
	struct direntry	de;					// Information about the file opened
	u16	currentSector;  	// Actual sector address in memory
	u8  *buffer;				// buffer pointer to memory (cache sector)
	u32	bytePointer;		// byte pointer to the actual byte (divide by 512 to find the current buffer position)
	u8	sectorHasChanged;	// TRUE if the sector in memory has changed and needs to be write before a close file or change in sector
	t_fat_info *info;
}TFILE;


// number of directory entries in one sector
#define DIRENTRIES_PER_SECTOR	0x10

// Structure of a Win95 long name directory entry
struct winentry {
	u8		weCnt;
	#define WIN_LAST	0x40
	#define WIN_CNT		0x3f
	u8		wePart1[10];
	u8		weAttributes;
	#define ATTR_WIN95	0x0f
	u8		weReserved1;
	u8		weChksum;
	u8		wePart2[12];
	u16 		weReserved2;
	u8		wePart3[4];
};

#define WIN_CHARS	13      // Number of chars per winentry

// Maximum filename length in Win95
// Note: Must be < sizeof(dirent.d_name)
#define WIN_MAXLEN      255


// Prototypes
t_fat_info *fatInit ( t_workspace *workspace, unsigned char prPartType );
u16      fatClusterSize        (t_fat_info *info);
u32      fatNextCluster        (t_fat_info *info, u32 cluster);
u32      fatGetFirstDirCluster (t_fat_info *info);
u32      fatClustToSect        (t_fat_info *info, u32 clust);
u32      fatSectToClust        (t_fat_info *info, u32 sect);
struct direntry      *fatDir                (t_fat_info *info, u32 cluster, u32 offset);
struct direntry   *fatGetFileInfo        (t_fat_info *info, struct direntry *rde, char *shortName);
char              *fatGetVolLabel        (t_fat_info *info);
struct partrecord *fatGetPartInfo        (t_fat_info *info);
u16      fatGetSecPerClust     (t_fat_info *info);
u32      fatGetFirstFATSector  (t_fat_info *info);
u32      fatGetFirstFAT2Sector (t_fat_info *info);
u32      fatGetFirstDataSector (t_fat_info *info);
u32      fatGetNumClusters     (t_fat_info *info);
u8       fatCddir              (t_fat_info *info, char *path);
TFILE   *fatFopen              (t_fat_info *info, char *shortName);
unsigned char     fatFgetc     ( TFILE *fp );
long     fatFtell ( TFILE *fp );
u16      fatFseek              ( TFILE *fp, u32 offSet, u8 mode );
u8       fatFeof               ( TFILE *fp );
void     fatNormalize          (t_fat_info *info, char *string );
u32      fatGetCurDirCluster   (t_fat_info *info);
////////////////////
#ifndef ATA_READ_ONLY
u8      fatMkdir              ( t_fat_info *info, char *path);
u8      fatRename             ( t_fat_info *info, char *oldShortName, char *newShortName);
u8      fatRemove             ( t_fat_info *info, char *shortName);
TFILE  *fatFcreate            ( t_fat_info *info, char *shortName);
u8      fatFclose             ( TFILE *fp);
u8      fatFflush             ( TFILE *fp);
u8      fatFputc              ( TFILE *fp, unsigned char c);
struct direntry   *fatNextFreeDirEntry   (t_fat_info *info, u32 cluster);
u32     fatNextFreeCluster    (t_fat_info *info, u32 startSector);
void    fatWriteEOC           (t_fat_info *info, u32 cluster);
void    fatWrite              (t_fat_info *info, u32 cluster, u32 data);
u32     fatTableClustToSect   (t_fat_info *info, u32 cluster);
u32     fatLastCluster        (t_fat_info *info, u32 cluster);
u8      fatDirectoryIsEmpty   (t_fat_info *info, u32 DirCluster);
void    fatRemoveAll          (t_fat_info *info);
struct direntry  *fatNextFreeDirEntryCount( t_fat_info *info, u32 cluster, u32 min_entries );
int     fatExtendCurrentDir   ( t_fat_info *info );
int     fatFindUnique         ( t_fat_info *info, char *str_dest );
int     fatProcessLFN         ( struct direntry *lfn_de, struct direntry *de );
int     fatWriteLFN           ( t_fat_info *info, struct direntry *src_de, struct direntry *dst_de );
#endif
////////////////////
#endif

