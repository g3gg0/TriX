;/************************************************************************************
;**                                                                                 **
;**                            DCT4 NOKIA CUSTOM ALGORITHM                          **
;**                                  JuniorJack, 2005                               **
;**                                                                                 **
;**---------------------------------------------------------------------------------**
;**                                                                                 **
;**  File name:                                                                     **
;**  Description:                                                                   **
;**  Last Modified:                                                                 **
;**                                                                                 **
;************************************************************************************/

                AREA        BOOTROMASM, CODE, READONLY
                CODE16
				
				EXPORT		AsmBootcall


AsmBootcall
				PUSH	{LR}

;- ptr via R0, 0x1F800 in RAM
				
				MOV		R0, #0xFC
                LSL     R0, R0, #9                			

;- decode size				               
                MOV		R1,#0x08
                
;- ram limit or something
             
                ADD		R2,	R0,	R1

;- decode type                
                MOV     R3, #0x20

;- jump address + 1 (Thumb jump)               

                LDR		R4, =0x80038B

;- save PC and jump
                
                MOV		LR,	PC
                              
                BX		R4
                
				
				POP		{PC}

				END
