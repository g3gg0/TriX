#ifndef __TRIX_FMT_WINTESLA_C__
#define __TRIX_FMT_WINTESLA_C__

#include <stdlib.h>
#include "defines.h"
#include "segment.h"
#include "stage.h"
#include "symbols.h"

#include "fmt.h"

#include "mem.h"


unsigned int
fmt_wintesla_free ( t_stage * s )
{
	DBG ( DEBUG_FMT, "%s ( %s )\n", __FUNCTION__, s->name );
    return E_FAIL;
}

unsigned int
fmt_wintesla_decode ( t_stage * source, t_stage * target )
{
	DBG ( DEBUG_FMT, " => %s ( ) called\n", __FUNCTION__ );
	
	DBG ( DEBUG_FMT, " -> %s ( ) failed !!\n", __FUNCTION__ );
    return E_FAIL;
}

unsigned int
fmt_wintesla_encode ( t_stage * source, t_stage * target )
{
	DBG ( DEBUG_FMT, "%s ( %s, %s )\n", __FUNCTION__, source->name, target->name );
    return E_FAIL;
}

t_fmt_handler wintesla_handler = {
    "WINTESLA",
    "FORMAT",
    "fmt_wintesla_decode",
    fmt_wintesla_decode,
    "fmt_wintesla_encode",
    fmt_wintesla_encode,
    "fmt_wintesla_free",
    fmt_wintesla_free,
	NULL,
	NULL
};


unsigned int
fmt_wintesla_init (  )
{
	// this is not supported... dont register :)
    //fmt_register_handler ( &wintesla_handler );
    return E_OK;
}

#endif
