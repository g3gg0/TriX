#ifndef __TRIX_CHUNK_TEXT_COMP_C__
#define __TRIX_CHUNK_TEXT_COMP_C__



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


unsigned int ppmodify_text_comp_token_escape ( t_comp_dict *dict )
{
	unsigned int token = 0;
	unsigned int pos = 0;

	while ( pos < dict->data_entries )
	{
		token = 0;
		while ( token < dict->data[pos].entries )
		{
			if ( dict->data[pos].data[token].type == TOKEN_TYPE_PLAIN )
			{
				if ( token + 1 >= dict->data[pos].entries || dict->data[pos].data[token+1].type != TOKEN_TYPE_PLAIN )
					PLUGIN_ERROR ( "Single TOKEN_TYPE_PLAIN there?!", E_FAIL );

				// token must be escaped
				dict->data[pos].data = realloc ( dict->data[pos].data, sizeof ( t_token_entry ) * (dict->data[pos].entries+1) );
				memmove ( &dict->data[pos].data[token+1], &dict->data[pos].data[token], sizeof ( t_token_entry ) * (dict->data[pos].entries - token ) );
				dict->data[pos].entries++;

				dict->data[pos].data[token  ].type = TOKEN_TYPE_TOKEN;
				dict->data[pos].data[token  ].value = 0xFF;
				dict->data[pos].data[token+1].type = TOKEN_TYPE_TOKEN;
				dict->data[pos].data[token+2].type = TOKEN_TYPE_TOKEN;

				// restart that loop
				token = 0;
			}
			else
				token++;
		}
		pos++;
	}

	return E_OK;
}


unsigned int ppmodify_text_comp_token_align ( t_comp_dict *dict )
{
	unsigned int token = 0;
	unsigned int pos = 0;

	while ( pos < dict->data_entries )
	{
		if ( dict->data[pos].entries % 2 )
		{
			token = 0;
			while ( token < dict->data[pos].entries )
			{
				if ( dict->data[pos].data[token].type == TOKEN_TYPE_TOKEN )
				{
					unsigned int tokenpos = 0;

					while ( tokenpos < dict->dict_entries && 
						dict->dict[tokenpos].priv_data != dict->data[pos].data[token].value  )
						tokenpos++;

					if ( tokenpos < dict->dict_entries && dict->dict[tokenpos].token_1 != dict->dict[tokenpos].token_2 )
					{
						// token can be replaced
						dict->data[pos].data = realloc ( dict->data[pos].data, sizeof ( t_token_entry ) * (dict->data[pos].entries+1) );
						memmove ( &dict->data[pos].data[token+2], &dict->data[pos].data[token+1], sizeof ( t_token_entry ) * (dict->data[pos].entries - token - 1 ) );
						dict->data[pos].entries++;

						dict->data[pos].data[token  ].type = TOKEN_TYPE_TOKEN;
						dict->data[pos].data[token  ].value = dict->dict[tokenpos].token_1;
						dict->data[pos].data[token+1].type = TOKEN_TYPE_TOKEN;
						dict->data[pos].data[token+1].value = dict->dict[tokenpos].token_2;

						//finish that loop
						token = dict->data[pos].entries;
					}
				}
				token++;
			}
			if ( dict->data[pos].entries % 2 )
				PLUGIN_ERROR ( "Could not replace tokens?!", E_FAIL );
		}

		pos++;
	}

	return E_OK;
}


unsigned int ppmodify_text_comp_dict_finish ( t_comp_dict *dict, unsigned char *tokenbuffer, unsigned short flags1 )
{
	unsigned int pos = 0;
	unsigned int token_start = 0xFF;

	while ( pos < dict->dict_entries )
	{
		if ( dict->dict[pos].type == TOKEN_TYPE_TOKEN && dict->dict[pos].priv_data < token_start )
			token_start = dict->dict[pos].priv_data;

		if ( dict->dict[pos].priv_data < dict->max_entries )
		{
			tokenbuffer[2*dict->dict[pos].priv_data  ] = dict->dict[pos].token_1;
			tokenbuffer[2*dict->dict[pos].priv_data+1] = dict->dict[pos].token_2;
		}
		else 
			return E_FAIL;
		pos++;
	}

	// type 0x90 compression, set the token start
	if ( (flags1 & 0xF0) == 0x90 )
	{
		tokenbuffer[2*0xFF  ] = token_start;
		tokenbuffer[2*0xFF+1] = 0xFF;
	}

	return E_OK;
}

unsigned int ppmodify_text_comp_dict_init ( t_comp_dict *dict )
{
	if ( !dict )
		return E_FAIL;

	dict->dict_entries = 0;
	dict->dict_allocated = 1;
	dict->data_entries = 0;
	dict->data_allocated = 1;

	dict->dict = malloc ( sizeof ( t_compress_buffer ) * dict->dict_allocated );
	dict->data = malloc ( sizeof ( t_compress_buffer ) * dict->data_allocated );

	if ( !dict->dict || !dict->data )
		PLUGIN_ERROR ( "Failed to allocate buffers?!", E_FAIL );

	// init dictionary table
	R(ppmodify_text_compress_init ( dict->dict ));

	// init string table
	R(ppmodify_text_compress_init ( dict->data ));

	return E_OK;
}

unsigned int ppmodify_text_comp_dict_release ( t_comp_dict *dict )
{
	unsigned int pos = 0;
	if ( !dict )
		return E_FAIL;

	pos = 0;
	// free dictionary table
	while ( pos < dict->dict_entries )
		R(ppmodify_text_compress_release ( &dict->dict[pos++] ));

	pos = 0;
	// free string table
	while ( pos < dict->data_entries )
		R(ppmodify_text_compress_release ( &dict->data[pos++] ));

	CHECK_AND_FREE ( dict->dict );
	CHECK_AND_FREE ( dict->data );

	return E_OK;
}

unsigned int ppmodify_text_comp_dict_add_token ( t_comp_dict *dict, unsigned char type, unsigned char token_1, unsigned char token_2 )
{
	int token_number = 0;
	unsigned int pos = 0;

	if ( dict->dict_entries >= dict->dict_allocated )
	{
		dict->dict_allocated++;
		dict->dict = realloc ( dict->dict, sizeof ( t_compress_buffer ) * dict->dict_allocated );
		if ( !dict->dict )
			PLUGIN_ERROR ( "Failed to reallocate dictionary data?!", E_FAIL );
	}

	dict->dict[dict->dict_entries].allocated = 0;
	dict->dict[dict->dict_entries].entries = 0;
	dict->dict[dict->dict_entries].data = NULL;
	dict->dict[dict->dict_entries].type = type;
	dict->dict[dict->dict_entries].token_1 = token_1;
	dict->dict[dict->dict_entries].token_2 = token_2;

	// priv_data will hold the position in the tokentable
	// -1 means, not allocated yet
	dict->dict[dict->dict_entries].priv_data = -1;


	// add plain tokens at the startt and token-tokens from the end
	if ( type == TOKEN_TYPE_PLAIN )
	{
		token_number = 0;
		while ( token_number < dict->max_entries )
		{
			pos = 0;
			while ( pos < dict->dict_entries && dict->dict[pos].priv_data != token_number )
				pos++;			

			if ( pos == dict->dict_entries )
			{
				// this token number is free
				dict->dict[dict->dict_entries].priv_data = token_number;
				token_number = dict->max_entries;
			}
			else
				token_number++;
		}
	}
	else
	{
		token_number = dict->max_entries - 1;
		while ( token_number >= 0 )
		{
			pos = 0;
			while ( pos < dict->dict_entries && dict->dict[pos].priv_data != token_number )
				pos++;			

			if ( pos == dict->dict_entries )
			{
				// this token number is free
				dict->dict[dict->dict_entries].priv_data = token_number;
				token_number = -1;
			}
			else
				token_number--;
		}
	}

	if ( dict->dict[dict->dict_entries].priv_data == -1 )
		return E_FAIL;

	// return the token position
	return dict->dict_entries++;
}

unsigned int ppmodify_text_comp_dict_is_free ( t_comp_dict *dict, unsigned char value )
{
	unsigned int pos = 0;

	while ( pos < dict->dict_entries )
	{
		if ( dict->dict[pos].priv_data == value )
			return 0;
		pos++;
	}

	return 1;
}

unsigned char *ppmodify_text_comp_retrieve_tokenbuf ( t_comp_dict *dict, unsigned int entry, unsigned int *length )
{
	unsigned int bytes = 0;
	unsigned int pos = 0;
	unsigned char *buffer = NULL;

	if ( !dict || !length || entry > dict->data_entries )
		return NULL;

	buffer = malloc ( 1 );
	while ( pos < dict->data[entry].entries )
	{
		buffer = realloc ( buffer, bytes + 1 );
		if ( !buffer )
			return NULL;
		buffer[bytes] = dict->data[entry].data[pos].value;
		bytes++;
		pos++;
	}

	*length = bytes;

	return buffer;
}

unsigned int ppmodify_text_comp_dict_add_tokenchar ( t_comp_dict *dict, unsigned char value )
{
	if ( !dict )
		return E_FAIL;

	if ( !ppmodify_text_comp_dict_is_free ( dict, value ) )
		return E_FAIL;

	if ( dict->dict_entries >= dict->dict_allocated )
	{
		dict->dict_allocated++;
		dict->dict = realloc ( dict->dict, sizeof ( t_compress_buffer ) * dict->dict_allocated );
		if ( !dict->dict )
			PLUGIN_ERROR ( "Failed to reallocate dictionary data?!", E_FAIL );
	}

	dict->dict[dict->dict_entries].allocated = 0;
	dict->dict[dict->dict_entries].entries = 0;
	dict->dict[dict->dict_entries].data = NULL;
	dict->dict[dict->dict_entries].type = TOKEN_TYPE_TOKEN;
	dict->dict[dict->dict_entries].token_1 = value;
	dict->dict[dict->dict_entries].token_2 = value;

	// priv_data will hold the position in the tokentable
	dict->dict[dict->dict_entries].priv_data = value;

	// return the token position
	return dict->dict_entries++;
}


unsigned int ppmodify_text_comp_dict_add_data ( t_comp_dict *dict, unsigned char *buffer, unsigned int length )
{
	unsigned int pos = 0;

	if ( dict->data_entries >= dict->data_allocated )
	{
		dict->data_allocated++;
		dict->data = realloc ( dict->data, sizeof ( t_compress_buffer ) * dict->data_allocated );
		if ( !dict->data )
			PLUGIN_ERROR ( "Failed to reallocate dictionary data?!", E_FAIL );
	}

	// entry is unused, initialize first
	R(ppmodify_text_compress_init ( &dict->data[dict->data_entries] ));

	while ( pos < length )
		ppmodify_text_compress_add ( &dict->data[dict->data_entries], TOKEN_TYPE_PLAIN, buffer[pos++] );

	dict->data_entries++;

	return E_OK;
}


//
// get the byte pairs with the highest appearance rate
// designed for compression scheme 0xA0
//
unsigned int ppmodify_text_comp_dict_get_highfreq ( t_comp_dict *dict, t_compress_buffer *highest, unsigned short flags1 )
{
	unsigned int pos = 0;
	unsigned int entry_pos = 0;
	unsigned int first = 0;

	unsigned int appearance[0x10000] = { 0 };
	unsigned int appearance_tokens[0x10000] = { 0 };

	unsigned char token_1 = 0;
	unsigned char token_2 = 0;
	unsigned int highest_count = 0;
	unsigned int highest_pos = 0;
	unsigned int highest_tokentype = 0;

	if ( !dict || !dict->data )
		return E_FAIL;

	if ( (flags1 & 0xF0) == 0xA0 )
	{
		while ( pos < dict->data_entries )
		{
			first = 1;
			entry_pos = 0;
			while ( entry_pos < dict->data[pos].entries )
			{
				if ( first )
				{
					first = 0;
					// token_1 will get overwritten next loop, but set it to a defined state
					token_1 = 0;
					token_2 = dict->data[pos].data[entry_pos].value;
				}
				else
				{
					token_1 = token_2;
					token_2 = dict->data[pos].data[entry_pos].value;

					// now check for token_1 token_2 appearance and update counter
					appearance[(token_1<<8) | token_2]++;
					if ( appearance[(token_1<<8) | token_2] > highest_count )
					{
						highest_count = appearance[(token_1<<8) | token_2];
						highest_pos = (token_1<<8) | token_2;
						highest_tokentype = TOKEN_TYPE_TOKEN;
					}
				}
				entry_pos++;
			}
			pos++;
		}
	}
	else if ( (flags1 & 0xF0) == 0x90 )
	{
		while ( pos < dict->data_entries )
		{
			entry_pos = 0;
			while ( entry_pos + 1 < dict->data[pos].entries )
			{
				if ( dict->data[pos].data[entry_pos].type == TOKEN_TYPE_PLAIN )
				{
					unsigned int tokentype = 0;
					unsigned short tokenpair = 0;

					if ( dict->data[pos].data[entry_pos].type != TOKEN_TYPE_PLAIN )
						PLUGIN_ERROR ( "Expected PLAIN token?!", E_FAIL );

					token_1 = dict->data[pos].data[entry_pos].value;
					token_2 = dict->data[pos].data[entry_pos+1].value;

					tokenpair = (token_1<<8) | token_2;

					// now check for token_1 token_2 appearance and update counter
					appearance[tokenpair]++;

					if ( appearance[tokenpair] > highest_count )
					{
						highest_count = appearance[tokenpair];
						highest_pos = tokenpair;
						highest_tokentype = TOKEN_TYPE_PLAIN;
					}
					entry_pos += 2;
				}
				else if ( dict->data[pos].data[entry_pos].type == TOKEN_TYPE_TOKEN )
				{
					if ( dict->data[pos].data[entry_pos+1].type == TOKEN_TYPE_TOKEN )
					{
						unsigned short tokenpair = 0;

						token_1 = dict->data[pos].data[entry_pos].value;
						token_2 = dict->data[pos].data[entry_pos+1].value;

						tokenpair = (token_1<<8) | token_2;

						// now check for token_1 token_2 appearance and update counter
						appearance_tokens[tokenpair]++;

						if ( appearance_tokens[tokenpair] > highest_count )
						{
							highest_count = appearance_tokens[tokenpair];
							highest_pos = tokenpair;
							highest_tokentype = TOKEN_TYPE_TOKEN;
						}
					}

					entry_pos++;
				}
				else
					PLUGIN_ERROR ( "Invalid token type?!", E_FAIL );
			}
			pos++;
		}
	}
	else
		return E_FAIL;

	if ( !highest_count )
		return E_FAIL;

	highest->token_1 = highest_pos >> 8;
	highest->token_2 = highest_pos & 0xFF;
	highest->type = highest_tokentype;

	return E_OK;
}


unsigned int ppmodify_text_comp_dict_subst_token ( t_comp_dict *dict, t_compress_buffer *highest, unsigned short flags1 )
{
	unsigned int replacements = 0;
	unsigned int pos = 0;
	unsigned int entry_pos = 0;
	unsigned int first = 0;
	unsigned char token_1 = 0;
	unsigned char token_2 = 0;

	if ( !dict || !dict->data )
		return E_FAIL;

	if ( (flags1 & 0xF0) == 0xA0 )
	{
		while ( pos < dict->data_entries )
		{
			first = 1;
			entry_pos = 0;
			while ( entry_pos < dict->data[pos].entries )
			{
				if ( first )
				{
					first = 0;
					// token_1 will get overwritten next loop, but set it to a defined state
					token_1 = 0;
					token_2 = dict->data[pos].data[entry_pos].value;
				}
				else
				{
					token_1 = token_2;
					token_2 = dict->data[pos].data[entry_pos].value;

					// now check for token_1 token_2 and replace
					if ( token_1 == highest->token_1 && token_2 == highest->token_2 )
					{
						if ( highest->priv_data >= dict->dict_entries )
							PLUGIN_ERROR ( "Invalid token offset?!", E_FAIL );

						dict->data[pos].data[entry_pos-1].value = dict->dict[highest->priv_data].priv_data;
						dict->data[pos].data[entry_pos-1].type = TOKEN_TYPE_TOKEN;

						memmove ( &dict->data[pos].data[entry_pos], &dict->data[pos].data[entry_pos+1], sizeof ( t_token_entry ) * (dict->data[pos].entries - entry_pos - 1) );
						dict->data[pos].entries--;

						// rescan the string from start
						first = 1;
						entry_pos = 0;

						replacements++;
					}
				}
				entry_pos++;
			}
			pos++;
		}
	}
	else if ( (flags1 & 0xF0) == 0x90 )
	{
		while ( pos < dict->data_entries )
		{
			entry_pos = 0;
			while ( entry_pos + 1 < dict->data[pos].entries )
			{
				if ( dict->data[pos].data[entry_pos].type == TOKEN_TYPE_PLAIN )
				{
					if ( dict->data[pos].data[entry_pos].type != TOKEN_TYPE_PLAIN )
						PLUGIN_ERROR ( "Expected PLAIN token?!", E_FAIL );

					token_1 = dict->data[pos].data[entry_pos].value;
					token_2 = dict->data[pos].data[entry_pos+1].value;

					// now check for token_1 token_2 and replace
					if ( token_1 == highest->token_1 && token_2 == highest->token_2 && dict->data[pos].data[entry_pos].type == highest->type )
					{
						if ( highest->priv_data >= dict->dict_entries )
							PLUGIN_ERROR ( "Invalid token offset?!", E_FAIL );

						mem_check_overwrite ( dict->data[pos].data );
						memmove ( &dict->data[pos].data[entry_pos+1], &dict->data[pos].data[entry_pos+2], sizeof ( t_token_entry ) * (dict->data[pos].entries - entry_pos - 2) );
						dict->data[pos].entries--;

						dict->data[pos].data[entry_pos].value = dict->dict[highest->priv_data].priv_data;
						dict->data[pos].data[entry_pos].type = TOKEN_TYPE_TOKEN;
						mem_check_overwrite ( dict->data[pos].data );

						// rescan the string from start
						first = 1;
						entry_pos = 0;

						replacements++;
					}
					else
						entry_pos += 2;
				}
				else if ( dict->data[pos].data[entry_pos].type == TOKEN_TYPE_TOKEN )
				{
					if ( dict->data[pos].data[entry_pos+1].type == TOKEN_TYPE_TOKEN )
					{

						token_1 = dict->data[pos].data[entry_pos].value;
						token_2 = dict->data[pos].data[entry_pos+1].value;

						// now check for token_1 token_2 and replace
						if ( token_1 == highest->token_1 && token_2 == highest->token_2 && dict->data[pos].data[entry_pos].type == highest->type )
						{
							if ( highest->priv_data >= dict->dict_entries )
								PLUGIN_ERROR ( "Invalid token offset?!", E_FAIL );

							mem_check_overwrite ( dict->data[pos].data );
							memmove ( &dict->data[pos].data[entry_pos+1], &dict->data[pos].data[entry_pos+2], sizeof ( t_token_entry ) * (dict->data[pos].entries - entry_pos - 2) );
							dict->data[pos].entries--;

							dict->data[pos].data[entry_pos].value = dict->dict[highest->priv_data].priv_data;
							dict->data[pos].data[entry_pos].type = TOKEN_TYPE_TOKEN;

							mem_check_overwrite ( dict->data[pos].data );
							// rescan the string from start
							first = 1;
							entry_pos = 0;

							replacements++;
						}
						else
							entry_pos++;
					}
					else
						entry_pos++;
				}
				else
					PLUGIN_ERROR ( "Invalid token type?!", E_FAIL );
			}
			pos++;
		}
	}
	else 
		return E_FAIL;

	return replacements;
}

//
// add all appearing chars to the tokentable
// just used for the 0xA0 compression scheme
//
unsigned int ppmodify_text_comp_dict_add_all_chars ( t_comp_dict *dict )
{
	unsigned int used = 0;
	unsigned int pos = 0;
	unsigned int entry_pos = 0;

	if ( !dict || !dict->data )
		return E_FAIL;

	while ( pos < dict->data_entries )
	{
		entry_pos = 0;
		while ( entry_pos < dict->data[pos].entries )
		{
			if ( dict->data[pos].data[entry_pos].type == TOKEN_TYPE_PLAIN )
			{
				int token_pos = ppmodify_text_comp_dict_add_tokenchar ( dict, dict->data[pos].data[entry_pos].value );
				if ( token_pos != E_FAIL )
				{
//					if ( dict->data[pos].data[entry_pos].value > 0x20 && dict->data[pos].data[entry_pos].value < 0x80 )
//						printf ( "Associated char token %02X ('%c') to token id %02X\n", dict->data[pos].data[entry_pos].value, dict->data[pos].data[entry_pos].value, token_pos );
//					else
//						printf ( "Associated char token %02X       to token id %02X\n", dict->data[pos].data[entry_pos].value, token_pos );
					used++;
				}
			}
			entry_pos++;
		}
		pos++;
	}

	return used;
}

t_compress_buffer *ppmodify_text_comp_build_table ( unsigned char *tokentable, unsigned short flags1, unsigned short flags2 )
{
	unsigned int pos = 0;
	unsigned int done = 0;
	unsigned int actual = 0;
	unsigned int highest_id = 0;
	unsigned int highest_length = 0;
	unsigned int max_tokens = 0;
	t_compress_buffer *token_list = NULL;
	t_compress_buffer backup;

	if ( !tokentable )
		return NULL;

	// allocate token table
	token_list = malloc ( sizeof ( t_compress_buffer) * 0x100 );
	if ( !token_list )
		return NULL;

	if ( (flags1 & 0xF0) == 0xA0 )
		max_tokens = 0x100;
	else
		max_tokens = 0xFF;

	// allocate t_compress_buffer
	for ( pos=0; pos<max_tokens; pos++ )
		ppmodify_text_compress_init ( &token_list[pos] );

	// dump all tokens
	for ( pos=0; pos<max_tokens; pos++ )
	{
		unsigned char tokenbuf[1];

		tokenbuf[0] = pos;
		ppmodify_text_comp_compress ( tokenbuf, 1, tokentable, &token_list[pos], flags1, 0 );
		token_list[pos].priv_data = pos;
		token_list[pos].token_1 = tokentable[2*pos];
		token_list[pos].token_2 = tokentable[2*pos+1];
	}

	// sort tokens by their length
	while ( actual < max_tokens )
	{
		highest_length = 0;		
		for ( pos=actual; pos<max_tokens; pos++ )
		{
			if ( token_list[pos].entries > highest_length )
			{
				highest_id = pos;
				highest_length = token_list[pos].entries;
			}
		}

		// swap current pos with the found highest
		memcpy ( &backup, &token_list[actual], sizeof(t_compress_buffer) );
		memcpy ( &token_list[actual], &token_list[highest_id], sizeof(t_compress_buffer) );
		memcpy ( &token_list[highest_id], &backup, sizeof(t_compress_buffer) );

		actual++;
	}

	return token_list;
}

unsigned int ppmodify_text_compress_init ( t_compress_buffer *buffer )
{
	if ( !buffer )
		return E_FAIL;

	buffer->entries = 0;
	buffer->priv_data = 0;
	buffer->allocated = 1;

	buffer->data = malloc ( sizeof ( t_token_entry ) * buffer->allocated );

	if ( !buffer->data )
		return E_FAIL;

	return E_OK;
}

unsigned int ppmodify_text_compress_release ( t_compress_buffer *buffer )
{
	if ( !buffer )
		return E_FAIL;

	CHECK_AND_FREE ( buffer->data );

	return E_OK;
}

unsigned int ppmodify_text_comp_decompress ( t_compress_buffer *buffer, unsigned char *tokens, unsigned short flags1 )
{
	unsigned int loop = 0;
	int done = 0;
	unsigned int pos = 0;

	if ( !buffer || !tokens )
		return E_FAIL;

	while ( !done )
	{
		pos = 0;
		done = 1;
		while ( pos < buffer->entries )
		{
			if ( loop++ > 0x1000000 )
				PLUGIN_ERROR ( "Deadlock while decompressing?!", E_FAIL );

			if ( (flags1 & 0xF0) == 0xA0 )
			{
				if ( tokens[2*buffer->data[pos].value] == tokens[2*buffer->data[pos].value+1] &&
					tokens[2*buffer->data[pos].value] == buffer->data[pos].value )
				{
					buffer->data[pos++].type = TOKEN_TYPE_PLAIN;
				}
				else
				{
					unsigned char token_1 = tokens[2*buffer->data[pos].value];
					unsigned char token_2 = tokens[2*buffer->data[pos].value+1];

					buffer->data = realloc ( buffer->data, sizeof ( t_token_entry ) * (buffer->entries+1) );
					memmove ( &buffer->data[pos+2], &buffer->data[pos+1], sizeof ( t_token_entry ) * (buffer->entries - pos - 1 ) );

					buffer->entries++;

					buffer->data[pos  ].value = token_1;
					buffer->data[pos  ].type  = TOKEN_TYPE_TOKEN;
					buffer->data[pos+1].value = token_2;
					buffer->data[pos+1].type  = TOKEN_TYPE_TOKEN;

					// re-run again
					done = 0;
					pos = 0;
				}
			}
			else if ( (flags1 & 0xF0) == 0x90 )
			{
				if ( buffer->data[pos].type == TOKEN_TYPE_PLAIN )
				{
					pos++;
				}
				else if ( buffer->data[pos].value == 0xFF )
				{
					if ( pos + 2 >= buffer->entries )
						PLUGIN_ERROR ( "Decompress token at end?!", E_FAIL );
					// have a 0xFF 0xAA 0xBB sequence, cut the 0xFF and mark the 2 bytes as plain chars
					mem_check_overwrite ( buffer->data );
					memmove ( &buffer->data[pos], &buffer->data[pos+1], sizeof ( t_token_entry ) * (buffer->entries - pos - 1 ) );
					buffer->entries--;
					buffer->data = realloc ( buffer->data, sizeof ( t_token_entry ) * buffer->entries );

					buffer->data[pos  ].type  = TOKEN_TYPE_PLAIN;
					buffer->data[pos+1].type  = TOKEN_TYPE_PLAIN;

					if ( !endianess_be )
					{
						unsigned char tempval = 0;
						tempval = buffer->data[pos  ].value;
						buffer->data[pos  ].value = buffer->data[pos+1].value;
						buffer->data[pos+1].value = tempval;
					}

					mem_check_overwrite ( buffer->data );

				}
				else
				{
					unsigned char token_1 = tokens[2*buffer->data[pos].value];
					unsigned char token_2 = tokens[2*buffer->data[pos].value+1];
					unsigned char token = buffer->data[pos].value;

					mem_check_overwrite ( buffer->data );

					buffer->data = realloc ( buffer->data, sizeof ( t_token_entry ) * (buffer->entries+1) );
					memmove ( &buffer->data[pos+2], &buffer->data[pos+1], sizeof ( t_token_entry ) * (buffer->entries - pos - 1 ) );
					buffer->entries++;

					if ( token < tokens[0x1FE] )
					{
						if ( endianess_be )
						{
							buffer->data[pos  ].value = token_1;
							buffer->data[pos+1].value = token_2;
						}
						else
						{
							buffer->data[pos  ].value = token_2;
							buffer->data[pos+1].value = token_1;
						}

						buffer->data[pos  ].type  = TOKEN_TYPE_PLAIN;
						buffer->data[pos+1].type  = TOKEN_TYPE_PLAIN;
					}
					else
					{
						buffer->data[pos  ].value = token_1;
						buffer->data[pos+1].value = token_2;
						buffer->data[pos  ].type  = TOKEN_TYPE_TOKEN;
						buffer->data[pos+1].type  = TOKEN_TYPE_TOKEN;
					}
					mem_check_overwrite ( buffer->data );

					// re-run again
					done = 0;
					pos = 0;
				}
			}
			else
				return E_FAIL;
		}
	}
	return E_OK;
}

unsigned int ppmodify_text_compress_add ( t_compress_buffer *buffer, unsigned char type, unsigned char data )
{
	if ( !buffer )
		return E_FAIL;

	// doesnt exist, allocate
	if ( !buffer->data )
	{
		buffer->entries = 0;
		buffer->allocated = 1;
		buffer->data = malloc ( sizeof ( t_token_entry ) * buffer->allocated );

		if ( !buffer->data )
			return E_FAIL;
	}

	// is too small, reallocate
	if ( buffer->entries >= buffer->allocated )
	{
		buffer->allocated++;
		buffer->data = realloc ( buffer->data, sizeof ( t_token_entry ) * buffer->allocated );

		if ( !buffer->data )
			return E_FAIL;
	}

	buffer->data[buffer->entries].done = 0;
	buffer->data[buffer->entries].type = type;
	buffer->data[buffer->entries].value = data;

	buffer->entries++;

	return E_OK;
}


unsigned int ppmodify_text_comp_compress ( unsigned char *inbuffer, int length, unsigned char *tokens, t_compress_buffer *buffer, unsigned short flags1, int recursive )
{
	unsigned int subcall_len = 0;
	unsigned int pos = 0;
	unsigned int comp1 = 0;
	unsigned char subcall_buf[2];
	unsigned char token = 0;

	if ( recursive > 1000 )
		return E_FAIL;

	if ( !inbuffer || !tokens || !buffer )
		return E_FAIL;

	if ( (flags1 & 0xF0) == 0xA0 )
		comp1 = 1;
	else if ( (flags1 & 0xF0) != 0x90 )
		printf ( "flags1 of TEXT subchunk probably not supported. Unknown compression.\n" );

	while ( pos < length )
	{
		HEAP_CHECK;
		token = inbuffer[pos++];

		if ( !comp1 && token < tokens[0x1FE] )
		{
			ppmodify_text_compress_add ( buffer, TOKEN_TYPE_PLAIN, tokens[2*token] );
			ppmodify_text_compress_add ( buffer, TOKEN_TYPE_PLAIN, tokens[2*token+1] );
		}
		else if ( comp1 && token == tokens[2*token] )
		{
			ppmodify_text_compress_add ( buffer, TOKEN_TYPE_PLAIN, token );
		}
		else
		{
			if ( !comp1 && token == 0xFF )
			{
				if ( pos + 2 > length )
					return E_FAIL;
					
				ppmodify_text_compress_add ( buffer, TOKEN_TYPE_PLAIN, inbuffer[pos] );
				ppmodify_text_compress_add ( buffer, TOKEN_TYPE_PLAIN, inbuffer[pos + 1] );
				pos += 2;
			}

			else
			{
				subcall_len = 2;
				subcall_buf[0] = tokens[2*token];
				subcall_buf[1] = tokens[2*token + 1];
				ppmodify_text_comp_compress ( subcall_buf, subcall_len, tokens, buffer, flags1, recursive+1 );
			}
		}
	}
	return E_OK;
}





#endif

