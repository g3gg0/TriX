/* Instruction printing code for the ARM
Copyright (C) 1994, 95, 96, 97, 98, 99, 2000 Free Software Foundation, Inc.
Contributed by Richard Earnshaw (rwe@pegasus.esprit.ec.org)
Modification by James G. Smith (jsmith@cygnus.co.uk)

This file is part of libopcodes.

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation; either version 2 of the License, or (at your option)
any later version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#define DEFINE_TABLE
/*#include <stdio.h>
#include <stdlib.h>
#include <string.h>*/
#include "arm-opc.h"
#include "arm-dis.h"

//#include "BTreeTypes.h"
//#include "Styles.h"

typedef unsigned int DWORD;
typedef unsigned char BYTE;

BYTE *fdata;
DWORD foffset;
void *fbtree;
char dis_line[512];
char tmp[128];
unsigned long reg_val[16];	// reg value
unsigned char reg_ok[16];	// true if reg is valid

#define CALL	1
#define JUMP	0

// Line info generation variables
INFO info;
int need_info = 0;
int need_string = 1;
int need_regs = 0;
/////////////////////////////////////

void *get_addr_data(void *brtree, DWORD address, DWORD type);

void print_string(int saddr);

unsigned char read_byte (unsigned long address, void *i);
unsigned short read_hword (unsigned long address, void *i);
unsigned long read_word (unsigned long address, void *i);
void write_byte (unsigned long address, unsigned char data, void *i);
void write_hword (unsigned long address, unsigned short data, void *i);
void write_word (unsigned long address, unsigned long data, void *i);
unsigned long read_aligned_word (unsigned long address, void *i);
unsigned short read_aligned_hword (unsigned long address, void *i);

#define false	0
#define true	1

#ifndef streq
#define streq(a,b)	(strcmp ((a), (b)) == 0)
#endif

#ifndef strneq
#define strneq(a,b,n)	(strncmp ((a), (b), (n)) == 0)
#endif

#ifndef NUM_ELEM
#define NUM_ELEM(a)     (sizeof (a) / sizeof (a)[0])
#endif

static char * arm_conditional[] =
{
	"eq", "ne", "cs", "cc", "mi", "pl", "vs", "vc",
	"hi", "ls", "ge", "lt", "gt", "le", "", "nv"
};

const char *arm_regnames[16] = {
	"R0","R1","R2","R3","R4","R5","R6","R7","R8","R9","R10","R11","R12","SP","LR","PC"
};

#define NUM_ARM_REGNAMES  NUM_ELEM (regnames)

static char * arm_fp_const[] =
{
	"0.0", "1.0", "2.0", "3.0", "4.0", "5.0", "0.5", "10.0"
};


static char * arm_shift[] =
{
	"lsl", "lsr", "asr", "ror"
};


#define PARAMS(x) x


void print_address_func ( int address, int type )
{
	if (need_string)
	{
		sprintf(tmp, "%08X", address );
		strcat(dis_line, tmp);
	}
	if (need_info){
		switch(type){
			case CALL_BIT:
				info.mask |= CALL_BIT;
				info.jump_addr = address;
				break;
			case JUMP_BIT:
				info.mask |= JUMP_BIT;
				info.jump_addr = address;
				break;
			case DATA_BIT:
				info.mask |= DATA_BIT;
				info.data_addr = address;
				break;
		}
	}
}


static void
arm_decode_shift (long given)
{
	sprintf(tmp, "%s", arm_regnames[given & 0xf]);
	strcat(dis_line, tmp);

	if ((given & 0xff0) != 0)
	{
		if ((given & 0x10) == 0)
		{
			int amount = (given & 0xf80) >> 7;
			int shift = (given & 0x60) >> 5;

			if (amount == 0)
			{
				if (shift == 3)
				{
					strcat(dis_line, ", rrx");
					return;
				}

				amount = 32;
			}

			sprintf(tmp, ", %s #%d", arm_shift[shift], amount);
			strcat(dis_line, tmp);
		}
		else {
			sprintf(tmp, ", %s %s", arm_shift[(given & 0x60) >> 5],
				arm_regnames[(given & 0xf00) >> 8]);
			strcat(dis_line, tmp);
		}
	}
}


/* Print one instruction from PC on INFO->STREAM.
Return the size of the instruction (always 4 on ARM). */

static int
print_insn_arm (int pc, long given, void* i)
{
	struct arm_opcode *  insn;
	static pc_used = 0;
	DWORD last_addr;

	tmp[0] = 0;

	for (insn = arm_opcodes; insn->assembler; insn++)
	{
		if ((given & insn->mask) == insn->value)
		{
			char * c;

			for (c = insn->assembler; *c; c++)
			{
				if (*c == '%')
				{
					switch (*++c)
					{
					case '%':
						sprintf(tmp, "%%");
						strcat(dis_line, tmp);
						break;

					case 'a':
						if (((given & 0x000f0000) == 0x000f0000)
								&& ((given & 0x02000000) == 0))
						{
							int offset = given & 0xfff;

							//sprintf(tmp, "[pc");
							//strcat(dis_line, tmp);

							if (given & 0x01000000)
							{
								if ((given & 0x00800000) == 0)
									offset = - offset;

								/* pre-indexed */

								//sprintf(tmp, ", #%X]", offset);
								//strcat(dis_line, tmp);

								offset += pc + 8;
								info.mask |= XREF_BIT;
								info.xref_addr = offset;
// FIXME: reference across objects
								last_addr = read_word(offset,i);
								print_address_func(last_addr, DATA_BIT);

								/* Cope with the possibility of write-back
								being used.  Probably a very dangerous thing
								for the programmer to do, but who are we to
								argue ?  */
								if (given & 0x00200000)
									strcat(dis_line, "!");
							}
							else
							{
								/* Post indexed.  */
								sprintf(tmp, "[pc], #0x%X", offset);
								strcat(dis_line, tmp);

								offset = pc + 8;  /* ie ignore the offset.  */
		
								strcat(dis_line, "\t; ");
								print_address_func (offset, DATA_BIT);
							}

						}
						else
						{
							sprintf(tmp, "[%s",
								arm_regnames[(given >> 16) & 0xf]);
							strcat(dis_line, tmp);

							if ((given & 0x01000000) != 0)
							{
								if ((given & 0x02000000) == 0)
								{
									int offset = given & 0xfff;
									if (offset){
										sprintf(tmp, ", %s#%d",
											(((given & 0x00800000) == 0)
												? "-" : ""), offset);
										strcat(dis_line, tmp);
									}
								}
								else
									{
									sprintf(tmp, ", %s",
										(((given & 0x00800000) == 0)
											? "-" : ""));
									strcat(dis_line, tmp);
									arm_decode_shift (given);
								}

								sprintf(tmp, "]%s",
									((given & 0x00200000) != 0) ? "!" : "");
								strcat(dis_line, tmp);
							}
							else
							{
								if ((given & 0x02000000) == 0)
								{
									int offset = given & 0xfff;
									if (offset){
										sprintf(tmp, "], %s#%d",
											(((given & 0x00800000) == 0)
												? "-" : ""), offset);
										strcat(dis_line, tmp);
									} else
										strcat(dis_line, "]");
								}
								else
								{
									sprintf(tmp, "], %s",
										(((given & 0x00800000) == 0)
											? "-" : ""));
									strcat(dis_line, tmp);
									arm_decode_shift (given);
								}
							}
						}
						break;

					case 's':
						if ((given & 0x004f0000) == 0x004f0000)
						{
							/* PC relative with immediate offset.  */
							int offset = ((given & 0xf00) >> 4) | (given & 0xf);

							if ((given & 0x00800000) == 0)
								offset = -offset;

// FIXME: reference across objects
							last_addr = read_word(offset+pc+8,i);

							//sprintf(tmp, "[pc, #%X]\t; ", offset);
							//strcat(dis_line, tmp);

							print_address_func(last_addr, DATA_BIT);
						}
						else
							{
							sprintf(tmp, "[%s",
								arm_regnames[(given >> 16) & 0xf]);
							strcat(dis_line, tmp);
							if ((given & 0x01000000) != 0)
							{
								/* Pre-indexed.  */
								if ((given & 0x00400000) == 0x00400000)
								{
									/* Immediate.  */
									int offset = ((given & 0xf00) >> 4) | (given & 0xf);
									if (offset){
										sprintf(tmp, ", %s#%d",
											(((given & 0x00800000) == 0)
												? "-" : ""), offset);
										strcat(dis_line, tmp);
									}
								}
								else
									{
									/* Register.  */
									sprintf(tmp, ", %s%s",
										(((given & 0x00800000) == 0)
											? "-" : ""),
										arm_regnames[given & 0xf]);
									strcat(dis_line, tmp);
								}

								sprintf(tmp, "]%s",
									((given & 0x00200000) != 0) ? "!" : "");
								strcat(dis_line, tmp);
							}
							else
							{
								/* Post-indexed.  */
								if ((given & 0x00400000) == 0x00400000)
								{
									/* Immediate.  */
									int offset = ((given & 0xf00) >> 4) | (given & 0xf);
									if (offset){
										sprintf(tmp, "], %s#%d",
											(((given & 0x00800000) == 0)
												? "-" : ""), offset);
										strcat(dis_line, tmp);
									} else
										strcat(dis_line, "]");
								}
								else
								{
									/* Register.  */
									sprintf(tmp, "], %s%s",
										(((given & 0x00800000) == 0)
											? "-" : ""),
										arm_regnames[given & 0xf]);
									strcat(dis_line, tmp);
								}
							}
						}
						break;

					case 'b':
						print_address_func(BDISP (given) * 4 + pc + 8, JUMP_BIT);
						break;

					case 'c':
						sprintf(tmp, "%s",
							arm_conditional [(given >> 28) & 0xf]);
						strcat(dis_line, tmp);
						break;

					case 'm':
						{
							int started = 0;
							int reg;

							strcat(dis_line, "{");
							for (reg = 0; reg < 16; reg++)
								if ((given & (1 << reg)) != 0)
								{
									if (started)
										strcat(dis_line, ", ");
									started = 1;
									strcat(dis_line, arm_regnames[reg]);

								}
							strcat(dis_line, "}");
						}
						break;

					case 'o':
						if ((given & 0x02000000) != 0)
						{
							int rotate = (given & 0xf00) >> 7;
							int immed = (given & 0xff);
							immed = (((immed << (32 - rotate))
									| (immed >> rotate)) & 0xffffffff);
							if (pc_used)
							{
								sprintf(tmp, "0x%02X \t;", immed);
								strcat(dis_line, tmp);
								print_address_func(pc+immed+8, DATA_BIT);
								pc_used = 0;
							} else {
								sprintf(tmp, "0x%02X", immed);
								strcat(dis_line, tmp);
							}
						}
						else
							arm_decode_shift (given);
						break;

					case 'p':
						if ((given & 0x0000f000) == 0x0000f000)
							strcat(dis_line, "p");
						break;

					case 't':
						if ((given & 0x01200000) == 0x00200000)
							strcat(dis_line, "t");
						break;

					case 'h':
						if ((given & 0x00000020) == 0x00000020)
							strcat(dis_line, "h");
						else
							strcat(dis_line, "b");
						break;

					case 'A':
						sprintf(tmp, "[%s", arm_regnames [(given >> 16) & 0xf]);
						if ((given & 0x01000000) != 0)
						{
							int offset = given & 0xff;
							if (offset){
								sprintf(tmp, ", %s#%d]%s",
									((given & 0x00800000) == 0 ? "-" : ""),
									offset * 4,
									((given & 0x00200000) != 0 ? "!" : ""));
								strcat(dis_line, tmp);
							} else
								strcat(dis_line, "]");
						}
						else
						{
							int offset = given & 0xff;
							if (offset){
								sprintf(tmp, "], %s#%d",
									((given & 0x00800000) == 0 ? "-" : ""),
									offset * 4);
								strcat(dis_line, tmp);
							} else
								strcat(dis_line, "]");
						}
						break;

					case 'B':
						/* Print ARM V5 BLX(1) address: pc+25 bits.  */
						{
							long address;
							long offset = 0;

							if (given & 0x00800000)
								/* Is signed, hi bits should be ones.  */
								offset = (-1) ^ 0x00ffffff;

							/* Offset is (SignExtend(offset field)<<2).  */
							offset += given & 0x00ffffff;
							offset <<= 2;
							address = offset + pc + 8;

							if (given & 0x01000000)
								/* H bit allows addressing to 2-byte boundaries.  */
								address += 2;

							print_address_func (address, CALL_BIT);
						}
						break;

					case 'C':
						strcat(dis_line, "_");
						if (given & 0x80000)
							strcat(dis_line, "f");
						if (given & 0x40000)
							strcat(dis_line, "s");
						if (given & 0x20000)
							strcat(dis_line, "x");
						if (given & 0x10000)
							strcat(dis_line, "c");
						break;

					case 'F':
						switch (given & 0x00408000)
						{
						case 0:
							strcat(dis_line, "4");
							break;
						case 0x8000:
							strcat(dis_line, "1");
							break;
						case 0x00400000:
							strcat(dis_line, "2");
							break;
						default:
							strcat(dis_line, "3");
						}
						break;

					case 'P':
						switch (given & 0x00080080)
						{
						case 0:
							strcat(dis_line, "s");
							break;
						case 0x80:
							strcat(dis_line, "d");
							break;
						case 0x00080000:
							strcat(dis_line, "e");
							break;
						default:
							strcat(dis_line, "<illegal precision>");
							break;
						}
						break;
					case 'Q':
						switch (given & 0x00408000)
						{
						case 0:
							strcat(dis_line, "s");
							break;
						case 0x8000:
							strcat(dis_line, "d");
							break;
						case 0x00400000:
							strcat(dis_line, "e");
							break;
						default:
							strcat(dis_line, "p");
							break;
						}
						break;
					case 'R':
						switch (given & 0x60)
						{
						case 0:
							break;
						case 0x20:
							strcat(dis_line, "p");
							break;
						case 0x40:
							strcat(dis_line, "m");
							break;
						default:
							strcat(dis_line, "z");
							break;
						}
						break;

					case '0':
					case '1':
					case '2':
					case '3':
					case '4':
					case '5':
					case '6':
					case '7':
					case '8':
					case '9':
						{
							int bitstart = *c++ - '0';
							int bitend = 0;
							while (*c >= '0' && *c <= '9')
								bitstart = (bitstart * 10) + *c++ - '0';

							switch (*c)
							{
							case '-':
								c++;

								while (*c >= '0' && *c <= '9')
									bitend = (bitend * 10) + *c++ - '0';

								if (!bitend)
									abort ();

								switch (*c)
								{
								case 'r':
									{
										long reg;

										reg = given >> bitstart;
										reg &= (2 << (bitend - bitstart)) - 1;

										if (reg==15 && bitstart==16){
											pc_used = 1;
										}

										strcat(dis_line, arm_regnames[reg]);
									}
									break;
								case 'd':
									{
										long reg;

										reg = given >> bitstart;
										reg &= (2 << (bitend - bitstart)) - 1;

										sprintf(tmp, "%d", reg);
										strcat(dis_line, tmp);
									}
									break;
								case 'x':
									{
										long reg;

										reg = given >> bitstart;
										reg &= (2 << (bitend - bitstart)) - 1;

										sprintf(tmp, "0x%08x", reg);
										strcat(dis_line, tmp);

										/* Some SWI instructions have special
										meanings.  */
										if ((given & 0x0fffffff) == 0x0FF00000)
											strcat(dis_line, "\t; IMB");
										else if ((given & 0x0fffffff) == 0x0FF00001)
											strcat(dis_line, "\t; IMBRange");
									}
									break;
								case 'X':
									{
										long reg;

										reg = given >> bitstart;
										reg &= (2 << (bitend - bitstart)) - 1;

										sprintf(tmp, "%01x", reg & 0xf);
										strcat(dis_line, tmp);
									}
									break;
								case 'f':
									{
										long reg;

										reg = given >> bitstart;
										reg &= (2 << (bitend - bitstart)) - 1;

										if (reg > 7){
											sprintf(tmp, "#%s",
												arm_fp_const[reg & 7]);
											strcat(dis_line, tmp);
										} else {
											sprintf(tmp, "f%d", reg);
											strcat(dis_line, tmp);
										}
									}
									break;
								default:
									abort ();
								}
								break;

							case '`':
								c++;
								if ((given & (1 << bitstart)) == 0){
									sprintf(tmp, "%c", *c);
									strcat(dis_line, tmp);
								}
								break;
							case '\'':
								c++;
								if ((given & (1 << bitstart)) != 0){
									sprintf(tmp, "%c", *c);
									strcat(dis_line, tmp);
								}
								break;
							case '?':
								++c;
								if ((given & (1 << bitstart)) != 0){
									sprintf(tmp, "%c", *c++);
									strcat(dis_line, tmp);
								} else {
									sprintf(tmp, "%c", *++c);
									strcat(dis_line, tmp);
								}
								break;
							default:
								abort ();
							}
							break;

						default:
							abort ();
						}
					}
				}
				else {
					sprintf(tmp, "%c", *c);
					strcat(dis_line, tmp);
				}
			}
			return 4;
		}
	}
	abort ();

	return 4;
}


/* Print one instruction from PC 
Return the size of the instruction. */
static int
print_insn_thumb (int pc, long given,void *i)
{
	struct thumb_opcode * insn;
	static int last_addr = 0;
	char spec;
	int bADR = false;
	int n, reg, reg_s, reg_d, val;

	for (insn = thumb_opcodes; insn->assembler; insn++)
	{
		if ((given & insn->mask) == insn->value)
		{
			char * c = insn->assembler;

			/* Special processing for casting (lsl - lsr)*/
			if (*c == 'z'){
				int r2, r3, im1, im2;
				// If not lsr continue
				if ((given & 0xF8000000) != 0x08000000)
					continue;
				r2 = (given >> 3) & 7;
				im1 = (given >> 6) & 31;

				r3 = (given >> 16) & 7;
				im2 = (given >> 22) & 31;

				if (im1 != im2){
					// For now
					continue;
				} else {
					DWORD val = 0x7FFFFFFF >> (im1-1);
					if (r2 == r3)
						sprintf(tmp, "cst  \t%s, 0x%X (0x%X)",
							arm_regnames[r2], val, 32-im1);
					else
						sprintf(tmp, "cst  \t%s, %s, 0x%X (0x%X)",
							arm_regnames[r3], arm_regnames[r2], val, 32-im1);
					strcat(dis_line, tmp);
				}
				return 4;
			}

			/* Special processing for Thumb 2 instruction BL sequence:  */
			if (!*c) /* Check for empty (not NULL) assembler string.  */
			{
				long offset;

				offset = BDISP23 (given);

				if ((given & 0x10000000) == 0)
				{
					sprintf(tmp, "blx  \t");
					strcat(dis_line, tmp);

					/* The spec says that bit 1 of the branch's destination
					address comes from bit 1 of the instruction's
					address and not from the offset in the instruction.  */
					if (offset & 0x1)
					{
						/* sprintf(tmp, "*malformed!* "); */
						offset &= ~ 0x1;
					}

					offset |= ((pc & 0x2) >> 1);
				}
				else {
					strcat(dis_line, "bl   \t");
				}

				last_addr = offset * 2 + pc + 4;
				print_address_func (last_addr, CALL_BIT);
				// reset the registers
				if (need_regs){
					for (n=0; n<4; n++)
						reg_ok[n] = 0;
				}
				return 4;
			}
			else
				{
				given &= 0xffff;

				for (; *c; c++)
				{
					if (*c == '%')
					{
						int domaskpc = 0;
						int domasklr = 0;

						switch (*++c)
						{
						case '%':
							strcat(dis_line, "%%");
							break;

						case '+':
							// Add special info to the instruction
							spec = *++c;
							if (need_info){
								switch(spec){
									case '1':
										// bx reg
										if ((given & 0x78) == 0x70){
											// bx lr
											info.mask |= RET_BIT;
										}
										break;
									case '2':
										// pop pc
										if (given & 0x100){
											info.mask |= RET_BIT;
										}
										break;
									case '3':
										// push
										info.mask |= PUSH_BIT;
										break;
									case '4':
										if (need_regs){
											// mov rX, NN
											reg = (given >> 8) & 7;
											val = (given & 0xFF);
											switch (given >> 11){
												case 0: //0x0000 lsl rD, rS, NN
													reg_s = (given >> 3) & 7;
													if (reg_ok[reg_s]){
														reg_d = given & 7;
														val = (given >> 6) & 0x1F;
														info.value = reg_val[reg_d] = reg_val[reg_s] << val;
														info.mask |= VAL_BIT;
													}
													break;
												case 4: //0x2000 mov rX, NN
													reg_val[reg] = val;
													reg_ok[reg] = true;
													break;
												case 6:	//0x3000 add rX, NN
													if (reg_ok[reg]){
														reg_val[reg] += val;
														info.value = reg_val[reg];
														info.mask |= VAL_BIT;
													}
													break;
												case 7: //0x3800 sub rX, NN
													if (reg_ok[reg]){
														reg_val[reg] -= val;
														info.value = reg_val[reg];
														info.mask |= VAL_BIT;
													}
													break;
												case 9: //0x4800 ldr rX, NNNN
													reg_val[reg] = info.data_addr;
													reg_ok[reg] = true;
													break;
											}
										}
										break;
									case '5':
										if (need_regs){
											// Register is invalid
											if ((given & 0xF800) >= 0x9800)
												reg = (given >> 8) &7;
											else
												reg = given & 7;
											reg_ok[reg] = false;
										}
										break;
								}// switch
							}
							break;

						case 'S':
							{
								long reg;

								reg = (given >> 3) & 0x7;
								if (given & (1 << 6))
									reg += 8;

								strcat(dis_line, arm_regnames[reg]);
							}
							break;

						case 'D':
							{
								long reg;

								reg = given & 0x7;
								if (given & (1 << 7))
									reg += 8;

								strcat(dis_line, arm_regnames[reg]);
							}
							break;

						case 'T':
							strcat(dis_line, arm_conditional [(given >> 8) & 0xf]);
							break;

						case 'N':
							if (given & (1 << 8))
								domasklr = 1;
							/* Fall through.  */
						case 'O':
							if (*c == 'O' && (given & (1 << 8)))
								domaskpc = 1;
							/* Fall through.  */
						case 'M':
							{
								int started = 0, range = 0;
								int reg, last_reg = -3;

								strcat(dis_line, "{");

								for (reg = 0; reg < 8; reg++)
									if ((given & (1 << reg)) != 0)
									{
										if (reg == last_reg+1){
											if (!range){
												strcat(dis_line, "-");
												range = 1;
											}
										} else {
											if (range)
												strcat(dis_line, arm_regnames[last_reg]);
											if (started)
												strcat(dis_line, ", ");
											strcat(dis_line, arm_regnames[reg]);
											started = 1;
											range = 0;
										}
										last_reg = reg;
									}
								if (range){
									strcat(dis_line, arm_regnames[last_reg]);
								}

								if (domasklr)
								{
									if (started)
										strcat(dis_line, ", ");
									started = 1;
									strcat(dis_line, arm_regnames[14] /* "lr" */);
								}

								if (domaskpc)
								{
									if (started)
										strcat(dis_line, ", ");
									strcat(dis_line, arm_regnames[15] /* "pc" */);
								}

								strcat(dis_line, "}");
							}
							break;

						case '0':
						case '1':
						case '2':
						case '3':
						case '4':
						case '5':
						case '6':
						case '7':
						case '8':
						case '9':
							{
								int bitstart = *c++ - '0';
								int bitend = 0;

								while (*c >= '0' && *c <= '9')
									bitstart = (bitstart * 10) + *c++ - '0';

								switch (*c)
								{
								case '-':
									{
										long reg;

										c++;
										while (*c >= '0' && *c <= '9')
											bitend = (bitend * 10) + *c++ - '0';
										if (!bitend)
											return 0;
										reg = given >> bitstart;
										reg &= (2 << (bitend - bitstart)) - 1;
										switch (*c)
										{
										case 'r':
											strcat(dis_line, arm_regnames[reg]);
											break;

										case 'd':
											sprintf(tmp, "%d", reg);
											strcat(dis_line, tmp);
											break;

										case 'H':
											sprintf(tmp, "0x%02X", reg << 1);
											strcat(dis_line, tmp);
											break;

										case 'W':
											sprintf(tmp, "0x%02X", reg << 2);
											strcat(dis_line, tmp);
											break;

										case 'A':
										case 'a':
											/* PC-relative address -- the bottom two
											bits of the address are dropped
											before the calculation.  */
											last_addr = ((pc + 4) & ~3) + (reg << 2);
											print_address_func(last_addr, (*c=='A')?DATA_BIT:JUMP_BIT);
											break;

										case 'z':
											/* Show content of PC-rel address */
											last_addr = ((pc + 4) & ~3) + (reg << 2);
											info.mask |= XREF_BIT;
											info.xref_addr = last_addr;
// FIXME: reference across objects
											last_addr = read_word(last_addr,i);
											print_address_func(last_addr, DATA_BIT);
											break;


										case 'x':
											sprintf(tmp, "0x%02X", reg);
											strcat(dis_line, tmp);
											break;

										case 'I':
											reg = ((reg ^ (1 << bitend)) - (1 << bitend));
											sprintf(tmp, "%d", reg);
											strcat(dis_line, tmp);
											break;

										case 'B':
											reg = ((reg ^ (1 << bitend)) - (1 << bitend));
											print_address_func(reg * 2 + pc + 4, JUMP_BIT);
											break;

										default:
											return 0;
										}
									}
									break;

								case '\'':
									c++;
									if ((given & (1 << bitstart)) != 0){
										sprintf(tmp, "%c", *c);
										strcat(dis_line, tmp);
									}
									break;

								case '?':
									++c;
									if ((given & (1 << bitstart)) != 0){
										sprintf(tmp, "%c", *c++);
										strcat(dis_line, tmp);
									} else {
										sprintf(tmp, "%c", *++c);
										strcat(dis_line, tmp);
									}
									break;

								default:
									return 0;
								}
							}
							break;

						default:
							return 0;
						}
					}
					else {
						sprintf(tmp, "%c", *c);
						strcat(dis_line, tmp);
					}
				}
			}
			return 2;
		}
	}

	/* No match.  */
	return 0;
}

/* NOTE: There are no checks in these routines that
the relevant number of data bytes exist.  */
int print_instruction ( unsigned char *data, unsigned int offset, void *btree, unsigned int pc, int is_thumb, void *i )
{
	long given;
	int size;
	char rbuf[512];

	fdata = data;
	foffset = offset;
	fbtree = btree;
	if (need_info)
		memset(&info, 0, sizeof(info));

	if (is_thumb)
		given = (read_hword(pc+2,i) <<16 ) | read_hword(pc,i);
	else
		given = read_word(pc,i);


	dis_line[0] = 0;
	if (is_thumb) {
		size = print_insn_thumb (pc, given,i);
	} else {
		size = print_insn_arm (pc, given,i);
	}
	if (size==2)
		sprintf(rbuf, "%08X %04X      ", pc, read_hword(pc,i) );
	else
		sprintf(rbuf, "%08X %08X  ", pc, read_word(pc,i) );
	strcat(rbuf, dis_line);
	strcpy(data, rbuf);
	return size;
}

void init_regs()
{
	int i;
	for (i=0;i<16;i++)
		reg_ok[i] = 0;
}
