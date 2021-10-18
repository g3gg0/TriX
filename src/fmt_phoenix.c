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

#define TOKEN_PLATFORM		0xC2	// BB5 DCT4...
#define TOKEN_ALGORITHM		0xC3	// DCT4 ALGORITHM, BB5 ALGORITHM
#define TOKEN_ERASE			0xC8
#define TOKEN_CLAUDIA		0xD3
#define TOKEN_LAST			0xF7	// only in bb5, to be checked

#define TOKEN_E1				0xE1  // TIKU token 0xE1 ( halfword length )
#define TOKEN_EX				0xF3	// tokens in token...
											// F3 43C5 B0 00000B40 00000008 ...

int flash_block_align = 1;
int flash_block_align_size = 0x1000;
int skip_claudia = 0;
int blocksize = 0x10000;
int use_flashtype = 0x14;
int add_flashareas = 1;
int bb5_map_flashid = 0;
int flash_first_erase_area = 0;



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
	unsigned short length;      // e.g. 0x40
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
	unsigned int type;         // type of flash block e.g. 0x14
	unsigned int subtype;         // type of flash block e.g. 0x14
	unsigned int flash_dev[2];
	unsigned int blocksize;    // blocksize used in original flash
	unsigned int fix_checksum;
	unsigned char block_header[128];
	unsigned int block_header_length;
};



unsigned short
fmt_phoenix_checksum_xor ( unsigned char *data, int length )
{
	unsigned short chk = 0;
	unsigned int pos = 0;

	while ( pos < length )
	{
		chk ^= (data[pos]<<8) | data[pos+1];
		pos += 2;
	}
	return chk;
}

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
fmt_phoenix_init_segment ( t_segment * s, unsigned int type, unsigned int subtype, unsigned int blocksize, unsigned int *flash_dev, int fix_checksum )
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
	priv->subtype = subtype;
	priv->blocksize = blocksize;
	priv->flash_dev[0] = flash_dev[0];
	priv->flash_dev[1] = flash_dev[1];
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
fmt_phoenix_add_token ( t_stage * s, unsigned char token_id, unsigned short length, unsigned char *data, unsigned char double_length )
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
	t_phoenix_segpriv *seg_priv = NULL;

	unsigned int tmp_pos = 0;
	unsigned int pos = offset;
	unsigned int last_end = 0;
	unsigned int type = 0;
	unsigned int subtype = 0;
	unsigned int last_type = 0;
	unsigned int last_subtype = 0;
	unsigned int done = 0;
	unsigned int store_header = 0;
	unsigned int mapped = 0;
	unsigned int last_mapped = 0;

	unsigned int length = 0;
	unsigned int start = 0;
	unsigned int flash_dev[2];
	unsigned int header_length = 0;
	unsigned short d_chk = 0;
	unsigned short h_chk = 0;
	unsigned short d_chk_calced = 0;
	unsigned short h_chk_calced = 0;
	unsigned int fix_checksum = 0;
	unsigned int temp_length = 0;
	unsigned int malloc_step = 1000000;

	unsigned char last_name[256];
	unsigned char expected_name[256];
	unsigned char part_name[13];
	unsigned char tmp_buf[32];

   flash_dev[0] = 0;
   flash_dev[1] = 0;
	strcpy ( last_name, "" );
	strcpy ( expected_name, "" );

	while ( pos < source->length )
	{
		type = GET_BYTE ( source, pos );
		subtype = 0; // will get read in BB5 handlers
		header_length = 0;
		fix_checksum = 0;
		mapped = 0;


		switch ( ((t_phoenix_priv*)target->priv)->type )
		{
			case 0xA0:  // u_2nd.fia
			case 0xA1:  // u_amd.fia
				header_length = 0;
				mapped = 1;
				start = 0;
				length = source->length - pos;
				type = 0xFFFF;
				sprintf ( expected_name, "Loader" );
				break;

				// DCT4 flash file
			case 0xA2:  
				switch ( type )
				{
					case 0x14:
						// default DCT4 flash blocks
						header_length = 10;
						sprintf ( expected_name, "FLASH (0x14)" );

						start = GET_WORD ( source, pos + 1 );
						d_chk = GET_BYTE ( source, pos + 5 );
						length = GET_TRIP ( source, pos + 6 );
						h_chk = GET_BYTE ( source, pos + 9 );

						h_chk_calced = fmt_phoenix_checksum ( GET_PTR ( source, pos + 1 ), header_length - 2 );
						d_chk_calced = fmt_phoenix_checksum ( GET_PTR ( source, pos + header_length ), length );
						// dirty checksum fix check - but that was just needed for 8310 afaik
						fix_checksum = ((unsigned char)(d_chk - d_chk_calced));

						if ( d_chk_calced != d_chk && fix_checksum != 0 && fix_checksum != 1 )
							printf ( "DCT4 DATA CHECKSUM ERROR! File offset: 0x%08X, Block type: 0x%02X, d_chk: 0x%04X, d_chk_calced: 0x%04X\n", pos, type, d_chk, d_chk_calced );

						if ( h_chk_calced != h_chk )
							printf ( "DCT4 HEADER CHECKSUM ERROR! File offset: 0x%08X, Block type: 0x%02X, h_chk: 0x%04X, h_chk_calced: 0x%04X\n", pos, type, h_chk, h_chk_calced );

						mapped = 1;
						break;

					case 0x20:
						// bluetooth firmware
						header_length = 6;
						sprintf ( expected_name, "BT" );

						start = 0;
						length = GET_HALF ( source, pos + 3 );

						mapped = 0;
						break;

					case 0x21:
						header_length = 16;
						sprintf ( expected_name, "FLASH (0x21)" );

						start = GET_WORD ( source, pos + 7 );
						d_chk = GET_BYTE ( source, pos + 11 );
						length = GET_TRIP ( source, pos + 12 );
						h_chk = GET_BYTE ( source, pos + 15 );

						/* TODO: make this working */
#if false
						h_chk_calced = fmt_phoenix_checksum ( GET_PTR ( source, pos + 1 ), header_length - 2 );
						d_chk_calced = fmt_phoenix_checksum ( GET_PTR ( source, pos + header_length ), length );

						if ( d_chk_calced != d_chk )
							printf ( "DCT4 DATA CHECKSUM ERROR! File offset: 0x%08X, Block type: 0x%02X, d_chk: 0x%04X, d_chk_calced: 0x%04X\n", pos, type, d_chk, d_chk_calced );

						if ( h_chk_calced != h_chk )
							printf ( "DCT4 HEADER CHECKSUM ERROR! File offset: 0x%08X, Block type: 0x%02X, h_chk: 0x%04X, h_chk_calced: 0x%04X\n", pos, type, h_chk, h_chk_calced );
#endif
						mapped = 1;
						break;

					case 0x25:
						// have to handle this one. whats that?
						//  NEM-4: 25 03 32 32 98 
						header_length = 0;
						sprintf ( expected_name, "RAW (0x%02X)", type );

						start = 0;
						length = 2 + GET_BYTE ( source, pos + 1 );

						mapped = 0;
						break;

					case 0x26:
						// have to handle this one. whats that?
						//  NEM-4: 26 00 02 DD 94 63 55 7D 01 56
						header_length = 0;
						sprintf ( expected_name, "RAW (0x%02X)", type );

						start = 0;
						length = 10;

						mapped = 0;
						break;

					case 0x2E:
						header_length = 10;

						// mass memory flash?
						flash_dev[0] = GET_HALF ( source, pos + 1 );
						flash_dev[1] = flash_dev[0];
						start = GET_HALF ( source, pos + 3 ) << 9;
						d_chk = GET_BYTE ( source, pos + 5 );
						length = GET_TRIP ( source, pos + 6 );
						h_chk = GET_BYTE ( source, pos + 9 );

						sprintf ( expected_name, "Flash, Dev: 0x%08X", flash_dev[0] );

						/* TODO: make this working */
#if false
						h_chk_calced = fmt_phoenix_checksum ( GET_PTR ( source, pos + 1 ), header_length - 2 );
						d_chk_calced = fmt_phoenix_checksum ( GET_PTR ( source, pos + header_length ), length );

						if ( d_chk_calced != d_chk )
							printf ( "DCT4 DATA CHECKSUM ERROR! File offset: 0x%08X, Block type: 0x%02X, d_chk: 0x%04X, d_chk_calced: 0x%04X\n", pos, type, d_chk, d_chk_calced );

						if ( h_chk_calced != h_chk )
							printf ( "DCT4 HEADER CHECKSUM ERROR! File offset: 0x%08X, Block type: 0x%02X, h_chk: 0x%04X, h_chk_calced: 0x%04X\n", pos, type, h_chk, h_chk_calced );
#endif

						mapped = 1;
						break;

					default:
						return E_FAIL;
					}
				break;

				// BB5 flash file
			case 0xB2:

				subtype = GET_HALF ( source, pos + 1 );
				header_length = 4 + GET_BYTE ( source, pos + 3 ) + 1;

				switch ( type )
				{
					case 0x54:
						// 54 01 17 0E 00 00 00 27 06 01 00 00 01 C0 00 00 04 00 E6 <- RM36 MCU

						// 54 01 17 0E 01 01 00 50 4C 01 00 00 20 00 01 06 00 00 13 <- RM36 PPM
						// 54 01 17 0E 01 01 00 4F 1F 01 00 00 20 00 01 06 20 00 21

						// 54 01 17 0E 00 03 00 A3 F6 01 00 02 00 00 01 F2 00 00 47 <- RM470 PPM, different flash_dev bytes

						// HH ST ST HL FF FF FF DC DC ?? LL LL LL LL SS SS SS SS HC

						// HH - Header
						// ST - SubType
						// HL - Header Length
						// FF - Flash ID
						// DC - Data Checksum
						// LL - Length
						// SS - Start
						// HC - Header Checksum

						// rm540/rm509 CNT
						// 54 01 30 15 00 03 00 61 81 00 00 00 08 FF FF FF FF 00 04 00 00 00 00 00 00 CC
						// HH ST ST HL FF FF FF DC DC ?? ?? ?? ?? ?? ?? ?? ?? LL LL LL LL SS SS SS SS HC

                  //flash_dev = GET_BYTE ( source, pos + 0x24 );
						//if ( flash_dev != GET_BYTE ( source, pos + 5 ) )
						//   printf ( "Phoenix BB5: Flash ID inconsistency at 0x%08X in file\r\n", pos );

						flash_dev[0] = GET_BYTE ( source, pos + 4 );
						flash_dev[1] = GET_BYTE ( source, pos + 5 );

						start = GET_WORD ( source, pos + header_length - 5 );
						d_chk = GET_HALF ( source, pos + 7 );
						length = GET_WORD ( source, pos + header_length - 9 );
						h_chk = GET_BYTE ( source, pos + header_length - 1 );

						sprintf ( expected_name, "Flash, Dev: %d", flash_dev[0] );

						if ( (fmt_phoenix_checksum ( GET_PTR ( source, pos + 1 ), header_length - 2 ) & 0xFF) != h_chk )
							printf ( "CHECKSUM ERROR\n" );

						store_header = 1;

						if ( flash_dev[0] == bb5_map_flashid )
							mapped = 1;
						else
							mapped = 0;
						break;

					case 0x5D:
						//
						// 5D 01 27 2D ............... .............. 00 00 00 23 0C 00 00 04 00 00 00 06 C0 AE
						// 5D 01 27 2D ............... .............. 00 00 00 8D B6 00 00 04 00 00 00 06 C0 D6
						// 5D 01 27 2D ............... .............. 01 01 00 C1 26 00 00 04 00 00 07 00 00 48
						//
						// HH ST ST HL [20 byte SHA1?] [12 byte name] FF FF FF DC DC LL LL LL LL SS SS SS SS HC
						//

						//
						// 5D 01 28 43 ............... .............. 00 00 00 FD 05 00 00 03 00 FF FF FF FF ................ 02 00 13
						// 5D 01 28 43 ............... .............. 00 00 00 FD 05 00 00 03 00 FF FF FF FF ................ 02 00 13
						// 5D 01 28 43 ............... .............. 00 00 00 5A 5A 00 00 03 00 FF FF FF FF ................ 02 00 9D
						// 5D 01 28 43 ............... .............. 01 01 00 BF 2B 00 00 04 00 FF FF FF FF ................ 02 00 48
						// 5D 01 28 43 ............... .............. 00 00 00 8C 2E 00 00 02 80 FF FF FF FF ................ 01 00 25
						//
						// HH ST ST HL [20 byte SHA1?] [12 byte name] FF FF FF DC DC LL LL LL LL SS SS SS SS [20 bytes SHA1?] ?? ?? HC
						//

						memset ( part_name, 0x00, 13 );
						strncpy ( part_name, GET_PTR ( source, pos + 0x18 ), 10 );

						//flash_dev = GET_BYTE ( source, pos + 0x24 );
						//if ( flash_dev != GET_BYTE ( source, pos + 0x25 ) )
						//	printf ( "Phoenix BB5: Flash ID inconsistency at 0x%08X in file\r\n", pos );

						flash_dev[0] = GET_BYTE ( source, pos + 0x24 );
						flash_dev[1] = GET_BYTE ( source, pos + 0x25 );

						d_chk = GET_HALF ( source, pos + 0x27 );
						length = GET_WORD ( source, pos + 0x29 );
						start = GET_WORD ( source, pos + 0x2D );
						h_chk = GET_BYTE ( source, pos + header_length - 1 );

						sprintf ( expected_name, "Flash: <%s>, Dev: %d, Hash: ", part_name, flash_dev[0] );
						for ( tmp_pos = 0; tmp_pos < 0x10; tmp_pos++ )
						{
							sprintf ( tmp_buf, "%02X", GET_BYTE ( source, pos + 4 + tmp_pos ));
							strcat ( expected_name, tmp_buf );
						}

						// RSAPUBKEYS have strange start address. for sure not written directly into flash. loader will handle this
						if ( subtype == 0x0128 )
						{
							start = 0;
							strcat ( expected_name, ", Hash: " );
							for ( tmp_pos = 0; tmp_pos < 0x10; tmp_pos++ )
							{
								sprintf ( tmp_buf, "%02X", GET_BYTE ( source, pos + 0x31 + tmp_pos ));
								strcat ( expected_name, tmp_buf );
							}
						}

						if ( (fmt_phoenix_checksum ( GET_PTR ( source, pos + 1 ) , header_length - 2 ) & 0xFF) != h_chk )
							printf ( "CHECKSUM ERROR\n" );

						// store header in priv structure - we need the SHA1 "root hashes"
						store_header = 1;

						if ( flash_dev[0] == bb5_map_flashid && subtype == 0x127 )
							mapped = 1;
						else
							mapped = 0;

						break;

					default:
						return E_FAIL;
				}
				break;

			default:
				return E_FAIL;
		}


		if ( pos + length > source->length )
			return E_FAIL;

		// finish work on the current segment, next data will go into a different segment
		if ( type != last_type || last_end != start || strcmp ( last_name, expected_name ) )
		{
			// update end addresses if this is mapped into memory
			if ( current_seg )
			{
				current_seg->end = current_seg->start + current_seg->length;

				// reallocate memory to its real size
				current_seg->data = realloc ( current_seg->data, current_seg->length );
			}

			existing_seg = NULL;

			// the first time we have to add first segment
			if ( !last_type )
			{
				target->segments = segment_create (  );
				current_seg = target->segments;
			}
			else  // else just append one
			{
				if ( start )
					existing_seg = segment_find_by_end_and_name ( target->segments, start, expected_name, (mapped ? FLAGS_MAP_IN_MEM:FLAGS_HIDE_IN_MEM) );
				else
					existing_seg = segment_find_by_name ( target->segments, expected_name, (mapped ? FLAGS_MAP_IN_MEM:FLAGS_HIDE_IN_MEM) );

				// segment found and is same type
				if ( existing_seg && existing_seg->priv && ((t_phoenix_segpriv*) existing_seg->priv)->type == type )
					current_seg = existing_seg;
				else
					current_seg = segment_add ( current_seg );
			}

			if ( existing_seg )
			{
				// already had that segment, update priv pointer
				seg_priv = (t_phoenix_segpriv*)current_seg->priv;
			}
			else
			{
				// none found, setup the newly created segment
				fmt_phoenix_init_segment ( current_seg, type, subtype, length, flash_dev, fix_checksum );
				seg_priv = (t_phoenix_segpriv*)current_seg->priv;

				// BB5 flashes have SHA1 hashes in some blocks. save this SHA1 (same for all flashblocks of one segment)
				if ( store_header )
				{
					v_memcpy_get_raw ( source, seg_priv->block_header, pos, header_length );
					seg_priv->block_header_length = header_length;
					store_header = 0;
				}

				current_seg->start = start;
				current_seg->name = strdup ( expected_name );

				// update segment flags
				current_seg->flags |= FLAGS_FREE_NAME;
				if ( mapped )
					segment_map_in_mem ( current_seg );
				else
					segment_hide_in_mem ( current_seg );

				temp_length = malloc_step;
				current_seg->data = malloc ( temp_length );
				current_seg->length = 0;
			}
		}
		if ( !current_seg )
			return E_FAIL;

		// extend the segment
		if ( existing_seg || current_seg->length + length > temp_length ) 
		{
			temp_length = current_seg->length + length + malloc_step;
			current_seg->data = realloc ( current_seg->data, temp_length );
			if ( !current_seg->data )
				return E_FAIL;
		}

		// skip header
		pos += header_length;

		// copy block data to segment
		memcpy ( current_seg->data + current_seg->length, GET_PTR ( source, pos ), length );
		current_seg->length += length;

		// skip payload
		pos += length;

		// if this block was bigger then the current set blocksize, use this as blocksize
		if ( seg_priv->blocksize < length )
			seg_priv->blocksize = length;

		last_end = start + length;
		last_type = type;
		last_subtype = subtype;
		last_mapped = mapped;
		strcpy ( last_name, expected_name );
	}

	if ( pos != source->length )
		return E_FAIL;

	// update the last segment we worked with
	if(current_seg != NULL)
	{
		current_seg->end = current_seg->start + current_seg->length;
	}

	return E_OK;
}


unsigned int
fmt_phoenix_parse_tokens ( t_stage * source, t_stage * target, unsigned int offset, unsigned int length )
{
	unsigned int pos = offset;
	unsigned char token = 0;
	unsigned short t_bytes = 0;
	unsigned char tokens = 0;
	unsigned char double_length = 0;

	tokens = GET_WORD ( source, pos );  // number of tokens in header
	pos += 4;
	while ( pos + 1 < length + offset ) // enough space for token and length byte?
	{
		token = GET_BYTE ( source, pos );
		pos++;
		if ( token == TOKEN_EX )
		{
         t_bytes = GET_HALF ( source, pos );
			pos += 2;
         double_length = 1;
		}
		else
		{
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
				data[length++] = BYTEPOS(erase->start,3);
				data[length++] = BYTEPOS(erase->start,2);
				data[length++] = BYTEPOS(erase->start,1);
				data[length++] = BYTEPOS(erase->start,0);
				data[length++] = BYTEPOS(erase->end,3);
				data[length++] = BYTEPOS(erase->end,2);
				data[length++] = BYTEPOS(erase->end,1);
				data[length++] = BYTEPOS(erase->end,0);
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

			// fix for the strange TIKU token 0xE1 and BB5 token 0xF3
			if ( t->double_length )
				data[length++] = t->length >> 8;

			data[length++] = t->length & 0xFF;
			memcpy ( &data[length], t->data, t->length );

			length += t->length;       // the data length
			tokens++;
		}
		t = t->next;
	}

	data[0] = BYTEPOS(tokens,3);
	data[1] = BYTEPOS(tokens,2);
	data[2] = BYTEPOS(tokens,1);
	data[3] = BYTEPOS(tokens,0);

	*ret_length = length;
	return data;
}

unsigned int
fmt_phoenix_add_flashblock_5D ( unsigned char *data, unsigned int addr, unsigned char *in_data, unsigned int blocklen, unsigned int *flash_dev, unsigned int subtype, unsigned char *old_header, unsigned int old_header_length )
{
	unsigned int header_length = old_header_length;
	unsigned short d_chk = fmt_phoenix_checksum_xor ( in_data, blocklen );
	unsigned int chk_pos = 0;

	if ( !data || !in_data )
		return 0;

	if ( subtype == 0x0127 )
		chk_pos = 0x31;
	else if ( subtype == 0x0128 )
	{
		chk_pos = 0x47;
		addr = 0xFFFFFFFF;
	}
	else 
		return 0;

	memcpy ( data, old_header, old_header_length );

	// only update the fields that are necessary
	data[0x24] = flash_dev[0];
	data[0x25] = flash_dev[1];
	data[0x26] = 0;

	data[0x27] = BYTEPOS(d_chk,1);
	data[0x28] = BYTEPOS(d_chk,0);

	data[0x29] = BYTEPOS(blocklen,3);
	data[0x2A] = BYTEPOS(blocklen,2);
	data[0x2B] = BYTEPOS(blocklen,1);
	data[0x2C] = BYTEPOS(blocklen,0);

	data[0x2D] = BYTEPOS(addr,3);
	data[0x2E] = BYTEPOS(addr,2);
	data[0x2F] = BYTEPOS(addr,1);
	data[0x30] = BYTEPOS(addr,0);

	data[chk_pos] = fmt_phoenix_checksum ( &data[1], chk_pos - 1 );

	memcpy ( &data[header_length], in_data, blocklen );

	return blocklen + header_length;
}

unsigned int
fmt_phoenix_add_flashblock_54 ( unsigned char *data, unsigned int addr, unsigned char *in_data, unsigned int blocklen, unsigned int *flash_dev, unsigned int subtype, unsigned char *old_header, unsigned int old_header_length )
{
	unsigned int header_length = old_header_length;
	unsigned short d_chk = fmt_phoenix_checksum_xor ( in_data, blocklen );

	if ( !data || !in_data )
		return 0;

	memcpy ( data, old_header, old_header_length );

	data[0x04] = flash_dev[0];
	data[0x05] = flash_dev[1];
	data[0x06] = 0x00;

	data[0x07] = BYTEPOS(d_chk,1);
	data[0x08] = BYTEPOS(d_chk,0);

	data[header_length-9] = BYTEPOS(blocklen,3);
	data[header_length-8] = BYTEPOS(blocklen,2);
	data[header_length-7] = BYTEPOS(blocklen,1);
	data[header_length-6] = BYTEPOS(blocklen,0);

	data[header_length-5] = BYTEPOS(addr,3);
	data[header_length-4] = BYTEPOS(addr,2);
	data[header_length-3] = BYTEPOS(addr,1);
	data[header_length-2] = BYTEPOS(addr,0);

	data[header_length-1] = fmt_phoenix_checksum ( &data[1], header_length-2 );

	memcpy ( &data[header_length], in_data, blocklen );

	return blocklen + header_length;
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
	data[2] = BYTEPOS(addr,2);
	data[3] = BYTEPOS(addr,1);
	data[4] = BYTEPOS(addr,0);
	data[5] = fmt_phoenix_checksum ( in_data, blocklen ) + fix_checksum;
	data[6] = BYTEPOS(blocklen,2);
	data[7] = BYTEPOS(blocklen,1);
	data[8] = BYTEPOS(blocklen,0);
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
	data[1] = BYTEPOS(addr,3);
	data[2] = BYTEPOS(addr,2);
	data[3] = BYTEPOS(addr,1);
	data[4] = BYTEPOS(addr,0);
	data[5] = fmt_phoenix_checksum ( in_data, blocklen ) + fix_checksum;
	data[6] = BYTEPOS(blocklen,2);
	data[7] = BYTEPOS(blocklen,1);
	data[8] = BYTEPOS(blocklen,0);
	data[9] = fmt_phoenix_checksum ( &data[1], 8 );
	memcpy ( &data[10], in_data, blocklen );

	return blocklen + 10;
}

char *
fmt_phoenix_create_blocks ( t_stage *stage, unsigned int *ret_length )
{
	char *data = NULL;
	unsigned int length = 0;
	unsigned int blocklen = 0;
	unsigned int hdr_len = 0;
	unsigned int ofs = 0;
	unsigned int pos = 0;
	unsigned int flash_dev[2];
	t_phoenix_segpriv *priv = NULL;
	t_segment *s = NULL;

   flash_dev[0] = 0;
   flash_dev[1] = 0;
	data = malloc ( 1 );
	if ( !stage || !data )
		return NULL;

	s = stage->segments;

	while ( s )
	{
		priv = (t_phoenix_segpriv*)s->priv;
		if ( !priv && add_flashareas )
		{
			fmt_phoenix_init_segment ( s, use_flashtype, 0, blocksize, flash_dev, 0 );
			priv = (t_phoenix_segpriv*)s->priv;
		}

		if ( !priv || !priv->blocksize )
		{
			free ( data );
			return NULL;
		}

		switch ( (( t_phoenix_priv * ) stage->priv)->type )
		{
			case 0xA0:
			case 0xA1:
				hdr_len = 0;
				break;

			case 0xA2:
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
					case 0x54:
						hdr_len = 19;
						break;
					default:
						printf ( "fmt_phoenix: we dont support type 0x%02X for DCT4 files\n", priv->type );
						free ( data );
						return NULL;
				}
				break;

			case 0xB2:
				switch ( priv->type )
				{
					case 0x54:
						hdr_len = priv->block_header_length;
						break;
					case 0x5D:
						hdr_len = priv->block_header_length;
						break;
					default:
						printf ( "fmt_phoenix: we dont support type 0x%02X for BB5 files\n", priv->type );
						free ( data );
						return NULL;
				}
				break;
		}


		blocklen = priv->blocksize;
		// should be correct... ...better more than less bytes reserved
		length += ((s->length / blocklen) + 1) * (blocklen + hdr_len);
		data = realloc ( data, length );

		pos = 0;
		while ( pos < s->length )
		{
			// default block size for every written block
			blocklen = priv->blocksize;

			switch ( (( t_phoenix_priv * ) stage->priv)->type )
			{
				case 0xA0:
				case 0xA1:
					memcpy ( data, s->data, s->length );
					break;

				case 0xA2:
					// align only DCT4 files
					if ( flash_block_align )
					{
						// when we dont start at block boundary
						// just align when writing a full block, that doesnt start at boundary
						if ( blocklen == priv->blocksize && (s->start + pos) % flash_block_align_size && blocklen > ((s->start + pos) % flash_block_align_size) )
							blocklen -= ( (s->start + pos) % flash_block_align_size );
					}

					//
					// some special block sizes patch...
					// ... just increase blocksize, else we 
					// could run out of memory because of too many headers
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

					// limit to segment size
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

						default:
							free ( data );
							return NULL;
					}
					break;

				case 0xB2:
					// for BB5 behave a little different from DCT4 since it has some kind of random access

					// if some non-standard size is set as blocksize, set to maximum
					if ( flash_block_align && blocklen % flash_block_align_size )
						blocklen = flash_block_align_size;

					// but limit to segment size
					if ( pos + blocklen > s->length )
						blocklen = s->length - pos;


					switch ( priv->type )
					{
						case 0x54:
							ofs += fmt_phoenix_add_flashblock_54 ( &data[ofs], s->start+pos, &s->data[pos], blocklen, &priv->flash_dev, priv->subtype, priv->block_header, priv->block_header_length );
							pos += blocklen;
							break;

						case 0x5D:
							ofs += fmt_phoenix_add_flashblock_5D ( &data[ofs], s->start+pos, &s->data[pos], blocklen, &priv->flash_dev, priv->subtype, priv->block_header, priv->block_header_length );
							pos += blocklen;
							break;

						default:
							free ( data );
							return NULL;
					}
					break;
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
	data[1] = BYTEPOS(tokens_length,3);
	data[2] = BYTEPOS(tokens_length,2);
	data[3] = BYTEPOS(tokens_length,1);
	data[4] = BYTEPOS(tokens_length,0);

	blocks = fmt_phoenix_create_blocks ( source, &blocks_length );
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

	DBG ( DEBUG_FMT, " => %s ( ) called\n", __FUNCTION__ );

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

	data = fmt_phoenix_create ( source, &length );

	if ( !data )
	{
		DBG ( DEBUG_FMT, " -> %s ( %s, %s ) failed !!\n", __FUNCTION__, source->name, target->name );
		return E_FAIL;
	}

	if ( target->segments )
		segment_release ( target->segments );
	target->segments = segment_create (  );
	target->segments->name = "DATA";
	target->segments->start = 0;
	target->segments->end = length;
	target->segments->length = length;
	target->segments->data = data;

	/* set the parser again (see file_format() ). all parsers should do this. shouldn't they? */
	source->parser = "PHOENIX";
	source->type = "PHOENIX";


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
fmt_phoenix_create_blocks_diff ( t_stage *stage, unsigned int *ret_length, t_phoenix_erase * erase )
{
	char *data = NULL;
	unsigned int length = 0;
	unsigned int blocklen = 0;
	unsigned int hdr_len = 0;
	unsigned int ofs = 0;
	unsigned int pos = 0;
	t_phoenix_segpriv *priv = NULL;
	t_segment *s = NULL;

	data = malloc ( 1 );
	if ( !stage || !data )
		return NULL;

	s = stage->segments;

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
			else
			{
				if ( flash_first_erase_area && !pos )
				{
					printf ( "  including first erase area to reflash 0x%08X - 0x%08X\n", start, end );
					erase_addr = LIST_NEW(erase_addr, t_phoenix_erase);
					erase_addr->start = start;
					erase_addr->end = end;
					if ( !ret )
						ret = erase_addr;

				}

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
	data[1] = BYTEPOS(tokens_length,3);
	data[2] = BYTEPOS(tokens_length,2);
	data[3] = BYTEPOS(tokens_length,1);
	data[4] = BYTEPOS(tokens_length,0);

	blocks = fmt_phoenix_create_blocks_diff ( source, &blocks_length, erase_addr );
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
	options_add_core_option ( OPT_BOOL, "fmt.phoenix", flash_block_align, "Align flash blocks to start at block boundaries." );
	options_add_core_option ( OPT_HEX, "fmt.phoenix", flash_block_align_size, "Flash block boundary." );
	options_add_core_option ( OPT_HEX, "fmt.phoenix", blocksize, "Partial flash blocksize, maybe needed for some flash devices." );
	options_add_core_option ( OPT_BOOL, "fmt.phoenix", add_flashareas, "Allow to create new flash areas." );
	options_add_core_option ( OPT_BOOL, "fmt.phoenix.bb5", bb5_map_flashid, "Flash ID to map into memory. The other one will not get mapped into mem. (0-1)" );
	options_add_core_option ( OPT_HEX, "fmt.phoenix", use_flashtype, "Which Flashtype to use for new created flash areas. (0x14 for flash, 0x2E for TIKU's MM disk)" );
	options_add_core_option ( OPT_BOOL, "fmt.phoenix", flash_first_erase_area, "Flash first erase area in any case (matters only on partial flash)." );

	return E_OK;
}

#endif
