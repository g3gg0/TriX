#ifndef __TRIX_CHUNK_TONE_C__
#define __TRIX_CHUNK_TONE_C__

#include "defines.h"
#include "seer.h"
#include "segment.h"
#include "stage.h"
#include "file_io.h"
#include "file.h"
#include "workspace.h"
#include "util.h"
#include "options.h"
#include "mem.h"
#include "treenode.h"
#include "trixplug.h"
#include "ppmodify.h"

extern int dump_chunks;
extern int endianess_be;
extern struct trix_functions *ft;

#define TONE_HDR_UNK    0
#define TONE_HDR_DCT3   1
#define TONE_HDR_DCT4   2
#define TONE_HDR_BB5    3

#define TONE_TYPE_UNK   0
#define TONE_TYPE_RE    1
#define TONE_TYPE_MIDI  2
#define TONE_TYPE_AAC   3
#define TONE_TYPE_WAV   4

extern int tone_header_ver;
extern int alignment_byte;

#include "trixplug_wrapper.h"

//-------------------------------------------------------------------------------------------------

unsigned int ppmodify_build_tone_subchunk ( t_treenode *node )
{
	t_treenode *infonode = NULL;
	t_treenode *datanode = NULL;
	t_treenode *worknode = NULL;

	subchunk_hdr header;
	unsigned char *buffer = NULL;
	unsigned char *tone_name = NULL;
	unsigned char *tone_data = NULL;
	unsigned int data_len = 0;
	unsigned int name_len = 0;
	t_workspace *ws = NULL;
	unsigned int hdr_size = 0;
	unsigned int pos = 0;
	unsigned int tone_id = 0;
	unsigned int length = 0;
	unsigned int alignedlength = 0;

	if ( !node )
		PLUGIN_ERROR ( "(!node) failed", E_FAIL );

	// retrieve chunk headers
	memset ( header.name, 0x00, 0x04 );

	infonode = treenode_get_byname ( treenode_children ( node ), "INFORMATION" );
	if ( !infonode )
		PLUGIN_ERROR ( "failed retrieving INFORMATION", E_FAIL );

	worknode = treenode_get_byname ( infonode, "ID" );
	if ( !worknode )
		PLUGIN_ERROR ( "failed retrieving INFORMATION/ID content", E_FAIL );
	header.id = treenode_get_content_data_hexval ( worknode );

	worknode = treenode_get_byname ( infonode, "NAME" );
	if ( !worknode )
		PLUGIN_ERROR ( "failed retrieving INFORMATION/NAME content", E_FAIL );
	strcpy ( header.name, treenode_get_content_data ( worknode ) );


	if( !header.id || !strcmp ( header.name, "_END" ) || !strcmp ( header.name, "TGRP" ) )
		return ppmodify_build_ppm_subchunk ( node );

	worknode = treenode_get_byname ( infonode, "LENGTH" );
	if ( !worknode )
		PLUGIN_ERROR ( "failed retrieving INFORMATION/LENGTH content", E_FAIL );
	header.length = treenode_get_content_data_hexval ( worknode );

	worknode = treenode_get_byname ( infonode, "FLAGS1" );
	if ( !worknode )
		PLUGIN_ERROR ( "failed retrieving INFORMATION/FLAGS1 content", E_FAIL );
	header.flags1 = treenode_get_content_data_hexval ( worknode );

	worknode = treenode_get_byname ( infonode, "FLAGS2" );
	if ( !worknode )
		PLUGIN_ERROR ( "failed retrieving INFORMATION/FLAGS2 content", E_FAIL );
	header.flags2 = treenode_get_content_data_hexval ( worknode );

	worknode = treenode_get_byname ( infonode, "FLAGS3" );
	if ( !worknode )
		header.flags3 = 0;
	else
		header.flags3 = treenode_get_content_data_hexval ( worknode );

	worknode = treenode_get_byname ( infonode, "FLAGS4" );
	if ( !worknode )
		header.flags4 = 0;
	else
		header.flags4 = treenode_get_content_data_hexval ( worknode );

	// get chunk data
	worknode = treenode_get_byname ( treenode_children ( node ), "TONE" );
	if ( worknode )
	{
		datanode = treenode_get_byname ( treenode_children ( worknode ), "ID" );
		if ( datanode )
		{
			tone_id = treenode_get_content_data_hexval ( datanode );
			treenode_release ( datanode );
		}

		datanode = treenode_get_byname ( treenode_children ( worknode ), "NAME" );
		if ( datanode )
			tone_name = strdup ( treenode_get_content_data ( datanode ) );

		if ( !tone_name )
			tone_name = strdup ( "" );

		name_len = strlen ( tone_name ) + 1;

		if ( tone_header_ver == TONE_HDR_DCT3 )
		{
			hdr_size = 0x10 + name_len + 1;
			buffer = malloc ( hdr_size );
			memcpy ( buffer + 0x11, tone_name, name_len );
			buffer[0x10] = name_len;
		}
		else if ( tone_header_ver == TONE_HDR_DCT4 )
		{
			hdr_size = 0x10 + name_len * 2 + 4;
			buffer = malloc ( hdr_size );
			memcpy ( buffer + 0x14, tone_name, name_len );
		}
		else if ( tone_header_ver == TONE_HDR_BB5 )
		{
			hdr_size = 0x10 + name_len * 2 + 8;
			buffer = malloc ( hdr_size );
			memcpy ( buffer + 0x18, tone_name, name_len );
		}
		else
			PLUGIN_ERROR ( "Unknown TONE header version", E_FAIL );

		treenode_release ( datanode );
		CHECK_AND_FREE ( tone_name );

		datanode = treenode_get_byname ( treenode_children ( worknode ), "DUMP" );
		if ( datanode )
			tone_data = treenode_get_content_data_binary ( datanode, &data_len );
		if ( !tone_data )
		{
			tone_data = strdup ( "" );
			data_len = 0;
		}

		length = hdr_size + data_len;
		alignedlength = ALIGN_WORD ( length );
		buffer = realloc ( buffer, alignedlength );
		if ( !buffer )
			return E_FAIL;

		memcpy ( buffer + hdr_size, tone_data, data_len );
		memset ( buffer + length, alignment_byte, alignedlength - length );
		CHECK_AND_FREE ( tone_data );
		treenode_release ( datanode );

		ws = workspace_create_file_from_buffer ( buffer, alignedlength );
		if ( !ws )
			PLUGIN_ERROR ( "failed creating temporary workspace", E_FAIL );

		ws->flags &= ~FLAGS_ENDIANESS;
		if ( endianess_be )
			ws->flags |= FLAGS_ENDIANESS_BE;
		else
			ws->flags |= FLAGS_ENDIANESS_LE;

		v_set_w ( ws, 0x00, header.id );

		if ( header.length != alignedlength )
			printf ( " [i] SubChunk '%s' length changed from %i to %i\n", header.name, header.length, alignedlength );

		v_set_w ( ws, 0x04, alignedlength );
		v_memcpy_put ( ws, 0x08, header.name, 4 );
		v_set_b ( ws, 0x0C, header.flags1 );
		v_set_b ( ws, 0x0D, header.flags2 );
		v_set_b ( ws, 0x0E, header.flags3 );
		v_set_b ( ws, 0x0F, header.flags4 );

		if ( tone_header_ver == TONE_HDR_DCT4 || tone_header_ver == TONE_HDR_BB5 )
		{
			v_set_b ( ws, 0x10, tone_id );
			v_set_b ( ws, 0x11, name_len );
			if ( tone_header_ver == TONE_HDR_DCT4 )
			{
				v_set_h ( ws, 0x12, data_len );
				pos = 0x14;
			}
			else
			{
				v_set_h ( ws, 0x12, 0 );
				v_set_w ( ws, 0x14, data_len );
				pos = 0x18;
			}

			while ( name_len-- )
				v_set_h ( ws, pos + name_len * 2, v_get_b ( ws, pos + name_len ) );
		}
	}

	// replace with constructed data
	treenode_set_name ( node, "SUBCHUNK-BINARY" );
	treenode_set_content_data_binary ( node, buffer, length );

	workspace_release ( ws );

	return E_OK;
}

unsigned int ppmodify_tone_subchunk_dump_tone ( t_treenode *headernode, t_workspace *ws, unsigned int start, unsigned int length )
{
	t_treenode *node = NULL;
	unsigned int pos = start;
	char *tone_name = NULL;
	char *tone_data = NULL;
	unsigned int data_len = 0;
	unsigned int name_len = 0;
	unsigned int tone_type = TONE_TYPE_UNK;
	unsigned int tone_id = 0;
	unsigned int i = 0;

	if ( !length || !headernode || !ws )
		return E_FAIL;


	if ( tone_header_ver == TONE_HDR_DCT3 )
	{
		name_len = v_get_b ( ws, start );
		tone_name = malloc ( name_len );
		v_memcpy_get ( ws, tone_name, start + 1, name_len );

		pos = start + name_len + 1;
		data_len = length - name_len - 1;

		tone_data = malloc ( data_len );
		v_memcpy_get ( ws, tone_data, pos, data_len );
		tone_type = TONE_TYPE_RE;
	}
	else if ( tone_header_ver == TONE_HDR_DCT4 || tone_header_ver == TONE_HDR_BB5 )
	{
		tone_id = v_get_b ( ws, start );
		name_len = v_get_b ( ws, start + 1 );
		tone_name = malloc ( name_len );

		if ( tone_header_ver == TONE_HDR_DCT4 )
		{
			data_len = v_get_h ( ws, pos + 2 );
			pos += 4;
		}
		else
		{
			data_len = v_get_w ( ws, pos + 4 );
			pos += 8;
		}

		while ( v_get_h ( ws, pos ) )
		{
			tone_name[i] = v_get_h ( ws, pos );
			i++;
			pos += 2;
		}
		tone_name[i] = 0;
		pos += 2;

		tone_data = malloc ( data_len );
		v_memcpy_get ( ws, tone_data, pos, data_len );

		if ( !strncmp ( "MThd", tone_data, 4 ) )
			tone_type = TONE_TYPE_MIDI;
		else if ( v_get_b ( ws, pos ) == 0xFF ) // AAC
			tone_type = TONE_TYPE_AAC;
		else if ( 0 ) // WAV
			tone_type = TONE_TYPE_WAV;
		else // RE
			tone_type = TONE_TYPE_RE;
	}
	else
		PLUGIN_ERROR ( "Unknown TONE header version", E_FAIL );


	node = treenode_addchild ( headernode );
	treenode_set_name ( node, "NAME" );
	treenode_set_content_data ( node, tone_name );

	node = treenode_addchild ( headernode );
	treenode_set_name ( node, "TYPE" );

	switch ( tone_type )
	{
		case TONE_TYPE_RE:
			treenode_set_content_data ( node, "RE" );
			break;
		case TONE_TYPE_MIDI:
			treenode_set_content_data ( node, "MIDI" );
			break;
		case TONE_TYPE_AAC:
			treenode_set_content_data ( node, "AAC" );
			break;
		case TONE_TYPE_WAV:
			treenode_set_content_data ( node, "WAV" );
			break;
	}

	node = treenode_addchild ( headernode );
	treenode_set_name ( node, "ID" );
	treenode_set_content_data_hexval ( node, tone_id );

	node = treenode_addchild ( headernode );
	treenode_set_name ( node, "DUMP" );
	treenode_set_content_data_binary ( node, tone_data, data_len );

	CHECK_AND_FREE (tone_name);
	CHECK_AND_FREE (tone_data);

	return E_OK;
}

unsigned int ppmodify_tone_subchunk_dump ( t_workspace *ws, unsigned int start, unsigned int length, t_treenode *node )
{
	subchunk_hdr header;
	t_treenode *headernode = NULL;
	t_treenode *worknode = NULL;
	char *buff = NULL;
	unsigned hdr_size=0;

	if ( !ws || !node || !v_valid ( ws, start ) )
		PLUGIN_ERROR ( "( !ws || !node || !v_valid ( ws, start ) ) failed", E_FAIL );

	header.id = v_get_w ( ws, start + 0x00 );
	header.length = v_get_w ( ws, start + 0x04 );
	v_memcpy_get ( ws, header.name, start + 0x08, 4 );
	header.flags1 = v_get_b ( ws, start + 0x0C );
	header.flags2 = v_get_b ( ws, start + 0x0D );
	header.flags3 = v_get_b ( ws, start + 0x0E );
	header.flags4 = v_get_b ( ws, start + 0x0F );

	if(!header.id) // terminator
		return ppmodify_ppm_subchunk_dump ( ws, start, length, node );

	if ( header.length < 0x10 )
		header.length = 0x10;

	if ( header.length > length )
		PLUGIN_ERROR ( "header.length > length", E_FAIL );

	if ( header.length > v_get_size ( ws ) )
		PLUGIN_ERROR ( "header.length > filesize", E_FAIL );

	HEAP_CHECK;
	header.name[4] = '\000';
	if ( dump_chunks )
	{
		printf ( "    Position: 0x%08X\n", start );
		printf ( "    SubChunk: '%s'\n", header.name );
		printf ( "    Length  : 0x%08X\n", header.length );
		printf ( "    Flags1  : 0x%02X\n", header.flags1 );
		printf ( "    Flags2  : 0x%02X\n", header.flags2 );
		printf ( "    Flags3  : 0x%02X\n", header.flags3 );
		printf ( "    Flags4  : 0x%02X\n", header.flags4 );
		printf ( "    ID      : 0x%08X\n", header.id );
		printf ( "\n" );
	}
	treenode_set_name ( node, "SUBCHUNK" );

	headernode = treenode_addchild ( node );
	treenode_set_name ( headernode, "INFORMATION" );

	worknode = treenode_addchild ( headernode );
	treenode_set_name ( worknode, "ID" );
	treenode_set_content_data_hexval ( worknode, header.id );

	worknode = treenode_addchild ( headernode );
	treenode_set_name ( worknode, "NAME" );
	treenode_set_content_type ( worknode, TREENODE_TYPE_ASCII );
	treenode_set_content_data ( worknode, header.name );

	worknode = treenode_addchild ( headernode );
	treenode_set_name ( worknode, "LENGTH" );
	treenode_set_content_type ( worknode, TREENODE_TYPE_HEXVAL );
	treenode_set_content_data_hexval ( worknode, header.length );

	worknode = treenode_addchild ( headernode );
	treenode_set_name ( worknode, "FLAGS1" );
	treenode_set_content_type ( worknode, TREENODE_TYPE_HEXVAL );
	treenode_set_content_data_hexval ( worknode, header.flags1 );

	worknode = treenode_addchild ( headernode );
	treenode_set_name ( worknode, "FLAGS2" );
	treenode_set_content_type ( worknode, TREENODE_TYPE_HEXVAL );
	treenode_set_content_data_hexval ( worknode, header.flags2 );

	worknode = treenode_addchild ( headernode );
	treenode_set_name ( worknode, "FLAGS3" );
	treenode_set_content_type ( worknode, TREENODE_TYPE_HEXVAL );
	treenode_set_content_data_hexval ( worknode, header.flags3 );

	worknode = treenode_addchild ( headernode );
	treenode_set_name ( worknode, "FLAGS4" );
	treenode_set_content_type ( worknode, TREENODE_TYPE_HEXVAL );
	treenode_set_content_data_hexval ( worknode, header.flags4 );

	if ( !strcmp ( header.name, "_END" ) || !strcmp ( header.name, "TGRP" ) )
	{
		worknode = treenode_addchild ( node );
		treenode_set_name ( worknode, "DUMP" );
		buff = malloc ( header.length - 0x10 );
		v_memcpy_get ( ws, buff, start + 0x10, header.length - 0x10 );
		treenode_set_content_type ( worknode, TREENODE_TYPE_HEXCODED );
		treenode_set_content_data_binary ( worknode, buff, header.length - 0x10 );
		CHECK_AND_FREE ( buff );
	}
	else
	{
		// detect tone header version
		if ( tone_header_ver == TONE_HDR_UNK )
		{
			if ( ( v_get_strlen ( ws, start + 0x11 ) + 1 == v_get_b ( ws, start + 0x10 ) )
					&& ( v_get_b ( ws, start + 0x11 ) >= 0x20 )
			)
				tone_header_ver = TONE_HDR_DCT3;
			else if ( v_get_h ( ws, start + 0x12 ) ) // dct4
				tone_header_ver = TONE_HDR_DCT4;
			else if ( v_get_w ( ws, start + 0x14 ) ) // dc4+/bb5
				tone_header_ver = TONE_HDR_BB5;
			else
				PLUGIN_ERROR ( "Unknown TONE header version", E_FAIL );
		}

		headernode = treenode_addchild ( node );
		treenode_set_name ( headernode, "TONE" );
		ppmodify_tone_subchunk_dump_tone ( headernode, ws, start + 0x10, header.length - 0x10 );
	}

	return E_OK;
}

#endif /* __TRIX_CHUNK_TONE_C__ */
