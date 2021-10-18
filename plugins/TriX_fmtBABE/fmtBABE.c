#ifndef __TRIX_FMTBABE_C__
#define __TRIX_FMTBABE_C__

#include "defines.h"
#include "seer.h"
#include "segment.h"
#include "stage.h"
#include "file_io.h"
#include "file.h"
#include "workspace.h"
#include "util.h"
#include "fmt.h"
#include "symbols.h"
#include "options.h"
#include "mem.h"
#include "treenode.h"
#include "ui.h"
#include "crypto.h"

#include "trixplug.h"

#define SEPARATOR "\n----------------------------------------------------\n\n"

typedef struct s_babe_list t_babe_list;
struct s_babe_list
{
	STRUCT_HEADER_LIST(t_babe_list);
	unsigned int start;
   unsigned int length;
   unsigned char *data;
};

unsigned char *last_error_string = "None";

t_babe_list *babe_backup = NULL;
char *trix_path    = NULL;
char *platform     = NULL;
char *patch_lines  = NULL;
unsigned int color = 0;
unsigned int cid   = 0;


unsigned int fmtBABE_init ( );
unsigned int fmtBABE_cleanup ( );
unsigned char *fmtBABE_lasterror ( );

// include all the TriXPluG function/init stuff
TRIXPLUG_STUBS
#include "trixplug_wrapper.h"

#define FMTBABE_HEADERS \
	"import unsigned int __fmt_babe_create_patch ( t_workspace *ws );\n" \
   "import unsigned int __fmt_babe_compile_code ( t_workspace *ws, const char *path, const char *file );\n" \
	"import unsigned int __fmt_babe_symbol_add ( const char *name, unsigned int swi, unsigned int address );\n" \
	"import unsigned int __fmt_babe_backup_data ( t_workspace *ws, unsigned int address, unsigned int length );\n"\
	"import unsigned int __fmt_babe_add_script_line ( const char *line );\n" \
	"import unsigned int __fmt_babe_reinit ( t_workspace *ws );\n"

#define FMTBABE_SYMBOLS \
   REGISTER_SYMBOL ( fmt_babe_create_patch ); \
   REGISTER_SYMBOL ( fmt_babe_compile_code ); \
   REGISTER_SYMBOL ( fmt_babe_symbol_add ); \
   REGISTER_SYMBOL ( fmt_babe_backup_data ); \
   REGISTER_SYMBOL ( fmt_babe_add_script_line ); \
   REGISTER_SYMBOL ( fmt_babe_reinit );


#define FMTBABE_TITLE "fmtBABE"
#define FMTBABE_MAJOR 0
#define FMTBABE_MINOR 54

PLUGIN_INFO_BEGIN
	PLUGIN_NAME ( FMTBABE_TITLE )
	PLUGIN_VERSION ( FMTBABE_MAJOR, FMTBABE_MINOR )
	PLUGIN_INIT ( fmtBABE_init )
	PLUGIN_CLEANUP ( fmtBABE_cleanup )
	PLUGIN_LASTERROR ( fmtBABE_lasterror )
PLUGIN_INFO_FINISH


#define SWI_THUMB 0x10000000
#define SWI_ARM   0x20000000
#define SWI_DATA  0x40000000


unsigned short fmt_babe_checksum ( unsigned char *data, unsigned int length )
{
	unsigned short chk = 0;

	while ( length )
		chk += data[--length];

	return chk;
}

/*
   DB2000 - K608 K600 V600 V800 W900 Z800 Z1010
   DB2010 - D750 F500 J300 K300 K310 K500 K508 K510 K700 K750 S600 S700 S710 W300 W550 W600 W700 W800 W810 Z500 Z520 Z525 Z530 Z550 Z558
   DB2012 - K320 W200
   DB2020 - K530 K550 K610 K618 K770 K790 K800 K810 S500 T650 V630 W580 W610 W660 W710 W830 W850 W880 W888 Z610 Z710
   PNX5230 - W350 W380 Z310 Z555
   DB3150 - C702 C902 G502 K630 K660 K850 T700 TM506 V640 W595 W760 W890 W902 W910 W980 Z750 Z770 Z780
   DB3200 - W508 T707
   DB3210 - C510 C901 C903 C905 G705 W705 W707 W715 W995
   DB3350 - U10i Aino, U100 Yari
*/

unsigned int fw_platform ( unsigned int id )
{
   switch ( id )
   {
      case 0x00010000:
         platform = "DB2000";
         break;

      case 0x00100000:
         platform = "DB2010";
         break;

      case 0x00300000:
         platform = "DB2012";
         break;

      case 0x01000000:
         platform = "PNX5230";
         break;

      case 0x10000000:
         platform = "DB2020";
         break;

      case 0x30353133:
         platform = "DB3150";
         break;

      case 0x30303233:
         platform = "DB3200";
         break;

      case 0x30313233:
         platform = "DB3210";
         break;

      case 0x30353333:
         platform = "DB3350";
         break;

      default:
         platform = "Unknown platform";
         return E_FAIL;
   }

   return E_OK;
}

typedef struct
{
	unsigned int id;
	const char *fver;
} fw_ver_t;

fw_ver_t fw_table[] =
{
	{ 2395,       "C510" },
	{ 9289,       "C702" },
	{ 2029,       "C901" },
	{ 1731,       "C902" },
	{ 6139,       "C903" },
	{ 9079,       "C905" },
	{ 0xDEAD0003, "D750/K750" },
	{ 0xDEAD0004, "F500" },
	{ 3445,       "G502" },
	{ 8254,       "G502" },
	{ 4726,       "G705" },
	{ 1,          "J300" },
	{ 78,         "K300" },
	{ 466,        "K310" },
	{ 682,        "K320" },
	{ 73,         "K500" },
	{ 0xDEAD0006, "K508" },
	{ 231,        "K510" },
	{ 242,        "K510" },
	{ 912,        "K530" },
	{ 669,        "K550" },
	{ 14,         "K600" },
	{ 0xDEAD0008, "K608" },
	{ 374,        "K610" },
	{ 576,        "K618" },
	{ 0xDEAD000A, "K630" },
	{ 9286,       "K660" },
	{ 21,         "K700" },
	{ 52,         "K750" },
	{ 7587,       "K770" },
	{ 477,        "K790" },
	{ 210,        "K800" },
	{ 811,        "K810" },
	{ 4400,       "K850" },
	{ 326,        "S500" },
	{ 0xDEAD000C, "S600" },
	{ 0xDEAD000D, "S700" },
	{ 0xDEAD000E, "S710" },
	{ 2562,       "T650" },
	{ 9071,       "T700" },
	{ 3360,       "T707" },
	{ 0xDEAD0010, "TM506" },
	{ 6530,       "U100 [Yari]" },
	{ 0xDEAD0011, "V600" },
	{ 0xDEAD0012, "V630" },
	{ 0xDEAD0013, "V640" },
	{ 0xDEAD0014, "V800" },
	{ 719,        "W200" },
	{ 260,        "W300" },
	{ 0xDEAD0015, "W350" },
	{ 0xDEAD0016, "W380" },
	{ 0xDEAD0017, "W508" },
	{ 0xDEAD0018, "W550" },
	{ 362,        "W580" },
	{ 1537,       "W595" },
	{ 5129,       "W595" },
	{ 106,        "W600" },
	{ 747,        "W610" },
	{ 446,        "W660" },
	{ 514,        "W700" },
	{ 9608,       "W705" },
	{ 0xDEAD001A, "W707" },
	{ 433,        "W710" },
	{ 0xDEAD001B, "W715" },
	{ 3592,       "W760" },
	{ 92,         "W800" },
	{ 316,        "W810" },
	{ 705,        "W830" },
	{ 200,        "W850" },
	{ 640,        "W880" },
	{ 0xDEAD001C, "W888" },
	{ 9164,       "W890" },
	{ 57,         "W900" },
	{ 7421,       "W902" },
	{ 9123,       "W910" },
	{ 0xDEAD001D, "W980" },
	{ 7995,       "W995" },
	{ 594,        "Z310" },
	{ 0xDEAD001F, "Z500" },
	{ 143,        "Z520" },
	{ 290,        "Z525" },
	{ 454,        "Z530" },
	{ 583,        "Z550" },
	{ 0xDEAD0020, "Z555" },
	{ 0xDEAD0021, "Z558" },
	{ 546,        "Z610" },
	{ 434,        "Z710" },
	{ 0xDEAD0022, "Z750" },
	{ 2338,       "Z770" },
	{ 0xDEAD0024, "Z780" },
	{ 46,         "Z800" },
	{ 0xDEAD0026, "Z1010" },
	{ 0x00000000, NULL }
};

unsigned int fw_identify ( t_workspace *ws )
{
   char tmp[256];
   char fver[256];
   char *p = NULL;
   unsigned int v = 0;
   unsigned int model = 0;
   unsigned int rc = 0;
   unsigned int i = 0;

   memset ( tmp,  0, 256 );
   memset ( fver, 0, 256 );

   rc = util_simple_find_pattern ( ws, "RXXXXXX     prg",
           "\xff\x00\x00\x00\x00\x00\x00\xff\xff\xff\xff\xff\xff\xff\xff",
           15, 1 );

   v_memcpy_get ( ws, tmp, rc, 8 );

   rc += 15;
   if ( v_get_b ( ws, rc ) == 0x43 )
      rc += 7;
   else rc +=4;
   
   if ( v_get_b ( ws, rc ) == 0x2D )
      rc++;

   model = util_str2int( v_get_str ( ws, rc ) );


	while ( fw_table[i].id && fw_table[i].id != model )
		i++;

	if ( fw_table[i].id == model )
	{
		sprintf ( fver, "%s SW-%s", fw_table[i].fver, tmp );

		rc = util_quick_find_pattern ( ws, "_", NULL, 1, rc, MEM_AUTO, 0 ) + 1;

		strcpy ( tmp, v_get_str ( ws, rc ) );
		p = tmp;
		while ( *p )
		{
			if ( *p == '_')
			*p = ' ';

			p++;
		}
		strcat ( fver, tmp );

		rc = E_OK;
	}
	else
	{
		sprintf ( fver, "Unknown [0x%08X] ", model );

		rc = E_FAIL;
	}


   symbols_add ( NULL, "FW_NAME", 0, fver );
   symbols_add ( NULL, "FW_PLATFORM", 0, platform );

   return rc;
}

fmt_babe_backup_data ( t_workspace *ws, unsigned int address, unsigned int length )
{
   char *data = malloc ( length );
   if ( !data )
      return E_FAIL;

   babe_backup = LIST_NEW(babe_backup, t_babe_list);

   babe_backup->start = address;
   babe_backup->length = length;
   babe_backup->data = data;

   v_memcpy_get ( ws, data, address, length );

   return E_OK;
}

unsigned int fmt_babe_add_script_line ( const char *line )
{
   t_symbols  *ts  = NULL;

   if ( patch_lines == NULL )
   {
      ts = symbols_get ( NULL, "FW_NAME" );
      if ( !ts )
         return E_FAIL;

      patch_lines = calloc ( strlen ( ts->data ) + strlen ( line ) + 4, 1 );
      sprintf ( patch_lines, "; %s\n", ts->data );
   }
   else
      patch_lines = realloc ( patch_lines, strlen ( patch_lines ) + strlen ( line ) + 1 );

   strcat ( patch_lines, line );

   return E_OK;
}

unsigned int fmt_babe_symbol_add ( const char *name, unsigned int swi, unsigned int address )
{
   char buff[16];

   if ( swi & SWI_THUMB )
      address |= 1;

   sprintf ( buff, "0x%08X", address );
   symbols_add ( NULL, name, swi, buff );

   return E_OK;
}

// convert backslashes to slashes
char *bs2s ( char *st )
{
   unsigned int i = 0;

   while ( st[i] )
   {
      if ( st[i] == '\\' )
         st[i] = '/';
      i++;
   }
   if ( st[0] == '/' )
      st++;

   return st;
}

// convert slashes to backslashes
char *s2bs ( char *st )
{
   unsigned int i = 0;

   while ( st[i] )
   {
      if ( st[i] == '/' )
         st[i] = '\\';
      i++;
   }
   if ( st[0] == '\\' )
      st++;

   return st;
}

// 0 - gnu, 1 - fasmarm
unsigned int run_script ( const char *path , const char *file )
{
   t_fileinfo *fi = NULL;
   char *buff = NULL;

   buff = (char*)malloc ( 400 + strlen ( path ) );

   fi = file_io_create_empty ();
   if ( !fi || !buff )
      return E_FAIL;

   sprintf ( buff, "export PATH=`pwd`/tools/se/bin\n" );
   strcat ( buff, "cd" );
   strcat ( buff, path );
   strcat ( buff, "\n" );
   if ( !file ) // gnu tools
   {
      strcat ( buff, "rm *.o >/dev/null 2>&1\n" );
      strcat ( buff, "make > gnu.log 2>&1\n" );
   }
   else // fasmarm
   {
      strcat ( buff, "rm *.bin > /dev/null 2>&1\n" );
      sprintf ( buff + strlen ( buff ), "fasmarm %s > fasmarm.log 2>&1\n", file );
   }

   fi->stages->segments->length = strlen ( buff );
   fi->stages->segments->data = (char*)strdup ( buff );

   if ( file_write ( "run.sh", fi ) != E_OK )
   {
      DBG ( DEBUG_ARM, "## %s: file write failed\n", __FUNCTION__ );
      return E_FAIL;
   }

	free ( buff );
	file_release_all ( fi );

   system ( "tools\\se\\bin\\sh.exe run.sh" );
   
   fi = file_io_open ( path, file?"fasmarm.log":"gnu.log" );
   if ( fi )
   {
      printf ( "\n%s\n", fi->stages->segments->data );
      printf ( SEPARATOR );
      file_release_all ( fi );
   }

   return E_OK;
}

/******************************************************************************/
// FAMSARM

unsigned int dump_bin_file ( t_workspace *ws, const char *path, const char *fbin )
{
   t_fileinfo *fb = NULL;
   t_stage *ts = NULL;
   unsigned int pos = 0;
   unsigned int offset = 0;
   unsigned int length = 0;

   fb = file_io_open ( path, fbin );

   if ( !fb )
	   return E_FAIL;

   fb->stages->flags &= ~FLAGS_ENDIANESS;
   fb->stages->flags |= FLAGS_ENDIANESS_LE;
   ts = fb->stages;

   while ( pos < ts->length )
   {
      if ( v_get_w_raw ( ts, pos )     == 0x7473756A || // 'just'
           v_get_w_raw ( ts, pos + 4 ) == 0x65736164 )  // 'dase'
      {
         if ( length > 0 )
         {
            fmt_babe_backup_data ( ws, offset, length );
            v_memcpy_put ( ws, offset, GET_PTR ( fb->stages, pos - length ), length );
         }
         offset = v_get_w_raw ( ts, pos + 8 );
         length = 0;
         pos += 11;
      }
      else
         length++;

      pos++;
   }

   if ( length > 0 )
   {
      fmt_babe_backup_data ( ws, offset, length );
      v_memcpy_put ( ws, offset, GET_PTR ( fb->stages, pos - length ), length );
   }

   file_release_all ( fb );
   return E_OK;
}

unsigned int fasmarm ( t_workspace *ws, const char *path, const char *file )
{
   t_fileinfo *fi = NULL;
   t_symbols  *ts = NULL;
   unsigned int bufflen = 0;
   unsigned int i = 0;
   char *buff     = NULL;
   char *dotpos = NULL;
   char tmp[1024];

   buff = (char *)calloc ( 0x100000, 1 ); // 1MB
   fi = file_io_create_empty ( );

   if ( !fi || !buff )
	  return E_FAIL;

   ts = symbols_get ( NULL, "FW_PLATFORM" );

   sprintf ( buff, "SE_PLATFORM_VER equ %s\ninclude \"%s\\tools\\se\\include\\x.inc\"\n", ts->data, trix_path );
   bufflen = strlen ( buff );

   ts = ts->next;
   while ( ts )
   {
      sprintf ( tmp, "%s equ %s\n", ts->name, ts->data );
      strcat ( buff, tmp );
      bufflen += strlen ( tmp );
	  ts = ts->next;
   }

   buff = realloc ( buff, bufflen );
   fi->stages->segments->length = bufflen;
   fi->stages->segments->data = buff;

   sprintf ( tmp, "%s/addresses.inc", path );
   if ( file_io_write ( tmp, fi ) != E_OK )
   {
      DBG ( DEBUG_FMT, "## %s: file write failed\n", __FUNCTION__ );
      return E_FAIL;
   }
   file_io_release_all ( fi );

   run_script ( path, file );

   memset (tmp, 0, 1024);
   strcpy ( tmp , file );
   dotpos = strchr ( tmp, '.' );
   if ( dotpos )
      strcpy ( dotpos, ".bin" );
   else
      strcat ( dotpos, ".bin" );
   dump_bin_file ( ws, path, tmp );
   
   return E_OK;
}


/******************************************************************************/
// GNU

unsigned int create_makefile ( const char *path )
{
   t_fileinfo *fi = NULL;
   t_symbols *ts = NULL;
   unsigned int ret = E_OK;
   char tmp[1024];
   char fp[256];

   fi = file_io_create_empty ();
   if ( !fi )
      return E_FAIL;

   ts = symbols_get ( NULL, "FW_PLATFORM" );

   memset ( tmp, 0, 1024 );
   sprintf ( tmp, "TRIX_ROOT = %s\nSE_PLATFORM_VER = %s\n", trix_path, ts->data );
   strcat ( tmp, "include OFILES\ninclude $(TRIX_ROOT)/tools/se/include/makefile.glo\n" );
   bs2s ( tmp );
   
   sprintf ( fp, "%s/makefile", path );

   fi->stages->segments->length = strlen ( tmp );
   fi->stages->segments->data = (char*)strdup ( tmp );

   if ( file_write ( fp, fi ) != E_OK )
   {
      DBG ( DEBUG_ARM, "## %s: file write failed\n", __FUNCTION__ );
      ret = E_FAIL;
   }

	file_release_all ( fi );
   return E_OK;
}

/******************************************************************************/

unsigned int fmt_babe_compile_code ( t_workspace *ws, const char *path, const char *file )
{
   if ( !file ) // gnu
   {
      create_makefile ( path );
      run_script ( path, NULL );
   }
   else
      fasmarm ( ws, path, file );

   return E_OK;
}


unsigned int dump_script ( t_workspace *ws, unsigned int base )
{
   unsigned int mstart = 0;
   unsigned int mlen = 0;
   unsigned int pos = 0;
   char tmp[256];
   char *org = NULL;
   char *mod = NULL;

   if ( !ws || !babe_backup )
      return E_FAIL;


	LIST_REWND ( babe_backup );

   while ( babe_backup )
   {
      mstart = babe_backup->start;
      mlen = babe_backup->length;

      while ( mlen > 16 )
      {
         v_memcpy_get ( ws, tmp, mstart, 16 );
         org = util_hexpack ( babe_backup->data + babe_backup->length - mlen, 16 );
         mod = util_hexpack ( tmp, 16 );
         sprintf ( tmp, "%08X: %s %s\n", mstart - base, org, mod );
         fmt_babe_add_script_line ( tmp );
         free ( org );
         free ( mod );
         mstart += 16;
         mlen -= 16;
      }
      v_memcpy_get ( ws, tmp, mstart, mlen );
      org = util_hexpack ( babe_backup->data + babe_backup->length - mlen, mlen );
      mod = util_hexpack ( tmp, mlen );
		sprintf ( tmp, "%08X: %s %s\n", mstart - base, org, mod );
      fmt_babe_add_script_line ( tmp );
      free ( org );
      free ( mod );

      LIST_NEXT ( babe_backup );
   }

   return E_OK;
}


unsigned int fmt_babe_create_patch ( t_workspace *ws )
{
   t_stage   * cs  = NULL;
   t_segment *smod = NULL;
   t_fileinfo *fi = NULL;
   char *filename  = NULL;
   unsigned int base = 0;
   char tmp[32];

   if ( !ws->fileinfo )
		return E_FAIL;

	cs = stage_get_current( ws->fileinfo->stages );
	if ( !cs )
      return E_FAIL;

	smod = segment_find_by_name ( cs->segments, "DATA", -1 );
   if ( !smod )
      return E_FAIL;

   base = smod->start; // our base addr


   printf ( "\nCreating patch. Please wait...\n\n" );
   
   sprintf ( tmp, "; Generated by TriX\n\n+%08X\n", base );
   fmt_babe_add_script_line ( tmp );

   dump_script ( ws, base );

   ui_dlg_save_file ( &filename, "Save patch as", "VKP patch (*.vkp)"  );

   if ( filename )
   {
      fi = file_io_create_empty ();
      if ( !fi )
         return E_FAIL;

      fi->stages->segments->length = strlen ( patch_lines );
      fi->stages->segments->data = (char*)strdup ( patch_lines );

      if ( file_write ( filename, fi ) != E_OK )
      {
         DBG ( DEBUG_ARM, "## %s: file write failed\n", __FUNCTION__ );
         return E_FAIL;
      }
      file_release_all ( fi );

      printf ( "Done!\n" );
   }
   else
   {
      printf ( "<------------------ BEGIN PATCH --------------------\n" );
      printf ( "%s", patch_lines );
      printf ( "-------------------- END PATCH -------------------->\n" );
   }

	return E_OK;
}

unsigned int init_protected_memory ( t_stage * target )
{
   t_segment *ts = NULL;
   t_segment *rom = NULL;
   t_babe_list *priv = NULL;
   unsigned int pos = 0;
   unsigned int p = 0;
   unsigned int start = 0;
   unsigned int end = 0;
   unsigned int rs = 0;
   unsigned int re = 0;
   unsigned int src = 0;
   unsigned int fota = 0;
   unsigned int tmp  = 0;


   if ( !target )
      return E_FAIL;

   rom = segment_find_by_name ( target->segments, "DATA", -1 );
   pos = rom->start;

   while ( pos < rom->end &&
         ( v_get_w_raw ( target, pos )      != 0xE92D4000 ||    // stmfd   sp!, {lr}
           v_get_w_raw ( target, pos + 4 )  != 0xE1A0E00F ||    // mov     lr, pc
           v_get_w_raw ( target, pos + 8 )  != 0xE59FF014 ||    // ldr     pc, off...
           v_get_w_raw ( target, pos + 12 ) != 0xE8BD4000 ) )   // ldmfd   sp!, {lr}
      pos += 4;

   if ( v_get_w_raw ( target, pos ) != 0xE92D4000 )
      return E_FAIL;

   pos = v_get_w_raw ( target, pos + 0x24 ) & ~1;

   while ( v_get_h_raw ( target, pos ) != 0xBD00 ) // pop pc
      pos += 2;

   if ( v_get_h_raw ( target, pos ) != 0xBD00 )
      return E_FAIL;

   if ( v_get_h_raw ( target, pos + 2 ) == 0x46C0 )
      pos += 4;
   else
      pos += 2;

   src = v_get_w_raw ( target, pos + 8 );
   if ( src < rom->start || src > rom->end ) // fota
   {
      fota = 4;
      pos += 8;
   }
   else // // ACC/APP CPU
      fota = 0;

   while ( p != 3 )
   {
      src   = v_get_w_raw ( target, pos + 8 - fota );

      if ( src < rom->start || src > rom->end )
      {
         p++;
         pos += 4;
      }
      else
      {
         start = v_get_w_raw ( target, pos );
         end   = v_get_w_raw ( target, pos + 4 + fota );
         if ( start > end ) // sometimes it's swapped ;)
         {
            tmp = start;
            start = end;
            end = tmp;
         }

         if ( end - start )
         {
            ts = segment_add ( target->segments );
            ts->name = "DATA";
            ts->start = start;
            ts->end   = end;
            ts->length = end - start;
            ts->data = malloc ( ts->length );

            priv = (t_babe_list *)PRIV_MALLOC(t_babe_list);
            if ( !priv )
               return E_FAIL;

            priv->start = src;
            priv->length = ts->length;
            ts->priv = (t_stage_priv *)priv;

            memcpy ( ts->data, rom->data + src - rom->start, ts->length );
            memset ( rom->data + src - rom->start, 0xCC, ts->length );
         }
         pos += 12;
      }
   }

   return E_OK;
}

unsigned int fmt_babe_reinit ( t_workspace *ws )
{
   if ( !ws->memmap )
      return E_FAIL;

   // maybe workspace endianness should be set same as last segment?
   ws->flags = FLAGS_ENDIANESS_LE;

   file_sync ( ws->fileinfo );
   workspace_update_memmap ( ws );
   symbols_free_all ( NULL );

   fw_identify ( ws );

   // get the current working directory
   if ( !trix_path )
      trix_path = _getcwd ( NULL, 0 );

   return E_OK;
}


unsigned int fmt_babe_decode ( t_stage * source, t_stage * target )
{
   t_segment *ts = NULL;
   t_babe_list *priv = NULL;
   unsigned int pos = 0;
   unsigned int length = 0;
   unsigned int blocks_count = 0;
   unsigned int processed_blocks_count = 0;
   unsigned int offset = 0;
   unsigned int prev_offset = 0;
   unsigned int prev_length = 0;
   unsigned int pagehdr_size = 0;

   unsigned int header_version = 0;
   unsigned int color = 0;
   unsigned int cid = 0;


	DBG ( DEBUG_FMT, " => %s ( ) called\n", __FUNCTION__ );

	if ( !strcmp ( source->type, "BABE" ) )
		return E_FAIL;

	if ( !target )
        target = source->next;

   if ( !source || !target || !source->segments || !source->segments->data )
   {
      DBG ( DEBUG_FMT, " -> %s ( ) failed (null ptr)!!\n", __FUNCTION__ );
      return E_FAIL;
   }

	if ( source->length <= 24 )
	{
		DBG ( DEBUG_FMT, " -> %s ( ) failed (length)!!\n", __FUNCTION__ );
		return E_FAIL;
	}

   source->flags &= ~FLAGS_ENDIANESS;
   source->flags |= FLAGS_ENDIANESS_LE;
   target->flags &= ~FLAGS_ENDIANESS;
   target->flags |= FLAGS_ENDIANESS_LE;

   target->segments = segment_create();
   ts = target->segments;

   if ( v_get_h_raw ( source, 0) == 0xBEBA ) // mbn file
   {
      header_version = v_get_h_raw ( source, 2 );

      switch ( header_version )
      {
         case 0x0200: // A1 platform
         case 0x0300:
         case 0x0400:
         {
            if ( v_get_w_raw ( source, 0x2C0 ) != 0x210 )
            {
               DBG ( DEBUG_FMT, " -> %s ( ) failed (unknown format)!!\n", __FUNCTION__ );
               return E_FAIL;
            }

            color = v_get_w_raw ( source, 4 );
            cid   = v_get_w_raw ( source, 0x10 );
            blocks_count = v_get_w_raw ( source, 0x2E8 );
            fw_platform ( v_get_w_raw ( source, 8 ) );
            break;
         }
         case 0x0002: // A2 platform
         {
            color = v_get_w_raw ( source, 0x10 );
            cid   = v_get_w_raw ( source, 0x30 );
            blocks_count = v_get_w_raw ( source, 0x20 );
			pos = 0x60;
			while ( pos < 0x200 && v_get_h_raw ( source, pos ) != 0x4244 ) // 'DB'
				pos += 2;

			if ( v_get_h_raw ( source, pos ) != 0x4244 )
				return E_FAIL;

            fw_platform ( v_get_w_raw ( source, pos + 2 ) );
            break;
         }
         default:
            DBG ( DEBUG_FMT, " -> %s ( ) failed (unknown header version)!!\n", __FUNCTION__ );
            return E_FAIL;
      }

      switch ( header_version )
      {
         case 0x0200:
            pos = 0x100 + 0x380;
            break;

         case 0x0300:
            pos = blocks_count + 0x380;
            break;

         case 0x0400:

            pos = (20 * blocks_count) + 0x380;
            break;

         case 0x0002:
         {
            pos = ( v_get_w_raw ( source, 0x50 ) + 7 ) & ~7;
            break;
         }
         default:
            DBG ( DEBUG_FMT, " -> %s ( ) failed (unknown header version)!!\n", __FUNCTION__ );
            return E_FAIL;
      }

      ts->name = "DATA";
      ts->start = v_get_w_raw ( source, pos );
      ts->data = malloc ( 0x10000000 ); // 256MB
      memset ( ts->data, 0xFF, 0x10000000 );
      ts->length = 0;

      for ( processed_blocks_count = 0; processed_blocks_count < blocks_count; processed_blocks_count++ )
      {
         offset = v_get_w_raw ( source, pos );
         length = v_get_w_raw ( source, pos + 4 );
		   pos += 8;

         if ( offset  == 0 || length == 0)
            break;

         if ( processed_blocks_count )
         {
            if ( offset > prev_offset + prev_length )
               ts->length += offset - ( prev_offset + prev_length );
         }

         memcpy ( ts->data + ts->length, GET_PTR ( source, pos ), length );
         ts->length += length;

		   prev_offset = offset;
		   prev_length = length;
         pos += length;
      }

      ts->data = realloc ( ts->data, ts->length );
      ts->end = ts->start + ts->length;
   }
   else if ( v_get_h_raw ( source, 0x0080 ) == 0xBEBA ||
	         v_get_h_raw ( source, 0x1080 ) == 0xBEBA ||
            v_get_w_raw ( source, 0x3C ) + v_get_w_raw ( source, 0x48 ) + 0x1000 == source->length ) // cxc file
   {
	   if ( v_get_h_raw ( source, 0x1080 ) == 0xBEBA ) // corrupted file, try to fix it
	   {
         memcpy ( GET_PTR ( source, 0 ), GET_PTR ( source, 0x1000 ), v_get_w_raw ( source, 0x1038 ) );
         stage_set_modified ( target );
      }

      fw_platform ( v_get_w_raw ( source, offset + 0x80 + v_get_b_raw ( source, offset + 0x80 + 0x2C ) ) );

      if ( v_get_w_raw ( source, 0x40 ) ) // SHA-1 table start
      {
         ts->name = "SHA1T";
         ts->start = v_get_w_raw ( source, 0x40 );
         length = v_get_w_raw ( source, 0x3C );
         ts->length = length;
         ts->data = malloc ( length );
         memset ( target->segments->data, 0xCC, length );
         ts->end = ts->start + length;

         priv = (t_babe_list *)PRIV_MALLOC(t_babe_list);
         if ( !priv )
            return E_FAIL;

         priv->data = malloc ( 0x1000 );
         if ( !priv->data )
            return E_FAIL;

         priv->start = 0;
         priv->length = 0x1000;
         memcpy ( priv->data, GET_PTR ( source, 0 ), 0x1000 );

         ts->priv = (t_stage_priv *)priv;

         ts = segment_add ( target->segments );
      }

      ts->name = "DATA";
      ts->start = v_get_w_raw ( source, 0x4C );
      length = v_get_w_raw ( source, 0x64 );
      ts->length = v_get_w_raw ( source, 0x64 );
      ts->data = malloc ( length );
      memset ( ts->data, 0, length );
      pos = v_get_w_raw ( source, 0x44 );
      memcpy ( ts->data, GET_PTR ( source, pos ), length );
      ts->end = ts->start + length;

      if ( v_get_h_raw ( target, ts->start ) == 0xDADF ) // pCore
      {
         offset = 0;
         pos = ts->start;

         while ( v_get_h_raw ( target, pos ) == 0xDADF )
         {
            length = v_get_h_raw ( target, pos + 0x14 );
            offset += length;
            memcpy ( ts->data + v_get_w_raw ( target, pos + 0x10 ) - ts->start, GET_PTR ( target, pos - ts->start + 0x20 ), length );
            pos += 0x20 + length;
         }
         ts->length = offset;
         // memset ( ts->data + offset, 0,ts->length - offset );
         ts->data = realloc ( ts->data, ts->length );
         ts->end = ts->start + ts->length;
      }
      else // at OBT some protected routines are copied to separate segment in memory, we should do it also
         init_protected_memory ( target );
   }
   else
      return E_FAIL;

   if ( v_get_w_raw ( ts, ts->start ) == 0xFFFFFFFF ) // A2 mbn file
   {
      pos = ts->start;
      length = 0;

      while ( v_get_h_raw ( ts, pos ) != 0xDADF )
         pos += 0x10;

      pos += 0x800;

      if ( v_get_w_raw ( ts, pos ) == 0xFF00FFFF )
         pagehdr_size = 0x40;
      else
         pagehdr_size = 0; // k850

      pos += pagehdr_size;

      while ( v_get_h_raw ( ts, pos + 0x1FE ) != 0xAA55 )
         pos += pagehdr_size + 0x800;

      pos += pagehdr_size + 0xE00;

      if ( v_get_h_raw ( ts, pos + 0x1FE ) != 0xAA55 )
         return E_FAIL;

      memcpy ( GET_PTR ( target, length ), GET_PTR ( target, pos - ts->start ), 0x200 );
      length += 0x200;
      pos += 0x200;

      while ( v_get_w_raw ( ts, pos ) == 0xFF00FFFF )
      {
         memcpy ( GET_PTR ( target, length ), GET_PTR ( target, pos + 0x40 - ts->start ), 0x800 );
         length +=0x800;
         pos += 0x840;
      }
      ts->data = realloc ( ts->data, length );
      ts->start = 0;
      ts->length = length;
      ts->end = length;

      target->type   = "FAT16";
   }
   else
   {
      length = ts->length;

      ts = segment_find_by_name ( target->segments, "SHA1T", -1 );
      if ( ts )
         length += ts->length; // TODO: length

      target->type   = "BABE";
   }

   target->name   = "PARSED";
   target->length = length;
   target->parser = "BABE";

	HEAP_CHECK;

	DBG ( DEBUG_FMT, " ## %s ( %s, %s ) done\n", __FUNCTION__, source->name, target->name );
   return E_OK;
}


unsigned int fmt_babe_free ( t_stage * s )
{
   DBG ( DEBUG_FMT, " => %s ( ) called\n", __FUNCTION__ );

   CHECK_AND_FREE ( patch_lines );
   //CHECK_AND_FREE ( trix_path ); // assert occurs...

   LIST_REWND ( babe_backup );
   while ( babe_backup )
   {
      CHECK_AND_FREE ( babe_backup->data );
      LIST_FREE ( babe_backup );
   }

   return E_OK;
}


/*
   pCore DADF header:
   DFDA 1600 D798D30A 7EF72F49 FF7F 4255 FFFFFFFF E00F0000 00000000 00000000
   DFDA 1600 D798D30A 7EF72F49 0100 4936 00000028 E00F0000 00000000 00000000

   00000000 [32] - pCore DADF....
   00000020 [04] - unk
   00000024 [20] - SHA1-T hash
   00000038 [04] - SHA1-T offset
   0000003C [04] - SHA1-T length
   00000040 [04] - SHA1-T mapping address
   00000044 [04] - DATA offset
   00000048 [04] - DATA length ( 4K aligned )
   0000004C [04] - DATA mapping address
   00000050 [20] - DATA hash
   00000064 [04] - DATA length
   00000080 [02] - BABE header
   000000C0 [04] - 00000064
*/


unsigned int fmt_babe_encode_data ( t_stage *dst, t_stage *src )
{
   t_segment *rom = NULL;
   t_segment *ts = NULL;
   t_babe_list *priv = NULL;
   unsigned int len = 0;
   unsigned int id = 0;
   unsigned int pos = 0;
   char *hash = NULL;
   unsigned int offset = 0;
   unsigned short chk = 0;


   ts = segment_find_by_name ( src->segments, "SHA1T", -1 );
   if ( !ts || !ts->priv )
      return E_FAIL;

   rom = segment_find_by_name ( src->segments, "DATA", -1 );
   if ( !rom || !dst->segments->data )
      return E_FAIL;

   // fix cxc header
   priv = ts->priv;
   memcpy ( GET_PTR ( dst, 0 ), priv->data, 0x1000 );

   len = rom->length;
   v_set_w_raw ( dst, 0x64, len );
   v_set_w_raw ( dst, 0xC0, len );

   len = ( len + 0xFFF ) & ~ 0xFFF;
   v_set_w_raw ( dst, 0x48, len );
   v_set_w_raw ( dst, 0x38, len + 0x1000 );
   v_set_w_raw ( dst, 0x3C, ( len >> 10 ) * 0x14 );

   CHECK_AND_FREE ( priv->data );

   // write protected segments back to the main segment
   id = 2;
   ts = segment_find_by_num ( src->segments, id );
   while ( ts && ts->priv )
   {
      priv = (t_babe_list *)ts->priv;
      memcpy ( rom->data + priv->start - rom->start, ts->data, priv->length );
      ts = segment_find_by_num ( src->segments, id++ );
   }

   // generate main segment hash
   crypto_generate_sha1 ( rom->data, v_get_w_raw ( dst, 0x64 ), GET_PTR ( dst, 0x50 ) );

   if ( v_get_h_raw ( dst, 0 ) == 0xDADF ) // pCore
   {
      chk = fmt_babe_checksum ( GET_PTR ( dst, 0x20 ), 0xFE0 );
      v_set_h_raw ( dst, pos + 0x0E, chk );
      pos = dst->segments->start + 0x1000;
      do
      {
         len = v_get_h_raw ( dst, pos + 0x14 );
         offset = v_get_w_raw ( dst, pos + 0x10 );
         chk = fmt_babe_checksum ( rom->data + offset - rom->start, len );
         memcpy ( GET_PTR ( dst, pos + 0x20 - dst->segments->start ), rom->data + offset - rom->start, len );
         v_set_h_raw ( dst, pos + 0x0E, chk );
         id = v_get_h_raw ( dst, pos + 0x0C );
         pos += len + 0x20; 
      }
      while ( id );
   }
   else
   {
      dst->segments->length = v_get_w_raw ( dst, 0x38 ) + v_get_w_raw ( dst, 0x3C );
      dst->segments->data = realloc ( dst->segments->data, dst->segments->length );
      memset ( GET_PTR ( dst, 0x1000 ), 0, dst->segments->length - 0x1000 );
      dst->segments->end = dst->segments->start + dst->segments->length;

      pos = 0x1000;
      len = v_get_w_raw ( dst, 0x38 );
      hash = GET_PTR ( dst, v_get_w_raw ( dst, 0x38 ) );

      memcpy ( GET_PTR ( dst, 0x1000 ), rom->data, rom->length );
      while ( pos < len )
      {
         crypto_generate_sha1 ( GET_PTR ( dst, pos ), 0x400, hash );
         pos += 0x400;
         hash += 20;
      }
      crypto_generate_sha1 ( GET_PTR ( dst, v_get_w_raw ( dst, dst->segments->start + 0x38 ) ), v_get_w_raw ( dst, dst->segments->start + 0x3C ), GET_PTR ( dst, 0x24 ) );
   }

   return E_OK;
}

unsigned int fmt_babe_encode ( t_stage * source, t_stage * target )
{
   DBG ( DEBUG_FMT, " => %s ( ) called\n", __FUNCTION__ );

   if ( !source || !target )
   {
      DBG ( DEBUG_FMT, " -> %s ( ) failed !!\n", __FUNCTION__ );
      return E_FAIL;
   }

	if ( fmt_babe_encode_data ( target, source ) != E_OK )
   {
      DBG ( DEBUG_FMT, " -> %s ( %s, %s ) failed !!\n", __FUNCTION__, source->name, target->name );
      return E_FAIL;
   }

	/* preserve everything, just replace data and length */
	target->segments->flags &= ~FLAGS_FREE_NAME;
	target->segments->name = "DATA";

	/* set the parser again (see file_format() ). all parsers should do this. shouldn't they? */
	source->parser = "BABE";
	source->type = "BABE";

	HEAP_CHECK;

	DBG ( DEBUG_FMT, " ## %s ( %s, %s ) done\n", __FUNCTION__, source->name, target->name );
   return E_OK;
}


t_fmt_handler fmtBABE_handler = {
   "BABE",
   "FORMAT",
	"fmt_babe_decode",
    fmt_babe_decode,
	"fmt_babe_encode",
    fmt_babe_encode,
	"fmt_babe_free",
    fmt_babe_free,
	NULL,
	NULL
};


unsigned char *fmtBABE_lasterror ( )
{
	unsigned char *ret = last_error_string;
	last_error_string = "None";

	return ret;
}


unsigned int fmtBABE_cleanup ( )
{
	fmt_unregister_handler ( fmtBABE_handler.name );
	printf ( "FMT BABE Plugin Unloaded" );
   return E_OK;
}


unsigned int fmtBABE_init ( )
{
   REGISTER_HEADER ( "trix_fmtbabe", FMTBABE_HEADERS );
   FMTBABE_SYMBOLS
   fmt_register_handler ( &fmtBABE_handler );

   printf ( FMTBABE_TITLE " v%01i.%02i Plugin Loaded", FMTBABE_MAJOR, FMTBABE_MINOR  );
   return E_OK;
}

#endif /* __TRIX_FMTBABE_C__ */
