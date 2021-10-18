#ifndef UFSH
#define UFSH

#include "ftd2xx.h"
#include "FIA.h"

#define UFS_ID 0x08885508

enum {
  UFSERR_OK = 0,
  UFSERR_OPEN,
  UFSERR_UFSBOOT,
  UFSERR_UFSAUTH1,
  UFSERR_UFSCABLE,
  UFSERR_EXCHANGEFAILED,
  UFSERR_NOACK,
  UFSERR_BADACK,
  UFSERR_BADCS,
  UFSERR_NOFIA
};

enum {
  UFSMODE_ATMEL = 0,
  UFSMODE_UART,
  UFSMODE_FBUS,
  UFSMODE_FLASH
};

enum {
  UFSSPEED_PREBOOT = 0,
  UFSSPEED_BOOT,
  UFSSPEED_FLASH
};

typedef struct {
  unsigned char address;
  unsigned long ID;
  unsigned char data1[ 2 ];
  unsigned char rev;
  unsigned char data2[ 1 ];
  unsigned short sector;
} TFlashID;

typedef struct {
  unsigned char read;
  unsigned char write;
} TUFSSpeed;

class TUFSx
{
  private:
    FT_HANDLE handle;
    int mode;

    unsigned char busstate;
    TUFSSpeed UFSSpeed[ 3 ];
    unsigned char speed;

    unsigned char CheckSum( unsigned char *data, DWORD size, bool add );
    int Exchange( unsigned char *out_data, DWORD out_size, unsigned char *in_data, DWORD *in_size );
    bool WaitState( unsigned char mask, bool state );

    bool ReceiveData( unsigned short size );
    bool SendData( unsigned char *data, unsigned short size );

  public:
    unsigned char *inbuf;

    int SerNum;
    int AtmelID;
    int FWID;
    char BootVer[ 33 ];
    char FWVer[ 33 ];
    TFlashID *FlashID;
    unsigned long asicID;
    unsigned char FlashNum;
    unsigned char FlashCfg;
    unsigned char ExtBUSC[ 16 ];
    unsigned char IMEIESN[ 8 ];

    int Init();
    int Open( unsigned char *sn );
    void Close();
    int SetMode( int ufsmode );
    int BootLdr( DWORD *asicID );
    int Boot2ndLdr( TFIA *ldr );
    int FlashLdr( TFIA *ldr );
    int GetASK( unsigned char *outdata );
    int WriteFlashRPL( unsigned char *indata );
    int WriteUEMRPL( unsigned char *indata );
    int HWInit();
    int CBusLdr( TFIA *ldr );
    int FlashCmd( unsigned char cmd, unsigned char *indata = NULL, unsigned short size = 0 );

    TFIA *SearchFIA( char *mask, unsigned char type, unsigned short asicID, int flashid = -1 );

    char *GetError( int rc );

    unsigned short EndianWORD( unsigned short i );
    unsigned long EndianDWORD( unsigned long i );

    TUFSx();
    ~TUFSx();
};
#endif