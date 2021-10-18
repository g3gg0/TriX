#ifndef __TRIX_CHUNK_ANIM_C__
#define __TRIX_CHUNK_ANIM_C__

#include <string.h>
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
extern int recompress;
extern int bitmask_depth;
extern int endianess_be;
extern int add_anim_uid;
extern int alignment_byte;
extern struct trix_functions *ft;

int anim_header_ver = 0;

#include "trixplug_wrapper.h"

#define ANIM_TYPE_UNK   0
#define ANIM_TYPE_NIF   1
#define ANIM_TYPE_GIF   2
#define ANIM_TYPE_PNG   3
#define ANIM_TYPE_BMP   4
#define ANIM_TYPE_JPG   5
#define ANIM_TYPE_M3G   6
#define ANIM_TYPE_3GP   7

/*
   anim_header_ver 0: 8000000000000000 8000000100001970

   first 4 bytes:
   0000 ID_COUNT[2byte, is one too much, maybe it's last entry] 0000 0000

   every entry:
   unknow   2 bytes  [always 0x8000?]
   id       2 bytes
   offset   4 bytes

   anim_header_ver 1: 800000000000000000000001 8000000100001AE800000001

   first 4 bytes:
   0000 ID_COUNT[2byte, is one too much, maybe it's last entry] 0000 0000

   every entry:
   unknow   2 bytes  [always 0x8000?]
   id       2 bytes
   offset   4 bytes
   unknow   4 bytes
*/
typedef struct
{
	unsigned int unk_1;
	int id;
	unsigned int offset;
	unsigned int unk_2;
	int size;
} t_anim_entry;

/* rev2766-chunk_anim.c
int ppmodify_build_anim_subchunk ( t_treenode *node )
{
	t_treenode *infonode = NULL;
	t_treenode *dumpnode = NULL;
	t_treenode *worknode = NULL;
	subchunk_hdr header;
	unsigned char *buffer = NULL;
	unsigned char *chunkdata = NULL;
	t_workspace *ws = NULL;
	int length = 0;

	if ( !node )
		PLUGIN_ERROR ( "(!node) failed" );

	// retrieve chunk headers
	memset ( header.name, 0x00, 0x04 );

	infonode = treenode_get_byname ( treenode_children ( node ), "INFORMATION" );
	if ( !infonode )
		PLUGIN_ERROR ( "failed retrieving INFORMATION" );

	worknode = treenode_get_byname ( infonode, "ID" );
	if ( !worknode )
		PLUGIN_ERROR ( "failed retrieving INFORMATION/ID content" );
	header.id = treenode_get_content_data_hexval ( worknode );

	worknode = treenode_get_byname ( infonode, "NAME" );
	if ( !worknode )
		PLUGIN_ERROR ( "failed retrieving INFORMATION/NAME content" );
	strcpy ( header.name, treenode_get_content_data ( worknode ) );

	worknode = treenode_get_byname ( infonode, "LENGTH" );
	if ( !worknode )
		PLUGIN_ERROR ( "failed retrieving INFORMATION/LENGTH content" );
	header.length = treenode_get_content_data_hexval ( worknode );

	worknode = treenode_get_byname ( infonode, "FLAGS1" );
	if ( !worknode )
		PLUGIN_ERROR ( "failed retrieving INFORMATION/FLAGS1 content" );
	header.flags1 = treenode_get_content_data_hexval ( worknode );

	worknode = treenode_get_byname ( infonode, "FLAGS2" );
	if ( !worknode )
		PLUGIN_ERROR ( "failed retrieving INFORMATION/FLAGS2 content" );
	header.flags2 = treenode_get_content_data_hexval ( worknode );

	worknode = treenode_get_byname ( infonode, "FLAGS3" );
	if ( !worknode )
		PLUGIN_ERROR ( "failed retrieving INFORMATION/FLAGS3 content" );
	header.flags3 = treenode_get_content_data_hexval ( worknode );

	worknode = treenode_get_byname ( infonode, "FLAGS4" );
	if ( !worknode )
		PLUGIN_ERROR ( "failed retrieving INFORMATION/FLAGS4 content" );
	header.flags4 = treenode_get_content_data_hexval ( worknode );


	// get chunk data

	//here we need to parse ANIMS/*ANIM/DUMP
	//count them, write length
	//id-offset table creation, write it
	//read dumps, write them, alignment 4byte
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
		PLUGIN_ERROR ( "failed creating temporary workspace" );

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

unsigned int ppmodify_anim_subchunk_dump_anims_entry ( t_treenode *headernode, unsigned char *buffer, unsigned int length )
{
	t_treenode *childnode = NULL;
	//int i;
	int size;
	int info_size;
	int name_length;
	char compression_type[16];
	int width;
	int height;

	if ( !length || !buffer || !headernode )
		return E_FAIL;

	childnode = treenode_addchild ( headernode );
	treenode_set_name ( childnode, "HEADER" );
	treenode_set_content_data_binary ( childnode, buffer, 0xC );

	if ( length <= 0xC || buffer[1] != 0x02 )
		return E_OK;

	//0x10
	size = buffer[0x10] << 8 | buffer[0x11];
	childnode = treenode_addchild ( headernode );
	treenode_set_name ( childnode, "SIZE" );
	treenode_set_content_data_integer (childnode, size );

	//0x14
	info_size = buffer[0x14];
	childnode = treenode_addchild ( headernode );
	treenode_set_name ( childnode, "INFO_SIZE" );
	treenode_set_content_data_integer ( childnode, info_size );

	name_length = (int)strlen ( buffer + 0x15 );
	childnode = treenode_addchild ( headernode );
	treenode_set_name ( childnode, "NAME" );
	treenode_set_content_data ( childnode, buffer + 0x15 );

	strncpy ( compression_type, buffer + 0x15 + name_length + 1, 4 );
	compression_type[4] = 0;
	childnode = treenode_addchild ( headernode );
	treenode_set_name ( childnode, "COMPRESSION" );
	treenode_set_content_data ( childnode, compression_type );

	width = buffer[0x15 + name_length + 1 + 4 ] << 8 | buffer[0x15 + name_length + 1 + 5 ];
	childnode = treenode_addchild ( headernode );
	treenode_set_name ( childnode, "WIDTH" );
	treenode_set_content_data_integer ( childnode, width );

	height = buffer[0x15 + name_length + 1 + 6 ] << 8 | buffer[0x15 + name_length + 1 + 7 ];
	childnode = treenode_addchild ( headernode );
	treenode_set_name ( childnode, "HEIGHT" );
	treenode_set_content_data_integer ( childnode, height);

	return E_OK;
}
*/

unsigned int ppmodify_build_anim_subchunk ( t_treenode *node )
{
	t_treenode *infonode = NULL;
	t_treenode *dumpnode = NULL;
	t_treenode *worknode = NULL;
	t_treenode *animnode = NULL;
	t_treenode *child = NULL;
	subchunk_hdr header;
	unsigned char *buffer = NULL;
	unsigned char *chunkdata = NULL;
	unsigned char *tmp_buf = NULL;
	t_workspace *ws = NULL;
	t_workspace *ws_tmp = NULL;
	char *hdr_buffer = NULL;
	int hdr_length = 0;
	int length = 0;
    unsigned int alignedlength = 0;
	int ent = 0;
	int unk2 = 0;
	unsigned int anim_entries = 0;
	unsigned int anim_id = 0;
	unsigned int headerpos = 0;
	unsigned char *workbuffer = NULL;
	unsigned int worklength = 0;

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

	if(!header.id) // terminator
		return ppmodify_build_ppm_subchunk ( node );

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
		PLUGIN_ERROR ( "failed retrieving INFORMATION/FLAGS3 content", E_FAIL );
	header.flags3 = treenode_get_content_data_hexval ( worknode );

	worknode = treenode_get_byname ( infonode, "FLAGS4" );
	if ( !worknode )
		PLUGIN_ERROR ( "failed retrieving INFORMATION/FLAGS4 content", E_FAIL );
	header.flags4 = treenode_get_content_data_hexval ( worknode );

	worknode = treenode_get_byname ( infonode, "HEADERVER" );
	if ( !worknode )
		PLUGIN_ERROR ( "failed retrieving INFORMATION/HEADERVER content", E_FAIL );
	anim_header_ver = treenode_get_content_data_hexval ( worknode );

	animnode = treenode_get_byname ( treenode_children ( node ), "ANIMS" );

	// get chunk data
	if(animnode)
	{
		// count the number of anims
		anim_entries = 0;
		child = treenode_get_byname ( treenode_children ( animnode ), "ANIM" );
		while ( child )
		{
			anim_entries++;
			child = treenode_get_byname ( treenode_next ( child ), "ANIM" );
		}

		if(anim_header_ver)
			hdr_length = anim_entries*12 + 4;
		else
			hdr_length = anim_entries*8 + 4;

		length = 0;
		chunkdata  = malloc ( 1 );
		hdr_buffer = calloc ( hdr_length, sizeof(char) );
		if ( !chunkdata || !hdr_buffer )
			return E_FAIL;

		ws_tmp = workspace_create_file_from_buffer ( hdr_buffer, hdr_length );
		ws_tmp->flags &= ~FLAGS_ENDIANESS;
		if ( endianess_be )
			ws_tmp->flags |= FLAGS_ENDIANESS_BE;
		else
			ws_tmp->flags |= FLAGS_ENDIANESS_LE;

		v_set_w ( ws_tmp, 0x00, anim_entries );
		headerpos = 4;

		child = treenode_get_byname ( treenode_children ( animnode ), "ANIM" );
		while ( child )
		{
			worknode = treenode_get_byname ( treenode_children ( child ), "ID" );
         anim_id = treenode_get_content_data_hexval( worknode );

			worknode = treenode_get_byname ( treenode_children ( child ), "UNK2" );
			if ( !worknode )
				unk2=0;
			else
				unk2 = treenode_get_content_data_hexval ( worknode );

			worknode = treenode_get_byname ( treenode_children ( child ), "DUMP" );
			workbuffer = treenode_get_content_data_binary ( worknode, &worklength );

			alignedlength = ALIGN_WORD ( worklength );
			chunkdata = realloc ( chunkdata, length + alignedlength );
			if ( !chunkdata )
				return E_FAIL;

			memcpy ( chunkdata + length, workbuffer, worklength );
            memset ( chunkdata + length + worklength, alignment_byte, alignedlength - worklength );

			if ( anim_header_ver )
			{
				v_set_w ( ws_tmp, headerpos + 0x00, anim_id | 0x80000000 );
				v_set_w ( ws_tmp, headerpos + 0x04, length );
				v_set_w ( ws_tmp, headerpos + 0x08, unk2 );
			}

			length += alignedlength;

			CHECK_AND_FREE ( workbuffer );

			if(anim_header_ver)
				headerpos += 12;
			else
				headerpos += 8;

			worknode = treenode_get_byname ( treenode_next ( child ), "ANIM" );
			child = worknode;
		}
	}

	tmp_buf = malloc ( length + hdr_length );
	memcpy ( tmp_buf, hdr_buffer, hdr_length );
	memcpy ( tmp_buf + hdr_length, chunkdata, length );
	length += hdr_length;
	free(chunkdata);
	chunkdata = tmp_buf;
	workspace_release ( ws_tmp ); // this will also release the old hdr_buffer (due to workspace_create_file_from_buffer)

	// create binary representation
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

unsigned int ppmodify_anim_subchunk_dump_anims ( t_treenode *headernode, t_workspace *ws, unsigned int start, unsigned int length )
{
	unsigned int entries = 0;
	unsigned int entry = 0;
	unsigned int pos = start;
	unsigned int data_pos = 0;
	t_anim_entry *anim_entries = NULL;
	t_treenode *childnode = NULL;
	t_treenode *worknode = NULL;
	unsigned char *buffer = NULL;

	if ( !length || !headernode || !ws )
		return E_FAIL;

	entries = v_get_w ( ws, pos );
	pos += 4;

	// just to make sure ;)
   // huh X3 has >2200 animations ;)
	if ( entries >= 0x1000 )
		return E_FAIL;

	anim_entries = malloc ( entries * sizeof ( t_anim_entry ) );
	if ( !anim_entries )
		return E_FAIL;

	entry = 0;

	if(anim_header_ver)
	{
		while ( entry < entries && v_valid ( ws, pos + 0x0C ) && (pos - start) < length )
		{
			anim_entries[entry].id     = v_get_w ( ws, pos ) & 0xFFFF;
			anim_entries[entry].unk_1  = v_get_w ( ws, pos ) >> 16;
			anim_entries[entry].offset = v_get_w ( ws, pos + 4 );
			anim_entries[entry].unk_2  = v_get_w ( ws, pos + 8 );

			pos += 0x0C;
			entry++;
		}
	}
	else
	{
		while ( entry < entries && v_valid ( ws, pos + 0x08 ) && (pos - start) < length )
		{
			anim_entries[entry].id     = v_get_w ( ws, pos ) & 0xFFFF;
			anim_entries[entry].unk_1  = v_get_w ( ws, pos ) >> 16;
			anim_entries[entry].offset = v_get_w ( ws, pos + 4 );

			pos += 0x08;
			entry++;
		}
	}

	if ( !v_valid ( ws, pos ) )
	{
		free ( anim_entries );
		return E_FAIL;
	}

	entry = 0;
	while ( entry < entries )
	{
		unsigned int next_offset = MEM_MAX;
		unsigned int scan_entry = 0;

		while ( scan_entry < entries )
		{
			// yeah, many ifs - but thats easier to comment :)
			// when not the same entry as we currently check
			if ( scan_entry != entry )
			{
				// check if its offset is behind that of our current entry
				if ( anim_entries[scan_entry].offset > anim_entries[entry].offset )
				{
					// if so and its smaller then the current smallest, update it
					if ( next_offset > anim_entries[scan_entry].offset )
						next_offset = anim_entries[scan_entry].offset;
				}
			}
			scan_entry++;
		}

		if ( next_offset == MEM_MAX )
			anim_entries[entry].size = length - (pos-start) - anim_entries[entry].offset;
		else
			anim_entries[entry].size = next_offset - anim_entries[entry].offset;

		data_pos = pos + anim_entries[entry].offset;

		buffer = malloc ( anim_entries[entry].size );
		if ( !buffer )
			return E_FAIL;
		v_memcpy_get ( ws, buffer, pos + anim_entries[entry].offset, anim_entries[entry].size );

		worknode = treenode_addchild ( headernode );
		treenode_set_name ( worknode, "ANIM" );

		childnode = treenode_addchild ( worknode );
		treenode_set_name ( childnode, "ID" );
		treenode_set_content_data_hexval ( childnode, anim_entries[entry].id );

		childnode = treenode_addchild ( worknode );
		treenode_set_name ( childnode, "TYPE" );
		if( !memcmp(buffer, "GIF89a", 6) )
			treenode_set_content_data ( childnode, "GIF" );
		else if( !memcmp(buffer, "\x89\x50\x4E\x47\x0D\x0A\x1A\x0A", 8) )
			treenode_set_content_data ( childnode, "PNG" );
		else if( !memcmp(buffer, "\xAB\x4A\x53\x52\x31\x38\x34\xBB", 8) )
			treenode_set_content_data ( childnode, "M3G" );
		else if( !memcmp(buffer + 4, "\x66\x74\x79\x70\x33\x67\x70", 7) )
			treenode_set_content_data ( childnode, "3GP" );
		else if( !memcmp(buffer + 6, "\x4A\x46\x49\x46\x00", 5) )
			treenode_set_content_data ( childnode, "JPG" );
		else if( !memcmp(buffer, "\x42\x4D", 2) )
			treenode_set_content_data ( childnode, "BMP" );
		else
		{
			treenode_set_content_data ( childnode, "NIF" );

         if ( add_anim_uid )
         {
            childnode = treenode_addchild ( worknode );
            treenode_set_name ( childnode, "UID" );
            treenode_set_content_data_hexval ( childnode, ppmodify_calc_checksum ( buffer, anim_entries[entry].size ) );
         }
		}

		childnode = treenode_addchild ( worknode );
		treenode_set_name ( childnode, "UNK1" );
		treenode_set_content_data_hexval ( childnode, anim_entries[entry].unk_1 );

		if(anim_header_ver)
		{
			childnode = treenode_addchild ( worknode );
			treenode_set_name ( childnode, "UNK2" );
			treenode_set_content_data_hexval ( childnode, anim_entries[entry].unk_2 );
		}

		childnode = treenode_addchild ( worknode );
		treenode_set_name ( childnode, "DUMP" );
		treenode_set_content_data_binary ( childnode, buffer, anim_entries[entry].size );

		free ( buffer );

		entry++;
	}

	free ( anim_entries );

	return E_OK;
}

unsigned int ppmodify_anim_subchunk_dump ( t_workspace *ws, unsigned int start, unsigned int length, t_treenode *node )
{
	subchunk_hdr header;
	t_treenode *headernode = NULL;
	t_treenode *worknode = NULL;

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

	if ( v_get_w ( ws, start + 0x20 ) & 0x80000000 )
		anim_header_ver = 1;
	else
		anim_header_ver = 0; // default

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

	worknode = treenode_addchild ( headernode );
	treenode_set_name ( worknode, "HEADERVER" );
	treenode_set_content_data_hexval ( worknode, anim_header_ver );

	headernode = treenode_addchild ( node );
	treenode_set_name ( headernode, "ANIMS" );
	ppmodify_anim_subchunk_dump_anims ( headernode, ws, start + 0x10, header.length - 0x10 );

	return E_OK;
}

#endif
