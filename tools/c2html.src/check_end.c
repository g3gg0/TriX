/*-------------------------------------------------------------------------------
Name               : check_end.c
Author             : Marvin Raaijmakers
Description        : Checks for a end of every codepart
Date of last change: 19-December-2004
History            :
                     19-December-2004  Added function check_end_identif() created
                                       by David Ireland

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
#include <ctype.h>
#include <c2html.h>

/*--------------------------------------------------------------------------
Shared variables of check_begin.c
----------------------------------------------------------------------------*/
extern const char esc;
/*--------------------------------------------------------------------------*/

char
check_end_comment (char line[])
/*
input:
	line		- The string to check
globals:
	comment1	- To indicate what kind of comment to check the end for
returns:
	TRUE if *line is the end of a comment, FALSE if it isn't
description:
	This function checks if *line is the end of a comment.
*/
{
	const char *end1 = "*/";
	const char  end2 = '\n';

	return ( (isbeginend(end1, (line-1)) && comment1) || ((*line == end2) && (comment1 == FALSE)) );
}


char
check_end_keyword (char line[])
/*
input:
	line	- The string to check
returns:
	TRUE if *line is the end of a keyword, FALSE if it isn't
description:
	This function checks if *line is the end of a keyword.
*/
{
	/* *line is the end of a keyword if the next character is not a letter */
	return ( !isalpha(line[1]) );
}


char
check_end_type_specif (char line[])
/*
input:
	line	- The string to check
returns:
	TRUE if *line is the end of a type specfier, FALSE if it isn't
description:
	This function checks if *line is the end of a type specifier.
*/
{
	/* *line is the end of a type specifier if the next character is not a letter */
	return ( !isalpha(line[1]) );
}


char
check_end_char (char line[])
/*
input:
	line	- The string to check
returns:
	TRUE if *line is the end of a character, FALSE if it isn't
description:
	This function checks if *line is the end of a character.
*/
{
	const char end = '\'';

	/*
	   *line is the end of a char if it's the endcharacter of a char and
	   the previous character it is not a escapecharacter
	*/
	return ( (*line == end) && !((*(line-1) == esc) && (*(line-2) != esc)) );
}


char
check_end_oct (char line[])
/*
input:
	line	- The string to check
returns:
	TRUE if *line is the end of an octal, FALSE if it isn't
description:
	This function checks if *line is the end of an octal.
*/
{
	/* *line is the end of an octal is the next character is not a digit or greater than 7 */
	return ( !isdigit(line[1]) || (line[1] > '7') );
}


char
check_end_int (char line[])
/*
input:
	line	- The string to check
returns:
	TRUE if *line is the end of an integer, FALSE if it isn't
description:
	This function checks if *line is the end of an integer.
*/
{
	/* *line is the end of an integer is the next character is not a digit */
	return ( !isdigit(line[1]) );
}


char
check_end_float (char line[])
/*
input:
	line	- The string to check
returns:
	TRUE if *line is the end of a float, FALSE if it isn't
description:
	This function checks if *line is the end of a float.
*/
{
	const char point = '.';

	/* *line is the end of a float is the next character is not a digit or a point */
	return ( !(isdigit(line[1]) || (line[1] == point)) );
}


char
check_end_hex (char line[])
/*
input:
	line	- The string to check
returns:
	TRUE if *line is the end of a hexadecimal, FALSE if it isn't
description:
	This function checks if *line is the end of a hexadecimal.
*/
{
	const char highest_letter = 'F';

	/* *line is the end of a hexadecimal if the next character is not a valid hexadecimal */
	return ( !( isdigit(line[1]) || (isalpha(line[1]) && (line[1] <= highest_letter)) ) );
}


char
check_end_preprocessor (char line[])
/*
input:
	line	- The string to check
returns:
	TRUE if *line is the end of a preprocessor line, FALSE if it isn't
description:
	This function checks if *line is the end of a preprocessor line.
*/
{
	const char end = '\n';

	/*
	   *line is the end of a preprocessorline if it's a newline-character and
	   the previous character it is not a escapecharacter, or the next character
	   is the beginning of a comment
	*/
	return ( ((*line == end) && (*(line-1) != esc)) || ( check_begin_comment(line+1) ) );
}


char
check_end_str (char line[])
/*
input:
	line	- The string to check
returns:
	TRUE if *line is the end of a string, FALSE if it isn't
description:
	This function checks if *line is the end of a string.
*/
{
	const char end = '\"';

	/*
	   *line is the end of a string if it's a newline-character or an endstring-character
	   and the previous character it is not a escapecharacter
	*/
	return ( (*line == '\n') || ( (*line == end) && !((*(line-1) == esc) && (*(line-2) != esc)) ) );
}


char
check_end_identif (char line[])
/*
input:
	line	- The string to check
returns:
	TRUE if *line is the end of an identifier, FALSE if it isn't
description:
	This function checks if *line is the end of a idenitfier.
*/
{
	/*
	   *line is the end of an identifier if he next character is not [A-Za-z0-9_]
	*/
	return ( (!isalnum(line[1]) && line[1] != '_') );
}
