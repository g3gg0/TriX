#ifndef __TRIX_FMT_EPOC_C__
#define __TRIX_FMT_EPOC_C__

#include <stdlib.h>
#include <string.h>

#include "defines.h"
#include "segment.h"
#include "stage.h"
#include "symbols.h"

#include "fmt.h"
#include "options.h"

#include "file_io.h"
#include "file.h"
#include "workspace.h"
#include "util.h"
#include "epoc.h"
#include "fmt_epoc.h"

#include "e32image_deflate.h"

#include "mem.h"

unsigned int fmt_epoc_enabled = 0;

unsigned int
fmt_epoc_free ( t_stage * s )
{
	DBG ( DEBUG_FMT, " => %s ( ) called\n", __FUNCTION__ );

	return 0;
}

unsigned int
fmt_epoc_calc_checksum ( unsigned char *data, unsigned int length )
{
	unsigned int checksum = 0;
	unsigned int *ptr = (unsigned int *)data;

	if ( length & 3 )
		return 0;

	while ( length )
	{
		checksum += *ptr;
		ptr++;
		length -= 4;
	}

	return checksum;
}


unsigned int
fmt_epoc_parse_sections_v2 ( t_stage *source, t_stage *target, E32ImageHeaderv2 header )
{
	t_segment *seg = NULL;
	unsigned char *buffer = NULL;
	void *ctx = NULL;
	int tmp = 0;

	if ( !source || !target )
		return E_FAIL;


	/*	ctx = deflate_init ();
		if ( !ctx )
			return E_FAIL;

		outlen = header.iUncompressedSize;
		inlen = source->segments->length - header.iCodeOffset;
		buffer = malloc ( outlen );
		do {
			tmp = deflate_decompress ( ctx, source->segments->data + header.iCodeOffset, inlen, buffer, &outlen );
			header.iCodeOffset++;
			inlen--;
		} while ( tmp == -1 && inlen && ( tmp || outlen < 0x1000) );
//		deflate_decompress ( ctx, buffer, outlen, source->segments->data + header.iCodeOffset, &inlen );

//		outlen = header.iUncompressedSize;
//		inlen = source->segments->length - header.iCodeOffset;
//		deflate_decompress ( ctx, source->segments->data + header.iCodeOffset, inlen, buffer, &outlen );
*/


	/* check TEXT segment (code/data)*/
	if ( header.iCodeOffset && header.iCodeSize )
	{
		seg = segment_create (  );
		if ( !seg )
			return E_FAIL;
		target->segments = seg;
		seg->start = header.iCodeBase;
		seg->length = header.iCodeSize;
		seg->end = seg->start + seg->length;
		seg->data = malloc ( seg->length );
		if ( !seg->data )
			return E_FAIL;
		v_memcpy_get_raw ( source, seg->data, header.iCodeOffset, seg->length );
		seg->name = ".text";
	}

	/* check data section */
	if ( header.iDataOffset )
	{
		seg = segment_create (  );
		if ( !seg )
			return E_FAIL;

		tmp = 0;
		if ( header.iImportOffset > tmp )
			tmp = header.iImportOffset;
		else if ( header.iCodeRelocOffset > tmp )
			tmp = header.iCodeRelocOffset;
		else if ( header.iDataRelocOffset > tmp )
			tmp = header.iDataRelocOffset;
		else
			tmp = source->segments->length;

		if ( tmp < header.iDataOffset )
		{
			ERR ( 0, "FATAL: iDataOffset > end!" ); 
			return E_FAIL;
		}
		tmp -= header.iDataOffset;

		if ( target->segments )
			segment_get_last ( target->segments )->next = seg;

		seg->start = header.iDataBase;
		seg->length = tmp;
		seg->end = seg->start + seg->length;
		seg->flags |= FLAGS_HIDE_IN_MEM;
		seg->data = malloc ( seg->length );
		if ( !seg->data )
			return E_FAIL;
		v_memcpy_get_raw ( source, seg->data, header.iDataOffset, seg->length );
		seg->name = ".data";
	}

	/* check import section */
	if ( header.iImportOffset )
	{
		seg = segment_create (  );
		if ( !seg )
			return E_FAIL;

		tmp = 0;
		if ( header.iCodeRelocOffset > tmp )
			tmp = header.iCodeRelocOffset;
		else if ( header.iDataRelocOffset > tmp )
			tmp = header.iDataRelocOffset;
		else
			tmp = source->segments->length;

		if ( tmp < header.iImportOffset )
		{
			ERR ( 0, "FATAL: iImportOffset > end!" ); 
			return E_FAIL;
		}
		tmp -= header.iImportOffset;

		if ( target->segments )
			segment_get_last ( target->segments )->next = seg;

		seg->start = 0;
		seg->length = tmp;
		seg->end = seg->start + seg->length;
		seg->flags |= FLAGS_HIDE_IN_MEM;
		seg->data = malloc ( seg->length );
		if ( !seg->data )
			return E_FAIL;
		v_memcpy_get_raw ( source, seg->data, header.iImportOffset, seg->length );
		seg->name = ".import";
	}



	/* check CodeReloc section */
	if ( header.iCodeRelocOffset )
	{
		seg = segment_create (  );
		if ( !seg )
			return E_FAIL;

		tmp = 0;
		if ( header.iDataRelocOffset > tmp )
			tmp = header.iDataRelocOffset;
		else
			tmp = source->segments->length;

		if ( tmp < header.iImportOffset )
		{
			ERR ( 0, "FATAL: iCodeRelocOffset > end!" ); 
			return E_FAIL;
		}
		tmp -= header.iCodeRelocOffset;

		if ( target->segments )
			segment_get_last ( target->segments )->next = seg;

		seg->start = 0;
		seg->length = tmp;
		seg->end = seg->start + seg->length;
		seg->flags |= FLAGS_HIDE_IN_MEM;
		seg->data = malloc ( seg->length );
		if ( !seg->data )
			return E_FAIL;
		v_memcpy_get_raw ( source, seg->data, header.iCodeRelocOffset, seg->length );
		seg->name = ".codereloc";
	}

	/* check DataReloc section */
	if ( header.iDataRelocOffset )
	{
		seg = segment_create (  );
		if ( !seg )
			return E_FAIL;

		tmp = source->segments->length;

		if ( tmp < header.iDataRelocOffset )
		{
			ERR ( 0, "FATAL: iDataRelocOffset > end!" ); 
			return E_FAIL;
		}
		tmp -= header.iDataRelocOffset;

		if ( target->segments )
			segment_get_last ( target->segments )->next = seg;

		seg->start = 0;
		seg->length = tmp;
		seg->end = seg->start + seg->length;
		seg->flags |= FLAGS_HIDE_IN_MEM;
		seg->data = malloc ( seg->length );
		if ( !seg->data )
			return E_FAIL;
		v_memcpy_get_raw ( source, seg->data, header.iDataRelocOffset, seg->length );
		seg->name = ".datareloc";
	}

	return E_OK;
}


unsigned int
fmt_epoc_parse_sections ( t_stage *source, t_stage *target, E32ImageHeader header )
{
	t_segment *seg = NULL;
	unsigned int tmp = 0;

	if ( !source || !target )
		return E_FAIL;

	/* check TEXT segment (code/data)*/
	if ( header.iCodeOffset && header.iCodeSize )
	{
		seg = segment_create (  );
		if ( !seg )
			return E_FAIL;
		target->segments = seg;
		seg->start = header.iCodeBase;
		seg->length = header.iCodeSize;
		seg->end = seg->start + seg->length;
		seg->data = malloc ( seg->length );
		if ( !seg->data )
			return E_FAIL;
		v_memcpy_get_raw ( source, seg->data, header.iCodeOffset, seg->length );
		seg->name = ".text";
	}

	/* check data section */
	if ( header.iDataOffset )
	{
		seg = segment_create (  );
		if ( !seg )
			return E_FAIL;

		tmp = 0;
		if ( header.iImportOffset > tmp )
			tmp = header.iImportOffset;
		else if ( header.iCodeRelocOffset > tmp )
			tmp = header.iCodeRelocOffset;
		else if ( header.iDataRelocOffset > tmp )
			tmp = header.iDataRelocOffset;
		else
			tmp = source->segments->length;

		if ( tmp < header.iDataOffset )
		{
			ERR ( 0, "FATAL: iDataOffset > end!" ); 
			return E_FAIL;
		}
		tmp -= header.iDataOffset;

		if ( target->segments )
			segment_get_last ( target->segments )->next = seg;

		seg->start = header.iDataBase;
		seg->length = tmp;
		seg->end = seg->start + seg->length;
		seg->flags |= FLAGS_HIDE_IN_MEM;
		seg->data = malloc ( seg->length );
		if ( !seg->data )
			return E_FAIL;
		v_memcpy_get_raw ( source, seg->data, header.iDataOffset, seg->length );
		seg->name = ".data";
	}

	/* check import section */
	if ( header.iImportOffset )
	{
		seg = segment_create (  );
		if ( !seg )
			return E_FAIL;

		tmp = 0;
		if ( header.iCodeRelocOffset > tmp )
			tmp = header.iCodeRelocOffset;
		else if ( header.iDataRelocOffset > tmp )
			tmp = header.iDataRelocOffset;
		else
			tmp = source->segments->length;

		if ( tmp < header.iImportOffset )
		{
			ERR ( 0, "FATAL: iImportOffset > end!" ); 
			return E_FAIL;
		}
		tmp -= header.iImportOffset;

		if ( target->segments )
			segment_get_last ( target->segments )->next = seg;

		seg->start = 0;
		seg->length = tmp;
		seg->end = seg->start + seg->length;
		seg->flags |= FLAGS_HIDE_IN_MEM;
		seg->data = malloc ( seg->length );
		if ( !seg->data )
			return E_FAIL;
		v_memcpy_get_raw ( source, seg->data, header.iImportOffset, seg->length );
		seg->name = ".import";
	}



	/* check CodeReloc section */
	if ( header.iCodeRelocOffset )
	{
		seg = segment_create (  );
		if ( !seg )
			return E_FAIL;

		tmp = 0;
		if ( header.iDataRelocOffset > tmp )
			tmp = header.iDataRelocOffset;
		else
			tmp = source->segments->length;

		if ( tmp < header.iImportOffset )
		{
			ERR ( 0, "FATAL: iCodeRelocOffset > end!" ); 
			return E_FAIL;
		}
		tmp -= header.iCodeRelocOffset;

		if ( target->segments )
			segment_get_last ( target->segments )->next = seg;

		seg->start = 0;
		seg->length = tmp;
		seg->end = seg->start + seg->length;
		seg->flags |= FLAGS_HIDE_IN_MEM;
		seg->data = malloc ( seg->length );
		if ( !seg->data )
			return E_FAIL;
		v_memcpy_get_raw ( source, seg->data, header.iCodeRelocOffset, seg->length );
		seg->name = ".codereloc";
	}

	/* check DataReloc section */
	if ( header.iDataRelocOffset )
	{
		seg = segment_create (  );
		if ( !seg )
			return E_FAIL;

		tmp = source->segments->length;

		if ( tmp < header.iDataRelocOffset )
		{
			ERR ( 0, "FATAL: iDataRelocOffset > end!" ); 
			return E_FAIL;
		}
		tmp -= header.iDataRelocOffset;

		if ( target->segments )
			segment_get_last ( target->segments )->next = seg;

		seg->start = 0;
		seg->length = tmp;
		seg->end = seg->start + seg->length;
		seg->flags |= FLAGS_HIDE_IN_MEM;
		seg->data = malloc ( seg->length );
		if ( !seg->data )
			return E_FAIL;
		v_memcpy_get_raw ( source, seg->data, header.iDataRelocOffset, seg->length );
		seg->name = ".datareloc";
	}

	return E_OK;
}

unsigned int
fmt_epoc_parse ( t_stage * source, t_stage * target )
{
	unsigned int i = 0;
	E32ImageHeader header;
	E32ImageHeaderv2 headerv2;
	t_epoc_priv *priv;
	unsigned char *deflate_buffer = NULL;

	v_memcpy_get_raw ( source, &header, 0, sizeof (E32ImageHeader)  );
	v_memcpy_get_raw ( source, &headerv2, 0, sizeof (E32ImageHeaderv2)  );

	DBG ( DEBUG_FMT_EPOC,  "ident: '%c%c%c%c'\n", header.iSignature1, header.iSignature2, header.iSignature3, header.iSignature4 );
	if ( !(header.iSignature1 == EPOCMAG1 &&
		   header.iSignature2 == EPOCMAG2 &&
		   header.iSignature3 == EPOCMAG3 &&
		   header.iSignature4 == EPOCMAG4) )
	{
		DBG ( DEBUG_FMT_EPOC,  "Unknown file\n" );
		return E_FAIL;
	}
	DBG ( DEBUG_FMT_EPOC,  "This is a EPOC File" );
	source->flags &= ~FLAGS_ENDIANESS;
	source->flags |= FLAGS_ENDIANESS_LE;
	target->flags &= ~FLAGS_ENDIANESS;
	target->flags |= FLAGS_ENDIANESS_LE;


	target->priv = PRIV_MALLOC(t_epoc_priv);
	priv = target->priv;

	if ( (header.iVersion & 0xFFFF) == 2 || (header.iVersion & 0xFFFF) == 0x0102 )
	{
		DBG ( DEBUG_FMT_EPOC,  "New EPOC format for Symbian OS 9" );

		priv->version = 2;

//		DBG ( DEBUG_FMT_EPOC,  "iCheckSumCode: %08X\n", headerv2.iCheckSumCode );
//		DBG ( DEBUG_FMT_EPOC,  "iCheckSumData: %08X\n", headerv2.iCheckSumData );
		DBG ( DEBUG_FMT_EPOC,  "iCodeBase    : %08X\n", headerv2.iCodeBase );
		DBG ( DEBUG_FMT_EPOC,  "iCodeSize    : %08X\n", headerv2.iCodeSize );
		DBG ( DEBUG_FMT_EPOC,  "iCodeOffset  : %08X\n", headerv2.iCodeOffset );
		DBG ( DEBUG_FMT_EPOC,  "iDataBase    : %08X\n", headerv2.iDataBase );
		DBG ( DEBUG_FMT_EPOC,  "iDataSize    : %08X\n", headerv2.iDataSize );
		DBG ( DEBUG_FMT_EPOC,  "iDataOffset  : %08X\n", headerv2.iDataOffset );
		DBG ( DEBUG_FMT_EPOC,  "iEntryPoint  : %08X\n", headerv2.iEntryPoint );
		DBG ( DEBUG_FMT_EPOC,  "iUncompressedSize  : %08X\n", headerv2.iUncompressedSize );
		DBG ( DEBUG_FMT_EPOC,  "iDllRefTableCount  : %08X\n", headerv2.iDllRefTableCount );
		DBG ( DEBUG_FMT_EPOC,  "iImportOffset  : %08X\n", headerv2.iImportOffset );

		if ( headerv2.iCodeOffset > sizeof ( E32ImageHeaderv2 ) )
		{
			ERR ( 0, "Invalid header size. Header too big! (%i > %i)", headerv2.iCodeOffset, sizeof ( E32ImageHeaderv2 ) );
			free ( priv );
			return E_FAIL;
		}

		//mem_check_all();

		memcpy ( &priv->original_header_v2, &headerv2, sizeof ( E32ImageHeaderv2 ) );

		if ( headerv2.iCompressionType == KUidCompressionDeflate )
		{			
			int outlen = headerv2.iUncompressedSize;
			int inlen = source->segments->length - headerv2.iCodeOffset;

			deflate_buffer = e32image_deflate_decompress (
				source->segments->data + headerv2.iCodeOffset, inlen, outlen );

			//mem_check_all();

			source->segments->data = (unsigned char*)realloc ( source->segments->data, headerv2.iUncompressedSize + headerv2.iCodeOffset );
			source->segments->length = headerv2.iUncompressedSize + headerv2.iCodeOffset;
			source->segments->end = source->segments->start + source->segments->length;

			source->length = headerv2.iUncompressedSize + headerv2.iCodeOffset;

			if ( !deflate_buffer )
			{
				ERR ( 0, "could not deflate compressed part" );	
				free ( priv );
				return E_FAIL;
			}
			
			memcpy ( source->segments->data + headerv2.iCodeOffset, deflate_buffer, headerv2.iUncompressedSize );

			//mem_check_all();
		}

		fmt_epoc_parse_sections_v2 ( source, target, headerv2 );

		if ( deflate_buffer )
		{
			free ( deflate_buffer );
			deflate_buffer = NULL;
		}
		//mem_check_all();

	} 
	else if ( (header.iVersion & 0xFFFF) == 1 )
	{
		priv->version = 1;

		DBG ( DEBUG_FMT_EPOC,  "iCheckSumCode: %08X\n", header.iCheckSumCode );
		DBG ( DEBUG_FMT_EPOC,  "iCheckSumData: %08X\n", header.iCheckSumData );
		DBG ( DEBUG_FMT_EPOC,  "iCodeBase    : %08X\n", header.iCodeBase );
		DBG ( DEBUG_FMT_EPOC,  "iCodeSize    : %08X\n", header.iCodeSize );
		DBG ( DEBUG_FMT_EPOC,  "iCodeOffset  : %08X\n", header.iCodeOffset );
		DBG ( DEBUG_FMT_EPOC,  "iDataBase    : %08X\n", header.iDataBase );
		DBG ( DEBUG_FMT_EPOC,  "iDataSize    : %08X\n", header.iDataSize );
		DBG ( DEBUG_FMT_EPOC,  "iDataOffset  : %08X\n", header.iDataOffset );
		DBG ( DEBUG_FMT_EPOC,  "iEntryPoint  : %08X\n", header.iEntryPoint );

		memcpy ( &priv->original_header, &header, sizeof ( E32ImageHeader ) );
		fmt_epoc_parse_sections ( source, target, header );
	}
	else
	{
		// unknown file format
		DBG ( DEBUG_FMT_EPOC,  "Unknown file format (not v1 nor v2)\n" );
		free ( priv );
		return E_FAIL;
	}

	return E_OK;
}

unsigned int
fmt_epoc_decode ( t_stage * source, t_stage * target )
{

	if ( !fmt_epoc_enabled )
	{
		DBG ( DEBUG_FMT, " -> %s ( ) disabled !!\n", __FUNCTION__ );
		return E_FAIL;
	}

	DBG ( DEBUG_FMT, " => %s ( ) called\n", __FUNCTION__ );

	if ( !target )
		target = source->next;

	if ( !source || !target || !source->segments || !source->segments->data )
	{
		DBG ( DEBUG_FMT, " -> %s ( ) failed !!\n", __FUNCTION__ );
		return E_FAIL;
	}

	if ( source->length < sizeof (E32ImageHeaderv2) )
	{
		DBG ( DEBUG_FMT, " -> %s ( %s, %s ) failed !!\n", __FUNCTION__, source->name, target->name );
		return E_FAIL;
	}

	R(fmt_epoc_parse ( source,  target ));

	target->name = "PARSED";
	target->length = 0;
	target->parser = "EPOC";
	target->type = "EPOC";


	DBG ( DEBUG_FMT, " ## %s ( %s, %s ) done\n", __FUNCTION__, source->name, target->name );
	return E_OK;
}


char *
fmt_epoc_create ( t_stage * source, unsigned int *ret_length )
{
	unsigned int length = 0;
	char *data = NULL;
	t_epoc_priv *priv = NULL;
	E32ImageHeader header;
	t_segment *seg = NULL;

	if ( !source || !source->priv )
		return NULL;

	priv = ( t_epoc_priv * ) source->priv;

	length = sizeof ( priv->original_header );
	data = malloc ( length );
	if ( !data )
		return NULL;

	memcpy ( &header, &priv->original_header, sizeof ( header ) );

	seg = source->segments;
	while ( seg )
	{
		if ( !strcmp ( seg->name, ".text" ) )
		{
			/* TEXT segment */
			header.iCodeOffset = length;
			header.iCodeBase = seg->start;
			header.iCodeSize = seg->length;
			header.iCheckSumCode = fmt_epoc_calc_checksum ( seg->data, seg->length );
		}
		else if ( !strcmp ( seg->name, ".data" ) )
		{
			/* DATA segment */
			header.iDataOffset = length;
			header.iDataBase = seg->start;
			header.iDataSize = seg->length;
			header.iCheckSumData = fmt_epoc_calc_checksum ( seg->data, seg->length );
		}
		else if ( !strcmp ( seg->name, ".import" ) )
		{
			/* import segment */
			header.iImportOffset = length;
		}
		else if ( !strcmp ( seg->name, ".codereloc" ) )
		{
			/* codereloc segment */
			header.iCodeRelocOffset = length;
		}
		else if ( !strcmp ( seg->name, ".datareloc" ) )
		{
			/* datareloc segment */
			header.iDataRelocOffset = length;
		}
		else
		{
			ERR ( 0, "Invalid data segment: <%s>", seg->name );
		}

		data = realloc ( data, length+seg->length );
		if ( !data )
			return NULL;
		memcpy ( data+length, seg->data, seg->length );

		length += seg->length;

		LIST_NEXT(seg);
	}

	memcpy ( data, &header, sizeof ( priv->original_header ) );

	*ret_length = length;
	return data;
}

unsigned int
fmt_epoc_encode ( t_stage * source, t_stage * target )
{
	char * data = NULL;
	unsigned int length = 0;

	if ( !fmt_epoc_enabled )
	{
		DBG ( DEBUG_FMT, " -> %s ( ) disabled !!\n", __FUNCTION__ );
		return E_FAIL;
	}

	DBG ( DEBUG_FMT, " => %s ( ) called\n", __FUNCTION__ );
	if ( !target )
		target = source->next;


	if ( !source || !target )
	{
		
		DBG ( DEBUG_FMT, " -> %s ( ) failed !!\n", __FUNCTION__ );
		return E_FAIL;
	}

	if ( segment_count ( source->segments ) < 1 )
	{
		
		DBG ( DEBUG_FMT, " -> %s ( %s, %s ) failed !!\n", __FUNCTION__, source->name, target->name );
		return E_FAIL;
	}

	data = fmt_epoc_create ( source, &length );

	if ( !data )
	{
		
		DBG ( DEBUG_FMT, " -> %s ( %s, %s ) failed !!\n", __FUNCTION__, source->name, target->name );
		return E_FAIL;
	}

	target->segments = segment_create (  );
	target->segments->name = "DATA";
	target->segments->start = 0;
	target->segments->end = 0;
	target->segments->length = length;
	target->segments->data = data;

	target->name = "RAW";
	target->length = length;
	target->parser = "EPOC";
	target->type = "EPOC";

	/* set the parser again (see file_format() ). all parsers should do this. shouldn't they? */
	source->parser = "EPOC";
	source->type = "EPOC";


	DBG ( DEBUG_FMT, " ## %s ( %s, %s ) done\n", __FUNCTION__, source->name, target->name );
	return E_OK;

}

t_fmt_handler epoc_handler = {
	"EPOC",
	"FORMAT",
	"fmt_epoc_decode",
	fmt_epoc_decode,
	"fmt_epoc_encode",
	fmt_epoc_encode,
	"fmt_epoc_free",
	fmt_epoc_free,
	NULL,
	NULL
};


unsigned int
fmt_epoc_init ( )
{
	fmt_register_handler ( &epoc_handler );
	options_add_core_option ( OPT_BOOL, "fmt.epoc", fmt_epoc_enabled, "Enable EPOC Format. Disabled by default since there are some bugs" );
	return E_OK;
}

#endif
