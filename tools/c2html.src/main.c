/*-------------------------------------------------------------------------------
Name               : main.c
Author             : Marvin Raaijmakers
Description        : Startup
Date of last change: 11-March-2004

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
#include <string.h>
#include <c2html.h>

/*--------------------------------------------------------------------------
Prototypes of the functions in this file with a file scope
----------------------------------------------------------------------------*/
static char *create_output_filename (char *input_filename);
static char *process_command_line (int argc, char *argv[]);
/*--------------------------------------------------------------------------*/


static char
*create_output_filename (char *input_filename)
/*
input:
	*input_filename	- The name of the input file
returns:
	the output filename
description:
	This function creates a output filename by appending the html
	file-extention to the input filename.
*/
{
	char *output_filename;

	/* Allocate memory for the output filename */
	output_filename = (char *) malloc( strlen(input_filename) + strlen(HTML_EXTENTION) + 1 );
	if (output_filename == NULL) /* If no memory has been allocated */
	{
		/* A message will be printed to stderr and the program will exit */
		handle_error (ERR_MALLOC, NULL);
	}
	strcpy (output_filename, input_filename); /* Copy the input_filename to the output_filename */
	/* Append the html file-extention to the end of the output filename */
	strcat (output_filename, HTML_EXTENTION);
	return (output_filename);
}


static char
*process_command_line (int argc, char *argv[])
/*
input:
	argc	- The number of arguments
	*argv	- An array of arguments
returns:
	The name of the specified input file (if specified)
description:
	This function reads the name of the input file from the command line and
	returns the address of the location where it's stored. If a vallid argument
	is specified instead of the input filename, this function calls the function
	which belongs to the argument and exits the program.
*/
{
	char *argument;

	if (argc == 1) /* If there is no argument */
	{
		/* A message will be printed to stderr and the program will exit */
		handle_error (ERR_NO_ARG, NULL);
	}
	if (argc > MAX_NUM_ARGS) /* If the too many arguments are specified */
	{
		/* A message will be printed to stderr and the program will exit */
		handle_error (ERR_2MANY_ARGS, NULL);
	}

	argument = argv[1];
	if (argv[1][0] == '-') /* If an argument is specified */
	{
		/* Check now which argument is specified */
		if ( (strcmp( argument, "-h" ) == EQUAL) || (strcmp( argument, "--help") == EQUAL) )
		{
			print_help();
			exit (0);
		}
		else if (strcmp( argument, "-a") == EQUAL)
		{
			print_about();
			exit (0);
		}
		else if (strcmp( argument, "--version") == EQUAL)
		{
			print_version();
			exit (0);
		}
		else
		{
			/* There is an unrecognized argument specified
			   A message will be printed to stderr and the program will exit */
			handle_error (ERR_UNREC_ARG, argument);
		}
	}

	return (argument);
}


int
main (int argc, char *argv[])
{
	FILE *input_file, *output_file;
	char *input_filename, *output_filename;


	input_filename = "unknown";
	output_filename = "unknown";
//	input_filename = process_command_line(argc, argv);
//	input_file = fopen(input_filename, "r"); /* Open the input file */
	input_file = stdin;
	if (input_file == NULL)
	{
		/* A message will be printed to stderr and the program will exit */
		handle_error (ERR_OPEN_INPUT_FILE, input_filename);
	}
	output_filename = create_output_filename (input_filename); /* Create the output filename */
	
	output_file = stdout;
//	output_file = fopen(output_filename, "w"); /* Open the output file */
	if (output_file == NULL)
	{
		/* A message will be printed to stderr and the program will exit */
		handle_error (ERR_OPEN_OUTPUT_FILE, output_filename);
	}

	/* Write the header of the html-file */
	write_html_header (output_file, input_filename);
	/* Convert the input file to html an write it into the output file */
	convert (input_file, output_file);
	/* Write the footer of the html-file */
	write_html_footer (output_file);

/*
	fclose (input_file);
	fclose (output_file);
*/
	exit (0);
}
