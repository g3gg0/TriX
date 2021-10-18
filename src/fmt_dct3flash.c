#ifndef __TRIX_FMT_DCT3FLASH_C__
#define __TRIX_FMT_DCT3FLASH_C__

#include <stdlib.h>
#include "defines.h"
#include "segment.h"
#include "stage.h"
#include "symbols.h"

#include "file_io.h"
#include "file.h"
#include "workspace.h"
#include "util.h"
#include "fmt.h"

#include "mem.h"


t_locator fmt_dct3flash_mcu_pattern = {
    "fmt_dct3flash_mcu_pattern",
    "\x20\x61\x07\x48\x00\x80\x00\x10\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF",
    "\xFF\xFB\xE7\x48\xF0\x80\x7F\xF8\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF",
    16,
    LOC_UNIQUE
};
t_locator fmt_dct3flash_ppm_pattern = {
    "fmt_dct3flash_ppm_pattern",
    "\x50\x50\x4D\x00\x56\x20",
    "\xFF\xFF\xFF\xFF\xFF\xFF",
    6,
    LOC_UNIQUE
};

unsigned int
fmt_dct3flash_decode ( t_stage * source, t_stage * target )
{
    char *type = NULL;
    unsigned int start = 0;

	
	DBG ( DEBUG_FMT, " => %s ( ) called\n", __FUNCTION__ );

    if ( !target )
        target = source->next;

    if ( !source || !target || !source->segments || !source->segments->data )
    {
		
	    DBG ( DEBUG_FMT, " -> %s ( ) failed !!\n", __FUNCTION__ );
        return E_FAIL;
    }

    if ( source->length < 16 )
    {
		
	    DBG ( DEBUG_FMT, " -> %s ( %s, %s ) failed !!\n", __FUNCTION__, source->name, target->name );
        return E_FAIL;
    }

    if ( !strcmp ( source->parser, "DCT3FLASH" ) )
    {
		
		DBG ( DEBUG_FMT, " -> %s ( %s, %s ) failed !!\n", __FUNCTION__, source->name, target->name );
        return E_FAIL;
    }

    if ( util_check_pattern_raw ( source, 0, &fmt_dct3flash_mcu_pattern ) == E_OK )
    {
        type = "DCT3MCU";
        start = 0x00200000;
    }
    else if ( util_check_pattern_raw ( source, 0, &fmt_dct3flash_ppm_pattern ) == E_OK )
    {
        // what to do here?
        // somehow register the MCU's length and "remember" it
        // in case of a additional loaded PPM?
        //
        // krisha: we can search for a MCU stage (prev/next or with fi as argument) and get the length from there
        // but is that enough accurate ? the ppm start is then only guessed. since there won't be any new dct3 models
        // we can use a fixed table for starting addr
        // what todo with files including mcu + ppm (+ eeprom )? split them and handle both ?
		//
		// g3gg0: yes, i think the only clean method is looking for a previously loaded MCU
		// in case of no loaded MCU, use 0x0000 as offset (if not overriden by a user setting)
		// MCUPPM's dont matter. PPM editing functions have to search the PPM on their own.
		// we just want it to be at the correct position.
        type = "DCT3PPM";
        start = 0x003D0000;
    }
    else
    {
		
	    DBG ( DEBUG_FMT, " -> %s ( %s, %s ) failed !!\n", __FUNCTION__, source->name, target->name );
        return E_FAIL;
    }

    target->segments = segment_create (  );
    target->segments->name = "FLASH";
    target->segments->start = start;
    target->segments->end = start + source->length;
    target->segments->length = source->length;
    target->segments->data = malloc ( source->length );
    if ( !target->segments->data )
    {
		
	    DBG ( DEBUG_FMT, " -> %s ( %s, %s ) failed !!\n", __FUNCTION__, source->name, target->name );
        return E_FAIL;
    }
    memcpy ( target->segments->data, GET_PTR ( source, 0 ), source->length );

    target->name = "PARSED";
    target->length = source->length;
    target->parser = "DCT3FLASH";
    target->type = type;


	
	DBG ( DEBUG_FMT, " ## %s ( %s, %s ) done\n", __FUNCTION__, source->name, target->name );
    return E_OK;
}

unsigned int
fmt_dct3flash_encode ( t_stage * source, t_stage * target )
{
	
	DBG ( DEBUG_FMT, " => %s ( ) called\n", __FUNCTION__ );
    if ( !target )
        target = source->next;

    if ( !source || !target || !source->segments || !source->segments->data )
    {
	    DBG ( DEBUG_FMT, " -> %s ( ) failed !!\n", __FUNCTION__ );
        return E_FAIL;
    }

    if ( segment_count ( source->segments ) != 1 || source->length < 16 )
    {
	    DBG ( DEBUG_FMT, " -> %s ( %s, %s ) failed !!\n", __FUNCTION__, source->name, target->name );
        return E_FAIL;
    }

    if ( strcmp ( source->parser, "DCT3FLASH" ) )
    {
	    DBG ( DEBUG_FMT, " -> %s ( %s, %s ) failed !!\n", __FUNCTION__, source->name, target->name );
        return E_FAIL;
    }

    if ( util_check_pattern_raw ( source, 0, &fmt_dct3flash_mcu_pattern ) != E_OK &&
		 util_check_pattern_raw ( source, 0, &fmt_dct3flash_ppm_pattern ) != E_OK )
    {
	    DBG ( DEBUG_FMT, " -> %s ( %s, %s ) failed !!\n", __FUNCTION__, source->name, target->name );
        return E_FAIL;
    }

    target->segments = segment_create (  );
    target->segments->name = "FLASH";
    target->segments->start = 0;
    target->segments->end = source->length;
    target->segments->length = source->length;
    target->segments->data = malloc ( source->length );
    if ( !target->segments->data )
    {
	    DBG ( DEBUG_FMT, " -> %s ( %s, %s ) failed !!\n", __FUNCTION__, source->name, target->name );
        segment_release_all ( target->segments );
        return E_FAIL;
    }
    memcpy ( target->segments->data, GET_PTR ( source, 0 ), source->length );

    target->name = "RAW";
    target->length = source->length;
    target->parser = "DCT3FLASH";
    target->type = "DATA";
	
	DBG ( DEBUG_FMT, " ## %s ( %s, %s ) done\n", __FUNCTION__, source->name, target->name );
    return E_OK;
}

t_fmt_handler dct3flash_handler = {
    "DCT3FLASH",
    "FORMAT",
    "fmt_dct3flash_decode",
    fmt_dct3flash_decode,
    "fmt_dct3flash_encode",
    fmt_dct3flash_encode,
	NULL,
	NULL,
	NULL,
	NULL
};


unsigned int
fmt_dct3flash_init (  )
{
    fmt_register_handler ( &dct3flash_handler );
    return E_OK;
}

#endif
