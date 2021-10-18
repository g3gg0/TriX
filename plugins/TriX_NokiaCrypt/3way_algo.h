#define UINT32 unsigned long
#define UINT16 unsigned short
#define UINT8 unsigned char

void DecodeMSID( UINT8 *pmsid );
void EncodeFAID( UINT8 *pfaid, UINT8 *pmsid );
void DecodeIMEI( UINT8 *pimei, UINT8 *pmsid );
void DecodeFAID( UINT8 *pfaid, UINT8 *pmsid );
void DecodeSPLock( UINT8 *pimei, UINT8 *pmsid );
