#ifndef __TRIX_FMT_PHOENIX_C__
#define __TRIX_FMT_PHOENIX_C__

#include <stdlib.h>
#include "defines.h"
#include "segment.h"
#include "stage.h"
#include "symbols.h"

#include "fmt.h"
#include "file_io.h"
#include "file.h"
#include "workspace.h"
#include "util.h"
#include "options.h"

#include "mem.h"

#define TOKEN_ERASE         0xC8
#define TOKEN_CLAUDIA       0xD3

int skip_claudia = 0;
int blocksize = 0x00010000;
int use_flashtype = 0x14;
int add_flashareas = 1;
int bb5_split_keydata = 1;

/*
BB5:
54 0117 0E000000 833D 0100 0000A0 00000400 74
54 0117 0E000000 29B9 0100 000200 00080000 EC
54 0117 0E000000 A883 0100 004000 000E0400 DC
54 0117 0E000000 EF58 0100 002580 000C0400 XX
54 0117 0E000000 4C25 0100 000400 000CE180 XX
54 0117 0E000000 5EEA 0100 004000 000F0400 XX
54 0117 0E000000 5B56 0100 004000 000F4400 XX
54 0117 0E010100 934F 0100 0000A0 00080000 4C
HH      DEVICE   DCHK UNK  BLKSZ  STARTADD HC


5D 0127 2D A5404AE83A594ECADEE532F0C236BFA6C53C9AF0 4E4F4C4F0000000000000000000000 6D92 00000400 000C0000 D4
5D 0127 2D E9EFF4BFAA5393217CA6B17755FC3E1415658E9A 4E4F4C4F0000000000000000000000 EADF 00000400 000C0000 CE
5D 0127 2D BAF3A9C3DBFA8454937DB77F2B8852B124CD5A6C 4E4F4C4F0000000000000000000000 FCBA 00000400 000C0000 33
5D 0128 43 E9EFF4BFAA5393217CA6B17755FC3E1415658E9A 50415055424B455953000000000000 1D26 00000600 FFFFFFFF 64 3B2F33E3E6EEFC2D6F45213419C8E368FCB9960100 6D
HH      HLEN                                        NAME/DESC                      DCHK BLKSZ             LAST_BYTE_IS_H_CHECKSUM

*/

typedef struct s_phoenix_erase t_phoenix_erase;
struct s_phoenix_erase
{
	STRUCT_HEADER_LIST(t_phoenix_erase);
	unsigned int start;
	unsigned int end;
};

typedef struct s_phoenix_tokens t_phoenix_tokens;
struct s_phoenix_tokens
{
	STRUCT_HEADER_LIST(t_phoenix_tokens);
    unsigned char token_id;     // e.g. 0xD3
    unsigned char length;       // e.g. 0x40
	unsigned char double_length;
    unsigned char *data;
};

typedef struct s_phoenix_priv t_phoenix_priv;
struct s_phoenix_priv
{
	STRUCT_HEADER;
    unsigned char type;         // type of flash: 0xA0 0xA1 0xA2
    t_phoenix_tokens *tokens;   // all the flash tokens
};

typedef struct s_phoenix_segpriv t_phoenix_segpriv;
struct s_phoenix_segpriv
{
	STRUCT_HEADER;
    unsigned char type;         // type of flash block e.g. 0x14
    unsigned int blocksize;    // blocksize used in original flash
	unsigned int fix_checksum;
};



unsigned char
fmt_phoenix_checksum ( unsigned char *data, int length )
{
	char chk = 0;

	while ( length )
		chk += data[--length];
	return chk ^ 0xFF;
}


unsigned int
fmt_phoenix_init_stage ( t_stage * s, unsigned char type )
{
    t_phoenix_priv *priv = NULL;
    if ( s->priv )
        return E_FAIL;
    s->priv = (t_stage_priv*)PRIV_MALLOC(t_phoenix_priv);
    if ( !s->priv )
        return E_FAIL;

    priv = ( t_phoenix_priv * ) s->priv;
	priv->struct_refs = 1;
    priv->type = type;
    priv->tokens = NULL;

    return E_OK;
}

unsigned int
fmt_phoenix_init_segment ( t_segment * s, unsigned char type, unsigned int blocksize, int fix_checksum )
{
    t_phoenix_segpriv *priv = NULL;
    if ( s->priv )
        return E_FAIL;
    s->priv = (t_seg_priv*)PRIV_MALLOC(t_phoenix_segpriv);
    if ( !s->priv )
        return E_FAIL;

    priv = ( t_phoenix_segpriv * ) s->priv;
	priv->struct_refs = 1;
    priv->type = type;
    priv->blocksize = blocksize;
    priv->fix_checksum = fix_checksum;

    return E_OK;
}

unsigned int
fmt_phoenix_free_tokens ( t_phoenix_tokens * t )
{

    if ( !t )
        return E_FAIL;

    while ( t->next )
        t = t->next;

    while ( t->prev )
    {
        t = t->prev;
        if ( t->next->data )
            free ( t->next->data );
        free ( t->next );
    }
    if ( t->data )
        free ( t->data );
    free ( t );

    return E_OK;
}

unsigned int
fmt_phoenix_free_segments ( t_segment * s )
{
    if ( !s )
        return E_FAIL;

	R ( segment_release_all ( s ) );

    return E_OK;
}

unsigned int
fmt_phoenix_free ( t_stage * s )
{
    t_phoenix_priv *priv = NULL;

    if ( !s )
        return E_OK;

    if ( s->priv )
    {
        priv = ( t_phoenix_priv * ) s->priv;
		if ( priv->struct_refs == 0 )
			printf ( "already freed?!\n" );
		else
			priv->struct_refs--;

		if ( priv->struct_refs == 0 )
		{
			if ( priv->tokens )
				R ( fmt_phoenix_free_tokens ( priv->tokens ) );
			priv->tokens = NULL;
			free ( priv );
			s->priv = NULL;
		}
    }

    fmt_phoenix_free_segments ( s->segments );
    s->segments = NULL;

    return E_OK;
}

unsigned int
fmt_phoenix_add_token ( t_stage * s, unsigned char token_id, unsigned char length, unsigned char *data, unsigned char double_length )
{
    t_phoenix_priv *priv = NULL;
    t_phoenix_tokens *t = NULL;

    if ( !s || !s->priv )
        return E_FAIL;

    priv = ( t_phoenix_priv * ) s->priv;

	t = LIST_NEW(priv->tokens,t_phoenix_tokens);
    t->token_id = token_id;
    t->length = length;
	t->double_length = double_length;
    t->data = malloc ( length );
    if ( !t->data )
        return E_FAIL;
    memcpy ( t->data, data, length );

	if ( !priv->tokens )
		priv->tokens = t;

    return E_OK;
}

unsigned int
fmt_phoenix_parse_data ( t_stage * source, t_stage * target, unsigned int offset )
{
    t_segment *current_seg = NULL;
    t_segment *existing_seg = NULL;
    unsigned int pos = offset;
    unsigned int last_end = 0;
    unsigned char type = 0;
    unsigned char last_type = 0;
    unsigned int done = 0;

	unsigned int target_dev = 0;
	unsigned int length = 0;
    unsigned int start = 0;
    unsigned short d_chk = 0;
    unsigned short h_chk = 0;
	unsigned int fix_checksum = 0;
	unsigned int temp_length = 0;
	unsigned int malloc_step = 1000000;

	unsigned char expected_name[64];


    while ( pos < source->length )
    {
        type = GET_BYTE ( source, pos );
		switch ( ((t_phoenix_priv*)target->priv)->type )
		{
			case 0xA0:
			case 0xA1:
			case 0xA2:
				if ( type == 0x14 )
				{
					start = GET_WORD ( source, pos + 1 );
					d_chk = GET_BYTE ( source, pos + 5 );
					length = GET_TRIP ( source, pos + 6 );
					h_chk = GET_BYTE ( source, pos + 9 );
					pos += 10;
				}
				else if ( type == 0x20 )
				{
					length = GET_HALF ( source, pos + 3 );
					pos += 6;
				}
				else if ( type == 0x21 )
				{
					start = GET_WORD ( source, pos + 7 );
					d_chk = GET_BYTE ( source, pos + 11 );
					length = GET_TRIP ( source, pos + 12 );
					h_chk = GET_BYTE ( source, pos + 15 );
					pos += 16;
				}
				else if ( type == 0x2E )
				{
					target_dev = GET_HALF ( source, pos + 1 );
					start = GET_HALF ( source, pos + 3 ) << 9;
					d_chk = GET_BYTE ( source, pos + 5 );
					length = GET_TRIP ( source, pos + 6 );
					h_chk = GET_BYTE ( source, pos + 9 );

					sprintf ( expected_name, "FLASH DEV: 0x%08X", target_dev );

					if ( (fmt_phoenix_checksum ( GET_PTR ( source, pos ), 10 ) & 0xFF) != h_chk )
						printf ( "CHECKSUM ERROR\n" );

					pos += 10;
				}
				else 
					return E_FAIL;

				break;

			case 0xB2:
				if ( type == 0x54 )
				{
					target_dev = GET_WORD ( source, pos + 3 );
					start = GET_WORD ( source, pos + 14 );
					d_chk = GET_HALF ( source, pos + 7 );
					length = GET_TRIP ( source, pos + 11 );
					h_chk = GET_BYTE ( source, pos + 18 );

					sprintf ( expected_name, "FLASH DEV: 0x%08X", target_dev );

					if ( (fmt_phoenix_checksum ( GET_PTR ( source, pos + 1 ), 17 ) & 0xFF) != h_chk )
						printf ( "CHECKSUM ERROR\n" );

					pos += 19;
				}
				else if ( type == 0x5D )
				{
					unsigned short header_len = 4 + GET_BYTE ( source, pos + 3 ) + 1;

					length = GET_WORD ( source, pos + 0x29 );
					h_chk = GET_BYTE ( source, pos + header_len - 1 );

					if ( bb5_split_keydata )
						sprintf ( expected_name, "KEYDATA: <%s>", GET_PTR ( source, pos + 0x18 ) );
					else
						sprintf ( expected_name, "KEYDATA", target_dev );

					if ( (fmt_phoenix_checksum ( GET_PTR ( source, pos + 1 ) , header_len - 2 ) & 0xFF) != h_chk )
						printf ( "CHECKSUM ERROR\n" );

					pos += header_len;
				}
				else 
					return E_FAIL;
				break;

			default:
				return E_FAIL;
		}


        if ( pos + length > source->length )
            return E_FAIL;

        if ( type != last_type || last_end != start )
        {
            if ( current_seg && (last_type == 0x14) )
                current_seg->end = current_seg->start + current_seg->length;
            if ( current_seg && (last_type == 0x2E) )
                current_seg->end = current_seg->start + current_seg->length;
            if ( current_seg && (last_type == 0x54) )
                current_seg->end = current_seg->start + current_seg->length;
			if ( current_seg )
				current_seg->data = realloc ( current_seg->data, current_seg->length );

            existing_seg = NULL;
            if ( !last_type )   // the first time we have to add first segment
            {
                target->segments = segment_create (  );
                current_seg = target->segments;
            }
            else                // else just append one
            {
                // first try to find an existing segment
                if ( type == 0x14 )
                    existing_seg = segment_find_by_end ( target->segments, start );
                else if ( type == 0x21 )
                    existing_seg = segment_find_by_end ( target->segments, start );
                else if ( type == 0x2E )
                    existing_seg = segment_find_by_end ( target->segments, start );
                else if ( type == 0x54 )
                    existing_seg = segment_find_by_end_and_name ( target->segments, start, expected_name );
                else if ( type == 0x5D )
                    existing_seg = segment_find_by_end_and_name ( target->segments, start, expected_name );
                else if ( type == 0x20 )
                    existing_seg = segment_find_by_name ( target->segments, "BT" );
                else
                    existing_seg = segment_find_by_name ( target->segments, "UNK" );

				// segment found and is same type
				if ( existing_seg && existing_seg->priv && ((t_phoenix_segpriv*) existing_seg->priv)->type == type )
                    current_seg = existing_seg;
                else
                    current_seg = segment_add ( current_seg );
            }
            if ( !existing_seg )
            {
                if ( type == 0x14 )
					fix_checksum = d_chk - (fmt_phoenix_checksum ( GET_PTR ( source, pos ), length ) & 0xFF);
                else if ( type == 0x2E )
					fix_checksum = d_chk - (fmt_phoenix_checksum ( GET_PTR ( source, pos ), length ) & 0xFF);
                else if ( type == 0x54 )
					fix_checksum = d_chk - (fmt_phoenix_checksum ( GET_PTR ( source, pos ), length ) & 0xFF);

                fmt_phoenix_init_segment ( current_seg, type, length, fix_checksum );

                if ( type == 0x14 )
                {
                    current_seg->name = "FLASH (0x14)";
                    current_seg->start = start;
                }
                else if ( type == 0x21 )
                {
                    current_seg->name = "FLASH (0x21)";
                    current_seg->start = start;
                }
                else if ( type == 0x2E )
                {
                    current_seg->name = "FLASH (0x2E)";
                    current_seg->start = start;
                }
                else if ( type == 0x54 )
                {
                    current_seg->name = strdup ( expected_name );
                    current_seg->start = start;
					current_seg->flags |= FLAGS_FREE_NAME;
                }
                else if ( type == 0x5D )
                {
					if ( bb5_split_keydata )
					{
						current_seg->name = strdup ( expected_name );
						current_seg->flags |= FLAGS_FREE_NAME;
					}
					else
					{
						current_seg->name = "KEYDATA";
					}
					current_seg->start = 0;
                }
                else if ( type == 0x20 )
                {
                    current_seg->name = "BT";
                    current_seg->start = 0;
                }
                else
                {
                    current_seg->name = "UNK";
                    current_seg->start = 0;
                }
				temp_length = malloc_step;
                current_seg->data = malloc ( temp_length );
                current_seg->length = 0;
            }
        }
		if ( existing_seg || current_seg->length + length > temp_length  )
		{
			temp_length = current_seg->length + length + malloc_step;
			current_seg->data = realloc ( current_seg->data, temp_length );
			if ( !current_seg->data )
				return E_FAIL;
		}
        memcpy ( current_seg->data + current_seg->length, GET_PTR ( source, pos ), length );
        current_seg->length += length;
        pos += length;

        last_end = start + length;
        last_type = type;
    }
    if ( pos != source->length )
        return E_FAIL;

    if ( type == 0x14 )
        current_seg->end = current_seg->start + current_seg->length;
    else if ( type == 0x21 )
        current_seg->end = current_seg->start + current_seg->length;
    else if ( type == 0x2E )
        current_seg->end = current_seg->start + current_seg->length;
    else if ( type == 0x54 )
        current_seg->end = current_seg->start + current_seg->length;
    return E_OK;
}


unsigned int
fmt_phoenix_parse_tokens ( t_stage * source, t_stage * target, unsigned int offset, unsigned int length )
{
    unsigned int pos = offset;
    unsigned char token = 0;
    unsigned char t_bytes = 0;
    unsigned char tokens = 0;
	unsigned char double_length = 0;

    tokens = GET_WORD ( source, pos );  // number ob tokens in header
    pos += 4;
    while ( pos + 1 < length + offset ) // enough space for token and length byte?
    {
        token = GET_BYTE ( source, pos );
        pos++;
        t_bytes = GET_BYTE ( source, pos );
        pos++;
		/*
			Workaround for a strange thing found in TIKU PPM flash 'HL100V0400.lba'

			E1 00 23 A700200000000003000000F401FE01020000000000AB0100005A00AB01490001006400
		*/
		double_length = 0;

		if ( !t_bytes )
		{
	        t_bytes = GET_BYTE ( source, pos );
	        pos++;
			double_length = 1;
		}

        if ( t_bytes + pos > length + offset )  // enough space for data?
            return E_FAIL;
        R ( fmt_phoenix_add_token ( target, token, t_bytes, GET_PTR ( source, pos ), double_length ) );
        pos += t_bytes;

        if ( !tokens-- )        // if header said, no more tokens but we still have space
            return E_FAIL;
    }
    if ( tokens )
        return E_FAIL;          // getting beyond header size, but still tokens left?!

    return E_OK;
}

unsigned int
fmt_phoenix_decode ( t_stage * source, t_stage * target )
{
    t_phoenix_priv *priv = NULL;

	DBG ( DEBUG_FMT, " => %s ( ) called\n", __FUNCTION__ );

    if ( !target )
        target = source->next;

    if ( !source || !target || !source->segments || !source->segments->data )
    {
	    DBG ( DEBUG_FMT, " -> %s ( ) failed !!\n", __FUNCTION__ );
        return E_FAIL;
    }

    if ( source->length < 5 )
    {
	    DBG ( DEBUG_FMT, " -> %s ( %s, %s ) failed !!\n", __FUNCTION__, source->name, target->name );
        return E_FAIL;
    }
    if ( GET_BYTE ( source, 0 ) != 0xA0 && GET_BYTE ( source, 0 ) != 0xA1 && GET_BYTE ( source, 0 ) != 0xA2 && GET_BYTE ( source, 0 ) != 0xB2 )
    {
	    DBG ( DEBUG_FMT, " -> %s ( %s, %s ) failed !!\n", __FUNCTION__, source->name, target->name );
        return E_FAIL;
    }
    if ( GET_WORD ( source, 1 ) + 5 > source->length )
    {
	    DBG ( DEBUG_FMT, " -> %s ( %s, %s ) failed !!\n", __FUNCTION__, source->name, target->name );
        return E_FAIL;
    }

    fmt_phoenix_init_stage ( target, GET_BYTE ( source, 0 ) );

	if ( fmt_phoenix_parse_tokens ( source, target, 5, GET_WORD ( source, 1 ) ) != E_OK || fmt_phoenix_parse_data ( source, target, GET_WORD ( source, 1 ) + 5 ) != E_OK )
    {
        fmt_phoenix_free ( target );
		DBG ( DEBUG_FMT, " -> %s ( %s, %s ) failed !!\n", __FUNCTION__, source->name, target->name );
        return E_FAIL;
    }

    target->name = "PARSED";
    target->length = 0;
    target->parser = "PHOENIX";
    target->type = "PHOENIX";

	// TEST!!
    //target->flags = S_MODIFIED;

	DBG ( DEBUG_FMT, " ## %s ( %s, %s ) done\n", __FUNCTION__, source->name, target->name );
    return E_OK;
}

char *
fmt_phoenix_create_tokens ( t_phoenix_tokens * t, unsigned int * ret_length, t_phoenix_erase * erase, unsigned int options )
{
	char *data = NULL;
	unsigned int length = 4;
	unsigned int tokens = 0;

	data = malloc ( length );
	if ( !data )
		return NULL;

	while ( t )
	{
		/*
			if we got the erase addresses "manually", check
			for the erase token and fill in these addresses
		*/
		if ( erase && (t->token_id == TOKEN_ERASE) )
		{
			data = realloc ( data, length + (LIST_COUNT(erase) * 8) + 2);
			if ( !data )
				return NULL;
			data[length] = TOKEN_ERASE;
			data[length+1] = LIST_COUNT(erase) * 8;
			length++;                  // the token itself
			length++;                  // the token length

			while ( erase )
			{
				data[length++] = (erase->start >> 24) & 0xFF;
				data[length++] = (erase->start >> 16) & 0xFF;
				data[length++] = (erase->start >> 8) & 0xFF;
				data[length++] = (erase->start >> 0) & 0xFF;
				data[length++] = (erase->end >> 24) & 0xFF;
				data[length++] = (erase->end >> 16) & 0xFF;
				data[length++] = (erase->end >> 8) & 0xFF;
				data[length++] = (erase->end >> 0) & 0xFF;
				LIST_NEXT(erase);
			}
			tokens++;
		}
		else if ( skip_claudia && t->token_id == TOKEN_CLAUDIA )
		{
			// just skip claudia :)
		}
		else
		{
			data = realloc ( data, length + t->length + 3 );
			if ( !data )
				return NULL;
			data[length++] = t->token_id;

			// fix for the strange TIKU token 0xE1
			if ( t->double_length )
				data[length++] = 0x00;

			data[length++] = t->length;
			memcpy ( &data[length], t->data, t->length );

			length += t->length;       // the data length
			tokens++;
		}
		t = t->next;
	}

	data[0] = (tokens>>24) & 0xFF;
	data[1] = (tokens>>16) & 0xFF;
	data[2] = (tokens>>8) & 0xFF;
	data[3] = (tokens) & 0xFF;

	*ret_length = length;
	return data;
}

unsigned int
fmt_phoenix_add_flashblock_2E ( unsigned char *data, unsigned int addr, unsigned char *in_data, unsigned int blocklen, unsigned int fix_checksum )
{
	if ( !data || !in_data )
		return 0;

	if ( addr & 0x000001FF )
	{
		ERR ( 1, "wanted to flash at unaligned address 0x%08X. lowest 9 bits need to be 0.", addr );
		return 0;
	}

	addr >>= 9;

	data[0] = 0x2E;
	data[1] = 0x01;
	data[2] = (addr>>16) & 0xFF;
	data[3] = (addr>>8) & 0xFF;
	data[4] = addr & 0xFF;
	data[5] = fmt_phoenix_checksum ( in_data, blocklen ) + fix_checksum;
	data[6] = (blocklen>>16) & 0xFF;
	data[7] = (blocklen>>8) & 0xFF;
	data[8] = blocklen & 0xFF;
	data[9] = fmt_phoenix_checksum ( &data[1], 8 );
	memcpy ( &data[10], in_data, blocklen );

	return blocklen + 10;
}


unsigned int
fmt_phoenix_add_flashblock_14 ( unsigned char *data, unsigned int addr, unsigned char *in_data, unsigned int blocklen, unsigned int fix_checksum )
{
	if ( !data || !in_data )
		return 0;

	data[0] = 0x14;
	data[1] = (addr>>24) & 0xFF;
	data[2] = (addr>>16) & 0xFF;
	data[3] = (addr>>8) & 0xFF;
	data[4] = addr & 0xFF;
	data[5] = fmt_phoenix_checksum ( in_data, blocklen ) + fix_checksum;
	data[6] = (blocklen>>16) & 0xFF;
	data[7] = (blocklen>>8) & 0xFF;
	data[8] = blocklen & 0xFF;
	data[9] = fmt_phoenix_checksum ( &data[1], 8 );
	memcpy ( &data[10], in_data, blocklen );

	return blocklen + 10;
}

char *
fmt_phoenix_create_blocks ( t_segment *s, unsigned int *ret_length )
{
	char *data = NULL;
	unsigned int length = 0;
	unsigned int blocklen = 0;
	unsigned int hdr_len = 0;
	unsigned int ofs = 0;
	unsigned int pos = 0;
    t_phoenix_segpriv *priv = NULL;

	data = malloc ( 1 );
	if ( !data )
		return NULL;

	while ( s )
	{
		priv = (t_phoenix_segpriv*)s->priv;
		if ( !priv && add_flashareas )
		{
			fmt_phoenix_init_segment ( s, use_flashtype, blocksize, 0 );
			priv = (t_phoenix_segpriv*)s->priv;
		}

		if ( !priv || !priv->blocksize )
		{
			free ( data );
			return NULL;
		}

		switch ( priv->type )
		{
			case 0x14:
				hdr_len = 10;
				break;
			case 0x2E:
				hdr_len = 10;
				break;
			case 0x20:
				hdr_len = 5;
				break;
			case 0x21:
				hdr_len = 4;
				break;
			default:
				free ( data );
				return NULL;
		}
		blocklen = priv->blocksize;
		// should be correct... ...better more than less bytes reserved
		length += ((s->length / blocklen) + 1) * (blocklen + hdr_len);
		data = realloc ( data, length );
		
		pos = 0;
		while ( pos < s->length )
		{

			//
			// some special block sizes patch...
			// ... just increase blocksize, else we 
			// could run out of memory because of too much headers
			//
			if ( blocklen & 0x3FFF )
			{
				if ( ((s->start+pos) & 0xFFFF ) == 0x2000 )
					blocklen = 0x2000;
				else if ( ((s->start+pos) & 0xFFFF ) == 0x4000 )
					blocklen = 0x4000;
				else if ( ((s->start+pos) & 0xFFFF ) == 0x8000 )
					blocklen = 0x8000;
			}
			if ( pos + blocklen > s->length )
				blocklen = s->length - pos;

			switch ( priv->type )
			{
				case 0x14:
					ofs += fmt_phoenix_add_flashblock_14 ( &data[ofs], s->start+pos, &s->data[pos], blocklen, priv->fix_checksum );
					pos += blocklen;

					break;

				case 0x2E:
					ofs += fmt_phoenix_add_flashblock_2E ( &data[ofs], s->start+pos, &s->data[pos], blocklen, priv->fix_checksum );
					pos += blocklen;

					break;

				case 0x20:   // TODO: code
				case 0x21:
				default:
					free ( data );
					return NULL;
			}
		}
		s = s->next;
	}

	data = realloc ( data, ofs );
	*ret_length = ofs;
	return data;
}


char *
fmt_phoenix_create ( t_stage * source, unsigned int *ret_length )
{
	unsigned int length = 5;
	char *data = NULL;
	char *tokens = NULL;
	char *blocks = NULL;
	unsigned int tokens_length = 0;
	unsigned int blocks_length = 0;
    t_phoenix_priv *priv = NULL;

    if ( !source || !length || !source->priv )
        return NULL;

    priv = ( t_phoenix_priv * ) source->priv;

	data = malloc ( length );
	if ( !data )
		return NULL;
	data[0] = priv->type;

	tokens = fmt_phoenix_create_tokens ( priv->tokens, &tokens_length, NULL, 0 );
	if ( !tokens )
	{
		free ( data );
		return NULL;
	}
	data[1] = (tokens_length>>24) & 0xFF;
	data[2] = (tokens_length>>16) & 0xFF;
	data[3] = (tokens_length>>8) & 0xFF;
	data[4] = (tokens_length) & 0xFF;

	blocks = fmt_phoenix_create_blocks ( source->segments, &blocks_length );
	if ( !blocks )
	{
		free ( data );
		return NULL;
	}

	length += tokens_length;
	length += blocks_length;
	data = realloc ( data, length +100 );
	if ( !data )
		return NULL;

	memcpy ( &data[5], tokens, tokens_length );
	memcpy ( &data[5+tokens_length], blocks, blocks_length );

	free ( tokens );
	free ( blocks );
	*ret_length = length;
	return data;
}

unsigned int
fmt_phoenix_encode ( t_stage * source, t_stage * target )
{
	char *data = NULL;
	unsigned int length = 0;
	t_stage *t = NULL;

	
	DBG ( DEBUG_FMT, " => %s ( ) called\n", __FUNCTION__ );

	t = stage_create ();

    if ( !source || !target )
    {
		
	    DBG ( DEBUG_FMT, " -> %s ( ) failed !!\n", __FUNCTION__ );
        return E_FAIL;
    }

    if ( segment_count ( source->segments ) < 1 )
    {
		
	    DBG ( DEBUG_FMT, " -> %s ( %s, %s ) failed !!\n", __FUNCTION__, source->name, t->name );
        return E_FAIL;
    }

	data = fmt_phoenix_create ( source, &length );

	if ( !data )
    {
		
	    DBG ( DEBUG_FMT, " -> %s ( %s, %s ) failed !!\n", __FUNCTION__, source->name, t->name );
        return E_FAIL;
    }

    t->segments = segment_create (  );
    t->segments->name = "DATA";
    t->segments->start = 0;
    t->segments->end = length;
    t->segments->length = length;
    t->segments->data = data;

	t->name = target->name;
    t->length = length;
	t->parser = target->parser;
	t->type = target->type;

	stage_replace ( target, t );


	
	DBG ( DEBUG_FMT, " ## %s ( %s, %s ) done\n", __FUNCTION__, source->name, target->name );
    return E_OK;

}

unsigned int
fmt_phoenix_get_erased_bytes ( t_phoenix_erase * erase, unsigned int addr )
{
	while ( erase )
	{
		if ( addr >= erase->start && addr <= erase->end )
			return (erase->end - addr) + 1;
		LIST_NEXT(erase);
	}
	return 0;
}

unsigned int
fmt_phoenix_is_erased ( t_phoenix_erase * erase, unsigned int addr )
{
	while ( erase )
	{
		if ( addr >= erase->start && addr <= erase->end )
			return 1;
		LIST_NEXT(erase);
	}
	return 0;
}

char *
fmt_phoenix_create_blocks_diff ( t_segment *s, unsigned int *ret_length, t_phoenix_erase * erase )
{
	char *data = NULL;
	unsigned int length = 0;
	unsigned int blocklen = 0;
	unsigned int hdr_len = 0;
	unsigned int ofs = 0;
	unsigned int pos = 0;
    t_phoenix_segpriv *priv = NULL;

	data = malloc ( 1 );
	if ( !data )
		return NULL;

	while ( s )
	{
		priv = (t_phoenix_segpriv*)s->priv;
		if ( !priv || !priv->blocksize )
		{
			free ( data );
			return NULL;
		}

		switch ( priv->type )
		{
			case 0x14:
				hdr_len = 10;
				break;
			case 0x2E:
				hdr_len = 10;
				break;
			case 0x20:
				hdr_len = 5;
				break;
			case 0x21:
				hdr_len = 4;
				break;
			default:
				free ( data );
				return NULL;
		}

		blocklen = 0x1000;
		// should be correct... ...better more than less bytes reserved
		length += ((s->length / blocklen) + 1) * (blocklen + hdr_len);
		data = realloc ( data, length );
		if ( !data )
			return NULL;
		
		pos = 0;
		while ( pos < s->length )
		{
			blocklen = 0x1000;
			if ( pos + blocklen > s->length )
				blocklen = s->length - pos;

			switch ( priv->type )
			{
				case 0x14:
					if ( fmt_phoenix_is_erased ( erase, s->start + pos ) )
					{
						if ( fmt_phoenix_get_erased_bytes ( erase, s->start + pos ) < blocklen )
							blocklen = fmt_phoenix_get_erased_bytes ( erase, s->start + pos );

						ofs += fmt_phoenix_add_flashblock_14 ( &data[ofs], s->start+pos, &s->data[pos], blocklen, priv->fix_checksum );
						pos += blocklen;
					}
					else 
						pos++;
					break;

				case 0x2E:
					if ( fmt_phoenix_is_erased ( erase, s->start + pos ) )
					{
						if ( fmt_phoenix_get_erased_bytes ( erase, s->start + pos ) < blocklen )
							blocklen = fmt_phoenix_get_erased_bytes ( erase, s->start + pos );

						ofs += fmt_phoenix_add_flashblock_2E ( &data[ofs], s->start+pos, &s->data[pos], blocklen, priv->fix_checksum );
						pos += blocklen;
					}
					else 
						pos++;
					break;

				case 0x20:   // TODO: code
				case 0x21:
				default:
					free ( data );
					return NULL;
			}
		}
		s = s->next;
	}

	data = realloc ( data, ofs );
	*ret_length = ofs;
	return data;
}


t_phoenix_erase *
fmt_phoenix_get_erase ( t_stage * source, t_stage * modified )
{
	unsigned int pos = 0;
	unsigned int start = 0;
	unsigned int end = 0;
	unsigned int length = 0;
	t_phoenix_erase *erase_addr = NULL;
	t_phoenix_erase *ret = NULL;
	t_segment *s = NULL;
	t_segment *d = NULL;

	s = source->segments;
	d = modified->segments;

	while ( s && d )
	{
		pos = 0;
		while ( pos < s->length )
		{
			start = (s->start + pos) & ~(blocksize-1);
			end = start + blocksize-1;

			length = blocksize;

			// if data starts not blockaligned, decrease length to stop at block end
			if ( !pos && (s->start & (blocksize-1)) )
				length = blocksize - (s->start & (blocksize-1));

			// if length of check is too long, cut length
			if ( pos + blocksize > s->length )
				length = s->length - pos;

			if ( memcmp ( &(s->data[pos]), &(d->data[pos]), length) )
			{
				printf ( "  difference in block 0x%08X - 0x%08X\n", start, end );
				erase_addr = LIST_NEW(erase_addr, t_phoenix_erase);
				erase_addr->start = start;
				erase_addr->end = end;
				if ( !ret )
					ret = erase_addr;

			}
			pos += length;
		}
		s = s->next;
		d = d->next;
	}

	return ret;
}

char *
fmt_phoenix_create_diff ( t_stage * source, t_stage * modified, unsigned int *ret_length )
{
	unsigned int length = 5;
	char *data = NULL;
	char *tokens = NULL;
	char *blocks = NULL;
	unsigned int tokens_length = 0;
	unsigned int blocks_length = 0;
    t_phoenix_priv *priv = NULL;
	t_phoenix_erase *erase_addr = NULL;

    if ( !source || !modified || !length || !source->priv )
        return NULL;

	if ( LIST_COUNT(source) != LIST_COUNT(modified) )
		return NULL;

    priv = ( t_phoenix_priv * ) source->priv;

	data = malloc ( length );
	if ( !data )
		return NULL;

	data[0] = priv->type;

	erase_addr = fmt_phoenix_get_erase ( source, modified );
	if ( !erase_addr )
	{
		printf ( "%s: no differences found\n", __FUNCTION__ );
		return fmt_phoenix_create ( source, ret_length );
	}

	tokens = fmt_phoenix_create_tokens ( priv->tokens, &tokens_length, erase_addr, 0 );
	if ( !tokens )
	{
		free ( data );
		return NULL;
	}
	data[1] = (tokens_length>>24) & 0xFF;
	data[2] = (tokens_length>>16) & 0xFF;
	data[3] = (tokens_length>>8) & 0xFF;
	data[4] = (tokens_length) & 0xFF;

	blocks = fmt_phoenix_create_blocks_diff ( source->segments, &blocks_length, erase_addr );
	if ( !blocks )
	{
		free ( data );
		return NULL;
	}

	length += tokens_length;
	length += blocks_length;
	data = realloc ( data, length +100 );
	if ( !data )
		return NULL;

	memcpy ( &data[5], tokens, tokens_length );
	memcpy ( &data[5+tokens_length], blocks, blocks_length );

	free ( tokens );
	free ( blocks );
	*ret_length = length;
	return data;
}
unsigned int
fmt_phoenix_encode_diff ( t_stage * source, t_stage * modified, t_stage * target )
{
	char *data = NULL;
	unsigned int length = 0;
	t_stage *t = NULL;

	
	DBG ( DEBUG_FMT, " => %s ( ) called\n", __FUNCTION__ );

	t = stage_create ();

    if ( !source || !modified || !target )
    {
		
	    DBG ( DEBUG_FMT, " -> %s ( ) failed !!\n", __FUNCTION__ );
        return E_FAIL;
    }

	if ( segment_count ( source->segments ) < 1 && segment_count ( source->segments ) != segment_count ( modified->segments ) )
    {
		
	    DBG ( DEBUG_FMT, " -> %s ( %s, %s ) failed !!\n", __FUNCTION__, source->name, t->name );
        return E_FAIL;
    }

	data = fmt_phoenix_create_diff ( source, modified, &length );

	if ( !data )
    {
		
	    DBG ( DEBUG_FMT, " -> %s ( %s, %s ) failed !!\n", __FUNCTION__, source->name, t->name );
        return E_FAIL;
    }

    t->segments = segment_create (  );
    t->segments->name = "DATA";
    t->segments->start = 0;
    t->segments->end = length;
    t->segments->length = length;
    t->segments->data = data;

	t->name = target->name;
    t->length = length;
	t->parser = target->parser;
	t->type = target->type;

	stage_replace ( target, t );
	
	DBG ( DEBUG_FMT, " ## %s ( %s, %s ) done\n", __FUNCTION__, source->name, target->name );
    return E_OK;

}

t_fmt_handler phoenix_handler = {
    "PHOENIX",
    "FORMAT",
    "fmt_phoenix_decode",
    fmt_phoenix_decode,
    "fmt_phoenix_encode",
    fmt_phoenix_encode,
    "fmt_phoenix_free",
    fmt_phoenix_free,
    "fmt_phoenix_encode_diff",
    fmt_phoenix_encode_diff
};


unsigned int
fmt_phoenix_init (  )
{
    fmt_register_handler ( &phoenix_handler );
	options_add_core_option ( OPT_BOOL, "fmt.phoenix", skip_claudia, "Enable to skip CLAUDIA sequence, needed for some flashers (especially when using partial flashes)" );
	options_add_core_option ( OPT_HEX, "fmt.phoenix", blocksize, "Partial flash blocksize, maybe needed for some flash devices." );
	options_add_core_option ( OPT_BOOL, "fmt.phoenix", add_flashareas, "Allow to create new flash areas." );
	options_add_core_option ( OPT_BOOL, "fmt.phoenix", bb5_split_keydata, "Split Key data into seperate segments" );
	options_add_core_option ( OPT_HEX, "fmt.phoenix", use_flashtype, "Which Flashtype to use for new created flash areas. (0x14 for flash, 0x2E for TIKU's MM disk)" );

	return E_OK;
}

#endif
