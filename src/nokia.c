#ifndef __TRIX_NOKIA_C__
#define __TRIX_NOKIA_C__

/*! \file nokia.c
 */

#include "string.h"

#include "defines.h"
#include "seer.h"
#include "segment.h"    //for v_set_byte
#include "stage.h"
#include "symbols.h"

#include "file_io.h"
#include "file.h"
#include "workspace.h"
#include "util.h"
#include "arm.h"
#include "nokia.h"

#include "trixplug_funcs.h"
#include "mem.h"

NOKIA_PLUG_INIT;


unsigned int
nokia_init ( )
{
    return E_OK;
}

unsigned int
nokia_reserve_ram ( unsigned int size )
{
    //store ram as symbol ? in dct3/4 file ?
    return E_FAIL;
}

unsigned int
nokia_release_ram ( unsigned int id )
{
    return E_FAIL;
}

unsigned int
nokia_reserve_eeprom ( unsigned int size )
{
    return E_FAIL;
}

unsigned int
nokia_release_eeprom ( unsigned int id )
{
    return E_FAIL;
}


unsigned int
nokia_is_section_name_relative ( unsigned char *name )
{
	if ( strchr ( name, '+') )
		return 1;

	return 0;
}

unsigned int
nokia_get_section_dest ( t_workspace *ws, unsigned char *name )
{
	unsigned int value = 0;
	unsigned int offset = E_FAIL;
	t_symbols *sym = NULL;
	unsigned char *tempstr = NULL;

	if ( !strncmp ( name , ".", 1 ) )
	{
		// to convert ".rodata.str1.4+0x00" to ".rodata.str1.4"
		tempstr = strdup ( name );
		if ( strchr ( tempstr, '+' ) )
			*(strchr(tempstr,'+')) = 0x00;

		// find that symbol
		offset = symbols_get_address ( NULL, tempstr );
		if ( offset == E_FAIL )
			return E_FAIL;

		if ( nokia_is_section_name_relative ( name ) )
		{
			// it is a relative one
			value = util_str2int ( strchr ( name, '+' ) + 1 );
			if ( value == E_FAIL )
				return E_FAIL;
			offset += value;
		}

		return offset;
	}
	else if ( symbols_get_address ( NULL, name ) != E_FAIL )
		return symbols_get_address ( NULL, name );
	else if ( util_str2int ( name ) != E_FAIL )
		return util_str2int ( name );

	return E_FAIL;
}

unsigned int
nokia_export_segments ( t_workspace *ws, object *func )
{
	int i = 0;
	unsigned int offset = 0;
	unsigned char *sym_name = NULL;
	unsigned char *tmpstr = NULL;
	unsigned char str16[16];
	
    while ( sym_name = util_block_get_val_ptr ( func, "!SYM_EXP", i ) )
    {
        //calc offset
        tmpstr = util_block_get_offset_ptr ( func, "!SYM_EXP", i );
		offset = nokia_get_section_dest ( ws, tmpstr );

		if ( offset == E_FAIL )
        {
            DBG ( DEBUG_NOKIA,  "## %s: we didn't find %s or its offset is wrong\n", __FUNCTION__, tmpstr );
            return E_FAIL;
        }

        sprintf ( str16, "0x%08X", offset );
        symbols_add ( NULL, sym_name, SYM_PTR, str16 );

        i++;
    }
	if ( i )
		return E_OK;

	return E_FAIL;
}

unsigned int
nokia_fill_section_data ( t_workspace *ws, object *func, unsigned int offset, unsigned int base )
{
	int i = 0;
	unsigned int size = 0;
	unsigned int pos = 0;
	unsigned int len = 0;
	unsigned char str16[16];

    for ( i = 0; i < util_block_get_segment_count ( func ); i++ )
    {
        // get size of segment
        size = util_block_get_number ( func, util_block_get_segment_name_ptr ( func, i ), 0 );
        if ( size == E_FAIL )
        {
            DBG ( DEBUG_NOKIA, "## %s: maybe object struct faulty: %s\n", __FUNCTION__, util_block_get_segment_name_ptr ( func, i ) );
            return E_FAIL;
        }
        if ( size % 2 )
            size++;

        pos = base + offset + len;

        DBG ( DEBUG_NOKIA, "## %s(): %s trying to create of %s at 0x%08X\n", __FUNCTION__, util_block_get_segment_name_ptr ( func, i ), util_block_get_val_ptr ( func, "name", 0 ), pos );
        
		//create the segment there
        v_memcpy_put ( ws, pos - base, util_block_get_val_ptr ( func, util_block_get_segment_name_ptr ( func, i ), 0 ), util_block_get_number ( func, util_block_get_segment_name_ptr ( func, i ), 0 ) );

        DBG ( DEBUG_NOKIA, "## %s(): %s created of %s at 0x%08X\n", __FUNCTION__, util_block_get_segment_name_ptr ( func, i ), util_block_get_val_ptr ( func, "name", 0 ), pos );

        sprintf ( str16, "0x%08X", pos );	 
		symbols_add ( NULL, strdup ( util_block_get_segment_name_ptr ( func, i ) ), SYM_DATA, str16 );

        len += size;
    }

	return len;
}


/*!
 * Creates one or more functions in the MCU. Segments with the same name (e.g. .text and .text) are NOT supported!
 * \param func an object, with one or more functions. Could also include relocatable data. See object for an example.
 * \param offset address where the function(s) should be created. Could be MEM_AUTO. Use !SYM_EXP if you want to export more than one Symbol.
 * \param ws current workspace
 * \param base base address for absolute addressing opcodes
 * \return E_FAIL or number of modified bytes
 */
unsigned int
nokia_create_func2 ( t_workspace *ws, object *func, unsigned int offset, unsigned int base )
{

    // maybe todo in the future: split groups of similiar algorithm to own functions (SYM_PTR, SYM_CALL, SSYM_EXP)
    unsigned int text_pos;
    //int text_len;
    unsigned int len = 0;
    unsigned int tmpint = 0;
    unsigned int i = 0;
	unsigned int destination = 0;
	char *tempstr = NULL;
    char *sym_name = NULL, *tmpstr = NULL;
    char str16[16];

    if ( !ws || !func )
        return E_FAIL;

    if ( !v_valid_inline ( ws, offset ) && offset != MEM_AUTO )
    {
        DBG ( DEBUG_NOKIA, "## %s: address not in file\n", __FUNCTION__ );
        return E_FAIL;
    }

    if ( util_block_get_segment_count ( func ) == E_FAIL )
    {
        DBG ( DEBUG_NOKIA, "## %s: non legal object\n", __FUNCTION__ );
        return E_FAIL;
    }


    //set correct patching addr
    if ( offset == MEM_AUTO )
    {
        offset = arm_get_freespace ( ws, tmpint, offset );
        if ( offset == E_FAIL )
        {
            DBG ( DEBUG_NOKIA, "## %s : getting free space failed\n", __FUNCTION__ );
            return E_FAIL;
        }
    }

    // handle all SYM_DATA's (data sections)
	len = nokia_fill_section_data ( ws, func, offset, base );
	if ( len == E_FAIL )
	{
        DBG ( DEBUG_NOKIA, "## %s: failed filling segment data\n", __FUNCTION__ );
		return E_FAIL;
	}

    // handle all !SYM_EXP's (exports)
	if ( nokia_export_segments ( ws, func ) == E_FAIL )
	{
		//if no symbol was exported, export with start offset in file
		tmpstr = util_block_get_val_ptr ( func, "name", 0 );
		if ( !tmpstr )
			return E_FAIL;

		sprintf ( str16, "0x%08x", offset );
		symbols_add( NULL, tmpstr, SYM_PTR, str16 );
	}


    // handle all SYM_CALL's
    DBG ( DEBUG_NOKIA, "## %s(): handling all !SYM_CALL's\n", __FUNCTION__ );
    i = 0;
    while ( sym_name = util_block_get_val_ptr ( func, "!SYM_CALL", i ) )
    {
        //calc offset
        tmpstr = util_block_get_offset_ptr ( func, "!SYM_CALL", i );
        DBG ( DEBUG_NOKIA, "## %s(): calcing SYM_CALL %s -> %s\n", __FUNCTION__, sym_name, tmpstr );

		offset = nokia_get_section_dest ( ws, tmpstr );
		if ( offset == E_FAIL )
        {
            DBG ( DEBUG_NOKIA,  "## %s: didn't find %s or its offset is wrong\n", __FUNCTION__, tmpstr );
            return E_FAIL;
        }

		destination = nokia_get_section_dest ( ws, sym_name );
		if ( destination == E_FAIL )
        {
            printf (  "## %s: destination %s not found/is not a number\n", __FUNCTION__, sym_name );
            return E_FAIL;
        }

        DBG ( DEBUG_NOKIA,  "## %s: SYM_CALL 0x%08X -> 0x%08X calced\n", __FUNCTION__, offset, destination );

		//
		// subtract base (e.g. 0xDEAD0000) from both, so we have instead  of:
		//   0xDEAD0010 -> 0xDEAD0100
		// this one:
		//   0x00000010 -> 0x00000100
		//
        if ( arm_set ( ws, THUMB_BL, offset - base, destination - base ) == E_FAIL )
        {
            DBG ( DEBUG_NOKIA, "## %s: BL not created!!!\n", __FUNCTION__ );
            return E_FAIL;
        }

        i++;
    }

	DBG ( DEBUG_NOKIA, "## %s(): handling all !SYM_ARM_CALL's\n", __FUNCTION__ );
	i = 0;
    while ( sym_name = util_block_get_val_ptr ( func, "!SYM_ARM_CALL", i ) )
    {
        //calc offset
        tmpstr = util_block_get_offset_ptr ( func, "!SYM_ARM_CALL", i );
        DBG ( DEBUG_NOKIA, "## %s(): calcing SYM_ARM_CALL %s -> %s\n", __FUNCTION__, sym_name, tmpstr );

		offset = nokia_get_section_dest ( ws, tmpstr );
		if ( offset == E_FAIL )
        {
            DBG ( DEBUG_NOKIA,  "## %s: didn't find %s or its offset is wrong\n", __FUNCTION__, tmpstr );
            return E_FAIL;
        }

		destination = nokia_get_section_dest ( ws, sym_name );
		if ( destination == E_FAIL )
        {
            printf (  "## %s: destination %s not found/is not a number\n", __FUNCTION__, sym_name );
            return E_FAIL;
        }

        DBG ( DEBUG_NOKIA,  "## %s: SYM_ARM_CALL 0x%08X -> 0x%08X calced\n", __FUNCTION__, offset, destination );
		
		// ARM32 BL
		arm_set_32 ( ws, ARM_BL, offset - base, destination - base, v_get_w (ws,offset-base)>>28 );
        i++;
    }


	DBG ( DEBUG_NOKIA, "## %s(): handling all !SYM_ARM_BRANCH's\n", __FUNCTION__ );
	i = 0;
    while ( sym_name = util_block_get_val_ptr ( func, "!SYM_ARM_BRANCH", i ) )
    {
        //calc offset
        tmpstr = util_block_get_offset_ptr ( func, "!SYM_ARM_BRANCH", i );
        DBG ( DEBUG_NOKIA, "## %s(): calcing SYM_ARM_BRANCH %s -> %s\n", __FUNCTION__, sym_name, tmpstr );

		offset = nokia_get_section_dest ( ws, tmpstr );
		if ( offset == E_FAIL )
        {
            DBG ( DEBUG_NOKIA,  "## %s: didn't find %s or its offset is wrong\n", __FUNCTION__, tmpstr );
            return E_FAIL;
        }

		destination = nokia_get_section_dest ( ws, sym_name );
		if ( destination == E_FAIL )
        {
            printf (  "## %s: destination %s not found/is not a number\n", __FUNCTION__, sym_name );
            return E_FAIL;
        }

        DBG ( DEBUG_NOKIA,  "## %s: SYM_ARM_BRANCH 0x%08X -> 0x%08X calced\n", __FUNCTION__, offset, destination );
		
		// jump
		arm_set_32 ( ws, ARM_B, offset - base, destination - base, v_get_w (ws,offset-base)>>28 );
        i++;
    }

    //handle all !SYM_PTR's
    DBG ( DEBUG_NOKIA, "## %s(): handling all !SYM_PTR's\n", __FUNCTION__ );
	i = 0;
    while ( sym_name = util_block_get_val_ptr ( func, "!SYM_PTR", i ) )
    {
        //calc offset
        tmpstr = util_block_get_offset_ptr ( func, "!SYM_PTR", i );
        DBG ( DEBUG_NOKIA, "## %s(): calcing SYM_PTR %s -> %s\n", __FUNCTION__, sym_name, tmpstr );

		offset = nokia_get_section_dest ( ws, tmpstr );
		if ( offset == E_FAIL )
        {
            DBG ( DEBUG_NOKIA,  "## %s: didn't find %s or its offset is wrong\n", __FUNCTION__, tmpstr );
            return E_FAIL;
        }

		destination = nokia_get_section_dest ( ws, sym_name );
		if ( destination == E_FAIL )
        {
            printf (  "## %s: destination %s not found/is not a number\n", __FUNCTION__, sym_name );
            return E_FAIL;
        }

        DBG ( DEBUG_NOKIA,  "## %s: SYM_PTR 0x%08X -> 0x%08X calced\n", __FUNCTION__, offset, destination );
		
		// set the pointer
        v_set_w ( ws, offset - base, destination );
        i++;
    }

    return len;
}



/* create func at offset, if offset = -1, then free space is searched */
/*!
 * \brief <b>DEPRECATED</b>
 *
 * use nokia_create_func2 !
 */
unsigned int
nokia_create_func ( t_workspace *ws, t_function *func, unsigned int offset )
{

    /*  text = code
        data = global variables (?)
        bss  = malloc (?), but this should be handled in the text ...
    */
	unsigned int pos = 0;
    unsigned int text_pos, data_pos, i, j;
    unsigned int text_len, data_len;
    struct s_function_relative_data *symbols = NULL;

    char str10[11];

    if ( !ws || !func )
        return E_FAIL;

    if ( !v_valid_inline ( ws, offset ) && offset != MEM_AUTO )
    {
        DBG ( DEBUG_NOKIA, "## %s: address not in file\n", __FUNCTION__ );
        return E_FAIL;
    }

    /* TODO: check if everything exists... */

    text_len = util_block_get_size( func, ".text" );
    //data_len = util_block_get_size( func, ".data" );

    //get free space
    if ( text_len > 0 )
    {
        /*if ( data_len > 0 )
        {
            text_pos =  arm_get_freespace( s, ( text_len % 2 ? text_len + 1 : text_len ) + ( data_len % 2 ? data_len + 1 : data_len ), offset );
            data_pos = text_pos + ( text_len % 2 ? text_len + 1 : text_len );
        }
        else*/
        if ( offset == MEM_AUTO )
        {
            text_pos = arm_get_freespace( ws, text_len % 2 ? text_len + 1 : text_len, offset );
            if ( text_pos == E_FAIL )
            {
                DBG ( DEBUG_NOKIA, "## %s : getting freespace failed\n", __FUNCTION__ );
                return E_FAIL;
            }
            data_len = E_FAIL;
            data_pos = 0;
        }
        else
            text_pos = offset;
    }
    else
    {
        DBG ( DEBUG_NOKIA, "## %s: .text size <= 0\n", __FUNCTION__ );
        return E_FAIL;
    }

    /* copy func */
    //for ( i = 0; i < text_len; i++ )
    //    v_set_b ( s, text_pos+i, *(util_block_get_data_pointer ( func, ".text" )+i) );

    DBG ( DEBUG_NOKIA, "## %s: creating %s at 0x%08X\n", __FUNCTION__, func->name, text_pos );

    v_memcpy_put ( ws, text_pos, util_block_get_data_pointer ( func, ".text" ), text_len );


    /* copy data */
    /*
    if ( data_pos )
        for ( i = 0; i < data_len; i++ )
            v_set_b ( s, data_pos+i, *(util_block_get_data_pointer ( func, ".data" )+i) );
    */

    /* TODO: reserve eeprom + ram */

    symbols = func->rel_data;

	pos = 0;
    while ( symbols && symbols[pos].dest )
    {
        DBG ( DEBUG_NOKIA, "## %s: handling reloc data offset: %04X\n", __FUNCTION__, symbols[pos].offset );
        if ( symbols[pos].offset > util_block_get_size ( func, ".text" ) )
        {
            DBG ( DEBUG_NOKIA, "## %s: overflow, offset not in .text\n", __FUNCTION__ );
            return E_FAIL;  //offset not in ".text"
        }

        /*if ( !strncmp ( ".data", symbols->dest, 5 ) )
        {
            if ( !data_pos )
                return E_FAIL;  //no data segment
            i = data_pos;
            if ( symbols->dest[5] == '+' )
            {
                //TODO
                //parse + value
                //i+=
            }
            else
                if ( strlen ( symbols->dest ) > 5 ) //no +, so no additional content!
                    return E_FAIL;

            //TODO: difference SYM_PTR, SYM_CALL
            v_set_w ( s, text_pos + symbols->offset, i );

        }
        else
            if ( !strncmp ( ".bss", symbols->dest, 4 ) )
            {
                //ram
                //reserve ram
                //TODO: difference SYM_PTR, SYM_CALL
            }
            else*/
       /* if ( !strncmp ( ".text", symbols->dest, 5 ) )
        {
            //pointer to itself
            //case ".text"
            //TODO: difference SYM_PTR, SYM_CALL
        }
        else
        {
            //get symbol, if fails, try to parse value
            //TODO: difference SYM_PTR, SYM_CALL
        }*/

        if ( symbols[pos].type == SYM_CALL || symbols[pos].type == SYM_PTR )
        {

            //check if such a symbol exists
            if ( symbols_get ( NULL, symbols[pos].dest ) )
            {
                DBG ( DEBUG_NOKIA, "## %s: reloc: dest is an already known symbol\n", __FUNCTION__ );
                /* TODO:
                    create destination parsing funcs in util_symbols
                */
                //sscanf ( symbols->offset, "0x%x", &i );
                sscanf ( symbols_get ( NULL, symbols[pos].dest )->data, "0x%x", &j );
                if ( symbols[pos].type == SYM_CALL )
                    arm_set ( ws, THUMB_BL, text_pos+symbols[pos].offset, j );
                else
                    v_set_w ( ws, text_pos+symbols[pos].offset, j );
            }
            else    //assume it's a pointer!
            {
                if ( !strncmp ( "0x", symbols[pos].dest, 2 ) )
                {

                    //sscanf ( symbols->offset, "0x%x", &i );
                    sscanf ( symbols[pos].dest, "0x%x", &j );
                    DBG ( DEBUG_NOKIA, "## %s: reloc: dest is beginning with 0x ... 0x%08X \n", __FUNCTION__, j );
                    if ( symbols[pos].type == SYM_CALL )
                        arm_set ( ws, THUMB_BL, text_pos+symbols[pos].offset, j );
                    else
                        v_set_w ( ws, text_pos+symbols[pos].offset, j );
                }
                else
                {
                    DBG ( DEBUG_NOKIA, "## %s: don't know destination: %s\n", __FUNCTION__, symbols[pos].dest );
                    return E_FAIL;
                }
            }
        }


		pos++;
//        symbols += sizeof ( struct s_function_relative_data );
    }

    sprintf ( str10, "0x%08x", text_pos );
    symbols_add( NULL, func->name, SYM_CALL /*type, FUNC*/, str10 );

    return text_pos;

}


/* TODO: ppm funcs and other specifics */


#endif
