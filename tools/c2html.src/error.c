/*-------------------------------------------------------------------------------
Name               : error.c
Author             : Marvin Raaijmakers
Description        : Handling errors
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
#include <stdlib.h>
#include <stdio.h>
#include <c2html.h>


void
handle_error (ERROR error, char *extra_info)
/*
input:
	error		- The error type
	*extra_info	- A string with extra informating about the error which is
			  sometimes needed for printing an error message
returns:
	-
description:
	This function prints an error message, which is related to the error type, to the
	stderr and exits the program with, an also to the error type related, return value.
*/
{
	/* The elemenst of error_msg are in the same order as in ERROR (see c2html.h */
	char *error_msg[] = {
		/*---------------------------------------------------------------------------------------
		MESSAGE                                                             ERROR
		-----------------------------------------------------------------------------------------*/
		"argument missing\n"
		"Try 'c2html --help' for more information.\n",                   /* ERR_NO_ARG           */
		"too many arguments\n"
		"Try 'c2html --help' for more information.\n",                   /* ERR_2MANY_ARGS       */
		"unrecognized option '%s'\n"
		"Try 'c2html --help' for more information.\n",                   /* ERR_UNREC_ARG        */
		"could not open '%s'\n"
		"Please check your permissions or the existence of the file.\n", /* ERR_OPEN_INPUT_FILE  */
		"could not create '%s'\n"
		"Please check your permissions.\n",                              /* ERR_OPEN_OUTPUT_FILE */
		"could not allocate any memory\n",                               /* ERR_MALLOC           */
		"unknown error\n"
		"Note: If this happends a lot of times in the same or a "
		"simulair situation, please contact me (try 'c2html -a').\n"     /* ERR_UNKNOWN          */
		/*---------------------------------------------------------------------------------------*/
	};

	fprintf (stderr,
		 "c2html:\n"
		 "ERROR: ");
	fprintf (stderr, error_msg[error], extra_info); /* Print the error message */
	exit (error+1); /* Exit the program and return the errorcode */
}
