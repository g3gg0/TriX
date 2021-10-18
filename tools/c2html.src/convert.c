/*-------------------------------------------------------------------------------
Name               : convert.c
Author             : Marvin Raaijmakers
Description        : Convert the input file to html and write it to the
                     output file
Date of last change: 19-December-2004

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
#include <stdio.h>
#include <stdlib.h>
#include <c2html.h>

#define NUM_DIFF_PARTS	11
#define NUM_HTML_CHARS	6

/*--------------------------------------------------------------------------
Typedefs in this file
----------------------------------------------------------------------------*/
typedef struct {
	char bold;
	char italic;
	char underlined;
} FONT_STYLE; /* (used by FONT) Contains information about a font style */
typedef struct {
	char *color;
	FONT_STYLE font_style;
} FONT; /* (used by CODEPART) Contains information about a formatted text */
typedef struct {
	char (*check_begin)();
	char (*check_end)();
	FONT font;
} CODEPART; /* Contains information about a code part*/
typedef struct {
	char ascii;
	char *html;
} HTML_CHAR; /* Contains a replacing string (html) which equals to a
		html-comverted character (ascii) */
/*------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
Prototypes of the functions in this file with a file scope
----------------------------------------------------------------------------*/
static int write_char (char c, int position, FILE *output_file);
static void write_begin_tag (FONT font, FILE *output_file);
static void write_end_tag (FONT font, FILE *output_file);
/*--------------------------------------------------------------------------*/

/* An array containing information for every codepart */
const CODEPART codepart[NUM_DIFF_PARTS] = {
	/*-----------------------------------------------------------------------------------------
	 (*CHECK_BEGIN)()         (*CHECK_END)()          COLOR           BOLD   ITALIC  UNDERLINED
	-------------------------------------------------------------------------------------------*/
	{check_begin_comment,     check_end_comment,     {GREY,          {FALSE, TRUE,   FALSE     }}},
	{check_begin_keyword,     check_end_keyword,     {BLUE,          {TRUE,  FALSE,  FALSE     }}},
	{check_begin_type_specif, check_end_type_specif, {RED,           {FALSE, FALSE,  FALSE     }}},
	{check_begin_char,        check_end_char,        {LIGHT_MAGENTA, {FALSE, FALSE,  FALSE     }}},
	{check_begin_float,       check_end_float,       {MAGENTA,       {FALSE, FALSE,  FALSE     }}},
	{check_begin_hex,         check_end_hex,         {TURQUOISE,     {FALSE, FALSE,  FALSE     }}},
	{check_begin_oct,         check_end_oct,         {TURQUOISE,     {FALSE, FALSE,  FALSE     }}},
	{check_begin_int,         check_end_int,         {LIGHT_BLUE,    {FALSE, FALSE,  FALSE     }}},
	{check_begin_preprocessor,check_end_preprocessor,{GREEN,         {FALSE, FALSE,  FALSE     }}},
	{check_begin_str,         check_end_str,         {LIGHT_RED,     {FALSE, FALSE,  FALSE     }}},
/* added DAI */
	{check_begin_identif,     check_end_identif,     {BLACK,         {FALSE, FALSE,  FALSE     }}}
	/*-----------------------------------------------------------------------------------------*/
};


void
convert (FILE *input_file, FILE *output_file)
/*
input:
	input_file	- The file we read the sourcecode from
	output_file	- The file we write the to html-converted input file to
returns:
	-
description:
	This function converts the sourcecode from the input file and writes it
	to the output file.
*/
{
	char *line, isbegin;
	int count, count2, position;
	const CODEPART *selected;

	/* Allocate memory for the line */
	line = (char *) malloc (MAX_LINE_SIZE+1);
	if (line == NULL) /* If no memory has been allocated */
	{
		/* A message will be printed to stderr and the program will exit */
		handle_error (ERR_MALLOC, NULL);
	}
	/*
	   If we want to check the beginning of a codepart at the first character of the line correctly,
	   we have to do the following 2 thing:
	*/
	*line = ' ';	/* - Assign ' ' to *line */
	line++;		/* - Let line point to the address after the current */

	selected = NULL; /* Let selected point to no codepart */
	/* Read every line from the input file until it reaches EOF */
	while ( fgets( line, MAX_LINE_SIZE, input_file) )
	{
		position = 0;
		/* Read every character from the line */
		for (count = 0; line[count]; count++)
		{
			if (selected) /* If the character belongs to a codepart */
			{
				/* Write the character to the output file */
				position += write_char (line[count], position, output_file);
    				/* If the character is the end of the codepart */
				if ( selected->check_end(&line[count]) )
				{
					/* Write the tag which closes the codepart to the output file */
					write_end_tag (selected->font, output_file);
					/* Let selected point to no codepart */
					selected = NULL;
				}
			}
			else
			{
				isbegin = FALSE;
				/* Check for every codepart if line[count] is a beginning character of it */
				for (count2 = 0; (count2 < NUM_DIFF_PARTS) && (selected == NULL); count2++)
				{
					/* Check if line[count] is a beginning of a codepart */
					isbegin = codepart[count2].check_begin(&line[count]);
					/* If line[count] is a beginning of a codepart */
					if (isbegin)
					{
						/* Write the tag which starts the codepart */
						write_begin_tag (codepart[count2].font, output_file);
						/* Select codepart[count2] */
						selected = &codepart[count2];
					}
				}
				/* Write the character to the output_file */
				position += write_char (line[count], position, output_file);
				/* If the codepart only consists of one char */
				if (isbegin == ONLY1CHAR)
				{
					/* Write the tag which closes the codepart to the output file */
					write_end_tag (selected->font, output_file);
					/* Let selected point to no codepart */
					selected = NULL;
				}
			}
		}
		putc ('\n', output_file);
	}
}

static void
write_begin_tag (FONT font, FILE *output_file)
/*
input:
	font		- Contains information about the font we have to write the
			  tags for
	output_file	- The file we write the tags to
returns:
	-
description:
	This function writes the beginningtags of a formatted text to the output file.
	font contains information about how the text is formatted and prescribes what
	we should write to the output file.
*/
{
	const char *bold = "<b>";
	const char *italic = "<i>";
	const char *underlined = "<u>";

	fprintf (output_file, "<font color=\"%s\">", font.color);
	if (font.font_style.bold)
	{
		fprintf (output_file, bold);
	}
	if (font.font_style.italic)
	{
		fprintf (output_file, italic);
	}
	if (font.font_style.underlined)
	{
		fprintf (output_file, underlined);
	}
}

static void
write_end_tag (FONT font, FILE *output_file)
/*
input:
	font		- Contains information about the font we have to write the
			  tags for
	output_file	- The file we write the tags to
returns:
	-
description:
	This function writes the endtags of a formatted text to the output file.
	font contains information about how the text is formatted and prescribes what
	we should write to the output file,
*/
{
	const char *bold = "</b>";
	const char *italic = "</i>";
	const char *underlined = "</u>";

	if (font.font_style.underlined)
	{
		fputs (underlined, output_file);
	}
	if (font.font_style.italic)
	{
		fputs (italic, output_file);
	}
	if (font.font_style.bold)
	{
		fputs (bold, output_file);
	}
	fputs ("</font>", output_file);
}

static int
write_char (char c, int position, FILE *output_file)
/*
input:
	c		- The character we should to write to the output file
	position	- The position of c in the line it appears (so c is the
			  positionst letter of the line)
	output_file	- The file we should write the character to
returns:
	The number of written characters.
description:
	This function writes c to the output file. Because the output file is a
	html-file we have to convert some characters to a string wich equals to
	c converted to html. If c is a tab ('\t') it will be replaced by a
	calculated number of spaces.
*/
{
	const HTML_CHAR html_char[NUM_HTML_CHARS] = {
		{'"',  "&quot;"},
		{'<',  "&lt;"},
		{'>',  "&gt;"},
		{'&',  "&amp;"},
		{' ',  "&nbsp;"},
		{'\n', "<br>"}
	};
	const char *space = "&nbsp;";
	int count, num_spaces;

	if (c == '\t') /* If c is a tab */
	{
		/*
		   Write spaces to the output file until a we reach a tab stop.
		   Note: after every TAB_SIZE characters a tab stop appears.
		*/
		num_spaces = TAB_SIZE - (position % TAB_SIZE);
		for (count = num_spaces; count; count--)
		{
			fputs (space, output_file); /* Write a space to the output file */
		}
		return (num_spaces); /* Exit this function */
	}

	/* Check if c equals to one of the characters we have to replace by a string */
	for (count = 0; count < NUM_HTML_CHARS; count++)
	{
		if (c == html_char[count].ascii)
		{
			/* Write the replacing string to the output file */
			fputs (html_char[count].html, output_file);
			return (1); /* Exit this function */
		}
	}
	/* If we do not have to replace c by a string: */
	putc (c, output_file);
	return (1);
}
