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

#include <string.h>

#include "bootdef.h"
#include "bootrom.h"

/* Bootrom call prototype */
void 	(*pvBootCall)(register uchar *,register ulong,register uchar *,register uchar);
void 	(*pvBootCallx)(register uchar *,register ulong,register uchar,register uchar);
void 	(*pvBootCally)(register uchar *,register uchar *);
//void 	(*pvBootCallz)(register uchar *,register ulong);
uchar 	(*pvBootCallu)(void);

#ifdef	DCT4
#define CBUS_UNLOCK_REG		0x084001F
#endif

#ifdef	WD2
#define CBUS_UNLOCK_REG		0xC000D01
#endif

/*******************************************************************************/

/* Call the bootrom to unlock the Cbus 

   - maybe function declaration is not correct
*/
void BootCbusUnlock(uchar *pucAuthBuffer)
{
#ifdef	DCT4
	uchar *pucFirstParam = pucAuthBuffer + 0x0D;
#else	// WD2
	uchar *pucFirstParam = pucAuthBuffer + 0x0C;
#endif	

	/* Jump in Thumb mode, the ulFuncAddr should have the T flag already set! */
	pvBootCallx = (void (*)(register uchar *,register ulong,register uchar,register uchar))CBUS_UNLOCK_REG;
    (pvBootCallx)(pucAuthBuffer,*pucFirstParam,0x0B,0x00);
}

/* Call to bootrom decode - this will accept larger size blocks */
void BootDecodeA(uchar *pucBuffer,ulong ulSize,uchar ucEncType, ulong ulFuncAddr)
{
	uchar *pucRamLimit = pucBuffer + ulSize;	/* reserved max size */
	
	/* Jump in Thumb mode, the ulFuncAddr should have the T flag already set! */
	pvBootCall = (void (*)(register uchar *,register ulong,register uchar *,register uchar))ulFuncAddr;
    (pvBootCall)(pucBuffer,ulSize,pucRamLimit,ucEncType); 
}

/* First 8 bytes of an ask file */
void BootX(uchar *pucBuffer)
{
	//ulong	ulVal;
   
   	#ifdef  DCT4
   
   		uchar 	*pucSecPtr;
   		
		pucSecPtr = pucBuffer + 0x40;
		
		/* Jump in Thumb mode, the ulFuncAddr should have the T flag already set! */
		pvBootCally = (void (*)(register uchar *,register uchar *))0x084001B;
	    (pvBootCally)(pucBuffer,pucSecPtr); 
	    
	#endif	 
	
	#ifdef  TIKU
   
   		//pucSecPtr = pucBuffer + 0x84;
   		//ulVal = 0xC300000;
   		   		
   		/* Unkown bootrom call, to init something in the source array */
		//pvBootCallz = (void (*)(register uchar *,register ulong))0xC000FA9;
	    //(pvBootCallz)(pucSecPtr,ulVal); 
	    
		/* Generate the 8 bytes ?? */
		//pvBootCally = (void (*)(register uchar *,register uchar *))0xC000F9B;
	    //(pvBootCally)(pucBuffer,pucSecPtr); 
	    
	#endif	    
	
}  

void l_memcpy(uchar *ucDest,uchar *ucSrc, ulong ulSize)
{
	ulong i;

	for(i = 0;i < ulSize;i++)
		*ucDest++ = *ucSrc++;
}

void l_memset(uchar *ucDest,uchar ucVal, ulong ulSize)
{
	ulong i;

	for(i = 0;i < ulSize;i++)
		*ucDest++ = ucVal;
}

#ifdef  TIKU


/* Read the asic type from bootrom */
uchar ReadTikuAsicType(void)
{		
	pvBootCallu = (uchar (*)(void))0xC000FA5;
	return (pvBootCallu)();
}

/* Swap endianes in TIKU phone */
void CopyFromFlashSwapped(uchar *ucBuffer,uchar *ucMem, ulong ulSize)
{
	ulong i;
	
	for(i = 0;i < ulSize;i+=2)
	{	
		*(ucBuffer + 0 + i) = *(ucMem + 1 + i);		
		*(ucBuffer + 1 + i) = *(ucMem + 0 + i);
	}	
}

#endif