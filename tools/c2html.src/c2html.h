/*-------------------------------------------------------------------------------
Name               : c2html.h
Author             : Marvin Raaijmakers
Description        : c2html headerfile
Date of last change: 19-December-2004
History            :
                     19-December-2004  Added function check_begin_identif() and 
                                       check_end_identif() created by David
                                       Ireland

    Copyright (C) 2004 Marvin Raaijmakers

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    You can contact me at: marvin_raaijmakers(at)lycos(dot)nl
    (replace (at) by @ and (dot) by .)
---------------------------------------------------------------------------------*/
#define VERSION	"1.1.0"

#define TRUE		1
#define ONLY1CHAR	2
#define FALSE		0
#define EQUAL		0

#define HTML_EXTENTION	".html"

#define MAX_NUM_ARGS	2
#define MAX_LINE_SIZE	999

#define FONT_FACE	"Monospace"
#define TAB_SIZE	8

#define BLACK		"#000000"
#define GREY		"#808080"
#define BLUE		"#000080"
#define RED		"#800000"
#define LIGHT_MAGENTA	"#FF00FF"
#define LIGHT_BLUE	"#0000FF"
#define MAGENTA		"#800080"
#define TURQUOISE	"#008080"
#define GREEN		"#008000"
#define LIGHT_RED	"#FF0000"

#define NUM_TYPE_SPECIFIERS	14
#define NUM_KEYWORDS		19
#define NUM_HYPHENS		6

typedef enum {
	ERR_NO_ARG,
	ERR_2MANY_ARGS,
	ERR_UNREC_ARG,
	ERR_OPEN_INPUT_FILE,
	ERR_OPEN_OUTPUT_FILE,
	ERR_MALLOC,
	ERR_UNKNOWN
} ERROR;

/*--------------------------------------------------------------------------
Prototypes of the functions of convert.c
----------------------------------------------------------------------------*/
extern void convert (FILE *input_file, FILE *output_file);
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
Prototypes of the functions of error.c
----------------------------------------------------------------------------*/
extern void handle_error (ERROR error, char *extra_info);
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
Prototypes of the functions of write_header_footer.c
----------------------------------------------------------------------------*/
extern void write_html_header (FILE *output_file, char *title);
extern void write_html_footer (FILE *output_file);
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
Prototypes of the functions of check_begin.c
----------------------------------------------------------------------------*/
extern char isbeginend (const char beginend[], char line[]);
extern char check_begin_comment (char line[]);
extern char check_begin_keyword (char line[]);
extern char check_begin_type_specif (char line[]);
extern char check_begin_char (char line[]);
extern char check_begin_oct (char line[]);
extern char check_begin_int (char line[]);
extern char check_begin_float (char line[]);
extern char check_begin_hex (char line[]);
extern char check_begin_preprocessor (char line[]);
extern char check_begin_str (char line[]);
extern char check_begin_identif (char line[]);
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
Prototypes of the functions of check_end.c
----------------------------------------------------------------------------*/
extern char check_end_comment (char line[]);
extern char check_end_keyword (char line[]);
extern char check_end_type_specif (char line[]);
extern char check_end_char (char line[]);
extern char check_end_oct (char line[]);
extern char check_end_int (char line[]);
extern char check_end_float (char line[]);
extern char check_end_hex (char line[]);
extern char check_end_preprocessor (char line[]);
extern char check_end_str (char line[]);
extern char check_end_identif (char line[]);
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
Prototypes of the functions of print_info.c
----------------------------------------------------------------------------*/
extern void print_help (void);
extern void print_about (void);
extern void print_version (void);
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
Global variables of check_begin.c
----------------------------------------------------------------------------*/
extern char comment1;
/*--------------------------------------------------------------------------*/
