


#define false 0
#define true 1

int	GetOsVersion(int *IsNT);

int	SetDeviceMode(unsigned char cmdMode,unsigned char mEntry);
unsigned short	PowerControl(unsigned char state);

unsigned short	WaitToReadFbus(LPBYTE buffer,LPLONG readsize,int longdelay);
int	ExecuteMbusCmdXp(unsigned char *out_buffer,unsigned char *responce,
					     UINT current,LONG *rbytes);

unsigned char	DeviceControlHandler(unsigned char *devicedata,unsigned char mode,unsigned char entry);
int	ReadRxBuffer(LPBYTE buffer,LPLONG readsize);


unsigned short	OpenVxdDriver(void);
unsigned short	CloseVxdDriver(void);

unsigned short	OpenAfdDriver(HANDLE *pHandle,unsigned long PortNo);
unsigned short	CloseNTDriver(void);

unsigned char	SetSubMode(unsigned char Mode);
unsigned char	ResetSubMode(VOID);

unsigned short	StartDriver(void);

int	OpenHandleToDriver(void);
unsigned char	InstallAfdDriver(void);

unsigned short	SetIdleStateXP(unsigned char *uppdata);
unsigned short	InitBootstrapXP(LPBYTE bootf,LPBYTE boots,LPBYTE configdata,LPBYTE mread,unsigned char fMode);
unsigned short	SendAlgorithmXP(unsigned char *data,int count);
unsigned short	EraseBlockXP(unsigned long StartAddress,unsigned long EndAddress,unsigned char *error);
unsigned short	Dct4AuthXP(unsigned char *claudia, int size,unsigned char *rmsid,unsigned char *dmsid);
unsigned short	SendToPhoneXP(unsigned char *pBlockBuffer,unsigned long BlockSize,unsigned long DelayLow,unsigned long DelayHigh);
unsigned short	ProgrammingStatusXP(unsigned char *error);
unsigned short	MCMHardwareVersionXP(unsigned char *mcmver);
unsigned short	WriteBlockXp(unsigned char *pBlockBuffer,unsigned short nBufferSize,unsigned char *pFlashMessage,unsigned char nMsgSize,unsigned char wType);
unsigned short  SetDefaultTimesA(unsigned char *deftimes);
unsigned short	Dct3PowerUp(void);
unsigned short	UEMCalls(unsigned char *upData, unsigned char upSize,unsigned char upType,unsigned char CallSwitch);