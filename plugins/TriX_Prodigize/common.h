// common.h - common sys/exe definitions for Port I/O example
//=============================================================================
//
// Compuware Corporation
// NuMega Lab
// 9 Townsend West
// Nashua, NH 03060  USA
//
// Copyright (c) 1998 Compuware Corporation. All Rights Reserved.
// Unpublished - rights reserved under the Copyright laws of the
// United States.
//
//=============================================================================

/*           LOW LEVEL ACCESS IMPLEMENTATION          */

#define IOCTL_PRODIGY_OPENPORT \
	CTL_CODE(FILE_DEVICE_UNKNOWN, 0, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_PRODIGY_CLOSEPORT \
	CTL_CODE(FILE_DEVICE_UNKNOWN, 1, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_PRODIGY_MBUSPORT \
	CTL_CODE(FILE_DEVICE_UNKNOWN, 2, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_PRODIGY_SETDEVMODE \
	CTL_CODE(FILE_DEVICE_UNKNOWN, 3, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_PRODIGY_DEVDATA \
	CTL_CODE(FILE_DEVICE_UNKNOWN, 4, METHOD_BUFFERED, FILE_ANY_ACCESS)

/*               FLASHING IMPLEMENTATION              */

#define IOCTL_PRODIGY_DCT4_SET_IDLE_STATE \
	CTL_CODE(FILE_DEVICE_UNKNOWN, 5, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_PRODIGY_DCT4SENDBOOT \
	CTL_CODE(FILE_DEVICE_UNKNOWN, 6, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_PRODIGY_DCT4SENDALGO \
	CTL_CODE(FILE_DEVICE_UNKNOWN, 7, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_PRODIGY_DCT4AUTHENTIFICATION \
	CTL_CODE(FILE_DEVICE_UNKNOWN, 8, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_PRODIGY_DCT4ERASE \
	CTL_CODE(FILE_DEVICE_UNKNOWN, 9, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_PRODIGY_WRITEBLOCK \
	CTL_CODE(FILE_DEVICE_UNKNOWN, 10, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_PRODIGY_SENDTOPHONE \
	CTL_CODE(FILE_DEVICE_UNKNOWN, 11, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_PRODIGY_PROGSTATUS \
	CTL_CODE(FILE_DEVICE_UNKNOWN, 12, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_PRODIGY_MCMHWVERSION \
	CTL_CODE(FILE_DEVICE_UNKNOWN, 13, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_PRODIGY_DEFAULT_TIMES \
	CTL_CODE(FILE_DEVICE_UNKNOWN, 14, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_PRODIGY_DCT3_POWER_UP \
	CTL_CODE(FILE_DEVICE_UNKNOWN, 15, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_PRODIGY_UEM_CALLS \
	CTL_CODE(FILE_DEVICE_UNKNOWN, 16, METHOD_BUFFERED, FILE_ANY_ACCESS)



/*               UART IMPLEMENTATION                  */

#define IOCTL_PRODIGY_UART_OPEN \
	CTL_CODE(FILE_DEVICE_UNKNOWN, 17, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_PRODIGY_UART_CLOSE \
	CTL_CODE(FILE_DEVICE_UNKNOWN, 18, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_PRODIGY_UART_WRITE \
	CTL_CODE(FILE_DEVICE_UNKNOWN, 19, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_PRODIGY_UART_READ \
	CTL_CODE(FILE_DEVICE_UNKNOWN, 20, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_PRODIGY_UART_SET_TIMEOUTS \
	CTL_CODE(FILE_DEVICE_UNKNOWN, 21, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_PRODIGY_UART_GET_QUEUE_STATUS \
	CTL_CODE(FILE_DEVICE_UNKNOWN, 22, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_PRODIGY_UART_PURGE \
	CTL_CODE(FILE_DEVICE_UNKNOWN, 23, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_PRODIGY_UART_SET_BAUD_RATE \
	CTL_CODE(FILE_DEVICE_UNKNOWN, 24, METHOD_BUFFERED, FILE_ANY_ACCESS)

/*             NOKIA FBUS IMPLEMENTATION              */

#define IOCTL_PRODIGY_NOKIA_READ \
	CTL_CODE(FILE_DEVICE_UNKNOWN, 25, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_PRODIGY_NOKIA_GET_QUEUE_STATUS \
	CTL_CODE(FILE_DEVICE_UNKNOWN, 26, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_PRODIGY_NOKIA_PURGE \
	CTL_CODE(FILE_DEVICE_UNKNOWN, 27, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_PRODIGY_NOKIA_POWER \
	CTL_CODE(FILE_DEVICE_UNKNOWN, 28, METHOD_BUFFERED, FILE_ANY_ACCESS)


typedef struct _PORTOPENPARAMS
{
	unsigned long open_Address;		// port address 
	unsigned long open_PortSize;	// 1=byte 2=word 3=long
	unsigned long open_PortCount;	// number of ports in block
} PORTOPENPARAMS;

typedef struct _PORTWRITEPARAMS
{
	unsigned long wr_Handle;	
	unsigned long wr_Index;
	unsigned char wr_Data;
} PORTWRITEPARAMS;

typedef struct _PORTCLOSEPARAMS
{
	unsigned long close_Handle;

} PORTCLOSEPARAMS;

typedef struct _SETDEVMODEPARAMS
{

	unsigned char *nMode;
	unsigned char mEntry;

} SETDEVMODEPARAMS;

typedef struct _PORTREADPARAMS
{
	unsigned char *pValue;
	unsigned char nIndex;
} PORTREADPARAMS;

typedef struct _GETPORTPARAMS
{
	unsigned char *pValue;
	unsigned char nIndex;
} GETPORTPARAMS;

typedef struct _HALFPORTPARAMS
{
	unsigned char nNible;
	unsigned char nType;
} HALFPORTPARAMS;

typedef struct _DEVDATAPARAMS
{
	unsigned char *pData;		// prt to data
	unsigned char nMode;		// sub mode
	unsigned char mEntry;		// call switch
	unsigned char *nError;		// error code
} DEVDATAPARAMS;

// DCT4 Set Idle State param
typedef struct _DCT4SETIDLESTATEPARAMS
{		
	
	unsigned char      *errcode; // ptr to result
	unsigned char      *uppdata; // pointer for UPP data
	
} DCT4SETIDLESTATEPARAMS;

// DCT4 SendBoot arguments structure
typedef struct _DCT4BOOTSENDPARAMS
{	

	unsigned char      *errcode; // ptr to result
    unsigned char        *bootf; // pointer to opened boot loader
	unsigned char        *boots; // pointer to boot size array 	
    unsigned char   *configdata; // readed config data
	unsigned char        *mread; // read count ptr
	unsigned char		  fMode; // flash mode (dct4,3 or linda)

} DCT4BOOTSENDPARAMS;


// DCT4 Send Algo arguments structure
typedef struct _DCT4ALGOSENDPARAMS
{
	
	unsigned char      *errcode; // ptr to result
    unsigned char         *data; // algo file 	
	int	          count; // size

} DCT4ALGOSENDPARAMS;

// DCT4 Authentification
typedef struct _DCT4AUTHENTIFICATIONPARAMS
{

	unsigned char      *errcode; // ptr to result
    unsigned char         *data; // algo file 
	int           count; // size
	unsigned char        *rmsid; // readed msid
	unsigned char        *dmsid; // decoded uem id

} DCT4AUTHENTIFICATIONPARAMS;

// DCT4 Erase
typedef struct _DCT4ERASEPARAMS
{

	unsigned char      *errcode; // ptr to result
	unsigned char       *errors; // error string	
    unsigned long  StartAddress;
	unsigned long    EndAddress;    

} DCT4ERASEPARAMS;

// Write block arguments structure
typedef struct _WRITEBLOCKPARAMS
{

    unsigned char        *errcode; // ptr to result
	unsigned char		*BlockBuff; // block ptr
	unsigned long	   nBufferSize; // buffer size
	unsigned char   *pFlashMessage; // msg ptr 
	unsigned char         nMsgSize; // size of message
	unsigned char            wType; // write mode		

} WRITEBLOCKPARAMS;

// DCT4 Send to phone arguments structure
typedef struct _DCT4SENDTOPHONEPARAMS
{
	
	unsigned char        *errcode; // ptr to result
    unsigned char	*pBlockBuffer;	
	unsigned long		BlockSize;
	unsigned long		 DelayLow;
	unsigned long		DelayHigh;	

} DCT4SENDTOPHONEPARAMS;

// Programming status
typedef struct _PROGSTATUSPARAMS
{

	unsigned char      *errcode; // ptr to result
    unsigned char        *error; // buffer 
	
} PROGSTATUSPARAMS;

// MCM HW Version
typedef struct _MCMHWVERPARAMS
{

	unsigned char      *errcode; // ptr to result
    unsigned char       *mcmver; // buffer 

} MCMHWVERPARAMS;

// Default times
typedef struct _DEFTIMESPARAMS
{

	unsigned char      *errcode; // ptr to result
    unsigned char     *deftimes; // buffer 

} DEFTIMESPARAMS;

// UEM calls
typedef struct _UEMCALLPARAMS
{

	unsigned char      *errcode; // ptr to result
	unsigned char	    *upData;		
	unsigned char	     upSize;
	unsigned char	     upType;
	unsigned char	 CallSwitch;
	
} UEMCALLPARAMS;

// UART_Read struct
typedef struct _UARTREADPARAMS
{
	unsigned char *pReadBuffer;
    unsigned long nBufferSize;

} UARTREADPARAMS;

// UART_Purge struct
typedef struct _UARTPURGEPARAMS
{
    unsigned int pMask;

} UARTPURGEPARAMS;

// UART_Purge struct
typedef struct _UARTSETBAUDRATEPARAMS
{
    unsigned long BaudRate;

} UARTSETBAUDRATEPARAMS;

// UART_Purge struct
typedef struct _UARTSETIMEOUTSPARAMS
{
    unsigned long dwReadTimeout;
	unsigned long dwWriteTimeout;

} UARTSETIMEOUTSPARAMS;

#define BUSTYPE			Isa
#define BUSNUMBER		0

