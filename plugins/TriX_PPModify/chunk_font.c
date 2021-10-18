#ifndef __TRIX_CHUNK_FONT_C__
#define __TRIX_CHUNK_FONT_C__

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
extern int endianess_be;
extern int font_build_textdump;
extern int alignment_byte;
extern struct trix_functions *ft;

#include "trixplug_wrapper.h"

/*
	FONT chunk:   (UNC_N175V0403_6255_03)
		16000000   (0x16 entries with 0x20 width)

		DC= DefaultChar

		[bitstbl]  [offset1]  [offset2]  [e1] [e2]          [DC]                     [flags]

		E84D 0200  A84C 0200  C002 0000  0900 3315     0714 00E0 0F00 0002 FFFF FFFF 07800000
		C84D 0200  D84C 0200  40AC 0000  0000 E201     6C01 00E0 0C00 0200 0F00 0002 07800000
		A84D 0200  C04C 0200  38BB 0000  0000 0E02     A701 00E0 0F00 8000 0F00 0002 07800000
		884D 0200  A84C 0200  90CB 0000  0000 0A02     B301 00E0 0F00 4000 0F00 0002 07800000
		684D 0200  904C 0200  C8DB 0000  0900 6715     2F14 00E0 1300 8000 FFFF FFFF 07800000
		484D 0200  C04C 0200  E886 0100  0000 1A03     0A02 00E0 1A00 0200 1300 8000 07800000
		284D 0200  A84C 0200  A09F 0100  0000 2300     1F00 00E0 0D00 2000 FFFF FFFF 07800000
		084D 0200  904C 0200  A0A0 0100  0000 2700     2300 00E0 1000 2000 FFFF FFFF 07800000
		E84C 0200  784C 0200  C0A1 0100  0000 2300     1F00 00E0 1B00 2000 FFFF FFFF 07800000
		C84C 0200  604C 0200  C0A2 0100  0000 3800     0000 2000 0F00 0001 FFFF FFFF 07800000
		A84C 0200  484C 0200  68A4 0100  0000 0600     0000 2000 1300 0001 FFFF FFFF 07800000
		884C 0200  304C 0200  80A4 0100  0000 FD01     7D01 00E0 0C00 0100 0C00 0200 07800000
		684C 0200  184C 0200  50B4 0100  0000 BF01     6201 00E0 0C00 0500 0C00 0100 07800000
		484C 0200  004C 0200  30C2 0100  0000 D301     7001 00E0 0F00 0500 0F00 4000 07800000
		284C 0200  E84B 0200  B0D0 0100  0000 3202     C901 00E0 1300 0500 1300 8000 07800000
		084C 0200  D04B 0200  28E2 0100  0000 2A02     AB01 00E0 1300 0200 1300 8000 07800000
		E84B 0200  B84B 0200  60F3 0100  0000 E701     8701 00E0 0C00 0600 0C00 0200 07800000
		C84B 0200  A04B 0200  8002 0200  0000 0000     0000 00E0 0F00 0100 0F00 4000 07800000
		A84B 0200  884B 0200  6802 0200  0000 E101     7B01 00E0 0F00 0200 0F00 4000 07800000
		884B 0200  704B 0200  5811 0200  0000 EC01     8801 00E0 0F00 0600 0F00 0200 07800000
		684B 0200  584B 0200  A020 0200  0000 E602     F601 00E0 1300 0100 1300 8000 07800000
		484B 0200  404B 0200  B837 0200  0000 4902     E201 00E0 1300 0600 1300 0200 07800000

		bits:
		0000 0000 1401 0000 0100 0100 
		3800 0000 1001 0000 0100 0200
		B804 0000 3402 0000 0100 0300
		7814 0000 D809 0000 0100 0400
		9821 0000 981A 0000 0100 0500
		0049 0000 2C48 0000 0100 0600
		3066 0000 C494 0000 0100 0700
		5860 0000 08E9 0000 0100 0800
		504C 0000 4C35 0100 0100 0900
		8837 0000 BC73 0100 0100 0A00
		B034 0000 8CB5 0100 0100 0B00
		80E7 0100 D053 0400 0100 0C00
		8022 0000 8487 0400 0100 0D00
		1820 0000 A0BB 0400 0100 0E00
		F019 0000 F8E8 0400 0100 0F00
		2878 0200 388A 0900 0100 1000
		900C 0000 4CA3 0900 0100 1100
		5006 0000 ACB0 0900 0100 1200
		E802 0000 2CB7 0900 0100 1300
		E800 0000 48B9 0900 0100 1400
		E800 0000 80BB 0900 0100 1500

	FONT chunk:   (TKI_N190V0800_6255_07)
		16000000    (0x16 entries with 0x20 width)

		285E 0200  E85C 0200  C002 0000  0900 3215 0714 00E0 0F00 0002 FFFF FFFF 07800000
		085E 0200  185D 0200  38AC 0000  0000 E201 6C01 00E0 0C00 0200 0F00 0002 07800000
		E85D 0200  005D 0200  30BB 0000  0000 0E02 A701 00E0 0F00 8000 0F00 0002 07800000
		C85D 0200  E85C 0200  88CB 0000  0000 0A02 B301 00E0 0F00 4000 0F00 0002 07800000
		A85D 0200  D05C 0200  C0DB 0000  0900 6615 2F14 00E0 1300 8000 FFFF FFFF 07800000
		885D 0200  005D 0200  D886 0100  0000 1A03 0A02 00E0 1A00 0200 1300 8000 07800000
		685D 0200  E85C 0200  909F 0100  0000 2300 1F00 00E0 0D00 2000 FFFF FFFF 07800000
		485D 0200  D05C 0200  90A0 0100  0000 2700 2300 00E0 1000 2000 FFFF FFFF 07800000
		285D 0200  B85C 0200  B0A1 0100  0000 2300 1F00 00E0 1B00 2000 FFFF FFFF 07800000
		085D 0200  A05C 0200  B0A2 0100  0000 3800 0000 2000 0F00 0001 FFFF FFFF 07800000
		E85C 0200  885C 0200  58A4 0100  0000 0600 0000 2000 1300 0001 FFFF FFFF 07800000
		C85C 0200  705C 0200  70A4 0100  0000 FD01 7D01 00E0 0C00 0100 0C00 0200 07800000
		A85C 0200  585C 0200  40B4 0100  0000 BF01 6201 00E0 0C00 0500 0C00 0100 07800000
		885C 0200  405C 0200  20C2 0100  0000 D301 7001 00E0 0F00 0500 0F00 4000 07800000
		685C 0200  285C 0200  A0D0 0100  0000 3202 C901 00E0 1300 0500 1300 8000 07800000
		485C 0200  105C 0200  18E2 0100  0000 2A02 AB01 00E0 1300 0200 1300 8000 07800000
		285C 0200  F85B 0200  50F3 0100  0000 E701 8701 00E0 0C00 0600 0C00 0200 07800000
		085C 0200  E05B 0200  7002 0200  0000 0A02 B301 00E0 0F00 0100 FFFF FFFF 07800000
		E85B 0200  C85B 0200  A812 0200  0000 E101 7B01 00E0 0F00 0200 0F00 0100 07800000
		C85B 0200  B05B 0200  9821 0200  0000 EC01 8801 00E0 0F00 0600 0F00 0200 07800000
		A85B 0200  985B 0200  E030 0200  0000 E602 F601 00E0 1300 0100 FFFF FFFF 07800000
		885B 0200  805B 0200  F847 0200  0000 4902 E201 00E0 1300 0600 1300 0200 07800000


	FONT chunk:   (3410 nhm2 5.46a)
		00000009    (0x09 entries with 0x2C width)

		[bitstbl] [offset1] [offset2] [e1] [e2]      [DC] [flags]   size, e.g. "large"    width, e.g. "bold"
		00002D5C  00002D14  0000018C  0000 0059 0059 E000 07800000 6C61726765000000000000 626F6C640000000000
		00002D30  00002CF0  00000430  0000 006E 006E E000 07800000 736D616C6C000000000000 706C61696E00FF0000
		00002D04  00002CCC  0000077C  0000 005D 005D E000 07800000 6D656469756D0000000000 626F6C640000FE0000
		00002CD8  00002CA8  00000A40  0000 0060 0060 E000 07800000 736D616C6C000000000000 626F6C640000FD0000
		00002CAC  00002C84  00000D1C  0000 01F5 0170 E000 07800000 74696E7900000000000000 706C61696E00FC0000
		00002C80  00002C60  00001CA0  0000 01E8 016D E000 07800000 74696E7900000000000000 626F6C640000FB0000
		00002C54  00002C3C  00002BBC  0000 0002 0000 0020 07800000 6469676900000000000000 323400000000FA0000
		00002C28  00002C18  00002BA8  0000 0002 0000 0020 07800000 6469676900000000000000 313300000000F90000
		00002BFC  00002BF4  00002B94  0000 0003 0000 0020 07800000 6469676900000000000000 303800000000F70000


	FONT chunk:   (3100 rh19__06.31a)
		0000000E    (0x0E entries with 0x3C width)

		00008F88  00008F18  00000348  000000000000000000000000000000000000000000000000 0000 01E4 0168 E000 000C 0002 000F 0040 07800000 00000000
		00008F4C  00008EE4  00001234  000000000000000000000000000000000000000000000000 0000 02CA 01E4 E000 000F 0040 FFFF FFFF 07800000 00000000
		00008F10  00008EB0  00002850  000000000000000000000000000000000000000000000000 0000 020F 01A2 E000 000F 0080 000F 0040 07800000 00000000
		00008ED4  00008E7C  00003894  000000000000000000000000000000000000000000000000 0000 02E6 01F0 E000 0013 0080 FFFF FFFF 07800000 00000000
		00008E98  00008E48  00004F90  000000000000000000000000000000000000000000000000 0000 031D 0204 E000 001A 0002 0013 0080 07800000 00000000
		00008E5C  00008E14  00006844  000000000000000000000000000000000000000000000000 0000 0022 001E E000 000D 0020 FFFF FFFF 07800000 00000000
		00008E20  00008DE0  00006920  000000000000000000000000000000000000000000000000 0000 0026 0022 E000 0010 0020 FFFF FFFF 07800000 00000000
		00008DE4  00008DAC  00006A1C  000000000000000000000000000000000000000000000000 0000 0022 001E E000 001B 0020 FFFF FFFF 07800000 00000000
		00008DA8  00008D78  00006AF8  000000000000000000000000000000000000000000000000 0000 0020 001D E000 000F 0200 FFFF FFFF 07800000 00000000
		00008D6C  00008D44  00006BC4  000000000000000000000000000000000000000000000000 0000 01FF 0178 E000 000C 0001 000C 0002 07800000 00000000
		00008D30  00008D10  00007B88  000000000000000000000000000000000000000000000000 0000 0000 0000 E000 000C 0005 000C 0001 07800000 00000000
		00008CF4  00008CDC  00007B54  000000000000000000000000000000000000000000000000 0000 0000 0000 E000 000F 0005 000F 0040 07800000 00000000
		00008CB8  00008CA8  00007B20  000000000000000000000000000000000000000000000000 0000 0000 0000 E000 0013 0005 0013 0080 07800000 00000000
		00008C7C  00008C74  00007AEC  000000000000000000000000000000000000000000000000 0000 0223 01A5 E000 0013 0002 0013 0080 07800000 00000000

	FONT chunk:   (8310 nhm75506.20t)
		00000006    (0x06 entries with 0x20 width)

		0000D7D8  0000D758  000000C0  0006 1512 13FF E000 07800000 0001 0002 FFFF FFFF
		0000D7B8  0000D770  0000A938  0000 01CB 0173 E000 07800000 0002 0002 0001 0002
		0000D798  0000D758  0000B778  0002 01D7 016D E000 07800000 0003 0001 0001 0002
		0000D778  0000D750  0000C618  0000 01CA 016B E000 07800000 0003 0002 0001 0002
		0000D758  0000D738  0000D450  0000 0045 0045 E000 07800000 0004 0001 0001 0002
		0000D738  0000D720  0000D660  0002 000A 0009 E000 07800000 0008 0001 0003 0001
*/

/*
	8310:
	00000005
	00003F10 00003EE8 000000A0 0000 0213 01B8 E000 07800000 0001 0002 FFFF FFFF
	00003EF0 00003ED0 00001120 0000 01CB 0173 E000 07800000 0002 0002 0001 0002
	00003ED0 00003EB8 00001F60 0000 01D7 016D E000 07800000 0003 0001 0001 0002
	00003EB0 00003EA0 00002E00 0000 01CA 016B E000 07800000 0003 0002 0001 0002
	00003E90 00003E88 00003C38 0000 0045 0045 E000 07800000 0004 0001 0001 0002
	0020 0021 00DD85AA    <-- old CMAP version
	0022 0022 0419D1AA
	0023 0025 01FA99AA
	0026 0026 0130DDAA
	0027 0027 00E405AA
	0028 0029 00D089AA
	002A 002B 020459AA
	002C 002C 00D709AA

	6610:
	00000008
	00005948 00005908 00000100 0000 01DE 0167 E000 07800000 000C 0002 000F 0040
	00005928 000058F0 00000FD8 0000 02C3 01E3 E000 07800000 000F 0040 FFFF FFFF
	00005908 000058D8 000025D8 0000 0002 0002 E000 07800000 000F 0080 FFFF FFFF
	000058E8 000058C0 000025D0 0000 02E0 01EF E000 07800000 0013 0080 FFFF FFFF
	000058C8 000058A8 00003CB8 0000 0317 0203 E000 07800000 001A 0002 0013 0080
	000058A8 00005890 00005558 0000 0020 001D E000 07800000 000D 0020 FFFF FFFF
	00005888 00005878 00005640 0000 0020 001D E000 07800000 001B 0020 FFFF FFFF
	00005868 00005860 00005728 0000 001F 001C E000 07800000 000F 0020 FFFF FFFF
	0020 01 02 00000127    <-- new CMAP version
	0022 00 03 00000127
	0023 00 07 00000127
	0024 00 05 00000127
	0025 01 06 00000127
	0027 00 01 00000127

    CMAP ver. 3
	0020 0020 0000 0600020C0900 
	0021 0021 0000 00F0010C0900
	0022 0022 0000 08DF030C0900
*/

/* font_hdr details
	DCT3:
		0x00 [04] - table_3 offset
		0x04 [04] - tracking offset
		0x08 [04] - table_1 offset
		0x0C [02] - tracking len (no. of entries - 1)
		0x0E [02] - table_1 len (no. of entries - 1)
		0x10 [02] - unk len
		0x12 [02] - default char
		0x14 [04] - flags
		0x18 [11] - size (e.g. "large")
		0x23 [06] - weight (e.g. "bold")
		0x29 [01] - font to fall back if char isn't present
		0x2A [02] - SBZ

	8310/6610:
		0x00 [04] - table_3 offset
		0x04 [04] - tracking offset
		0x08 [04] - table_1 offset
		0x0C [02] - tracking len (no. of entries - 1)
		0x0E [02] - table_1 len (no. of entries - 1)
		0x10 [02] - unk len
		0x12 [02] - default char
		0x14 [04] - flags
		0x18 [02] - font id1
		0x1A [02] - font id2 (some flags)
		0x1C [02] - fall back font id1
		0x1E [02] - fall back font id2

	3100:
		0x00 [04] - table_3 offset
		0x04 [04] - tracking offset
		0x08 [04] - table_1 offset
		0x0C [24] - SBZ
		0x24 [02] - tracking len (no. of entries - 1)
		0x26 [02] - table_1 len (no. of entries - 1)
		0x28 [02] - unk len
		0x2A [02] - default char
		0x2C [02] - font id1
		0x2E [02] - font id2 (some flags)
		0x30 [02] - fall back font id1
		0x32 [02] - fall back font id2
		0x34 [04] - flags
		0x38 [04] - SBZ

	default:
		0x00 [04] - table_3 offset
		0x04 [04] - tracking offset
		0x08 [04] - table_1 offset
		0x0C [02] - tracking len (no. of entries - 1)
		0x0E [02] - table_1 len (no. of entries - 1)
		0x10 [02] - unk len
		0x12 [02] - default char
		0x14 [02] - font id1
		0x16 [02] - font id2 (some flags; italic, bold...)
		0x18 [02] - fall back font id1
		0x1A [02] - fall back font id2
		0x1C [04] - flags

	exported/imported as *.font files ( FONT_HEADER_FONT_EDIT, BE always )
		0x00 [04] - table_3 offset
		0x04 [04] - tracking offset
		0x08 [04] - table_1 offset
		0x0C [02] - tracking len (no. of entries - 1)
		0x0E [02] - table_1 len (no. of entries - 1)
		0x10 [02] - unk len
		0x12 [02] - default char
		0x14 [04] - flags

		dct3:
			0x18 [11] - size (e.g. "large")
			0x23 [06] - weight (e.g. "bold")
			0x29 [01] - font to fall back if char isn't present
			0x2A [02] - SBZ

		other:
			0x18 [09] - 'ID1/ID2' string
			0x21 [02] - 00 00
			0x23 [09] - 'FB1/FB2' string

*/

#define FONT_HEADER_UNKNOWN   0
#define FONT_HEADER_DCT3      1  // dct3
#define FONT_HEADER_3100      2  // 3100, 2330
#define FONT_HEADER_8310      3  // 8310,
#define FONT_HEADER_DCT4      4  // 3510i, 3110c, 6255, bb5
#define FONT_HEADER_FEDIT     5  // Font Edit format

#define FONT_HEADER_FEDIT_SIZE   0x2C
#define FONT_HEADER_DCT3_SIZE    0x2C
#define FONT_HEADER_3100_SIZE    0x3C
#define FONT_HEADER_8310_SIZE    0x20
#define FONT_HEADER_DCT4_SIZE    0x20


#define FONT_CMAP_UNKNOWN   0
#define FONT_CMAP_V1        1  // dct3 and old dct4 versions like 8310
#define FONT_CMAP_V2        2  // dct4
#define FONT_CMAP_V3        3  // 2330

unsigned int font_header_ver = FONT_HEADER_UNKNOWN;
unsigned int font_cmap_ver = FONT_CMAP_UNKNOWN;


// letter-spacing struct
typedef struct {
	unsigned short start;
	unsigned short end;
	unsigned char  left;
	unsigned char  right;
	unsigned short pad;
} t_font_trac;

// glyph struct
typedef struct {
	unsigned int start;
	unsigned int end;
	unsigned int bdt;       // bitmap data offset;
	unsigned int offset;
	unsigned short unk;     // always 1?
	unsigned short width;
	unsigned int bitlen;
	unsigned char *data;
} t_font_glyf;

// character to glyph mapping
typedef struct {
	unsigned short start;
	unsigned short end;
	unsigned int offset;    // 18 bits
	unsigned int gid;        // 4; glyph id
	unsigned int height;    // 5
	unsigned int base_line; // 5
} t_font_cmap;

// font header
typedef struct {
	t_font_glyf *glyf;   // glyphs table
	t_font_trac *trac;   // letter-spacing table
	t_font_cmap *cmap;   // cmap table

	unsigned int length;        // without header

	unsigned int glyfo;  // glyphs table offset
	unsigned int traco;  // letter-spacing table offset
	unsigned int cmapo;  // cmap table offset

	unsigned int trac_len; // no. of entries - 1
	unsigned int cmap_len; // no. of entries - 1
	unsigned int glyf_len;
	unsigned int unk0;

	unsigned int default_height;
	unsigned int default_char;

	unsigned int id1;   // !dct3
	unsigned int id2;   // !dct3
	unsigned int fb1;   // fall back font id1 (8 bit in dct3)
	unsigned int fb2;   // !dct3, fall back font id2

	unsigned char flags[4];

	char size[16];             // dct3, for example "large"
	char weight[8];            // dct3, for example "bold"
} t_font_hdr;

// font subchunk header
typedef struct {
	unsigned int   count;	// font count in subchunk
	unsigned char  *data;	// font headers list
} t_font_subchunk_hdr;


//-------------------------------------------------------------------------------------------------


unsigned int free_font_hdr ( t_font_hdr *f, unsigned int entries )
{
	unsigned int entry = 0;
	unsigned int id = 0;

	if ( !f )
		return E_OK;

	while ( entry < entries )
	{
		id = 0;
		while ( id < f[entry].glyf_len )
		{
			CHECK_AND_FREE ( f[entry].glyf[id].data );
			id++;
		}

		CHECK_AND_FREE ( f[entry].trac );
		CHECK_AND_FREE ( f[entry].cmap );
		CHECK_AND_FREE ( f[entry].glyf );

		entry++;
	}

	CHECK_AND_FREE ( f );
	return E_OK;
}


unsigned char *ppmodify_font_maketext ( t_font_glyf *glyf )
{
	unsigned int width = glyf->width;
	unsigned int bitlen = glyf->bitlen;
	unsigned char *data = glyf->data;

	unsigned int lines = (bitlen*8) / width;
	unsigned int outbuf_pos = 0;
	unsigned int outbuf_len = lines * (width + 2) + 10;
	unsigned char *outbuf = malloc ( outbuf_len );

	unsigned int inbuf_pos = 0;
	unsigned int bitval = 0x01;
	unsigned int col = 0;

	memset ( outbuf, 0x00, lines * (width + 2) + 10 );
	outbuf[outbuf_pos++] = '\r'; // use DOS LF (easier for debugging with MSVC)
	outbuf[outbuf_pos++] = '\n';

	while ( inbuf_pos < bitlen - (width - 1) )
	{
		for ( col = 0; col < width; col++ )
		{
			if ( data[inbuf_pos+col] & bitval )
				outbuf[outbuf_pos++] = 'X';
			else
				outbuf[outbuf_pos++] = '.'; // or use ' ' for better readability?
		}
		outbuf[outbuf_pos++] = '\r';
		outbuf[outbuf_pos++] = '\n';

		bitval <<= 1;
		if ( bitval > 0x80 )
		{
			bitval = 0x01;
			inbuf_pos += width;
		}
	}
	outbuf[outbuf_pos++] = '\r';
	outbuf[outbuf_pos++] = '\n';
	outbuf[outbuf_pos++] = '\000';

	return outbuf;
}


unsigned char *ppmodify_build_font_dump ( t_font_hdr *f, unsigned int hdr_ver )
{
	t_workspace *ws = NULL;
	t_treenode *dumpnode = NULL;
	unsigned char *dump = NULL;
	unsigned int pos = 0;
	unsigned int bpos = 0;
	unsigned int id = 0; 
	unsigned int tmp = 0;
	unsigned int alignedlength = 0;

	if ( !f )
		return NULL;

	switch ( hdr_ver )
	{
		case FONT_HEADER_DCT3:
			f->cmapo = FONT_HEADER_DCT3_SIZE;
			break;

		case FONT_HEADER_8310:
			f->cmapo = FONT_HEADER_8310_SIZE;
			break;

		case FONT_HEADER_3100:
			f->cmapo = FONT_HEADER_3100_SIZE;
			break;

		case FONT_HEADER_DCT4:
			f->cmapo = FONT_HEADER_DCT4_SIZE;
			break;

		case FONT_HEADER_FEDIT:
			f->cmapo = FONT_HEADER_FEDIT_SIZE;
			break;
	}


	f->traco = f->cmapo + 8 * f->cmap_len;
	f->glyfo = f->traco + 8 * f->trac_len;
	f->length +=  f->cmapo;

	dump = calloc ( f->length + f->cmapo, sizeof ( unsigned char ) );
	if ( !dump )
		return NULL;

	ws = workspace_create_file_from_buffer ( dump, f->length );

	ws->flags &= ~FLAGS_ENDIANESS;
	if ( hdr_ver == FONT_HEADER_FEDIT )
		ws->flags |= FLAGS_ENDIANESS_BE;
	else if ( endianess_be )
		ws->flags |= FLAGS_ENDIANESS_BE;
	else
		ws->flags |= FLAGS_ENDIANESS_LE;


	// font header
	v_set_w ( ws, 0x00, f->glyfo );
	v_set_w ( ws, 0x04, f->traco );
	v_set_w ( ws, 0x08, f->cmapo );

	if ( hdr_ver == FONT_HEADER_FEDIT || hdr_ver == FONT_HEADER_DCT3 )
	{
		v_set_h ( ws, 0x0C, f->trac_len - 1 );
		v_set_h ( ws, 0x0E, f->cmap_len - 1 );
		v_set_h ( ws, 0x10, f->unk0 - 1 );
		v_set_h ( ws, 0x12, f->default_char );

		v_set_b ( ws, 0x14, f->flags[0] );
		v_set_b ( ws, 0x15, f->flags[1] );
		v_set_b ( ws, 0x16, f->flags[2] );
		v_set_b ( ws, 0x17, f->flags[3] );

		if ( f->id1 )
		{
			sprintf ( dump + 0x18, "%04X/%04X", f->id1, f->id2 );
			v_set_h ( ws, 0x21, 0 ); // SBZ
			sprintf ( dump + 0x23, "%04X/%04X", f->fb1, f->fb2 );
		}
		else
		{
			v_memcpy_put ( ws, 0x18, f->size, 11 );
			v_memcpy_put ( ws, 0x23, f->weight, 6 );
			v_set_b ( ws, 0x29, f->fb1 );
			v_set_h ( ws, 0x2A, 0 ); // SBZ
		}
	}
	else if ( hdr_ver == FONT_HEADER_8310 )
	{
		v_set_h ( ws, 0x0C, f->trac_len - 1 );
		v_set_h ( ws, 0x0E, f->cmap_len - 1 );
		v_set_h ( ws, 0x10, f->unk0 - 1 );
		v_set_h ( ws, 0x12, f->default_char );

		v_set_b ( ws, 0x14, f->flags[0] );
		v_set_b ( ws, 0x15, f->flags[1] );
		v_set_b ( ws, 0x16, f->flags[2] );
		v_set_b ( ws, 0x17, f->flags[3] );

		v_set_h ( ws, 0x18, f->id1 );
		v_set_h ( ws, 0x1A, f->id2 );
		v_set_h ( ws, 0x1C, f->fb1 );
		v_set_h ( ws, 0x1E, f->fb2 );
	}
	else if ( hdr_ver == FONT_HEADER_3100 )
	{
		v_set_h ( ws, 0x24, f->trac_len - 1 );
		v_set_h ( ws, 0x26, f->cmap_len - 1 );
		v_set_h ( ws, 0x28, f->unk0 - 1 );
		v_set_h ( ws, 0x2A, f->default_char );

		v_set_h ( ws, 0x2C, f->id1 );
		v_set_h ( ws, 0x2E, f->id2 );
		v_set_h ( ws, 0x30, f->fb1 );
		v_set_h ( ws, 0x32, f->fb2 );

		v_set_b ( ws, 0x34, f->flags[0] );
		v_set_b ( ws, 0x35, f->flags[1] );
		v_set_b ( ws, 0x36, f->flags[2] );
		v_set_b ( ws, 0x37, f->flags[3] );
	}
	else if ( hdr_ver == FONT_HEADER_DCT4 )
	{
		v_set_h ( ws, 0x0C, f->trac_len - 1 );
		v_set_h ( ws, 0x0E, f->cmap_len - 1 );
		v_set_h ( ws, 0x10, f->unk0 - 1 );
		v_set_h ( ws, 0x12, f->default_char );

		v_set_h ( ws, 0x14, f->id1 );
		v_set_h ( ws, 0x16, f->id2 );
		v_set_h ( ws, 0x18, f->fb1 );
		v_set_h ( ws, 0x1A, f->fb2 );

		v_set_b ( ws, 0x1C, f->flags[0] );
		v_set_b ( ws, 0x1D, f->flags[1] );
		v_set_b ( ws, 0x1E, f->flags[2] );
		v_set_b ( ws, 0x1F, f->flags[3] );
	}
	else
	{
		workspace_release ( ws );
		return NULL;
	}


	// cmaps table
	id = 0;
	pos = f->cmapo;
	while ( id < f->cmap_len ) 
	{
		v_set_h ( ws, pos + 0x00, f->cmap[id].start );

		f->cmap[id].offset	 &= 0x3FFFFF;
		f->cmap[id].height	 &= 0x1F;
		f->cmap[id].base_line &= 0x1F;

		if ( hdr_ver == FONT_HEADER_FEDIT || font_cmap_ver == FONT_CMAP_V2 )
		{
			v_set_b ( ws, pos + 0x02, f->cmap[id].end - f->cmap[id].start );
			v_set_b ( ws, pos + 0x03, f->cmap[id].gid );

			if ( ws->flags & FLAGS_ENDIANESS_BE )
			{
				tmp  = f->cmap[id].offset << 10;
				tmp |= f->cmap[id].height << 5;
				tmp |= f->cmap[id].base_line;
			}
			else
			{
				tmp  = f->cmap[id].offset;
				tmp |= f->cmap[id].height << 22;
				tmp |= f->cmap[id].base_line << 27;
			}
		}
		else // FONT_CMAP_V1
		{
			v_set_h ( ws, pos + 0x02, f->cmap[id].end );

			f->cmap[id].offset &= 0x3FFFF;
			f->cmap[id].gid	 &= 0x0F;

			tmp  = f->cmap[id].offset << 14;
			tmp |= f->cmap[id].gid << 10;
			tmp |= f->cmap[id].height << 5;
			tmp |= f->cmap[id].base_line;
		}

		v_set_w ( ws, pos + 0x04, tmp );

		pos += 8;
		id++;
	}

	// tracs table
	id = 0;
	pos = f->traco;
	while ( id < f->trac_len ) 
	{
		v_set_h ( ws, pos + 0x00, f->trac[id].start );
		v_set_h ( ws, pos + 0x02, f->trac[id].end );
		v_set_b ( ws, pos + 0x04, f->trac[id].left );
		v_set_b ( ws, pos + 0x05, f->trac[id].right );
		v_set_h ( ws, pos + 0x06, f->trac[id].pad );

		pos += 8;
		id++;
	}

	// glyphs table
	id = 0;
	pos = f->glyfo;
	bpos = f->glyfo + 12 * f->glyf_len;
	while ( id < f->glyf_len ) 
	{
		v_set_w ( ws, pos + 0x00, bpos - pos );
		v_set_h ( ws, pos + 0x04, f->glyf[id].unk );
		v_set_h ( ws, pos + 0x06, f->glyf[id].width );
		v_set_w ( ws, pos + 0x08, 0 ); // not important

		v_memcpy_put ( ws, bpos, f->glyf[id].data, f->glyf[id].bitlen );
		bpos = bpos + f->glyf[id].bitlen;

		pos += 12;
		id++;
	}

	alignedlength = ALIGN_WORD ( f->length );
	dump = malloc ( alignedlength );
	if ( !dump )
	{
		workspace_release ( ws );
		return NULL;
	}

	memcpy ( dump, ws->fileinfo->stages->segments->data, f->length );
	memset ( dump + f->length, alignment_byte, alignedlength - f->length );
	f->length = alignedlength;

	workspace_release ( ws );

	return dump;
}


unsigned int search_width ( t_font_glyf *gt, unsigned int glen, unsigned int width )
{
	unsigned int gid = 0;

	if ( !gt )
		return E_FAIL;

	/* this loop checks if such width was already seen */
	while ( gid < glen )
	{
		if ( gt[gid].width == width )
			return gid;
		gid++;
	}

	return E_FAIL;
}


t_font_glyf *ppmodify_font_glyf_dump ( t_workspace *ws, t_font_hdr *f )
{
	unsigned int cid = 0;
	unsigned int gid = 0;
	unsigned int pos = 0;
	unsigned int shift = 0;
	unsigned int k = 0;
	t_font_glyf g;
	t_font_glyf *gt = NULL;

	if ( !ws )
		return NULL;

	f->glyf_len = 0;
	cid = 0;
	while ( cid < f->cmap_len )
	{
		pos     = f->glyfo + 12 * f->cmap[cid].gid;
		g.bdt   = pos + v_get_w ( ws, pos );
		g.width =       v_get_h ( ws, pos + 6 );
		g.unk   =       v_get_h ( ws, pos + 4 );

		/* calculate bit shift, start and end address of bitmap data */
		shift     = f->cmap[cid].offset & 7;
		g.bitlen  = ( f->cmap[cid].end - f->cmap[cid].start + 1 ) * g.width * f->cmap[cid].height;
		g.bitlen += g.width * shift;

		if ( g.bitlen & 7 )
			g.bitlen = ( g.bitlen >> 3 ) + 1;
		else
			g.bitlen = g.bitlen >> 3;

		if ( g.bitlen % g.width )
			g.bitlen += g.width - ( g.bitlen % g.width );

		g.start = g.bdt + ( f->cmap[cid].offset >> 3 ) * g.width;
		g.end = g.start + g.bitlen;

		gid = search_width ( gt, f->glyf_len, g.width );
		if ( gid == E_FAIL ) /* this width is being used first time */
		{
			gid = f->glyf_len;
			f->glyf_len++; /* new width was added */
			gt = realloc ( gt, f->glyf_len * sizeof ( t_font_glyf ) );

			gt[gid].start   = g.start;
			gt[gid].end     = g.end;
			gt[gid].bitlen  = g.bitlen;
			gt[gid].width   = g.width;
			gt[gid].offset  = f->cmap[cid].offset;
			gt[gid].unk     = g.unk;

			/* new offset is just the shift since all data before it are being removed */
			f->cmap[cid].offset = shift;
		}
		else /* this width was added before */
		{
			/* are the new data before the already used? */
			if ( g.start < gt[gid].start )
			{
				/* we have to move the start address and base offset */
				shift = ( ( gt[gid].start - g.start ) /  g.width ) << 3;
				gt[gid].start   = g.start;
				gt[gid].offset -= shift;

				/* now we have to move offsets in all already processed char ranges */
				k = 0;
				while ( k < cid )
				{
					if ( f->cmap[k].gid == gid )
						f->cmap[k].offset += shift;
					k++;
				}
			}

			/* are the new data after the already used? */
			if ( g.end > gt[gid].end )
				gt[gid].end = g.end; /* just move the end address */

			gt[gid].bitlen = gt[gid].end - gt[gid].start;

			/* difference between original offsets lets us calculate the new offset */
			f->cmap[cid].offset = f->cmap[cid].offset - gt[gid].offset + ( gt[gid].offset & 7 );
		}

		/* update info */
		f->cmap[cid].gid = gid;

		cid++;
	}

	gid = 0;
	while ( gid < f->glyf_len )
	{
		gt[gid].data = malloc( gt[gid].bitlen );
		v_memcpy_get ( ws, gt[gid].data , gt[gid].start, gt[gid].bitlen );
		f->length += gt[gid].bitlen;
		gid++;
	}

	f->length += 12 * f->glyf_len;

	return gt;
}


t_font_trac *ppmodify_font_trac_dump ( t_workspace *ws, t_font_hdr *f )
{
	unsigned int entry = 0;
	unsigned int pos = f->traco;
	t_font_trac *tt = NULL;

	if ( !ws )
		return NULL;

	tt = calloc ( f->trac_len, sizeof ( t_font_trac ) );
	if ( !tt )
		return NULL;

	while ( entry < f->trac_len )
	{
		tt[entry].start   = v_get_h ( ws, pos );
		tt[entry].end     = v_get_h ( ws, pos + 2 );
		tt[entry].left    = v_get_b ( ws, pos + 4 );
		tt[entry].right   = v_get_b ( ws, pos + 5 );
		tt[entry].pad     = v_get_h ( ws, pos + 6 );

		pos += 8;
		entry++;
	}

	f->length += 8 * f->trac_len;

	return tt;
}


t_font_cmap *ppmodify_font_cmap_dump ( t_workspace *ws, t_font_hdr *f, unsigned int hdr_ver )
{
	unsigned int entry = 0;
	unsigned int pos = f->cmapo;
	unsigned int chars = 0;
	unsigned int tmp;
	t_font_cmap *ct = NULL;

	if ( !ws )
		return NULL;

	ct = calloc ( f->cmap_len, sizeof ( t_font_cmap ) );
	if ( !ct )
		return NULL;

	// check for CMAP version which differs even within phone generation
	if ( hdr_ver != FONT_HEADER_FEDIT && font_cmap_ver == FONT_CMAP_UNKNOWN )
	{
		int start_char = v_get_h ( ws, pos );
		int next_start_char = v_get_h ( ws, pos + 8 );
		int length = v_get_b ( ws, pos + 2 ) + 1;
		int end = v_get_h ( ws, pos + 2 );

		if ( start_char + length == next_start_char )
			font_cmap_ver = FONT_CMAP_V2;
		else if ( end + 1 == next_start_char )
			font_cmap_ver = FONT_CMAP_V1;
		else if ( end + 1 == v_get_h ( ws, pos + 12 ) )
			font_cmap_ver = FONT_CMAP_V3;
		else
		{
			printf ( "Warning! Could not detect FONT CMAP version. Will default to the common CMAP version.\r\n" );
			font_cmap_ver = FONT_CMAP_V2;
		}
	}

	f->default_height = 0;
	while ( entry < f->cmap_len )
	{
		ct[entry].start = v_get_h ( ws, pos );
		tmp             = v_get_w ( ws, pos + 4 );

		if ( hdr_ver == FONT_HEADER_FEDIT || font_cmap_ver == FONT_CMAP_V2 )
		{
			ct[entry].end = ct[entry].start + v_get_b ( ws, pos + 2 );
			ct[entry].gid = v_get_b ( ws, pos + 3 );

			// bit fields sucks
			if ( ws->flags & FLAGS_ENDIANESS_BE )
			{
				ct[entry].offset     = tmp >> 10;
				ct[entry].height     = ( tmp >> 5 ) & 0x1F;
				ct[entry].base_line  = tmp & 0x1F;
			}
			else
			{
				ct[entry].offset     = tmp & 0x3FFFFF;
				ct[entry].height     = ( tmp >> 22 ) & 0x1F;
				ct[entry].base_line  = ( tmp >> 27 );
			}
		}
		else if ( font_cmap_ver == FONT_CMAP_V1 )
		{
			ct[entry].end        = v_get_h ( ws, pos + 2 );

			ct[entry].offset     = tmp >> 14;
			ct[entry].gid        = (tmp >> 10 ) & 0x0F;
			ct[entry].height     = (tmp >> 5 ) & 0x1F;
			ct[entry].base_line  = tmp & 0x1F;
		}
		else
			PLUGIN_ERROR ( "Invalid CMAP version detected!", NULL );

		chars += ct[entry].end - ct[entry].start + 1;
		f->default_height += ct[entry].height * ( ct[entry].end - ct[entry].start +1 );
		pos += 8;
		entry++;
	}

	f->default_height /= chars;
	f->length += 8 * f->cmap_len;

	return ct;
}


t_font_hdr *ppmodify_font_hdr_dump ( t_workspace *ws, unsigned int start, unsigned int length, unsigned int hdr_ver )
{
	t_font_hdr *f = NULL;
	unsigned int pos = start;
	unsigned int entry = 0;
	unsigned int entries = 0;

	if ( !ws )
		return NULL;

	if ( hdr_ver == FONT_HEADER_FEDIT )
		entries = 1;
	else
	{
		entries = v_get_w ( ws, start );
		pos += 4;
	}

	f = calloc ( entries, sizeof ( t_font_hdr ) );
	if ( !f )
		return NULL;


	if ( hdr_ver == FONT_HEADER_FEDIT || hdr_ver == FONT_HEADER_DCT3 )
	{
		while ( entry < entries && v_valid ( ws, pos + FONT_HEADER_DCT3_SIZE ) && (pos - start) < length )
		{
			f[entry].glyfo = pos + v_get_w ( ws, pos );
			f[entry].traco = pos + v_get_w ( ws, pos + 4 );
			f[entry].cmapo = pos + v_get_w ( ws, pos + 8 );

			f[entry].trac_len  = v_get_h ( ws, pos + 0x0C ) + 1;
			f[entry].cmap_len  = v_get_h ( ws, pos + 0x0E ) + 1;

			f[entry].unk0   = v_get_h ( ws, pos + 0x10 ) + 1;

			f[entry].default_char = v_get_h ( ws, pos + 0x12 );

			f[entry].flags[0]     = v_get_b ( ws, pos + 0x14 );
			f[entry].flags[1]     = v_get_b ( ws, pos + 0x15 );
			f[entry].flags[2]     = v_get_b ( ws, pos + 0x16 );
			f[entry].flags[3]     = v_get_b ( ws, pos + 0x17 );

			if ( !v_get_h ( ws, pos + 0x2A ) ) // dct3
			{
				v_memcpy_get ( ws, f[entry].size, pos + 0x18 , 11 );
				v_memcpy_get ( ws, f[entry].weight, pos + 0x23 , 6 );
				f[entry].fb1  = v_get_b ( ws, pos + 0x29 );

			}
			else
			{
				char tmp[10];
				v_memcpy_get ( ws, tmp, pos + 0x18 , 10 );
				sscanf ( tmp, "%04X/%04X", &f[entry].id1, &f[entry].id2 );
				v_memcpy_get ( ws, tmp, pos + 0x23 , 10 );
				sscanf ( tmp, "%04X/%04X", &f[entry].fb1, &f[entry].fb2 );
			}

			f[entry].length = 0;
			f[entry].cmap = ppmodify_font_cmap_dump ( ws, &f[entry], hdr_ver );
			f[entry].trac = ppmodify_font_trac_dump ( ws, &f[entry] );
			f[entry].glyf = ppmodify_font_glyf_dump ( ws, &f[entry] );

			pos += FONT_HEADER_DCT3_SIZE;
			entry++;
		}
	}
	else if ( hdr_ver == FONT_HEADER_8310 )
	{
		while ( entry < entries && v_valid ( ws, pos + FONT_HEADER_8310_SIZE ) && (pos - start) < length )
		{
			f[entry].glyfo = pos + v_get_w ( ws, pos );
			f[entry].traco = pos + v_get_w ( ws, pos + 4 );
			f[entry].cmapo = pos + v_get_w ( ws, pos + 8 );

			f[entry].trac_len     = v_get_h ( ws, pos + 0x0C ) + 1;
			f[entry].cmap_len     = v_get_h ( ws, pos + 0x0E ) + 1;
			f[entry].unk0         = v_get_h ( ws, pos + 0x10 ) + 1;

			f[entry].default_char = v_get_h ( ws, pos + 0x12 );

			f[entry].flags[0]     = v_get_b ( ws, pos + 0x14 );
			f[entry].flags[1]     = v_get_b ( ws, pos + 0x15 );
			f[entry].flags[2]     = v_get_b ( ws, pos + 0x16 );
			f[entry].flags[3]     = v_get_b ( ws, pos + 0x17 );

			f[entry].id1          = v_get_h ( ws, pos + 0x18 );
			f[entry].id2          = v_get_h ( ws, pos + 0x1A );
			f[entry].fb1          = v_get_h ( ws, pos + 0x1C );
			f[entry].fb2          = v_get_h ( ws, pos + 0x1E );

			f[entry].length = 0;
			f[entry].cmap = ppmodify_font_cmap_dump ( ws, &f[entry], hdr_ver );
			f[entry].trac = ppmodify_font_trac_dump ( ws, &f[entry] );
			f[entry].glyf = ppmodify_font_glyf_dump ( ws, &f[entry] );

			pos += FONT_HEADER_8310_SIZE;
			entry++;
		}
	}
	else if ( hdr_ver == FONT_HEADER_3100 )
	{
		while ( entry < entries && v_valid ( ws, pos + FONT_HEADER_3100_SIZE ) && (pos - start) < length )
		{
			f[entry].glyfo = pos + v_get_w ( ws, pos );
			f[entry].traco = pos + v_get_w ( ws, pos + 4 );
			f[entry].cmapo = pos + v_get_w ( ws, pos + 8 );

			f[entry].trac_len     = v_get_h ( ws, pos + 0x24 ) + 1;
			f[entry].cmap_len     = v_get_h ( ws, pos + 0x26 ) + 1;
			f[entry].unk0         = v_get_h ( ws, pos + 0x28 ) + 1;

			f[entry].default_char = v_get_h ( ws, pos + 0x2A );

			f[entry].id1          = v_get_h ( ws, pos + 0x2C );
			f[entry].id2          = v_get_h ( ws, pos + 0x2E );
			f[entry].fb1          = v_get_h ( ws, pos + 0x30 );
			f[entry].fb2          = v_get_h ( ws, pos + 0x32 );

			f[entry].flags[0]     = v_get_b ( ws, pos + 0x34 );
			f[entry].flags[1]     = v_get_b ( ws, pos + 0x35 );
			f[entry].flags[2]     = v_get_b ( ws, pos + 0x36 );
			f[entry].flags[3]     = v_get_b ( ws, pos + 0x37 );

			f[entry].length = 0;
			f[entry].cmap = ppmodify_font_cmap_dump ( ws, &f[entry], hdr_ver );
			f[entry].trac = ppmodify_font_trac_dump ( ws, &f[entry] );
			f[entry].glyf = ppmodify_font_glyf_dump ( ws, &f[entry] );

			pos += FONT_HEADER_3100_SIZE;
			entry++;
		}
	}
	else if ( hdr_ver == FONT_HEADER_DCT4 )
	{
		while ( entry < entries && v_valid ( ws, pos + FONT_HEADER_DCT4_SIZE ) && (pos - start) < length )
		{
			f[entry].glyfo = pos + v_get_w ( ws, pos );
			f[entry].traco = pos + v_get_w ( ws, pos + 4 );
			f[entry].cmapo = pos + v_get_w ( ws, pos + 8 );

			f[entry].trac_len  = v_get_h ( ws, pos + 0x0C ) + 1;
			f[entry].cmap_len  = v_get_h ( ws, pos + 0x0E ) + 1;
			f[entry].unk0      = v_get_h ( ws, pos + 0x10 ) + 1;

			f[entry].default_char = v_get_h ( ws, pos + 0x12 );

			f[entry].id1          = v_get_h ( ws, pos + 0x14 );
			f[entry].id2          = v_get_h ( ws, pos + 0x16 );
			f[entry].fb1          = v_get_h ( ws, pos + 0x18 );
			f[entry].fb2          = v_get_h ( ws, pos + 0x1A );

			f[entry].flags[0]     = v_get_b ( ws, pos + 0x1C );
			f[entry].flags[1]     = v_get_b ( ws, pos + 0x1D );
			f[entry].flags[2]     = v_get_b ( ws, pos + 0x1E );
			f[entry].flags[3]     = v_get_b ( ws, pos + 0x1F );

			f[entry].length = 0;
			f[entry].cmap = ppmodify_font_cmap_dump ( ws, &f[entry], hdr_ver );
			f[entry].trac = ppmodify_font_trac_dump ( ws, &f[entry] );
			f[entry].glyf = ppmodify_font_glyf_dump ( ws, &f[entry] );

			pos += FONT_HEADER_DCT4_SIZE;
			entry++;
		}
	}
	else
		return NULL;

	return f;
}


unsigned int ppmodify_font_subchunk_dump_fonts ( t_treenode *headernode, t_workspace *ws, unsigned int start, unsigned int length )
{
	t_treenode *node1 = NULL;
	t_treenode *node2 = NULL;
	t_treenode *node3 = NULL;
	t_treenode *node4 = NULL;
	t_font_hdr *f = NULL;
	unsigned char *dump = NULL;
	unsigned int entries = 0;
	unsigned int entry = 0;
	unsigned int id = 0;


	if ( !length || !headernode || !ws )
		return E_FAIL;

	entries = v_get_w ( ws, start );

	f = ppmodify_font_hdr_dump ( ws, start, length, font_header_ver );
	if ( !f )
		return E_FAIL;

	while ( entry < entries )
	{
		node1 = treenode_addchild ( headernode );
		treenode_set_name ( node1, "FONT" );

		// INFORMATION
		node2 = treenode_addchild ( node1 );
		treenode_set_name ( node2, "INFORMATION" );

		if ( font_header_ver == FONT_HEADER_DCT3 )
		{
			node3 = treenode_addchild ( node2 );
			treenode_set_name ( node3, "SIZE" );
			treenode_set_content_data ( node3, f[entry].size );

			node3 = treenode_addchild ( node2 );
			treenode_set_name ( node3, "WEIGHT" );
			treenode_set_content_data ( node3, f[entry].weight );

			node3 = treenode_addchild ( node2 );
			treenode_set_name ( node3, "FB" );
			treenode_set_content_data_hexval ( node3, f[entry].fb1 );
		}
		else
		{
			node3 = treenode_addchild ( node2 );
			treenode_set_name ( node3, "ID1" );
			treenode_set_content_data_hexval ( node3, f[entry].id1 );

			node3 = treenode_addchild ( node2 );
			treenode_set_name ( node3, "ID2" );
			treenode_set_content_data_hexval ( node3, f[entry].id2 );

			node3 = treenode_addchild ( node2 );
			treenode_set_name ( node3, "FB1" );
			treenode_set_content_data_hexval ( node3, f[entry].fb1 );

			node3 = treenode_addchild ( node2 );
			treenode_set_name ( node3, "FB2" );
			treenode_set_content_data_hexval ( node3, f[entry].fb2 );
		}

			node3 = treenode_addchild ( node2 );
			treenode_set_name ( node3, "DEFAULT_HEIGHT" );
			treenode_set_content_data_hexval ( node3, f[entry].default_height );

			node3 = treenode_addchild ( node2 );
			treenode_set_name ( node3, "DEFAULT_CHAR" );
			treenode_set_content_data_hexval ( node3, f[entry].default_char );

			node3 = treenode_addchild ( node2 );
			treenode_set_name ( node3, "FLAGS1" );
			treenode_set_content_data_hexval ( node3, f[entry].flags[0] );

			node3 = treenode_addchild ( node2 );
			treenode_set_name ( node3, "FLAGS2" );
			treenode_set_content_data_hexval ( node3, f[entry].flags[1] );

			node3 = treenode_addchild ( node2 );
			treenode_set_name ( node3, "FLAGS3" );
			treenode_set_content_data_hexval ( node3, f[entry].flags[2] );

			node3 = treenode_addchild ( node2 );
			treenode_set_name ( node3, "FLAGS4" );
			treenode_set_content_data_hexval ( node3, f[entry].flags[3] );


		// CMAPS
		node2 = treenode_addchild ( node1 );
		treenode_set_name ( node2, "CMAPS" );

		id = 0;
		while ( id < f[entry].cmap_len )
		{
			node3 = treenode_addchild ( node2 );
			treenode_set_name ( node3, "CMAP" );

				node4 = treenode_addchild ( node3 );
				treenode_set_name ( node4, "START" );
				treenode_set_content_data_hexval ( node4, f[entry].cmap[id].start );

				node4 = treenode_addchild ( node3 );
				treenode_set_name ( node4, "END" );
				treenode_set_content_data_hexval ( node4, f[entry].cmap[id].end );

				node4 = treenode_addchild ( node3 );
				treenode_set_name ( node4, "GLYPH_ID" );
				treenode_set_content_data_hexval ( node4, f[entry].cmap[id].gid );

				node4 = treenode_addchild ( node3 );
				treenode_set_name ( node4, "OFFSET" );
				treenode_set_content_data_hexval ( node4, f[entry].cmap[id].offset );

				node4 = treenode_addchild ( node3 );
				treenode_set_name ( node4, "HEIGHT" );
				treenode_set_content_data_hexval ( node4, f[entry].cmap[id].height );

				node4 = treenode_addchild ( node3 );
				treenode_set_name ( node4, "BASE_LINE" );
				treenode_set_content_data_hexval ( node4, f[entry].cmap[id].base_line );

			id++;
		}


		// TRACS
		node2 = treenode_addchild ( node1 );
		treenode_set_name ( node2, "TRACS" );

		id = 0;
		while ( id < f[entry].trac_len )
		{
			node3 = treenode_addchild ( node2 );
			treenode_set_name ( node3, "TRAC" );

				node4 = treenode_addchild ( node3 );
				treenode_set_name ( node4, "START" );
				treenode_set_content_data_hexval ( node4, f[entry].trac[id].start );

				node4 = treenode_addchild ( node3 );
				treenode_set_name ( node4, "END" );
				treenode_set_content_data_hexval ( node4, f[entry].trac[id].end );

				node4 = treenode_addchild ( node3 );
				treenode_set_name ( node4, "LEFT" );
				treenode_set_content_data_hexval ( node4, f[entry].trac[id].left );

				node4 = treenode_addchild ( node3 );
				treenode_set_name ( node4, "RIGHT" );
				treenode_set_content_data_hexval ( node4, f[entry].trac[id].right );

			id++;
		}


		// GLYPHS
		node2 = treenode_addchild ( node1 );
		treenode_set_name ( node2, "GLYPHS" );

		id = 0;
		while ( id < f[entry].glyf_len )
		{
			node3 = treenode_addchild ( node2 );
			treenode_set_name ( node3, "GLYPH" );

				node4 = treenode_addchild ( node3 );
				treenode_set_name ( node4, "WIDTH" );
				treenode_set_content_data_hexval ( node4, f[entry].glyf[id].width );

				node4 = treenode_addchild ( node3 );
				treenode_set_name ( node4, "BITMAP" );
				treenode_set_content_data_binary ( node4, f[entry].glyf[id].data, f[entry].glyf[id].bitlen );

			if ( font_build_textdump )
			{
				dump = ppmodify_font_maketext ( &f[entry].glyf[id] );

				node4 = treenode_addchild ( node3 );
				treenode_set_name ( node4, "TEXTDUMP" );
				treenode_set_content_data ( node4, dump );

				free ( dump );
			}

			id++;
		}

		dump = ppmodify_build_font_dump ( &f[entry], FONT_HEADER_FEDIT );
		if ( dump )
		{
			node2 = treenode_addchild ( node1 );
			treenode_set_name ( node2, "DUMP" );
			treenode_set_content_data_binary ( node2, dump, f[entry].length );
			free ( dump );
		}

		entry++;
	}


	free_font_hdr ( f, entries );

	return E_OK;
}


unsigned int ppmodify_font_subchunk_dump ( t_workspace *ws, unsigned int start, unsigned int length, t_treenode *node )
{
	subchunk_hdr header;
	t_treenode *headernode = NULL;
	t_treenode *worknode = NULL;
	unsigned hdr_size = 0;

	if ( !ws || !node || !v_valid ( ws, start ) )
		PLUGIN_ERROR ( "( !ws || !node || !v_valid ( ws, start ) ) failed", E_FAIL );

	header.id = v_get_w ( ws, start + 0x00 );
	header.length = v_get_w ( ws, start + 0x04 );
	v_memcpy_get ( ws, header.name, start + 0x08, 4 );
	header.flags1 = v_get_b ( ws, start + 0x0C );
	header.flags2 = v_get_b ( ws, start + 0x0D );
	header.flags3 = v_get_b ( ws, start + 0x0E );
	header.flags4 = v_get_b ( ws, start + 0x0F );

	if ( !header.id ) // terminator
		return ppmodify_ppm_subchunk_dump ( ws, start, length, node );

	if ( header.length < 0x10 )
		header.length = 0x10;

	if ( header.length > length )
		PLUGIN_ERROR ( "header.length > length", E_FAIL );

	if ( header.length > v_get_size ( ws ) )
		PLUGIN_ERROR ( "header.length > filesize", E_FAIL );

	if ( v_get_w ( ws, start + 0x10 ) == 1 )
		hdr_size = v_get_w ( ws, start + 0x1C );
	else if (
		v_get_b ( ws, start + 0x28 ) == 0x07 &&
		v_get_b ( ws, start + 0x29 ) == 0x80 &&
		v_get_b ( ws, start + 0x2A ) == 0x00 &&
		v_get_b ( ws, start + 0x2B ) == 0x00
		)
	{
		if (
			v_get_b ( ws, start + 0x48 ) == 0x07 &&
			v_get_b ( ws, start + 0x49 ) == 0x80 &&
			v_get_b ( ws, start + 0x4A ) == 0x00 &&
			v_get_b ( ws, start + 0x4B ) == 0x00
			)
		{
			// set CMAP version to unknown for now, will get autodetected
			font_cmap_ver = FONT_CMAP_UNKNOWN;
			font_header_ver = FONT_HEADER_8310;
		}
		else
		{
			// set CMAP version v1
			font_cmap_ver = FONT_CMAP_V1;
			font_header_ver = FONT_HEADER_DCT3;
		}
	}
	else if (
		v_get_b ( ws, start + 0x30 ) == 0x07 &&
		v_get_b ( ws, start + 0x31 ) == 0x80 &&
		v_get_b ( ws, start + 0x32 ) == 0x00 &&
		v_get_b ( ws, start + 0x33 ) == 0x00
		)
	{
		// set CMAP version v2
		font_cmap_ver = FONT_CMAP_V2;
		font_header_ver = FONT_HEADER_DCT4;
	}
	else if (
		v_get_b ( ws, start + 0x48 ) == 0x07 &&
		v_get_b ( ws, start + 0x49 ) == 0x80 &&
		v_get_b ( ws, start + 0x4A ) == 0x00 &&
		v_get_b ( ws, start + 0x4B ) == 0x00
		)
	{
		// set CMAP version v2
		font_cmap_ver = FONT_CMAP_UNKNOWN;
		font_header_ver = FONT_HEADER_3100;
	}
	else
	{
		PLUGIN_ERROR ( "Could not detect FONT chunk version!\r\n", NULL );
	}

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
	treenode_set_name ( headernode, "FONTS" );
	ppmodify_font_subchunk_dump_fonts ( headernode, ws, start + 0x10, header.length - 0x10 );

	return E_OK;
}

unsigned int ppmodify_build_font_subchunk ( t_treenode *node )
{
	subchunk_hdr header;
	t_workspace *wsi = NULL;
	t_workspace *wso = NULL;
	t_treenode *infonode = NULL;
	t_treenode *datanode = NULL;
	t_treenode *worknode = NULL;
	t_font_hdr *f = NULL;
	unsigned char *datai = NULL;
	unsigned char *datao = NULL;
	unsigned char *dump = NULL;
	unsigned int font_count = 0;
	unsigned int datai_size = 0;
	unsigned int datao_size = 0;
	unsigned int datai_pos = 0;
	unsigned int datao_pos = 0;
	unsigned int id = 0;
	unsigned int hdr_size = 0;
	unsigned int reloc = 0;

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

	if ( !header.id ) // terminator
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

	worknode = treenode_children ( treenode_get_byname ( treenode_children ( node ), "FONTS" ) );
	font_count = treenode_count ( worknode );

	switch ( font_header_ver )
	{
		case FONT_HEADER_DCT3:
			hdr_size = FONT_HEADER_DCT3_SIZE;
			break;

		case FONT_HEADER_8310:
			hdr_size = FONT_HEADER_8310_SIZE;
			break;

		case FONT_HEADER_3100:
			hdr_size = FONT_HEADER_3100_SIZE;
			break;

		case FONT_HEADER_DCT4:
			hdr_size = FONT_HEADER_DCT4_SIZE;
			break;

		default:
			return E_FAIL;
	}

	datao_size = 0x10 + 0x04 + hdr_size * font_count; // subchunk header size + 4 bytes font_count
	datao_pos = datao_size;

	datao = calloc ( datao_size, sizeof ( unsigned char ) );
	if ( !datao )
		return E_FAIL;

	wso = workspace_create_file_from_buffer ( datao, datao_size );
	if ( !wso )
		PLUGIN_ERROR ( "failed creating temporary workspace", E_FAIL );

	wso->flags &= ~FLAGS_ENDIANESS;
	if ( endianess_be )
		wso->flags |= FLAGS_ENDIANESS_BE;
	else
		wso->flags |= FLAGS_ENDIANESS_LE;

	v_set_w ( wso, 0x10, font_count );

	LIST_FFWD ( worknode );
	id = font_count - 1;

	while ( worknode )
	{
		datai = treenode_get_content_data_binary ( treenode_get_byname ( worknode, "DUMP" ), &datai_size );

		wsi = workspace_create_file_from_buffer ( datai, datai_size );
		wsi->flags &= ~FLAGS_ENDIANESS;
		wsi->flags |= FLAGS_ENDIANESS_BE;

		f = ppmodify_font_hdr_dump ( wsi, 0, datai_size, FONT_HEADER_FEDIT );
		if ( !f )
			return E_FAIL;

		dump = ppmodify_build_font_dump ( f, font_header_ver );
		if ( dump )
		{
			datao_size += f->length;
			datao = realloc ( datao, datao_size );
			memcpy ( datao + 0x14 + id * hdr_size, dump, hdr_size ); // header
			memcpy ( datao + datao_pos, dump + hdr_size, f->length - hdr_size ); // data

			wso->fileinfo->stages->segments->data = datao;
			wso->fileinfo->stages->segments->length = datao_size;
			wso->fileinfo->stages->segments->end = datao_size;
			workspace_update_memmap ( wso );

			// update table offsets
			v_set_w ( wso, 0x14 + id * hdr_size + 0x00, reloc + v_get_w ( wso, 0x14 + id * hdr_size + 0x00 ) );
			v_set_w ( wso, 0x14 + id * hdr_size + 0x04, reloc + v_get_w ( wso, 0x14 + id * hdr_size + 0x04 ) );
			v_set_w ( wso, 0x14 + id * hdr_size + 0x08, reloc + v_get_w ( wso, 0x14 + id * hdr_size + 0x08 ) );

			datao_pos += f->length - hdr_size;
			reloc += f->length;

			free ( dump );
		}

		workspace_release ( wsi );
		LIST_PREV ( worknode );
		id--;
	}


	v_set_w ( wso, 0x00, header.id );

	if ( header.length != datao_size )
		printf ( " [i] SubChunk '%s' length changed from %i to %i\n", header.name, header.length, datao_size );

	v_set_w ( wso, 0x04, datao_size );
	v_memcpy_put ( wso, 0x08, header.name, 4 );
	v_set_b ( wso, 0x0C, header.flags1 );
	v_set_b ( wso, 0x0D, header.flags2 );
	v_set_b ( wso, 0x0E, header.flags3 );
	v_set_b ( wso, 0x0F, header.flags4 );


	// free subnodes
	treenode_release ( infonode );
	treenode_release ( treenode_parent ( worknode ) );

	// replace with constructed data
	treenode_set_name ( node, "SUBCHUNK-BINARY" );
	treenode_set_content_data_binary ( node, datao, datao_size );

	workspace_release ( wso );
	return E_OK;
}

#endif /* __TRIX_CHUNK_FONT_C__ */
