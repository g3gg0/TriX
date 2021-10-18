
#define TRIXPLUG_MODE

#include "defines.h"
#include "seer.h"
#include "segment.h"
#include "stage.h"
#include "file_io.h"
#include "file.h"
#include "workspace.h"
#include "util.h"
#include "fmt.h"

#include "trixplug.h"
#include "mem.h"

unsigned int fmttest_init ( );

// include all the TriXPluG function/init stuff
TRIXPLUG_STUBS
#include "trixplug_wrapper.h"

PLUGIN_INFO_BEGIN
	PLUGIN_NAME ( "fmttest" )
	PLUGIN_INIT ( fmttest_init )
PLUGIN_INFO_FINISH


unsigned int
fmt_test_free ( t_stage * s )
{
    return E_FAIL;
}
unsigned int
fmt_test_decode ( t_stage * source, t_stage * target )
{
	DBG ( DEBUG_FMT, " => %s ( ) called\n", __FUNCTION__ );
    return E_FAIL;
}
unsigned int
fmt_test_encode ( t_stage * source, t_stage * target )
{
	DBG ( DEBUG_FMT, " => %s ( ) called\n", __FUNCTION__ );
    return E_FAIL;
}
t_fmt_handler fmttest_handler = {
    "TEST",
    "COMPRESSION",
	"fmt_test_decode",
    fmt_test_decode,
	"fmt_test_encode",
    fmt_test_encode,
	"fmt_test_free",
    fmt_test_free,
	NULL,
	NULL
};

unsigned int fmttest_init ( )
{
	fmt_register_handler ( &fmttest_handler );
	printf ( "FMT test Plugin Loaded" );
    return E_OK;
}
