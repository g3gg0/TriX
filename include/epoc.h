#ifndef __TRIX_EPOC_H__
#define __TRIX_EPOC_H__

typedef int TInt32;
typedef int TInt;
typedef unsigned char TUint8;
typedef char Tint8;
typedef unsigned short TUint16;
typedef short Tint16;
typedef unsigned int TUint32;
typedef unsigned int TUint;
typedef unsigned int TCpu;
typedef unsigned int TVersion;
typedef unsigned int TProcessPriority;

#define EPOCMAG1 'E'
#define EPOCMAG2 'P'
#define EPOCMAG3 'O'
#define EPOCMAG4 'C'

typedef struct _E32ImageHeader
{
   TUint32 iUid1;
   TUint32 iUid2;
   TUint32 iUid3;
   TUint32 iCheck;
   unsigned char iSignature1;
   unsigned char iSignature2;
   unsigned char iSignature3;
   unsigned char iSignature4;
//   TUint iSignature; // 'EPOC'
   TCpu iCpu; // 0x1000 = X86, 0x2000 = ARM, 0x4000 = M*Core
   TUint iCheckSumCode; // sum of all 32 bit words in .text
   TUint iCheckSumData; // sum of all 32 bit words in .data
   TVersion iVersion;
   TUint32 iTimeLo;
   TUint32 iTimeHi;
   TUint iFlags; // 0 = exe, 1 = dll, +2 = no call entry points
   TInt iCodeSize; // size of code, import address table, constant data and export dir
   TInt iDataSize; // size of initialized data
   TInt iHeapSizeMin;
   TInt iHeapSizeMax;
   TInt iStackSize;
   TInt iBssSize;
   TUint iEntryPoint; // offset into code of entry point
   TUint iCodeBase; // where the code is linked for
   TUint iDataBase; // where the data is linked for
   TInt iDllRefTableCount; // filling this in enables E32ROM to leave space for it
   TUint iExportDirOffset; // offset into the file of the export address table
   TInt iExportDirCount;
   TInt iTextSize; // size of just the text section
   TUint iCodeOffset; // file offset to code section
   TUint iDataOffset; // file offset to data section
   TUint iImportOffset; // file offset to import section
   TUint iCodeRelocOffset; // relocations for code and const
   TUint iDataRelocOffset; // relocations for data
   TProcessPriority iPriority; // priority of this process
} E32ImageHeader;


typedef struct _E32ImportSection
{
   TInt iSize; // size of this section
               // E32ImportBlock[iDllRefTableCount];
} E32ImportSection;

typedef struct _E32RelocSection
{
   TInt iSize; // size of this relocation section
   TInt iNumberOfRelocs; // number of relocations in this section
} E32RelocSection;


typedef struct _E32ImageHeaderv2
{
	TUint32 iUid1;
	TUint32 iUid2;
	TUint32 iUid3;
	TUint32 iUidChecksum;
	unsigned char iSignature1;
	unsigned char iSignature2;
	unsigned char iSignature3;
	unsigned char iSignature4;
	TUint32 iHeaderCrc; // CRC-32 of entire header
	TUint32 iModuleVersion; // Version number for this executable (used in link resolution)
	TUint32 iCompressionType; // Type of compression used (UID or 0 for none)
	TVersion iToolsVersion; // Version of PETRAN/ELFTRAN which generated this file
	TUint32 iTimeLo;
	TUint32 iTimeHi;
	TUint iFlags; 
	/* (http://developer.symbian.org/wiki/Symbian_OS_Internals/Appendix_2:_The_E32ImageHeader)

		Flags:

		Bits 	Size in bits Description
		0 	    1 	Executable type:
		            0 - EXE
		            1 - DLL
		1 	    1 	Whether to call entry point (not used in EKA2):
		            0 - call entry point
		            1 - don't call entry point
		2 	    1 	Whether this is a fixed address EXE:
		            0 - not fixed address
		            1 - fixed address
		4-3 	2 	ABI:
		            0 - GCC98r2
		            1 - EABI
		7-5 	3 	Entry point type:
		            0 - EKA1
		            1 - EKA2
		23-8 	16 	Reserved
		27-24 	4 	Header format:
		            0 - Basic
		            1 - J-format
		            2 - V-format
		31-28 	4 	Import format:
		            0 - Standard PE format
		            1 - ELF format
		            2 - PE format without redundant copy of import ordinals. Standard PE format is only used with original
		                and J-format headers. V-format headers are either ELF or PE without redundant import ordinals. 
	*/
	TInt iCodeSize; // size of code, import address table, constant data and export dir
	TInt iDataSize; // size of initialised data
	TInt iHeapSizeMin;
	TInt iHeapSizeMax;
	TInt iStackSize;
	TInt iBssSize;
	TUint iEntryPoint; // offset into code of entry point
	TUint iCodeBase; // where the code is linked for
	TUint iDataBase; // where the data is linked for
	TInt iDllRefTableCount; // filling this in enables E32ROM to leave space for it
	TUint iExportDirOffset; // offset into the file of the export address table
	TInt iExportDirCount;
	TInt iTextSize; // size of just the text section, also doubles as the offset for the
	// iat w.r.t. the code section
	TUint iCodeOffset; // file offset to code section, also doubles as header size
	TUint iDataOffset; // file offset to data section
	TUint iImportOffset; // file offset to import section
	TUint iCodeRelocOffset; // relocations for code and const
	TUint iDataRelocOffset; // relocations for data
	TUint16 iProcessPriority; // executables priority
	TUint16 iCpuIdentifier; // 0x1000 = X86, 0x2000 = ARM

	TUint32 iUncompressedSize; // Uncompressed size of file
	// For J format this is file size - sizeof(E32ImageHeader)
	// and this is included as part of the compressed data :-(
	// For other formats this is file size - total header size

	TUint32 iSecureId;
	TUint32 iVendorId;
	TUint32 iCapsHi;
	TUint32 iCapsLo;

	//	SCapabilitySet iCaps; // Capabilities re. platform security
	//	SSecurityInfo iS;

	// Use iSpare1 as offset to Exception Descriptor
	TUint32 iExceptionDescriptor;  // Offset in bytes from start of code section to Exception Descriptor,
	// bit 0 set if valid
	TUint32 iSpare2;
	TUint16 iExportDescSize; // size of bitmap section
	TUint8 iExportDescType; // type of description of holes in export table
	TUint8 iExportDesc; // description of holes in export table - extend
} E32ImageHeaderv2;

#define KUidCompressionDeflate  0x101F7AFC
#define KUidCompressionBytePair 0x102822AA


#endif
