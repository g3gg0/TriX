

#define PRODIGY_READ_DUSN                              0x07
#define PRODIGY_READ_BOOT_VERSION                      0x08
#define PRODIGY_READ_DEVICE                            0x01
#define PRODIGY_READ_TESTS                             0x0F
#define PRODIGY_EEPROM_WRITE                           0x07
#define PRODIGY_UPLOAD_LICENCE                         0x05
#define PRODIGY_READ_COUNTERS                          0x03
#define PRODIGY_READ_MANIF_DATE                        0x06
#define PRODIGY_READ_SALES_DATE                        0x07
#define PRODIGY_WRITE_MANIF_DATE                       0x08
#define PRODIGY_WRITE_SALES_DATE                       0x09
#define PRODIGY_READ_DEVICE_TESTS                      0x0B


UCHAR	ControlEntry(UCHAR *pData,UCHAR nMode,UCHAR mEntry);
UCHAR	LoadDeviceData(UCHAR *devicedata,UCHAR mode);
UCHAR	ReadDUSN(UCHAR *ldata, UCHAR mode);
UCHAR	ProgramFlash(UCHAR *ldata,UCHAR Mode);
UCHAR	ProgramNewDeviceVersion(UCHAR *ldata);
UCHAR	LicenceUpdate(UCHAR *ldata);
UCHAR	ReadCounters(UCHAR *counters);
UCHAR	ReadManifDate(UCHAR *dates);
UCHAR	ReadSalesDate(UCHAR *dates);
UCHAR	ManifDateWrite(UCHAR *ldata);
UCHAR	SalesDateWrite(UCHAR *ldata);
UCHAR	ReadDeviceTests(UCHAR *pResults);

