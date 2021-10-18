#ifndef __TRIX_MAIN_C__
#define __TRIX_MAIN_C__

/*! \mainpage TriX DoX
 *
 * This is the TriX Documentation.\n
 * We hope you will enjoy it and that it will help you creating your own patches ;)\n
 * \n
 * All documentation is usually for internal use.\n
 * The TriX user documentation can be found in the \link user.dox\endlink.\n
 */

#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "main.h"
#include "defines.h"
#include "segment.h"
#include "stage.h"
#include "symbols.h"

#include "file_io.h"
#include "file.h"
#include "workspace.h"
#include "util.h"
#include "misc.h"
#include "elf.h"
#include "http_lib.h"
#include "crypto.h"
#include "workspace.h"
#include "netrix.h"

#include "mem.h"


t_workspace *trix_workspace = NULL;


t_workspace *
get_workspace (  )
{
	return trix_workspace;
}

unsigned int
test_run (  )
{
	char *script_data = NULL;
	t_stage *stage = NULL;
	t_workspace *ws = NULL, *ws2 = NULL;
	char *script = "test.trx";
	char *flash = "test.fls";

	char *out = "test_out.fls";
	char *part = "test_part.fls";

	unsigned int length = 0;

	clock_t val = 0, val_tot = 0;
	object *o = NULL;

    val_tot = clock();

//	return netrix_execute ( "http://trix.g3gg0.de/main" );


	printf ( "Loading '%s'...\n", script );
	ws = workspace_startup ( script );
	if ( !ws )
        return E_FAIL;
	printf ( "Loading '%s'...\n", flash );
	ws2 = workspace_startup ( flash );

	if ( !ws2 )
        return E_FAIL;

	LIST_ADD(ws, ws2);


	if ( !ws || v_get_size(ws) == E_FAIL )
		return E_FAIL;

	script_data = malloc ( v_get_size(ws) + 2 );
	if ( !script_data )
		return E_FAIL;

	v_memcpy_get ( ws, script_data, v_get_start(ws), v_get_size(ws) );
	script_data[v_get_size(ws)] = '\000';
	strcat ( script_data, "\n" );

	trix_workspace = ws->next;
	if ( !trix_workspace )
		return E_FAIL;

	printf ( "Executing Script...\n" );
	printf ( "----------------------------------------------------\n" );
	val = clock ();
	seer_run ( script_data );
	val = clock () - val;
	printf ( "----------------------------------------------------\n" );
	printf ( "Clocks: %i\n", val );

//	printf ( "Saving Full Flash    '%s'...\n", out );
//	file_write ( out, trix_workspace->fileinfo );
	trix_workspace->fileinfo->options |= FILE_DIFF;
	printf ( "Saving Partial Flash '%s'...\n", part );
	file_write ( part, trix_workspace->fileinfo );
	printf ( "\nTotal clocks: %i\n", clock() - val_tot );
	printf ( "\nDone\n", part );


	file_io_release_all ( ws->fileinfo );
	if ( ws->next )
	{
		ws = ws->next;
		file_io_release_all ( ws->fileinfo );
		free ( ws->prev );
	}
	free ( ws );
	free ( script_data );


	getchar();
    return E_OK;
}

unsigned int
trix_run ( char *script, char *flash )
{
	char *script_data = NULL;
	t_stage *stage = NULL;
	t_fileinfo *fi = NULL;
	t_workspace *ws = NULL, *ws2 = NULL;
	char *out = "test_out.fls";
	char *part = "test_part.fls";
	int length = 0;

	clock_t val = 0, val_tot = 0;
	object *o = NULL;

    val_tot = clock();

	printf ( "[TriX]  Loading '%s'...\n", script );

	ws = workspace_startup ( script );
	if ( !ws )
        return E_FAIL;

	if ( flash )
	{
		printf ( "[TriX]  Loading '%s'...\n", flash );
		ws2 = workspace_startup ( flash );

		if ( !ws2 )
			return E_FAIL;

		LIST_ADD(ws, ws2);
	}

	if ( !ws || v_get_size(ws) == E_FAIL )
		return E_FAIL;

	script_data = malloc ( v_get_size(ws) + 2 );
	if ( !script_data )
		return E_FAIL;

	v_memcpy_get ( ws, script_data, v_get_start(ws), v_get_size(ws) );
	script_data[v_get_size(ws)] = '\000';

	trix_workspace = ws->next;

//	printf ( "[TriX]  Executing Script...\n" );
//	printf ( "----------------------------------------------------\n" );
	val = clock ();
	seer_run ( script_data );
	val = clock () - val;
//	printf ( "----------------------------------------------------\n" );
//	printf ( "[TriX]  Script Clocks: %i\n", val );

	if ( trix_workspace )
	{
		printf ( "[TriX]  Saving Full Flash    '%s'...\n", out );
		trix_workspace->fileinfo->options &= ~FILE_DIFF;
		file_write ( out, trix_workspace->fileinfo );

		printf ( "[TriX]  Saving Partial Flash '%s'...\n", part );
		trix_workspace->fileinfo->options |= FILE_DIFF;
		file_write ( part, trix_workspace->fileinfo );
	}
//	printf ( "\n[TriX]  Total Clocks: %i\n", clock() - val_tot );

	file_io_release_all ( ws->fileinfo );
	if ( ws->next )
	{
		ws = ws->next;
		file_io_release_all ( ws->fileinfo );
		free ( ws->prev );
	}
	free ( ws );
	free ( script_data );

//	getchar();
    return E_OK;
}



/**
* Initializing most of the parts.
*/
unsigned int
main_setup (  )
{
	mem_init ();
	fmt_init ();
	seer_init ();
	arm_init ();
	nokia_init ();
	http_init ();
	trixplug_init ();
	project_init ();
	util_init ();

	options_init ();

	printf ( "\n" );
	printf ( "[TriX] -------------------------------------\n" );
	printf ( "[TriX] -           TriX  "__TRIX_VERSION__"             -\n" );
	printf ( "[TriX] -   (c) 2006 by g3gg0 and krisha    -\n" );
	printf ( "[TriX] -------------------------------------\n" );
	printf ( "\n" );

	return E_OK;
}



/**
* Cleanup of most of the parts.
*/
unsigned int
main_cleanup (  )
{
	/*
    if ( !mem_check_all () )
	getchar();

	*/
   
   return E_OK;
}


/* Qt version has its own main() in ui-qt/ui_main.c */
#ifndef QT_CORE_LIB

/**
* The main function. Command line version.
*/
int
main ( int argc, char *argv[] )
{
   int ret, r;

   ret = main_setup();
   if ( ret == E_OK )
   {
      if ( argc == 1 )
      {
         printf ( "[TriX]  No parameters given. Enter NeTriX mode...\n" );
         ret = trix_run ( "http://trix.g3gg0.de/main", NULL );
      }
      else if ( argc == 2 )
      {
         printf ( "[TriX]  simple mode...\n" );
         ret = trix_run ( argv[1], NULL );
      }
      else if ( argc == 3 )
      {
         printf ( "[TriX]  simple mode with flashfile...\n" );
         ret = trix_run ( argv[1], argv[2] );
      }
      else
      {
         printf ( "[TriX]  sorry not supported yet ;)\n" );
         ret = E_FAIL;
      }

      r = main_cleanup();
      if ( ret == E_OK && r != E_OK ) ret = r;
   }

   return ret;
}

#endif /* QT_CORE_LIB */


#endif
