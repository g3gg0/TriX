#ifndef __TRIX_ARM_C__
#define __TRIX_ARM_C__

/*! \file arm.c
 */

#include <stdlib.h>
#include <string.h>


#include "defines.h"
#include "seer.h"
#include "segment.h"
#include "stage.h"
#include "symbols.h"

#include "file_io.h"
#include "file.h"
#include "workspace.h"
#include "util.h"
#include "elf.h"
#include "arm.h"

#include "trixplug_funcs.h"
#include "mem.h"

ARM_PLUG_INIT;

//init arm
unsigned int arm_init ()
{
/*    scAdd_Internal_Header ( "arm", ARM_HEADERS );
    ARM_SYMBOLS
*/
   /*
 t_function func_test =
         {
             "new_function",
             {
                 { ".text",    "\xB5\x00\x20\x00\xF0\x00\xF8\x00\xBC\x00", 10 },
                 { ".data",    "\x00\x00\x00\x00\x00\x00\x00\x00", 8 },
                 { ".bss",     NULL, 20 },
                 { NULL,       NULL, 0 }
             },
             {
                 { 0x04, SYM_CALL, "test_callee" },
                 { 0x00, SYM_PTR,  ".bss+0x14" },
                 { 0x08, SYM_PTR,  ".data+0x04" },
                 { 0x0A, SYM_PTR,  ".data+0x08" },
                 { 0, 0, NULL }
             }
         };

   t_function func_test =
        {
            "new_function",
            ".text",    "\xB5\x00\x20\x00\xF0\x00\xF8\x00\xBC\x00", "10",
            ".data",    "\x00\x00\x00\x00\x00\x00\x00\x00", "8",
            ".bss",     "", "20",
			".reloc",   "CALL:.text+0x10@.text+0x20 PTR:.text+0x18@.data+0x00 PTR:.data+0x04@.bss+0x00", "",
			NULL
        };
	t_function func_test =
	{
		"new_function",
		".text",    "\xB5\x00\x20\x00\xF0\x00\xF8\x00\xBC\x00", "10",
		".data",    "\x00\x00\x00\x00\x00\x00\x00\x00", "8",
		".bss",     NULL, "20",
		"!SYM_CALL", ".text+0x10", ".text+0x20",
		"!SYM_PTR",  ".text+0x18", ".data+0x00",
		"!SYM_PTR",  ".data+0x04", ".bss+0x00",
		NULL
	};

		*/

    return E_OK;
}

//set B, BL, ldr, bne ...
// LDR will use r0!! same ADR
unsigned int arm_set ( t_workspace *ws, unsigned int type, unsigned int offset, unsigned int dest )
{
	return arm_set_32 ( ws, type,  offset, dest, 0x0E );
}

unsigned int arm_set_32 ( t_workspace *ws, unsigned int type, unsigned int offset, unsigned int dest, unsigned int cond_field )
{
    unsigned char condition = 0;
	unsigned int value = 0;
	unsigned int link_bit = 0;
	int distance = 0;


    if ( !ws || offset > MEM_MAX || dest > MEM_MAX )
    {
        DBG ( DEBUG_ARM, "## %s: value undefined or wrong\n", __FUNCTION__ );
        return E_FAIL;
    }

    if ( !v_valid_inline ( ws, offset ) )
    {
        DBG ( DEBUG_ARM, "## %s: source/offset (0x%08x) not in that stage available\n", __FUNCTION__, offset );
        return E_FAIL;
    }

    switch ( type )
    {
        case ARM_BLX:
        case ARM_BLX1:
			cond_field = 0x0F;
			// fall-through

        case ARM_BL:
			link_bit = 1;
			// fall-through

        case ARM_B:
			distance = dest - (offset+8);

			if ( ABS(distance) > 0x01FFFFFF  )
            {
                DBG ( DEBUG_ARM, "## %s: dest to far away\n", __FUNCTION__ );
                return E_FAIL;
            }
            distance = arm_convert_to_x_bit ( distance >> 2, 24 );

			value = (cond_field << 28);
			value |= (5 << 25);
			value |= distance;

			// for BLX set the H bit if needed
			if ( cond_field == 0x0F )
			{
				// use the higher half at this address?
				if (( dest & 0x02 ) == 0x02 )
					value |= (1 << 24);
			}
			else
			{
				value |= (link_bit << 24);
			}

			v_set_w ( ws, offset, value );

            return E_OK;
	
		case THUMB_BL:
			distance = dest - (offset+4);
            if ( ABS(distance) > 4194304 )
            {
                DBG ( DEBUG_ARM, "## %s: dest to far away\n", __FUNCTION__ );
                return E_FAIL;
            }
            dest = arm_convert_to_x_bit ( distance / 2, 22 );

			value = 0xf000 | ((dest >> 11) & 0x07FF);
			v_set_h ( ws, offset, value );

			value = 0xf800 | (dest & 0x07FF);
			v_set_h ( ws, offset+2, value );

            return E_OK;

        case THUMB_BLX:
			distance = dest - (offset+4);
            if ( ABS(distance) > 4194304 )
            {
                DBG ( DEBUG_ARM, "## %s: dest to far away\n", __FUNCTION__ );
                return E_FAIL;
            }
            dest = arm_convert_to_x_bit ( distance / 2, 22 );

			value = 0xf000 | ((dest >> 11) & 0x07FF);
			v_set_h ( ws, offset, value );

			value = 0xe800 | (dest & 0x07FF);
			v_set_h ( ws, offset+2, value );

            return E_OK;

        case THUMB_LDR:
            if ( dest < offset || dest - offset > 1024 )
            {
                DBG ( DEBUG_ARM, "## %s: dest to far away\n", __FUNCTION__ );
                return E_FAIL;
            }

			value = 0x4800 | ((dest-offset-4) >> 2);
			v_set_h ( ws, offset, value ); //can only be positive, so no arm_convert needed

            return E_OK;

        case THUMB_ADR:
            if ( dest - offset > 1024 || dest - offset < 0 )
            {
                DBG ( DEBUG_ARM, "## %s: dest to far away\n", __FUNCTION__ );
                return E_FAIL;
            }

			value = 0xA000 | ((dest-offset-4) >> 2);
			v_set_h ( ws, offset, value ); //can only be positive, so no arm_convert needed

            return E_OK;

        case THUMB_B:
            if ( ABS(dest-offset) > 2048 )
            {
                DBG ( DEBUG_ARM, "## %s: dest to far away\n", __FUNCTION__ );
                return E_FAIL;
            }
            dest = arm_convert_to_x_bit ( (dest - offset - 4)/2, 11 );

			value = 0xe000 | (dest & 0x07FF);
			v_set_h ( ws, offset, value ); 

            return E_OK;

        case THUMB_BLE:
            condition++;
        case THUMB_BGT:
            condition++;
        case THUMB_BLT:
            condition++;
        case THUMB_BGE:
            condition++;
        case THUMB_BLS:
            condition++;
        case THUMB_BHI:
            condition++;
        case THUMB_BVC:
            condition++;
        case THUMB_BVS:
            condition++;
        case THUMB_BPL:
            condition++;
        case THUMB_BMI:
            condition++;
        case THUMB_BCC:
            condition++;
        case THUMB_BCS:
            condition++;
        case THUMB_BNE:
            condition++;
        case THUMB_BEQ:
			distance = dest - (offset+4);
            if ( ABS(distance) > 256 )
            {
                DBG ( DEBUG_ARM, "## %s: dest to far away\n", __FUNCTION__ );
                return E_FAIL;
            }
            dest = arm_convert_to_x_bit ( distance/2, 8 );

			value = 0xd000 | (condition<<8) | (dest & 0x00FF);
			v_set_h ( ws, offset, value );

            return E_OK;

        default:
            return E_FAIL;

    }


    //return E_FAIL;
}

//convert 32 bit int to x bit value
/* no error chks!!!! */
INLINE_MODE
int arm_convert_to_x_bit ( int val, unsigned char bits )
{
    if ( val >= 0 )
        return val & (1 << bits - 1) - 1;
    else
        return (-val & (1 << bits - 1) - 1 ^ ( (1 << bits - 1 ) - 1 ) ) + 1 | 1 << bits - 1;

}

//convert x bit value to 32 bit int
/* no error chks!!!! */
INLINE_MODE
int arm_convert_to_int ( int arm_value, unsigned char bits )
{
    if ( arm_value & (1 << (bits-1)) ) //negative bit set
        return  -1 * (( (arm_value & (( 1 << (bits-1) ) - 1 )) - 1) ^ (( 1 << (bits-1) ) - 1 ));
    else
        return arm_value;

}

unsigned int arm_get_bl ( t_workspace *ws, unsigned int offset )
{
	unsigned int rel = 0;
	unsigned int H = 0;
	unsigned int PC = offset+4;
	unsigned int offset_11 = 0;

	// first BL/BLX instruction
	offset_11 = v_get_h ( ws, offset ) & 0x07FF;
	PC = PC + (arm_convert_to_int(offset_11,11) << 12);

	// second BL/BLX instruction
	offset_11 = v_get_h ( ws, offset + 2 ) & 0x07FF;
	H = (v_get_h ( ws, offset + 2 )>>11) & 0x03;

	switch ( H )
	{
		case 1:
			// BLX instruction
			PC = (PC + (offset_11 << 1)) & 0xFFFFFFFC;
			break;
		case 3:
			// BL instruction
			PC = PC + (offset_11 << 1);
			break;
		default:
			return E_FAIL;
	}

	return PC;
}

//get destination of b, bl, ldr, ...
unsigned int arm_get_32 ( t_workspace *ws, unsigned int offset )
{
	return arm_get_32_adv(ws, offset, ARM_DEST_VAL);
}

unsigned int arm_get_32_adv ( t_workspace *ws, unsigned int offset, unsigned int type )
{
    unsigned int data = 0x00;

    if ( !ws || !v_valid_inline ( ws, offset) || !v_valid_inline ( ws, offset+1) || !v_valid_inline ( ws, offset+2) || !v_valid_inline ( ws, offset+3) )
        return E_FAIL;

	data = v_get_w ( ws, offset );

	/* B/BL */
	if((data & 0x0E000000) == 0x0A000000)
	{
		unsigned int PC = offset + 8;
		unsigned int offset_24 = 0;
		offset_24 = data & 0x00FFFFFF;
		PC = PC + 4 * arm_convert_to_int(offset_24,24);

		return PC;
	}

	/* MOV w/ imm */
	if((data & 0x0FE00000) == 0x03A00000)
	{
		unsigned int reg = (data >> 12) & 0x0F;
		unsigned int op = data & 0xFFF;
		unsigned int val = 0;

		unsigned int ror = op >> 8;
		val = op & 0xFF;

		while(ror)
		{
			val = (val >> 2) | ((val & 3) << 30);
			ror--;
		}

		if(type == ARM_REG_RD)
		{
			return reg;
		}
		return val;
	}

	/* ADD Rx, PC, val  = ADR R2, <symbol> */
	if((data & 0x0FFF0000) == 0x028F0000)
	{
		unsigned int PC = offset + 8;
		unsigned int reg = (data >> 12) & 0x0F;
		unsigned int op = data & 0xFFF;
		unsigned int val = 0;

		unsigned int ror = op >> 8;
		val = op & 0xFF;
		while(ror)
		{
			val = (val >> 2) | ((val & 3) << 30);
			ror--;
		}

		if(type == ARM_REG_RD)
		{
			return reg;
		}
		return PC + val;
	}

	/* LDR Rd, [PC, val]   LDR Rd, =(value)   (immediate offset/index) */
	if((data & 0x0E1F0000) == 0x041F0000)
	{
		unsigned int PC = offset + 8;
		unsigned int reg = (data >> 12) & 0x0F;
		unsigned int offset = data & 0xFFF;
		unsigned int add = (data >> 23) & 1;
		unsigned int width = (data >> 22) & 1;
		int val = 0;

		if(type == ARM_REG_RD)
		{
			return reg;
		}

		if(add)
		{
			val = offset;
		}
		else
		{
			val = -offset;
		}

		if(width)
		{
			return v_get_b ( ws, PC + val );
		}
		else
		{
			return v_get_w ( ws, PC + val );
		}
	}

	/* SUB Rx, PC, val  = ADR R2, <symbol> */
	if((data & 0x0FFF0000) == 0x024F0000)
	{
		unsigned int PC = offset + 8;
		unsigned int reg = (data >> 12) & 0x0F;
		unsigned int op = data & 0xFFF;
		unsigned int val = 0;

		unsigned int ror = op >> 8;
		val = op & 0xFF;
		while(ror)
		{			
			val = (val >> 2) | ((val & 3) << 30);
			ror--;
		}

		if(type == ARM_REG_RD)
		{
			return reg;
		}
		return PC - val;
	}
	return E_FAIL;
}

//get destination of b, bl, ldr, ...
unsigned int arm_get ( t_workspace *ws, unsigned int offset )
{
    unsigned char byte_1 = 0x00;
    unsigned char byte_2 = 0x00;

    if ( !ws || !v_valid_inline ( ws, offset) || !v_valid_inline ( ws, offset+1) )
        return E_FAIL;

	byte_1 = v_get_h ( ws, offset ) >> 8;
	byte_2 = v_get_h ( ws, offset ) & 0xFF;

	switch ( byte_1 & 0xF8 )
    {
        case 0xD0:  //cond branch
        case 0xD8:
            if ( ( byte_1 & 0xF ) == 0xF )
                return E_FAIL;  //Software Int (SWI)
            if ( ( byte_1 & 0xE ) == 0xE )
                return E_FAIL;  //undefined. i thought about a printf to congratulate the user :-)
            /*TODO if ( GET_BYTE ( t, offset + 1 ) == E_FAIL )
                return E_FAIL;*/ // first a nice get_byte is needed
            return offset + arm_convert_to_int ( byte_2<<1, 9 ) + 4;     //+4 cpu prefetch PC
        case 0xE0:  //branch
            //check if byte available
            return offset + arm_convert_to_int ( (( (byte_1&7)<<8 ) + byte_2)<<1, 12 ) + 4;
        case 0xF0:  //branch with link
            //check if bytes available
            return arm_get_bl  ( ws, offset );
        case 0x48:  //LDR
        case 0xA0:  //ADR
            return ( ( offset + 4 ) & ~3 ) + ( byte_2 << 2 ); //can only be positive!
        default:
            return E_FAIL;
    }

	return E_FAIL;
}

//get destination of b, bl, ldr, ...
unsigned int arm_get_adv ( t_workspace *ws, unsigned int offset, unsigned int type )
{
	switch(type)
	{
		case THUMB_OPCODE:
			return arm_get(ws, offset);
		case ARM_OPCODE:
			return arm_get_32(ws, offset);
	}

	return E_FAIL;
}

//find next b, bl, ldr opcode
unsigned int arm_find ( t_workspace *ws, unsigned int type, unsigned int start, unsigned int end, unsigned int direction )
{
    t_locator loc;
    int subs = 0;
	int pos = 0;

	// big endian templates
	t_locator arm_loc_b =
    {
        "arm: ARM_B search (BE)",
        "\xEA\x00\x00\x00",
        "\x0F\x00\x00\x00",
        4
    };
	t_locator arm_loc_bl =
    {
        "arm: ARM_BL search (BE)",
        "\xEB\x00\x00\x00",
        "\x0F\x00\x00\x00",
        4
    };
	t_locator arm_loc_blx1 =
    {
        "arm: ARM_BLX1 search (BE)",
        "\xFA\x00\x00\x00",
        "\x0E\x00\x00\x00",
        4
    };
	t_locator arm_loc_blx2 =
    {
        "arm: ARM_BLX2 search (BE)",
        "\xE1\x20\x00\x00",
        "\x0F\xF0\x00\x00",
        4
    };
	t_locator arm_loc_adr =
    {
        "arm: ARM_ADR search (BE)",
        "\x02\x0F\x00\x00",
        "\x0F\x3F\x00\x00",
        4
    };
	t_locator arm_loc_ldr =
    {
        "arm: ARM_LDR search (BE)",
        "\x04\x1F\x00\x00",
        "\x0E\x1F\x00\x00",
        4
    };
	t_locator arm_loc_mov =
    {
        "arm: ARM_MOV search (BE)",
        "\x03\xA0\x00\x00",
        "\x0F\xE0\x00\x00",
        4
    };


	t_locator thumb_loc_bl =
    {
        "arm: THUMB_BL search (BE)",
        "\xF0\x00\xF8\x00",
        "\xF8\x00\xF8\x00",
        4
    };
	t_locator thumb_loc_blx =
    {
        "arm: THUMB_BLX search (BE)",
        "\xF0\x00\xE8\x00",
        "\xF8\x00\xF8\x00",
        4
    };
	t_locator thumb_loc_b =
    {
        "arm: THUMB_B search (BE)",
        "\xE0\x00",
        "\xF8\x00",
        2
    };
    t_locator thumb_loc_bcond =
    {
        "arm: THUMB_B<cond>search (BE)",
        "\xFF\xFF",
        "\xFF\x00",
        2
    };
    t_locator thumb_loc_ldr =
    {
        "arm: THUMB_LDR search (BE)",
        "\x48\x00",
        "\xF8\x00",
        2
    };
    t_locator thumb_loc_adr =
    {
        "arm: THUMB_ADR search (BE)",
        "\xA0\x00",
        "\xF8\x00",
        2
    };
    unsigned char b_cond_search[3];
    unsigned char condition = 0;
	unsigned char *loc_tempbuffer = NULL;

	/* defaulting to THUMB for backward compatibility */
	if((type & ARM_THUMB_MASK) == 0)
	{
		type |= THUMB_OPCODE;
	}

    //DBG ( DEBUG_ARM, "## %s() called\n", __FUNCTION__ );
    if ( !ws )
    {
        DBG ( DEBUG_ARM, "## %s: value undefined or wrong\n", __FUNCTION__ );
        return E_FAIL;
    }

	switch ( type )
	{
		case ARM_B:
			loc = arm_loc_b;
			break;
		case ARM_BL:
			loc = arm_loc_bl;
			break;
		case ARM_BLX:
		case ARM_BLX1:
			loc = arm_loc_blx1;
			break;
		case ARM_BLX2:
			loc = arm_loc_blx2;
			break;
		case ARM_ADR:
			loc = arm_loc_adr;
			break;
		case ARM_LDR:
			loc = arm_loc_ldr;
			break;
		case ARM_MOV:
			loc = arm_loc_mov;
			break;

		case THUMB_BL:
			loc = thumb_loc_bl;
			break;
		case THUMB_BLX:
			loc = thumb_loc_blx;
			break;
		case THUMB_B: //11100xxx...
			loc = thumb_loc_b;
			break;
		case THUMB_LDR:
			loc = thumb_loc_ldr;
			break;
		case THUMB_ADR:
			loc = thumb_loc_adr;
			break;
		case THUMB_BLE:
			condition++;
		case THUMB_BGT:
			condition++;
		case THUMB_BLT:
			condition++;
		case THUMB_BGE:
			condition++;
		case THUMB_BLS:
			condition++;
		case THUMB_BHI:
			condition++;
		case THUMB_BVC:
			condition++;
		case THUMB_BVS:
			condition++;
		case THUMB_BPL:
			condition++;
		case THUMB_BMI:
			condition++;
		case THUMB_BCC:
			condition++;
		case THUMB_BCS:
			condition++;
		case THUMB_BNE:
			condition++;
		case THUMB_BEQ:
			b_cond_search[0] = b_cond_search[2] = 0x00;
			b_cond_search[1] = 0xD0 | condition;
			loc = thumb_loc_bcond;
			loc.pattern = b_cond_search;
			break;
		default:
			return E_FAIL;
	}

	/* build a temporary buffer for mask and pattern for patching endianess */
	loc_tempbuffer = malloc(2 * loc.length);
	memcpy(loc_tempbuffer, loc.pattern, loc.length);
	memcpy(loc_tempbuffer + loc.length, loc.mask, loc.length);
	loc.pattern = loc_tempbuffer;
	loc.mask = loc_tempbuffer + loc.length;
		
	/* swap halfs/words */
	if ( ws->flags & FLAGS_ENDIANESS_LE )
	{
		switch(type & ARM_THUMB_MASK)
		{
			case THUMB_OPCODE:
				if(loc.length % 2)
				{
					DBG ( DEBUG_ARM, "## %s: invalid pattern length\n", __FUNCTION__ );
					return E_FAIL;
				}
				for(pos = 0; pos < loc.length; pos += 2)
				{
					unsigned short *ptr;
					ptr = &loc.pattern[pos];
					*ptr = util_swap_half(*ptr);
					ptr = &loc.mask[pos];
					*ptr = util_swap_half(*ptr);
				}
				break;

			case ARM_OPCODE:
				if(loc.length % 4)
				{
					DBG ( DEBUG_ARM, "## %s: invalid pattern length\n", __FUNCTION__ );
					return E_FAIL;
				}
				for(pos = 0; pos < loc.length; pos += 4)
				{
					unsigned int *ptr;
					ptr = &loc.pattern[pos];
					*ptr = util_swap_word(*ptr);
					ptr = &loc.mask[pos];
					*ptr = util_swap_word(*ptr);
				}
				break;
		}
	}

/*
BEQ label BEQ label Branch if Z set (equal)
0001 BNE label BNE label Branch if Z clear (not equal)
0010 BCS label BCS label Branch if C set (unsigned higher or
same)
0011 BCC label BCC label Branch if C clear (unsigned lower)
0100 BMI label BMI label Branch if N set (negative)
0101 BPL label BPL label Branch if N clear (positive or zero)
0110 BVS label BVS label Branch if V set (overflow)
0111 BVC label BVC label Branch if V clear (no overflow)
1000 BHI label BHI label Branch if C set and Z clear
(unsigned higher)
1001 BLS label
1010 BGE label BGE label Branch if N set and V set, or N
clear and V clear (greater or
equal)
1011 BLT label BLT label Branch if N set and V clear, or N
clear and V set (less than)
1100 BGT label BGT label Branch if Z clear, and either N set
and V set or N clear and V clear
(greater than)
1101 BLE label BLE label
*/
    start--;
    
    if ( direction & LOC_BACKWARD )
       subs = -1;
    else
       subs = 1;

    do
    {
        start = util_find_pattern ( ws, &loc, start + subs, end, direction );
        /*if ( start % 2 )
            printf ( "not modulo 2" );*/
    } while ( start != E_FAIL && start % 2 );


	free(loc_tempbuffer);

    return start;
}

//gets next free space
unsigned int arm_get_freespace ( t_workspace *ws, unsigned int size, unsigned int offset )
{
    t_locator loc;
    unsigned int ret_ofs = 0;

	if ( !ws || size < 1 || offset > MEM_MAX )
        return E_FAIL;

    if ( !v_valid_inline ( ws, offset ) && offset != MEM_AUTO )
    {
        DBG ( DEBUG_ARM, "## %s: wrong address\n", __FUNCTION__ );
        return E_FAIL;
    }

    size = size < MIN_FREESPACE ? MIN_FREESPACE : size;

    size++;     //add 1, so we also can increase the return val if not %2

    size = size % 2 ? size+1 : size;

    loc.func_name = "arm: freespace search";
    loc.pattern = (unsigned char*) malloc ( size );
    loc.mask = (unsigned char*) malloc ( size );
    loc.length = size;
    loc.options = 0;

    memset ( loc.pattern, 0xFF, size );
    memset ( loc.mask, 0xFF, size );

    ret_ofs = util_find_pattern( ws, &loc, 0, offset, LOC_FORWARD);
    free ( loc.pattern );
    free ( loc.mask );
    return ret_ofs == E_FAIL ? E_FAIL : (ret_ofs % 2 ? ret_ofs+1 : ret_ofs);
}

//searches for a BL/B/B<cond> to dest. starting at start
unsigned int arm_find_src_of_dest ( t_workspace *ws, int type, unsigned int dest, unsigned int start, unsigned int end, unsigned int direction )
{
    unsigned int loc = 0;
    int subs = 0;
    unsigned int DBG_count = 0;
	char pattern[4];

    if ( !ws || ( !v_valid_inline( ws, start ) && start != MEM_AUTO )|| ( !v_valid_inline( ws, end-1 ) && end != MEM_AUTO ) )
        return E_FAIL;

    DBG ( DEBUG_ARM, "## %s() called\n", __FUNCTION__ );

	/* look for an offset pointing to dest */
	if ( type == ADDRESS )
	{
		if ( ws->flags & FLAGS_ENDIANESS_LE )
		{
			pattern[3] = (dest>>24) & 0xFF;
			pattern[2] = (dest>>16) & 0xFF;
			pattern[1] = (dest>>8) & 0xFF;
			pattern[0] = (dest>>0) & 0xFF;
		}
		else
		{
			pattern[0] = (dest>>24) & 0xFF;
			pattern[1] = (dest>>16) & 0xFF;
			pattern[2] = (dest>>8) & 0xFF;
			pattern[3] = (dest>>0) & 0xFF;
		}
		return util_quick_find_pattern ( ws, pattern, NULL, 4, start, end, 0 );
	}
	start--;

    if ( direction & LOC_BACKWARD )
       subs = -1;
    else
       subs = 1;

    do
    {
		start = arm_find( ws, type, start + subs, end, direction );
		loc = arm_get_adv ( ws, start, type & ARM_THUMB_MASK );
        DBG_count++;
    } while ( v_valid ( ws, start ) && loc != E_FAIL && loc != dest && ( start < end || end == MEM_AUTO ) );

	/* why check for (loc != dest)?  this could cause a lockup. */

    DBG ( DEBUG_ARM, "## %s: found %d types, src<type>(@0x%08X)->dest\n", __FUNCTION__,DBG_count, start );

    return start;
}


//create a function, offset could be MEM_AUTO
// struct or array ?
/* better use nokia_create_func ! */
unsigned int arm_create_func ( t_workspace *ws, char *routine, unsigned int offset )
{
    return E_FAIL;
}

/*
    TODO:
    - same routines with arm_s_* for use without stage
    - reserve ram, reserve eeprom ????
        should be seperated, to be more general (DCT3, DCT4)
        maybe as seer script ?
*/

/*!
 * invokes make and creates an object from the .o file
 * \param makefile_path the path to the makefile excluding last slash. relative to TriX program file.
 * \param object_file the name of the object created by the makefile
 * \return NULL on failure or pointer to object. You have to free it with util_free_object!
 */
object *arm_create_object_from_makefile ( const char *makefile_path, const char *object_file )
{
    t_workspace *ws = NULL;
	t_fileinfo *fi = NULL;
	char *buff = NULL;
	object *obj = NULL;

	buff = (char*)malloc ( 200 + strlen(makefile_path) );

	fi = file_io_create_empty ();
	if ( !fi || !buff )
		return NULL;

	sprintf ( buff, "" );
	strcat ( buff, "# set our tools at first position in path\n" );
	strcat ( buff, "export PATH=`pwd`/tools/arm:$PATH\n" );
	//strcat ( buff, "echo $PATH\n" );

	strcat ( buff, "cd " );
	strcat ( buff, makefile_path );
	strcat ( buff, "\n" );

	strcat ( buff, "rm *.o >/dev/null 2>&1\n" );
	strcat ( buff, "make >err.log 2>&1\n" );

	fi->stages->segments->length = strlen ( buff );
	fi->stages->segments->data = (char*)strdup ( buff );


	if ( file_io_write ( "run.sh", fi ) != E_OK )
	{
		DBG ( DEBUG_ARM, "## %s: file write failed\n", __FUNCTION__ );
		return NULL;
	}

	system ( "tools\\arm\\bash.exe run.sh" );

	sprintf ( buff, "%s/%s", makefile_path, object_file );
	ws =  workspace_startup ( buff );
	if ( !ws )
		return NULL;

	free ( buff );

	obj = elf_create_object ( ws );

	file_io_release_all ( fi );
	file_io_release_all ( ws->fileinfo );
	free ( ws );

	return obj;

}


/*!
 * invokes make and creates objects from the .o file
 * \param makefile_path the path to the makefile excluding last slash. relative to TriX program file.
 * \return E_FAIL on failure or E_OK if everything was okay
 */
unsigned int arm_run_makefile ( const char *makefile_path )
{
    t_workspace *ws = NULL;
	t_fileinfo *fi = NULL;
	char *buff = NULL;

	buff = (char*)malloc ( 400 + strlen(makefile_path) );

	fi = file_io_create_empty ();
	if ( !fi || !buff )
		return E_FAIL;

	sprintf ( buff, "" );
	strcat ( buff, "export PATH=`pwd`/tools/arm:$PATH\n" );
	strcat ( buff, "cd " );
	strcat ( buff, makefile_path );
	strcat ( buff, "\n" );
	strcat ( buff, "rm *.o >/dev/null 2>&1\n" );
	strcat ( buff, "make >err.log 2>&1\n" );

	fi->stages->segments->length = strlen ( buff );
	fi->stages->segments->data = (char*)strdup ( buff );


	if ( file_io_write ( "run.sh", fi ) != E_OK )
	{
		DBG ( DEBUG_ARM, "## %s: file write failed\n", __FUNCTION__ );
		return E_FAIL;
	}

	system ( "tools\\arm\\bash.exe run.sh" );
//	printf ( "test" );

	return E_OK;
}

/*!
 * creates an object from the .o file
 * \param makefile_path the path to the makefile excluding last slash. relative to TriX program file.
 * \param object_file the name of the object created by the makefile
 * \return NULL on failure or pointer to object. You have to free it with util_free_object!
 */
object *arm_create_object ( const char *makefile_path, const char *object_file )
{
    t_workspace *ws = NULL;
	t_fileinfo *fi = NULL;
	char *buff = NULL;
	object *obj = NULL;

	buff = (char*)malloc ( 200 + strlen(makefile_path) );

	sprintf ( buff, "%s/%s", makefile_path, object_file );
	ws =  workspace_startup ( buff );
	if ( !ws )
		return NULL;

	free ( buff );

	obj = elf_create_object ( ws );

	file_io_release_all ( ws->fileinfo );
	free ( ws );

	return obj;

}




#endif
