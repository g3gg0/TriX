/************************************************************************************
**                                                                                 **
**                            DCT4 NOKIA CUSTOM ALGORITHM                          **
**                                  JuniorJack, 2005                               **
**                                                                                 **
**---------------------------------------------------------------------------------**
**                                                                                 **
**  File name:                                                                     **
**  Description:                                                                   **
**  Last Modified:                                                                 **
**                                                                                 **
************************************************************************************/

//#include <string.h>

#include "bootdef.h"

#include "cbus.h"
#include "bootrom.h"

#ifdef  DCT4 
#define CBUS_REG_A	   		0x06010420
#define CBUS_REG_B	   		0x06010422
#define CBUS_REG_C	   		0x06010424
#define CBUS_REG_D	   		0x06010426
#define FAID_START			0x0100002C
#endif

#ifdef  TIKU
#define CBUS_REG_A	   		0x0C308420
#define CBUS_REG_B	   		0x0C308422
#define CBUS_REG_C	   		0x0C308424
#define CBUS_REG_D	   		0x0C308426
#define FAID_START			0x0000012C

extern void   cbus_unlock(uchar *ucPassBuffer,uchar ucUnk,uchar ucSize);
#endif

#ifdef  WD2
#define CBUS_REG_A	   		0x0C150420
#define CBUS_REG_B	   		0x0C150422
#define CBUS_REG_C	   		0x0C150424
#define CBUS_REG_D	   		0x0C150426
#define FAID_START			0x0000012C

//extern void   cbus_unlock(uchar *ucPassBuffer,uchar ucUnk,uchar ucSize);
#endif

/* some pub var required by cbus_write */
//uchar reg_arr[38];

/* Init Cbus protocol */
ulong CbusInit(void)
{
	uchar *pucFaid = (uchar *)FAID_START;
	uchar ucFaidBuffer[14];
										
	/* First we need to unlock the cbus via bootrom call */
	//#ifdef  DCT4		    	
		
		/* Copy to Ram */
	//	l_memcpy(ucFaidBuffer,pucFaid,14);	
		
	//	BootCbusUnlock(ucFaidBuffer);
	
	//#endif

	#ifdef  TIKU
				
		/* Copy to Ram */
		CopyFromFlashSwapped(ucFaidBuffer,pucFaid,12);
		
		cbus_unlock(ucFaidBuffer,*(pucFaid + 12),11);
		
	#else
		
		/* Copy to Ram */
		l_memcpy(ucFaidBuffer,pucFaid,14);	
		
		BootCbusUnlock(ucFaidBuffer);
	
	#endif
		
	/* Set init flag */
	ucPrefetchCipherInitilized = 1;
	
	/* Now init the communication */
	hw_cbus_init(); 	
	
	/* We need to read a register to init the
	   communication, maybe bug in my impll. */
	return read_uem_reg(0x23FC00);
}

/* Wait for free bus */
uchar CbusWaitBusy(void)
{
	ushort i = 0x400;
	ushort *usCbusReg = (ushort *)CBUS_REG_B;
	
	/* Wait till busy for a given timeout */
	while(i != 0)
	{		
		if((*usCbusReg >> 0xA) & 1)
		  return 1;
	
		i--;
	}

	return 0;
}

/* Maybe read byte */
ulong CbusSendAck(ushort usCbusDevice,ushort Arg1,ushort Arg2)
{
	ushort *usCbusReg1 = (ushort *)CBUS_REG_A;
	ushort *usCbusReg2 = (ushort *)CBUS_REG_B;
	ushort *usCbusReg3 = (ushort *)CBUS_REG_C;
	ushort *usCbusReg4 = (ushort *)CBUS_REG_D;
	
	CbusWaitBusy();
	
	*usCbusReg1 = Arg2;
	
	usCbusDevice  = usCbusDevice << 5;
	usCbusDevice |= Arg1;
	usCbusDevice |= 0x100;
	
	/* Select device */
	*usCbusReg2 = usCbusDevice;
		
	CbusWaitBusy();
	
	return ((*usCbusReg3 << 16) | *usCbusReg4);
}

