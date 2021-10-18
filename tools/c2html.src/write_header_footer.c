/*-------------------------------------------------------------------------------
Name               : write_header_footer.c
Author             : Marvin Raaijmakers
Description        : Writes the html header/footer to the output file
Date of last change: 12-March-2004

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
#include <c2html.h>

void
write_html_header (FILE *output_file, char *title)
/*
input:
	*output_file	- The file to write the header to
	*title		- The title of the html-file
returns:
	-
description:
	This function writes a html header, with title as the title of
	the html-file, to output_file.
*/
{
	fprintf (output_file,
/*		"<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">\n"
		"<html>\n"
		"	<head>\n"
		"		<title>%s</title>\n"
		"		<meta name=\"generator\" content=\"c2html %s\" >\n"
		"		<meta http-equiv=\"content-type\" content=\"text/html; charset=ISO-8859-1\" >\n"
		"		<meta http-equiv=\"Content-Script-Type\" content=\"text/javascript\" >\n"
		"		<meta http-equiv=\"Content-Style-Type\" content=\"text/css\" >\n"
		"	</head>\n"
		"	<body text=\"%s\">\n"*/
		"		<font face=\"%s\">\n",
		/*title, VERSION, BLACK, */ FONT_FACE);
}


void
write_html_footer (FILE *output_file)
/*
input:
	*output_file	- The file to write the header to
returns:
	-
description:
	This function writes a html footer to output_file.
*/
{
	fprintf (output_file,
		"		</font>\n"
/*		"	</body>\n"
		"</html>\n"
*/
		 );
}
