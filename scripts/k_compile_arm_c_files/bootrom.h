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


#ifndef bootrom_h
#define bootrom_h


void 	BootCbusUnlock(uchar *pucAuthBuffer);
void 	BootDecodeA(uchar *pucBuffer,ulong ulSize,uchar ucEncType, ulong ulFuncAddr);
void 	BootX(uchar *pucBuffer);

void  	l_memcpy(uchar *ucDest,uchar *ucSrc, ulong ulSize);
void  	l_memset(uchar *ucDest,uchar ucVal, ulong ulSize);

#ifdef  TIKU

uchar 	ReadTikuAsicType(void);
void 	CopyFromFlashSwapped(uchar *ucBuffer,uchar *ucMem, ulong ulSize);

#endif

#endif