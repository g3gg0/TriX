#ifndef __TRIX_FMTBADA_C__
#define __TRIX_FMTBADA_C__

#include <stdlib.h>
#include <string.h>

#include "defines.h"
#include "seer.h"
#include "segment.h"
#include "stage.h"
#include "file_io.h"
#include "file.h"
#include "workspace.h"
#include "util.h"
#include "fmt.h"
#include "symbols.h"
#include "options.h"
#include "mem.h"
#include "treenode.h"
#include "ui.h"
#include "crypto.h"
#include "trixplug.h"
#include "trixplug_wrapper.h"

#include "fmtBADA.h"


unsigned char *last_error_string = "None";

#define FMTBADA_TITLE                   "fmtBADA"
#define FMTBADA_MAJOR                   0
#define FMTBADA_MINOR                   30

PLUGIN_INFO_BEGIN
	PLUGIN_NAME ( FMTBADA_TITLE )
	PLUGIN_VERSION ( FMTBADA_MAJOR, FMTBADA_MINOR )
	PLUGIN_INIT ( fmtBADA_init )
	PLUGIN_CLEANUP ( fmtBADA_cleanup )
	PLUGIN_LASTERROR ( fmtBADA_lasterror )
PLUGIN_INFO_FINISH


// =============================================================================

unsigned int decomp_ver[][4] =
{
	{ S8500v12 | BADA_APPS,  0x03050000, 0x01100000, 0x03500000 },
	{ S8500v12 | BADA_RSRC1, 0x04070000, 0x04800000, 0x06F00000 },
	{ S8500v12 | BADA_RSRCS, 0x04070000, 0x0B700000, 0x00F00000 },

	{ S8530v12 | BADA_APPS,  0x03050000, 0x01300000, 0x03500000 },
	{ S8530v12 | BADA_RSRC1, 0x04070000, 0x04A00000, 0x05000000 },
	{ S8530v12 | BADA_RSRCS, 0x04070000, 0x09A00000, 0x00F00000 },

	{ S8500v20 | BADA_APPS,  0x04020000, 0x01100000, 0x02000000 },
	{ S8500v20 | BADA_RSRC1, 0x05020000, 0x05D00000, 0x05A00000 },
	{ S8500v20 | BADA_RSRCS, 0x05020000, 0x03A00000, 0x02300000 },

	{ S8530v20 | BADA_APPS,  0x04020000, 0x01300000, 0x02000000 },
	{ S8530v20 | BADA_RSRC1, 0x05020000, 0x03600000, 0x04100000 },
	{ S8530v20 | BADA_RSRCS, 0x05020000, 0x07700000, 0x01E00000 },

	{ S8600    | BADA_APPS,  0x04020000, 0x08000000, 0x02000000 },
	{ S8600    | BADA_RSRC1, 0x05020000, 0x0A200000, 0x03200000 },
	{ S8600    | BADA_RSRCS, 0x05020000, 0x0D400000, 0x02800000 },

	{ S7250D   | BADA_APPS,  0x04020000, 0x00E00000, 0x01F00000 },
	{ S7250D   | BADA_RSRC1, 0x05020000, 0x02F00000, 0x01E00000 },
	{ S7250D   | BADA_RSRCS, 0x05020000, 0x04D00000, 0x01C00000 },

	{ 0, 0, 0 }
};


unsigned int fmt_bada_reinit ( t_workspace *ws )
{
   if ( !ws->memmap )
      return E_FAIL;

   // maybe workspace endianness should be set same as last segment?
   ws->flags = FLAGS_ENDIANESS_LE;

   file_sync ( ws->fileinfo );
   workspace_update_memmap ( ws );
   symbols_free_all ( NULL );

   return E_OK;
}

unsigned int fmt_bada_decrypt ( t_segment *dst, t_bada_footer *footer )
{
	unsigned char *data = dst->data;
	unsigned int length = footer->seed_length;

	// skip 0x60 bytes of FS in S8600/S7250
	if ( 0 == v_get_w_raw ( (t_stage *)dst, dst->start + 0x5C ) )
	{
		data += 0x60;
		length -= 0x60;
	}

	seed_decrypt ( data, footer->seed_key, length );

	dst->flags |= FB_SEED;

	return E_OK;
}

unsigned int fmt_bada_encrypt ( t_segment *dst, t_bada_footer *footer )
{
	unsigned char *data = dst->data;
	unsigned int length = footer->seed_length;

	// skip 0x60 bytes of FS in S8600/S7250
	if ( MAGIC_FS == v_get_w_raw ( (t_stage *)dst, dst->start + 0x60 ) )
	{
		data += 0x60;
		length -= 0x60;
	}

	seed_encrypt ( data, footer->seed_key, length );

	dst->flags &= ~FB_SEED;

	return E_OK;
}

unsigned int fmt_bada_decompress ( t_segment *dst )
{
	unsigned char *data;
	unsigned int length;

	length = qmd_get_size ( dst->data );
	if ( !length )
		return E_FAIL;

	data = malloc ( length );

	if ( E_FAIL == qmd_parser ( data, dst->data ) )
	{
		free ( data );
		return E_FAIL;
	}

	free ( dst->data );
	dst->data = data;
	dst->length = length;
	dst->end = dst->start + dst->length;

	return E_OK;
}


unsigned int fmt_bada_parse_bada ( t_stage *source, t_stage *target )
{
	t_segment *src, *dst;
	t_bada_main *hdr;
	t_bada_priv *priv;
	unsigned int id;

	dst = target->segments;
	src = source->segments;

	priv = (t_bada_priv *)PRIV_MALLOC(t_bada_priv);
	target->priv = (t_stage_priv *)priv;

	priv->header_len = v_get_w_raw ( (t_stage *)src, src->start + 0x20 ); // hdr_size
	priv->header = (unsigned char *)malloc ( priv->header_len );
	memcpy ( priv->header, src->data, priv->header_len );

	memcpy ( &priv->footer, src->data + src->length - sizeof ( t_bada_footer ), sizeof ( t_bada_footer ) );

	hdr = (t_bada_main *)priv->header;
	id = 0;
	while ( id < hdr->hdr_count )
	{
		if ( 1 == hdr->hdr_count )
			dst->start = v_get_w_raw ( (t_stage *)src, src->start + src->length - sizeof ( t_bada_footer ) + 4 );
		else
			dst->start = v_get_w_raw ( (t_stage *)src, src->start + hdr->files[id].start + 12 );

		dst->length = hdr->files[id].length;
		dst->end = dst->start + dst->length;

		dst->data = (unsigned char *)malloc ( dst->length );
		memcpy ( dst->data, src->data + hdr->files[id].start, dst->length );

		dst->name = (char *)strdup ( hdr->files[id].name );

		if ( ++id < hdr->hdr_count )
			dst = segment_add ( target->segments );
	}

	return 0;
}

unsigned int fmt_bada_parse_cafe ( t_stage *source, t_stage *target )
{
	t_segment *src, *dst;
	t_bada_csc_main *csc;
	t_bada_priv *priv;
	unsigned int id;

	dst = target->segments;
	src = source->segments;

	priv = (t_bada_priv *)PRIV_MALLOC(t_bada_priv);
	target->priv = (t_stage_priv *)priv;

	priv->header_len = v_get_w_raw ( (t_stage *)src, src->start + 0x18 ); // hdr_size
	priv->header = (unsigned char *)malloc ( priv->header_len );
	memcpy ( priv->header, src->data, priv->header_len );
	priv->footer.magic = MAGIC_CAFE;

	csc = (t_bada_csc_main *)priv->header;
	id = 0;
	while ( id < csc->hdr_count )
	{
		priv = (t_bada_priv *)PRIV_MALLOC(t_bada_priv);
		dst->priv = (t_seg_priv *)priv;
		priv->header_len = 0;
		priv->header = NULL;

		if ( csc->hdr_count - 1 == id )
		{
			dst->start = v_get_w_raw ( (t_stage *)src, src->start + csc->files[id].start + csc->files[id].length + 8 );
			dst->length = csc->files[id].length;

			memcpy ( &priv->footer, src->data + src->length - sizeof ( t_bada_footer ), sizeof ( t_bada_footer ) );
		}
		else
		{
			dst->start = v_get_w_raw ( (t_stage *)src, src->start + csc->files[id].start + csc->files[id].length - sizeof ( t_bada_footer ) + 4 );
			dst->length = csc->files[id].length - sizeof ( t_bada_footer );

			memcpy ( &priv->footer, src->data + csc->files[id].start + csc->files[id].length - sizeof ( t_bada_footer ), sizeof ( t_bada_footer ) );
		}

		dst->end = dst->start + dst->length;
		dst->data = (unsigned char *)malloc ( dst->length );
		memcpy ( dst->data, src->data + csc->files[id].start, dst->length );

		if ( MAGIC_SEED == priv->footer.magic_seed ) // seed is used
			fmt_bada_decrypt ( dst, &priv->footer );

		fmt_bada_decompress ( dst );

		dst->name = (char *)calloc ( 0x240, sizeof ( char ) );

		wcstombs ( dst->name, csc->files[id].path, 0x240 );

		if ( ++id < csc->hdr_count )
			dst = segment_add ( target->segments );
	}

	return 0;
}

unsigned int fmt_bada_parse ( t_stage *source, t_stage *target )
{
	t_segment *src, *dst;
	t_bada_csc_main *csc;
	t_bada_priv *priv;
	unsigned int id;

	dst = target->segments;
	src = source->segments;

	priv = (t_bada_priv *)PRIV_MALLOC(t_bada_priv);
	target->priv = (t_stage_priv *)priv;

	priv->header_len = 0;
	priv->header = NULL;

	memcpy ( &priv->footer, src->data + src->length - sizeof ( t_bada_footer ), sizeof ( t_bada_footer ) );

	dst->start = v_get_w_raw ( (t_stage *)src, src->start + source->length - sizeof ( t_bada_footer ) + 4 );
	dst->length = src->length - sizeof ( t_bada_footer );
	dst->end = dst->start + dst->length;

	dst->data = (unsigned char *)malloc ( dst->length );
	memcpy ( dst->data, src->data, dst->length );

	if ( MAGIC_SEED == priv->footer.magic_seed ) // seed is used
		fmt_bada_decrypt ( dst, &priv->footer );

	fmt_bada_decompress ( dst );

	if ( MAGIC_FS == v_get_w_raw ( (t_stage *)dst, dst->start + 0x60 ) ) // FS
	{
		dst->name = (char *)calloc ( 16, sizeof ( char ) );
		wcstombs ( dst->name, (const wchar_t *)dst->data, 16 );
	}
	else
		dst->name = (char *)strdup ( "DATA" );

	return dst->length;
}

unsigned int fmt_bada_decode ( t_stage *source, t_stage *target )
{
	t_bada_priv *priv = NULL;
	t_segment *dst, *src;
	unsigned int i, id;


	DBG ( DEBUG_FMT, " => %s ( ) called\n", __FUNCTION__ );

	if ( !target )
		target = source->next;

	if ( !source || !target || !source->segments || !source->segments->data )
	{
		DBG ( DEBUG_FMT, " -> %s ( ) failed (null ptr)!!\n", __FUNCTION__ );
		return E_FAIL;
	}

	if ( source->length <= sizeof ( t_bada_footer ) )
	{
		DBG ( DEBUG_FMT, " -> %s ( %s, %s ) failed (length)!!\n", __FUNCTION__, source->name, target->name );
		return E_FAIL;
	}

	if ( MAGIC_BADA != v_get_w_raw ( source, source->length - sizeof ( t_bada_footer ) ) ) // check MAGIC signature
	{
		DBG ( DEBUG_FMT, " -> %s ( %s, %s ) failed (MAGIC)!!\n", __FUNCTION__, source->name, target->name );
		return E_FAIL;
	}

	if ( 1 != segment_count ( source->segments ) )
	{
		DBG ( DEBUG_FMT, " -> %s ( %s, %s ) failed (need one segment)!!\n", __FUNCTION__, source->name, target->name );
		return E_FAIL;
	}

	// set endianess
	source->flags &= ~FLAGS_ENDIANESS;
	source->flags |= FLAGS_ENDIANESS_LE;
	target->flags &= ~FLAGS_ENDIANESS;
	target->flags |= FLAGS_ENDIANESS_LE;

	if ( target->segments )
		segment_release ( target->segments );

	target->segments = segment_create ( );

	switch ( v_get_w_raw ( source, 0 ) )
	{
		case HDR_BADA:
			target->length = fmt_bada_parse_bada ( source, target );
			break;

		case HDR_CAFE:
			target->length = fmt_bada_parse_cafe ( source, target );
			break;

		default:
			target->length = fmt_bada_parse ( source, target );
			break;
	}

	target->name   = "DECODED";
	target->type   = "BADA";
	target->parser = "BADA";

	HEAP_CHECK;

	DBG ( DEBUG_FMT, " ## %s ( %s, %s ) done\n", __FUNCTION__, source->name, target->name );
	return E_OK;
}


unsigned int fmt_bada_encode ( t_stage *source, t_stage *target )
{
	t_bada_priv *priv;
	t_segment *dst, *src;
	unsigned int i, pos;


	DBG ( DEBUG_FMT, " => %s ( ) called\n", __FUNCTION__ );

	if ( !target )
		target = source->next;

	if ( !source || !target )
	{
		DBG ( DEBUG_FMT, " -> %s ( ) failed !!\n", __FUNCTION__ );
		return E_FAIL;
	}

	src = segment_find_by_num ( source->segments, 0 );
	if ( !src || !src->data || !source->priv )
	{
		segment_release ( target->segments );

		DBG ( DEBUG_FMT, " -> %s ( %s, %s ) failed !!\n", __FUNCTION__, source->name, target->name );
		return E_FAIL;
	}

	if ( target->segments )
		segment_release ( target->segments );

	target->segments = segment_create ( );
	dst = target->segments;
	dst->name = "DATA";
	dst->start = 0;

	priv = (t_bada_priv *)source->priv;


	if ( !priv->header ) // no header
	{
		dst->length = ALIGN_WORD ( src->length ) + sizeof ( t_bada_footer );
		dst->data = (char *)malloc ( dst->length );
		if ( !dst->data )
			return E_FAIL;

		memset ( dst->data, 0, dst->length );
		memcpy ( dst->data, src->data, src->length );

		// generate md5 hash, remaining bytes needs to be zeroed
		crypto_generate_md5 ( dst->data, ALIGN_BY ( src->length, 0x40 ), priv->footer.md5_sum );
		memcpy ( dst->data + dst->length - sizeof ( t_bada_footer ), &priv->footer, sizeof ( t_bada_footer ) );
	}
	else if ( *(unsigned int *)priv->header == HDR_BADA )
	{
		t_bada_main *hdr = (t_bada_main *)priv->header;

		i = 0;
		dst->length = 0;
		while ( src = segment_find_by_num ( source->segments, i++ ) )
			dst->length += ALIGN_WORD ( src->length );

		pos = ALIGN_BY ( i * sizeof ( t_bada_file ), 0x200 );
		dst->length += pos; // headers
		dst->length += sizeof ( t_bada_footer );

		dst->data = malloc ( dst->length );
		memset ( dst->data, 0, dst->length );

		v_set_w_raw ( (t_stage *)dst, 0, HDR_BADA );
		v_set_w_raw ( (t_stage *)dst, 4, i - 1 );

		i = 0;
		while ( src = segment_find_by_num ( source->segments, i ) )
		{
			// first we need fix the header
			v_set_w_raw ( (t_stage *)dst, sizeof ( t_bada_main ) + i * sizeof ( t_bada_file ) + 0x00, pos );
			v_set_w_raw ( (t_stage *)dst, sizeof ( t_bada_main ) + i * sizeof ( t_bada_file ) + 0x04, ALIGN_WORD ( src->length ) );
			memcpy ( dst->data + sizeof ( t_bada_main ) + i * sizeof ( t_bada_file ) + 0x08, hdr->files[i].name, 24 );

			memcpy ( dst->data + pos, src->data, src->length );
			pos += ALIGN_WORD ( src->length );
			i++;
		}

		// generate md5 hash, remaining bytes needs to be zeroed
		crypto_generate_md5 ( dst->data, ALIGN_BY ( pos, 0x40 ), priv->footer.md5_sum );
		memcpy ( dst->data + dst->length - sizeof ( t_bada_footer ), &priv->footer, sizeof ( t_bada_footer ) );
	}
	else if ( *(unsigned int *)priv->header == HDR_CAFE )
	{
		t_bada_csc_main *csc = (t_bada_csc_main *)priv->header;

		i = 0;
		while ( src = segment_find_by_num ( source->segments, i++ ) )
		{
			priv = (t_bada_priv *)src->priv;

			dst->length = ALIGN_BY ( dst->length, csc->align ) + ALIGN_WORD ( src->length ) + ALIGN_WORD ( sizeof ( t_bada_footer ) );
		}

		priv = (t_bada_priv *)source->priv;

		pos = ALIGN_BY ( i * sizeof ( t_bada_csc_file ), csc->align ); // headers
		dst->length += pos + sizeof ( t_bada_footer ) + 4; // footer + cafe

		dst->data = malloc ( dst->length );
		memset ( dst->data, 0, dst->length );

		memcpy ( dst->data, csc, sizeof ( t_bada_csc_file ) * ( csc->hdr_count + 1 ) );

		// fix main header
		v_set_w_raw ( (t_stage *)dst, 4, dst->length - sizeof ( t_bada_footer ) - 4 );
		v_set_w_raw ( (t_stage *)dst, 8, pos );
		v_set_w_raw ( (t_stage *)dst, 0x10, i - 1 );

		i = 0;
		while ( src = segment_find_by_num ( source->segments, i ) )
		{
			priv = (t_bada_priv *)src->priv;

			pos = ALIGN_BY ( pos, csc->align );

			// fix file header
			v_set_w_raw ( (t_stage *)dst, sizeof ( t_bada_csc_main ) + i * sizeof ( t_bada_csc_file ) + 0x0C, pos );
			v_set_w_raw ( (t_stage *)dst, sizeof ( t_bada_csc_main ) + i * sizeof ( t_bada_csc_file ) + 0x18, ALIGN_WORD ( src->length ) + sizeof ( t_bada_footer ) );

			memcpy ( dst->data + pos, src->data, src->length );
			if ( priv->footer.magic ) // footer exists
			{
				// generate md5 hash, remaining bytes needs to be zeroed
				crypto_generate_md5 ( dst->data + pos, ALIGN_BY ( src->length, 0x40 ), priv->footer.md5_sum );
				memcpy ( dst->data + pos + ALIGN_WORD ( src->length ), &priv->footer, sizeof ( t_bada_footer ) );
			}

			pos += ALIGN_WORD ( src->length + sizeof ( t_bada_footer ) );
			i++;
		}

		priv = (t_bada_priv *)source->priv;

		v_set_w_raw ( (t_stage *)dst, pos, MAGIC_CAFE );
		// generate md5 hash, remaining bytes needs to be zeroed
		crypto_generate_md5 ( dst->data, ALIGN_BY ( pos + 4, 0x40 ), priv->footer.md5_sum );
		memcpy ( dst->data + pos + 4, &priv->footer, sizeof ( t_bada_footer ) );
	}

	CHECK_AND_FREE ( priv->header );

	dst->end = dst->start + dst->length;
	target->length = dst->length;

	/* set the parser again (see file_format() ). all parsers should do this. shouldn't they? */
	source->parser = "BADA";
	source->type = "BADA";

	HEAP_CHECK;

	DBG ( DEBUG_FMT, " ## %s ( %s, %s ) done\n", __FUNCTION__, source->name, target->name );
	return E_OK;
}

unsigned int fmt_bada_free ( t_stage *s )
{
	DBG ( DEBUG_FMT, " => %s ( ) called\n", __FUNCTION__ );

	return E_OK;
}


t_fmt_handler fmtBADA_handler =
{
	"BADA",
	"FORMAT",
	"fmt_bada_decode",
	 fmt_bada_decode,
	"fmt_bada_encode",
	 fmt_bada_encode,
	"fmt_bada_free",
	 fmt_bada_free,
	NULL,
	NULL
};

unsigned char *fmtBADA_lasterror ( )
{
	unsigned char *ret = last_error_string;
	last_error_string = "None";

	return ret;
}

unsigned int fmtBADA_cleanup ( )
{
	fmt_unregister_handler ( fmtBADA_handler.name );
	printf ( "FMT BADA Plugin Unloaded" );
	return E_OK;
}

unsigned int fmtBADA_init ( )
{
	REGISTER_HEADER ( "trix_fmtbada", FMTBADA_HEADERS );
	FMTBADA_SYMBOLS
	fmt_register_handler ( &fmtBADA_handler );

	printf ( FMTBADA_TITLE " v%01i.%02i Plugin Loaded", FMTBADA_MAJOR, FMTBADA_MINOR  );
	return E_OK;
}

#endif /* __TRIX_FMTBADA_C__ */
