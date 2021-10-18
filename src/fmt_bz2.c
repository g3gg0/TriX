#ifndef __TRIX_FMT_BZ2_C__
#define __TRIX_FMT_BZ2_C__

#define BZ_NO_STDIO
//#include <stdlib.h>
#include "defines.h"
#include "bz2_bzlib.h"
#include "segment.h"
#include "stage.h"
#include "symbols.h"

#include "fmt.h"
#include "options.h"

#include "mem.h"

int blockSize100k = 5;

unsigned int
fmt_bz2_free ( t_stage * s )
{
    if ( !s )
        return E_OK;

    segment_release_all ( s->segments );
    s->segments = NULL;

    return E_OK;
}

char *
fmt_bz2_compress ( char *buf, unsigned int length, unsigned int *c_length )
{
    bz_stream strm;
    unsigned int verbosity = 1;
    unsigned int workFactor = 30;
    int returncode = 0;
    unsigned int out_length = length + 600 + ( length * 2 );
    char *data = NULL;
    char *out_data = NULL;

    strm.opaque = NULL;
    strm.bzalloc = NULL;
    strm.bzfree = NULL;

    out_data = malloc ( out_length );
    if ( !out_data )
        return NULL;

    returncode = BZ2_bzCompressInit ( &strm, blockSize100k, verbosity, workFactor );
    if ( returncode != BZ_OK )
        return NULL;

    strm.next_in = buf;
    strm.avail_in = length;
    strm.next_out = out_data;
    strm.avail_out = out_length;

    do
    {
        returncode = BZ2_bzCompress ( &strm, BZ_FINISH );
    }
    while ( returncode == BZ_RUN_OK );


    if ( returncode < 0 )
        return NULL;

    data = malloc ( strm.total_out_lo32 );
    if ( !data )
        return NULL;

    if ( BZ2_bzCompressEnd ( &strm ) != BZ_OK )
        return NULL;

    memcpy ( data, out_data, strm.total_out_lo32 );

    if ( c_length )
        *c_length = strm.total_out_lo32;

    free ( out_data );

    return data;
}


char *
fmt_bz2_decompress ( char *buf, unsigned int length, unsigned int *o_length )
{
    bz_stream strm;
    unsigned int blocksize = 65537;
    unsigned int verbosity = 1;
    unsigned int small = 0;
    int returncode = 0;
    unsigned int out_length = blocksize;
    char *data = NULL;
    char *out_data = NULL;

    strm.opaque = NULL;
    strm.bzalloc = NULL;
    strm.bzfree = NULL;

    out_data = malloc ( out_length );
    if ( !out_data )
        return NULL;

    returncode = BZ2_bzDecompressInit ( &strm, verbosity, small );
    if ( returncode != BZ_OK )
	{
		BZ2_bzDecompressEnd ( &strm );
		free ( out_data );
        return NULL;
	}

    strm.next_in = buf;
    strm.avail_in = length;
    strm.next_out = out_data;
    strm.avail_out = out_length;

    do
    {
        returncode = BZ2_bzDecompress ( &strm );
        if ( !strm.total_in_lo32 )
            returncode = BZ_IO_ERROR;
        else
        {
            strm.next_in = buf + strm.total_in_lo32;
            strm.avail_in = length - strm.total_in_lo32;
            out_length = out_length + blocksize;
            out_data = realloc ( out_data, out_length );
            strm.next_out = out_data + strm.total_out_lo32;
            strm.avail_out = out_length - strm.total_out_lo32;
        }
//		printf ( out_data );
    }
    while ( returncode == BZ_OK );


    if ( returncode < 0 )
	{
		BZ2_bzDecompressEnd ( &strm );
		free ( out_data );
        return NULL;
	}

    data = malloc ( strm.total_out_lo32 );
    if ( !data )
	{
		BZ2_bzDecompressEnd ( &strm );
		free ( out_data );
        return NULL;
	}

    if ( BZ2_bzDecompressEnd ( &strm ) != BZ_OK )
	{
		BZ2_bzDecompressEnd ( &strm );
		free ( out_data );
		free ( data );
        return NULL;
	}

    memcpy ( data, out_data, strm.total_out_lo32 );
    *o_length = strm.total_out_lo32;
    free ( out_data );
	BZ2_bzDecompressEnd ( &strm );

    return data;
}

unsigned int
fmt_bz2_decode ( t_stage * source, t_stage * target )
{
    char *decompressed = NULL;
    unsigned int length = 0;

	
	DBG ( DEBUG_FMT, " => %s ( ) called\n", __FUNCTION__ );

    if ( !target )
        target = source->next;

    if ( !source || !target || !source->segments || !source->segments->data )
    {
	    DBG ( DEBUG_FMT, " -> %s ( ) failed !!\n", __FUNCTION__ );
        return E_FAIL;
    }

    decompressed = fmt_bz2_decompress ( source->segments->data, source->segments->length, &length );
	if ( decompressed )
		decompressed = realloc ( decompressed, length + 1 );

    if ( !decompressed )
    {
		DBG ( DEBUG_FMT, " -> %s ( %s, %s ) failed !!\n", __FUNCTION__, source->name, target->name );
        return E_FAIL;
    }

//	printf ( decompressed );

    target->name = "DECOMPRESSED";
    target->length = length;
    target->segments = segment_create (  );
    target->segments->name = "DATA";
    target->segments->data = decompressed;
    target->segments->start = 0;
    target->segments->end = length;
    target->segments->length = length;
    target->parser = "BZ2";
    target->type = "BZ2";

	// in case of text files, make sure it will end with an NULL byte
	target->segments->data[target->segments->length] = '\000'; 

    target->flags = S_MODIFIED;

	DBG ( DEBUG_FMT, " ## %s ( %s, %s ) done\n", __FUNCTION__, source->name, target->name );
    return E_OK;
}

unsigned int
fmt_bz2_encode ( t_stage * source, t_stage * target )
{
    unsigned int length = 0;
    char *compressed = NULL;

	DBG ( DEBUG_FMT, " => %s ( ) called\n", __FUNCTION__ );
    if ( !target )
        target = source->next;

    if ( !source || !target || !source->segments || !source->segments->data )
    {
	    DBG ( DEBUG_FMT, " -> %s ( ) failed !!\n", __FUNCTION__ );
        return E_FAIL;
    }

    if ( segment_count ( source->segments ) != 1 )
    {
	    DBG ( DEBUG_FMT, " -> %s ( %s, %s ) failed !!\n", __FUNCTION__, source->name, target->name );
        return E_FAIL;
    }
    compressed = fmt_bz2_compress ( GET_PTR ( source, 0 ), GET_SIZE ( source ), &length );
    if ( !compressed || !length )
        return E_FAIL;


	if ( target->segments )
		segment_release ( target->segments );
    target->segments = segment_create (  );
    target->segments->name = "DATA";
    target->segments->start = 0;
    target->segments->end = length;
    target->segments->length = length;
    target->segments->data = compressed;

	target->name = "RAW";
    target->length = length;
    target->parser = "BZ2";
    target->type = "BZ2";

	/* set the parser again (see file_format() ). all parsers should do this. shouldn't they? */
	source->parser = "BZ2";
	source->type = "BZ2";

	DBG ( DEBUG_FMT, " ## %s ( %s, %s ) done\n", __FUNCTION__, source->name, target->name );
    return E_OK;

}

t_fmt_handler bz2_handler = {
    "BZ2",
    "COMPRESSION",
	"fmt_bz2_decode",
    fmt_bz2_decode,
	"fmt_bz2_encode",
    fmt_bz2_encode,
	"fmt_bz2_free",
    fmt_bz2_free,
	NULL,
	NULL
};

unsigned int
fmt_bz2_init (  )
{
    fmt_register_handler ( &bz2_handler );
	options_add_core_option ( OPT_INT, "fmt.bz2", blockSize100k, "Compression block size in 100k's (1-9)" );

    return E_OK;
}

#endif
