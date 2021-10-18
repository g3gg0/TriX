/******************************************************************
*
*              Prodigy low level PIC DEVICE implementation
*                    B-phreaks Ltd,2004
*
******************************************************************/

#define LOW_ACK                                     0
#define HIGH_ACK									1

#define EIGHTBIT_WRITE								0
#define SIXTEENBIT_WRITE							1
#define THIRTYTWOBIT_WRITE							2
#define SIXTEENBITA_WRITE							3

void MicroDelay(unsigned long MsDelay);
void InteruptControl(bool status);

bool	ReadFlashingByte(UCHAR *readbyte);
bool	WriteFlashingByte(UCHAR byteValue);
bool	WriteDeviceByte(UCHAR byteValue);
void	WriteHighLowNible(UCHAR wData,UCHAR wPosition);
bool	GetHighLowNible(ULONG nPosition,ULONG ackdelay,UCHAR *nData);
bool	WaitHigh(ULONG ackhdelay);
bool	WaitLow(ULONG ackhdelay);
UCHAR	GetPort(VOID);
bool	GetAck(VOID);
UCHAR	ReadPort(ULONG rIndex);
VOID	WritePort(ULONG wIndex,UCHAR wData);

bool	SetDeviceModeSwitch(UCHAR *cmdMode,UCHAR mEntry);
bool	ReadDeviceString(UCHAR *redata,UCHAR nToRead,UCHAR *pU8numreadbytes);
ULONG	SetDeviceModeAndTest(UCHAR cmdMode);
ULONG	SetDeviceMode(UCHAR cmdMode);
UCHAR	SetSubMode(UCHAR Mode);
bool	ReadDeviceByte(UCHAR *redata);
bool	ReadMbusByteAndClear(UCHAR *ReadByte);
bool	WriteMbusByte(UCHAR ByteValue);
bool	ReadMbusByteNormal(UCHAR *ReadByte);
bool	WriteFastByte(UCHAR ByteValue,bool TriggerCheck);
bool	WriteFastBlockTwo(UCHAR *pData,ULONG Count,ULONG *BytesWritten);

