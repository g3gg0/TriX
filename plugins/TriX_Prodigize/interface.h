//---------------------------------------------------------------------------

#ifndef Dct4DebugCommH
#define Dct4DebugCommH
//---------------------------------------------------------------------------

// Loader commands
#define         READ_LINEAR_ADDRESS             0x15
#define         ROM_DECODE_FUNCTION             0x16
#define         ARM_INIT_CALL                   0x17
#define         UEM_READ_VERSION                0x18
#define         UEM_READ_IMEI                   0x19


// Bootrom addresses
#define         BOOTROM_DECODE                  0x0084000F
#define         BOOTROM_ENCODE                  0x00840011

// Decode arguments
#define         BOOTROM_SP_LOCK                  0x10
#define         BOOTROM_KEY_SEVEN                0x12
#define         BOOTROM_IMEI_OR_LOADER           0x20
#define         BOOTROM_ASK_DATA                 0x40

int OpenPdDriver( );
void ClosePdDriver( );

int CheckBootRomVersion ( unsigned char *pUpp );
int LoadSecond(unsigned char *pConfig,unsigned char *csize,unsigned char *loader, int loader_length );
int LoadThird ( unsigned short *err, unsigned char *loader, int loader_length );

int ReadStatus( );
int ReadMemoryBlock(unsigned char *pRamData, unsigned char nSize, unsigned long nAddress);
int RomDecode(unsigned char *pBlockBuffer,int ctype,unsigned char DecType);

int CallArmInit( );

int ReadUemVersion(unsigned char *UemVersion);
int ReadUemImei(unsigned char *UemImei);

unsigned char NegChkSum(unsigned char *pMessage,unsigned char mSize);




#endif
