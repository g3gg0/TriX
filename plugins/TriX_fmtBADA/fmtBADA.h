#ifndef __TRIX_FMTBADA_H__
#define __TRIX_FMTBADA_H__

#include "defines.h"

#define BYTE(data,pos)                  (*(unsigned char *)((data)+(pos)))
#define HALF(data,pos)                  (*(unsigned short *)((data)+(pos)))
#define WORD(data,pos)                  (*(unsigned int *)((data)+(pos)))


typedef struct s_bada_file
{
	unsigned int start;   // offset
	unsigned int length;
	char name[24];        // AMSS, DBL, FSBL, ...
} t_bada_file;

typedef struct s_bada_main
{
	unsigned int magic;      // 12345678
	unsigned int hdr_count;  // cafebabe headers count ( without main )
	unsigned int unk0[6];    // SBZ
	t_bada_file files[];
} t_bada_main;

typedef struct s_bada_csc_file
{
	unsigned int magic;      // CAFEBABE
	unsigned int index;      // 0,1,2
	unsigned int unk0;
	unsigned int start;      // offset
	unsigned int unk1;
	unsigned int unk2;
	unsigned int length;
	unsigned int unk3;
	wchar_t path[240];       // '\CSC.rcs', '\CSC.sfs'
} t_bada_csc_file;

typedef struct s_bada_csc_main
{
	unsigned int magic;      // CAFEBABE
	unsigned int data_end;   // file offset to "cafe" string (4 bytes before footer)
	unsigned int data_start; // file offset
	unsigned int unk0;
	unsigned int hdr_count;  // cafebabe headers count ( without main )
	unsigned int hdr_size;   // 0x200
	unsigned int align;      // equal to data_start
	unsigned int unk1;
	wchar_t path[240];       // SBZ
	t_bada_csc_file files[];
} t_bada_csc_main;


typedef struct s_bada_footer
{
	unsigned int magic;         // MAGIC_BADA
	unsigned int start;         // start address
	unsigned int unk0;          // SBZ
	unsigned char name[32];     // S8500, Lismore, etc.
	unsigned char extension[8]; // bin, rc1, mbn, ...
	unsigned int unk1;          // 6, 0
	unsigned int md5;           // 1 - disable, 2 - enable
	unsigned int unk2;          // 2
	unsigned int unk3;          // 0x800
	unsigned int unk4;          // 0x20000
	unsigned int magic_seed;    // MAGIC_SEED
	unsigned char unk6[0x40];   // diff
	unsigned char unk7[0x40];   // same
	unsigned int unk8;          // SBZ
	unsigned char unk9[0x40];   // diff
	unsigned int seed_key[32];
	unsigned int magic_seed2;   // MAGIC_SEED
	unsigned int seed_length;   // crypted data length
	unsigned char unk12[0x50];  // diff
	unsigned char fw_ver[64];   // S8500+XX+JEE, ...
	unsigned char tool_ver[32]; // TkToolVer...
	unsigned char md5_sum[16];
	unsigned char padd[0x1A8];  // SBZ
} t_bada_footer;


typedef struct s_bada_priv
{
	STRUCT_HEADER_LIST(struct s_bada_priv);
	unsigned int header_len;
	unsigned char *header;
	t_bada_footer footer;
} t_bada_priv;


#define MAGIC_BADA      0xABCDABCD
#define MAGIC_SEED      0x79461379
#define MAGIC_CAFE      0x65666163 // cafe
#define MAGIC_FS        0x3A424344 // DCB:
#define MAGIC_QMD       0x00444D51 // QMD

#define HDR_BADA        0x12345678
#define HDR_CAFE        0xCAFEBABE

#define FB_SEED         0x00000001 // crypted
#define FB_QMD          0x00000002 // compressed
#define FB_FS           0x00000004 // FS image

#define S8500v12        0x00000001
#define S8530v12        0x00000002
#define S8500v20        0x00000003
#define S8530v20        0x00000004
#define S8600           0x00000005
#define S7250D          0x00000006

#define BADA_APPS       0x10000000
#define BADA_RSRC1      0x20000000
#define BADA_RSRCS      0x40000000

#define DECOMP_CODE1    0x0305
#define DECOMP_DATA1    0x0407
#define DECOMP_CODE2    0x0402
#define DECOMP_DATA2    0x0502


/*
	FOOTER:
	 0x00 [04]: MAGIC
	 0x04 [04]: Start adress
	 0x08 [04]: unk(0)
	 0x0C [32]: Name(S8500)
	 0x2C [08]  extension

	HDR_200:
	 0x00 [04]: HDR_200 MAGIC
	 0x04 [04]: object count
	 0x08 [24]: padding[0]

	 offset   size     name
	 00020000 B44E0000 44424C0000000000 00000000000000000000000000000000
	 B4500000 D4D60200 4653424C00000000 00000000000000000000000000000000
	 88270300 FC230100 4F53424C00000000 00000000000000000000000000000000

	HDR_FS:
	 0x00 [32]: Name (UCS-2)
	 0x20 [56]: unk
	 0x58 [08]: padd (FF)

	DCB files (in FS):
	 0x000 [0004]: DCB MAGIC ('DCB:')
	 0x004 [0004]: type ( 0 - directory, 1 - file )
	 0x008 [0004]: size ( without file name )
	 0x00C [0004]: flags ( meaning yet unknown )
	 0x010 [2048]: Name (UCS-2)
	 0x810 [size]: file content


	[NAME]          [START]         [EXTENSION]     [HDR_LEN]       [COMPRESSED]    [COMMENT]

	boot_loader     0x00000000      mbn             0               +               bootloader
	dbl             0x00000000      mbn             0x200           -               baseband bootloader

	Amss            0x00000000      bin             0x200           -               ELF object
	Apps            0x00000000      bin             0               +
	Rsrc1           0x00000000      rc1             0               -
	Rsrc2           0x00000000      rc2             0               -
	fota            0x00000000      fota            0               -
	Factory FS      0x00000000      ffs             0x60            -               FS
	Partial FS      0x00000000      pfs             0x60            -               FS
	ShpApp          0x00000000      app             0x60            -               FS
	CSC             0x00000000      csc             0               -
*/

// =============================================================================


#define FMTBADA_HEADERS \
	"import unsigned int __fmt_bada_reinit ( t_workspace *ws );\n"

#define FMTBADA_SYMBOLS \
	REGISTER_SYMBOL ( fmt_bada_reinit );


unsigned int fmtBADA_init ( );
unsigned int fmtBADA_cleanup ( );
unsigned char *fmtBADA_lasterror ( );


// fmtBADA.c


// seed.c
unsigned char *seed_decrypt ( unsigned char *data, unsigned int *seed_key, unsigned int length );
unsigned char *seed_encrypt ( unsigned char *data, unsigned int *seed_key, unsigned int length );

// qmd.c
unsigned int qmd_parser ( unsigned char *dst, unsigned char *src );
unsigned int qmd_get_header ( unsigned char *src );
unsigned int qmd_get_ver ( unsigned char *src );
unsigned int qmd_get_size ( unsigned char *src );

#endif /* __TRIX_FMTBADA_H__ */
