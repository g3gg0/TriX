#ifndef FIAH
#define FIAH

#include <stdio.h>

#define FIA_2NDBOOT     0xA0
#define FIA_FLASH       0xA1
#define FIA_CBUS        0xA1

enum {
  FIAERR_OK = 0,
  FIAERR_OPEN,
  FIAERR_SECTION,
  FIAERR_NOASIC,
  FIAERR_NOFLASHID
};

typedef struct {
  unsigned char type;
  unsigned int length;
  unsigned int sections;
} TFIAHdr;

typedef struct {
  unsigned char *ptr;
  int size;
} TData;

class TFIA
{
  private:
    int handle;

  public:
    TFIAHdr hdr;
    TData dataBin;
    TData dataSec;
    char filepath[ 256 ];

    int Open( unsigned char *filename );
    void Close();
    int GetSection( unsigned char num, unsigned char *data, int *size );
    int CheckASIC( unsigned short asicid );
    int CheckFlashID( int flashid );

    unsigned long EndianDWORD( unsigned long i );
    unsigned short EndianWORD( unsigned short i );

    TFIA();
    ~TFIA();
};
#endif