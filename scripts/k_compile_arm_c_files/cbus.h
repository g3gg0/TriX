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

#ifndef CBUS_H
#define CBUS_H

/* External asm functions */

extern void   hw_cbus_init(void);
//extern ulong read_uem_imei(uchar *ucImei);

extern ulong read_uem_reg(ulong reg);
extern void	 write_uem_reg(register ulong ulReg, register ulong usVal); 			

extern uchar	ucPrefetchCipherInitilized;

/* C impl */
ulong  	CbusInit(void);
uchar 	CbusWaitBusy(void);
ulong   CbusSendAck(ushort usCbusDevice,ushort Arg1,ushort Arg2);

//ushort 	ReadCbusWord(ulong ulRegister);

#endif