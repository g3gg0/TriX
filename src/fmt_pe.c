#ifndef __TRIX_FMT_PE_C__
#define __TRIX_FMT_PE_C__

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
#include "pe.h"
#include "fmt_pe.h"

#include "mem.h"

unsigned int fmt_pe_enabled = 1;


unsigned int fmt_pe_free ( t_stage *s )
{
	DBG ( DEBUG_FMT, " => %s ( ) called\n", __FUNCTION__ );

	return E_OK;
}


unsigned int fmt_pe_add_section ( t_stage *source, t_stage *target, char *name, t_section_header shdr, unsigned int ImageBase, unsigned int section_size )
{
	t_segment *s = NULL;
	t_pe_sect_priv *priv = NULL;
	unsigned char *data = NULL;
	
	data = calloc ( section_size + 1, sizeof ( char ) );
	if ( !data )
		return E_FAIL;
	
	if ( shdr.Characteristics != IMAGE_SCN_LNK_REMOVE )
		v_memcpy_get_raw ( source, data, shdr.PointerToRawData, shdr.SizeOfRawData );

	if ( !target->segments )
	{
		target->segments = segment_create ();
		s = target->segments;
	}
	else
		s = segment_add ( target->segments );
	
	s->name = strdup ( name );
	s->start = ImageBase + shdr.VirtualAddress;
	s->length = section_size;
	s->end = s->start + s->length;
	s->data = data;
	s->flags = FLAGS_FREE_NAME | target->flags;

	if ( shdr.Characteristics == IMAGE_SCN_LNK_REMOVE )
		segment_hide_in_mem ( s );

	s->priv = PRIV_MALLOC ( t_pe_sect_priv );
	priv = s->priv;

	priv->section_size = section_size;
	memcpy ( &priv->header, &shdr, sizeof ( t_section_header ) );

	return E_OK;
}

unsigned int fmt_pe_parse_sections ( t_stage *source, t_stage *target, unsigned int start_offset, unsigned int headers )
{
	t_section_header shdr;
	t_pe_priv *priv = NULL;
	unsigned int offset = start_offset;
	unsigned int prev_VirtualAddress = E_FAIL;
	unsigned int section_size = 0;
	unsigned int ImageBase = 0;
	unsigned int i = 0;


	if ( !source || !target || !headers )
		return E_FAIL;

	priv = target->priv;

	DBG ( DEBUG_FMT_PE, " Name      VirtSize   VirtAddr   SizeRaw    PtrRaw     Flags" );
	DBG ( DEBUG_FMT_PE, "------------------------------------------------------------------" );

	ImageBase = priv->hdr32.OptionalHeader.ImageBase;

	while ( headers )
	{
		v_memcpy_get_raw ( source, &shdr, offset, sizeof ( t_section_header ) );

		DBG ( DEBUG_FMT_PE, " %-8s  %08Xh  %08Xh  %08Xh  %08Xh  %08Xh", shdr.Name, shdr.Misc.VirtualSize,
			shdr.VirtualAddress + ImageBase, shdr.SizeOfRawData, shdr.PointerToRawData, shdr.Characteristics );

		if ( prev_VirtualAddress == E_FAIL ) // first section in file
		{
			if ( shdr.SizeOfRawData )
				section_size = shdr.SizeOfRawData;
			else
				section_size = shdr.Misc.VirtualSize; // how to properly detect it? :?

			section_size = ALIGN_BY ( section_size, priv->hdr32.OptionalHeader.SectionAlignment );
		}
		else
			section_size = shdr.VirtualAddress - prev_VirtualAddress;

		R ( fmt_pe_add_section ( source, target, shdr.Name, shdr, ImageBase, section_size ) );

		offset += sizeof ( t_section_header );
		headers--;
	}



	return E_OK;
}

unsigned int fmt_pe_parse ( t_stage *source, t_stage *target )
{
	t_dos_header hdr;
	t_image_nt_header32 hdr32;
	t_pe_priv *priv = NULL;

	v_memcpy_get_raw ( source, &hdr, 0, sizeof ( t_dos_header ) );

	if ( hdr.e_magic != IMAGE_DOS_SIGNATURE || !hdr.e_lfanew )
		return E_FAIL;

	v_memcpy_get_raw ( source, &hdr32, hdr.e_lfanew, sizeof ( t_image_nt_header32 ) );

	if ( hdr32.Signature != IMAGE_NT_SIGNATURE ||
	     hdr32.OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR32_MAGIC
	)
		return E_FAIL;


	DBG ( DEBUG_FMT_PE,  "PE File, " );
	switch ( hdr32.FileHeader.Machine )
	{
		case IMAGE_FILE_MACHINE_I386:
			DBG ( DEBUG_FMT_PE,  "Intel 386" );
			break;

		case IMAGE_FILE_MACHINE_ARM:
			DBG ( DEBUG_FMT_PE,  "ARM Little-Endian" );
			break;

		/* TODO */

		default:
			DBG ( DEBUG_FMT_PE,  "invalid\n" );
			return E_FAIL;
	}

	if ( !hdr32.FileHeader.NumberOfSections )
	{
		DBG ( DEBUG_FMT_PE,  "No sections in file\n" );
		return E_FAIL;
	}


	source->flags &= ~FLAGS_ENDIANESS;
	source->flags |= FLAGS_ENDIANESS_LE;
	target->flags &= ~FLAGS_ENDIANESS;
	target->flags |= FLAGS_ENDIANESS_LE;


	target->priv = PRIV_MALLOC ( t_pe_priv );
	priv = target->priv;

	memcpy ( &priv->hdr, &hdr, sizeof ( t_dos_header ) );
	memcpy ( &priv->hdr32, &hdr32, sizeof ( t_image_nt_header32 ) );

	R ( fmt_pe_parse_sections ( source, target, hdr.e_lfanew + sizeof ( t_image_nt_header32 ), hdr32.FileHeader.NumberOfSections ) );

	return E_OK;
}


unsigned int fmt_pe_decode ( t_stage *source, t_stage *target )
{
	if ( !fmt_pe_enabled )
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

	if ( source->length < sizeof ( t_dos_header ) )
	{
		DBG ( DEBUG_FMT, " -> %s ( %s, %s ) failed !!\n", __FUNCTION__, source->name, target->name );
		return E_FAIL;
	}

	if ( fmt_pe_parse ( source, target ) != E_OK )
	{
		DBG ( DEBUG_FMT, " -> %s ( %s, %s ) failed !!\n", __FUNCTION__, source->name, target->name );
		return E_FAIL;
	}

	target->name = "PARSED";
	target->length = 0;
	target->parser = "PE";
	target->type = "PE";

	DBG ( DEBUG_FMT, " ## %s ( %s, %s ) done\n", __FUNCTION__, source->name, target->name );
	return E_OK;
}


unsigned int fmt_pe_encode ( t_stage *source, t_stage *target )
{
	if ( !fmt_pe_enabled )
	{
		DBG ( DEBUG_FMT, " -> %s ( ) disabled !!\n", __FUNCTION__ );
		return E_FAIL;
	}

	DBG ( DEBUG_FMT, " => %s ( ) called\n", __FUNCTION__ );
	if ( !target )
		target = source->next;

	if ( !source || !target || !source->segments )
	{
		DBG ( DEBUG_FMT, " -> %s ( ) failed !!\n", __FUNCTION__ );
		return E_FAIL;
	}

	/*if ( fmt_pe_encode_data ( source, target ) != E_OK )
		return E_FAIL;*/

	target->name = "RAW";
	target->length = 0;
	target->parser = "PE";
	target->type = "PE";


	/* set the parser again (see file_format() ). all parsers should do this. shouldn't they? */
	source->parser = "PE";
	source->type = "PE";

	DBG ( DEBUG_FMT, " ## %s ( %s, %s ) done\n", __FUNCTION__, source->name, target->name );
	return E_OK;
}


t_fmt_handler pe_handler = {
	"PE",
	"PARSER",
	"fmt_pe_decode",
	fmt_pe_decode,
	"fmt_pe_encode",
	fmt_pe_encode,
	"fmt_pe_free",
	fmt_pe_free,
	NULL,
	NULL
};


unsigned int fmt_pe_init ( )
{
	fmt_register_handler ( &pe_handler );

	options_add_core_option ( OPT_BOOL, "fmt.pe", fmt_pe_enabled, "Enable PE format." );
	return E_OK;
}

#endif
