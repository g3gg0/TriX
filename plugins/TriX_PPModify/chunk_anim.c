
#ifndef __TRIX_CHUNK_ANIM_C__
#define __TRIX_CHUNK_ANIM_C__



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
extern struct trix_functions *ft;

#include "trixplug_wrapper.h"


typedef struct 
{
	int id;
	int unk_1;
	int offset;
	int unk_2;
	int size;
} t_anim_entry;



/*

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

*/


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
	if ( entries >= 0x0500 )
		return E_FAIL;

	anim_entries = malloc ( entries * sizeof ( t_anim_entry ) );
	if ( !anim_entries )
		return E_FAIL;

	entry = 0;
	while ( entry < entries && v_valid ( ws, pos + 0x0C ) && (pos - start) < length )
	{
		anim_entries[entry].id = v_get_h ( ws, pos );
		anim_entries[entry].unk_1 = v_get_h ( ws, pos + 2 );
		anim_entries[entry].offset = v_get_w ( ws, pos + 4 );
		anim_entries[entry].unk_2 = v_get_w ( ws, pos + 8 );

		pos += 0x0C;
		entry++;
	}

	if ( !v_valid ( ws, pos ) )
	{
		free ( anim_entries );
		return E_FAIL;
	}

	entry = 0;
	while ( entry < entries && v_valid ( ws, pos ) )
	{
		if ( (entry + 1) == entries )
			anim_entries[entry].size = length - (pos-start) - anim_entries[entry].offset;
		else
			anim_entries[entry].size = anim_entries[entry+1].offset - anim_entries[entry].offset;

		data_pos = pos + anim_entries[entry].offset;

		worknode = treenode_addchild ( headernode );
		treenode_set_name ( worknode, "ANIM" );

			childnode = treenode_addchild ( worknode );
			treenode_set_name ( childnode, "ID" );
			treenode_set_content_data_hexval ( childnode, anim_entries[entry].id );

			childnode = treenode_addchild ( worknode );
			treenode_set_name ( childnode, "UNK1" );
			treenode_set_content_data_hexval ( childnode, anim_entries[entry].unk_1 );

			childnode = treenode_addchild ( worknode );
			treenode_set_name ( childnode, "UNK2" );
			treenode_set_content_data_hexval ( childnode, anim_entries[entry].unk_2 );


			buffer = malloc ( anim_entries[entry].size );
			if ( !buffer )
				return E_FAIL;
			v_memcpy_get ( ws, buffer, pos + anim_entries[entry].offset, anim_entries[entry].size );

			childnode = treenode_addchild ( worknode );
			treenode_set_name ( childnode, "DUMP" );
			treenode_set_content_data_binary ( childnode, buffer, anim_entries[entry].size );

			free ( buffer );

		entry++;
	}

	return E_OK;
}
unsigned int ppmodify_anim_subchunk_dump ( t_workspace *ws, unsigned int start, unsigned int length, t_treenode *node )
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
		treenode_set_name ( headernode, "ANIMS" );
		ppmodify_anim_subchunk_dump_anims ( headernode, ws, start + 0x10, header.length - 0x10 );

	free ( buffer );

	return E_OK;
}


#endif