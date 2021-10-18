;- CBUS asm implementation
;- 
;- these functions have to be checked and recoded in C
;-	
	AREA        CBUSASM, CODE, READONLY	
	CODE16
				
	IF :DEF:TIKU
				EXPORT		cbus_unlock
	ENDIF
				
				EXPORT		hw_cbus_init															
				EXPORT		read_uem_reg			
																																																
				IMPORT		CbusWaitBusy
				IMPORT		CbusSendAck
				
				IMPORT		ReadUppId				

;************************************************************************

	IF :DEF:TIKU
	
;- TIKU specific
cbus_unlock			
					
		PUSH	{R4,R5,LR}
		LDR		R3, =0xC305300		
		MOV		R4,	#0x1F
		LSL		R4,	R4,#8		
		STRH	R4, [R3]
		STRH	R1, [R3,#0x36]
		MOV		R4, #6
		LDR		R5, =0xC30531A

loc_C0B0ECC				

		LDRB	R1, [R0]
		LSL		R1, R1,	#0x18
		LSR		R1, R1,	#0x10
		ADD		R0, #1
		LDRB	R3, [R0]
		ORR		R3, R1
		LSL		R1, R3,	#0x10
		LSR		R1, R1,	#0x10
		ADD		R0, #1
		STRH	R1, [R5]
		ADD		R5, #2
		SUB		R4, #1
		BNE		loc_C0B0ECC
		LDR		R0, =0xC305300
		STRH	R2, [R0]
		POP		{R4,R5,PC}
		
; End of function cbus_unlock

	ENDIF
	
;************************************************************************
;-
hw_cbus_init                           

                PUSH    {R1-R7,LR}
                BL      get_system_type
                ADD     R5, R0, #0
                SUB     R0, R0, #1
                BEQ     loc_50B2
                SUB     R0, #1
                BEQ     loc_50AE
                SUB     R0, #1
                BEQ     loc_50AA
                SUB     R0, #1
                CMP     R0, #1
                BLS     loc_50A6
                SUB     R0, #2
                BEQ     loc_50A2
                MOV     R4, #0
                B       loc_50B4
; ---------------------------------------------------------------------------

loc_50A2                                
                MOV     R4, #0x5D 
                B       loc_50B4
; ---------------------------------------------------------------------------

loc_50A6                                
                MOV     R4, #0x66 
                B       loc_50B4
; ---------------------------------------------------------------------------

loc_50AA                                
                MOV     R4, #0x6C 
                B       loc_50B4
; ---------------------------------------------------------------------------

loc_50AE                               
                MOV     R4, #0x57 
                B       loc_50B4
; ---------------------------------------------------------------------------

loc_50B2                               
                MOV     R4, #0x4B 

loc_50B4                          
				                                                  
                MOV     R0, #0
                BL      hw_cbus_clock
                	                
    IF :DEF:DCT4
                LDR     R0, =0x6010430
    ENDIF
    
    IF :DEF:TIKU
                LDR     R0, =0xC308430
	ENDIF
	
	IF :DEF:WD2
                LDR     R0, =0xC150430
	ENDIF
                
                STRH    R4, [R0]
                
    IF :DEF:DCT4
                LDR     R0, =0x6010000
    ENDIF
                
    IF :DEF:TIKU
                LDR     R0, =0xC308000
    ENDIF
    
    IF :DEF:WD2
                LDR     R0, =0xC150000
    ENDIF
                
                LDRH    R1, [R0,#2]
                LSR     R1, R1, #3
                BCC     loc_50D8
                CMP     R5, #2
                BEQ     loc_50D2
                CMP     R5, #4
                BEQ     loc_50D2
                CMP     R5, #5
                BNE     loc_50D8

loc_50D2                                
                                                    
                MOV     R1, #0x40 
                STRH    R1, [R0,#0x34]
                B       loc_50DC
; ---------------------------------------------------------------------------

loc_50D8                                 
                                      
                MOV     R1, #0x40 
                STRH    R1, [R0,#0x36]

loc_50DC                                
                MOV     R0, #1
                BL      hw_cbus_clock
                POP     {R1-R7,PC}
; End of function hw_cbus_init


;************************************************************************
get_system_type             
	
	IF :DEF:DCT4				             
                LDR     R0, =0x6010000
                LDRH    R1, [R0,#2]
                LSR     R1, R1, #4
                BCC     loc_3A42
                MOV     R0, #4
                MOV     PC, LR
; ---------------------------------------------------------------------------

loc_3A42                                
                LDRH    R1, [R0,#2]
                LSR     R1, R1, #6
                BCC     loc_3A4C
                MOV     R0, #2
                MOV     PC, LR
; ---------------------------------------------------------------------------

loc_3A4C                                
                LDRH    R0, [R0,#2]
                MOV     R0, #1                
	ENDIF
	
	IF :DEF:TIKU
	 			MOV     R0, #5                	 
	ENDIF 
	
	IF :DEF:WD2
	 			MOV     R0, #1
	ENDIF 
	
				MOV     PC, LR
	                 
; End of function get_system_type


;************************************************************************
hw_cbus_clock                           
                                                   
                CMP     R0, #0
                BEQ     loc_510A
                SUB     R0, #1
                BEQ     loc_50FE
                SUB     R0, #1
                BEQ     loc_50F2
                MOV     PC, LR
; ---------------------------------------------------------------------------

loc_50F2                      

	IF :DEF:DCT4         
                LDR     R0, =0x6010000
	ENDIF  
	
	IF :DEF:TIKU
                LDR     R0, =0xC308000
	ENDIF   
	
	IF :DEF:WD2
                LDR     R0, =0xC150000
	ENDIF                
	
                MOV     R1, #0x20 
                STRH    R1, [R0,#0x36]
                MOV     R1, #0xC
                STRH    R1, [R0,#0x34]
                MOV     PC, LR
; ---------------------------------------------------------------------------

loc_50FE                              

	IF :DEF:DCT4
                LDR     R0, =0x6010000
	ENDIF   
	
	IF :DEF:TIKU
                LDR     R0, =0xC308000
	ENDIF  
	
	IF :DEF:WD2
                LDR     R0, =0xC150000
	ENDIF                
                
                MOV     R1, #8
                STRH    R1, [R0,#0x36]
                MOV     R1, #0x24
                STRH    R1, [R0,#0x34]
                MOV     PC, LR
; ---------------------------------------------------------------------------

loc_510A                              
                MOV     R0, #0x2C 
	
	IF :DEF:DCT4                
                LDR     R1, =0x6010036
	ENDIF
	
	IF :DEF:TIKU
                LDR     R1, =0xC308036
	ENDIF
	
	IF :DEF:WD2
                LDR     R1, =0xC150036
	ENDIF
	                
                STRH    R0, [R1]
                MOV     PC, LR
; End of function hw_cbus_clock
					
; 컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�
		
;- algo table		
	
	IF :DEF:DCT4    
	
regtbla	DCD 	0x00000000
		DCD 	0x3B004000
        DCD 	0x3B008000
        DCD 	0x3B00C000
        DCD 	0x3B010000
        DCD 	0x3C014000
        DCD 	0x3A898000
        DCD 	0x3B01C000
        DCD 	0x3B020000
        DCD 	0x3A0A4000
        DCD 	0x3A128000
        DCD 	0x3A1AC000
        DCD 	0x3A230000
        DCD 	0x3A2B4000
        DCD 	0x3A338000
        DCD 	0x3A3BC000
        DCD 	0x3A440000
        DCD 	0x3A4C4000
        DCD 	0x3A548000
        DCD 	0x3A5CC000
        DCD 	0x7B050000
        DCD 	0x7C054000
        DCD 	0x7A958000
        DCD 	0x7A65C000
        DCD 	0x7A6E0000
        DCD 	0x7A764000
        DCD 	0x7A7E8000
        DCD 	0x7906C000
        DCD 	0x79070000
        DCD 	0x79074000
        DCD 	0x79078000
        DCD 	0x7907C000
        DCD 	0x39044000
        DCD 	0x39048000
        DCD 	0x3904C000
        DCD 	0x79068000
        DCD 	0x7A86C000
        DCD 	0x79000000
        
	ENDIF
	
	IF :DEF:TIKU  
	
regtbla	DCD 0							
		DCD 0x2067
		DCD 0x4067
		DCD 0x6067
		DCD 0x8067
		DCD 0xA087
		DCD 0xD147
		DCD 0xE067
		DCD 0x10067
		DCD 0x12147
		DCD 0x14247
		DCD 0x16347
		DCD 0x18447
		DCD 0x1A547
		DCD 0x1C647
		DCD 0x1E747
		DCD 0x20847
		DCD 0x22947
		DCD 0x24A47
		DCD 0x26B47
		DCD 0x2806F
		DCD 0x2A08F
		DCD 0x2D24F
		DCD 0x2EC4F
		DCD 0x30D4F
		DCD 0x32E4F
		DCD 0x34F4F
		DCD 0x3602F
		DCD 0x3802F
		DCD 0x3A02F
		DCD 0x3C02F
		DCD 0x3E02F
		DCD 0x22027
		DCD 0x24027
		DCD 0x26027
		DCD 0x3402F
		DCD 0x3704F

tbl2	DCD 0x29CC1232	
		DCD 0xD6E941AD
		DCD 0xA524AA8F
		DCD 0xC9DE4E04
		DCD 0xDF07F346
		DCD 0x1026E6CA
		DCD 0xE925732D
		DCD 0xFAE221DD
		DCD 0x3076AE87
		DCD 0xF243CD44	
				
	ENDIF
	
	IF :DEF:WD2
				
regtbla	DCD 0							
		DCD 0x2067
		DCD 0x4067
		DCD 0x6067
		DCD 0x8067
		DCD 0xA087
		DCD 0xD147
		DCD 0xE067
		DCD 0x10067
		DCD 0x12147
		DCD 0x14247
		DCD 0x16347
		DCD 0x18447
		DCD 0x1A547
		DCD 0x1C647
		DCD 0x1E747
		DCD 0x20847
		DCD 0x22947
		DCD 0x24A47
		DCD 0x26B47
		DCD 0x2806F
		DCD 0x2A08F
		DCD 0x2D24F
		DCD 0x2EC4F
		DCD 0x30D4F
		DCD 0x32E4F
		DCD 0x34F4F
		DCD 0x3602F
		DCD 0x3802F
		DCD 0x3A02F
		DCD 0x3C02F
		DCD 0x3E02F
		DCD 0x22027
		DCD 0x24027
		DCD 0x26027
		DCD 0x3402F
		DCD 0x3704F				

tbl2	DCD 0xCED980E0
		DCD 0xA80B606F
		DCD 0x200C9130
		DCD 0x1A0A3A47
		DCD 0x2237D9EA
		DCD 0x6F34DBEF
		DCD 0x9B3CFE1D
		DCD 0x2011672F
		DCD 0x92A782A7
		DCD 0x9F919E59
						
	ENDIF
					
;************************************************************************
;-
;-
read_uem_reg                            
                                                    
                PUSH    {R1-R7,LR}
	
	IF :DEF:DCT4    
	                
                LSL     R1, R0, #0x10
                LSR     R4, R1, #0x10	; mask here
                
                LSL     R0, R0, #0xA
                LSR     R0, R0, #0x1A	; reg only
                
                LSL     R1, R0, #2		; x4
                LDR     R2, =regtbla
                LDR     R2, [R2,R1]
                MOV     R1, #1
                LSL     R3, R1, #0x19
                LSL     R1, R2, #5
                LSR     R1, R1, #0x1D
                LSL     R1, R1, #0x18
                CMP     R1, R3
                BEQ     loc_4DCW8
                LSL     R0, R2, #0xD
                LSR     R0, R0, #0x1B
                LSL     R0, R0, #0x18
                LSR     R1, R0, #0x18
                LSR     R0, R2, #0x1B
                LSL     R0, R0, #0x18
                LSR     R2, R0, #0x18
                MOV     R0, #1
                                                
                BL		CbusSendAck
                
                B       loc_4DEA
; ---------------------------------------------------------------------------

loc_4DCW8                                
                LDR     R1, =regtbla
                LSL     R0, R0, #2
                ADD     R0, R1, R0
                LDRB    R0, [R0,#1]
                LSR     R1, R0, #2
                MOV     R0, #0x3E 
                AND     R0, R1
                ;LDR     R1, =reg_arr
                ;LDRH    R0, [R1,R0]

loc_4DEA                               
                AND     R0, R4
                B       loc_4DF2
;- ---------------------------------------------------------------------------

loop11                                   
                LSR     R0, R0, #1
                LSR     R4, R4, #1

loc_4DF2                                
                LSR     R1, R4, #1
                BCC     loop11
    
    ENDIF
    
    IF :DEF:TIKU
    
			    LSL		R1, R0,	#0x10
				LSR		R4, R1,	#0x10
				LSL		R0, R0,	#0xA
				LSR		R0, R0,	#0x1A
				LSL		R1, R0,	#2
				LDR		R2, =regtbla
				LDR		R2, [R2,R1]
				MOV		R1, #0xE0
				AND		R1, R2
				CMP		R1, #0x40
				BEQ		loc_C0B16D2
				MOV		R0, #0x1F
				LSR		R1, R2,	#0xD
				AND		R1, R0
				LSL		R1, R1,	#0x18
				LSR		R1, R1,	#0x18
				AND		R2, R0
				LSL		R0, R2,	#0x18
				LSR		R2, R0,	#0x18
				MOV		R0, #1		
			    BL		CbusSendAck	
				B		loc_C0B16E2

loc_C0B16D2				
		
				LDR		R1, =regtbla
				LSL		R0, R0,	#2
				ADD		R0, R1,	R0
				LDRB	R0, [R0,#1]
				LSL		R0, R0,	#0x1B
				LSR		R0, R0,	#0x1A
				LDR		R1, =tbl2
				LDRH	R0, [R1,R0]

loc_C0B16E2			

				AND		R0, R4
				B		loc_C0B16EA
		
loc_C0B16E6			

				LSR		R0, R0,	#1
				LSR		R4, R4,	#1

loc_C0B16EA				

				LSR		R1, R4,	#1
				BCC		loc_C0B16E6
    
    ENDIF
    
;- This comm code is probably not very good ! To check with algorithm    
    IF :DEF:WD2
    
    			LSL		R1, R0,	#0x10
				LSR		R4, R1,	#0x10
				LSL		R0, R0,	#0xA
				LSR		R0, R0,	#0x1A
				LSL		R1, R0,	#2
				LDR		R2, =regtbla
				LDR		R2, [R2,R1]
				MOV		R1, #0xE0
				AND		R1, R2		
				MOV		R0, #0x1F
				LSR		R1, R2,	#0xD
				AND		R1, R0
				LSL		R1, R1,	#0x18
				LSR		R1, R1,	#0x18
				AND		R2, R0
				LSL		R0, R2,	#0x18
				LSR		R2, R0,	#0x18
				MOV		R0, #1		
			    BL		CbusSendAck	
			    AND		R0, R4
				B		loc_C0B16EA

loc_C0B16E6			

				LSR		R0, R0,	#1
				LSR		R4, R4,	#1

loc_C0B16EA				

				LSR		R1, R4,	#1		
				BCC		loc_C0B16E6
							                
    ENDIF
                                              
                POP     {R1-R7,PC}
;- End of function read_uem_reg

	
	END