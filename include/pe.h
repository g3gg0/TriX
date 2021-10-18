#ifndef __TRIX_PE_H__
#define __TRIX_PE_H__

/* Image format */

#define IMAGE_DOS_SIGNATURE                     0x5A4D      // MZ
#define IMAGE_OS2_SIGNATURE                     0x454E      // NE
#define IMAGE_OS2_SIGNATURE_LE                  0x454C      // LE
#define IMAGE_VXD_SIGNATURE                     0x454C      // LE
#define IMAGE_NT_SIGNATURE                      0x00004550  // PE00


typedef struct
{
	unsigned short e_magic;             // Magic number
	unsigned short e_cblp;              // Bytes on last page of file
	unsigned short e_cp;                // Pages in file
	unsigned short e_crlc;              // Relocations
	unsigned short e_cparhdr;           // Size of header in paragraphs
	unsigned short e_minalloc;          // Minimum extra paragraphs needed
	unsigned short e_maxalloc;          // Maximum extra paragraphs needed
	unsigned short e_ss;                // Initial (relative) SS value
	unsigned short e_sp;                // Initial SP value
	unsigned short e_csum;              // Checksum
	unsigned short e_ip;                // Initial IP value
	unsigned short e_cs;                // Initial (relative) CS value
	unsigned short e_lfarlc;            // File address of relocation table
	unsigned short e_ovno;              // Overlay number
	unsigned char  e_res[8];            // Reserved words
	unsigned short e_oemid;             // OEM identifier (for e_oeminfo)
	unsigned short e_oeminfo;           // OEM information; e_oemid specific
	unsigned char  e_res2[20];          // Reserved words
	int e_lfanew;                       // File address of new exe header
} t_dos_header;



/* File header format */

typedef struct
{
	unsigned short Machine;
	unsigned short NumberOfSections;
	unsigned int   TimeDateStamp;
	unsigned int   PointerToSymbolTable;
	unsigned int   NumberOfSymbols;
	unsigned short SizeOfOptionalHeader;
	unsigned short Characteristics;
} t_image_file_header;

#define IMAGE_SIZEOF_FILE_HEADER             20

#define IMAGE_FILE_RELOCS_STRIPPED           0x0001   // Relocation info stripped from file.
#define IMAGE_FILE_EXECUTABLE_IMAGE          0x0002   // File is executable  (i.e. no unresolved externel references).
#define IMAGE_FILE_LINE_NUMS_STRIPPED        0x0004   // Line nunbers stripped from file.
#define IMAGE_FILE_LOCAL_SYMS_STRIPPED       0x0008   // Local symbols stripped from file.
#define IMAGE_FILE_AGGRESIVE_WS_TRIM         0x0010   // Agressively trim working set
#define IMAGE_FILE_LARGE_ADDRESS_AWARE       0x0020   // App can handle >2gb addresses
#define IMAGE_FILE_BYTES_REVERSED_LO         0x0080   // Bytes of machine word are reversed.
#define IMAGE_FILE_32BIT_MACHINE             0x0100   // 32 bit word machine.
#define IMAGE_FILE_DEBUG_STRIPPED            0x0200   // Debugging info stripped from file in .DBG file
#define IMAGE_FILE_REMOVABLE_RUN_FROM_SWAP   0x0400   // If Image is on removable media, copy and run from the swap file.
#define IMAGE_FILE_NET_RUN_FROM_SWAP         0x0800   // If Image is on Net, copy and run from the swap file.
#define IMAGE_FILE_SYSTEM                    0x1000   // System File.
#define IMAGE_FILE_DLL                       0x2000   // File is a DLL.
#define IMAGE_FILE_UP_SYSTEM_ONLY            0x4000   // File should only be run on a UP machine
#define IMAGE_FILE_BYTES_REVERSED_HI         0x8000   // Bytes of machine word are reversed.

#define IMAGE_FILE_MACHINE_UNKNOWN           0
#define IMAGE_FILE_MACHINE_I386              0x014c   // Intel 386
#define IMAGE_FILE_MACHINE_R3000             0x0162   // MIPS little-endian, 0x160 big-endian
#define IMAGE_FILE_MACHINE_R4000             0x0166   // MIPS little-endian
#define IMAGE_FILE_MACHINE_R10000            0x0168   // MIPS little-endian
#define IMAGE_FILE_MACHINE_WCEMIPSV2         0x0169   // MIPS little-endian WCE v2
#define IMAGE_FILE_MACHINE_ALPHA             0x0184   // Alpha_AXP
#define IMAGE_FILE_MACHINE_POWERPC           0x01F0   // IBM PowerPC Little-Endian
#define IMAGE_FILE_MACHINE_SH3               0x01a2   // SH3 little-endian
#define IMAGE_FILE_MACHINE_SH3E              0x01a4   // SH3E little-endian
#define IMAGE_FILE_MACHINE_SH3DSP            0x01a3   // SH3DSP little-endian
#define IMAGE_FILE_MACHINE_SH4               0x01a6   // SH4 little-endian
#define IMAGE_FILE_MACHINE_ARM               0x01c0   // ARM Little-Endian
#define IMAGE_FILE_MACHINE_THUMB             0x01c2
#define IMAGE_FILE_MACHINE_IA64              0x0200   // Intel 64
#define IMAGE_FILE_MACHINE_MIPS16            0x0266   // MIPS
#define IMAGE_FILE_MACHINE_MIPSFPU           0x0366   // MIPS
#define IMAGE_FILE_MACHINE_MIPSFPU16         0x0466   // MIPS
#define IMAGE_FILE_MACHINE_ALPHA64           0x0284   // ALPHA64
#define IMAGE_FILE_MACHINE_AXP64             IMAGE_FILE_MACHINE_ALPHA64



/* Directory format */

typedef struct
{
	unsigned int VirtualAddress;
	unsigned int Size;
} t_image_data_directory;

#define IMAGE_NUMBEROF_DIRECTORY_ENTRIES     16



/* Optional header format */

typedef struct
{
	// Standard fields
	unsigned short Magic;
	unsigned char  MajorLinkerVersion;
	unsigned char  MinorLinkerVersion;
	unsigned int   SizeOfCode;
	unsigned int   SizeOfInitializedData;
	unsigned int   SizeOfUninitializedData;
	unsigned int   AddressOfEntryPoint;
	unsigned int   BaseOfCode;
	unsigned int   BaseOfData;

	// NT additional fields
	unsigned int   ImageBase;
	unsigned int   SectionAlignment;
	unsigned int   FileAlignment;
	unsigned short MajorOperatingSystemVersion;
	unsigned short MinorOperatingSystemVersion;
	unsigned short MajorImageVersion;
	unsigned short MinorImageVersion;
	unsigned short MajorSubsystemVersion;
	unsigned short MinorSubsystemVersion;
	unsigned int   Reserved1;
	unsigned int   SizeOfImage;
	unsigned int   SizeOfHeaders;
	unsigned int   CheckSum;
	unsigned short Subsystem;
	unsigned short DllCharacteristics;
	unsigned int   SizeOfStackReserve;
	unsigned int   SizeOfStackCommit;
	unsigned int   SizeOfHeapReserve;
	unsigned int   SizeOfHeapCommit;
	unsigned int   LoaderFlags;
	unsigned int   NumberOfRvaAndSizes;

	t_image_data_directory DataDirectory[IMAGE_NUMBEROF_DIRECTORY_ENTRIES];
} t_image_optional_header32;

typedef struct
{
	unsigned short Magic;
	unsigned char  MajorLinkerVersion;
	unsigned char  MinorLinkerVersion;
	unsigned long  SizeOfCode;
	unsigned long  SizeOfInitializedData;
	unsigned long  SizeOfUninitializedData;
	unsigned long  AddressOfEntryPoint;
	unsigned long  BaseOfCode;
	unsigned long  BaseOfData;
	unsigned long  BaseOfBss;
	unsigned long  GprMask;
	unsigned long  CprMask[4];
	unsigned long  GpValue;
} t_image_rom_optional_header;

typedef struct
{
	unsigned short Magic;
	unsigned char  MajorLinkerVersion;
	unsigned char  MinorLinkerVersion;
	unsigned long  SizeOfCode;
	unsigned long  SizeOfInitializedData;
	unsigned long  SizeOfUninitializedData;
	unsigned long  AddressOfEntryPoint;
	unsigned long  BaseOfCode;
	unsigned __int64 ImageBase;
	unsigned long  SectionAlignment;
	unsigned long  FileAlignment;
	unsigned short MajorOperatingSystemVersion;
	unsigned short MinorOperatingSystemVersion;
	unsigned short MajorImageVersion;
	unsigned short MinorImageVersion;
	unsigned short MajorSubsystemVersion;
	unsigned short MinorSubsystemVersion;
	unsigned long  Win32VersionValue;
	unsigned long  SizeOfImage;
	unsigned long  SizeOfHeaders;
	unsigned long  CheckSum;
	unsigned short Subsystem;
	unsigned short DllCharacteristics;
	unsigned __int64 SizeOfStackReserve;
	unsigned __int64 SizeOfStackCommit;
	unsigned __int64 SizeOfHeapReserve;
	unsigned __int64 SizeOfHeapCommit;
	unsigned long  LoaderFlags;
	unsigned long  NumberOfRvaAndSizes;
	t_image_data_directory DataDirectory[IMAGE_NUMBEROF_DIRECTORY_ENTRIES];
} t_image_optional_header64;

#define IMAGE_NT_OPTIONAL_HDR32_MAGIC        0x10B
#define IMAGE_NT_OPTIONAL_HDR64_MAGIC        0x20B
#define IMAGE_ROM_OPTIONAL_HDR_MAGIC         0x107

typedef struct
{
	unsigned int Signature;
	t_image_file_header FileHeader;
	t_image_optional_header32 OptionalHeader;
} t_image_nt_header32;

typedef struct
{
	unsigned int Signature;
	t_image_file_header FileHeader;
	t_image_optional_header64 OptionalHeader;
} t_image_nt_header64;

typedef struct
{
	t_image_file_header FileHeader;
	t_image_rom_optional_header OptionalHeader;
} t_image_rom_header;


// Subsystem Values
#define IMAGE_SUBSYSTEM_UNKNOWN              0   // Unknown subsystem
#define IMAGE_SUBSYSTEM_NATIVE               1   // Image doesn't require a subsystem
#define IMAGE_SUBSYSTEM_WINDOWS_GUI          2   // Image runs in the Windows GUI subsystem
#define IMAGE_SUBSYSTEM_WINDOWS_CUI          3   // Image runs in the Windows character subsystem
#define IMAGE_SUBSYSTEM_OS2_CUI              5   // image runs in the OS/2 character subsystem
#define IMAGE_SUBSYSTEM_POSIX_CUI            7   // image runs in the Posix character subsystem
#define IMAGE_SUBSYSTEM_NATIVE_WINDOWS       8   // image is a native Win9x driver
#define IMAGE_SUBSYSTEM_WINDOWS_CE_GUI       9   // Image runs in the Windows CE subsystem


// Directory Entries
#define IMAGE_DIRECTORY_ENTRY_EXPORT          0   // Export Directory
#define IMAGE_DIRECTORY_ENTRY_IMPORT          1   // Import Directory
#define IMAGE_DIRECTORY_ENTRY_RESOURCE        2   // Resource Directory
#define IMAGE_DIRECTORY_ENTRY_EXCEPTION       3   // Exception Directory
#define IMAGE_DIRECTORY_ENTRY_SECURITY        4   // Security Directory
#define IMAGE_DIRECTORY_ENTRY_BASERELOC       5   // Base Relocation Table
#define IMAGE_DIRECTORY_ENTRY_DEBUG           6   // Debug Directory
#define IMAGE_DIRECTORY_ENTRY_COPYRIGHT       7   // (X86 usage)
#define IMAGE_DIRECTORY_ENTRY_ARCHITECTURE    7   // Architecture Specific Data
#define IMAGE_DIRECTORY_ENTRY_GLOBALPTR       8   // RVA of GP
#define IMAGE_DIRECTORY_ENTRY_TLS             9   // TLS Directory
#define IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG    10   // Load Configuration Directory
#define IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT   11   // Bound Import Directory in headers
#define IMAGE_DIRECTORY_ENTRY_IAT            12   // Import Address Table
#define IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT   13   // Delay Load Import Descriptors
#define IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR 14   // COM Runtime descriptor



/* Section header format */

#define IMAGE_SIZEOF_SHORT_NAME              8

typedef struct
{
	unsigned char Name[IMAGE_SIZEOF_SHORT_NAME];
	union
	{
		unsigned int PhysicalAddress;
		unsigned int VirtualSize;
	} Misc;
	unsigned int VirtualAddress;
	unsigned int SizeOfRawData;
	unsigned int PointerToRawData;
	unsigned int PointerToRelocations;
	unsigned int PointerToLinenumbers;
	unsigned short int NumberOfRelocations;
	unsigned short int NumberOfLinenumbers;
	unsigned int Characteristics;
} t_section_header;

#define IMAGE_SIZEOF_SECTION_HEADER          40


/* Section characteristics */

//      IMAGE_SCN_TYPE_REG                   0x00000000  // Reserved
//      IMAGE_SCN_TYPE_DSECT                 0x00000001  // Reserved
//      IMAGE_SCN_TYPE_NOLOAD                0x00000002  // Reserved
//      IMAGE_SCN_TYPE_GROUP                 0x00000004  // Reserved
#define IMAGE_SCN_TYPE_NO_PAD                0x00000008  // Reserved
//      IMAGE_SCN_TYPE_COPY                  0x00000010  // Reserved

#define IMAGE_SCN_CNT_CODE                   0x00000020  // Section contains code
#define IMAGE_SCN_CNT_INITIALIZED_DATA       0x00000040  // Section contains initialized data
#define IMAGE_SCN_CNT_UNINITIALIZED_DATA     0x00000080  // Section contains uninitialized data

#define IMAGE_SCN_LNK_OTHER                  0x00000100  // Reserved
#define IMAGE_SCN_LNK_INFO                   0x00000200  // Section contains comments or some other type of information
//      IMAGE_SCN_TYPE_OVER                  0x00000400  // Reserved
#define IMAGE_SCN_LNK_REMOVE                 0x00000800  // Section contents will not become part of image
#define IMAGE_SCN_LNK_COMDAT                 0x00001000  // Section contents comdat
//                                           0x00002000  // Reserved
//      IMAGE_SCN_MEM_PROTECTED - Obsolete   0x00004000
#define IMAGE_SCN_NO_DEFER_SPEC_EXC          0x00004000  // Reset speculative exceptions handling bits in the TLB entries for this section
#define IMAGE_SCN_GPREL                      0x00008000  // Section content can be accessed relative to GP
#define IMAGE_SCN_MEM_FARDATA                0x00008000
//      IMAGE_SCN_MEM_SYSHEAP  - Obsolete    0x00010000
#define IMAGE_SCN_MEM_PURGEABLE              0x00020000
#define IMAGE_SCN_MEM_16BIT                  0x00020000
#define IMAGE_SCN_MEM_LOCKED                 0x00040000
#define IMAGE_SCN_MEM_PRELOAD                0x00080000

#define IMAGE_SCN_ALIGN_1BYTES               0x00100000  //
#define IMAGE_SCN_ALIGN_2BYTES               0x00200000  //
#define IMAGE_SCN_ALIGN_4BYTES               0x00300000  //
#define IMAGE_SCN_ALIGN_8BYTES               0x00400000  //
#define IMAGE_SCN_ALIGN_16BYTES              0x00500000  // Default alignment if no others are specified
#define IMAGE_SCN_ALIGN_32BYTES              0x00600000  //
#define IMAGE_SCN_ALIGN_64BYTES              0x00700000  //
#define IMAGE_SCN_ALIGN_128BYTES             0x00800000  //
#define IMAGE_SCN_ALIGN_256BYTES             0x00900000  //
#define IMAGE_SCN_ALIGN_512BYTES             0x00A00000  //
#define IMAGE_SCN_ALIGN_1024BYTES            0x00B00000  //
#define IMAGE_SCN_ALIGN_2048BYTES            0x00C00000  //
#define IMAGE_SCN_ALIGN_4096BYTES            0x00D00000  //
#define IMAGE_SCN_ALIGN_8192BYTES            0x00E00000  //
// Unused                                    0x00F00000

#define IMAGE_SCN_LNK_NRELOC_OVFL            0x01000000  // Section contains extended relocations
#define IMAGE_SCN_MEM_DISCARDABLE            0x02000000  // Section can be discarded
#define IMAGE_SCN_MEM_NOT_CACHED             0x04000000  // Section is not cachable
#define IMAGE_SCN_MEM_NOT_PAGED              0x08000000  // Section is not pageable
#define IMAGE_SCN_MEM_SHARED                 0x10000000  // Section is shareable
#define IMAGE_SCN_MEM_EXECUTE                0x20000000  // Section is executable
#define IMAGE_SCN_MEM_READ                   0x40000000  // Section is readable
#define IMAGE_SCN_MEM_WRITE                  0x80000000  // Section is writeable


/* TLS Characteristic Flags */

#define IMAGE_SCN_SCALE_INDEX                0x00000001  // Tls index is scaled



/* Symbol format */

typedef struct
{
	union
	{
		unsigned char ShortName[8];
		struct
		{
			unsigned long Short;             // if 0, use LongName
			unsigned long Long;              // offset into string table
		} Name;
		unsigned char LongName[2];
	} N;
	unsigned long Value;
	short SectionNumber;
	unsigned short Type;
	unsigned char StorageClass;
	unsigned char NumberOfAuxSymbols;
} t_image_symbol;

#define IMAGE_SIZEOF_SYMBOL                  18



/* Section values */

/* Symbols have a section number of the section in which they are defined.
 * Otherwise, section numbers have the following meanings:
 */

#define IMAGE_SYM_UNDEFINED                  (short)0    // Symbol is undefined or is common
#define IMAGE_SYM_ABSOLUTE                   (short)-1   // Symbol is an absolute value
#define IMAGE_SYM_DEBUG                      (short)-2   // Symbol is a special debug item

// Type (fundamental) values
#define IMAGE_SYM_TYPE_NULL                  0x0000   // no type
#define IMAGE_SYM_TYPE_VOID                  0x0001
#define IMAGE_SYM_TYPE_CHAR                  0x0002   // type character
#define IMAGE_SYM_TYPE_SHORT                 0x0003   // type short integer
#define IMAGE_SYM_TYPE_INT                   0x0004
#define IMAGE_SYM_TYPE_LONG                  0x0005
#define IMAGE_SYM_TYPE_FLOAT                 0x0006
#define IMAGE_SYM_TYPE_DOUBLE                0x0007
#define IMAGE_SYM_TYPE_STRUCT                0x0008
#define IMAGE_SYM_TYPE_UNION                 0x0009
#define IMAGE_SYM_TYPE_ENUM                  0x000A   // enumeration
#define IMAGE_SYM_TYPE_MOE                   0x000B   // member of enumeration
#define IMAGE_SYM_TYPE_BYTE                  0x000C
#define IMAGE_SYM_TYPE_WORD                  0x000D
#define IMAGE_SYM_TYPE_UINT                  0x000E
#define IMAGE_SYM_TYPE_DWORD                 0x000F
#define IMAGE_SYM_TYPE_PCODE                 0x8000

// Type (derived) values
#define IMAGE_SYM_DTYPE_NULL                 0     // no derived type
#define IMAGE_SYM_DTYPE_POINTER              1     // pointer
#define IMAGE_SYM_DTYPE_FUNCTION             2     // function
#define IMAGE_SYM_DTYPE_ARRAY                3     // array

// Storage classes
#define IMAGE_SYM_CLASS_END_OF_FUNCTION      (unsigned char)-1
#define IMAGE_SYM_CLASS_NULL                 0x0000
#define IMAGE_SYM_CLASS_AUTOMATIC            0x0001
#define IMAGE_SYM_CLASS_EXTERNAL             0x0002
#define IMAGE_SYM_CLASS_STATIC               0x0003
#define IMAGE_SYM_CLASS_REGISTER             0x0004
#define IMAGE_SYM_CLASS_EXTERNAL_DEF         0x0005
#define IMAGE_SYM_CLASS_LABEL                0x0006
#define IMAGE_SYM_CLASS_UNDEFINED_LABEL      0x0007
#define IMAGE_SYM_CLASS_MEMBER_OF_STRUCT     0x0008
#define IMAGE_SYM_CLASS_ARGUMENT             0x0009
#define IMAGE_SYM_CLASS_STRUCT_TAG           0x000A
#define IMAGE_SYM_CLASS_MEMBER_OF_UNION      0x000B
#define IMAGE_SYM_CLASS_UNION_TAG            0x000C
#define IMAGE_SYM_CLASS_TYPE_DEFINITION      0x000D
#define IMAGE_SYM_CLASS_UNDEFINED_STATIC     0x000E
#define IMAGE_SYM_CLASS_ENUM_TAG             0x000F
#define IMAGE_SYM_CLASS_MEMBER_OF_ENUM       0x0010
#define IMAGE_SYM_CLASS_REGISTER_PARAM       0x0011
#define IMAGE_SYM_CLASS_BIT_FIELD            0x0012

#define IMAGE_SYM_CLASS_FAR_EXTERNAL         0x0044

#define IMAGE_SYM_CLASS_BLOCK                0x0064
#define IMAGE_SYM_CLASS_FUNCTION             0x0065
#define IMAGE_SYM_CLASS_END_OF_STRUCT        0x0066
#define IMAGE_SYM_CLASS_FILE                 0x0067

// new
#define IMAGE_SYM_CLASS_SECTION              0x0068
#define IMAGE_SYM_CLASS_WEAK_EXTERNAL        0x0069



/* TODO */


#endif /* __TRIX_PE_H__ */