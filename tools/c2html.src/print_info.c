/*-------------------------------------------------------------------------------
Name               : print_info.c
Author             : Marvin Raaijmakers
Description        : Printing information
Date of last change: 17-March-2004

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
print_help (void)
{
	printf ("c2html, version %s\n\n"
		"usage: c2html [file]     convert the specified file\n"
		"   or: c2html [argument]\n\n"
		"Arguments:\n"
		"   -a                    Print gerneral information and exit\n"
		"   -h, --help            Print Help (this message) and exit\n"
		"   --version             Print version information and exit\n",
		VERSION );
}

void
print_about (void)
{
	printf (".--C2HTML----------------------------------------------------.\n"
		"|                                                            |\n"
		"|   Version: %.5s                                           |\n"
		"|        By: Marvin Raaijmakers                              |\n"
		"|      Year: 2004                                            |\n"
		"|   Contact: marvin_raaijmakers@lycos.nl                     |\n"
		"|   License: GPL >= v2 (http://www.gnu.org/licenses/gpl.txt) |\n"
		"| Thanks to: Wil Koenen                                      |\n"
		"|                                                            |\n"
		"'------------------------------------------------------------'\n",
		VERSION );
}

void
print_version (void)
{
	printf ("c2html, version %s\n", VERSION);
}
