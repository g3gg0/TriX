#ifndef __TRIX_CHUNK_VFNT_C__
#define __TRIX_CHUNK_VFNT_C__

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
#include "chunk_vfnt.h"

extern int dump_chunks;
extern int endianess_be;
extern struct trix_functions *ft;

#include "trixplug_wrapper.h"

//-------------------------------------------------------------------------------------------------

unsigned int ppmodify_build_vfnt_subchunk ( t_treenode *node )
{
	t_treenode *infonode = NULL;
	t_treenode *dumpnode = NULL;
	t_treenode *worknode = NULL;

	subchunk_hdr header;
	unsigned char *buffer = NULL;
	unsigned char *chunkdata = NULL;
	t_workspace *ws = NULL;
	unsigned int length = 0;
	unsigned int hdr_size = 0;

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


	if( !header.id || !strcmp ( header.name, "OPTI" ) ) // terminator or OPTI subchunk
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
   worknode = treenode_get_byname ( treenode_children ( node ), "VFNT" );
   if ( worknode )
   {
      dumpnode = treenode_get_byname ( treenode_children ( worknode ), "VFNT_HEADER" );
      if ( dumpnode )
         chunkdata = treenode_get_content_data_binary ( dumpnode, &length );

      if ( !chunkdata )
      {
         chunkdata = strdup ( "" );
         length = 0;
      }
      hdr_size = length;

      buffer = malloc ( 0x10 + hdr_size );
      memcpy ( buffer + 0x10, chunkdata, hdr_size );
      free ( chunkdata );
      treenode_release ( dumpnode );

      dumpnode = treenode_get_byname ( treenode_children ( worknode ), "DUMP" );
      if ( dumpnode )
         chunkdata = treenode_get_content_data_binary ( dumpnode, &length );
      if ( !chunkdata )
      {
         chunkdata = strdup ( "" );
         length = 0;
      }

      buffer = realloc ( buffer, length + 0x10 + hdr_size );
      memcpy ( buffer + 0x10 + hdr_size, chunkdata, length );
      free ( chunkdata );
      treenode_release ( dumpnode );

      ws = workspace_create_file_from_buffer ( buffer, length + 0x10 + hdr_size );
      if ( !ws )
         PLUGIN_ERROR ( "failed creating temporary workspace", E_FAIL );

      ws->flags &= ~FLAGS_ENDIANESS;
      if ( endianess_be )
         ws->flags |= FLAGS_ENDIANESS_BE;
      else
         ws->flags |= FLAGS_ENDIANESS_LE;

      v_set_w ( ws, 0x00, header.id );

      if ( header.length != length + 0x10 + hdr_size )
         printf ( " [i] SubChunk '%s' length changed from %i to %i\n", header.name, header.length, length + 0x10 + hdr_size );

      v_set_w ( ws, 0x04, length + 0x10 + hdr_size );
      v_memcpy_put ( ws, 0x08, header.name, 4 );
      v_set_b ( ws, 0x0C, header.flags1 );
      v_set_b ( ws, 0x0D, header.flags2 );
      v_set_b ( ws, 0x0E, header.flags3 );
      v_set_b ( ws, 0x0F, header.flags4 );
   }

	// replace with constructed data
	treenode_set_name ( node, "SUBCHUNK-BINARY" );
	treenode_set_content_data_binary ( node, buffer, length + 0x10 + hdr_size );

	workspace_release ( ws );

	return E_OK;
}

unsigned int ppmodify_vfnt_subchunk_dump_vfnt ( t_treenode *headernode, t_workspace *ws, unsigned int start, unsigned int length )
{
	t_treenode *node = NULL;
	char *vfnt_hdr = NULL;
	char *ttf_name = NULL;
	char *ttf_version = NULL;
	char *ttf_copyrights = NULL;
	char *ttf_data = NULL;
	t_stringbuffer utf16 = { NULL, 0, 0, 0 };
	unsigned int pos = 0;
	unsigned int tab_count = 0;
	unsigned int hdr_size = 0;
	unsigned int len = 0;
	unsigned int str_pos = 0;
	unsigned int nam_pos = 0;
	unsigned int v, v1, v2, v3;

	if ( !length || !headernode || !ws )
		return E_FAIL;

	while ( hdr_size < length )
	{
		// numTables
		v1 = ( v_get_b ( ws, start + hdr_size + 4 ) << 8 ) | v_get_b ( ws, start + hdr_size + 5 );
		// searchRange
		v2 = ( v_get_b ( ws, start + hdr_size + 6 ) << 8 ) | v_get_b ( ws, start + hdr_size + 7 ); 
		// rangeShift
		v3 = ( v_get_b ( ws, start + hdr_size + 10 ) << 8 ) | v_get_b ( ws, start + hdr_size + 11 ); 

		// number of tables should be positive
		if ( v1 && v2 && v3 )
		{
			// find ( maximum power of 2 <= numTables ) * 16
			v = v1 >> 1;
			v |= v >> 1;
			v |= v >> 2;
			v |= v >> 4;
			v |= v >> 8;
			v = ( v + 1 ) << 4;

			if ( ( v == v2 ) /*&& ( ( v1 << 4 ) - v2 == v3 )*/ )
				break;
		}
         
		hdr_size += 8;
	}

	if ( hdr_size >= length )
		return E_FAIL;

	vfnt_hdr = malloc ( hdr_size );
	if ( !vfnt_hdr )
		return E_FAIL;

	v_memcpy_get ( ws, vfnt_hdr, start, hdr_size );
	pos = start + hdr_size;

	ttf_data = malloc ( length - hdr_size );
	if ( !ttf_data )
		return E_FAIL;

	v_memcpy_get ( ws, ttf_data, pos, length - hdr_size );
	tab_count = ( v_get_b ( ws, pos + 4 ) << 8 ) | v_get_b ( ws, pos + 5 );

	pos += 12;
	while ( tab_count && v_valid ( ws, pos ) )
	{
		if ( v_get_b ( ws, pos + 0 ) == 'n' &&
		     v_get_b ( ws, pos + 1 ) == 'a' &&
		     v_get_b ( ws, pos + 2 ) == 'm' &&
		     v_get_b ( ws, pos + 3 ) == 'e'
			)
			break;
		
		pos += 16;
		tab_count--;
	}

	pos = ( v_get_b ( ws, pos + 8 ) << 24 ) | ( v_get_b ( ws, pos + 9 ) << 16 ) | ( v_get_b ( ws, pos + 10 ) << 8 ) | v_get_b ( ws, pos + 11 );
	pos = start + hdr_size + pos;

	tab_count = ( v_get_b ( ws, pos + 2 ) << 8 ) | v_get_b ( ws, pos + 3 );
	str_pos = pos + ( ( v_get_b ( ws, pos + 4 ) << 8 ) | v_get_b ( ws, pos + 5 ) );

	pos += 6; // t_ttf_name_rec
	while ( tab_count && v_valid ( ws, pos ) )
	{
		len = ( v_get_b ( ws, pos + 8 ) << 8 ) | v_get_b ( ws, pos + 9 );
		nam_pos = str_pos + ( ( v_get_b ( ws, pos + 10 ) << 8 ) | v_get_b ( ws, pos + 11 ) );

		switch ( v_get_b ( ws, pos + 7 ) )
		{
			case TTF_NAME_ID_COPYRIGHT_NOTICE:
			{
				if ( !ttf_copyrights )
				{
					if ( v_get_b ( ws, pos + 1 ) == TTF_PLATFORM_ID_MACINTOSH )
					{
               			ttf_copyrights = malloc ( len + 1 );
               			v_memcpy_get ( ws, ttf_copyrights, nam_pos, len );
               			ttf_copyrights[len] = 0;
					}
					else if ( v_get_b ( ws, pos + 1 ) == TTF_PLATFORM_ID_WINDOWS )
					{
						utf16.length = len/2;
						utf16.allocated = len;
						utf16.priv_data = 0;
						CHECK_AND_FREE ( utf16.buffer );

						utf16.buffer = malloc ( utf16.allocated );
						v_memcpy_get ( ws, utf16.buffer, nam_pos, utf16.allocated );
						utf16.buffer[len] = 0;
						utf16_switch_endianess ( &utf16 );
               			ttf_copyrights = utf16_to_utf8 ( &utf16 );
						CHECK_AND_FREE ( utf16.buffer );
					}
				}
			}
			break;

			case TTF_NAME_ID_FULL_FONT_NAME:
			{
				if ( !ttf_name )
				{
					if ( v_get_b ( ws, pos + 1 ) == TTF_PLATFORM_ID_MACINTOSH )
					{
               			ttf_name = malloc ( len + 1 );
               			v_memcpy_get ( ws, ttf_name, nam_pos, len );
               			ttf_name[len] = 0;
					}
					else if ( v_get_b ( ws, pos + 1 ) == TTF_PLATFORM_ID_WINDOWS )
					{
						utf16.length = len/2;
						utf16.allocated = len;
						utf16.priv_data = 0;
						CHECK_AND_FREE ( utf16.buffer );

						utf16.buffer = malloc ( utf16.allocated );
						v_memcpy_get ( ws, utf16.buffer, nam_pos, utf16.allocated );
						utf16.buffer[len] = 0;
						utf16_switch_endianess ( &utf16 );
               			ttf_name = utf16_to_utf8 ( &utf16 );
						CHECK_AND_FREE ( utf16.buffer );
					}
				}
			}
			break;

         case TTF_NAME_ID_VERSION_STRING:
			{
				if ( !ttf_version )
				{
					if ( v_get_b ( ws, pos + 1 ) == TTF_PLATFORM_ID_MACINTOSH )
					{
               			ttf_version = malloc ( len + 1 );
               			v_memcpy_get ( ws, ttf_version, nam_pos, len );
               			ttf_version[len] = 0;
					}
					else if ( v_get_b ( ws, pos + 1 ) == TTF_PLATFORM_ID_WINDOWS )
					{
						utf16.length = len/2;
						utf16.allocated = len;
						utf16.priv_data = 0;
						CHECK_AND_FREE ( utf16.buffer );

						utf16.buffer = malloc ( utf16.allocated );
						v_memcpy_get ( ws, utf16.buffer, nam_pos, utf16.allocated );
						utf16.buffer[len] = 0;
						utf16_switch_endianess ( &utf16 );
               			ttf_version = utf16_to_utf8 ( &utf16 );
						CHECK_AND_FREE ( utf16.buffer );
					}
				}
			}
			break;

		}

		pos += 12;
		tab_count--;
	}

	node = treenode_addchild ( headernode );
	treenode_set_name ( node, "NAME" );
	treenode_set_content_data ( node, ttf_name );

	node = treenode_addchild ( headernode );
	treenode_set_name ( node, "VERSION" );
	treenode_set_content_data ( node, ttf_version );

	node = treenode_addchild ( headernode );
	treenode_set_name ( node, "COPYRIGHTS" );
	treenode_set_content_data ( node, ttf_copyrights );

	node = treenode_addchild ( headernode );
	treenode_set_name ( node, "VFNT_HEADER" );
	treenode_set_content_data_binary ( node, vfnt_hdr, hdr_size );

	node = treenode_addchild ( headernode );
	treenode_set_name ( node, "DUMP" );
	treenode_set_content_data_binary ( node, ttf_data, length - hdr_size );

	free ( ttf_name );
	free ( ttf_version );
	free ( ttf_copyrights );
	free ( ttf_data );
	free ( vfnt_hdr );

	return E_OK;
}

unsigned int ppmodify_vfnt_subchunk_dump ( t_workspace *ws, unsigned int start, unsigned int length, t_treenode *node )
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

   if ( !strcmp ( header.name, "OPTI" ) )
   {
      worknode = treenode_addchild ( node );
      treenode_set_name ( worknode, "DUMP" );
      buff = malloc ( header.length - 0x10 );
      v_memcpy_get ( ws, buff, start + 0x10, header.length - 0x10 );
      treenode_set_content_type ( worknode, TREENODE_TYPE_HEXCODED );
      treenode_set_content_data_binary ( worknode, buff, header.length - 0x10 );
      free ( buff );
   }
   else
   {
      headernode = treenode_addchild ( node );
      treenode_set_name ( headernode, "VFNT" );
      ppmodify_vfnt_subchunk_dump_vfnt ( headernode, ws, start + 0x10, header.length - 0x10 );
   }

	return E_OK;
}

#endif /* __TRIX_CHUNK_VFNT_C__ */
