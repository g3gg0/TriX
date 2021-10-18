#ifndef __TRIX_FMT_DCT4CRYPT_C__
#define __TRIX_FMT_DCT4CRYPT_C__

#include <stdlib.h>
#include "defines.h"
#include "segment.h"
#include "stage.h"
#include "symbols.h"

#include "fmt.h"
#include "options.h"

#include "mem.h"

typedef struct s_dct4crypt_priv t_dct4crypt_priv;
struct s_dct4crypt_priv
{
	STRUCT_HEADER;
    int basecode;
	char *old_parser;
	char *old_name;
	t_stage_priv *old_priv;
};

typedef struct _
{
	unsigned int addr_bits;
	unsigned short xor_value;
}
ADDR_ADJ;


unsigned short mbit[] = {
	0x1221, 0xA91A, 0x52A5, 0x0908, // 0001 0002 0004 0008
	0xa918, 0x1020, 0xFFFF, 0x52A1, // 0010 0020 0040 0080
	0x0100, 0x1220, 0xAD1A, 0x0900, // 0100 0200 0400 0800
	0x1000, 0x2908, 0x5221, 0xa908, // 1000 2000 4000 8000
};

unsigned short maddr[] = {
	0x0FAE, 0x3E7F, 0xC99F, 0xD6F7, // 00.0002 00.0004 00.0008 000.0010
	0xA71B, 0x14C4, 0x52A5, 0xCBB1, // 00.0020 00.0040 00.0080 000.0100
	0x4285, 0xEFDF, 0xDFF7, 0x5080, // 00.0200 00.0400 00.0800 000.1000
	0xEE9F, 0x0000, 0x8432, 0x5221, // 00.2000 00.4000 00.8000 001.0000
	0x4084, 0xA91A, 0x56E7, 0xB93A, // 02.0000 04.0000 08.0000 010.0000
	0x5B21, 0xA818, 0x0000, 0xEFDF, // 20.0000 40.0000 80.0000 100.0000
};
ADDR_ADJ maddr_adj[] = {
	{0x00140,	0x1000},
	{0x00220,	0x52a1},
	{0x00480,	0x1221},
	{0x00600,	0xB928},
	{0x00810,	0x5221},
	{0x00840,	0x1220},
	{0x00900,	0x2008},
	{0x01020,	0x1221},
	{0x01080,	0x0908},
	{0x01100,	0x52A1},
	{0x02020,	0x0100},
	{0x02080,	0xFBBD},
	{0x04010,	0xA91A},
	{0x04040,	0xA908},
	{0x08008,	0x2908},
	{0x09000,	0x1000},
	{0x0A000,	0xBD3A},
	{0x10010,	0xAD1A},
	{0x10040,	0x5221},
	{0x10400,	0x0908},
	{0x20200,	0x53A5},
	{0x40040,	0xA91A},
	{0x44000,	0x1B20},
	{0x80100,	0xA918},
	{0x800000,	0xB908},

	{0, 0}
};

unsigned short en_codes[65536];
unsigned short de_codes[65536];
unsigned short de_addr[65536];

unsigned int crypt_basecode = 0;
unsigned int skip_autobasecode = 0;
unsigned int flash_start = 0;
unsigned int mcu_flash_start = 0x1000000;        // 6610 NHL4   MCU area start
unsigned int mcu_crypt_start = 0x84;             //             start offset of encrypted MCU data
unsigned int fls_endianess = 0;
unsigned int force_dct4crypt = 0;
unsigned int dct4crypt_use_algo = 0;		//use table or algorithm to de/encrypt codes

int fmt_dct4flash_detection[3][2][2] =
{
	{ { 0x0084, 0xFFFF }, { 0x0086, 0xFFFF } },
	{ { 0x009C, 0xFFFF }, { 0x009E, 0xFFFF } },
	{ { 0x0000, 0x5050 }, { 0x0002, 0x4D00 } }
};

//for RH-17/2280 at offset 0x084: 0x5E80


unsigned short
fmt_dct4crypt_get_half ( unsigned char *buf, unsigned int ofs )
{
	return ( buf[ofs] << 8 ) | buf[ofs ^ 1];
}

void
fmt_dct4crypt_set_half ( unsigned char *buf, unsigned int ofs, unsigned short code )
{
	buf[ofs] = code >> 8;
	buf[ofs ^ 1] = ( unsigned char ) code;
}


unsigned short
fmt_dct4crypt_address_bits ( unsigned short code, unsigned int addr )
{
	int i = 0;
	ADDR_ADJ *adj = maddr_adj;

	while ( adj->addr_bits )
	{
		if ( ( addr & adj->addr_bits ) == adj->addr_bits )
			code ^= adj->xor_value;
		adj++;
	}

	for ( i = 0; i < 24; i++ )
	{
		if ( addr & ( 1 << ( i + 1 ) ) )
			code ^= maddr[i];
	}
	return code;
}

void
fmt_dct4crypt_generate_codes (  )
{
	unsigned int c, nc, i;
	for ( c = 0; c <= 65535; c++ )
	{
		nc = 0;
		for ( i = 0; i < 16; i++ )
			if ( c & ( 1 << i ) )
				nc ^= mbit[i];
		de_codes[nc]  = (unsigned int)c;
		en_codes[c] = nc;
	}
	for ( c = 0; c <= 0xFFFF; c+=1 )
	{
		nc = 0;
		for ( i = 0; i < 24; i++ )
			if ( c & ( 1 << i ) )
				nc ^= maddr[i];

		de_addr[c] = nc;
	}
}


void
fmt_dct4crypt_crypt_encode ( unsigned char * buf, unsigned int addr, unsigned int len, int basecode )
{
	unsigned int ofs, fad;
	unsigned short code;

	for ( ofs = 0; ofs < len; ofs += 2 )
	{
		fad = addr + ofs - mcu_flash_start;

		code = fmt_dct4crypt_get_half ( buf, ofs ^ fls_endianess );

		// hack
		code ^= basecode;

		// Get the CT
		code = en_codes[code];

		// Clean the special address bits
		code = fmt_dct4crypt_address_bits ( code, addr + ofs );

		fmt_dct4crypt_set_half ( buf, ofs, code );
	}
}

void
fmt_dct4crypt_crypt_decode ( unsigned char * buf, unsigned int addr, unsigned int len )
{
	unsigned int ofs = 0;
	unsigned int fad = 0;
	unsigned int pos = 0;
	unsigned short code;

	for ( ofs = 0; ofs < len; ofs += 2 )
	{
		fad = addr + ofs - mcu_flash_start;
		code = fmt_dct4crypt_get_half ( buf, ofs ^ fls_endianess );

		// Clean the special address bits
		code = fmt_dct4crypt_address_bits ( code, addr + ofs );

		// Get the PT
		code = de_codes[code];

		fmt_dct4crypt_set_half ( buf, ofs ^ fls_endianess, code );
	}
}


unsigned int
fmt_dct4crypt_free ( t_stage * s )
{
	t_dct4crypt_priv *priv = NULL;

    if ( !s )
        return E_OK;

	priv = (t_dct4crypt_priv* )s->priv;
	s->priv = priv->old_priv;
	s->name = priv->old_name;
	s->parser = priv->old_parser;

	if ( !priv->struct_refs )
		free ( priv );

    return fmt_free_priv ( s );
}

unsigned int
fmt_dct4crypt_encode_segment ( t_segment *s, unsigned short basecode )
{
	char *data = NULL;
	int pos = 0;
	int addr = 0;
	int len = 0;

	if ( s->end - s->start != 0 )
	{
		if ( s->end >= mcu_flash_start + mcu_crypt_start )
		{
			if ( s->start <= mcu_flash_start + mcu_crypt_start )
				addr = mcu_flash_start + mcu_crypt_start;
			else
				addr = s->start;

			pos = addr - s->start;
			len = s->length - pos;
			fmt_dct4crypt_crypt_encode ( &s->data[pos], addr, len, basecode );
		}
	}
	return E_OK;
}
unsigned int
fmt_dct4crypt_decode_segment ( t_segment *s, unsigned int fix, unsigned short basecode )
{
	char *data = NULL;
	unsigned int pos = 0;
	unsigned int addr = 0;
	unsigned int len = 0;

	if ( s->end - s->start != 0 )
	{
		if ( s->end >= mcu_flash_start + mcu_crypt_start )
		{
			if ( s->start <= mcu_flash_start + mcu_crypt_start )
				addr = mcu_flash_start + mcu_crypt_start;
			else
				addr = s->start;

			pos = addr - s->start;
			len = s->length - pos;
			if ( !fix )
				fmt_dct4crypt_crypt_decode ( &s->data[pos], addr, len );
			else
			{
				while ( len )
				{
					s->data[pos] ^= (pos&1)?(basecode&0xFF):((basecode>>8) & 0xFF);
					pos++;
					len--;
				}
			}
		}
	}
	return E_OK;
}

unsigned int
fmt_dct4crypt_try_decode ( t_stage * source, t_stage * target, unsigned int auto_data[2][2] )
{
	t_stage *t = NULL;
	t_segment *segment = NULL;
	t_dct4crypt_priv *priv = NULL;
	unsigned int segments = 0;
	unsigned int pos = 0;


	t = stage_duplicate ( source );
	if ( !t )
    {
	    DBG ( DEBUG_FMT, " -> %s ( ) stage_duplicate() failed !!\n", __FUNCTION__ );
        return E_FAIL;
    }

	segments = segment_count ( t->segments );
    if ( segments < 1 )
    {
	    DBG ( DEBUG_FMT, " -> %s ( ) wrong segment count !!\n", __FUNCTION__ );
		stage_release ( t );
        return E_FAIL;
    }

	pos = 0;
	while ( pos < segments )
	{
		segment = segment_find_by_num ( t->segments, pos );
		if ( !segment )
		{
			DBG ( DEBUG_FMT, " -> %s ( ) error getting segment !!\n", __FUNCTION__ );
			stage_release ( t );
			return E_FAIL;
		}
		if ( !pos )
			flash_start = segment->start;

		fmt_dct4crypt_decode_segment ( segment, 0, 0  );
		pos++;
	}


	if ( !skip_autobasecode )
	{
		if ( flash_start < mcu_flash_start || !v_valid_raw (t, flash_start + auto_data[0][0]))
		{
			stage_release ( t );
			return E_FAIL;
		}

		crypt_basecode = v_get_h_raw (t, flash_start + auto_data[0][0]) ^ auto_data[0][1];
		if ( (v_get_h_raw (t, flash_start + auto_data[1][0]) ^ crypt_basecode) != auto_data[1][1] )
		{
			stage_release ( t );
			return E_FAIL;
		}
	}
	pos = 0;
	while ( pos < segments )
	{
		segment = segment_find_by_num ( t->segments, pos );
		if ( !segment )
		{
			DBG ( DEBUG_FMT, " -> %s ( ) error getting segment !!\n", __FUNCTION__ );
			stage_release ( t );
			return E_FAIL;
		}
		fmt_dct4crypt_decode_segment ( segment, 1, crypt_basecode  );
		pos++;
	}

	t->priv = PRIV_MALLOC ( t_dct4crypt_priv );
	if ( !t->priv )
	{
		stage_release ( t );
        return E_FAIL;
	}

	priv = (t_dct4crypt_priv* )t->priv;
	priv->struct_refs = 1;
	priv->basecode = crypt_basecode;
	priv->old_parser = source->parser;
	priv->old_name = source->name;
	priv->old_priv = source->priv;
	t->parser = "DCT4CRYPT";
	t->name = "DECRYPTED";

	if ( stage_replace ( target, t ) != E_OK )
    {
	    DBG ( DEBUG_FMT, " -> %s ( ) stage_replace() failed !!\n", __FUNCTION__ );
		stage_release ( t );
        return E_FAIL;
    }


    return E_OK;

}
unsigned int
fmt_dct4crypt_decode ( t_stage * source, t_stage * target )
{
	DBG ( DEBUG_FMT, " => %s ( ) called\n", __FUNCTION__ );

    if ( !target )
        target = source->next;

    if ( !source || !target || !source->segments || !source->segments->data )
    {
	    DBG ( DEBUG_FMT, " -> %s ( ) failed !!\n", __FUNCTION__ );
        return E_FAIL;
    }
	if ( !strcmp ( source->parser, "DCT4CRYPT" ) )
    {
	    DBG ( DEBUG_FMT, " -> %s ( ) already decrypted\n", __FUNCTION__ );
        return E_FAIL;
    }
	if ( !force_dct4crypt && strcmp ( source->parser, "PHOENIX" ) )
    {
	    DBG ( DEBUG_FMT, " -> %s ( ) no PHOENIX file\n", __FUNCTION__ );
        return E_FAIL;
    }

	if ( force_dct4crypt )
	{
		source->segments->start = 0x1000000;
		source->segments->end = source->segments->start + source->segments->length;
	}


	if ( fmt_dct4crypt_try_decode ( source, target, fmt_dct4flash_detection[0] ) != E_OK &&
		 fmt_dct4crypt_try_decode ( source, target, fmt_dct4flash_detection[1] ) != E_OK &&
		 fmt_dct4crypt_try_decode ( source, target, fmt_dct4flash_detection[2] ) != E_OK )
    {
	    DBG ( DEBUG_FMT, " -> %s ( ) seems not to be encrypted\n", __FUNCTION__ );
        return E_FAIL;
    }


	DBG ( DEBUG_FMT, " ## %s ( %s, %s ) done\n", __FUNCTION__, source->name, target->name );
    return E_OK;
}

unsigned int
fmt_dct4crypt_encode ( t_stage * source, t_stage * target )
{
	t_stage *t = NULL;
	t_segment *segment = NULL;
	t_dct4crypt_priv *priv = NULL;
	int segments = 0;
	int pos = 0;

	DBG ( DEBUG_FMT, " => %s ( ) called\n", __FUNCTION__ );

    if ( !source || !target || !source->segments || !source->segments->data || !source->priv )
    {
	    DBG ( DEBUG_FMT, " -> %s ( ) failed !!\n", __FUNCTION__ );
        return E_FAIL;
    }

	t = stage_duplicate ( source );
	if ( !t )
    {
	    DBG ( DEBUG_FMT, " -> %s ( ) stage_duplicate() failed !!\n", __FUNCTION__ );
        return E_FAIL;
    } 

	priv = (t_dct4crypt_priv *)source->priv;

	segments = segment_count ( t->segments );
    if ( segments < 1 )
    {
	    DBG ( DEBUG_FMT, " -> %s ( ) wrong segment count !!\n", __FUNCTION__ );
		stage_release ( t );
        return E_FAIL;
    }

	pos = 0;
	while ( pos < segments )
	{
		segment = segment_find_by_num ( t->segments, pos );
		if ( !segment )
		{
			DBG ( DEBUG_FMT, " -> %s ( ) error getting segment !!\n", __FUNCTION__ );
			stage_release ( t );
			return E_FAIL;
		}
		fmt_dct4crypt_encode_segment ( segment, priv->basecode  );
		pos++;
	}

	if ( t->priv )
		t->priv->struct_refs--;
	t->priv = priv->old_priv;
	t->name = priv->old_name;
	t->parser = priv->old_parser;
	t->priv->struct_refs++;


	/* set the parser again (see file_format() ). all parsers should do this. shouldn't they? */
	source->parser = "DCT4CRYPT";
	source->type = "DCT4CRYPT";


	if ( stage_replace ( target, t ) != E_OK )
    {
	    DBG ( DEBUG_FMT, " -> %s ( ) stage_replace() failed !!\n", __FUNCTION__ );
		stage_release ( t );
        return E_FAIL;
    }

	DBG ( DEBUG_FMT, " ## %s ( %s, %s ) done\n", __FUNCTION__, source->name, target->name );
    return E_OK;
}


t_fmt_handler dct4crypt_handler = {
    "DCT4CRYPT",
    "ENCRYPTION",
	"fmt_dct4crypt_decode",
    fmt_dct4crypt_decode,
	"fmt_dct4crypt_encode",
    fmt_dct4crypt_encode,
	"fmt_dct4crypt_free",
    fmt_dct4crypt_free,
	NULL,
	NULL
};

unsigned int
fmt_dct4crypt_init (  )
{
	fmt_dct4crypt_generate_codes ();
    fmt_register_handler ( &dct4crypt_handler );

	options_add_core_option ( OPT_HEX, "fmt.dct4crypt", flash_start, "The start address of the current flashfile" );
	options_add_core_option ( OPT_HEX, "fmt.dct4crypt", mcu_flash_start, "The start address of MCU data" );
	options_add_core_option ( OPT_HEX, "fmt.dct4crypt", mcu_crypt_start, "The start offset of crypted data relative to MCU start");
	options_add_core_option ( OPT_BOOL, "fmt.dct4crypt", fls_endianess, "Use Little Endianess" );
	options_add_core_option ( OPT_HEX, "fmt.dct4crypt", crypt_basecode, "Crypto Basecode" );
	options_add_core_option ( OPT_BOOL, "fmt.dct4crypt", skip_autobasecode, "Skip Crypto Basecode autodetection" );
	options_add_core_option ( OPT_BOOL, "fmt.dct4crypt", force_dct4crypt, "Force DCT4crypt (e.g. if no phoenix header used)" );
	options_add_core_option ( OPT_BOOL, "fmt.dct4crypt", dct4crypt_use_algo, "Use algorithm for codes instead of tables (todo)" );
	

	return E_OK;
}

#endif


