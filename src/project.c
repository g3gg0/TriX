#ifndef __TRIX_PROJECT_C__
#define __TRIX_PROJECT_C__

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


unsigned int project_init ( )
{
	project = malloc ( sizeof ( t_project ) );

	project->projectname = strdup ( "" );
	project->filename = strdup ( "" );
	project->files = NULL;
	project->num_files = 0;
	project->operationmode = 0;
	project->savemode = 0;
	project->settings = strdup ( "" );

	project->simple_in = 0;
	project->simple_input = strdup ( "" );
	project->simple_out = 0;
	project->simple_output = strdup ( "" );

	project->dct3_in = 0;
	project->dct3_mcu_input = strdup ( "" );
	project->dct3_seperate_ppm = 0;
	project->dct3_mcu_output = strdup ( "" );
	project->dct3_ppm_custom = 0;
	project->dct3_ppm_address = 0;

	project->dct3_out = 0;
	project->dct3_ppm_input = strdup ( "" );
	project->dct3_ppm_output = strdup ( "" );

	project->dct4_in = 0;
	project->dct4_mcu_input = strdup ( "" );
	project->dct4_mcu_output = strdup ( "" );

	project->dct4_out = 0;
	project->dct4_ppm_input = strdup ( "" );
	project->dct4_ppm_output = strdup ( "" );

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
	return NULL;
}

/*!
 * save a project to a file
 * \return E_OK or E_FAIL
 * \param p current project file
 * \param file the filename of the project
 */
unsigned int project_save ( t_project *p, char *file )
{
	return E_FAIL;
}

#endif