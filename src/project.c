#ifndef __TRIX_PROJECT_C__
#define __TRIX_PROJECT_C__

#include <stdio.h>
#include <stdlib.h>

#include "defines.h"
#include "segment.h"
#include "stage.h"
#include "symbols.h"

#include "file_io.h"
#include "file.h"
#include "workspace.h"
#include "util.h"

#include "options.h"

#include "crypto.h"
#include "seer.h"
#include "project.h"


#include "trixplug_funcs.h"
#include "mem.h"

PROJECT_PLUG_INIT;


t_project *project = NULL;
//char *input_file_path = NULL;
//char *output_file_path = NULL;


unsigned int project_free ( t_project *p )
{
	unsigned int reinit = 0;


	if ( !p ) // any better solution for unload main project from seer ?
	{
		p = project;
		reinit = 1;
	}

	if ( !p )
		return E_FAIL;

	// delete all scripts
	while ( !project_del_file ( p, 0 ) );

	CHECK_AND_FREE ( p->projectname );
	CHECK_AND_FREE ( p->filename );
	CHECK_AND_FREE ( p->settings );
	CHECK_AND_FREE ( p->simple_input );
	CHECK_AND_FREE ( p->simple_output );
	CHECK_AND_FREE ( p->dct3_mcu_input );
	CHECK_AND_FREE ( p->dct3_mcu_output );
	CHECK_AND_FREE ( p->dct3_ppm_input );
	CHECK_AND_FREE ( p->dct3_ppm_output );
	CHECK_AND_FREE ( p->dct4_mcu_input );
	CHECK_AND_FREE ( p->dct4_mcu_output );
	CHECK_AND_FREE ( p->dct4_ppm_input );
	CHECK_AND_FREE ( p->dct4_ppm_output );

	free ( p );

	if ( reinit )
		project_init ( );

	return E_OK;
}

t_project *project_create ( )
{
	t_project *p;

	p = malloc ( sizeof ( t_project ) );
	if ( !p )
		return NULL;

	p->projectname = strdup ( "" );
	p->filename = strdup ( "" );
	p->files = NULL;
	p->num_files = 0;
	p->operationmode = 1;
	p->savemode = 0;
	p->settings = strdup ( "" );

	p->simple_in = 0;
	p->simple_input = strdup ( "" );
	p->simple_out = 0;
	p->simple_output = strdup ( "" );

	p->dct3_in = 0;
	p->dct3_mcu_input = strdup ( "" );
	p->dct3_seperate_ppm = 0;
	p->dct3_mcu_output = strdup ( "" );
	p->dct3_ppm_custom = 0;
	p->dct3_ppm_address = 0;

	p->dct3_out = 0;
	p->dct3_ppm_input = strdup ( "" );
	p->dct3_ppm_output = strdup ( "" );

	p->dct4_in = 0;
	p->dct4_mcu_input = strdup ( "" );
	p->dct4_mcu_output = strdup ( "" );

	p->dct4_out = 0;
	p->dct4_ppm_input = strdup ( "" );
	p->dct4_ppm_output = strdup ( "" );

	return p;
}

unsigned int project_init ( )
{
	t_project *p = project_create ( );
	if ( !p )
		return E_FAIL;

	project_free ( project );
	project = p;

	// options
	//options_add_core_option ( OPT_STR, "project", input_file_path, "Default input files path" );
	//options_add_core_option ( OPT_STR, "project", output_file_path, "Default output files path" );

	return E_OK;
}


/*!
 * add a new script file to the project
 * \return E_OK or E_FAIL
 * \param p current project file
 * \param file the filename of the script to add
 */
unsigned int project_add_file ( t_project *p, char *file )
{
	unsigned int length = 0;
	int symbol = 0;
	scInstance *in = NULL;
	char *script = NULL;
	char *signature = NULL;
	char *new_script = NULL;
	t_workspace *ws = NULL;
	t_project_files *pfile = NULL;
	t_crypt_key pub_key[] = TRIX_PUBLIC_KEYS;

	if ( !p || !file )
		return E_FAIL;

	ws = workspace_startup ( file );
	if ( !ws )
	{
		ui_dlg_msg ( "Can not load the script. Invalid filename?", 0 );
		return E_FAIL;
	}
	if ( !v_get_str ( ws, 0 ) || (strlen ( (char*)v_get_str ( ws, 0 ) ) < 10 ) )
	{
		ui_dlg_msg ( "Can not load the script. Is this a plain textfile?", 0 );
		return E_FAIL;
	}
	script = (char*)strdup ( (char*)v_get_str ( ws, 0 ) );

	pfile = LIST_NEW ( p->files, t_project_files );
	pfile->filename = strdup ( file );
	pfile->settings = strdup ( "" );
	pfile->options = NULL;

	printf ( "_________________________________________________________________\n" );
	signature = crypto_check_integrity ( &script, strlen ( script ), pub_key );
	if ( signature )
	{
		printf ( "[Script]   '%s'\n", file );
		printf ( "    Signed by  '%s'\n", crypto_get_signer ( signature ) );
		printf ( "           for '%s'\n", crypto_get_signee ( signature ) );
		printf ( "           key '%s'\n", crypto_get_signkey ( signature ) );
		printf ( "           opt '%08X'\n", crypto_get_signopt ( signature ) );
	}
	else
	{
		printf ( "[Script]   '%s'\n", file );
		printf ( "           Script is not signed correctly.\n" );
	}

	new_script = (char*) malloc ( strlen ( script ) + 2 );
	strcpy ( new_script, script );
//	strcat ( new_script, "\n" );
	
	free ( script );

	// add script source
	pfile->content = ( new_script );

	// now compile and get options
	pfile->code = scCompile_Text ( new_script, &length );
	if ( pfile->code )
		in = scCreate_Instance ( pfile->code, "" );

	if ( pfile->code && signature && scGet_Symbol ( in, "main" ) )
		pfile->state = PROJ_FILE_SIGNED;
	else if ( pfile->code && scGet_Symbol ( in, "main" ) )
		pfile->state = PROJ_FILE_OK;
	else
		pfile->state = PROJ_FILE_FAILED;

	if ( pfile->code && in )
	{
		symbol = scGet_Symbol ( in, "script_options" );
		if ( symbol != E_FAIL )
			pfile->options = scPtr ( in, symbol, t_options );
		pfile->title = strdup ( scGet_Title ( pfile->code ));
	}
	else
		pfile->title = strdup ( "" );


	if ( !p->files )
		p->files = pfile;

	p->num_files++;

	printf ( "_________________________________________________________________\n\n" );
	return E_OK;
}


/*!
 * get a script file from the project
 * \return pointer to the t_project_files struct or NULL on error
 * \param p current project file
 * \param pos the filenumber of the script (0 to num_files-1)
 */
t_project_files *project_get_file ( t_project *p, unsigned int pos )
{
	t_project_files *pfile = NULL;

	if ( !p )
		return NULL;
	if ( pos >= p->num_files )
		return NULL;

	pfile = p->files;

	while ( pfile )
	{
		if ( pos == 0 )
			return pfile;
		pos--;
		LIST_NEXT(pfile);
	}

	return NULL;
}
/*!
 * del a script file from the project
 * \return E_OK or E_FAIL
 * \param p current project file
 * \param pos the filenumber of the script to del (0 to num_files-1)
 */
unsigned int project_del_file ( t_project *p, unsigned int pos )
{
	t_project_files *pfile = NULL;

	if ( !p )
		return E_FAIL;
	if ( pos >= p->num_files )
		return E_FAIL;

	pfile = p->files;

	while ( pfile )
	{
		if ( pos == 0 )
		{
			LIST_DEL_SIMPLE ( pfile, p->files );
			CHECK_AND_FREE ( pfile->content );
			CHECK_AND_FREE ( pfile->filename );
			CHECK_AND_FREE ( pfile->title );
			CHECK_AND_FREE ( pfile->settings );
			CHECK_AND_FREE ( pfile->code );
			p->num_files--;
			return E_OK;
		}
		pos--;
		LIST_NEXT(pfile);
	}

	return E_FAIL;
}

/*!
 * load project from a file
 * \return NULL on error, else the project struct
 * \param file the filename of the project
 */
t_project *project_load ( char *file )
{
	char line[8192];
	FILE *fh;
	t_project *p;


	if ( !file )
		return NULL;

	fh = fopen ( file, "r" );
	if ( !fh )
		return NULL;

	p = project_create ( );

	while ( fgets ( line, 8192, fh ) )
	{
		line[strlen ( line ) - 1] = 0;

		if ( ( !strlen ( line ) ) || ( line[0] == '#' ) )
			continue;

		if ( !strncmp ( line, "SAVE_MODE\t", 10 ) )
		{
			p->savemode = util_str2int ( line + 10 );
			if ( p->savemode == E_FAIL )
				p->savemode = 0;
		}
		else if ( !strncmp ( line, "OPERATION_MODE\t", 15 ) )
		{
			p->operationmode = util_str2int ( line + 15 );
			if ( p->operationmode == E_FAIL )
				p->operationmode = 0;
		}
		else if ( !strncmp ( line, "SIMPLE_INPUT\t", 13 ) )
		{
			CHECK_AND_FREE ( p->simple_input );
			p->simple_in = line[13] == 'Y' ? 1 : 0;
			p->simple_input = strdup ( line + 15 );
		}
		else if ( !strncmp ( line, "SIMPLE_OUTPUT\t", 14 ) )
		{
			CHECK_AND_FREE ( p->simple_output );
			p->simple_out = line[14] == 'Y' ? 1 : 0;
			p->simple_output = strdup ( line + 16 );
		}
		else if ( !strncmp ( line, "SCRIPT\t", 7 ) )
		{
			project_add_file ( p, line + 7 );
		}
	}

	fclose ( fh );

	return p;
}

/*!
 * save a project to a file
 * \return E_OK or E_FAIL
 * \param p current project file
 * \param file the filename of the project
 */
unsigned int project_save ( t_project *p, char *file )
{
	char buf[8192];
	char path[8192];
	t_project_files *pfile;
	FILE *fh;

	// what a pitty xml is only a plugin :(

	if ( !p || !file )
		return E_FAIL;

	fh = fopen ( file, "w" );
	if ( !fh )
		return E_FAIL;

	fputs ( "# TriX Project File\n", fh );
	//fprintf ( fh, "PROJECT_NAME\t%s\n", p->projectname );

	fputs ( "\n# General\n", fh );
	fprintf ( fh, "SAVE_MODE\t%d\n", p->savemode );
	fprintf ( fh, "OPERATION_MODE\t%d\n", p->operationmode );

	// should we save them all?
	switch ( p->operationmode )
	{
		case 1:
			fputs ( "\n# Simple\n", fh );
			fprintf ( fh, "SIMPLE_INPUT\t%c\t%s\n", p->simple_in ? 'Y' : 'N',   p->simple_input );
			fprintf ( fh, "SIMPLE_OUTPUT\t%c\t%s\n", p->simple_out ? 'Y' : 'N', p->simple_output );
			break;

		case 2:
			fputs ( "\n# DCT3 Phone Firmware\n", fh );
			/* TODO */
			break;

		case 3:
			fputs ( "\n# DCT4 Phone Firmware\n", fh );
			/* TODO */
			break;
	}

	if ( p->num_files )
	{
		fputs ( "\n# Scripts\n", fh );

		pfile = p->files;
		while ( pfile )
		{
			fprintf ( fh, "SCRIPT\t%s\n", pfile->filename );
			LIST_NEXT ( pfile );
		}
	}

	fclose ( fh );
	return E_OK;
}

#endif
