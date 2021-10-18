#ifndef __TRIX_PPMODIFY_C__
#define __TRIX_PPMODIFY_C__

/*
	FMAN (CNT) notes:

		ID ID ID ID
			0x00000044: info
			0x00000050: Gallery Tone
			0x00000058: Gallery Graphics / simple file?
			0x0000004C: Java Games
			0x00000084: Java Applications
		    0x00000090: Java Applications MIDLet 
		    0x00000094: Java Applications MIDLet

*/

// when just unpack should be available
//#define PPMODIFY_CLIENT

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

int dump_chunks = 1;
int alignment_byte = 0x00;
//int auto_resize = 0;
int wipe_rest = 1;
int recompress = 0;
int bitmask_depth = 4;
int endianess_be = 1;
int ppm_header_ver = 0;
int ppmodd_ignore_utf16 = 1;
int ppmodd_ignore_tokens = 1;

int anim_as_unknown = 0;
int font_as_unknown = 0;
int vfnt_as_unknown = 0;
int text_as_unknown = 0;
int tone_as_unknown = 0;
int plmn_as_unknown = 0;
int font_build_textdump = 0;

int add_text_id = 1;
int add_anim_uid = 1;
int tone_header_ver = 0; //TONE_HDR_UNK

// include all the TriXPluG function/init stuff
TRIXPLUG_STUBS
#include "trixplug_wrapper.h"

OPT_BEGIN
   OPTION( OPT_BOOL, "Wipe area behind PPM", "wipe_rest", &wipe_rest )
   OPTION( OPT_BOOL, "Dump Chunk headers", "dump_chunks", &dump_chunks )
   OPTION( OPT_INT,  "PPM header ver (0=normal or 1=needed for DCT4+ and BB5)", "ppm_header_ver", &ppm_header_ver )
   OPTION( OPT_INT,  "Alignment byte value", "alignment_byte", &alignment_byte )
   OPTION( OPT_BOOL, "Handle as unknown chunk", "anim.anim_as_unknown", &anim_as_unknown )
   OPTION( OPT_BOOL, "Add unique anim ID", "anim.add_anim_uid", &add_anim_uid )
   OPTION( OPT_BOOL, "Handle as unknown chunk", "font.font_as_unknown", &font_as_unknown )
   OPTION( OPT_BOOL, "Handle as unknown chunk", "vfnt.vfnt_as_unknown", &vfnt_as_unknown )
   OPTION( OPT_BOOL, "Handle as unknown chunk", "tone.tone_as_unknown", &tone_as_unknown )
   OPTION( OPT_INT,  "TONE header version (0=detect, 1=DCT3, 2=DCT4, 3=BB5)", "tone.tone_header_ver", &tone_header_ver )
   OPTION( OPT_BOOL, "Handle as unknown chunk", "plmn.plmn_as_unknown", &plmn_as_unknown )
   OPTION( OPT_BOOL, "Build some textual dump in XML data", "font.font_build_textdump", &font_build_textdump )
   OPTION( OPT_BOOL, "Handle as unknown chunk", "text.text_as_unknown", &text_as_unknown )
   OPTION( OPT_BOOL, "Ignore UTF-16 field in XML data", "text.ignore_utf16", &ppmodd_ignore_utf16 )
   OPTION( OPT_BOOL, "Ignore TOKEN field in XML data", "text.ignore_tokens", &ppmodd_ignore_tokens )
   OPTION( OPT_BOOL, "Add text string ID (for MCU refs)", "text.add_text_id", &add_text_id )
OPT_FINISH


#define PPMODIFY_TITLE "PPModify"
#define PPMODIFY_MAJOR 0
#define PPMODIFY_MINOR 88


PLUGIN_INFO_BEGIN
   PLUGIN_NAME ( PPMODIFY_TITLE )
   PLUGIN_VERSION ( PPMODIFY_MAJOR, PPMODIFY_MINOR )
   PLUGIN_INIT ( ppmodify_init )
   PLUGIN_OPTIONS
PLUGIN_INFO_FINISH


// number of already dumped string entries
unsigned int entries_dumped;

// LPCS table
t_lpcs_dict *lpcs = NULL;

unsigned int ppmodify_init ( )
{
	REGISTER_HEADER ( "trix_ppmodify", PPMODIFY_HEADERS );
	PPMODIFY_SYMBOLS

	printf ( PPMODIFY_TITLE " v%01i.%01i Plugin Loaded", PPMODIFY_MAJOR, PPMODIFY_MINOR  );

	return E_OK;
}

unsigned int ppmodify_calc_checksum ( unsigned char *buffer, int length )
{
	unsigned char *tmp = NULL;
	t_workspace *ws = NULL;
	unsigned int checksum = 0;
	int pos = 0;

	if ( !buffer || length < 0 )
		return 0;

	tmp = malloc ( length );
	if ( !tmp )
		return 0;

	memcpy ( tmp, buffer, length );

	ws = workspace_create_file_from_buffer ( tmp, length );
	if ( !ws )
		return 0;

	ws->flags &= ~FLAGS_ENDIANESS;
	if ( endianess_be )
		ws->flags |= FLAGS_ENDIANESS_BE;
	else
		ws->flags |= FLAGS_ENDIANESS_LE;

	while ( pos+4 <= length )
	{
		checksum += v_get_w ( ws, pos );
		pos += 4;
	}

	if ( pos != length )
		return 0;

	workspace_release ( ws );

	return checksum;
}

unsigned int ppmodify_ppm_subchunk_length ( t_workspace *ws, int start )
{
	subchunk_hdr header;

	header.length = v_get_w ( ws, start + 0x04 );

	if ( header.length < 0x10 )
	{
		printf ( " [i] needed subchunk length fix\n" );
		header.length = 0x10;
	}

	return header.length;
}

unsigned int ppmodify_ppm_subchunk_dump ( t_workspace *ws, unsigned int start, unsigned int length, t_treenode *node )
{
	subchunk_hdr header;
	t_treenode *headernode = NULL;
	t_treenode *worknode = NULL;
	unsigned char *buffer = NULL;

	if ( !ws || !node || !v_valid ( ws, start ) )
		PLUGIN_ERROR ( "( !ws || !node || !v_valid ( ws, start ) ) failed", E_FAIL );

	header.id = v_get_w ( ws, start + 0x00 );
	header.length = v_get_w ( ws, start + 0x04 );
	v_memcpy_get ( ws, header.name, start + 0x08, 4 );
	header.flags1 = v_get_b ( ws, start + 0x0C );
	header.flags2 = v_get_b ( ws, start + 0x0D );
	header.flags3 = v_get_b ( ws, start + 0x0E );
	header.flags4 = v_get_b ( ws, start + 0x0F );

	if ( header.length < 0x10 )
		header.length = 0x10;

	if ( header.length > length )
		PLUGIN_ERROR ( "header.length > length", E_FAIL );

	if ( header.length > v_get_size ( ws ) )
		PLUGIN_ERROR ( "header.length > filesize", E_FAIL );

	buffer = malloc ( header.length - 0x10 + 1 );
	if ( !buffer )
		PLUGIN_ERROR ( "failed allocating buffer", E_FAIL );
	v_memcpy_get ( ws, buffer, start + 0x10, header.length - 0x10 );


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
	treenode_set_content_data_hexval ( worknode, header.length );

	worknode = treenode_addchild ( headernode );
	treenode_set_name ( worknode, "FLAGS1" );
	treenode_set_content_data_hexval ( worknode, header.flags1 );

	worknode = treenode_addchild ( headernode );
	treenode_set_name ( worknode, "FLAGS2" );
	treenode_set_content_data_hexval ( worknode, header.flags2 );

	worknode = treenode_addchild ( headernode );
	treenode_set_name ( worknode, "FLAGS3" );
	treenode_set_content_data_hexval ( worknode, header.flags3 );

	worknode = treenode_addchild ( headernode );
	treenode_set_name ( worknode, "FLAGS4" );
	treenode_set_content_data_hexval ( worknode, header.flags4 );

	headernode = treenode_addchild ( node );
	treenode_set_name ( headernode, "DUMP" );
	treenode_set_content_data_binary ( headernode, buffer, header.length - 0x10  );

	free ( buffer );

	return E_OK;
}


unsigned int ppmodify_ppm_chunk_length ( t_workspace *ws, unsigned int start )
{
	chunk_hdr header;

	header.length = v_get_w ( ws, start + 0x04 );
	if ( header.length < 0x14 )
		header.length = 0x14;

	return header.length;
}

unsigned int ppmodify_ppm_lpcs_dump ( t_workspace *ws, unsigned int start )
{
	chunk_hdr header;
	unsigned int length = 0;
	unsigned int i = 0;
	unsigned short utf16 = 0;

	CHECK_AND_FREE ( lpcs );

	lpcs = calloc ( 1, sizeof ( t_lpcs_dict ) );

	if ( !lpcs )
		PLUGIN_ERROR ( "Failed LPCS buffer allocating!", E_FAIL );

	do
	{
		header.length = v_get_w ( ws, start + 0x04 );

		// CHUNK names are endianess based
		header.name[0] = v_get_w ( ws, start + 0x08 ) >> 24;
		header.name[1] = v_get_w ( ws, start + 0x08 ) >> 16;
		header.name[2] = v_get_w ( ws, start + 0x08 ) >> 8;
		header.name[3] = v_get_w ( ws, start + 0x08 ) ;
		header.name[4] = '\000';

		if ( !strcmp ( "LPCS", header.name ) )
		{
			if ( header.length != 0x234 )
				PLUGIN_ERROR ( "LPCS header length != 0x234", E_FAIL );

			while ( i < 0x100 )
			{
				utf16 = v_get_h ( ws, start + 0x24 + i * 2 );
            lpcs->utf16[i] = utf16;
				lpcs->ch[utf16] = i;
				i++;
			}

			return E_OK;
		}

		length = ppmodify_ppm_chunk_length ( ws, start );
		start = ALIGN_WORD ( start + length );
	}
	while ( ( length != 0x24 ) && v_valid ( ws, start ) && length );

	return E_FAIL;
}

unsigned int ppmodify_ppm_chunk_dump ( t_workspace *ws, unsigned int start, t_treenode *node )
{
	chunk_hdr header;
	unsigned int address = 0;
	unsigned int length = 0;
	unsigned int avail = 0;
	unsigned int ret = 0;
	unsigned int comm_text_count=0;
	t_treenode *headernode = NULL;
	t_treenode *worknode = NULL;

	header.chksum = v_get_w ( ws, start + 0x00 );
	header.length = v_get_w ( ws, start + 0x04 );

	if ( header.length > v_get_size ( ws ) )
		PLUGIN_ERROR ( "header.length > filesize", E_FAIL );

	// CHUNK names are endianess based
	//v_memcpy_get ( ws, header.name, start + 0x08, 4 );
	header.name[0] = v_get_w ( ws, start + 0x08 ) >> 24;
	header.name[1] = v_get_w ( ws, start + 0x08 ) >> 16;
	header.name[2] = v_get_w ( ws, start + 0x08 ) >> 8;
	header.name[3] = v_get_w ( ws, start + 0x08 ) ;
	header.name[4] = '\000';

	v_memcpy_get ( ws, header.version, start + 0x0C, 8 );

	header.version[8] = '\000';

	if ( dump_chunks )
	{
		printf ( "Position: 0x%08X\n", start );
		printf ( "Chunk   : '%s'\n", header.name );
		printf ( "Version : '%s'\n", header.version );
		printf ( "Length  : 0x%08X\n", header.length );
		printf ( "Checksum: 0x%08X\n", header.chksum );
		printf ( "\n" );
	}
	treenode_set_name ( node, "CHUNK" );

	headernode = treenode_addchild ( node );
	treenode_set_name ( headernode, "INFORMATION" );
	worknode = treenode_addchild ( headernode );
	treenode_set_name ( worknode, "NAME" );
	treenode_set_content_type ( worknode, TREENODE_TYPE_ASCII );
	treenode_set_content_data ( worknode, header.name );

	worknode = treenode_addchild ( headernode );
	treenode_set_name ( worknode, "VERSION" );
	treenode_set_content_type ( worknode, TREENODE_TYPE_ASCII );
	treenode_set_content_data ( worknode, header.version );

	worknode = treenode_addchild ( headernode );
	treenode_set_name ( worknode, "LENGTH" );
	treenode_set_content_data_hexval ( worknode, header.length );

	worknode = treenode_addchild ( headernode );
	treenode_set_name ( worknode, "CHECKSUM" );
	treenode_set_content_data_hexval ( worknode, header.chksum );


	address = start + 0x14;
	avail = header.length - 0x14;

	entries_dumped = 0;

	do
	{
		worknode = treenode_addchild ( node );
		if ( !strcmp ( "TEXT", header.name ) && !text_as_unknown )
		{
			ret = ppmodify_text_subchunk_dump ( ws, address, avail, worknode, entries_dumped );
			if ( comm_text_count )
				entries_dumped = comm_text_count;
			else
				comm_text_count = entries_dumped;
		}
		else if ( !strcmp ( "ANIM", header.name ) && !anim_as_unknown )
			ret = ppmodify_anim_subchunk_dump ( ws, address, avail, worknode );
		else if ( !strcmp ( "FONT", header.name ) && !font_as_unknown )
			ret = ppmodify_font_subchunk_dump ( ws, address, avail, worknode );
		else if ( !strcmp ( "VFNT", header.name ) && !vfnt_as_unknown )
			ret = ppmodify_vfnt_subchunk_dump ( ws, address, avail, worknode );
		else if ( !strcmp ( "TONE", header.name ) && !tone_as_unknown )
			ret = ppmodify_tone_subchunk_dump ( ws, address, avail, worknode );
		else if ( !strcmp ( "PLMN", header.name ) && !plmn_as_unknown )
			ret = ppmodify_plmn_subchunk_dump ( ws, address, avail, worknode );
		else
			ret = ppmodify_ppm_subchunk_dump ( ws, address, avail, worknode );

		if ( ret != E_OK )
			return E_FAIL;

		length = ppmodify_ppm_subchunk_length ( ws, address );
		address += length;
		avail -= length;

		address = ALIGN_WORD ( address );
	}
	while ( ( address < start + header.length ) && v_valid ( ws, address ) && length );

	return E_OK;
}

unsigned int ppmodify_ppm_header_dump ( t_workspace *ws, unsigned int start )
{
	unsigned char tag[64];
	unsigned char version[64];
	unsigned char lang[64];
	t_treenode *node = NULL;
	t_treenode *headernode = NULL;
	t_treenode *worknode = NULL;
	unsigned int length = 0;

	if ( ws->flags & FLAGS_ENDIANESS_BE )
		endianess_be = 1;
	else
		endianess_be = 0;

	node = treenode_create ();

	if ( ppm_header_ver == 1 )
	{
		v_memcpy_get ( ws, tag, start + 0x00, 4 );
		v_memcpy_get ( ws, version, start + 0x04, 0x3c );
		v_memcpy_get ( ws, lang, start + 0x40, 4 );
		tag[0x04] = '\000';
		version[0x3c] = '\000';
		lang[0x04] = '\000';
	}
	else
	{
		v_memcpy_get ( ws, tag, start + 0x00, 4 );
		v_memcpy_get ( ws, version, start + 0x04, 0x24 );
		v_memcpy_get ( ws, lang, start + 0x28, 4 );
		tag[0x04] = '\000';
		version[0x24] = '\000';
		lang[0x04] = '\000';
	}

	if ( dump_chunks )
	{
		printf ( "Tag      : '%s'\n", tag );
		printf ( "Version  :\n%s\n\n", version );
		printf ( "Lang     : '%s'\n", lang );
		printf ( "Endianess: '%s'\n", endianess_be?"BE":"LE" );
	}

	treenode_set_name ( node, "PPM" );

	headernode = treenode_addchild ( node );
	treenode_set_name ( headernode, "INFORMATION" );
	worknode = treenode_addchild ( headernode );
	treenode_set_name ( worknode, "VERSION" );
	treenode_set_content_data ( worknode, version );

	worknode = treenode_addchild ( headernode );
	treenode_set_name ( worknode, "LANG" );
	treenode_set_content_data ( worknode, lang );

	worknode = treenode_addchild ( headernode );
	treenode_set_name ( worknode, "ENDIANESS" );
	treenode_set_content_data ( worknode, endianess_be?"BE":"LE" );

	worknode = treenode_addchild ( headernode );
	treenode_set_name ( worknode, "HEADERVER" );
	treenode_set_content_data_hexval ( worknode, ppm_header_ver );

	if ( ppm_header_ver == 1 )
		start += 0x44;
	else
		start += 0x2C;

	ppmodify_ppm_lpcs_dump ( ws, start );

	do
	{
		worknode = treenode_addchild ( node );

		if ( ppmodify_ppm_chunk_dump ( ws, start, worknode ) != E_OK )
		{
			treenode_release ( node );
			return E_FAIL;
		}
		length = ppmodify_ppm_chunk_length ( ws, start );
		start += length;

		start = ALIGN_WORD ( start );
	}
	while ( ( length != 0x24 ) && v_valid ( ws, start ) && length );

	treenode_dump ( node );

	return E_OK;
}

t_treenode * ppmodify_ppm_header_dump_xml ( t_workspace *ws, unsigned int start )
{
	unsigned char tag[64];
	unsigned char version[64];
	unsigned char lang[64];
	t_treenode *node = NULL;
	t_treenode *headernode = NULL;
	t_treenode *worknode = NULL;
	unsigned int length = 0;

	if ( v_get_b ( ws, start + 0x00 ) == 0x00 &&
		v_get_b ( ws, start + 0x01 ) == 0x4D &&
		v_get_b ( ws, start + 0x02 ) == 0x50 &&
		v_get_b ( ws, start + 0x03 ) == 0x50 
		)
	{
		endianess_be = 0;
		ws->flags &= ~FLAGS_ENDIANESS;
		ws->flags |= FLAGS_ENDIANESS_LE;
	}
	else if ( v_get_b ( ws, start + 0x00 ) == 0x50 &&
		v_get_b ( ws, start + 0x01 ) == 0x50 &&
		v_get_b ( ws, start + 0x02 ) == 0x4D &&
		v_get_b ( ws, start + 0x03 ) == 0x00 
		)
	{
		endianess_be = 1;
		ws->flags &= ~FLAGS_ENDIANESS;
		ws->flags |= FLAGS_ENDIANESS_BE;
	}

	if ( v_get_w ( ws, start + 0x00 ) != 0x50504D00 ) 
		PLUGIN_ERROR ( "no PPM header found", NULL );

	node = treenode_create ();

	tag[0] = (v_get_w ( ws, start + 0x00 ) >> 24) & 0xFF;
	tag[1] = (v_get_w ( ws, start + 0x00 ) >> 16) & 0xFF;
	tag[2] = (v_get_w ( ws, start + 0x00 ) >> 8) & 0xFF;
	tag[3] = (v_get_w ( ws, start + 0x00 ) >> 0) & 0xFF;
    tag[4] = '\000';

	if ( ppm_header_ver == 1 )
	{
		v_memcpy_get ( ws, version, start + 0x04, 0x3c );
		v_memcpy_get ( ws, lang, start + 0x40, 4 );
		version[0x3c] = '\000';
		lang[0x04] = '\000';
	}
	else
	{
		v_memcpy_get ( ws, version, start + 0x04, 0x24 );
		v_memcpy_get ( ws, lang, start + 0x28, 4 );
		version[0x24] = '\000';
		lang[0x04] = '\000';
	}

	if ( dump_chunks )
	{
		printf ( "Tag      : '%s'\n", tag );
		printf ( "Version  :\n%s\n\n", version );
		printf ( "Lang     : '%s'\n", lang );
		printf ( "Endianess: '%s'\n", endianess_be?"BE":"LE" );
	}

	treenode_set_name ( node, "PPM" );

	headernode = treenode_addchild ( node );
	treenode_set_name ( headernode, "INFORMATION" );
	worknode = treenode_addchild ( headernode );
	treenode_set_name ( worknode, "VERSION" );
	treenode_set_content_data ( worknode, version );

	worknode = treenode_addchild ( headernode );
	treenode_set_name ( worknode, "LANG" );
	treenode_set_content_data ( worknode, lang );

	worknode = treenode_addchild ( headernode );
	treenode_set_name ( worknode, "ENDIANESS" );
	treenode_set_content_data ( worknode, endianess_be?"BE":"LE" );

	worknode = treenode_addchild ( headernode );
	treenode_set_name ( worknode, "HEADERVER" );
	treenode_set_content_data_hexval ( worknode, ppm_header_ver );

	if ( ppm_header_ver == 1 )
		start += 0x44;
	else
		start += 0x2C;

	// set original alignment byte
	alignment_byte = v_get_b ( ws, start - 1 );

	ppmodify_ppm_lpcs_dump ( ws, start );

	do
	{
		worknode = treenode_addchild ( node );

		if ( ppmodify_ppm_chunk_dump ( ws, start, worknode ) != E_OK )
		{
			treenode_release ( node );
			return NULL;
		}
		length = ppmodify_ppm_chunk_length ( ws, start );
		start += length;

		start = ALIGN_WORD ( start );
	}
	while ( ( length != 0x24 ) && v_valid ( ws, start ) && length );

	//treenode_dump ( node );

	return node;
}

unsigned int ppm_header_version_get ( t_workspace *ws, unsigned int start )
{
   unsigned int val;
   
   if ( !ws )
		return E_FAIL;

   val = v_get_w ( ws, start + 0x34 ); // old ppms should have chunk ID here
   
   if ( val == 0 || val == 0xFFFFFFFF )
      ppm_header_ver = 1;
   else
      ppm_header_ver = 0;

   return E_OK;
}

unsigned int ppmodify_dump ( t_workspace *ws, unsigned int start )
{
	unsigned int ret;

   ppm_header_version_get ( ws, start );

	ret = ppmodify_ppm_header_dump ( ws, start );

	if ( ret != E_OK )
	{
		if ( ppm_header_ver == 1 )
			ppm_header_ver = 0;
		else
			ppm_header_ver = 1;
		ret = ppmodify_ppm_header_dump ( ws, start );
	}

	return ret;
}

t_treenode *ppmodify_dump_xml ( t_workspace *ws, unsigned int start )
{
	t_treenode *node = NULL;

	ppm_header_version_get ( ws, start );

	node = ppmodify_ppm_header_dump_xml ( ws, start );

	if ( !node )
	{
		if ( ppm_header_ver == 1 )
			ppm_header_ver = 0;
		else
			ppm_header_ver = 1;
		node = ppmodify_ppm_header_dump_xml ( ws, start );
	}

	return node;
}


unsigned int ppmodify_build_ppm_subchunk ( t_treenode *node )
{
	t_treenode *infonode = NULL;
	t_treenode *dumpnode = NULL;
	t_treenode *worknode = NULL;
	subchunk_hdr header;
	unsigned char *buffer = NULL;
	unsigned char *chunkdata = NULL;
	t_workspace *ws = NULL;
	unsigned int length = 0;

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
	dumpnode = treenode_get_byname ( treenode_children ( node ), "DUMP" );
	if ( dumpnode )
		chunkdata = treenode_get_content_data_binary ( dumpnode, &length );

	if ( !chunkdata )
	{
		chunkdata = strdup ( "" );
		length = 0;
	}

	buffer = malloc ( length + 0x10 );

	ws = workspace_create_file_from_buffer ( buffer, length + 0x10 );
	if ( !ws )
		PLUGIN_ERROR ( "failed creating temporary workspace", E_FAIL );

	ws->flags &= ~FLAGS_ENDIANESS;
	if ( endianess_be )
		ws->flags |= FLAGS_ENDIANESS_BE;
	else
		ws->flags |= FLAGS_ENDIANESS_LE;

	v_set_w ( ws, 0x00, header.id );

	if ( header.length != length + 0x10 )
		printf ( " [i] SubChunk '%s' length changed from %i to %i\n", header.name, header.length, length + 0x10 );

	v_set_w ( ws, 0x04, length + 0x10 );
	v_memcpy_put ( ws, 0x08, header.name, 4 );
	v_set_b ( ws, 0x0C, header.flags1 );
	v_set_b ( ws, 0x0D, header.flags2 );
	v_set_b ( ws, 0x0E, header.flags3 );
	v_set_b ( ws, 0x0F, header.flags4 );

	v_memcpy_put ( ws, 0x10, chunkdata, length );

	free ( chunkdata );

	// free subnodes
	treenode_release ( infonode );
	treenode_release ( dumpnode );

	// replace with constructed data
	treenode_set_name ( node, "SUBCHUNK-BINARY" );
	treenode_set_content_data_binary ( node, buffer, length + 0x10 );

	workspace_release ( ws );

	return E_OK;
}

unsigned int ppmodify_build_lpcs_subchunk ( t_treenode *node )
{
	t_treenode *infonode = NULL;
	t_treenode *dumpnode = NULL;
	t_treenode *worknode = NULL;
	subchunk_hdr header;
	unsigned char *buffer = NULL;
	unsigned char *chunkdata = NULL;
	t_workspace *ws = NULL;

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
	dumpnode = treenode_get_byname ( treenode_children ( node ), "DUMP" );

	buffer = malloc ( header.length );

	ws = workspace_create_file_from_buffer ( buffer, header.length );
	if ( !ws )
		PLUGIN_ERROR ( "failed creating temporary workspace", E_FAIL );

	ws->flags &= ~FLAGS_ENDIANESS;
	if ( endianess_be )
		ws->flags |= FLAGS_ENDIANESS_BE;
	else
		ws->flags |= FLAGS_ENDIANESS_LE;

	v_set_w ( ws, 0x00, header.id );

	v_set_w ( ws, 0x04, header.length );
	v_memcpy_put ( ws, 0x08, header.name, 4 );
	v_set_b ( ws, 0x0C, header.flags1 );
	v_set_b ( ws, 0x0D, header.flags2 );
	v_set_b ( ws, 0x0E, header.flags3 );
	v_set_b ( ws, 0x0F, header.flags4 );

	if ( header.length == 0x210 )
	{
      unsigned int i = 0;

		while ( i < 0x100 )
		{
			v_set_h ( ws, 0x10 + i * 2, lpcs->utf16[i] );
			i++;
		}
	}

	// free subnodes
	treenode_release ( infonode );
	treenode_release ( dumpnode );

	// replace with constructed data
	treenode_set_name ( node, "SUBCHUNK-BINARY" );
	treenode_set_content_data_binary ( node, buffer, header.length );

	workspace_release ( ws );

	return E_OK;
}

unsigned int ppmodify_build_ppm_chunk ( t_treenode *node )
{
	t_treenode *child  = NULL;
	t_treenode *infonode = NULL;
	t_treenode *worknode = NULL;
	chunk_hdr header;
	unsigned char *buffer = NULL;
	unsigned char *workbuffer = NULL;
	unsigned char *chunkdata = NULL;
	t_workspace *ws = NULL;
	unsigned int length = 0;
	unsigned int worklength = 0;
	unsigned int alignedlength = 0;
	unsigned int checksum = 0;

	if ( !node )
		PLUGIN_ERROR ( "(!node) failed", E_FAIL );

	// retrieve chunk headers
	memset ( header.name, 0x00, 0x04 );
	memset ( header.version, 0x00, 0x08 );

	infonode = treenode_get_byname ( treenode_children ( node ), "INFORMATION" );
	if ( !infonode )
		PLUGIN_ERROR ( "failed retrieving INFORMATION", E_FAIL );

	worknode = treenode_get_byname ( treenode_children ( infonode ), "VERSION" );
	if ( !worknode )
		PLUGIN_ERROR ( "failed retrieving INFORMATION/VERSION content", E_FAIL );
	buffer = treenode_get_content_data ( worknode );
	if ( strlen ( buffer ) <= 8 )
		strcpy ( header.version, buffer );

	worknode = treenode_get_byname ( treenode_children ( infonode ), "NAME" );
	if ( !worknode )
		PLUGIN_ERROR ( "failed retrieving INFORMATION/NAME content", E_FAIL );
	buffer = treenode_get_content_data ( worknode );
	if ( strlen ( buffer ) <= 4 )
		strcpy ( header.name, buffer );

	worknode = treenode_get_byname ( treenode_children ( infonode ), "LENGTH" );
	if ( !worknode )
		PLUGIN_ERROR ( "failed retrieving INFORMATION/LENGTH content", E_FAIL );
	header.length = treenode_get_content_data_hexval ( worknode );

	worknode = treenode_get_byname ( treenode_children ( infonode ), "CHECKSUM" );
	if ( !worknode )
		PLUGIN_ERROR ( "failed retrieving INFORMATION/CHECKSUM content", E_FAIL );
	header.chksum = treenode_get_content_data_hexval ( worknode );


	// first construct all subchunks in that chunk

	child = treenode_get_byname ( treenode_children ( node ), "SUBCHUNK" );
	while ( child )
	{
		if ( !strcmp ( "TEXT", header.name ) && !text_as_unknown )
			ppmodify_build_text_subchunk ( child );
		else if ( !strcmp ( "ANIM", header.name ) && !anim_as_unknown )
			ppmodify_build_anim_subchunk ( child );
		else if ( !strcmp ( "FONT", header.name ) && !font_as_unknown )
			ppmodify_build_font_subchunk ( child );
		else if ( !strcmp ( "VFNT", header.name ) && !vfnt_as_unknown )
			ppmodify_build_vfnt_subchunk ( child );
		else if ( !strcmp ( "TONE", header.name ) && !tone_as_unknown )
			ppmodify_build_tone_subchunk ( child );
		else if ( !strcmp ( "PLMN", header.name ) && !plmn_as_unknown )
			ppmodify_build_plmn_subchunk ( child );
		else if ( !strcmp ( "LPCS", header.name ) )
			ppmodify_build_lpcs_subchunk ( child );
		else
			ppmodify_build_ppm_subchunk ( child );
		child = treenode_get_byname ( treenode_next ( child ), "SUBCHUNK" );
	}

	length = 0x14;
	buffer = malloc ( length );

	// now construct the chunk from that subchunks

	child = treenode_get_byname ( treenode_children ( node ), "SUBCHUNK-BINARY" );
	while ( child )
	{
		workbuffer = treenode_get_content_data_binary ( child, &worklength );
		if ( !workbuffer || worklength < 0 )
			PLUGIN_ERROR ( "failed retrieving SUBCHUNK-BINARY content", E_FAIL );

		alignedlength = ALIGN_WORD ( worklength );
		buffer = realloc ( buffer, length + alignedlength );
		memcpy ( buffer + length, workbuffer, worklength ); 

		free ( workbuffer );

		// set alignment bytes 
		memset ( buffer + length + worklength, alignment_byte, alignedlength - worklength );

		length += alignedlength;

		worknode = treenode_get_byname ( treenode_next ( child ), "SUBCHUNK-BINARY" );
		treenode_release ( child );
		child = worknode;
	}

	treenode_release ( infonode );

	// headers
	ws = workspace_create_file_from_buffer ( buffer, length );
	if ( !ws )
		PLUGIN_ERROR ( "failed creating temporary workspace", E_FAIL );

	ws->flags &= ~FLAGS_ENDIANESS;
	if ( endianess_be )
		ws->flags |= FLAGS_ENDIANESS_BE;
	else
		ws->flags |= FLAGS_ENDIANESS_LE;

	if ( header.length != length )
		printf ( " [i] Chunk '%s' length changed from %i to %i\n", header.name, header.length, length );

	v_set_w ( ws, 0x04, length );
	v_set_w ( ws, 0x08, (header.name[0]<<24) | (header.name[1]<<16) | (header.name[2]<<8) | header.name[3]  );

	v_memcpy_put ( ws, 0x0C, header.version, 0x08 );

	if ( strlen ( header.name ) )
	{
		checksum = ppmodify_calc_checksum ( buffer + 0x04, length - 0x04 );
		if ( header.chksum != checksum )
			printf ( " [i] Checksum changed in '%s' chunk 0x%08X != 0x%08X\n", header.name, checksum, header.chksum );
	}
	else
		checksum = 0;

	v_set_w ( ws, 0x00, checksum );

	// replace with constructed data
	treenode_set_name ( node, "CHUNK-BINARY" );
	treenode_set_content_data_binary ( node, buffer, length );

	workspace_release ( ws );

	return E_OK;
}


unsigned int ppmodify_build_ppm ( t_workspace *ws, unsigned int start, unsigned int max_length, t_treenode *node )
{
	t_treenode *child = NULL;
	t_treenode *headernode = NULL;
	t_treenode *infonode = NULL;
	t_treenode *worknode = NULL;
	t_treenode *lpcsnode = NULL;
	ppm_hdr header;
	unsigned char *buffer = NULL;
	unsigned char *chunkdata = NULL;
	unsigned char *workbuffer = NULL;
	t_workspace *tmp_ws = NULL;
	unsigned int length = 0;
	unsigned int alignedlength = 0;
	unsigned int worklength = 0;
	char *data=NULL;


#ifdef PPMODIFY_CLIENT
	return E_OK;
#else
	if ( !ws || !node )
		PLUGIN_ERROR ( "(!ws || !node) failed", E_FAIL );

	node = treenode_get_byname ( node, "PPM" );

	// retrieve ppm headers
	memset ( header.ppm_string, 0x00, 0x04 );
	memset ( header.type, alignment_byte, 0x04 );
	memset ( header.version, alignment_byte, 0x3c );

	infonode = treenode_get_byname ( treenode_children ( node ), "INFORMATION" );
	if ( !infonode )
		PLUGIN_ERROR ( "failed retrieving INFORMATION content", E_FAIL );

	worknode = treenode_get_byname ( treenode_children ( infonode ), "VERSION" );
	if ( !worknode )
		PLUGIN_ERROR ( "failed retrieving INFORMATION/VERSION content", E_FAIL );
	buffer = treenode_get_content_data ( worknode );
	if ( strlen ( buffer ) <= 0x3c )
		strcpy ( header.version, buffer );

	worknode = treenode_get_byname ( treenode_children ( infonode ), "LANG" );
	if ( !worknode )
		PLUGIN_ERROR ( "failed retrieving INFORMATION/LANG content", E_FAIL );
	buffer = treenode_get_content_data ( worknode );
	if ( strlen ( buffer ) <= 0x04 )
		strcpy ( header.type, buffer );

	worknode = treenode_get_byname ( treenode_children ( infonode ), "ENDIANESS" );
	if ( !worknode )
		PLUGIN_ERROR ( "failed retrieving INFORMATION/ENDIANESS content", E_FAIL );
	buffer = treenode_get_content_data ( worknode );

	if ( strlen ( buffer ) == 0x02 && !strcmp ( buffer, "BE" ) )
		endianess_be = 1;
	else if ( strlen ( buffer ) == 0x02 && !strcmp ( buffer, "LE" ) )
		endianess_be = 0;
	else
	{
		printf ( "failed retrieving INFORMATION/ENDIANESS content, defaulting to BE" );
		endianess_be = 1;
	}


	// then construct all chunks in that PPM

	child = treenode_get_byname ( treenode_children ( node ), "CHUNK" );
	while ( child )
	{
		if ( !lpcsnode )
		{
			infonode = treenode_get_byname ( treenode_children ( child ), "INFORMATION" );
			if ( !infonode )
				PLUGIN_ERROR ( "failed retrieving INFORMATION content", E_FAIL );

			worknode = treenode_get_byname ( infonode, "NAME" );
			if ( !worknode )
				PLUGIN_ERROR ( "failed retrieving INFORMATION/NAME content", E_FAIL );

			if ( !strcmp ( "LPCS", treenode_get_content_data ( worknode ) ) )
				lpcsnode = child;
		}

		if ( ( lpcsnode != child ) && ( ppmodify_build_ppm_chunk ( child ) != E_OK ) )
		{
			printf ( "chunk creation failed\n" );
		}
		child = treenode_get_byname ( treenode_next ( child ), "CHUNK" );
	}

	if ( lpcsnode && ( ppmodify_build_ppm_chunk ( lpcsnode ) != E_OK ) )
		printf ( "chunk creation failed\n" );

	treenode_release ( infonode );


	// now construct the PPM from that chunks

	if ( ppm_header_ver == 1 )
		length = 0x44;
	else
		length = 0x2C;
	buffer = malloc ( length );

	// now construct the chunk from that subchunks

	child = treenode_get_byname ( treenode_children ( node ), "CHUNK-BINARY" );
	while ( child )
	{
		workbuffer = treenode_get_content_data_binary ( child, &worklength );
		if ( !workbuffer || worklength < 0 )
			PLUGIN_ERROR ( "failed retrieving CHUNK-BINARY content", E_FAIL );

		alignedlength = ALIGN_WORD ( worklength );

		buffer = realloc ( buffer, length + alignedlength );
		memcpy ( buffer + length, workbuffer, worklength ); 

		free ( workbuffer );

		// set alignment bytes 
		memset ( buffer + length + worklength, alignment_byte, alignedlength - worklength );

		length += alignedlength;

		worknode = treenode_get_byname ( treenode_next ( child ), "CHUNK-BINARY" );
		treenode_release ( child );
		child = worknode;
	}

	// headers

	tmp_ws = workspace_create_file_from_buffer ( buffer, length );
	if ( !tmp_ws )
		PLUGIN_ERROR ( "failed creating temporary workspace", E_FAIL );

	tmp_ws->flags &= ~FLAGS_ENDIANESS;
	if ( endianess_be )
		tmp_ws->flags |= FLAGS_ENDIANESS_BE;
	else
		tmp_ws->flags |= FLAGS_ENDIANESS_LE;


	v_set_w ( tmp_ws, 0x00, 0x50504D00 );

	if ( ppm_header_ver == 1 )
	{
		v_memcpy_put ( tmp_ws, 0x04, header.version, 0x3c );
		v_memcpy_put ( tmp_ws, 0x40, header.type, 0x04 );
	}
	else
	{
		v_memcpy_put ( tmp_ws, 0x04, header.version, 0x24 );
		v_memcpy_put ( tmp_ws, 0x28, header.type, 0x04 );
	}

	// replace with constructed data
	treenode_set_name ( node, "PPM-BINARY" );
	treenode_set_content_data_binary ( node, buffer, length );

	workspace_release ( tmp_ws );

	workbuffer = treenode_get_content_data_binary ( node, &worklength );
	if ( !workbuffer || worklength < 0 )
		PLUGIN_ERROR ( "failed retrieving PPM-BINARY content", E_FAIL );

	printf ( " [i] Using addresses 0x%X - 0x%08X for PPM.\n", start, start + worklength );

	if ( worklength < max_length )
	{
		printf ( " [i] New PPM (0x%X bytes) is smaller than input workspace (0x%X bytes).\n", worklength, max_length );
		printf ( " [i] 0x%X less bytes needed.\n", max_length - worklength );
	}
	if ( worklength > max_length )
	{
		printf ( " [i] New PPM (0x%X bytes) is bigger than input workspace (0x%X bytes).\n", worklength, max_length );
		printf ( " [i] 0x%X more bytes needed.\n", worklength - max_length );
	}

	v_memcpy_put ( ws, start, workbuffer, worklength );

	free ( workbuffer );

	if ( wipe_rest && (max_length > worklength) )
	{
		workbuffer = malloc ( max_length - worklength );
		if ( !workbuffer )
			PLUGIN_ERROR ( "Error allocating memory for wiping!", E_FAIL );
		memset ( workbuffer, 0xFF, max_length - worklength );
		v_memcpy_put ( ws, start+worklength, workbuffer, max_length - worklength );
		free ( workbuffer );
	}

	return E_OK;
#endif
}

unsigned int ppmodify_merge_ppm ( t_treenode *node )
{
	t_treenode *child = NULL;
	t_treenode *headernode = NULL;
	t_treenode *infonode = NULL;
	t_treenode *worknode = NULL;
	t_treenode *lpcsnode = NULL;
	ppm_hdr header;
	unsigned char *buffer = NULL;
	unsigned char *chunkdata = NULL;
	unsigned char *workbuffer = NULL;
	t_workspace *tmp_ws = NULL;
	int length = 0;
	int alignedlength = 0;
	int worklength = 0;

	if ( !node )
		PLUGIN_ERROR ( "(!node) failed", E_FAIL );

	node = treenode_get_byname ( node, "PPM" );

	// retrieve ppm headers
	memset ( header.ppm_string, 0x00, 0x04 );
	memset ( header.type, alignment_byte, 0x04 );
	// original ppm version string is padded by 0xFF
	memset ( header.version, alignment_byte, 0x3c );

	infonode = treenode_get_byname ( treenode_children ( node ), "INFORMATION" );
	if ( !infonode )
		PLUGIN_ERROR ( "failed retrieving INFORMATION content", E_FAIL );

	worknode = treenode_get_byname ( treenode_children ( infonode ), "VERSION" );
	if ( !worknode )
		PLUGIN_ERROR ( "failed retrieving INFORMATION/VERSION content", E_FAIL );
	buffer = treenode_get_content_data ( worknode );
	if ( strlen ( buffer ) <= 0x3c )
		strcpy ( header.version, buffer );

	worknode = treenode_get_byname ( treenode_children ( infonode ), "LANG" );
	if ( !worknode )
		PLUGIN_ERROR ( "failed retrieving INFORMATION/LANG content", E_FAIL );
	buffer = treenode_get_content_data ( worknode );
	if ( strlen ( buffer ) <= 0x04 )
		strcpy ( header.type, buffer );

	worknode = treenode_get_byname ( treenode_children ( infonode ), "ENDIANESS" );
	buffer = treenode_get_content_data ( worknode );

	if ( buffer && strlen ( buffer ) == 0x02 && !strcmp ( buffer, "BE" ) )
		endianess_be = 1;
	else if ( buffer && strlen ( buffer ) == 0x02 && !strcmp ( buffer, "LE" ) )
		endianess_be = 0;
	else
	{
		printf ( "failed retrieving INFORMATION/ENDIANESS content, defaulting to BE" );
		endianess_be = 1;
	}

	worknode = treenode_get_byname ( treenode_children ( infonode ), "HEADERVER" );
	if ( worknode )
		ppm_header_ver = treenode_get_content_data_hexval ( worknode );

	treenode_release ( infonode );


	// then construct all chunks in that PPM

	child = treenode_get_byname ( treenode_children ( node ), "CHUNK" );
	while ( child )
	{
		if ( !lpcsnode )
		{
			infonode = treenode_get_byname ( treenode_children ( child ), "INFORMATION" );
			if ( !infonode )
				PLUGIN_ERROR ( "failed retrieving INFORMATION content", E_FAIL );

			worknode = treenode_get_byname ( infonode, "NAME" );
			if ( !worknode )
				PLUGIN_ERROR ( "failed retrieving INFORMATION/NAME content", E_FAIL );

			if ( !strcmp ( "LPCS", treenode_get_content_data ( worknode ) ) )
				lpcsnode = child;
		}

		if ( ( lpcsnode != child ) && ( ppmodify_build_ppm_chunk ( child ) != E_OK ) )
		{
			printf ( "chunk creation failed\n" );
		}
		child = treenode_get_byname ( treenode_next ( child ), "CHUNK" );
	}

	if ( lpcsnode && ( ppmodify_build_ppm_chunk ( lpcsnode ) != E_OK ) )
		printf ( "chunk creation failed\n" );


	// now construct the PPM from that chunks
	if ( ppm_header_ver == 1 )
		length = 0x44;
	else
		length = 0x2C;
	buffer = malloc ( length );

	// now construct the chunk from that subchunks

	child = treenode_get_byname ( treenode_children ( node ), "CHUNK-BINARY" );
	while ( child )
	{
		workbuffer = treenode_get_content_data_binary ( child, &worklength );
		if ( !workbuffer || worklength < 0 )
			PLUGIN_ERROR ( "failed retrieving CHUNK-BINARY content", E_FAIL );

		alignedlength = ALIGN_WORD ( worklength );

		buffer = realloc ( buffer, length + alignedlength );
		memcpy ( buffer + length, workbuffer, worklength ); 

		free ( workbuffer );

		// set alignment bytes 
		memset ( buffer + length + worklength, alignment_byte, alignedlength - worklength );

		length += alignedlength;

		worknode = treenode_get_byname ( treenode_next ( child ), "CHUNK-BINARY" );
		treenode_release ( child );
		child = worknode;
	}

	// headers

	tmp_ws = workspace_create_file_from_buffer ( buffer, length );
	if ( !tmp_ws )
		PLUGIN_ERROR ( "failed creating temporary workspace", E_FAIL );

	tmp_ws->flags &= ~FLAGS_ENDIANESS;
	if ( endianess_be )
		tmp_ws->flags |= FLAGS_ENDIANESS_BE;
	else
		tmp_ws->flags |= FLAGS_ENDIANESS_LE;

	v_set_w ( tmp_ws, 0x00, 0x50504D00 );
	if ( ppm_header_ver == 1 )
	{
		v_memcpy_put ( tmp_ws, 0x04, header.version, 0x3c );
		v_memcpy_put ( tmp_ws, 0x40, header.type, 0x04 );
	}
	else
	{
		v_memcpy_put ( tmp_ws, 0x04, header.version, 0x24 );
		v_memcpy_put ( tmp_ws, 0x28, header.type, 0x04 );
	}

	// replace with constructed data
	treenode_set_name ( node, "PPM-BINARY" );
	treenode_set_content_data_binary ( node, buffer, length );

	workspace_release ( tmp_ws );


	return E_OK;
}

#endif

