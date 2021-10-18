#ifndef __TRIX_CHUNK_TEXT_C__
#define __TRIX_CHUNK_TEXT_C__



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

unsigned int ppmodify_string_unescape ( unsigned char *string )
{
	unsigned int err = 0;
	unsigned int inpos = 0;
	unsigned int outpos = 0;
	unsigned short hexval;
	unsigned char tmp[10];
	unsigned char *buffer = NULL;

	if ( !string )
		PLUGIN_ERROR ( "(!string) failed", E_FAIL );

	// we will for sure have a less or equal number of bytes
	buffer = malloc ( strlen ( string ) );

	while ( string[inpos] )
	{
		if ( string[inpos] == '\\' )
		{
			memcpy ( tmp, &string[inpos+1], 4 );
			tmp[4] = 0x00;

			hexval = util_hex_get ( tmp, &err );
			if ( err != 1  )
			{
//				PLUGIN_ERROR ( "util_hex_get_buffer () failed", E_FAIL );

				if ( hexval < 0x80 )
				{
					buffer[outpos++] = hexval & 0xFF;
				}
				else if ( hexval < 0x800 )
				{
					buffer[outpos++] = 0xc0 | (hexval >> 6);
					buffer[outpos++] = 0x80 | (hexval & 0x3f);
				}
				else if ( hexval < 0x10000 )
				{
					buffer[outpos++] = 0xe0 | (hexval >> 12);
					buffer[outpos++] = 0x80 | ((hexval >> 6) & 0x3f);
					buffer[outpos++] = 0x80 | (hexval & 0x3f);
				}
				inpos += 5;
			}
			else
				buffer[outpos++] = string[inpos++];
		}
		else
			buffer[outpos++] = string[inpos++];
	}

	memcpy ( string, buffer, outpos );
	string[outpos] = '\000';

	return E_OK;
}

unsigned int utf8_to_utf16 ( unsigned char *coded, t_stringbuffer *string )
{
	unsigned int inpos = 0;
	unsigned int outpos = 0;
	unsigned short value = 0;

	if ( !string || !coded )
		return E_FAIL;

	string->allocated = strlen ( coded ) * 2 + 2;
	string->buffer = malloc ( string->allocated );
	if ( !string->buffer )
		return NULL;


	while ( inpos < strlen ( coded ) )
	{
		if ( (coded[inpos] & 0xE0) == 0xC0  )
		{
			value = (coded[inpos] & 0x1F) << 6;

			inpos++;
			if ( (coded[inpos] & 0xC0) != 0x80 )
				return E_FAIL;
			value |= coded[inpos] & 0x3F;
		}
		else if ( (coded[inpos] & 0xF0) == 0xE0  )
		{
			value = (coded[inpos] & 0x0F) << 12;

			inpos++;
			if ( (coded[inpos] & 0xC0) != 0x80 )
				return E_FAIL;
			value |= (coded[inpos] & 0x3F) << 6;

			inpos++;
			if ( (coded[inpos] & 0xC0) != 0x80 )
				return E_FAIL;
			value |= coded[inpos] & 0x3F;
		}
		else if ( (coded[inpos] & 0xF8) == 0xF0  )
		{
			value = (coded[inpos] & 0x07) << 18;

			inpos++;
			if ( (coded[inpos] & 0xC0) != 0x80 )
				return E_FAIL;
			value |= (coded[inpos] & 0x3F) << 12;

			inpos++;
			if ( (coded[inpos] & 0xC0) != 0x80 )
				return E_FAIL;
			value |= (coded[inpos] & 0x3F) << 6;

			inpos++;
			if ( (coded[inpos] & 0xC0) != 0x80 )
				return E_FAIL;
			value |= coded[inpos] & 0x3F;
		}
		else
			value = coded[inpos] & 0x7F;

		string->buffer[outpos++] = value;

		inpos++;
	}
	string->buffer[outpos] = '\000';

	string->length = outpos;
	return E_OK;
}


unsigned char *utf16_to_utf8 ( t_stringbuffer *string )
{
	unsigned int inpos = 0;
	unsigned int outpos = 0;
	unsigned char *buffer = NULL;
	unsigned char *coded = NULL;
	unsigned char tmpbuff[4];

	if ( !string || !string->buffer )
		return NULL;

	HEAP_CHECK;
	buffer = malloc ( string->length * 20 + 1 );
	if ( !buffer )
		return NULL;

	while ( inpos < string->length )
	{
		if ( string->buffer[inpos] < 0x0020 || string->buffer[inpos] > 0xFFF8  )
		{
			tmpbuff[0] = (string->buffer[inpos] >> 8);
			tmpbuff[1] = string->buffer[inpos] & 0xFF;
			coded = util_hexpack ( tmpbuff, 2 );
			if ( !coded )
				return NULL;

			buffer[outpos++] = '\\';
			buffer[outpos++] = coded[0];
			buffer[outpos++] = coded[1];
			buffer[outpos++] = coded[2];
			buffer[outpos++] = coded[3];
			free ( coded );
		}
		else if ( string->buffer[inpos] < 0x80 )
		{
			buffer[outpos++] = (unsigned char)(string->buffer[inpos] & 0xFF);
		}
		else if ( string->buffer[inpos] < 0x800 )
		{
			buffer[outpos++] = 0xc0 | (string->buffer[inpos] >> 6);
			buffer[outpos++] = 0x80 | (string->buffer[inpos] & 0x3f);
		}
		else
		{
			buffer[outpos++] = 0xe0 | (string->buffer[inpos] >> 12);
			buffer[outpos++] = 0x80 | ((string->buffer[inpos] >> 6) & 0x3f);
			buffer[outpos++] = 0x80 | (string->buffer[inpos] & 0x3f);
		}

		inpos++;
	}
	buffer[outpos++] = '\000';

	buffer = realloc ( buffer, strlen ( buffer ) + 1 );

	return buffer;
}

unsigned int utf16_switch_endianess ( t_stringbuffer *string )
{
	unsigned int pos = 0;
	unsigned int value = 0;
	unsigned char *temp = NULL;

	if ( !string || !string->buffer )
		return E_FAIL;

	temp = (unsigned char*)string->buffer;
	while ( pos < string->length )
	{
		value = string->buffer[pos];
		temp[2*pos] = value >> 8;
		temp[2*pos + 1] = value & 0xFF;

		pos++;
	}

	return E_OK;
}

unsigned int utf16_strlen ( t_stringbuffer *string )
{
	unsigned int pos = 0;

	if ( !string || !string->buffer )
		return E_FAIL;

	return string->length;
}


unsigned int utf16_stradd ( t_stringbuffer *string, unsigned short c )
{
	if ( !string || !string->buffer )
		return E_FAIL;

	if ( string->length >= string->allocated )
		return E_FAIL;

	string->buffer[string->length++] = c;
	string->buffer[string->length] = 0;

	HEAP_CHECK;

	return E_OK;
}


unsigned int ppmodify_text_tokens_to_utf16 ( t_compress_buffer *buffer, t_stringbuffer *string )
{
	unsigned int pos = 0;
	unsigned short data = 0;

	if ( !buffer || !string || !string->buffer )
		return E_FAIL;

	if ( buffer->entries % 2 &&  buffer->data[buffer->entries-1].value != 0x00 )
	{
		utf16_stradd ( string, '(' );
		utf16_stradd ( string, 'I' );
		utf16_stradd ( string, 'n' );
		utf16_stradd ( string, 'v' );
		utf16_stradd ( string, 'a' );
		utf16_stradd ( string, 'l' );
		utf16_stradd ( string, 'i' );
		utf16_stradd ( string, 'd' );
		utf16_stradd ( string, ')' );
		
//		return E_FAIL;
	}

	while ( pos < buffer->entries )
	{
		if ( buffer->data[pos].type != TOKEN_TYPE_PLAIN )
			return E_FAIL;

		if ( endianess_be )
			data = buffer->data[pos].value << 8;
		else
			data = buffer->data[pos].value;

		pos++;

		if ( buffer->data[pos].type != TOKEN_TYPE_PLAIN )
			return E_FAIL;

		if ( endianess_be )
			data |= buffer->data[pos].value;
		else
			data |= buffer->data[pos].value << 8;

		pos++;
		if ( string->length >= string->allocated )
		{
			string->allocated += 10;
			string->buffer = realloc ( string->buffer, string->allocated * sizeof ( unsigned short ) );
			if ( !string->buffer )
				return E_FAIL;
		}
		utf16_stradd ( string, data );
	}
	
	return E_OK;
}

unsigned int ppmodify_text_subchunk_dump_lang_comp_token ( unsigned char *inbuffer, unsigned int length, unsigned char *tokens, t_stringbuffer *buffer, unsigned short flags1, int recursive )
{
	unsigned int ret = 0;
	unsigned int pos = 0;
	t_compress_buffer compressbuf;

	ppmodify_text_compress_init ( &compressbuf );

	while ( pos < length )
		ppmodify_text_compress_add ( &compressbuf, TOKEN_TYPE_TOKEN, inbuffer[pos++] )	;

	ret = ppmodify_text_comp_decompress ( &compressbuf, tokens, flags1 );

	ppmodify_text_tokens_to_utf16 ( &compressbuf, buffer );

	ppmodify_text_compress_release ( &compressbuf );

	return ret;
}

unsigned int ppmodify_text_subchunk_dump_lang_plain_entry ( t_workspace *ws, unsigned int start, unsigned int length, t_treenode *node, unsigned short flags1, unsigned short flags2 )
{
	t_treenode *worknode = NULL;
	t_treenode *childnode = NULL;
	t_stringbuffer utf16;
	unsigned char *utf8 = NULL;
	unsigned int pos = 0;
	unsigned int outpos = 0;

	if ( !ws || !node || length < 0 )
		return E_FAIL;

	utf16.length = 0;
	utf16.allocated = 8192;
	utf16.buffer = (unsigned short *) malloc ( utf16.allocated );
	if ( !utf16.buffer )
		return E_FAIL;

	utf16.buffer[0] = 0;

	if ( !(flags1 & 0x80) )
	{
		utf8 = malloc ( 2 );
		if ( !utf8 )
			return E_FAIL;
	}

	pos = 0;
	outpos = 0;
	while ( pos < length )
	{
		if ( flags1 & 0x80 )
			utf16_stradd ( &utf16, v_get_h ( ws, start + 2*pos ) );
		else
		{
			if ( v_get_b ( ws, start + pos ) < 0x80 )
			{
				utf8[outpos++] = v_get_b ( ws, start + pos );
				utf8[outpos] = '\000';
			}
			else
			{
				utf8[outpos++] = v_get_b ( ws, start + pos );
				utf8[outpos] = '\000';
//				utf8[outpos++] = 0xc0 | (v_get_b ( ws, start + pos ) >> 6);
//				utf8[outpos++] = 0x80 | (v_get_b ( ws, start + pos ) & 0x3f);
//				utf8[outpos] = '\000';
			}
			utf8 = realloc ( utf8, outpos+3 );
		}
		pos++;
	}

	if ( flags1 & 0x80 )
	{
		utf8 = utf16_to_utf8 ( &utf16 );
		if ( !utf8 )
			return E_FAIL;

		// switch endianess for clean dump
		utf16_switch_endianess ( &utf16 );
	}


	worknode = treenode_addchild ( node );
	treenode_set_name ( worknode, "STRING" );
/////
	if ( flags1 & 0x80 )
	{
		childnode = treenode_addchild ( worknode );
		treenode_set_name ( childnode, "UTF16" );
		treenode_set_content_data_binary ( childnode, (unsigned char*)utf16.buffer, utf16_strlen ( &utf16 )*2 );
	}
/////
	childnode = treenode_addchild ( worknode );
	treenode_set_name ( childnode, "UTF8" );
	treenode_set_content_data ( childnode, utf8 );


	CHECK_AND_FREE ( utf16.buffer );
	CHECK_AND_FREE ( utf8 );

	return E_OK;
}

unsigned int ppmodify_text_subchunk_dump_lang_comp_entry ( t_workspace *ws, unsigned int start, unsigned int length, t_treenode *node, unsigned short flags1, unsigned short flags2, unsigned char *tokens )
{
	t_treenode *worknode = NULL;
	t_treenode *childnode = NULL;
	t_stringbuffer utf16;
	unsigned char *utf8 = NULL;
	unsigned char *tokenbuffer = NULL;

	if ( !ws || !node || length < 0 )
		return E_FAIL;

	utf16.length = 0;
	utf16.allocated = 8192;
	utf16.buffer = (unsigned short *) malloc ( utf16.allocated );
	if ( !utf16.buffer )
		return E_FAIL;

	utf16.buffer[0] = 0;

	tokenbuffer = malloc ( length  + 1 );
	if ( !tokenbuffer )
	{
		free ( utf16.buffer );
		return E_FAIL;
	}

	v_memcpy_get ( ws, tokenbuffer, start, length );

	ppmodify_text_subchunk_dump_lang_comp_token ( tokenbuffer, length, tokens, &utf16, flags1, 0 );

	utf8 = utf16_to_utf8 ( &utf16 );
	if ( !utf8 )
		return E_FAIL;

	// switch endianess for clean dump
	utf16_switch_endianess ( &utf16 );

	worknode = treenode_addchild ( node );
	treenode_set_name ( worknode, "STRING" );

///
		childnode = treenode_addchild ( worknode );
		treenode_set_name ( childnode, "TOKEN" );
		treenode_set_content_data_binary ( childnode, tokenbuffer, length );
	
		childnode = treenode_addchild ( worknode );
		treenode_set_name ( childnode, "UTF16" );
		treenode_set_content_data_binary ( childnode, (unsigned char*)utf16.buffer, utf16_strlen ( &utf16 )*2 );
	
		childnode = treenode_addchild ( worknode );
		treenode_set_name ( childnode, "UTF8" );
		treenode_set_content_data ( childnode, utf8 );

////

	free ( tokenbuffer );
	free ( utf16.buffer );
	free ( utf8 );

	return E_OK;
}

unsigned int ppmodify_text_subchunk_dump_lang_comp_strings ( t_workspace *ws, unsigned int start, unsigned int length, t_treenode *node, unsigned short flags1, unsigned short flags2, unsigned char *tokens, unsigned char *lengths, int entries )
{
	unsigned int entry = 0;
	unsigned int pos = 0;

	if ( !ws || !node || !lengths || !tokens )
		return E_FAIL;

	while ( entry < entries )
	{
		ppmodify_text_subchunk_dump_lang_comp_entry ( ws, start + pos, lengths[entry] * ((flags1 & 0xF0) == 0xA0 ? 2 : 1 ), node, flags1, flags2, tokens );
		pos += lengths[entry] * ((flags1 & 0xF0) == 0xA0 ? 2 : 1 );
		entry++;
	}

	return E_OK;
}

unsigned int ppmodify_text_subchunk_dump_lang_plain_strings ( t_workspace *ws, unsigned int start, unsigned int length, t_treenode *node, unsigned short flags1, unsigned short flags2, unsigned char *lengths, int entries )
{
	int entry = 0;
	int pos = 0;

	if ( !ws || !node || !lengths )
		return E_FAIL;

	while ( entry < entries )
	{
		ppmodify_text_subchunk_dump_lang_plain_entry ( ws, start + pos, lengths[entry], node, flags1, flags2 );
		if ( flags1 & 0x80 )
			pos += 2*lengths[entry];
		else
			pos += lengths[entry];
		entry++;
	}

	return E_OK;
}

unsigned int ppmodify_text_subchunk_dump_lang_comp ( t_workspace *ws, unsigned int start, unsigned int length, t_treenode *node, unsigned short flags1, unsigned short flags2 )
{
	unsigned char tokens[0x200];
	unsigned char *lengths = NULL;
	unsigned int entries = 0;
	unsigned int total_length = 0;
	t_treenode *childnode = NULL;
	t_treenode *worknode = NULL;

	if ( !ws || !node )
		return E_FAIL;

	if ( length < 0x201 )
		return E_FAIL;

	v_memcpy_get ( ws, tokens, start, 0x200 );
	start += 0x200;
	length -= 0x200;

	while ( start % 2 )
		start++;

	while ( 
		   v_get_b ( ws, start + entries ) != 0xFF 
		&& total_length + entries < length
		//&& entries < length 
		)
	{
		if (  (flags1 & 0xF0) == 0xA0 )
			total_length += 2*v_get_b ( ws, start + entries );
		else
			total_length += v_get_b ( ws, start + entries );
		entries++;
	}

	if ( entries >= length )
		return E_FAIL;

	lengths = (unsigned char *) malloc ( entries );
	if ( !lengths )
		return E_FAIL;

	v_memcpy_get ( ws, lengths, start, entries );
	start += entries;
	length -= entries;

	while ( start % 2 )
		start++;


	worknode = treenode_addchild ( node );
	treenode_set_name ( worknode, "TABLES" );

		childnode = treenode_addchild ( worknode );
		treenode_set_name ( childnode, "TOKENS" );
		treenode_set_content_data_binary ( childnode, tokens, 0x200 );

		childnode = treenode_addchild ( worknode );
		treenode_set_name ( childnode, "LENGTHS" );
		treenode_set_content_data_binary ( childnode, lengths, entries );

	ppmodify_text_subchunk_dump_lang_comp_strings ( ws, start, length, node, flags1, flags2, tokens, lengths, entries );

	free ( lengths );

	return E_OK;
}


unsigned int ppmodify_text_subchunk_dump_lang_plain ( t_workspace *ws, unsigned int start, unsigned int length, t_treenode *node, unsigned short flags1, unsigned short flags2 )
{
	unsigned char *lengths = NULL;
	unsigned int entries = 0;
	unsigned int total_length = 0;
	t_treenode *childnode = NULL;
	t_treenode *worknode = NULL;

	if ( !ws || !node )
		return E_FAIL;

	if ( length < 0x20 )
		return E_FAIL;

	while ( 
		   total_length + entries < length
		&& entries < length 
		)
	{
		if ( flags1 & 0x80 )
			total_length += 2*v_get_b ( ws, start + entries );
		else
			total_length += v_get_b ( ws, start + entries );

		entries++;
	}

	if ( entries >= length )
		return E_FAIL;

	lengths = (unsigned char *) malloc ( entries );
	if ( !lengths )
		return E_FAIL;

	v_memcpy_get ( ws, lengths, start, entries );
	start += entries;
	length -= entries;

	while ( start % 2 )
		start++;

	if ( lengths[entries-1] == 0xFF )
		entries--;

	worknode = treenode_addchild ( node );
	treenode_set_name ( worknode, "TABLES" );

		childnode = treenode_addchild ( worknode );
		treenode_set_name ( childnode, "LENGTHS" );
		treenode_set_content_data_binary ( childnode, lengths, entries );

	ppmodify_text_subchunk_dump_lang_plain_strings ( ws, start, length, node, flags1, flags2, lengths, entries );


	return E_OK;
}

unsigned int ppmodify_text_subchunk_dump_lang ( t_workspace *ws, unsigned int start, unsigned int length, t_treenode *node, unsigned short flags1, unsigned short flags2 )
{
	treenode_set_name ( node, "STRINGS" );

	if ( (flags1 & 0xF0) == 0x90 )
		return ppmodify_text_subchunk_dump_lang_comp ( ws, start, length, node, flags1, flags2 );
	else if ( (flags1 & 0xF0) == 0xA0 )
		return ppmodify_text_subchunk_dump_lang_comp ( ws, start, length, node, flags1, flags2 );
	else 
		return ppmodify_text_subchunk_dump_lang_plain ( ws, start, length, node, flags1, flags2 );

	return E_FAIL;
}

unsigned int ppmodify_text_subchunk_dump ( t_workspace *ws, unsigned int start, unsigned int length, t_treenode *node )
{
	subchunk_hdr header;
	t_treenode *headernode = NULL;
	t_treenode *worknode = NULL;

	if ( !ws || !node )
		return E_FAIL;

	header.id = v_get_w ( ws, start + 0x00 );
	header.length = v_get_w ( ws, start + 0x04 );
	v_memcpy_get ( ws, header.name, start + 0x08, 4 );
	header.flags1 = v_get_b ( ws, start + 0x0C );
	header.flags2 = v_get_b ( ws, start + 0x0D );
	header.flags3 = v_get_b ( ws, start + 0x0E );
	header.flags4 = v_get_b ( ws, start + 0x0F );

	header.name[4] = '\000';

	if ( dump_chunks )
	{
		printf ( "    SubChunk: '%s'\n", header.name );
		printf ( "    Length  : 0x%08X\n", header.length );
		printf ( "    Flags1  : 0x%04X\n", header.flags1 );
		printf ( "    Flags2  : 0x%04X\n", header.flags2 );
		printf ( "    Flags3  : 0x%04X\n", header.flags3 );
		printf ( "    Flags4  : 0x%04X\n", header.flags4 );
		printf ( "    ID      : 0x%08X\n", header.id );
		printf ( "\n" );
	}
	treenode_set_name ( node, "SUBCHUNK" );

		headernode = treenode_addchild ( node );
		treenode_set_name ( headernode, "INFORMATION" );

			worknode = treenode_addchild ( headernode );
			treenode_set_name ( worknode, "ID" );
			treenode_set_content_type ( worknode, TREENODE_TYPE_HEXVAL );
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

	start += 0x10;

	worknode = treenode_addchild ( node );
	ppmodify_text_subchunk_dump_lang ( ws, start, header.length - 0x10, worknode, header.flags1, header.flags2 );


	return E_OK;
}


//
//
//   PPM build area
//
//

unsigned int ppmodify_build_text_utf16 ( t_treenode *node )
{
	t_stringbuffer utf16_string;
	t_treenode *worknode = NULL;
	unsigned char *workbuffer = NULL;

	// just do when UTF16 string is missing

	worknode = treenode_get_byname ( treenode_children ( node ), "UTF16" );
	workbuffer = treenode_get_content_data ( worknode );
	if ( !workbuffer )
	{
		utf16_string.length = 0;
		utf16_string.buffer = NULL;

		worknode = treenode_get_byname ( treenode_children ( node ), "UTF8" );
		if ( !worknode )
			return E_FAIL;

		workbuffer = treenode_get_content_data ( worknode );
		if ( !workbuffer )
			return E_FAIL;

		ppmodify_string_unescape ( workbuffer );
		if ( utf8_to_utf16 ( workbuffer, &utf16_string ) != E_OK )
			return E_FAIL;

		worknode = treenode_get_byname ( treenode_children ( node ), "UTF16" );
		if ( !worknode )
		{
			worknode = treenode_addchild ( node );
			treenode_set_name ( worknode, "UTF16" );
		}

		// switch endianess for a clean dump
		utf16_switch_endianess ( &utf16_string );

		treenode_set_content_data_binary ( worknode, (unsigned char*)utf16_string.buffer, utf16_string.length * 2 );

		CHECK_AND_FREE ( utf16_string.buffer );
	}
	return E_OK;
}


unsigned int ppmodify_text_tokenbuf_cmp ( t_token_entry *source, t_compress_buffer *token, unsigned int length )
{
	int pos = 0;
	int done = 0;

	if ( !token || !source )
		return 1;

	if ( token->entries < length )
		return 1;

	while ( !done )
	{
		done = 1;
		if ( pos == length )
			return 0;

		if ( source[pos].type == TOKEN_TYPE_PLAIN )
		{
			if ( source[pos].value == token->data[pos].value )
			{
				pos++;
				done = 0;
			}
		}
	}
	return 1;
}

unsigned int ppmodify_build_text_subchunk ( t_treenode *node )
{
	t_treenode *stringnode = NULL;
	t_treenode *tablenode = NULL;
	t_treenode *infonode = NULL;
	t_treenode *dumpnode = NULL;
	t_treenode *worknode = NULL;
	t_treenode *child = NULL;
	subchunk_hdr header;
	unsigned char *buffer = NULL;
	unsigned char *workbuffer = NULL;
	unsigned char *chunkdata = NULL;
	t_workspace *ws = NULL;
	unsigned int alignment = 0;
	unsigned int length = 0;
	unsigned int worklength = 0;
	unsigned int string_entries = 0;
	unsigned int stringpos = 0;
	unsigned int lengthtable_pos = 0;
	unsigned int tokentable_pos = 0;
	unsigned int tablelength = 0;

	t_compress_buffer *comp_token_table = NULL;
	unsigned int compressed = 0;
	unsigned int unicode = 0;

	if ( !node )
		return E_FAIL;

	// retrieve chunk headers
	memset ( header.name, 0x00, 0x04 );

	infonode = treenode_get_byname ( treenode_children ( node ), "INFORMATION" );
	if ( !infonode )
		return E_FAIL;

	worknode = treenode_get_byname ( treenode_children ( infonode ), "ID" );
	if ( !worknode )
 		return E_FAIL;
	header.id = treenode_get_content_data_hexval ( worknode );

	worknode = treenode_get_byname ( treenode_children ( infonode ), "NAME" );
	if ( !worknode )
		return E_FAIL;
	strcpy ( header.name, treenode_get_content_data ( worknode ) );

	worknode = treenode_get_byname ( treenode_children ( infonode ), "LENGTH" );
	if ( !worknode )
		return E_FAIL;
	header.length = treenode_get_content_data_hexval ( worknode );

	worknode = treenode_get_byname ( treenode_children ( infonode ), "FLAGS1" );
	if ( !worknode )
		PLUGIN_ERROR ( "failed retrieving INFORMATION/FLAGS1 content", E_FAIL );
	header.flags1 = treenode_get_content_data_hexval ( worknode );

	worknode = treenode_get_byname ( treenode_children ( infonode ), "FLAGS2" );
	if ( !worknode )
		PLUGIN_ERROR ( "failed retrieving INFORMATION/FLAGS2 content", E_FAIL );
	header.flags2 = treenode_get_content_data_hexval ( worknode );

	worknode = treenode_get_byname ( treenode_children ( infonode ), "FLAGS3" );
	if ( !worknode )
		header.flags3 = 0;
	else
		header.flags3 = treenode_get_content_data_hexval ( worknode );

	worknode = treenode_get_byname ( treenode_children ( infonode ), "FLAGS4" );
	if ( !worknode )
		header.flags4 = 0;
	else
		header.flags4 = treenode_get_content_data_hexval ( worknode );

	stringnode = treenode_get_byname ( treenode_children ( node ), "STRINGS" );
	tablenode = treenode_get_byname ( treenode_children ( stringnode ), "TABLES" );


	length = 0;
	chunkdata = malloc ( 1 );

	if ( stringnode && tablenode )
	{
		// get chunk data

		if ( (header.flags1 & 0xF0) == 0x90 )
			compressed = 1;
		else if ( (header.flags1 & 0xF0) == 0xA0 )
			compressed = 2;
		else 
			compressed = 0;

		if ( (header.flags1 & 0x80) == 0x80 )
			unicode = 1;

		// prepare token table space
		if ( compressed )
		{
			worklength = 0x200;

			chunkdata = realloc ( chunkdata, length + worklength );
			if ( !chunkdata )
				return E_FAIL;

			memset ( chunkdata + length, 0xFF, worklength ); 


			tokentable_pos = length;
			length += worklength;


		}

		// count the number of strings
		string_entries = 0;
		child = treenode_get_byname ( treenode_children ( stringnode ), "STRING" );
		while ( child )
		{
			string_entries++;
			child = treenode_get_byname ( treenode_next ( child ), "STRING" );
		}	

		alignment = 0;
		while ( (alignment + string_entries) % 2 )
			alignment++;

		chunkdata = realloc ( chunkdata, length + string_entries + alignment );
		if ( !chunkdata )
			return E_FAIL;

		memset ( chunkdata + length, 0xFF, string_entries + alignment ); 

		lengthtable_pos = length;
		length += string_entries + alignment;


		// generate UTF16 strings from UTF8 if neccessary
		child = treenode_get_byname ( treenode_children ( stringnode ), "STRING" );
		while ( child )
		{
			if ( unicode )
			{
				if ( ppmodify_build_text_utf16 ( child ) == E_FAIL )
				{
					printf ( " [e] Could not convert String %i in '%s' to UTF16.\n", stringpos, header.name );
					return E_FAIL;
				}
				worknode = treenode_get_byname ( treenode_children ( child ), "UTF16" );
				workbuffer = treenode_get_content_data_binary ( worknode, &worklength );

				if ( !workbuffer )
				{
					printf ( " [e] UTF-16 String %i in '%s' could not get built. Could be that this entry is empty! Worknode: %s, Type: %s, Content: %s\n", stringpos, header.name, worknode->name, worknode->content_type, worknode->content_data );
					//return E_FAIL;
				}

				if ( worklength % 2 )
				{
					CHECK_AND_FREE ( workbuffer );
					printf ( " [e] UTF-16 String %i in '%s' has odd length, must be 2 byte aligned.\n", stringpos, header.name );
					return E_FAIL;
				}

			}
			else
			{
				worknode = treenode_get_byname ( treenode_children ( child ), "UTF8" );
				workbuffer = treenode_get_content_data_binary ( worknode, &worklength );
				//workbuffer = treenode_get_content_ascii ( worknode, &worklength );
				if ( !workbuffer )
				{
					printf ( " [e] UTF-8 String %i in '%s' could not get retrieved. Worknode name: %s, worknode length: %d, content data: %s\n", stringpos, header.name, worknode->name, worklength, worknode->content_type );
					return E_FAIL;
				}
			}

			//if ( worklength )
			//	printf ( " [i] String %i in '%s' has content: 0x%02X%02X%02X%02X ...\n", stringpos, header.name, workbuffer[0], workbuffer[1], workbuffer[2], workbuffer[3] );

			CHECK_AND_FREE ( workbuffer );

			worknode = treenode_get_byname ( treenode_next ( child ), "STRING" );
			child = worknode;
			stringpos++;
		}

		if ( compressed )
		{
			unsigned int pos = 0;
			unsigned int used = 0;
			t_comp_dict dictionary;

			// initialize dictionary
			ppmodify_text_comp_dict_init ( &dictionary );

			if ( compressed == 1 )
				dictionary.max_entries = 0xFF;
			else
				dictionary.max_entries = 0x100;

			child = treenode_get_byname ( treenode_children ( stringnode ), "STRING" );
			while ( child )
			{
				worknode = treenode_get_byname ( treenode_children ( child ), "UTF16" );
				workbuffer = treenode_get_content_data_binary ( worknode, &worklength );

				// when we have LE, we have to swap bytes in every halfword
				if ( !endianess_be && (compressed == 2) )
				{
					t_stringbuffer utf16;

					utf16.buffer = (unsigned short*)workbuffer;
					utf16.allocated = worklength;
					utf16.length = worklength/2;

					utf16_switch_endianess ( &utf16 );
				}
				ppmodify_text_comp_dict_add_data ( &dictionary, workbuffer, worklength );

				CHECK_AND_FREE ( workbuffer );
				worknode = treenode_get_byname ( treenode_next ( child ), "STRING" );
				child = worknode;
			}

			if ( compressed == 2 )
				used = ppmodify_text_comp_dict_add_all_chars ( &dictionary );

			// now process all strings and replace until we have no more
			// empty tokens in the tokentable

			while ( used < dictionary.max_entries )
			{
				t_compress_buffer highest;
				highest.data = NULL;
				
				// that function never may return a failure except we have a pack
				// without any strings... cover that?
				R(ppmodify_text_comp_dict_get_highfreq ( &dictionary, &highest, header.flags1 ));

				// add the most frequent token to the table and
				// set the priv_data to the temporal token offset in our dictionary
				highest.priv_data = ppmodify_text_comp_dict_add_token ( &dictionary, highest.type, highest.token_1, highest.token_2 );

				if ( highest.priv_data == E_FAIL )
					PLUGIN_ERROR ( "Failed to add a token?!", E_FAIL );

				// now substitute all byte pairs with this token
				if ( ppmodify_text_comp_dict_subst_token ( &dictionary, &highest, header.flags1 ) == E_FAIL )
					PLUGIN_ERROR ( "Not expected to get no more replacements?!", E_FAIL ) ;

				used++;
			}

			// generate the final tokentable
			ppmodify_text_comp_dict_finish ( &dictionary, chunkdata + tokentable_pos, header.flags1 );

			// fixup compression 0xA0 tokens, meaning to align it to 2-byte size
			// or escape all uncompressed characters with 0xFF on type 0x90
			if ( compressed == 2 )
				ppmodify_text_comp_token_align ( &dictionary );
			else
				ppmodify_text_comp_token_escape ( &dictionary );

			// now write all tokens into the token fields of each string
			child = treenode_get_byname ( treenode_children ( stringnode ), "STRING" );
			pos = 0;
			while ( child )
			{
				unsigned char *buffer = NULL;
				unsigned int bytes = 0;

				worknode = treenode_get_byname ( treenode_children ( child ), "TOKEN" );
				if ( !worknode )
				{
					worknode = treenode_addchild ( child );

					treenode_set_name ( worknode, "TOKEN" );
				}

				buffer = ppmodify_text_comp_retrieve_tokenbuf ( &dictionary, pos, &worklength );

				treenode_set_content_data_binary ( worknode, buffer, worklength );
				CHECK_AND_FREE ( buffer );

				worknode = treenode_get_byname ( treenode_next ( child ), "STRING" );
				child = worknode;
				pos++;
			}

			// free dictionary
			ppmodify_text_comp_dict_release ( &dictionary );
		}

		// get all string tokens and append them
		stringpos = 0;
		child = treenode_get_byname ( treenode_children ( stringnode ), "STRING" );
		while ( child )
		{
			// prepare tokens
			if ( compressed == 1 )
			{
				worknode = treenode_get_byname ( treenode_children ( child ), "TOKEN" );
				workbuffer = treenode_get_content_data_binary ( worknode, &worklength );

				tablelength = worklength;
			}
			else if ( compressed == 2 )
			{
				worknode = treenode_get_byname ( treenode_children ( child ), "TOKEN" );
				workbuffer = treenode_get_content_data_binary ( worknode, &worklength );

				if ( worklength % 2 )
					PLUGIN_ERROR ( "Compression type 2 returned uneven token length?!", E_FAIL );
				tablelength = worklength / 2;
			}
			else if ( unicode )
			{
				worknode = treenode_get_byname ( treenode_children ( child ), "UTF16" );
				workbuffer = treenode_get_content_data_binary ( worknode, &worklength );

				// when we have LE, we have to swap bytes in every halfword
				if ( !endianess_be )
				{
					t_stringbuffer utf16;

					utf16.buffer = (unsigned short*)workbuffer;
					utf16.allocated = worklength;
					utf16.length = worklength/2;

					utf16_switch_endianess ( &utf16 );
				}
				tablelength = worklength / 2;
			}
			else
			{
				worknode = treenode_get_byname ( treenode_children ( child ), "UTF8" );
				workbuffer = treenode_get_content_data_binary ( worknode, &worklength );
				//workbuffer = treenode_get_content_ascii ( worknode, &worklength );

				tablelength = worklength;
			}

			if ( !workbuffer || worklength < 0 )
			{
				printf ( " [e] String %i in '%s' failed to build. Assuming it's a zero string.\n", stringpos, header.name );
				//return E_FAIL;
			}

			if ( tablelength >= 0xFF )
			{
				t_treenode *tempnode = treenode_get_byname ( treenode_children ( child ), "UTF8" );
				int templength = 0;
				unsigned char *tempbuffer = NULL;

				tempbuffer = treenode_get_content_ascii ( tempnode );
				if ( tempbuffer )
					printf ( " [!] String %i in '%s' too long! Truncated from %i to %i entries.\n      \"%s\"", stringpos, header.name, tablelength, 0xFE, tempbuffer );
				else
					printf ( " [!] String %i in '%s' too long! Truncated from %i to %i entries.\n      \"(null)\"", stringpos, header.name, tablelength, 0xFE  );
				tablelength = 0xFE;
			}

			chunkdata = realloc ( chunkdata, length + worklength );
			memcpy ( chunkdata + length, workbuffer, worklength ); 
			CHECK_AND_FREE ( workbuffer );

			length += worklength;

			chunkdata[lengthtable_pos + stringpos++] = tablelength;

			worknode = treenode_get_byname ( treenode_next ( child ), "STRING" );
			treenode_release ( child );
			child = worknode;
		}

	}

	// create binary representation
	buffer = malloc ( length + 0x10 );
	ws = workspace_create_file_from_buffer ( buffer, length + 0x10 );
	if ( !ws )
		return E_FAIL;

	ws->flags &= FLAGS_ENDIANESS;
	if ( endianess_be )
		ws->flags |= FLAGS_ENDIANESS_BE;
	else
		ws->flags |= FLAGS_ENDIANESS_LE;

	v_set_w ( ws, 0x00, header.id );

	if ( header.length != length + 0x10 )
		printf ( "[i] SubChunk '%s' length changed from %i to %i\n", header.name, header.length, length + 0x10 );

	v_set_w ( ws, 0x04, length + 0x10 );
	v_memcpy_put ( ws, 0x08, header.name, 0x04 );
	v_set_b ( ws, 0x0C, header.flags1 );
	v_set_b ( ws, 0x0D, header.flags2 );
	v_set_b ( ws, 0x0E, header.flags3 );
	v_set_b ( ws, 0x0F, header.flags4 );

	v_memcpy_put ( ws, 0x10, chunkdata, length );

	free ( chunkdata );

	// free subnodes

	treenode_release ( infonode );
	treenode_release ( stringnode );

	// replace with constructed data
	treenode_set_name ( node, "SUBCHUNK-BINARY" );
	treenode_set_content_data_binary ( node, buffer, length + 0x10 );

	workspace_release ( ws );


	return E_OK;
}

#endif

