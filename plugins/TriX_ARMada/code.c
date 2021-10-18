
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "code.h"
#include "compiler.h"
#include "error.h"

unsigned char *base  = NULL;
unsigned long length = 0;

code_t code[256];
code_t global_c; // for global symbols


int function         = 0;
unsigned long highest_pos = 0;

void fill_code ( struct code_data *c );



struct s_comp comp[] = {
	{ MODE_ANY, 0, ".CODE",  comp_code,      0,  -2, PARM_VAL,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 0,  ".align boundary\r\nAligns to given boundary (e.g. 2, 4 or 8)" },

	{ MODE_ANY, 0, ".ALIGN",  comp_align,      0,  -2, PARM_VAL,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 0,  ".align boundary\r\nAligns to given boundary (e.g. 2, 4 or 8)" },
	{ MODE_ANY, 0, ".GSC",  comp_gsc,      0,  0, PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 0,  ".gsc\r\nSets the output data to g3n0lite script [CODE]" },
	{ MODE_ANY, 0, ".DMP",  comp_dmp,      0,  0, PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 0,  ".dmp\r\nDumps the located function adresses" },
	{ MODE_ANY, 0, ".LOC",  comp_loc,      0,  0, PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 0,  ".loc\r\nSearchs for predefined functions by patterns defined in pattern.txt" },
	{ MODE_ANY, 0, ".INJ",  comp_inj,      0,  1, PARM_VAL,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 0,  ".inj addr\r\nInjects the generated code at position <addr> if a file is opened" },
	{ MODE_ANY, 0, ".IDD",  comp_idd,      0,  2, PARM_UNK,    PARM_VAL,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 0,  ".idd name abs\r\nInserts a dword patch for g3n0lite with name. If abs = 0 no 0x00200000 is added" },
	{ MODE_ANY, 0, ".IMP",  comp_imp,      0,  2, PARM_UNK,    PARM_VAL,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 0,  ".imp func addr\r\nImports the address of a function" },
	{ MODE_ANY, 0, ".ORG",  comp_org,      0,  1, PARM_VAL,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 0,  ".org addr\r\nSpecifies the Origin of this code - used for calculating BL's" },
	{ MODE_ANY, 0, ".HEX",  comp_hex,      0,  0, PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 0,  ".hex\r\nSets the output data to Hexadecimal dump mode" },
	{ MODE_ANY, 0, ".TRIX", comp_trix,     0,  0, PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 0,  ".trix\nSets the output data to Trix dump mode" },
	{ MODE_ANY, 0, ".ENS",  comp_ens,      0,  1, PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 0,  ".ens\r\nSets the endianess BE or LE" },
	{ MODE_ANY, 0, ".SUB",  comp_nam,      0,  1, PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 0,  ".sub text\r\nSets the function name" },
	{ MODE_ANY, 0, ".DES",  comp_des,      0,  1, PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 0,  ".des text\r\nSets the function description" },
	{ MODE_ANY, 0, ".END",  comp_end,      0,  -2,PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 0,  ".end\r\nEnds the function" },
	{ MODE_ANY, 0, ".UNI",  comp_uni, PARM_TXT,  -2,PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 0,  ".uni \"string\" 0d 0a 00\r\nInserts an UNICODE string. You may use \" or ' around the string to include spaces.\r\n ex:  .asc \"test\" 0d 0a 00\r\nplease note, that no \\000 is added automatically!" },
	{ MODE_ANY, 0, ".ASC",  comp_asc, PARM_TXT,  -2,PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 0,  ".asc \"string\" 0d 0a 00\r\nInserts an ASCII string. You may use \" or ' around the string to include spaces.\r\n ex:  .asc \"test\" 0d 0a 00\r\nplease note, that no \\000 is added automatically!" },
	{ MODE_ANY, 0, ".SIZE",  comp_size,      0,  1, PARM_VAL,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 0,  ".size bytes\r\nSpecifies the maximal size for that code. You get a warning if the code gets too big." },


	// Branch with eXchange
	{ MODE_ARM32, 1, "BX",    comp32_bx,     4,  1, PARM_REG32, PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 1, "(ARM32) BX Rn\r\nThis instruction performs a branch by copying the contents of a general register, Rn, into the program counter, PC. The branch causes a pipeline flush and refill from theaddress specified by Rn. This instruction also permits the instruction set to be exchanged. When the instruction is executed, the value of Rn[0] determines whether the instruction stream will be decoded as ARM or THUMB instructions." },

	// Branch and Branch with Link
	{ MODE_ARM32, 1, "B",     comp32_b,      4,  1, PARM_UNK,   PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 1, "(ARM32) B @mark\r\n" },
	{ MODE_ARM32, 1, "BL",    comp32_bl,     4,  1, PARM_UNK,   PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 1, "(ARM32) BL @mark\r\n" },

	// 1=cond, 2=S_bit, 4=LDR_T_bit, 8=LDR_H_SH_SB_bits, 16=STR_H_D_bits, 64=LDM, 128=STM, 256=B_bit

	// Data Processing
	{ MODE_ARM32, 3, "AND",   comp32_and,    4,  -2, PARM_UNK,  PARM_UNK,  PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 1, "(ARM32) AND{cond}{S} Rd,Rn,<Op2>\r\n" },
	{ MODE_ARM32, 3, "EOR",   comp32_eor,    4,  -2, PARM_UNK,  PARM_UNK,  PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 1, "(ARM32) EOR{cond}{S} Rd,Rn,<Op2>\r\n" },
	{ MODE_ARM32, 3, "SUB",   comp32_sub,    4,  -2, PARM_UNK,  PARM_UNK,  PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 1, "(ARM32) SUB{cond}{S} Rd,Rn,<Op2>\r\n" },
	{ MODE_ARM32, 3, "RSB",   comp32_rsb,    4,  -2, PARM_UNK,  PARM_UNK,  PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 1, "(ARM32) RSB{cond}{S} Rd,Rn,<Op2>\r\n" },
	{ MODE_ARM32, 3, "ADD",   comp32_add,    4,  -2, PARM_UNK,  PARM_UNK,  PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 1, "(ARM32) ADD{cond}{S} Rd,Rn,<Op2>\r\n" },
	{ MODE_ARM32, 3, "ADC",   comp32_adc,    4,  -2, PARM_UNK,  PARM_UNK,  PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 1, "(ARM32) ADC{cond}{S} Rd,Rn,<Op2>\r\n" },
	{ MODE_ARM32, 3, "SBC",   comp32_sbc,    4,  -2, PARM_UNK,  PARM_UNK,  PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 1, "(ARM32) SBC{cond}{S} Rd,Rn,<Op2>\r\n" },
	{ MODE_ARM32, 3, "RSC",   comp32_rsc,    4,  -2, PARM_UNK,  PARM_UNK,  PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 1, "(ARM32) RSC{cond}{S} Rd,Rn,<Op2>\r\n" },
	{ MODE_ARM32, 1, "TST",   comp32_tst,    4,  -2, PARM_UNK,  PARM_UNK,  PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 1, "(ARM32) TST{cond} Rn,<Op2>\r\n" },
	{ MODE_ARM32, 1, "TEQ",   comp32_teq,    4,  -2, PARM_UNK,  PARM_UNK,  PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 1, "(ARM32) TEQ{cond} Rn,<Op2>\r\n" },
	{ MODE_ARM32, 1, "CMP",   comp32_cmp,    4,  -2, PARM_UNK,  PARM_UNK,  PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 1, "(ARM32) CMP{cond} Rn,<Op2>\r\n" },
	{ MODE_ARM32, 1, "CMN",   comp32_cmn,    4,  -2, PARM_UNK,  PARM_UNK,  PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 1, "(ARM32) CMN{cond} Rn,<Op2>\r\n" },
	{ MODE_ARM32, 3, "ORR",   comp32_orr,    4,  -2, PARM_UNK,  PARM_UNK,  PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 1, "(ARM32) CMN{cond}{S} Rd,Rn,<Op2>\r\n" },
	{ MODE_ARM32, 3, "MOV",   comp32_mov,    4,  -2, PARM_UNK,  PARM_UNK,  PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 1, "(ARM32) MOV{cond}{S} Rd,<Op2\r\n" },
	{ MODE_ARM32, 3, "BIC",   comp32_bic,    4,  -2, PARM_UNK,  PARM_UNK,  PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 1, "(ARM32) BIC{cond}{S} Rd,Rn,<Op2>\r\n" },
	{ MODE_ARM32, 3, "MVN",   comp32_mvn,    4,  -2, PARM_UNK,  PARM_UNK,  PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 1, "(ARM32) MVN{cond}{S} Rd,<Op2\r\n" },

	// PSR transfer
	{ MODE_ARM32, 1, "MRS",   comp32_mrs,    4,  2, PARM_REG32, PARM_REG_CPSR|PARM_REG_SPSR,  PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 1, "(ARM32) MRS{cond} Rd,<psr>\r\n" },
	{ MODE_ARM32, 1, "MSR",   comp32_msr,    4,  2, PARM_REG_CPSR|PARM_REG_CPSRF|PARM_REG_SPSR|PARM_REG_SPSRF, PARM_REG32|PARM_VAL,  PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 1, "(ARM32) MSR{cond} <psrf>,<#expression>\r\n" },

	// MUL/MLA
	{ MODE_ARM32, 3, "MUL",   comp32_mul,    4,  3, PARM_REG32,    PARM_REG32,    PARM_REG32,    PARM_UNK,    PARM_UNK,    PARM_UNK, 1, "(ARM32) MUL{cond}{S} Rd,Rm,Rs\r\n" },
	{ MODE_ARM32, 3, "MLA",   comp32_mla,    4,  4, PARM_REG32,    PARM_REG32,    PARM_REG32,    PARM_REG32,    PARM_UNK,    PARM_UNK, 1, "(ARM32) MLA{cond}{S} Rd,Rm,Rs,Rn\r\n" },

	// MULL/MLAL
	{ MODE_ARM32, 3, "UMULL",   comp32_umull,    4,  4, PARM_REG32,    PARM_REG32,    PARM_REG32,    PARM_REG32,    PARM_UNK,    PARM_UNK, 1, "(ARM32) UMULL{cond}{S} RdLo,RdHi,Rm,Rs\r\n" },
	{ MODE_ARM32, 3, "UMLAL",   comp32_umlal,    4,  4, PARM_REG32,    PARM_REG32,    PARM_REG32,    PARM_REG32,    PARM_UNK,    PARM_UNK, 1, "(ARM32) UMLAL{cond}{S} RdLo,RdHi,Rm,Rs\r\n" },
	{ MODE_ARM32, 3, "SMULL",   comp32_smull,    4,  4, PARM_REG32,    PARM_REG32,    PARM_REG32,    PARM_REG32,    PARM_UNK,    PARM_UNK, 1, "(ARM32) SMULL{cond}{S} RdLo,RdHi,Rm,Rs\r\n" },
	{ MODE_ARM32, 3, "SMLAL",   comp32_smlal,    4,  4, PARM_REG32,    PARM_REG32,    PARM_REG32,    PARM_REG32,    PARM_UNK,    PARM_UNK, 1, "(ARM32) SMLAL{cond}{S} RdLo,RdHi,Rm,Rs\r\n" },

	// LDR/STR
	{ MODE_ARM32, 261, "LDR",   comp32_ldr,    4,  -2, PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 1, "(ARM32) <LDR|STR>{cond}{B}{T} Rd,<Address>\r\n" },
	{ MODE_ARM32, 261, "STR",   comp32_str,    4,  -2, PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 1, "(ARM32) <LDR|STR>{cond}{B}{T} Rd,<Address>\r\n" },

	// LDRH/STRH
	{ MODE_ARM32, 9, "LDR",   comp32_ldrh,   4,  -2, PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 1, "(ARM32) <LDR|STR>{cond}<H|SH|SB> Rd,<address>\r\n" },
	{ MODE_ARM32, 17, "STR",   comp32_strh,   4,  -2, PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 1, "(ARM32) <LDR|STR>{cond}<H|SH|SB> Rd,<address>\r\n" },
	{ MODE_ARM32, 33, "LDR",   comp32_strh,   4,  -2, PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 1, "(ARM32) <LDR|STR>{cond}<H|SH|SB> Rd,<address>\r\n" },

	// LDM/STM
	{ MODE_ARM32, 65,  "LDM",   comp32_ldm,   4,  -2, PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 1, "(ARM32) <LDM|STM>{cond}<FD|ED|FA|EA|IA|IB|DA|DB> Rn{!},<Rlist>{^}\r\n" },
	{ MODE_ARM32, 129, "STM",   comp32_stm,   4,  -2, PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 1, "(ARM32) <LDM|STM>{cond}<FD|ED|FA|EA|IA|IB|DA|DB> Rn{!},<Rlist>{^}\r\n" },

	// SWP
	{ MODE_ARM32, 257, "SWP",   comp32_swp,   4,  3, PARM_REG32,    PARM_REG32,    PARM_REG32,    PARM_UNK,    PARM_UNK,    PARM_UNK, 1, "(ARM32) <SWP>{cond}{B} Rd,Rm,[Rn]\r\n" },

	// SWI
	{ MODE_ARM32, 1, "SWI",   comp32_swi,   4,  0, PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 1, "(ARM32) SWI{cond} <expression>\r\n" },

	// CDP
	{ MODE_ARM32, 1, "CDP",   comp32_cdp,   4,  5, PARM_CPID,    PARM_VAL,    PARM_CREG,    PARM_CREG,    PARM_CREG,    PARM_UNK, 1, "(ARM32) CDP{cond} p#,<expression1>,cd,cn,cm{,<expression2>}\r\n" },
	{ MODE_ARM32, 1, "CDP",   comp32_cdp,   4,  6, PARM_CPID,    PARM_VAL,    PARM_CREG,    PARM_CREG,    PARM_CREG,    PARM_VAL, 1, "(ARM32) \r\n" },

	// Coprocessor Data Transfers (LDC, STC)
	// Coprocessor Register Transfers (MRC, MCR)
	// 


	{ MODE_THUMB, 0, "STMIA", comp_stmia,    2, -1, PARM_REG,    PARM_REG,    PARM_VAL,    PARM_UNK,    PARM_UNK,    PARM_UNK, 15, "STMIA Rb, Rlist,...\r\nStore the Registers specified in Rlist, starting at the base address in Rb. \r\nWrite back the new address"  },
	{ MODE_THUMB, 0, "LDMIA", comp_ldmia,    2, -1, PARM_REG,    PARM_REG,    PARM_VAL,    PARM_UNK,    PARM_UNK,    PARM_UNK, 15, "LDMIA Rb, Rlist,...\r\nLoad the Registers specified in Rlist, starting at the base address in Rb. \r\nWrite back the new address"  },
	{ MODE_THUMB, 0, "STRB",  comp_strb,     2,  3, PARM_REG,    PARM_REG,    PARM_REG,    PARM_UNK,    PARM_UNK,    PARM_UNK, 7,  "STRB Rd, Rb, Ro\r\nStore byte in Rd at address Rb at offset Ro" },
	{ MODE_THUMB, 0, "STRB",  comp_strbr,    2,  3, PARM_REG,    PARM_REG,    PARM_VAL,    PARM_UNK,    PARM_UNK,    PARM_UNK, 9,  "STRB Rd, Rb, val\r\nStore byte in Rd at address Rb at offset val" },
	{ MODE_THUMB, 0, "LDRB",  comp_ldrb,     2,  3, PARM_REG,    PARM_REG,    PARM_REG,    PARM_UNK,    PARM_UNK,    PARM_UNK, 7,  "LDRB Rd, Rb, Ro\r\nLoad byte in Rd at address Rb at offset Ro" },
	{ MODE_THUMB, 0, "LDRB",  comp_ldrbr,    2,  3, PARM_REG,    PARM_REG,    PARM_VAL,    PARM_UNK,    PARM_UNK,    PARM_UNK, 9,  "LDRB Rd, Rb, val\r\nLoad byte in Rd at address Rb at offset val" },
	{ MODE_THUMB, 0, "STRH",  comp_strh,     2,  3, PARM_REG,    PARM_REG,    PARM_REG,    PARM_UNK,    PARM_UNK,    PARM_UNK, 8,  "STRH Rd, Rb, Ro\r\nStores the halfword Rd at Rb plus offset Ro" },
	{ MODE_THUMB, 0, "STRH",  comp_strhr,    2,  3, PARM_REG,    PARM_REG,    PARM_VAL,    PARM_UNK,    PARM_UNK,    PARM_UNK, 10, "STRH Rd, Rb, val\r\nStores the halfword Rd at Rb plus offset val"},
	{ MODE_THUMB, 0, "LDRH",  comp_ldrh,     2,  3, PARM_REG,    PARM_REG,    PARM_REG,    PARM_UNK,    PARM_UNK,    PARM_UNK, 8,  "LDRH Rd, Rb, Ro\r\nLoads the halfword Rd at Rb plus offset Ro" },
	{ MODE_THUMB, 0, "LDSB",  comp_ldsb,     2,  3, PARM_REG,    PARM_REG,    PARM_REG,    PARM_UNK,    PARM_UNK,    PARM_UNK, 8,  "LDSB Rd, Rb, Ro\r\nLoads a sign-extended byte Rd at Rb plus offset Ro" },
	{ MODE_THUMB, 0, "LDSH",  comp_ldsh,     2,  3, PARM_REG,    PARM_REG,    PARM_REG,    PARM_UNK,    PARM_UNK,    PARM_UNK, 8,  "LDSH Rd, Rb, Ro\r\nLoads a sign-extended halfword Rd at Rb plus offset Ro" },
	{ MODE_THUMB, 0, "LDRH",  comp_ldrhr,    2,  3, PARM_REG,    PARM_REG,    PARM_VAL,    PARM_UNK,    PARM_UNK,    PARM_UNK, 10, "LDRH Rd, Rb, val\r\nLoads the halfword Rd at Rb plus offset val"},
	{ MODE_THUMB, 0, "MOV",   comp_movpcrel, 2,  2, PARM_REG,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 12, "MOV Rd, @mark\r\nPlace the address of the given mark in Rd\r\nMust be DWORD aligned. (same as ADR)" },
	{ MODE_THUMB, 0, "ADR",   comp_movpcrel, 2,  2, PARM_REG,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 12, "ADR Rd, @mark\r\nPlace the address of the given mark in Rd\r\nMust be DWORD aligned. (same as MOV)" },
	{ MODE_THUMB, 0, "MOV",   comp_mov,      2,  2, PARM_REG,    PARM_VAL,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 3,  "MOV Rd, val\r\nChanges Rd to val"  },
	{ MODE_THUMB, 0, "ADD",   comp_addpcr,   2,  3, PARM_REG,    PARM_REG_PC, PARM_VAL,    PARM_UNK,    PARM_UNK,    PARM_UNK, 12, "ADDPC Rd, val\r\nAdd PC and val and place the result in Rd" },
	{ MODE_THUMB, 0, "ADD",   comp_addspr,   2,  3, PARM_REG,    PARM_REG_SP, PARM_VAL,    PARM_UNK,    PARM_UNK,    PARM_UNK, 12, "ADDSP Rd, val\r\nAdd SP and val and place the result in Rd" },
	{ MODE_THUMB, 0, "ADD",   comp_addsp,    2,  2, PARM_REG_SP, PARM_VAL,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 13, "ADDSP val\r\nAdd val to SP" },
	{ MODE_THUMB, 0, "SUB",   comp_subsp,    2,  2, PARM_REG_SP, PARM_VAL,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 13, "SUBSP val\r\nSubtract val from SP" },
	{ MODE_THUMB, 0, "SUB",   comp_subr,     2,  3, PARM_REG,    PARM_REG,    PARM_REG,    PARM_UNK,    PARM_UNK,    PARM_UNK, 2,  "SUB Rd, Rs, Rn\r\nPlaces the result of Rs minus Rn into Rd" },
	{ MODE_THUMB, 0, "SUB",   comp_subv,     2,  3, PARM_REG,    PARM_REG,    PARM_VAL,    PARM_UNK,    PARM_UNK,    PARM_UNK, 2,  "SUB Rd, Rs, val\r\nPlaces the result of Rs minus val into Rd" },
	{ MODE_THUMB, 0, "ADD",   comp_addr,     2,  3, PARM_REG,    PARM_REG,    PARM_REG,    PARM_UNK,    PARM_UNK,    PARM_UNK, 2,  "ADD Rd, Rs, Rn\r\nPlaces the result of Rs plus Rn into Rd" },
	{ MODE_THUMB, 0, "ADD",   comp_addv,     2,  3, PARM_REG,    PARM_REG,    PARM_VAL,    PARM_UNK,    PARM_UNK,    PARM_UNK, 2,  "ADD Rd, Rs, val\r\nPlaces the result of Rs plus val into Rd" },
	{ MODE_THUMB, 0, "ADD",   comp_addrh,    2,  2, PARM_REG,    PARM_REG_HI, PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 5,  "ADD Rh, Rl\r\nAdds a High Register to the given Low Register" },
	{ MODE_THUMB, 0, "ADD",   comp_addhr,    2,  2, PARM_REG_HI, PARM_REG,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 5,  "ADD Rl, Rh\r\nAdds a Low Register to the given Low Register" },
	{ MODE_THUMB, 0, "ADD",   comp_addhh,    2,  2, PARM_REG_HI, PARM_REG_HI, PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 5,  "ADD Rh, Rh\r\nAdds a High Register to the given High Register" },
	{ MODE_THUMB, 0, "ADD",   comp_addrr,    2,  2, PARM_REG,    PARM_REG,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 5,  "ADD Rl, Rl\r\nAdds a Low Register to the given Low Register" },
	{ MODE_THUMB, 0, "CMP",   comp_cmprh,    2,  2, PARM_REG,    PARM_REG_HI, PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 5,  "CMP Rl, Rh\r\nCompares a Low Register with an High Register" },
	{ MODE_THUMB, 0, "CMP",   comp_cmphr,    2,  2, PARM_REG_HI, PARM_REG,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 5,  "CMP Rh, Rl\r\nCompares a High Register with an Low Register" },
	{ MODE_THUMB, 0, "CMP",   comp_cmphh,    2,  2, PARM_REG_HI, PARM_REG_HI, PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 5,  "CMP Rh, Rh\r\nCompares a High Register with an High Register" },
	{ MODE_THUMB, 0, "MOV",   comp_movrh,    2,  2, PARM_REG,    PARM_REG_HI, PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 5,  "MOV Rh, Rl\r\nMoves the value in a High Register into the Low Register" },
	{ MODE_THUMB, 0, "MOV",   comp_movhr,    2,  2, PARM_REG_HI, PARM_REG,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 5,  "MOV Rl, Rh\r\nMoves the value in a Low Register into the High Register" },
	{ MODE_THUMB, 0, "MOV",   comp_movhh,    2,  2, PARM_REG_HI, PARM_REG_HI, PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 5,  "MOV Rh, Rh\r\nMoves the value in a High Register into the High Register" },
	{ MODE_THUMB, 0, "MOV",   comp_movrr,    2,  2, PARM_REG,    PARM_REG,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 5,  "MOV Rl, Rl\r\nMoves the value in a Low Register into the Low Register" },	
	{ MODE_THUMB, 0, "BX",    comp_bxr,      2,  1, PARM_REG,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 5,  "BX  Rl\r\nSets the new exection pointer to the value in the Low register" },
	{ MODE_THUMB, 0, "BX",    comp_bxh,      2,  1, PARM_REG_HI, PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 5,  "BX  Rh\r\nSets the new exection pointer to the value in the High register" },
	{ MODE_THUMB, 0, "SUB",   comp_sub,      2,  2, PARM_REG,    PARM_VAL,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 3,  "SUB Rd, val\r\nSubtracts val from the given register" },
	{ MODE_THUMB, 0, "ADD",   comp_add,      2,  2, PARM_REG,    PARM_VAL,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 3,  "ADD Rd, val\r\nAdds val to the given register" },
	{ MODE_THUMB, 0, "CMP",   comp_cmp,      2,  2, PARM_REG,    PARM_VAL,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 3,  "CMP Rd, val\r\nCompares the register with val" },
	{ MODE_THUMB, 0, "SWI",   comp_swi,      2,  1, PARM_VAL,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 17, "SWI val\r\nInitiates a software interrupt" },
	{ MODE_THUMB, 0, "LSL",   comp_lsl,      2,  3, PARM_REG,    PARM_REG,    PARM_VAL,    PARM_UNK,    PARM_UNK,    PARM_UNK, 1,  "LSL Rd, Rs, val\r\nShift Rs Logical Left val times and place in Rd" },
	{ MODE_THUMB, 0, "LSR",   comp_lsr,      2,  3, PARM_REG,    PARM_REG,    PARM_VAL,    PARM_UNK,    PARM_UNK,    PARM_UNK, 1,  "LSR Rd, Rs, val\r\nShift Rs Logical Right val times and place in Rd" },
	{ MODE_THUMB, 0, "ASR",   comp_asr,      2,  3, PARM_REG,    PARM_REG,    PARM_VAL,    PARM_UNK,    PARM_UNK,    PARM_UNK, 1,  "ASR Rd, Rs, val\r\nShift Rs Arithmetical Right val times and place in Rd" },
	{ MODE_THUMB, 0, "NOP",   comp_nop,      2,  0, PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 0,  "NOP\r\nIt's just an MOV R8, R8 - so it does nothing but consuming CPU time" },
	{ MODE_THUMB, 0, "AND",   comp_and,      2,  2, PARM_REG,    PARM_REG,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 4,  "AND Rd, Rs\r\nPlace the result of Rd AND Rs into Rd" },
	{ MODE_THUMB, 0, "EOR",   comp_eor,      2,  2, PARM_REG,    PARM_REG,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 4,  "EOR Rd, Rs\r\nPlace the result of Rd EOR Rs into Rd" },
	{ MODE_THUMB, 0, "LSL",   comp_lslr,     2,  2, PARM_REG,    PARM_REG,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 4,  "LSL Rd, Rs\r\nShift Rd Logical Left Rs times" },
	{ MODE_THUMB, 0, "LSR",   comp_lsrr,     2,  2, PARM_REG,    PARM_REG,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 4,  "LSR Rd, Rs\r\nShift Rd Logical Right Rs times" },
	{ MODE_THUMB, 0, "ASR",   comp_asrr,     2,  2, PARM_REG,    PARM_REG,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 4,  "ASR Rd, Rs\r\nShift Rd Arithmetical Right Rs times" },
	{ MODE_THUMB, 0, "ADC",   comp_adc,      2,  2, PARM_REG,    PARM_REG,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 4,  "ADC Rd, Rs\r\nAdd Rs and C bit to Rd" },
	{ MODE_THUMB, 0, "SBC",   comp_sbc,      2,  2, PARM_REG,    PARM_REG,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 4,  "SBC Rd, Rs\r\nSubtract Rs and C bit from Rd" },
	{ MODE_THUMB, 0, "ROR",   comp_ror,      2,  2, PARM_REG,    PARM_REG,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 4,  "ROR Rd, Rs\r\nRotate Rd Right Rs times" },
	{ MODE_THUMB, 0, "TST",   comp_tst,      2,  2, PARM_REG,    PARM_REG,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 4,  "TST Rd, Rs\r\nSet the Flags depending on Rd AND Rs" },
	{ MODE_THUMB, 0, "NEG",   comp_neg,      2,  2, PARM_REG,    PARM_REG,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 4,  "NEG Rd, Rs\r\nNegate Rs and place in Rd" },
	{ MODE_THUMB, 0, "CMP",   comp_cmpr,     2,  2, PARM_REG,    PARM_REG,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 4,  "CMP Rd, Rs\r\nSet Flags depending on Rd - Rs" },
	{ MODE_THUMB, 0, "CMN",   comp_cmn,      2,  2, PARM_REG,    PARM_REG,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 4,  "CMN Rd, Rs\r\nSet Flags depending on Rd + Rs" },
	{ MODE_THUMB, 0, "ORR",   comp_orr,      2,  2, PARM_REG,    PARM_REG,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 4,  "ORR Rd, Rs\r\nOr Rd with value in Rs" },
	{ MODE_THUMB, 0, "MUL",   comp_mul,      2,  2, PARM_REG,    PARM_REG,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 4,  "MUL Rd, Rs\r\nMultiply Rd with Rs" },
	{ MODE_THUMB, 0, "BIC",   comp_bic,      2,  2, PARM_REG,    PARM_REG,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 4,  "BIC Rd, Rs\r\nClear in Rd the bits set in Rs" },
	{ MODE_THUMB, 0, "MVN",   comp_mvn,      2,  2, PARM_REG,    PARM_REG,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 4,  "MVN Rd, Rs\r\nMove the negated value from Rs to Rd" },
    { MODE_THUMB, 0, "LDR",   comp_ldrd,     2,  2, PARM_REG,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 6,  "LDR Rd, @mark\r\nLoad DWord saved at the address mark specifies" },
	{ MODE_THUMB, 0, "LDR",   comp_ldr,      2,  3, PARM_REG,    PARM_REG,    PARM_REG,    PARM_UNK,    PARM_UNK,    PARM_UNK, 7,  "LDR Rd, Rb, Ro\r\nLoad DWord in Rd at address Rb at offset Ro" },
	{ MODE_THUMB, 0, "LDR",   comp_ldrr,     2,  3, PARM_REG,    PARM_REG,    PARM_VAL,    PARM_UNK,    PARM_UNK,    PARM_UNK, 9,  "LDR Rd, Rb, val\r\nLoad DWord in Rd at address Rb at offset val" },
	{ MODE_THUMB, 0, "STR",   comp_str,      2,  3, PARM_REG,    PARM_REG,    PARM_REG,    PARM_UNK,    PARM_UNK,    PARM_UNK, 7,  "STR Rd, Rb, Ro\r\nStore DWord in Rd at address Rb at offset Ro" },
	{ MODE_THUMB, 0, "STR",   comp_strr,     2,  3, PARM_REG,    PARM_REG,    PARM_VAL,    PARM_UNK,    PARM_UNK,    PARM_UNK, 9,  "STR Rd, Rb, val\r\nStore DWord in Rd at address Rb at offset val" },
	{ MODE_THUMB, 0, "LDR",   comp_ldrsp,    2,  3, PARM_REG,    PARM_REG_HI, PARM_VAL,    PARM_UNK,    PARM_UNK,    PARM_UNK, 11, "LDRSP Rd, SP, val\r\nLoad the Register Rd from SP + val. val must be DWord aligned" },
	{ MODE_THUMB, 0, "STR",   comp_strsp,    2,  3, PARM_REG,    PARM_REG_HI, PARM_VAL,    PARM_UNK,    PARM_UNK,    PARM_UNK, 11, "STRSP Rd, SP, val\r\nStore the Register Rd to SP + val. val must be DWord aligned" },
	{ MODE_THUMB, 0, "PUSH",  comp_push,     2, -1, PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 14, "PUSH Rx, Rx, LR...\r\nPushes the registers on Stack" },
	{ MODE_THUMB, 0, "POP",   comp_pop,      2, -1, PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 15, "POP Rx, Rx, LR...\r\nPops the registers from Stack" },
	{ MODE_THUMB, 0, "BEQ",   comp_beq,      2,  1, PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 16, "BEQ @mark\r\nJumps to mark when the result was equal" }, 
	{ MODE_THUMB, 0, "BNE",   comp_bne,      2,  1, PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 16, "BNE @mark\r\nJumps to mark when the result was not equal" },
	{ MODE_THUMB, 0, "BCS",   comp_bcs,      2,  1, PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 16, "BCS @mark\r\nJumps to mark when Carry is set" },
	{ MODE_THUMB, 0, "BCC",   comp_bcc,      2,  1, PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 16, "BCC @mark\r\nJumps to mark when Carry is cleared" },
	{ MODE_THUMB, 0, "BMI",   comp_bmi,      2,  1, PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 16, "BMI @mark\r\nJumps to mark when Negative flag is set" },
	{ MODE_THUMB, 0, "BPL",   comp_bpl,      2,  1, PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 16, "BPL @mark\r\nJumps to mark when Negative flag is cleared" },
	{ MODE_THUMB, 0, "BVS",   comp_bvs,      2,  1, PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 16, "BVS @mark\r\nJumps to mark when Overflow flag is set" },
	{ MODE_THUMB, 0, "BVC",   comp_bvc,      2,  1, PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 16, "BVC @mark\r\nJumps to mark when Overflow flag is cleared" },
	{ MODE_THUMB, 0, "BHI",   comp_bhi,      2,  1, PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 16, "BHI @mark\r\nJumps to mark when Carry flag is set and Zero is cleared" },
	{ MODE_THUMB, 0, "BLS",   comp_bls,      2,  1, PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 16, "BLS @mark\r\nJumps to mark when the result was less or same" },
	{ MODE_THUMB, 0, "BGE",   comp_bge,      2,  1, PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 16, "BGE @mark\r\nJumps to mark when the result was greater or equal" },
	{ MODE_THUMB, 0, "BLT",   comp_blt,      2,  1, PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 16, "BLT @mark\r\nJumps to mark when the result was less then" },
	{ MODE_THUMB, 0, "BGT",   comp_bgt,      2,  1, PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 16, "BGT @mark\r\nJumps to mark when the result was greater than" },
	{ MODE_THUMB, 0, "BLE",   comp_ble,      2,  1, PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 16, "BLE @mark\r\nJumps to mark when the result was less or equal" },
	{ MODE_THUMB, 0, "BL" ,   comp_bl,       4,  1, PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 19, "BL @routine\r\nDoes a long unconditional branch to the given routine after placing PC in LR\r\n\r\nBL @mark\r\n\tcalls the imported (.imp) or searched (.loc) routine or\r\n\tleaves a unresolved symbol for g3n0lite\r\nBL $addr\r\n\tcalls the defined fixed addresss\r\nBL _mark\r\n\tcalls the imported (.imp) routine, error when not defined" },
	{ MODE_THUMB, 0, "B"  ,   comp_b,        2,  1, PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 18, "B @mark\r\nJumps to the given mark\r\n\r\nB  @mark\r\n\tjumps to the defined label if one defined or to the imported (.imp)\r\n\tor searched (.loc) function" },
	{ MODE_THUMB, 0, "DB" ,   comp_db,       0,  -2, PARM_VAL,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 0,  "DB 0xde\r\nInserts the Byte followed" },
	{ MODE_THUMB, 0, "DW" ,   comp_dw,       0,  -2, PARM_VAL,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 0,  "DW 0xdead\r\nInserts the Word followed" },
	{ MODE_THUMB, 0, "DD" ,   comp_dd,       0,  -2, PARM_VAL,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK,    PARM_UNK, 0,  "DD 0xdeadbeef\r\nInserts the DWord followed" },
	{ NULL  ,   NULL }
};

t_arm32_cond arm32_conditions[] = {
	{ "EQ", 0x00 },
	{ "NE", 0x01 },
	{ "CS", 0x02 },
	{ "CC", 0x03 },
	{ "MI", 0x04 },
	{ "PL", 0x05 },
	{ "VS", 0x06 },
	{ "VC", 0x07 },
	{ "HI", 0x08 },
	{ "LS", 0x09 },
	{ "GE", 0x0a },
	{ "LT", 0x0b },
	{ "GT", 0x0c },
	{ "LE", 0x0d },
	{ "AL", 0x0e },
	{ NULL, 0 }
};


unsigned long get_highest_pos ( void )
{
	return highest_pos;
}


int freeCompiler ( void )
{
	int i = 0;

	for ( i=0; i<256; i++ )
		free_code ( &code[i] );
	return 0;
}

int initCompiler ( void )
{
	int i = 0;

	for ( i=0; i<256; i++ )
	{
		fill_code ( &code[i] );
	}
    return 0;
}

struct s_comp *GetOpcodes ( void )
{
	return (struct s_comp *)&comp;
}


unsigned int arm32_get_cond_field ( unsigned char *cond )
{
	int pos = 0;

	while ( arm32_conditions[pos].string )
	{
		if ( !strncmp ( cond, arm32_conditions[pos].string, strlen (arm32_conditions[pos].string) ) )
		{
			memmove ( cond, cond+strlen(arm32_conditions[pos].string), strlen ( cond ) - strlen (arm32_conditions[pos].string ) + 1 );
			return arm32_conditions[pos].code;
		}
		pos++;
	}

	return 0x0E;
}

unsigned int arm32_get_flag ( unsigned char *cond, unsigned char *flag )
{
	int pos = 0;

	if ( !strncmp ( cond, flag, strlen(flag) ))
	{
		memmove ( cond, cond+strlen(flag), strlen ( cond ) - strlen(flag) + 1 );
		return 1;
	}

	return 0;
}

unsigned int arm32_get_ldm_flags ( unsigned char *cond )
{
	unsigned int flags = 0;

	if ( arm32_get_flag ( cond, "ED" ) )
		return 3;
	if ( arm32_get_flag ( cond, "IB" ) )
		return 3;
	if ( arm32_get_flag ( cond, "FD" ) )
		return 1;
	if ( arm32_get_flag ( cond, "IA" ) )
		return 1;
	if ( arm32_get_flag ( cond, "EA" ) )
		return 2;
	if ( arm32_get_flag ( cond, "DB" ) )
		return 2;
	if ( arm32_get_flag ( cond, "FA" ) )
		return 0;
	if ( arm32_get_flag ( cond, "DA" ) )
		return 0;
}

unsigned int arm32_get_stm_flags ( unsigned char *cond )
{
	unsigned int flags = 0;

	if ( arm32_get_flag ( cond, "FA" ) )
		return 3;
	if ( arm32_get_flag ( cond, "IB" ) )
		return 3;
	if ( arm32_get_flag ( cond, "EA" ) )
		return 1;
	if ( arm32_get_flag ( cond, "IA" ) )
		return 1;
	if ( arm32_get_flag ( cond, "FD" ) )
		return 2;
	if ( arm32_get_flag ( cond, "DB" ) )
		return 2;
	if ( arm32_get_flag ( cond, "ED" ) )
		return 0;
	if ( arm32_get_flag ( cond, "DA" ) )
		return 0;
}


int compile_line ( struct code_data *c, char *t, int cmd )
{
	unsigned char *tmp_t = NULL;
	unsigned char *parmstring = NULL;
	unsigned char *parms = NULL;
	int pos = 0;
	int ret = 0;
	int found_opcode = 0;

	if (  !strncmp ( t, "/*", 2 ) )
	{
		c->comments++;
		return 0;
	}
	if (  !strncmp ( t, "*/", 2 ) )
	{
		c->comments--;
		return 0;
	}
	if ( c->comments )
		return 0;

	if (  !strcmp ( t, "\r\n" ) || 
          !strcmp ( t, "\n" ) || 
          !strncmp ( t, "#", 1 ) || 
          !strncmp ( t, "//", 2 ) || 
          !strncmp ( t, "@", 1 ) || 
          !strcmp ( t, "" )  
       )
		return 0;

	tmp_t = strupr(strdup ( t ));

	while ( tmp_t[pos] )
	{
		if ( tmp_t[pos] == '\t' )
			tmp_t[pos] = ' ';
		pos++;
	}
	pos = 0;

	while ( comp[pos].opcode )
	{
		int match = 0;

		if ( comp[pos].modifiers )
			match = !strncmp ( comp[pos].opcode, tmp_t, strlen ( comp[pos].opcode ) );
		else
		{
			match = !strncmp ( comp[pos].opcode, tmp_t, strlen ( comp[pos].opcode ) );
			if ( tmp_t[strlen ( comp[pos].opcode )] != ' ' && tmp_t[strlen ( comp[pos].opcode )] != '\000' )
				match = 0;
		}

		if ( match && (comp[pos].mode & c->arch_mode) )
		{
			parms = tmp_t + strlen ( comp[pos].opcode );
			if ( comp[pos].modifiers )
				while ( *parms != ' ' && *parms != '\000'  )
					parms++;

			while ( *parms == ' ' || *parms == '\t'  )
				parms++;

			if ( comp[pos].params <= 0 )
				found_opcode = 1;
			else if ( getparms ( parms ) == comp[pos].params  )
			{
				if ( pt_true ( getparm (parms,1), comp[pos].p1 ) &&
					 pt_true ( getparm (parms,2), comp[pos].p2 ) &&
					 pt_true ( getparm (parms,3), comp[pos].p3 ) &&
					 pt_true ( getparm (parms,4), comp[pos].p4 ) &&
					 pt_true ( getparm (parms,5), comp[pos].p5 ) &&
					 pt_true ( getparm (parms,6), comp[pos].p6 )
				 )
				{
					found_opcode = 1;
				}
			}
			
			if ( found_opcode )
			{
				if ( cmd == CMD_COMPILE )
				{
					// use original parameter instead of cleaned?
					if ( comp[pos].bytes & 0x80 )
						parmstring = (char*)((unsigned long)t + strlen ( comp[pos].opcode ) );
					else
						parmstring = (char*)((unsigned long)tmp_t + strlen ( comp[pos].opcode ) );

					if ( c->arch_mode == MODE_ARM32 )
					{
						unsigned char *cond = strdup ( parmstring );
						int cond_pos = 0;

						c->arm32_flag_h = 0;
						c->arm32_flag_sb = 0;
						c->arm32_flag_sh = 0;
						c->arm32_field_cond = 0;
						c->arm32_flag_s = 0;
						c->arm32_flag_b = 0;
						c->arm32_flag_t = 0;
						c->arm32_flag_pu = 0;

						// find the end
						while ( cond[cond_pos] != ' ' && cond[cond_pos] != '\000' )
							cond_pos++;
						cond[cond_pos] = '\000';

						// check condition
						if ( comp[pos].modifiers & 1 )
							c->arm32_field_cond = arm32_get_cond_field ( cond );

						// check S flag
						if ( comp[pos].modifiers & 2 )
							c->arm32_flag_s = arm32_get_flag ( cond, "S" );

						// check B flag
						if ( comp[pos].modifiers & 256 )
							c->arm32_flag_b = arm32_get_flag ( cond, "B" );

						// check T flag
						if ( comp[pos].modifiers & 4 )
							c->arm32_flag_t = arm32_get_flag ( cond, "T" );

						// check LDR H/SB/SH flags
						if ( comp[pos].modifiers & 8 )
						{
							c->arm32_flag_h = arm32_get_flag ( cond, "H" );
							c->arm32_flag_sb = arm32_get_flag ( cond, "SB" );
							c->arm32_flag_sh = arm32_get_flag ( cond, "SH" );
						}

						// next two handle STRD/LDRD opcodes which both use the STR command format

						// check STR H/D flags
						if ( comp[pos].modifiers & 16 )
						{
							c->arm32_flag_h = arm32_get_flag ( cond, "H" );
							c->arm32_flag_sh = arm32_get_flag ( cond, "D" );
						}

						// check LDR D flags
						if ( comp[pos].modifiers & 32 )
							c->arm32_flag_sb = arm32_get_flag ( cond, "D" );

						// check LDM flags
						if ( comp[pos].modifiers & 64 )
							c->arm32_flag_pu = arm32_get_ldm_flags ( cond );

						// check STM flags
						if ( comp[pos].modifiers & 128 )
							c->arm32_flag_pu = arm32_get_stm_flags ( cond );

						// could not parse cond flags
						if ( strlen ( cond ) )
						{
							free ( cond );
							goto continue_lookup;
						}
						free ( cond );
					}

					if ( comp[pos].modifiers )
						while ( *parmstring != ' ' && *parmstring != '\000'  )
							parmstring++;

					while ( *parmstring == ' ' || *parmstring == '\t'  )
						parmstring++;

					// call the handler
					ret = (comp[pos].function) ( c, parmstring, cmd );
					free ( tmp_t );
					return ret;
				}

				else if ( cmd == CMD_PRECOMPILE )
				{
					// is that opcode of dynamic size or is it hardcoded size?
					if ( comp[pos].params == PARM_DYNAMIC_SIZE )
					{
						// use original parameter instead of cleaned?
						if ( comp[pos].bytes & PARM_TXT )
							parmstring = (char*)((unsigned long)t + strlen ( comp[pos].opcode ) );
						else
							parmstring = (char*)((unsigned long)tmp_t + strlen ( comp[pos].opcode ) );

						if ( c->arch_mode == MODE_ARM32 )
						{
							unsigned char *cond = strdup ( parmstring );
							int cond_pos = 0;

							// find the end
							while ( cond[cond_pos] != ' ' && cond[cond_pos] != '\000' )
								cond_pos++;
							cond[cond_pos] = '\000';

							// check condition
							if ( comp[pos].modifiers & 1 )
								c->arm32_field_cond = arm32_get_cond_field ( cond );

							// check S flag
							if ( comp[pos].modifiers & 2 )
								c->arm32_flag_s = arm32_get_flag ( cond, "S" );

							// check B/T flags
							if ( comp[pos].modifiers & 4 )
							{
								c->arm32_flag_b = arm32_get_flag ( cond, "B" );
								c->arm32_flag_t = arm32_get_flag ( cond, "T" );
							}

							// check H/SB/SH flags
							if ( comp[pos].modifiers & 8 )
							{
								c->arm32_flag_h = arm32_get_flag ( cond, "H" );
								c->arm32_flag_sb = arm32_get_flag ( cond, "SB" );
								c->arm32_flag_sh = arm32_get_flag ( cond, "SH" );
							}

							// could not parse flags
							if ( strlen ( cond ) )
							{
								free ( cond );
								goto continue_lookup;
							}
							free ( cond );
						}

						// skip condition and spaces
						if ( comp[pos].modifiers )
							while ( *parmstring != ' ' && *parmstring != '\000'  )
								parmstring++;

						while ( *parmstring == ' ' || *parmstring == '\t'  )
							parmstring++;

						ret = (comp[pos].function) ( c, parmstring, cmd );
					}
					else
						ret = comp[pos].bytes;
					free ( tmp_t );
					return ret;
				}
			}
		}
continue_lookup:
		pos++;
	}
	free ( tmp_t );
	return -E_UNK_OPCODE;
}

void free_code ( struct code_data *c )
{
	int cur = 0;

	if ( c->data )
		free ( c->data );
	c->data = NULL;
	c->bytes = 0;

	for ( cur = 0; cur < MAX; cur++ )
	{
		if ( c->dword_refs[cur].data )
			free ( c->dword_refs[cur].data );
		c->dword_refs[cur].data = NULL;
		c->dword_refs[cur].pos  = 0;
		c->dword_refs[cur].used = 0;
	}
	for ( cur = 0; cur < MAX; cur++ )
	{
		if ( c->errors[cur].text )
			free ( c->errors[cur].text );
		c->errors[cur].text = NULL;
		c->errors[cur].line = 0;
	}
	for ( cur = 0; cur < MAX; cur++ )
	{
		if ( c->warnings[cur].text )
			free ( c->warnings[cur].text );
		c->warnings[cur].text = NULL;
		c->warnings[cur].line = 0;
	}
	for ( cur = 0; cur < MAX; cur++ )
	{
		if ( c->functions[cur].name )
			free ( c->functions[cur].name );
		c->functions[cur].name = NULL;
		c->functions[cur].pos  = 0;
	}
	for ( cur = 0; cur < MAX; cur++ )
	{
		if ( c->imports[cur].name )
			free ( c->imports[cur].name );
		c->imports[cur].name = NULL;
		c->imports[cur].addr = 0;
	}
	for ( cur = 0; cur < MAX; cur++ )
	{
		if ( c->labels[cur].name )
			free ( c->labels[cur].name );
		c->labels[cur].name = NULL;
		c->labels[cur].pos  = 0;
	}
	for ( cur = 0; cur < MAX; cur++ )
	{
		if ( c->d_patches[cur].name )
			free ( c->d_patches[cur].name );
		c->d_patches[cur].name = NULL;
		c->d_patches[cur].pos  = 0;
	}
	for ( cur = 0; cur < MAX; cur++ )
	{
		if ( c->b_patches[cur].name )
			free ( c->b_patches[cur].name );
		c->b_patches[cur].name = NULL;
		c->b_patches[cur].pos  = 0;
	}

	if ( c->description )
		free ( c->description );
	if ( c->name )
		free ( c->name );

	c->description = NULL;
	c->name = NULL;

	return;

}

void fill_code ( struct code_data *c )
{
	c->dword_refs[0].used  = 0;
	c->functions[0].name   = NULL;
	c->labels[0].name      = NULL;
	c->d_patches[0].name   = NULL;
	c->b_patches[0].name   = NULL;
	c->imports[0].name     = NULL;
	c->errors[0].text      = NULL;
	c->warnings[0].text    = NULL;
	c->data                = NULL;
	c->bytes               = 0;
	c->dump                = 0;
	c->located             = 0;
	c->mode                = MODE_NORM;
	c->arch_mode           = MODE_THUMB;
	c->arm32_flag_s        = 0;
	c->arm32_flag_b        = 0;
	c->arm32_flag_t        = 0;
	c->arm32_flag_h        = 0;
	c->arm32_flag_sh        = 0;
	c->arm32_flag_sb        = 0;
	c->arm32_flag_pu        = 0;
	c->arm32_field_cond    = 0x0E;
	c->base                = base;
	c->length              = length;
	c->inject              = 0;
	c->inj_pos             = 0;
	c->comments            = 0;
	c->end                 = 0;
	c->last                = 0;
	c->bytepos             = 0;
	c->use_shmem           = 0;
	c->endianess           = ENDIAN_BE;
	c->name                = strdup ( "func_new" ) ;
	c->description         = strdup ( "new_function" );
	c->cleanup             = free_code;

	return;

}



char *get_line ( char *text, unsigned int line )
{
	char          *ret = NULL;
	char          *tmp = NULL;
	unsigned int   len = 0;
	unsigned int   pos = 0;
	unsigned int   cur = 0;
	unsigned int start = 0;
	unsigned int  ende = 0;
	unsigned int  done = 0;

	static char          *prev_buf  = NULL;
	static char          *prev_text = NULL;
	static unsigned int   prev_line = 0;
	static unsigned int   prev_len  = 0;

	if ( !text )
	{
		prev_text = NULL;
		return NULL;
	}

	if ( prev_text != text || prev_line > line )
	{
		prev_buf  = text;
		prev_text = text;
		prev_line = 0;
		prev_len  = strlen (text);
	}
	else
		cur = prev_line;

	text = prev_buf;

	while ( cur <= line )
	{
		done  = 0;
		start = ende;
		if ( ret )
			free ( ret );
		ret   = NULL;
		
		while ( pos < prev_len && !done )
		{
			if ( text[pos] == '\n' )
			{
				if ( cur == line )
				{
					if ( pos - start < 1 )
						return strdup("");
					prev_line = line;
					prev_buf = (void*)((unsigned long)text + start);
					ret = malloc ( pos - start + 200 );
					memcpy ( ret, (void*)((unsigned long)text + start), pos - start );
					ret[pos-start] = '\000';
					if ( ret[pos-start-1] == '\r' || ret[pos-start-2] == '\n' )
						ret[pos-start-1] = '\000';
					if ( ret[pos-start-2] == '\r' || ret[pos-start-2] == '\n' )
						ret[pos-start-2] = '\000';
					tmp = ret;
					while ( *tmp == ' ' || *tmp == '\t' )
						tmp++;
					len = strlen ( tmp );
					tmp = malloc ( len + 200 );
					strcpy ( tmp, ret + strlen ( ret ) - len );
					free ( ret );
					ret = tmp;
					
				}
				ende = pos + 1;
				done = 1;
			}
			if ( text[pos] == '\000' )
			{
				prev_buf  = NULL;
				prev_text = NULL;
				prev_line = 0;
				prev_len  = 0;
			}
			pos++;
		}
		if ( pos > strlen ( text ) )
			return NULL;
		cur++;
	}

	return ret;
}

int add_label ( struct code_data *c, char *label, int pos )
{
	int cur = 0;

	while ( c->labels[cur].name )
	{
		if ( !strcmp ( c->labels[cur].name, label ) )
			return 0;
		cur++;
	}

	if ( cur > MAX )
		return -1;

	c->labels[cur].name = strdup ( label );
	c->labels[cur].pos  = pos;

	return 1;
}

int get_label ( struct code_data *c, char *label )
{
	int cur = 0;
	if ( !label || !c )
		return C_ERR;

	// check current labels
	while ( c->labels[cur].name )
	{
		if ( !strcmp ( strupr(c->labels[cur].name), strupr(label) ) )
			return c->labels[cur].pos;
		cur++;
	}

	cur = 0;
	// check global labels
	while ( global_c.labels[cur].name )
	{
		if ( !strcmp ( strupr(global_c.labels[cur].name), strupr(label) ) )
			return global_c.labels[cur].pos;
		cur++;
	}

	return C_ERR;
}

int add_func ( struct code_data *c, char *func, int pos )
{
	int cur = 0;
	if ( !func || !c )
		return C_ERR;

	while ( c->functions[cur].name )
		cur++;

	if ( cur > MAX )
		return -1;

	c->functions[cur].name = strdup ( func );
	c->functions[cur].pos  = pos;

	return 1;
}

int add_import_global ( char *func, unsigned long addr )
{
	int cur = 0;
	if ( !func )
		return C_ERR;

	while ( global_c.imports[cur].name )
		cur++;

	if ( cur > MAX )
		return -1;

	global_c.imports[cur].name = strupr ( strdup ( func ) );
	global_c.imports[cur].addr = addr;

	return 1;
}


int add_import ( struct code_data *c, char *func, unsigned long addr )
{
	int cur = 0;
	if ( !func || !c )
		return C_ERR;

	while ( c->imports[cur].name )
		cur++;

	if ( cur > MAX )
		return -1;

	c->imports[cur].name = strupr(strdup ( func ) );
	c->imports[cur].addr = addr;

	return 1;
}

unsigned long get_import ( struct code_data *c, char *func )
{
	int cur = 0;

	if ( !func || !c )
		return C_ERR;

	// check current labels
	while ( c->imports[cur].name )
	{
		if ( !strcmp ( strupr(c->imports[cur].name), strupr(func) ) )
			return c->imports[cur].addr;
		cur++;
	}

	cur = 0;
	// check global labels
	while ( global_c.imports[cur].name )
	{
		if ( !strcmp ( strupr(global_c.imports[cur].name), strupr(func) ) )
			return global_c.imports[cur].addr;
		cur++;
	}

	return C_ERR;

}


int add_dword ( struct code_data *c, char *data )
{
	int cur = 0;

	while ( c->dword_refs[cur].used )
	{
		if ( c->dword_refs[cur].data == data )
			return c->dword_refs[cur].pos;
		cur++;
	}

	if ( cur > MAX )
		return -1;

	c->dword_refs[cur].used = 1;
	c->dword_refs[cur].data = data;
	c->dword_refs[cur].pos  = c->data_pos;

	c->data_pos += 4;

	return 1;
}

int add_dpatch ( struct code_data *c, char *name, int pos, int abs )
{
	int cur = 0;

	while ( c->d_patches[cur].name )
		cur++;

	if ( cur > MAX )
		return -1;

	c->d_patches[cur].name = strdup ( name );
	c->d_patches[cur].pos  = pos;
	c->d_patches[cur].abs  = abs;

	return 1;
}



int add_error ( struct code_data *c, char *m, int line )
{
	int cur = 0;

	while ( c->errors[cur].text )
		cur++;
	if ( cur > MAX )
		return 0;

	c->errors[cur].text  = strdup ( m );
	c->errors[cur].line  = line;

	return 1;
}
int add_warning ( struct code_data *c, char *m, int line )
{
	int cur = 0;

	while ( c->warnings[cur].text )
		cur++;
	if ( cur > MAX )
		return 0;

	c->warnings[cur].text  = strdup ( m );
	c->warnings[cur].line  = line;

	return 1;
}

int dump_errors ( struct code_data *c, char **ret )
{
	int	cur = 0;
	int err = 0;
	char *temp = NULL;

	while ( c->errors[cur].text )
	{
		err = 1;
		if ( !*ret )
		{
			*ret = malloc ( 200 );
			sprintf ( *ret, "Error Messages:\r\n" );
		}
		temp = malloc ( strlen ( c->errors[cur].text ) + 200 );
		sprintf ( temp, "line %i: %s\r\n", c->errors[cur].line, c->errors[cur].text );
		*ret = realloc ( *ret, strlen ( *ret ) + strlen ( temp ) + 200 );
		strcat ( *ret, temp );
		free ( temp );
		cur++;
	}

	return err;
}


int dump_warnings ( struct code_data *c, char **ret )
{
	int	cur = 0;
	int err = 0;
	char *temp = NULL;

	while ( c->warnings[cur].text )
	{
		err = 1;
		if ( !*ret )
		{
			*ret = malloc ( 200 );
			sprintf ( *ret, "Warnings:\r\n" );
		}
		temp = malloc ( strlen ( c->warnings[cur].text ) + 200 );
		sprintf ( temp, "line %i: %s\r\n", c->warnings[cur].line, c->warnings[cur].text );
		*ret = realloc ( *ret, strlen ( *ret ) + strlen ( temp ) + 200 );
		strcat ( *ret, temp );
		free ( temp );
		cur++;
	}

	return err;
}

int dump_code ( struct code_data *c, char **ret )
{
	int	cur = 0;
	int byte = 0;
	char *temp = NULL;

	if ( !c->bytes )
		return 0;

   	temp = malloc ( 200 );
	switch ( c->mode)
	{
		case MODE_HEX:
			sprintf ( temp, "\r\n" );
			break;
		case MODE_GSC:
			sprintf ( temp, "Code = " );
			break;
        case MODE_TRIX:
            sprintf ( temp, "    \".text\", \"" );
            break;
		case MODE_NORM:
			sprintf ( temp, ",\r\n" );
			break;
		default:
			sprintf ( temp, "\r\n" );
			break;
	}
	*ret = realloc ( *ret, strlen ( *ret ) + strlen ( temp ) + 200 );
	strcat ( *ret, temp );
	free ( temp );


	while ( cur < c->bytes )
	{
		byte = 0;
		switch ( c->mode)
		{
			case MODE_NORM:
				temp = malloc ( 100 );
				sprintf ( temp, "    \"" );
				*ret = realloc ( *ret, strlen ( *ret ) + strlen ( temp ) + 200 );
				strcat ( *ret, temp );
				free ( temp );
				break;
			default:
				break;
		}
		while ( 
			    (byte < 32768 && cur < c->bytes && c->mode == MODE_GSC) ||
			    (byte < 8 && cur < c->bytes && c->mode == MODE_HEX) ||
				(byte < 4 && cur < c->bytes && c->mode == MODE_NORM) ||
				(byte < 32768 && cur < c->bytes && c->mode == MODE_TRIX) )
		{
			temp = malloc ( 100 );
			switch ( c->mode )
			{
				case MODE_HEX:
				case MODE_GSC:
					sprintf ( temp, "%02X", c->data[cur] ); 
					break;
				case MODE_NORM:
                case MODE_TRIX:
					sprintf ( temp, "\\x%02X", c->data[cur] );
					break;
				default:
					sprintf ( temp, "" );
					break;
			}
			*ret = realloc ( *ret, strlen ( *ret ) + strlen ( temp ) + 200 );
			strcat ( *ret, temp );
			free ( temp );
			cur++;
			byte++;
		}
		temp = malloc ( 100 );
		if ( cur == c->bytes )
			switch ( c->mode)
			{
				case MODE_HEX:
				case MODE_GSC:
					sprintf ( temp, "\r\n" );
					break;
                case MODE_TRIX:
                    sprintf ( temp, "\", " );
                    break;
				case MODE_NORM:
					sprintf ( temp, "\",\r\n" );
					break;
				default:
                    sprintf ( temp, "" );
					break;
			}
		else
			switch ( c->mode)
			{
				case MODE_HEX:
				case MODE_GSC:
					sprintf ( temp, "\r\n" );
					break;
				case MODE_NORM:
					sprintf ( temp, "\"\r\n" );
					break;
                case MODE_TRIX:
				default:
					sprintf ( temp, "" );
					break;
			}
		*ret = realloc ( *ret, strlen ( *ret ) + strlen ( temp ) + 200 );
		strcat ( *ret, temp );
		free ( temp );
	}
	temp = malloc ( 256 );
	switch ( c->mode)
	{
		case MODE_HEX:
			sprintf ( temp, "\r\nBytes: 0x%02X", c->bytes );
			break;
		case MODE_GSC:
			sprintf ( temp, "" );
			break;
		case MODE_NORM:
			sprintf ( temp, "    %i", c->bytes );
			break;
        case MODE_TRIX:
            sprintf ( temp, "\"%i\",\r\n", c->bytes );
            break;
		default:
			sprintf ( temp, "" );
			break;
	}
	*ret = realloc ( *ret, strlen ( *ret ) + strlen ( temp ) + 200 );
	strcat ( *ret, temp );
	free ( temp );

	return 1;
}

int dump_imports ( struct code_data *c, char **ret )
{
	char temp[100];
	int cur = 0;

	if ( c->dump )
	{
		while ( c->imports[cur].name )
		{
			*ret = realloc ( *ret, strlen ( *ret ) + strlen( c->imports[cur].name) + 200 ); 
			sprintf ( temp, "\r\n  0x%08X %s", c->imports[cur].addr, c->imports[cur].name );
			strcat ( *ret, temp );
			cur++;
		}
	}
	*ret = realloc ( *ret, strlen ( *ret ) + 200 ); 
	sprintf ( temp, "\r\n" );
	strcat ( *ret, temp );
	return 1;
}


int inject_code ( struct code_data *c, char **ret )
{
	if ( !c->inject )
		return 1;

#ifdef TRIXPLUG_MODE
	armada_inject_callback ( c->data, c->inj_pos, c->bytes  );
#else
	if ( !c->base )
	{
	    *ret = realloc ( *ret, strlen ( *ret ) + 200 ); 
		strcat ( *ret, "\r\n\r\n Could not inject, since no file was opened\r\n" );
		return 0;
	}

	if ( c->inj_pos + c->bytes > c->length )
	{
	    *ret = realloc ( *ret, strlen ( *ret ) + 200 ); 
		strcat ( *ret, "\r\n\r\n Could not inject, since code would be placed beyond end\r\n" );
		return 0;
	}

	memcpy ( (void*)((unsigned long)c->base + c->inj_pos), c->data, c->bytes );

    *ret = realloc ( *ret, strlen ( *ret ) + 200 ); 
	strcat ( *ret, "\r\n\r\n Code was patched successfully\r\n" );
#endif
	return 1;
}

int dump_patches ( struct code_data *c, char **ret )
{
	int	cur = 0;
	int patches = 0;
	int d_patches = 0;
	char *temp = NULL;

	while ( c->functions[patches].name )
		patches++;
	while ( c->d_patches[d_patches].name )
		d_patches++;


	temp = malloc ( 200 );
	switch ( c->mode)
	{
		case MODE_HEX:
			sprintf ( temp, "\r\nBL\'s: %i", patches );
			break;
		case MODE_GSC:
			sprintf ( temp, "" );
			break;
		case MODE_NORM:
			sprintf ( temp, ",\r\n    %i", patches );
			break;
        case MODE_TRIX:
            sprintf ( temp, "" );
			break;
		default:
			break;
	}
	*ret = realloc ( *ret, strlen ( *ret ) + strlen ( temp ) + 200 );
	strcat ( *ret, temp );
	free ( temp );

    if ( !patches && !d_patches )
        return 0;

    *ret = realloc ( *ret, strlen ( *ret ) + 200 ); 
	switch ( c->mode)
	{
		case MODE_HEX:
			strcat ( *ret, "\r\nBL-Positions: { " );
			break;
		case MODE_GSC:
			strcat ( *ret, "" );
			break;
		case MODE_NORM:
			strcat ( *ret, ",\r\n    { " );
			break;
        case MODE_TRIX:
            strcat ( *ret, "" );
            break;
		default:
			break;
	}

	if ( !patches )
	{
		temp = malloc ( 200 );
		if ( c->mode == MODE_GSC || c->mode == MODE_TRIX )
			sprintf ( temp, "" );
		else
			sprintf ( temp, "0 " );
		*ret = realloc ( *ret, strlen ( *ret ) + strlen ( temp ) + 200 );
		strcat ( *ret, temp );
		free ( temp );
	}
	while ( cur < patches )
	{
		temp = malloc ( 100 );
		if ( c->mode == MODE_GSC || c->mode == MODE_TRIX )
			sprintf ( temp, "" );
		else
		{
			if ( cur == patches - 1 )
				sprintf ( temp, "0x%02X ", c->functions[cur].pos );
			else
				sprintf ( temp, "0x%02X, ", c->functions[cur].pos );
		}
		*ret = realloc ( *ret, strlen ( *ret ) + strlen ( temp ) + 200 );
		strcat ( *ret, temp );
		free ( temp );
		cur++;
	}

	cur = 0;
	temp = malloc ( 200 );
	switch ( c->mode)
	{
		case MODE_HEX:
			sprintf ( temp, "}\r\nBL-Names: { " );
			break;
		case MODE_GSC:
		case MODE_TRIX:
			sprintf ( temp, "" );
			break;
		case MODE_NORM:
			sprintf ( temp, "},\r\n    { " );
			break;
		default:
			break;
	}
	*ret = realloc ( *ret, strlen ( *ret ) + strlen ( temp ) + 200 );
	strcat ( *ret, temp );
	free ( temp );

	if ( !patches )
	{
		temp = malloc ( 200 );
		if ( c->mode == MODE_GSC )
			sprintf ( temp, "" );
		else
			sprintf ( temp, "NULL " );
		*ret = realloc ( *ret, strlen ( *ret ) + strlen ( temp ) + 200 );
		strcat ( *ret, temp );
		free ( temp );
	}


	while ( cur < patches )
	{
		temp = malloc ( 1024 + strlen ( c->functions[cur].name ) );
		if ( c->mode == MODE_GSC )
			sprintf ( temp, "BL = 0x%02x -> %s\r\n", c->functions[cur].pos, c->functions[cur].name + 1 );
        else
            if ( c->mode == MODE_TRIX )
                sprintf ( temp, "    \"!SYM_CALL\", \"%s\", \"%i\",\r\n", strlwr (c->functions[cur].name + 1 ), c->functions[cur].pos );
		else
		{
			if ( cur == patches - 1 )
				sprintf ( temp, "&%s ", strlwr (c->functions[cur].name) + 1 );
			else
				sprintf ( temp, "&%s, ", strlwr (c->functions[cur].name) + 1 );
		}
		*ret = realloc ( *ret, strlen ( *ret ) + strlen ( temp ) + 200 );
		strcat ( *ret, temp );
		free ( temp );
		cur++;
	}
//
	if ( d_patches )
	{
		temp = malloc ( 200 );
		switch ( c->mode)
		{
			case MODE_HEX:
				sprintf ( temp, "}\r\nDataPatches: %i", d_patches );
				break;
			case MODE_NORM:
				sprintf ( temp, "},\r\n    %i", d_patches );
				break;
			case MODE_GSC:
			case MODE_TRIX:
			default:
				sprintf ( temp, "" );
				break;
		}

		*ret = realloc ( *ret, strlen ( *ret ) + strlen ( temp ) + 200 );
		strcat ( *ret, temp );
		free ( temp );


		cur = 0;
		temp = malloc ( 200 );
		switch ( c->mode)
		{
			case MODE_HEX:
				sprintf ( temp, "\r\nDataPatch-Pos: { " );
				break;
			case MODE_NORM:
				sprintf ( temp, ",\r\n    { " );
				break;
			case MODE_GSC:
			case MODE_TRIX:
			default:
				sprintf ( temp, "" );
				break;
		}
		*ret = realloc ( *ret, strlen ( *ret ) + strlen ( temp ) + 200 );
		strcat ( *ret, temp );
		free ( temp );

		while ( cur < d_patches )
		{
			temp = malloc ( 200 );
			if ( c->mode == MODE_GSC || c->mode == MODE_TRIX )
				sprintf ( temp, "" );
			else
			{
				if ( cur == d_patches - 1 )
					sprintf ( temp, "0x%02X ", c->d_patches[cur].pos );
				else
					sprintf ( temp, "0x%02X, ", c->d_patches[cur].pos );
			}
			*ret = realloc ( *ret, strlen ( *ret ) + strlen ( temp ) + 200 );
			strcat ( *ret, temp );
			free ( temp );
			cur++;
		}
	//

		cur = 0;
		temp = malloc ( 200 );
		switch ( c->mode)
		{
			case MODE_HEX:
				sprintf ( temp, "}\r\nDataPatch-Names: { " );
				break;
			case MODE_NORM:
				sprintf ( temp, "},\r\n    { " );
				break;
			case MODE_GSC:
			case MODE_TRIX:
			default:
				sprintf ( temp, "" );
				break;
		}

		*ret = realloc ( *ret, strlen ( *ret ) + strlen ( temp ) + 200 );
		strcat ( *ret, temp );
		free ( temp );

		while ( cur < d_patches )
		{
			temp = malloc ( 10 + strlen ( c->d_patches[cur].name ) );
			if ( c->mode == MODE_GSC || c->mode == MODE_TRIX )
				sprintf ( temp, "" );
			else
			{
				if ( cur == d_patches - 1 )
					sprintf ( temp, "&%s ", strlwr (c->d_patches[cur].name));
				else
					sprintf ( temp, "&%s, ", strlwr (c->d_patches[cur].name) );
			}
			*ret = realloc ( *ret, strlen ( *ret ) + strlen ( temp ) + 200 );
			strcat ( *ret, temp );
			free ( temp );
			cur++;
		}
//
		cur = 0;
		temp = malloc ( 200 );
		switch ( c->mode)
		{
			case MODE_HEX:
				sprintf ( temp, "}\r\nDataPatch absolute: { " );
				break;
			case MODE_NORM:
				sprintf ( temp, "},\r\n    { " );
				break;
			case MODE_GSC:
			case MODE_TRIX:
			default:
				sprintf ( temp, "" );
				break;
		}

		*ret = realloc ( *ret, strlen ( *ret ) + strlen ( temp ) + 200 );
		strcat ( *ret, temp );
		free ( temp );

		while ( cur < d_patches )
		{
			temp = malloc ( 1024 );
			if ( c->mode == MODE_GSC )
				sprintf ( temp, "Data = 0x%02x -> %s %c %c\r\n", c->d_patches[cur].pos, c->d_patches[cur].name, c->d_patches[cur].abs?'Y':'N', 'N' );
			else
                if ( c->mode == MODE_TRIX )
                    sprintf ( temp, "    \"!SYM_PTR\", \"%s\", \"%i\",\r\n", c->d_patches[cur].name, c->d_patches[cur].pos );
			else
			{
				if ( cur == d_patches - 1 )
					sprintf ( temp, "%i ", c->d_patches[cur].abs );
				else
					sprintf ( temp, "%i, ", c->d_patches[cur].abs );
			}
			*ret = realloc ( *ret, strlen ( *ret ) + strlen ( temp ) + 200 );
			strcat ( *ret, temp );
			free ( temp );
			cur++;
		}
	}


	temp = malloc ( 200 );
	if ( c->mode == MODE_GSC )
		sprintf ( temp, "" );
	else
		sprintf ( temp, "}" );
	*ret = realloc ( *ret, strlen ( *ret ) + strlen ( temp ) + 200 );
	strcat ( *ret, temp );
	free ( temp );

	return 1;
}



char *compile ( char *text )
{
	int start   = 0;
	int  ende   = 0;
	int   pos   = 0;
	int  fail   = 0;
	int   cur   = 0;
	int  bytes  = 0;
	int    rc   = 0;
	int done    = 0;
	int nextpos = 0;
	int oldpos  = 0;
	int banner  = 0;
	int last    = 0;
	char *line  = NULL;
	char *label = NULL;
	char *temp  = NULL;
	char *ret   = NULL;

	free_code ( &global_c );
	fill_code ( &global_c );

	// init
	get_line ( NULL, 0 );

	ret = malloc ( 10000 );
	sprintf ( ret, "" );
	function = 0;
	while ( !done )
	{
		free_code ( &code[function] );
		fill_code ( &code[function] );

		oldpos = nextpos;
		pos = oldpos;
		while ( (line = get_line ( text, pos )) != NULL && !code[function].end  )
		{
			if ( !strncmp ( "@", line, 1 ) )
			{
				fail = 0;
				start = 0;
				while ( line[start] == ' ' && line[start] != '\000' )  
					start++;

				ende = start;
				while ( line[ende] != ':' && line[ende] != ' ' && line[ende] != '\000' )  
					ende++;

				if ( line[start] == '\000' || line[ende] == '\000' )
					fail = 1;

				if ( ende - start > 0 )
				{
					label = malloc ( ende - start + 10 );
					memcpy ( label, line + start, ende - start );
					label[ende-start] = '\000';
					if ( !add_label ( &code[function], label, code[function].bytepos ) )
					{
						temp = malloc ( strlen ( label ) + 80 );
						sprintf ( temp, "Label %s in line %i, byte %i already exists", label, pos + 1, code[function].bytepos );
						add_error ( &code[function], temp, pos + 1 );
						free ( temp );
					}
					free ( label );
				}
				else
					fail = 1;

				if ( fail )
				{
					temp = malloc ( 200 );
					sprintf ( temp, "Label in line %i is invalid", pos + 1 );
					add_error ( &code[function], temp, pos + 1 );
					free ( temp );
				}
			}
			else
			{
				code[function].current_line = pos + 1;
				rc = compile_line ( &code[function], line, CMD_PRECOMPILE );
				if ( rc < 0 )
				{
					if ( rc == -1024 )
						pos--;
				}
				else
					code[function].bytepos += rc;

			}
			free ( line );
			pos++;
		}
		if ( line )
			free ( line );
		code[function].end = 0;

		if ( !line )
		{
			done = 1;
			code[function].last = 1;
		}
		nextpos = pos;

		if ( code[function].comments > 0 )
		{
			temp = malloc ( 200 );
			sprintf ( temp, "Unclosed comment - /*  " );
			add_error ( &code[function], temp, pos );
			free ( temp );
		}
		if ( code[function].comments < 0 )
		{
			temp = malloc ( 200 );
			sprintf ( temp, "Too many closed comments - */  " );
			add_error ( &code[function], temp, pos );
			free ( temp );
		}

		code[function].data_pos = ((code[function].bytepos / 4) + 1) * 4;

		pos = oldpos;
		line = NULL;
		code[function].bytepos = 0;
		code[function].arch_mode = MODE_THUMB;

		while ( (line = get_line ( text, pos )) && !code[function].end)
		{
			code[function].current_line = pos + 1;
			if ( (rc = compile_line ( &code[function], line, CMD_COMPILE )) < 0 )
			{
				if ( rc == -1024 )
					pos--;
				else
				{
					temp = malloc ( strlen ( errors[-rc] ) + strlen ( line ) + 20 );
					sprintf ( temp, "%s '%s'", errors[-rc], line );
					add_error ( &code[function], temp, pos + 1 );
					free ( temp );
					fail = 0;
				}
			}
			code[function].bytepos += rc;
			free ( line );
			pos++;
		}
		if ( line )
			free ( line );
		function++;
		code[function].mode = code[function-1].mode;
	}

	function = 0; 
	highest_pos = 0;

	do {
		if ( !(banner++) )
		{
			temp = malloc ( 256 );
			sprintf ( temp, VERSION " " __TIME__", "__DATE__"\r\n" );
			ret = realloc ( ret, strlen ( ret ) + strlen ( temp ) + 200 );
			strcat ( ret, temp );
			free ( temp );
		}
		dump_imports ( &code[function], &ret );
 		dump_warnings ( &code[function], &ret );
		if ( !dump_errors ( &code[function], &ret )  && code[function].bytes )
		{
			temp = malloc ( 8192 );

			if ( code[function].maxsize > 0 && code[function].bytes > code[function].maxsize )
			{
				ret = realloc ( ret, strlen ( ret ) + 300 );
				strcat ( ret, "WARNING!!!\r\n" );
				sprintf ( temp, "Your code is with %i bytes %i bytes bigger\r\nthan the maximum allowed size of %i bytes\r\n\r\n", code[function].bytes, code[function].bytes- code[function].maxsize, code[function].maxsize );
				strcat ( ret, temp );
			}

			switch ( code[function].mode)
			{
				case MODE_HEX:
					sprintf ( temp, "\r\nnew function to paste in hexeditor:\r\npaste at 0x%06X!\r\n", code[function].inj_pos );
					strcpy ( temp, "" );
					break;
				case MODE_GSC:
					sprintf ( temp, "\r\n[CODE]\r\nName = %s\r\n", code[function].name );
					break;
				case MODE_NORM:
					sprintf ( temp, "\r\nfunction %s =\r\n{\r\n    \"%s\"", code[function].name, code[function].description );
					break;
                case MODE_TRIX:
                    sprintf ( temp, "\r\nfunc %s[] =\r\n{\r\n    \"name\", \"%s\", \"\",\r\n", code[function].name, code[function].name );
                    break;
				default:
					break;
			}


			ret = realloc ( ret, strlen ( ret ) + strlen ( temp ) + 200 );
			strcat ( ret, "Endianess: " );
			strcat ( ret, (code[function].endianess == ENDIAN_BE)?"BE - Big Endian\r\n":"LE - Little Endian\r\n" );
			strcat ( ret, temp );


			free ( temp );

			dump_code ( &code[function], &ret );
			dump_patches ( &code[function], &ret );
			inject_code ( &code[function], &ret );

			if ( code[function].inject && code[function].inj_pos + code[function].bytes > highest_pos )
				highest_pos = code[function].inj_pos + code[function].bytes;

			ret = realloc ( ret, strlen ( ret ) + 200 );
			switch ( code[function].mode)
			{
				case MODE_HEX:
					strcat ( ret, "\r\n" );
					break;
				case MODE_GSC:
					strcat ( ret, "" );
					break;
                case MODE_TRIX:
                    strcat ( ret, "    NULL\r\n};\r\n" );
					break;
				case MODE_NORM:
					strcat ( ret, "\r\n};\r\n" );
					break;
				default:
					break;
			}
		}
		last = code[function].last;
		free_code ( &code[function] );
		function++;
	} 	while ( !last );

	return ret;
}

struct code_data *get_code ( void )
{
	return &code[function];
}

int set_flash ( unsigned char* _base, unsigned long _length )
{
	base   = _base;
	length = _length;

	return 1;
}