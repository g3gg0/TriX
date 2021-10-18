#define STANDARD_INSTRUCTION_DECODING\
	u32 lenght; \
	strcpy (dest, opcode_strings [((op&0xFF00000)>>16)|((op&0xF0)>>4)]);\
	strcat (dest, condition_strings [(op>>28)&0xF]);\
	lenght = strlen(dest);\
	while (lenght<15) {\
		strcat (dest, " ");\
		lenght++;\
	}

void second_operand (u32 op, u32 adress, char *dest)
{
	u32 temp;
	u32 shift_amount;
	char string [33], string2 [33];

	if (op&0x2000000) {
		shift_amount = ((op>>8)&0xF)<<1;								
		temp = ((op&0xFF)<<(32-shift_amount))|((op&0xFF)>>shift_amount);
		strcat (dest, "#0x");
		itoa(temp, string, 16);
		strcat (dest, string);
	}
	else {
		strcat (dest, register_strings[(op&0xF)]);
		strcpy (string, ", ");
		strcat (string, shift_strings[(op>>5)&0x3]);
		if (op&0x10) {
			strcat (string, " ");
			strcat (string, register_strings[(op>>7)&0xF]);
			strcat (dest, string);
		}
		else {
			temp = (op>>7)&0x1F;
			if (!temp) {
				switch ((op>>5)&0x3) {
					case 0: break;
					case 1: strcpy (string2, ", ");
							strcat (string2, "LSR #32");
							strcat (dest, string2);
							break;
					case 2: 
							strcpy (string2, ", ");
							strcat (string2, "ASR #32");
							strcat (dest, string2);
							break;
					case 3: strcpy (string2, ", ");
							strcat (string2, "RRX");
							strcat (dest, string2);
							break;
				}
			}else {
				strcat (string, " #");
				itoa(temp, string2, 10);
				strcat (string, string2);
				strcat (dest, string);
			}
		}
	}
}

void standard_debug_handle (u32 op, u32 adress, char *dest)
{
	STANDARD_INSTRUCTION_DECODING;
}

void b_debug_handle (u32 op, u32 adress, char *dest)
{
	u32 temp;
	char string [33];

	STANDARD_INSTRUCTION_DECODING;

	strcat (dest, "\t0x");
	if (op&0x800000)
		temp = adress + (((op&0xFFFFFF)<<2)-0x4000000) + 8;
	else
		temp = adress + ((op&0x7FFFFF)<<2) + 8;
	itoa (temp, string, 16);	
	strcat (dest, string);
}

void dpsingle_debug_handle (u32 op, u32 adress, char * dest)
{
	STANDARD_INSTRUCTION_DECODING;	

	strcat (dest, "\t");
	strcat (dest, register_strings[(op>>12)&0xF]);
	strcat (dest, ", ");
	
	second_operand (op, adress, dest);
}

void dpnw_debug_handle (u32 op, u32 adress, char * dest)
{
	STANDARD_INSTRUCTION_DECODING;
	
	strcat (dest, "\t");
	strcat (dest, register_strings[(op>>16)&0xF]);
	strcat (dest, ", ");

	second_operand (op, adress, dest);
}

void dp_debug_handle (u32 op, u32 adress, char * dest)
{
	STANDARD_INSTRUCTION_DECODING;	

	strcat (dest, "\t");
	strcat (dest, register_strings[(op>>12)&0xF]);
	strcat (dest, ", ");
	strcat (dest, register_strings[(op>>16)&0xF]);
	strcat (dest, ", ");
	
	second_operand (op, adress, dest);
}

void mul_debug_handle (u32 op, u32 adress, char *dest)
{
	STANDARD_INSTRUCTION_DECODING;	

	strcat (dest, "\t");
	strcat (dest, register_strings[(op>>16)&0xF]);
	strcat (dest, ", ");
	strcat (dest, register_strings[op&0xF]);
	strcat (dest, ", ");
	strcat (dest, register_strings[(op>>8)&0xF]);
	if (op&0x200000) {
		strcat (dest, ", ");
		strcat (dest, register_strings[(op>>12)&0xF]);
	}
}

void mull_debug_handle (u32 op, u32 adress, char *dest)
{
	STANDARD_INSTRUCTION_DECODING;
}

void sdt_debug_handle (u32 op, u32 adress, char *dest)
{
	u32 temp;
	char string [33], string2 [33];
	
	STANDARD_INSTRUCTION_DECODING;
	
	strcat (dest, "\t");
	strcat (dest, register_strings[(op>>12)&0xF]);
		
	if (op&0x2000000) {
		strcat (dest, ", [");
		strcat (dest, register_strings[(op>>16)&0xF]);
		if (!(op&0x1000000)) strcat (dest, "]");
		strcat (dest, ", ");
		strcat (dest, register_strings[(op&0xF)]);
		strcpy (string, ", ");
		strcat (string, shift_strings[(op>>5)&0x3]);

		temp = (op>>7)&0x1F;
		
		if (!temp) {
			switch ((op>>5)&0x3) {
				case 0: break;
				case 1: strcpy (string2, ", "); strcat (string2, "LSR #32"); strcat (dest, string2);
						break;
				case 2: strcpy (string2, ", "); strcat (string2, "ASR #32"); strcat (dest, string2);
						break;
				case 3: strcpy (string2, ", "); strcat (string2, "RRX"); strcat (dest, string2);
						break;
			}
			
		}else {
			strcat (string, " #");
			itoa(temp, string2, 10);
			strcat (string, string2);
			strcat (dest, string);
		}
 
		if ((op&0x1000000)) {
			strcat (dest, "]");
			if (op&0x200000) strcat(dest, "!");
		}
	}
	else {
		temp = (op&0xFFF);
		if (temp) {
			if ((((op>>16)&0xF)==15)&&(op&0x100000)) {
				strcat (dest, ", =0x");
				if (op&0x800000) temp = adress+8+temp; else temp = adress+8-temp;
				if (op&0x400000) temp = (u32)read_byte (temp); else temp = read_word (temp);
				itoa (temp, string, 16);
				strcat (dest, string);
			}else {
				strcat (dest, ", [");
				strcat (dest, register_strings[(op>>16)&0xF]);
				if (!(op&0x1000000))strcat (dest, "]");
				if (op&0x800000) strcat (dest, ", #"); else strcat (dest, ", -#");
				itoa(temp, string, 10);
				strcat (dest, string);
				if ((op&0x1000000)) {
					strcat(dest , "]");
					if (op&0x200000) strcat(dest, "!");
				}
			}
		}
		
		else {
			strcat (dest, ", [");
			strcat (dest, register_strings[(op>>16)&0xF]);
			strcat (dest, "]");
		}
	}
}            

void hwdt_debug_handle (u32 op, u32 adress, char *dest) 
{
	STANDARD_INSTRUCTION_DECODING;
}

void mdt_debug_handle (u32 op, u32 adress, char *dest)
{
	STANDARD_INSTRUCTION_DECODING;
}

void msr_debug_handle (u32 op, u32 adress, char *dest)
{
	STANDARD_INSTRUCTION_DECODING;
}

void mrs_debug_handle (u32 op, u32 adress, char *dest)
{
	STANDARD_INSTRUCTION_DECODING;
}

void swp_debug_handle (u32 op, u32 adress, char *dest)
{
	STANDARD_INSTRUCTION_DECODING;
}

void swi_debug_handle (u32 op, u32 adress, char *dest)
{
	STANDARD_INSTRUCTION_DECODING;
}
