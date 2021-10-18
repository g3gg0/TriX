/*-------------------------------------------------------------------------------
Name               : check_begin.c
Author             : Marvin Raaijmakers
Description        : Checks for a begin of every codepart
Date of last change: 19-December-2004
History            :
                     19-December-2004  Added function check_begin_identif()
                                       created by David Ireland

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
#include <string.h>
#include <c2html.h>


const char esc = '\\';

/*---------------------------------------------------------------------------------------------
Prototypes of the functions in this file with a file scope
-----------------------------------------------------------------------------------------------*/
static char ishyphen (char c);
static char check_begin_keyword_type_specif (char line[], const char *array[], int array_size);
/*---------------------------------------------------------------------------------------------*/

const char *type_specifier[NUM_TYPE_SPECIFIERS] = {
	"auto",
	"char",
	"const",
	"double",
	"float",
	"int",
	"long",
	"register",
	"short",
	"signed",
	"static",
	"unsigned",
	"void",
	"volatile"
};
const char *keyword[NUM_KEYWORDS] = {
	"break",
	"case",
	"continue",
	"default",
	"do",
	"else",
	"enum",
	"exit",
	"extern",
	"for",
	"goto",
	"if",
	"return",
	"sizeof",
	"struct",
	"switch",
	"typedef",
	"import",
	"union",
	"while"
};

/* These chars may not appear directly before or after a keyword or type specifier */
const char hyphens[NUM_HYPHENS] = {'_', '@', '$', '\'', '"', '`'};

/*
   There are two types of comments: those who start with "/*" and those who start with "//"
   If the current comment starts with "/*" comment1 is TRUE otherwise it is FALSE
*/
char comment1;


static char
ishyphen (char c)
/*
input:
	c	- The character to check
returns:
	TRUE if c is a hyphen, FALSE if it isn't
description:
	This function checks if c is a hyphen.
*/
{
	int count;

	/* Check if c is not a letter */
	if ( isalpha(c) )
	{
		return (TRUE);
	}
	/* Check if c is not a hyphen */
	for (count = 0; (count < NUM_HYPHENS); count++)
	{
		if (c == hyphens[count]) /* If c is a hyphen */
		{
			return (TRUE);
		}
	}
	/* If c is not a hyphen: */
	return (FALSE);
}


char
isbeginend (const char beginend[], char line[])
/*
input:
	beginend	- The function checks if this string appears at the beginning of line
	line		- The function checks if beginend appears at the beginning of this string
returns:
	TRUE if beginend appears at the beginning of line, FALSE if it doesn't
description:
	This function checks if beginend appears at the beginning of line.
*/
{
	int count;

	for (count = 0; beginend[count]; count++)
	{
		if (line[count] != beginend[count])
		{
			return (FALSE);
		}
	}
	return (TRUE);
}

char
check_begin_comment (char *line)
/*
input:
	line	- The string to check
returns:
	TRUE if *line is the beginning of a comment, FALSE if it isn't
description:
	This function checks if *line is the beginning of a comment.
*/
{
	const char *begin1 = "/*";
	const char *begin2 = "//";

	/*
	   If line is the beginning of a comment which begins with "/*"
	*/
	if ( isbeginend(begin1, line) )
	{
		comment1 = TRUE;
		return (TRUE); /* line is the beginning of a comment */
	}
	comment1 = FALSE; /* line is definitely not the beginnig of a comment starting with "/*" */

	/* line can now only be the beginning of a comment which begins with "//" */
	return ( isbeginend(begin2, line) );
}


char
check_begin_keyword_type_specif (char line[], const char *array[], int array_size)
/*
input:
	line		- The string to check
	*array		- The array of the strings to compare the beginning of line with
	array_size	- The number of elements of array
returns:
	TRUE if the beginning of line contains a element of array, FALSE if it doesn't
description:
	The function checks if the beginning of line equals to one of the elemensts of array.
*/
{
	int count;

	/* Check if the character before *line is a hyphen */
	if ( ishyphen( *(line-1) ) )
	{
		/* Because a hyphen may not appear before a keyword/type specifier, return FALSE */
		return (FALSE);
	}

	/* Check for every keyword/type specifier if line is the beginning of it */
	for (count = 0; count < array_size; count++)
	{
		if (isbeginend( array[count], line )) /* Is line is the beginning of array[count] */
		{
			/* Check if the first character after the keyword/type specifier in line is not a hyphen */
			if ( ishyphen( line[strlen( array[count] )/*-1*/] ) == FALSE )
			{
				/* Because *line is a beginning of a keyword/type specifier: */
				return (TRUE);
			}
		}
	}
	/* Because *line is not a vallid beginning of a keyword/type specifier: */
	return (FALSE);
}

char
check_begin_keyword (char line[])
/*
input:
	line	- The string to check
returns:
	TRUE if *line is the beginning of a keyword, FALSE if it isn't
description:
	This function checks if *line is the beginning of a keyword.
*/
{
	return ( check_begin_keyword_type_specif( line, keyword, NUM_KEYWORDS ) );
}


char
check_begin_type_specif (char line[])
/*
input:
	line	- The string to check
returns:
	TRUE if *line is the beginning of a type specifier, FALSE if it isn't
description:
	This function checks if *line is the beginning of a type specifier.
*/
{
	return ( check_begin_keyword_type_specif( line, type_specifier, NUM_TYPE_SPECIFIERS ) );
}

char
check_begin_char (char line[])
/*
input:
	line	- The string to check
returns:
	TRUE if *line is the beginning of a character, FALSE if it isn't
description:
	This function checks if *line is the beginning of a character.
*/
{
	const char begin = '\'';
	const char end = '\'';

	return ( (line[0] == begin) && ( (line[2] == end) || ((line[3] == end) && (line[1] == esc)) ) );
}


char
check_begin_oct (char line[])
/*
input:
	line	- The string to check
returns:
	TRUE if *line is the beginning of an octal, FALSE if it isn't
description:
	This function checks if *line is the beginning of an octal.
*/
{
	const char zero = '0';
	const char seven = '7';
	int count;

	/* Check if the character before *line is a hyphen */
	if ( ishyphen(*(line-1)) )
	{
		return (FALSE);
	}
	if (line[0] == zero) /* The first digit of an octal must be a zero */
	{
		for (count = 1; isdigit(line[count]); count++)
		{
			if (line[count] > seven) /* A digit in an octal may not be greater then 7 */
			{
				/* *line is not the beginning of an octal */
				return (FALSE);
			}
		}
		/* *line is a beginning of an octal if not only the zero read*/
		if (count > 1)
		{
			return (TRUE);
		}
	}
	/* *line is not the beginning of an octal */
	return (FALSE);
}


char
check_begin_int (char line[])
/*
input:
	line	- The string to check
returns:
	TRUE if *line is the beginning of an integer, FALSE if it isn't
description:
	This function checks if *line is the beginning of an integer.
*/
{
	int count;

	/* Check if the character before *line is a hyphen */
	if ( ishyphen(*(line-1)) )
	{
		return (FALSE);
	}
	for (count = 0; isdigit(line[count]); count++)
		; /* NULL Statement */
	/* If 1 only one character is a digit */
	if (count == 1)
	{
		/* *line is a beginning of an integer but only consists of one digit */
		return (ONLY1CHAR);
	}
	else if (count) /* If at leased 2 characters are a digit */
	{
		/* *line is a beginning of an integer */
		return (TRUE);
	}
	/* *line is not a beginning of an integer */
	return (FALSE);
}


char
check_begin_float (char line[])
/*
input:
	line	- The string to check
returns:
	TRUE if *line is the beginning of a float, FALSE if it isn't
description:
	This function checks if *line is the beginning of a float.
*/
{
	char point_found;
	int count;

	/* Check if the character before *line is a hyphen */
	if ( ishyphen(*(line-1)) )
	{
		return (FALSE);
	}

	point_found = FALSE;
	for (count = 0; isdigit(line[count]) || (line[count] == '.'); count++)
	{
		if (line[count] == '.')
		{
			point_found = TRUE;
		}
	}
	/*
	   *line is the beginning of a float if:
	   - at least 2 chars where read
	   - a point was found
	*/
	return ( (count > 1) && point_found );
}


char
check_begin_hex (char line[])
/*
input:
	line	- The string to check
returns:
	TRUE if *line is the beginning of a hexadecimal, FALSE if it isn't
description:
	This function checks if *line is the beginning of a hexadecimal.
*/
{
	const char *begin1 = "0x";
	const char *begin2 = "0X";
	const char highest_letter = 'F';

	/* Check if the character before *line is a hyphen */
	if ( ishyphen(*(line-1)) )
	{
		return (FALSE);
	}
	if ( isbeginend(begin1, line) || isbeginend(begin2, line) )
	{
		/* If the character after 0x or 0X is a valid hexadecimal */
		if ( isdigit(line[2]) || ( isalpha(line[2]) && (line[2] <= highest_letter) ) )
		{
			/* *line is a beginning of a hexadecimal */
			return (TRUE);
		}
	}
	/* *line is not a beginning of a hexadecimal */
	return (FALSE);
}


char
check_begin_preprocessor (char line[])
/*
input:
	line	- The string to check
returns:
	TRUE if *line is the beginning of a preprocessor line, FALSE if it isn't
description:
	This function checks if *line is the beginning of a preprocessor line.
*/
{
	const char begin = '#';

	return (line[0] == begin);
}


char
check_begin_str (char line[])
/*
input:
	line	- The string to check
returns:
	TRUE if *line is the beginning of a string, FALSE if it isn't
description:
	This function checks if *line is the beginning of a string.
*/
{
	const char begin = '"';

	return (line[0] == begin);
}

char
check_begin_identif (char line[])
/*
input:
	line	- The string to check
returns:
	TRUE if *line is the beginning of an identifier, FALSE if it isn't
description:
	This function checks if *line is the beginning of a idenitfier.
*/
{
	/* Check if the character before *line is a hyphen */
	if ( ishyphen(*(line-1)) )
	{
		return (FALSE);
	}
	/* If next char is [A-Za-z_] then we have an identifier */
	if ( (isalpha(line[0]) || line[0] == '_') )
	{
		/* Check for just a single char identifier */
		if ( (!isalnum(line[1]) && line[1] != '_') )
			return (ONLY1CHAR);
		else
			return (TRUE);
	}
	else
		return (FALSE);
}
