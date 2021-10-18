/*************************************************************************
*
*                  Low Level prodigy communication functions
*                          B-phreaks Ltd, 2003   
*
*************************************************************************/

#include <windows.h>
#include <winioctl.h>

#include <stdio.h>

#include "common.h"
#include "device.h"


#define STATUS_NO_SUCH_DEVICE             0x002
#define STATUS_SUCCESS					  0x3E5


#define UART_MAX_TRANSMIT_LENGHT			180

unsigned char			DeviceVersion = 0;

int                             NTTRUE = false;

// NT/Win98 driver handle
HANDLE                                  hPortIo;

int                                  status;
unsigned long                            nBytesReturned;
unsigned long                                PortHandle;

// low level access
PORTOPENPARAMS                         OpenPort;
PORTREADPARAMS                         ReadPort;
PORTWRITEPARAMS                       WritePort;
PORTCLOSEPARAMS                       ClosePort;
SETDEVMODEPARAMS                        SetMode;
GETPORTPARAMS                           GetPort;
HALFPORTPARAMS                         HalfPort;
DEVDATAPARAMS                           DevData;

// flashing calls
DCT4SETIDLESTATEPARAMS        SetIdleStateParam; 
DCT4BOOTSENDPARAMS            Dct4BootSendParam; 
DCT4ALGOSENDPARAMS            Dct4AlgoSendParam;  
DCT4AUTHENTIFICATIONPARAMS        Dct4AuthParam;
DCT4SENDTOPHONEPARAMS	   Dct4SendToPhoneParam;
PROGSTATUSPARAMS                ProgStatusParam;
MCMHWVERPARAMS                      McmVerParam;
DCT4ERASEPARAMS                  Dct4EraseParam;
WRITEBLOCKPARAMS            Dct4WriteBlockParam; 
DEFTIMESPARAMS				  Dct4DefTimesParam;
UEMCALLPARAMS					   Dct4UemParam;	

// uart calls
UARTREADPARAMS                    UartReadParam;
UARTPURGEPARAMS                  UartPurgeParam;
UARTSETBAUDRATEPARAMS      UartSetBaudRateParam;


unsigned char table_send[16] = {0x0,0x80,0x40,0xc0,0x10,0x90,0x50,0xd0,0x20,0xa0,0x60,0xe0,0x30,0xb0,0x70,0xf0};
unsigned char table_receive[16] = {0x02,0x0a,0x06,0x0e,0x03,0x0b,0x07,0x0f,0x0,0x08,0x04,0x0c,0x01,0x09,0x05,0x0d};
unsigned char out_port = 0;

// Get OS Version
//
int GetOsVersion(int *IsNT)
{
	OSVERSIONINFOEX osvi;
	int bOsVersionInfoEx;

	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	if( !(bOsVersionInfoEx = GetVersionEx ((OSVERSIONINFO *) &osvi)) )
	{
		osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
		if (! GetVersionEx ( (OSVERSIONINFO *) &osvi) ) 
			return FALSE;
	}

	switch (osvi.dwPlatformId)
	{
		// Test for the Windows NT product family.
	case VER_PLATFORM_WIN32_NT:

		*IsNT = TRUE; 
		break;

		// Test for the Windows 95 product family.
	case VER_PLATFORM_WIN32_WINDOWS:

		*IsNT = FALSE;  
		break;

	case VER_PLATFORM_WIN32s:

		*IsNT = FALSE;  
		break;
	}
	return TRUE; 
}


/******************************************************/
/******************************************************/
/******************************************************/
/*               UART IMPLEMENTATION                  */
/******************************************************/
/******************************************************/
/******************************************************/


// Open Prodigy UART
//
int UART_Open(unsigned long PortNo)
{

	HANDLE dHandle;

	// Check OS
	if (!GetOsVersion (&NTTRUE))
	{
		return(false);
	}


	// Open driver
	if (OpenAfdDriver (&dHandle,PortNo) != 0)
	{
		return(false);
	}

	// Set UART Mode
	status = DeviceIoControl(	hPortIo, 
		IOCTL_PRODIGY_UART_OPEN, 
		NULL, 
		0,
		NULL,
		0,
		&nBytesReturned,
		NULL
		);

	if (!status)
		return(false);


	return(true);
}

// Close Prodigy UART
//
int UART_Close(VOID)
{

	// Stop Rx polling - no 98 handling for now!
	if(NTTRUE)
	{   
		status = DeviceIoControl(	hPortIo, 
			IOCTL_PRODIGY_UART_CLOSE, 
			NULL, 
			0,
			NULL,
			0,
			&nBytesReturned,
			NULL
			);

		if (!status)
			return(false);
	}
	else
		return(false);

	// Close driver
	CloseNTDriver();

	return(true);
}


// Set PIC Uart Register
//
int UART_SetBaudRate(unsigned long BaudRate)
{

	unsigned char LocalError = 0;


	// Transfer the buffer size
	UartSetBaudRateParam.BaudRate = BaudRate;

	status = DeviceIoControl(	hPortIo, 
		IOCTL_PRODIGY_UART_SET_BAUD_RATE, 
		&UartSetBaudRateParam,           // arguments structure
		sizeof(UartSetBaudRateParam),    // structure size 
		NULL,                
		0,              
		&nBytesReturned,
		NULL
		);	  

	if (!status)
		return(false);



	return(true);
}

// Write UART buffer
//
int UART_WriteBuffer(unsigned char *pTxBuffer,unsigned long nBufferSize,unsigned long *nBytesWritten)
{


	// write buffer
	status = DeviceIoControl(	hPortIo, 
		IOCTL_PRODIGY_UART_WRITE, 
		pTxBuffer, 
		nBufferSize,
		NULL,
		0,
		&nBytesReturned,
		NULL
		);

	if ((!status) || (nBytesReturned != nBufferSize))
		return(false);

	*nBytesWritten = nBytesReturned;


	return(true);
}



// Read UART buffer
//
int UART_ReadBuffer(unsigned char *pRxBuffer,unsigned long nBufferSize,unsigned long *nBytesRead)
{

	unsigned char LocalError = 0;
	unsigned char iCheckSum = 0;


	// Transfer the buffer size
	UartReadParam.nBufferSize = nBufferSize;
	UartReadParam.pReadBuffer = pRxBuffer;

	status = DeviceIoControl(	hPortIo, 
		IOCTL_PRODIGY_UART_READ, 
		&UartReadParam,          
		sizeof(UartReadParam),    
		NULL,               
		0,              
		&nBytesReturned,
		NULL
		);	  

	if ( !status )
		return(false);

	*nBytesRead = nBytesReturned;

	return(true);
}

// Read UART RX buffer status
//
int UART_GetQueueStatus(unsigned long *nRxBytes)
{
	unsigned char LocalError = 0;

	// Not valid for now !!!!

	status = DeviceIoControl(	hPortIo, 
		IOCTL_PRODIGY_UART_GET_QUEUE_STATUS, 
		NULL,           
		0,     
		NULL,               
		0,              
		&nBytesReturned,
		NULL
		);	  

	if (!status)
		return(false);

	*nRxBytes = nBytesReturned;



	return(true);
}

// Purge UART buffers
//
int UART_Purge(unsigned long pMask)
{

	unsigned char LocalError = 0;


	// Transfer the buffer size
	UartPurgeParam.pMask = pMask;

	status = DeviceIoControl(	hPortIo, 
		IOCTL_PRODIGY_UART_PURGE, 
		&UartPurgeParam,           // arguments structure
		sizeof(UartPurgeParam),    // structure size 
		NULL,                
		0,              
		&nBytesReturned,
		NULL
		);	  

	if (!status)
		return(false);


	return(true);
}


/******************************************************/
/******************************************************/
/******************************************************/
/*             NOKIA FBUS IMPLEMENTATION              */
/******************************************************/
/******************************************************/
/******************************************************/



// Read UART RX buffer status
int NOKIA_GetQueueStatus(UINT *nRxBytes)
{
	unsigned char LocalError = 0;


	status = DeviceIoControl(	hPortIo, 
		IOCTL_PRODIGY_NOKIA_GET_QUEUE_STATUS, 
		NULL,           
		0,     
		NULL,               
		0,              
		&nBytesReturned,
		NULL
		);	  

	if (!status)
		return(false);

	*nRxBytes = nBytesReturned;



	return(true);
}

// Read UART buffer
//
int NOKIA_ReadBuffer(unsigned char *pRxBuffer,unsigned long nBufferSize,unsigned long *nBytesRead)
{

	//UINT i;
	unsigned char LocalError = 0;
	unsigned char iCheckSum = 0;
	//unsigned char tt[1];



	// Transfer the buffer size
	UartReadParam.pReadBuffer = pRxBuffer;
	UartReadParam.nBufferSize = nBufferSize;

	status = DeviceIoControl(	hPortIo, 
		IOCTL_PRODIGY_NOKIA_READ, 
		&UartReadParam,           // arguments structure
		sizeof(UartReadParam),    // structure size 
		NULL,                // read buffer address
		0,              // size of read buffer
		&nBytesReturned,
		NULL
		);	  

	if ( !status )
		return(false);

	*nBytesRead = nBytesReturned;

	return(true);
}

// Purge UART buffers
//
int NOKIA_Purge(UINT pMask)
{

	unsigned char LocalError = 0;


	// Transfer the buffer size
	UartPurgeParam.pMask = pMask;

	status = DeviceIoControl(	hPortIo, 
		IOCTL_PRODIGY_NOKIA_PURGE, 
		&UartPurgeParam,           // arguments structure
		sizeof(UartPurgeParam),    // structure size 
		NULL,                
		0,              
		&nBytesReturned,
		NULL
		);	  

	if (!status)
		return(false);


	return(true);
}

// Put power on/off in Fbus/Mbus menu
//
unsigned short PowerControl(unsigned char state)
{

	// Transfer the buffer size
	UartPurgeParam.pMask = state;

	status = DeviceIoControl(	hPortIo, 
		IOCTL_PRODIGY_NOKIA_POWER, 
		&UartPurgeParam,           // arguments structure
		sizeof(UartPurgeParam),    // structure size 
		NULL,                
		0,              
		&nBytesReturned,
		NULL
		);	  

	if (!status)
		return(1);

	return(0);
}


/*               NOKIA MBUS IMPLEMENTATION                  */


int ExecuteMbusCmdXp(unsigned char *out_buffer,unsigned char *responce,
					 UINT current,LONG *rbytes)
{
	//OutputDebugString("ExecuteMbusCmdXp->entry");

	status = DeviceIoControl(	hPortIo, 
		IOCTL_PRODIGY_MBUSPORT, 
		out_buffer, 
		current,
		responce,
		700,
		&nBytesReturned,
		NULL
		);

	if (!status){
		//OutputDebugString("ExecuteMbusCmdXp->err");
		return(false);
	}

	rbytes[0] = nBytesReturned;	  

	//DebugPrintHexArray(responce,nBytesReturned,"resp: ");
	//OutputDebugString("ExecuteMbusCmdXp->ok");

	return(true);
}


/******************************************************/
/******************************************************/
/******************************************************/
/*             NOKIA FLASH IMPLEMENTATION             */
/******************************************************/
/******************************************************/
/******************************************************/



//
//
unsigned short SetIdleStateXP(unsigned char *uppdata)					   
{

	unsigned char errcode[1];
	unsigned char *eCode = errcode;

	SetIdleStateParam.uppdata     =  uppdata;
	SetIdleStateParam.errcode     =  eCode;

	// Send Boot in Ring0
	status = DeviceIoControl(hPortIo, 
		IOCTL_PRODIGY_DCT4_SET_IDLE_STATE, 
		&SetIdleStateParam,           // arguments structure
		sizeof(SetIdleStateParam),    // structure size 
		NULL,                     
		0,                       
		&nBytesReturned,              // BootResult 
		NULL
		);	  

	if (!status)
		return(*eCode);

	return(0);
}

unsigned short InitBootstrapXP(
							   unsigned char	*bootf,
							   unsigned char	*boots,
							   unsigned char   *configdata,
							   unsigned char   *mread,
							   unsigned char	fMode
							   )

{

	unsigned char errcode[1];
	unsigned char *eCode = errcode;

	Dct4BootSendParam.bootf       =  bootf;
	Dct4BootSendParam.boots       =  boots;
	Dct4BootSendParam.errcode     =  eCode;
	Dct4BootSendParam.configdata  =  configdata;
	Dct4BootSendParam.mread       =  mread; 
	Dct4BootSendParam.fMode		  =  fMode;	

	// Send Boot in Ring0
	status = DeviceIoControl(hPortIo, 
		IOCTL_PRODIGY_DCT4SENDBOOT, 
		&Dct4BootSendParam,           // arguments structure
		sizeof(Dct4BootSendParam),    // structure size 
		NULL,                     
		0,                       
		&nBytesReturned,              // BootResult 
		NULL
		);	  

	if (!status)
		return(*eCode);

	return(0);
}

unsigned short SendAlgorithmXP(unsigned char *data,int count)
{

	unsigned char errcode[1];
	unsigned char *eCode = errcode;

	Dct4AlgoSendParam.data    = data;
	Dct4AlgoSendParam.count   = count;
	Dct4AlgoSendParam.errcode = eCode; 

	// Send 
	status = DeviceIoControl(hPortIo, 
		IOCTL_PRODIGY_DCT4SENDALGO, 
		&Dct4AlgoSendParam,           // arguments structure
		sizeof(Dct4AlgoSendParam),    // structure size 
		NULL,                     
		0,                       
		&nBytesReturned,              // Result 
		NULL
		);	  

	if (!status)
		return(errcode[1]);

	return(0);
}

unsigned short Dct4AuthXP(unsigned char *claudia, int size,unsigned char *rmsid,unsigned char *dmsid)
{

	unsigned char errcode[1];
	unsigned char *eCode = errcode;

	Dct4AuthParam.data    = claudia;
	Dct4AuthParam.count   = size;
	Dct4AuthParam.errcode = eCode;
	Dct4AuthParam.rmsid   = rmsid;
	Dct4AuthParam.dmsid   = dmsid;

	// Send 
	status = DeviceIoControl(hPortIo, 
		IOCTL_PRODIGY_DCT4AUTHENTIFICATION, 
		&Dct4AuthParam,           // arguments structure
		sizeof(Dct4AuthParam),    // structure size 
		NULL,                     
		0,                       
		&nBytesReturned,              // Result 
		NULL
		);	  

	if (!status)
		return(*eCode);

	return(0);
}

unsigned short EraseBlockXP(unsigned long StartAddress,unsigned long EndAddress,unsigned char *error)
{

	unsigned char errcode[1];
	unsigned char *eCode = errcode;

	Dct4EraseParam.StartAddress = StartAddress;
	Dct4EraseParam.EndAddress   = EndAddress; 
	Dct4EraseParam.errors       = error;
	Dct4EraseParam.errcode      = eCode; 

	// Send 
	status = DeviceIoControl(hPortIo, 
		IOCTL_PRODIGY_DCT4ERASE, 
		&Dct4EraseParam,           // arguments structure
		sizeof(Dct4EraseParam),    // structure size 
		NULL,                     
		0,                       
		&nBytesReturned,              // Result 
		NULL
		);	  

	if (!status)
		return(*eCode);

	return(0);
}

unsigned short WriteBlockXp(unsigned char *pBlockBuffer,unsigned short nBufferSize,unsigned char *pFlashMessage,unsigned char nMsgSize,unsigned char wType)
{

	unsigned char errcode[1];
	unsigned char *eCode = errcode;

	Dct4WriteBlockParam.BlockBuff       =		pBlockBuffer; 
	Dct4WriteBlockParam.nBufferSize     =		nBufferSize;
	Dct4WriteBlockParam.pFlashMessage   =		pFlashMessage;
	Dct4WriteBlockParam.nMsgSize        =		nMsgSize;
	Dct4WriteBlockParam.errcode         =		eCode;
	Dct4WriteBlockParam.wType           =		wType;

	// Flash Block in Ring0
	status = DeviceIoControl(hPortIo, 
		IOCTL_PRODIGY_WRITEBLOCK, 
		&Dct4WriteBlockParam,           // arguments structure
		sizeof(Dct4WriteBlockParam),    // structure size 
		NULL,                     
		0,                       
		&nBytesReturned,
		NULL
		);	  

	if (!status)
		return(*eCode);

	return 0;
}

// only 2 bytes block
//
unsigned short SendToPhoneXP(unsigned char *pBlockBuffer,unsigned long BlockSize,unsigned long DelayLow,unsigned long DelayHigh)
{

	unsigned char errcode[1];
	unsigned char *eCode = errcode;

	Dct4SendToPhoneParam.pBlockBuffer  = pBlockBuffer;
	Dct4SendToPhoneParam.BlockSize     = BlockSize; 
	Dct4SendToPhoneParam.DelayLow      = DelayLow;
	Dct4SendToPhoneParam.DelayHigh     = DelayHigh;
	Dct4SendToPhoneParam.errcode		= eCode; 

	// Send 
	status = DeviceIoControl(hPortIo, 
		IOCTL_PRODIGY_SENDTOPHONE, 
		&Dct4SendToPhoneParam,           // arguments structure
		sizeof(Dct4SendToPhoneParam),    // structure size 
		NULL,                     
		0,                       
		&nBytesReturned,              // Result 
		NULL
		);	  

	if (!status)
		return(*eCode);

	return(0);
}

unsigned short ProgrammingStatusXP(unsigned char *error)
{

	unsigned char errcode[1];
	unsigned char *eCode = errcode;

	ProgStatusParam.error    = error;
	ProgStatusParam.errcode = eCode;

	// Send 
	status = DeviceIoControl(hPortIo, 
		IOCTL_PRODIGY_PROGSTATUS, 
		&ProgStatusParam,           // arguments structure
		sizeof(ProgStatusParam),    // structure size 
		NULL,                     
		0,                       
		&nBytesReturned,              // Result 
		NULL
		);	  

	if (!status)
		return(*eCode);

	return(0);
}

unsigned short MCMHardwareVersionXP(unsigned char *mcmver)
{

	unsigned char errcode[1];
	unsigned char *eCode = errcode;

	McmVerParam.mcmver    = mcmver;
	McmVerParam.errcode = eCode;

	// Send 
	status = DeviceIoControl(hPortIo, 
		IOCTL_PRODIGY_MCMHWVERSION, 
		&McmVerParam,           // arguments structure
		sizeof(McmVerParam),    // structure size 
		NULL,                     
		0,                       
		&nBytesReturned,              // Result 
		NULL
		);	  

	if (!status)
		return(*eCode);

	return(0);
}

unsigned short SetDefaultTimesA(unsigned char *deftimes)
{

	unsigned char errcode[1];
	unsigned char *eCode = errcode;

	Dct4DefTimesParam.deftimes = deftimes;
	Dct4DefTimesParam.errcode  = eCode;

	// Send 
	status = DeviceIoControl(hPortIo, 
		IOCTL_PRODIGY_DEFAULT_TIMES, 
		&Dct4DefTimesParam,           // arguments structure
		sizeof(Dct4DefTimesParam),    // structure size 
		NULL,                     
		0,                       
		&nBytesReturned,              // Result 
		NULL
		);	  

	if (!status)
		return(*eCode);

	return(0);
}

//
//
unsigned short Dct3PowerUp(void)
{

	// Send 
	status = DeviceIoControl(hPortIo, 
		IOCTL_PRODIGY_DCT3_POWER_UP, 
		NULL,          
		0,    
		NULL,                     
		0,                       
		&nBytesReturned,              // Result 
		NULL
		);	  

	if (!status)
		return(1);

	return(0);
}

unsigned short UEMCalls(unsigned char *upData, unsigned char upSize,unsigned char upType,unsigned char CallSwitch)
{
	unsigned char errcode[1];
	unsigned char *eCode = errcode;

	Dct4UemParam.CallSwitch = CallSwitch;
	Dct4UemParam.upData     = upData;
	Dct4UemParam.upSize		= upSize;
	Dct4UemParam.upType		= upType;
	Dct4UemParam.errcode	=  eCode;

	// Send 
	status = DeviceIoControl(hPortIo, 
		IOCTL_PRODIGY_UEM_CALLS, 
		&Dct4UemParam,           // arguments structure
		sizeof(Dct4UemParam),    // structure size 
		NULL,                     
		0,                       
		&nBytesReturned,              // Result 
		NULL
		);	  

	if (!status)
		return(*eCode);


	return(0);
}

/******************************************************/
/******************************************************/
/******************************************************/



// Read device details
//
unsigned char DeviceControlHandler(unsigned char *devicedata,unsigned char mode,unsigned char entry)
{

	unsigned char Ecode[1];
	unsigned char *errCode = Ecode;

	DevData.mEntry = entry;		// func switch
	DevData.pData = devicedata;	// ptr to data
	DevData.nMode = mode;		// submode in device
	DevData.nError = errCode;	// err code here

	status = DeviceIoControl(hPortIo, 
		IOCTL_PRODIGY_DEVDATA, 
		&DevData, 
		sizeof(DevData),
		NULL,
		0,
		&nBytesReturned,
		NULL
		);	  

	if (!status){
		return(*errCode); 
	}


	return(0);
}

unsigned short OpenVxdDriver(void)
{
	const PCHAR VxDName = "\\\\.\\PRODIGY";

	//OutputDebugString("open vxd");

	// Open the VxD
	hPortIo = CreateFile(VxDName, 0,0,0, CREATE_NEW, 0, 0);

	if (hPortIo == INVALID_HANDLE_VALUE)
	{
		//hDevice = LoadAndOpenVxD(VxDFile, VxDName);

		//if (hDevice == INVALID_HANDLE_VALUE)
		//{
		//fprintf(stderr, "Cannot load VxD, error=%08lx\n",GetLastError());
		return(1);
		//}
	}

	return(0);
}

unsigned short OpenAfdDriver(HANDLE *pHandle,unsigned long PortNo)
{
	unsigned short StartResult;

	// Start the driver
	StartResult = StartDriver();
	if(StartResult != 0) 
		return StartResult;

	// Open port
	OpenPort.open_Address = PortNo;
	OpenPort.open_PortSize = 1;
	OpenPort.open_PortCount = 3;

	status = DeviceIoControl(	hPortIo, 
		IOCTL_PRODIGY_OPENPORT, 
		&OpenPort, 
		sizeof(OpenPort),
		&PortHandle,
		sizeof(PortHandle),
		&nBytesReturned,
		NULL
		);

	if ( !status || (PortHandle == NULL) )
		return(10);

	*pHandle = hPortIo;

	return(0);
}

unsigned short StartDriver(void)
{

	SC_HANDLE  SchSCManager;
	SC_HANDLE  schService;
	int       ret;
	DWORD      err;
	unsigned char InstallResult;

	// Open Handle to Service Control Manager 
	SchSCManager = OpenSCManager (NULL,// machine (NULL == local)
		NULL,// database (NULL == default)
		SC_MANAGER_ALL_ACCESS // access required
		);

	if (SchSCManager == NULL)
		if (GetLastError() == ERROR_ACCESS_DENIED) {
			// We do not have enough rights to open the SCM, therefore we must
			// be a poor user with only user rights. Check if we can open 
			// handle to driver 
			if (OpenHandleToDriver()) {
				//printf("The driver is running. [USER RIGHTS]\n");
				return(0); // All done 
			}
			// You do not have rights to access the Service Control Manager and\n");
			// the USBLPT-PD11 driver is not installed or stated. Please ask \n");
			// USBLPT-PD11: your administrator to install the driver on your behalf.\n");
			CloseServiceHandle (SchSCManager);
			return(1);
		}

		do {
			// Open a Handle to the Service Database
			schService = OpenService (SchSCManager,	// handle to service control manager database
				"PRODIGY",    // pointer to name of service to start
				SERVICE_ALL_ACCESS   // type of access to service
				);

			if (schService == NULL)
				switch (GetLastError()){
				case ERROR_ACCESS_DENIED:
					//You do not have rights to the prodigyXPDEV service database\n");
					CloseServiceHandle (schService);
					return(2);

				case ERROR_INVALID_NAME:
					//The specified service name is invalid.\n");
					CloseServiceHandle (schService);
					return(3);

				case ERROR_SERVICE_DOES_NOT_EXIST:
					//printf(" The driver does not exist. Installing driver.\n");
					//printf(" This can take up to 30 seconds on some machines . .\n");
					InstallResult = InstallAfdDriver();
					if(InstallResult != 0)
						return(InstallResult);
			}
		} while (schService == NULL);

		// Start the Driver. Errors will occur here if.SYS file doesn't exist
		ret = StartService (schService, // service identifier
			0,          // number of arguments
			NULL        // pointer to arguments
			);

		if (ret) {}//printf("The driver has been successfully started.\n");
		else {
			err = GetLastError();
			if (err == ERROR_SERVICE_ALREADY_RUNNING)
			{}//printf("The driver is already running.\n");
			else if (err == ERROR_SUCCESS )
			{}
			else {
				//Unknown error while starting prodigyXPDEV driver service.\n");
				//Does exist in your \\System32\\Drivers Directory?\n");
				CloseServiceHandle (schService);
				return(4);
			}
		}

		// Now driver is loaded and started, Open handle to driver
		// Driver so we can talk to it 
		if (!OpenHandleToDriver()) {
			// Couldn't create handle to driver, Please ensure driver is loaded.\n");
			CloseServiceHandle (schService);
			return(5);   
		}

		// Close handle to Service Control Manager 
		CloseServiceHandle (schService);

		return(0);
}

int OpenHandleToDriver(void)
{

	hPortIo = CreateFile("\\\\.\\Prodigy",
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		0,
		NULL);


	if(hPortIo == INVALID_HANDLE_VALUE) 
		return(FALSE);
	else	    						 
		return(TRUE);

}

unsigned char InstallAfdDriver(void)
{
	SC_HANDLE  SchSCManager;
	SC_HANDLE  schService;
	DWORD      err;
	CHAR 		DriverFileName[80];

	// Get Current Directory. Assumes .SYS driver is in this directory. 
	// Doesn't detect if file exists, nor if file is on removable media - if this  
	// is the case then when windows next boots, the driver will fail to load and  
	// error entry is made in the event viewer to reflect this

	// Get System Directory. This should be something like c:\windows\system32 or  
	// c:\winnt\system32 with a Maximum Character lenght of 20. As we have a       
	// buffer of 80 bytes and a string of 24 bytes to append, we can go for a max  
	// of 55 bytes 

	if (!GetSystemDirectory(DriverFileName, 55))
	{
		//Failed to get System Directory. Is System Directory Path > 55 Characters?
		//Please manually copy driver to your system32/driver directory
		return (11);
	}

	// Append our Driver Name 
	lstrcat(DriverFileName,"\\Drivers\\PROGIGY.SYS");
	//Copying driver to,DriverFileName

	// Copy Driver to System32/drivers directory. This fails if the file doesn't 
	// exist. 

	// if (!CopyFile("prodigyxpdev.sys", DriverFileName, FALSE))
	//     {
	//Failed to copy driver to ...
	//Please manually copy driver to your system32/driver directory
	//      return(12);    
	//}

	// Open Handle to Service Control Manager 
	SchSCManager = OpenSCManager (NULL,                 // machine (NULL == local)
		NULL,                 // database (NULL == default)
		SC_MANAGER_ALL_ACCESS // access required
		);

	// Create Service/Driver - This adds the appropriate registry keys in 
	// HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Services - It doesn't  
	// care if the driver exists, or if the path is correct. 
	schService = CreateService (SchSCManager,             // SCManager database
		"PRODIGY",         // name of service
		"PRODIGY",         // name to display
		SERVICE_ALL_ACCESS,    // desired access
		SERVICE_KERNEL_DRIVER, // service type
		SERVICE_DEMAND_START,  // start type
		SERVICE_ERROR_NORMAL,  // error control type
		"System32\\Drivers\\PRODIGY.SYS",   // service's binary
		NULL,                  // no load ordering group
		NULL,                  // no tag identifier
		NULL,                  // no dependencies
		NULL,                  // LocalSystem account
		NULL                   // no password
		);

	if (schService == NULL)
	{
		err = GetLastError();
		if (err != ERROR_SERVICE_EXISTS)
		{   //Unknown error while creating Service
			return(13);
		}
	}
	//else Driver successfully installed

	// Close Handle to Service Control Manager 
	CloseServiceHandle (schService);

	return(0);
}

// Close 98 driver
//
unsigned short CloseVxdDriver(void)
{
	//OutputDebugString("close vxd");
	CloseHandle(hPortIo);
	return(0);
}

// Close Nt driver
//
unsigned short CloseNTDriver(void)
{

	// Close data port
	ClosePort.close_Handle = PortHandle;				
	status = DeviceIoControl(	hPortIo, 
		IOCTL_PRODIGY_CLOSEPORT, 
		&ClosePort, 
		sizeof(ClosePort),
		NULL,
		0,
		&nBytesReturned,
		NULL
		);

	//printf("close failed: %x\n", GetLastError()); 
	CloseHandle(hPortIo);

	return true;
}

// Set Sub mode in device
//
unsigned char SetSubMode(unsigned char Mode)
{
	if(!SetDeviceMode(Mode,1))
		return(1);


	return (0);
}

// Return from submode to main menu
//
unsigned char ResetSubMode(VOID)
{

	if(!SetDeviceMode(0,2))
		return(1);


	return(0);
}

// Device Control Entry
// mEntry - 0 - set device mode
//          1 - set sub mode
//          2 - reset sub mode
//			3 - write device byte
//			4 - read device byte
//
int SetDeviceMode(unsigned char cmdMode,unsigned char mEntry)
{	

	unsigned char dummy[1];

	dummy[0]		= cmdMode;
	SetMode.mEntry	= mEntry;
	SetMode.nMode	= &dummy[0];

	status = DeviceIoControl(hPortIo, 
		IOCTL_PRODIGY_SETDEVMODE, 
		&SetMode, 
		sizeof(SetMode),
		NULL,
		0,
		&nBytesReturned,
		NULL
		);

	if ( !status )
		return(false);     

	return(true); 
}

