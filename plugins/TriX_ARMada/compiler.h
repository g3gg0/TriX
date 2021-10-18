
/*

  30.12.04    
     - fixed .asc/.uni problems with:  %'s, uppercase problem and wrong alignment
	 - fixed PUSH/POP  LR and PC interpreting
	 - added . (# already was) as a sign for DECIMAL values
	 - allowed DB, DW and DD to accept more than just one parameter
	 - added warnings for misaligned .org and possible alignment problems with DB

  01.01.05
     - added support for resizing the window
	 - .asc and .uni now no longer add a \000
	 - .asc and .uni allow extra bytes behind the string like "test" 0d 0a 00

  18.01.05
     - fixed nearly all memory leaks (one static allocated ptr for getparm still is)
	 - allow BLs to @marks and Bs to .imp functions
	 - BL $0x00001010 does a BL to that fixesd address
	 - minor crash situations fixed
	 - increased malloc sizes, in case there is a crash situation due to mem overwrite
	 - some internal fixes for a more nice coding scheme (lol!)

  18.02.05
     - now also uses shared memory (test tool + src available)
	 - theres already a 32768 bytes big buffer allocated for injection (.inj)
	   this allows creating a new file instead of only patching.
	   needed that for creating DCT-4 bootloaders
	 - added .ens for endianess selection (alpha test)
	 - emit error message in BL calc routine 

  24.02.05
	 - all ".xxx" also available as "#pragma xxx" for u µC freaks outta there

  05.04.05
	 - compatibility mode for lazy PUSH/POP LR/PC usage ;) (with warning)

  12.04.05
	 - Version v2.6 finally released - who cares?

*/



#define   TYPE1      0x0000
#define   TYPE1_LSL  0x0000
#define   TYPE1_LSR  0x0800
#define   TYPE1_ASR  0x1000
#define   TYPE1_I_O  0x06
#define   TYPE1_S_O  0x03

#define   TYPE2      0x1800
#define   TYPE2_ADDR 0x0000
#define   TYPE2_SUBR 0x0200
#define   TYPE2_ADDV 0x0400
#define   TYPE2_SUBV 0x0600
#define   TYPE2_O_O  0x06
#define   TYPE2_S_O  0x03

#define   TYPE3      0x2000
#define   TYPE3_MOV  0x0000
#define   TYPE3_CMP  0x0800
#define   TYPE3_ADD  0x1000
#define   TYPE3_SUB  0x1800
#define   TYPE3_R_O  0x08

#define   TYPE4      0x4000
#define   TYPE4_AND  0x0000
#define   TYPE4_EOR  0x0040
#define   TYPE4_LSL  0x0080
#define   TYPE4_LSR  0x00c0
#define   TYPE4_ASR  0x0100
#define   TYPE4_ADC  0x0140
#define   TYPE4_SBC  0x0180
#define   TYPE4_ROR  0x01c0
#define   TYPE4_TST  0x0200
#define   TYPE4_NEG  0x0240
#define   TYPE4_CMP  0x0280
#define   TYPE4_CMN  0x02c0
#define   TYPE4_ORR  0x0300
#define   TYPE4_MUL  0x0340
#define   TYPE4_BIC  0x0380
#define   TYPE4_MVN  0x03c0
#define   TYPE4_S_O  0x03

#define   TYPE5       0x4400
#define   TYPE5_S_O   0x03
#define   TYPE5_T_O   0x06
#define   TYPE5_ADDRR 0x00 << TYPE5_T_O
#define   TYPE5_ADDRH 0x01 << TYPE5_T_O 
#define   TYPE5_ADDHR 0x02 << TYPE5_T_O 
#define   TYPE5_ADDHH 0x03 << TYPE5_T_O 
#define   TYPE5_CMPRH 0x05 << TYPE5_T_O 
#define   TYPE5_CMPHR 0x06 << TYPE5_T_O 
#define   TYPE5_CMPHH 0x07 << TYPE5_T_O 
#define   TYPE5_MOVRR 0x08 << TYPE5_T_O 
#define   TYPE5_MOVRH 0x09 << TYPE5_T_O 
#define   TYPE5_MOVHR 0x0A << TYPE5_T_O 
#define   TYPE5_MOVHH 0x0B << TYPE5_T_O 
#define   TYPE5_BXR   0x0C << TYPE5_T_O 
#define   TYPE5_BXH   0x0D << TYPE5_T_O 

#define   TYPE6      0x4800
#define   TYPE6_D_O  0x08

#define   TYPE7      0x5000
#define   TYPE7_STR  0x00 << 10
#define   TYPE7_STRB 0x01 << 10
#define   TYPE7_LDR  0x02 << 10
#define   TYPE7_LDRB 0x03 << 10
#define   TYPE7_B_O  0x03
#define   TYPE7_O_O  0x06

#define   TYPE8      0x5200
#define   TYPE8_STRH 0x00 << 10
#define   TYPE8_LDSB 0x01 << 10
#define   TYPE8_LDRH 0x02 << 10
#define   TYPE8_LDSH 0x03 << 10
#define   TYPE8_B_O  0x03
#define   TYPE8_O_O  0x06

#define   TYPE9      0x6000
#define   TYPE9_STR  0x00 << 11
#define   TYPE9_STRB 0x02 << 11
#define   TYPE9_LDR  0x01 << 11
#define   TYPE9_LDRB 0x03 << 11
#define   TYPE9_B_O  0x03
#define   TYPE9_O_O  0x06

// LDRH/STRH
// LDRH Rd, Rb, val 
// Rd  => load word at Rb+val into this reg
// val => nr of words offset
#define   TYPE10      0x8000
#define   TYPE10_STRH 0x00 << 11
#define   TYPE10_LDRH 0x01 << 11
#define   TYPE10_B_O  0x03
#define   TYPE10_O_O  0x06

// STR/LDR 
// LDR R, SP, val
// R   => load dword at SP+val
// val => nr of dwords offset
#define   TYPE11      0x9000
#define   TYPE11_T_O  0x0B
#define   TYPE11_STR  0x00 << TYPE11_T_O
#define   TYPE11_LDR  0x01 << TYPE11_T_O
#define   TYPE11_D_O  0x08

// ADDSP, SUBPC
// ADDSP R, val
// R   => Reg to which PC/SP + val should be added
// val => nr of dwords to add
#define   TYPE12       0xA000
#define   TYPE12_T_O   0x0B
#define   TYPE12_R_O   0x08
#define   TYPE12_ADDPC 0x00 << TYPE12_T_O
#define   TYPE12_ADDSP 0x01 << TYPE12_T_O


// ADDSP, SUBSP
// ADDSP val
// val => nr of dwords
#define   TYPE13       0xB000
#define   TYPE13_T_O   0x07
#define   TYPE13_ADDSP 0x00 << TYPE13_T_O
#define   TYPE13_SUBSP 0x01 << TYPE13_T_O



#define   TYPE14       0xB400
#define   TYPE14_PUSH  0x0000 << 11
#define   TYPE14_POP   0x0001 << 11
#define   TYPE14_R0    0x0001
#define   TYPE14_R1    0x0002
#define   TYPE14_R2    0x0004
#define   TYPE14_R3    0x0008
#define   TYPE14_R4    0x0010
#define   TYPE14_R5    0x0020
#define   TYPE14_R6    0x0040
#define   TYPE14_R7    0x0080
#define   TYPE14_LR    0x0100
#define   TYPE14_PC    0x0100

// STMIA/LDMIA
// STMIA Rb, R1, R2, R3, Rn
#define   TYPE15       0xC000
#define   TYPE15_T_O   0x0B
#define   TYPE15_R_O   0x08
#define   TYPE15_LDMIA 0x01 << TYPE15_T_O
#define   TYPE15_STMIA 0x00 << TYPE15_T_O
#define   TYPE15_R0    0x0001
#define   TYPE15_R1    0x0002
#define   TYPE15_R2    0x0004
#define   TYPE15_R3    0x0008
#define   TYPE15_R4    0x0010
#define   TYPE15_R5    0x0020
#define   TYPE15_R6    0x0040
#define   TYPE15_R7    0x0080



#define   TYPE16       0xD000
#define   TYPE16_BEQ   0x0000
#define   TYPE16_BNE   0x0100
#define   TYPE16_BCS   0x0200
#define   TYPE16_BCC   0x0300
#define   TYPE16_BMI   0x0400
#define   TYPE16_BPL   0x0500
#define   TYPE16_BVS   0x0600
#define   TYPE16_BVC   0x0700
#define   TYPE16_BHI   0x0800
#define   TYPE16_BLS   0x0900
#define   TYPE16_BGE   0x0a00
#define   TYPE16_BLT   0x0b00
#define   TYPE16_BGT   0x0c00
#define   TYPE16_BLE   0x0d00

#define   TYPE17       0xDF00

#define   TYPE18       0xE000

#define   TYPE19     0xF000
#define   TYPE19_LO  0x0800
#define   TYPE19_HI  0x0000

#define   TYPE_NOP   0x46C0


#define ROL(val) {val = (val << 1) | (val >> 31 ); }
#define ROR(val) {val = (val >> 1) | (val << 31 ); }

#define TYPE32_COND_OFF  28
#define COND(c) (c->arm32_field_cond << TYPE32_COND_OFF)

#define TYPE32_OP_MASK    0x00000FFF
#define TYPE32_OP_IMM     0x00001000
#define TYPE32_OP_REG     0x00002000

#define TYPE32_BX         0x012FFF10
#define TYPE32_B          0x0A000000
#define TYPE32_B_L        0x01000000
#define TYPE32_B_O_MASK   0x00FFFFFF

#define TYPE32_DP         0x00000000
#define TYPE32_DP_I       0x02000000
#define TYPE32_DP_S       0x00100000
#define TYPE32_DP_I_OFF   25
#define TYPE32_DP_S_OFF   20
#define TYPE32_DP_OP_OFF  21
#define TYPE32_DP_RN_OFF  16
#define TYPE32_DP_RD_OFF  12

#define TYPE32_DP_OP_AND  0x00
#define TYPE32_DP_OP_EOR  0x01
#define TYPE32_DP_OP_SUB  0x02
#define TYPE32_DP_OP_RSB  0x03
#define TYPE32_DP_OP_ADD  0x04
#define TYPE32_DP_OP_ADC  0x05
#define TYPE32_DP_OP_SBC  0x06
#define TYPE32_DP_OP_RSC  0x07
#define TYPE32_DP_OP_TST  0x08
#define TYPE32_DP_OP_TEQ  0x09
#define TYPE32_DP_OP_CMP  0x0A
#define TYPE32_DP_OP_CMN  0x0B
#define TYPE32_DP_OP_ORR  0x0C
#define TYPE32_DP_OP_MOV  0x0D
#define TYPE32_DP_OP_BIC  0x0E
#define TYPE32_DP_OP_MVN  0x0F

#define TYPE32_MRS        0x010F0000
#define TYPE32_MRS_PS     0x00400000
#define TYPE32_MRS_RD_OFF 12

#define TYPE32_MSR        0x0128F000
#define TYPE32_MSR_PD     0x00400000
#define TYPE32_MSR_I      0x02000000
#define TYPE32_MSR_DIRECT 0x00010000
#define TYPE32_MSR_RM_OFF 0

#define TYPE32_MUL        0x00000090
#define TYPE32_MUL_A      0x00200000
#define TYPE32_MUL_S      0x00100000
#define TYPE32_MUL_A_OFF  21
#define TYPE32_MUL_S_OFF  20
#define TYPE32_MUL_RD_OFF 16
#define TYPE32_MUL_RM_OFF 0
#define TYPE32_MUL_RS_OFF 8
#define TYPE32_MUL_RN_OFF 12

#define TYPE32_MULL       0x00800090
#define TYPE32_MULL_U     0x00400000
#define TYPE32_MULL_A     0x00200000
#define TYPE32_MULL_S     0x00100000
#define TYPE32_MULL_U_OFF  22
#define TYPE32_MULL_A_OFF  21
#define TYPE32_MULL_S_OFF  20
#define TYPE32_MULL_RH_OFF 16
#define TYPE32_MULL_RL_OFF 12
#define TYPE32_MULL_RS_OFF 8
#define TYPE32_MULL_RM_OFF 0


#define TYPE32_LDR_STR    0x04000000
#define TYPE32_LDR_STR_I  0x02000000
#define TYPE32_LDR_STR_P  0x01000000
#define TYPE32_LDR_STR_U  0x00800000
#define TYPE32_LDR_STR_B  0x00400000
#define TYPE32_LDR_STR_W  0x00200000
#define TYPE32_LDR_STR_L  0x00100000
#define TYPE32_LDR_STR_RN_OFF  16
#define TYPE32_LDR_STR_RD_OFF  12


#define TYPE32_LDRH_STRH    0x00000090
#define TYPE32_LDRH_STRH_P  0x01000000
#define TYPE32_LDRH_STRH_U  0x00800000
#define TYPE32_LDRH_STRH_I  0x00400000
#define TYPE32_LDRH_STRH_W  0x00200000
#define TYPE32_LDRH_STRH_L  0x00100000
#define TYPE32_LDRH_STRH_S  0x00000040
#define TYPE32_LDRH_STRH_H  0x00000020
#define TYPE32_LDRH_STRH_RN_OFF  16
#define TYPE32_LDRH_STRH_RD_OFF  12
#define TYPE32_LDRH_STRH_OH_OFF   8
#define TYPE32_LDRH_STRH_OL_OFF   0

#define TYPE32_LDM_STM      0x08000000
#define TYPE32_LDM_STM_P    0x01000000
#define TYPE32_LDM_STM_U    0x00800000
#define TYPE32_LDM_STM_S    0x00400000
#define TYPE32_LDM_STM_W    0x00200000
#define TYPE32_LDM_STM_L    0x00100000
#define TYPE32_LDM_STM_RN_OFF 16


#define TYPE32_SWP          0x01000090
#define TYPE32_SWP_B        0x00400000
#define TYPE32_SWP_RN_OFF   16
#define TYPE32_SWP_RD_OFF   12
#define TYPE32_SWP_RM_OFF    0

#define TYPE32_SWI          0x0F000000

#define TYPE32_CDP          0x0E000000
#define TYPE32_CDP_CPO_OFF  20
#define TYPE32_CDP_CRN_OFF  16
#define TYPE32_CDP_CRD_OFF  12
#define TYPE32_CDP_CPN_OFF   8
#define TYPE32_CDP_CP_OFF    5
#define TYPE32_CDP_CRM_OFF   0

//
// registers:
//
//
//  CPSR PC LR SP R12 R11 R10 R9 R8 R7 R6 R5 R4 R3 R2 R1 R0
// |    |            |             |           |           |
//
//
//

#define   PARM_DYNAMIC_PARMS    -1
#define   PARM_DYNAMIC_SIZE     -2
#define   PARM_TXT        0x80

#define   PARM_UNK        0x000000
#define   PARM_VAL        0x010000

#define   PARM_REG_CPSR   0x0100000
#define   PARM_REG_CPSRF  0x0200000
#define   PARM_REG_SPSR   0x0400000
#define   PARM_REG_SPSRF  0x0800000
#define   PARM_CPID       0x1000000
#define   PARM_CREG       0x2000000

#define   PARM_REG        0x00FF
#define   PARM_REG_HI     0xFF00
#define   PARM_REG32      0xFFFF

#define   PARM_REG_R0     0x0001
#define   PARM_REG_R1     0x0002
#define   PARM_REG_R2     0x0004
#define   PARM_REG_R3     0x0008
#define   PARM_REG_R4     0x0010
#define   PARM_REG_R5     0x0020
#define   PARM_REG_R6     0x0040
#define   PARM_REG_R7     0x0080
#define   PARM_REG_R8     0x0100
#define   PARM_REG_R9     0x0200
#define   PARM_REG_R10    0x0400
#define   PARM_REG_R11    0x0800
#define   PARM_REG_R12    0x1000
#define   PARM_REG_R13    0x2000
#define   PARM_REG_R14    0x4000
#define   PARM_REG_R15    0x8000
#define   PARM_REG_SP     0x2000
#define   PARM_REG_LR     0x4000
#define   PARM_REG_PC     0x8000

#define   REG_R0     0x00
#define   REG_R1     0x01
#define   REG_R2     0x02
#define   REG_R3     0x03
#define   REG_R4     0x04
#define   REG_R5     0x05
#define   REG_R6     0x06
#define   REG_R7     0x07
#define   REG_R8     0x00
#define   REG_R9     0x01
#define   REG_R10    0x02
#define   REG_R11    0x03
#define   REG_R12    0x04
#define   REG_R13    0x05
#define   REG_R14    0x06
#define   REG_R15    0x07
#define   REG_UNK    0xFF

#define   REG32_R0     0x00
#define   REG32_R1     0x01
#define   REG32_R2     0x02
#define   REG32_R3     0x03
#define   REG32_R4     0x04
#define   REG32_R5     0x05
#define   REG32_R6     0x06
#define   REG32_R7     0x07
#define   REG32_R8     0x08
#define   REG32_R9     0x09
#define   REG32_R10    0x0a
#define   REG32_R11    0x0b
#define   REG32_R12    0x0c
#define   REG32_R13    0x0d
#define   REG32_R14    0x0e
#define   REG32_R15    0x0f
#define   REG32_UNK    0xFF

#define   ALIGN_2    if ( (c->bytes / 2) * 2 != c->bytes) return -E_WRONG_ALIGN;
#define   ALIGN_4    if ( (c->bytes / 4) * 4 != c->bytes) return -E_WRONG_ALIGN;


int set_path ( char* p );
char *getparm ( char *t, unsigned int p );
int getparms ( char *t );
unsigned int pt ( char *p );
int asm_inject_bl ( void * base, void * offset, void *src, void * jmp_addr );


