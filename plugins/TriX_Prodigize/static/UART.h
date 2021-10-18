/******************************************************************
*
*              Prodigy low level UART implementation
*                    B-phreaks Ltd,2004
*
******************************************************************/

#define READ_BUFFER_SIZE				9000
#define READ_DEV_BUFFER_SIZE			1150
#define READ_TIMEOUT_MS					10

#define	UART_PURGE_RX					1
#define	UART_PURGE_TX					2

#define UART_MAX_TRANSMIT_LENGHT		180

#define UART_DEVICE_700					0
#define UART_DEVICE_701					1

//#define UART_BAUD_300					300
//#define UART_BAUD_600					600
//#define UART_BAUD_1200				1200
#define UART_BAUD_2400					2400
#define UART_BAUD_4800					4800
#define UART_BAUD_9600					9600
#define UART_BAUD_14400					14400
#define UART_BAUD_19200					19200
#define UART_BAUD_28800					28800
#define UART_BAUD_38400					38400
#define UART_BAUD_57600					57600
#define UART_BAUD_115200				115200
#define UART_BAUD_230400				230400
#define UART_BAUD_460800				460800
#define UART_BAUD_921600				921600

// Uart calls
VOID	UART_TransferBuffer(UCHAR *pTransferBuffer,ULONG *nTransfered);

// Device calls 
bool	UART_Open(VOID);
bool	UART_Write(UCHAR *pDataBuffer,ULONG iBufferSize,ULONG *iBytesWritten);
bool	UART_ReadA(UCHAR *pDataBuffer,ULONG iBufferSize,ULONG *iBytesRead);
bool	UART_GetQueueStatusA(ULONG *iBytesReadFirst);
bool	UART_PurgeA(unsigned int pMask);
bool	UART_SetBaudRate(ULONG BaudRate);
bool	UART_PowerControl(UCHAR state);
