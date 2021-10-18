#ifndef __TRIX_FMTCNT_C__
#define __TRIX_FMTCNT_C__

#include "defines.h"
#include "seer.h"
#include "segment.h"
#include "stage.h"
#include "file_io.h"
#include "file.h"
#include "workspace.h"
#include "util.h"
#include "fmt.h"
#include "options.h"
#include "mem.h"

#include "trixplug.h"

/*
	format information:

	starts with:
		F0F0 0001 FF00 0000 FFF0 FFFF 00000000
		[some identifier]   [ident?]  [ part ]

	part is starting with 0

	every 0x208 bytes (including this 8 bytes):
		FFF0 FFFF [ part ]

	every 0x20000 bytes this sequence is starting again
	but part is still continuing

	letzter 0x20000 part:
	F0F0 0001 FFC0 0000 FFF0 FFFF 0000 xxxx
	            yy <- 0xC0 marks end

*/

/*
   info for rm237__07.21.image_b:

#define CNT_BYTES_PER_SECTOR			520 //bytes
#define CNT_BYTES_PER_BLOCK		 65536 //bytes
#define CNT_SECTORS_PER_BLOCK		 ( ( CNT_BYTES_PER_BLOCK - 16 ) / CNT_BYTES_PER_SECTOR )

typedef struct {
   unsigned short cnt_sid0; // FFF0
   unsigned short cnt_sid1; // FFFF
   unsigned int cnt_sector_id;
   unsigned char data[512];
} fmt_cnt_sector;

typedef struct {
   unsigned short cnt_bid0; // F0F0
   unsigned short cnt_bid1; // 0001
   unsigned short cnt_bid2; // FF00, (FFC0 in last block)
   unsigned short cnt_bid3; // 0000
   
   struct fmt_cnt_sector[CNT_SECTORS_PER_BLOCK];
   
   unsigned char cnt_padd[6]; // FFFF FFFF FFFF
   unsigned short cnt_bid4; // F0F0
} fmt_cnt_block;
*/


#define HDR_BLOCK  0xF0F0
#define HDR_SECT   0xFFF0
#define HDR_TAIL   0xFFFF
#define MARK_LAST  0xFFC0

unsigned char *last_error_string = "None";
unsigned int block_size  = 0x10000;
unsigned int sector_size = 0x200; 
/*
   until now just have seen padding to 0x10000 byte boundaries...
   i guess, thats to add padding for flash sectors.
   but lets keep that configurable
*/
unsigned int block_alignment = 0x10000;

unsigned short sector_header[4] = { 0xFFF0, 0xFFFF, 0x0000, 0x0000 };
unsigned short block_header[4]  = { 0xF0F0, 0x0001, 0xFF00, 0x0000 };
unsigned short block_trailer[4] = { 0xFFFF, 0xFFFF, 0xFFFF, 0xF0F0 };

unsigned int fmtCNT_init ( );
unsigned int fmtCNT_cleanup ( );
unsigned char *fmtCNT_lasterror ( );

// include all the TriXPluG function/init stuff
TRIXPLUG_STUBS
#include "trixplug_wrapper.h"

#define FMTCNT_TITLE  "fmtCNT"
#define FMTCNT_MAJOR  0
#define FMTCNT_MINOR  21

OPT_BEGIN
	OPTION( OPT_HEX, "Block size", "block_size", &block_size )
	//OPTION( OPT_HEX, "Block alignment (to fill with padding)", "block_alignment", &block_alignment )
	OPTION( OPT_INT, "Sector size", "sector_size", &sector_size )
OPT_FINISH

PLUGIN_INFO_BEGIN
	PLUGIN_NAME ( FMTCNT_TITLE )
	PLUGIN_VERSION ( FMTCNT_MAJOR, FMTCNT_MINOR )
	PLUGIN_INIT ( fmtCNT_init )
	PLUGIN_CLEANUP ( fmtCNT_cleanup )
	PLUGIN_LASTERROR ( fmtCNT_lasterror )
	PLUGIN_OPTIONS
PLUGIN_INFO_FINISH


unsigned int
fmt_cnt_free ( t_stage *s )
{
	DBG ( DEBUG_FMT, " => %s ( ) called\n", __FUNCTION__ );

	return E_OK;
}


unsigned int
fmt_cnt_parse ( t_segment *src, t_segment *dst )
{
	unsigned int block_count = 0;
	unsigned int part_count = 0;

	unsigned int pos_in = src->start;
	unsigned int pos_out = 0;

	unsigned short header_type = 0;
	unsigned int seq_number_last = 0;
	unsigned int seq_number = 0;
	unsigned int last_block = 0;

	unsigned int sector_count = 0;
	unsigned int sector_count_detected = 0;

	unsigned int sector_lengh = 0x200;

	while ( pos_in < dst->end )
	{
		header_type = v_get_h_raw ( src, pos_in );

		switch ( header_type )
		{
			case HDR_BLOCK:
				// thats just he block header..
				// maybe it contains some important stuff, but we default
				// to a blocksize of 512 bytes

				if ( v_get_h_raw ( src, pos_in + 4 ) == MARK_LAST )
					last_block = 1;
				pos_in += 8;
				break;

			case HDR_SECT:
				sector_count++;
				seq_number_last = seq_number;
				seq_number = v_get_w_raw ( src, pos_in + 4 );

				// skipped blocks are defaulted to zero
				if ( seq_number != 0 && seq_number != seq_number_last + 1 )
				{
					int difference = seq_number - seq_number_last;
					int skipped_size = (difference-1) * 0x200;

					dst->length += skipped_size;
					dst->data = (unsigned char*)realloc ( dst->data, dst->length );

					memset ( dst->data + pos_out, 0x00, skipped_size );

					pos_out += skipped_size;
				}
				pos_in += 8;

				memcpy ( dst->data + pos_out, src->data + pos_in - src->start, sector_lengh );
				pos_in += sector_lengh;
				pos_out += sector_lengh;
				break;

			case HDR_TAIL:
				/* autodetect sector count per block */
				if ( !sector_count_detected )
				{
					sector_count_detected = 1;
					block_size = sizeof (block_header) + (sector_count * (sizeof (sector_header) + sector_size )) + sizeof (block_trailer);
					block_size = ALIGN_BY ( block_size, block_alignment );
				}

				// stop on last block
				if ( last_block )
					pos_in = dst->end;

				pos_in += 8;
				break;

			default:
				DBG ( DEBUG_FMT, " => %s ( ) got unknow block header\n", __FUNCTION__ );
				return E_FAIL;
		}

		HEAP_CHECK;
	}

	dst->length = pos_out;
	dst->data = (unsigned char*)realloc ( dst->data, dst->length );

	return E_OK;
}


unsigned int
fmt_cnt_decode ( t_stage *source, t_stage *target )
{
	t_segment *s = NULL;
	t_segment *t = NULL;
	unsigned int endianess = 0;
	unsigned int num = 0;

	DBG ( DEBUG_FMT, " => %s ( ) called\n", __FUNCTION__ );

	if ( !target )
		target = source->next;

	if ( !source || !target || !source->segments || !source->segments->data )
	{
		DBG ( DEBUG_FMT, " -> %s ( ) failed (null ptr)!!\n", __FUNCTION__ );
		return E_FAIL;
	}

	if ( source->length <= 16 && source->segments->length <= 16 )
	{
		DBG ( DEBUG_FMT, " -> %s ( ) failed (length)!!\n", __FUNCTION__ );
		return E_FAIL;
	}

	if ( !strcmp ( source->type, "CNT" ) )
		return E_FAIL;

	/* we should check all segments in current stage. Phones without SD slot,
	   have additional internal FAT partition,
	*/
	s = source->segments;
	while ( s )
	{
		if ( GET_HALF_RAW ( s->data, 0 ) == 0xF0F0 && GET_HALF_RAW ( s->data, 2 ) == 0x0001 &&
		     GET_HALF_RAW ( s->data, 4 ) == 0xFF00 && GET_HALF_RAW ( s->data, 6 ) == 0x0000 )
		     endianess = FLAGS_ENDIANESS_BE;

		else if ( GET_HALF_RAW ( s->data, 0 ) == 0xF0F0 && GET_HALF_RAW ( s->data, 2 ) == 0x0100 &&
		          GET_HALF_RAW ( s->data, 4 ) == 0x00FF && GET_HALF_RAW ( s->data, 6 ) == 0x0000 )
			endianess = FLAGS_ENDIANESS_LE;

		s = s->next;
	}

	if ( !endianess )
	{
		DBG ( DEBUG_FMT, " -> %s ( ) failed (ident)!!\n", __FUNCTION__ );
		return E_FAIL;
	}
	else
	{
		source->flags &= ~FLAGS_ENDIANESS;
		source->flags |= endianess;
		target->flags &= ~FLAGS_ENDIANESS;
		target->flags |= endianess;
	}

	s = source->segments;
	while ( s )
	{
		if ( !target->segments )
		{
			target->segments = segment_create();
			t = target->segments;
		}
		else
			t = segment_add ( t );

		// default size to old size
		t->start = s->start;
		t->length = s->length;
		t->data = (unsigned char*)malloc ( t->length );
		t->end = t->start + t->length;

		if ( !t->length || !t->data )
		{
			DBG ( DEBUG_FMT, " -> %s ( ) failed (data ptr or length zero)!!\n", __FUNCTION__ );
			return E_FAIL;
		}

		// here's the thing - how to inherit endianess flags when I want to use v_get_... with segments only
		s->flags &= ~FLAGS_ENDIANESS;
		s->flags |= endianess;
		t->flags &= ~FLAGS_ENDIANESS;
		t->flags |= endianess;

		// CNT container detected
		if ( v_get_h_raw ( s, s->start + 0 ) == 0xF0F0 && v_get_h_raw ( s, s->start + 2 ) == 0x0001 &&
		     v_get_h_raw ( s, s->start + 4 ) == 0xFF00 && v_get_h_raw ( s, s->start + 6 ) == 0x0000 )
		{
			t->name = "CNT";

			// parse here
			if ( fmt_cnt_parse ( s, t ) != E_OK )
			{
				// allocated here, so release here...
				segment_release_all ( target->segments );
				DBG ( DEBUG_FMT, " -> %s ( ) fmt_cnt_parse failed !!\n", __FUNCTION__ );
				return E_FAIL;
			}
		}
		else
		{
			// FAT partition
			if ( v_get_h_raw ( source, s->start + 0x1FE ) == 0xAA55 )
				t->name = "FAT";
			else // RAW data
				t->name = "RAW";

			memcpy ( t->data, s->data, t->length );
		}

		t->data = (unsigned char*)realloc ( t->data, t->length );
		t->end = t->start + t->length;

		s = s->next;
	}

	target->name = "PARSED";
	target->length = 0;
	target->parser = "CNT";
	target->type = "CNT";

	HEAP_CHECK;

	DBG ( DEBUG_FMT, " ## %s ( %s, %s ) done\n", __FUNCTION__, source->name, target->name );
	return E_OK;
}


unsigned int
fmt_cnt_encode_data ( t_stage *src, t_stage *dst )
{
	t_segment *s = NULL;
	t_segment *d = NULL;

	unsigned int src_pos = 0;
	unsigned int dst_pos = 0;
	unsigned int sector_nr = 0;

	unsigned int sectors = 0;
	unsigned int sector = 0;
	unsigned int blocks = 0;
	unsigned int block = 0;

	unsigned int block_payload = 0;
	unsigned int block_padding = 0;
	unsigned int sectors_per_block = 0;
	unsigned int max_length = 0;


	s = src->segments;
	d = dst->segments;

	while ( s )
	{
		if ( strcmp ( s->name, "CNT" ) )
		{
			d->length = s->length;
			d->data = realloc ( d->data, d->length );
			if ( !d->data )
				return E_FAIL;

			memcpy ( d->data, s->data, d->length );
		}
		else
		{
			/* TODO:
			   check for empty subblocks and skip them
			*/

			/* calculate how many sectors fit into one block */
			sectors_per_block = ( block_size - 8 - 8 ) / ( 8 + sector_size );

			/* calculate padding */
			block_payload = 8 + sectors_per_block * ( 8 + sector_size ) + 8;
			block_padding = ( block_size - block_payload ) % block_size;

			/* calcuate number of data sectors */
			sectors = s->length / sector_size;
			if ( s->length % sector_size > 0 )
				sectors++;

			/* calcuate number of blocks */
			blocks = sectors / sectors_per_block;
			if ( sectors % sectors_per_block > 0 )
				blocks++;

			/* the final length */
			d->length = blocks * ( block_payload + block_padding );
			d->end = d->start + d->length;
			d->flags = s->flags;

			d->data = realloc ( d->data, d->length );
			if ( !d->data )
				return E_FAIL;

			/* fill whole area */
			memset ( d->data, 0xFF, d->length );

			block = 0;
			src_pos = s->start;
			dst_pos = d->start;
			while ( block < blocks )
			{
				block++;

				/* add block header */
				v_set_h_raw ( dst, dst_pos + 0, block_header[0] );
				v_set_h_raw ( dst, dst_pos + 2, block_header[1] );
				v_set_h_raw ( dst, dst_pos + 4, block_header[2] );
				v_set_h_raw ( dst, dst_pos + 6, block_header[3] );

				/* when last block reached, set end marker */
				if ( block == blocks )
					v_set_h_raw ( dst, dst_pos + 4, MARK_LAST );
				dst_pos += 8;


				/* write sectors */
				sector = 0;
				while ( sector < sectors_per_block )
				{
					max_length = 0;

					/* enough data to write? */
					if ( src_pos < s->end )
					{
						/* add sector header */
						v_set_h_raw ( dst, dst_pos + 0, sector_header[0] );
						v_set_h_raw ( dst, dst_pos + 2, sector_header[1] );
						v_set_w_raw ( dst, dst_pos + 4, sector_nr );
						dst_pos += 8;

						/* get the number of available bytes */
						max_length = s->length - src_pos;
						if ( max_length > sector_size )
							max_length = sector_size;

						/* write sector data */
						v_memcpy_get_raw ( src, d->data + dst_pos - d->start, src_pos, max_length );
						src_pos += max_length;
						dst_pos += sector_size;
					}
					else
					{
						/* just increment destination pointer keeping data FF'ed */
						dst_pos += 8;
						dst_pos += sector_size;
					}

					HEAP_CHECK;
					sector++;
					sector_nr++;
				}

				/* skipp padding bytes */
				dst_pos += block_padding;

				/* add trailer */
				v_set_h_raw ( dst, dst_pos + 0, block_trailer[0] );
				v_set_h_raw ( dst, dst_pos + 2, block_trailer[1] );
				v_set_h_raw ( dst, dst_pos + 4, block_trailer[2] );
				v_set_h_raw ( dst, dst_pos + 6, block_trailer[3] );
				dst_pos += 8;
			}
		}

      s = s->next;
      d = d->next;
	}

	return E_OK;
}


unsigned int
fmt_cnt_encode ( t_stage *source, t_stage *target )
{
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

	if ( fmt_cnt_encode_data ( source, target ) != E_OK )
	{
	   DBG ( DEBUG_FMT, " -> %s ( %s, %s ) failed !!\n", __FUNCTION__, source->name, target->name );
      return E_FAIL;
   }

	target->segments->flags &= ~FLAGS_FREE_NAME;
	target->segments->name = "DATA";

	/* set the parser again (see file_format() ). all parsers should do this. shouldn't they? */
	source->parser = "CNT";
	source->type = "CNT";

	HEAP_CHECK;

	DBG ( DEBUG_FMT, " ## %s ( %s, %s ) done\n", __FUNCTION__, source->name, target->name );
	return E_OK;
}


t_fmt_handler fmtCNT_handler = {
	"CNT",
	"CONTAINER",
	"fmt_cnt_decode",
	 fmt_cnt_decode,
	"fmt_cnt_encode",
	fmt_cnt_encode,
	"fmt_cnt_free",
	fmt_cnt_free,
	NULL,
	NULL
};

unsigned char *fmtCNT_lasterror ( )
{
	unsigned char *ret = last_error_string;
	last_error_string = "None";

	return ret;
}

unsigned int fmtCNT_cleanup ( )
{
	fmt_unregister_handler ( fmtCNT_handler.name );
	printf ( "FMT CNT Plugin Unloaded" );

	return E_OK;
}

unsigned int fmtCNT_init ( )
{
   fmt_register_handler ( &fmtCNT_handler );
   printf ( FMTCNT_TITLE " v%01i.%02i Plugin Loaded", FMTCNT_MAJOR, FMTCNT_MINOR  );

   return E_OK;
}

#endif /* __TRIX_FMTCNT_C__ */
