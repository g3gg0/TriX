#ifndef __TRIX_SEER_C__
#define __TRIX_SEER_C__


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "seer.h"
#include "defines.h"
#include "segment.h"
#include "stage.h"
#include "symbols.h"

#include "file_io.h"
#include "file.h"
#include "workspace.h"
#include "misc.h"
#include "util.h"
#include "elf.h"
#include "crypto.h"
#include "netrix.h"
#include "fmt.h"
#include "treenode.h"
#include "ui.h"
#include "http_lib.h"
#include "options.h"

#include "trixcrypt.h"
#include "trixplug_funcs.h"

#include "mem.h"

int seer_init_trix (  );

extern struct trix_functions *ft;
struct seer_funcs seer_functions;
extern int seer_stack_size;
extern int debug_flag;
extern int seer_memory_check;

int skip_signature_fail = 0;
int signature_details = 1;
int seer_debug_ins = 0;


/*
#########################################
	NOTES about the Seer C format:
#########################################

	A)
		To assign a char* directly a string
			segment->type = "RAW"
		is not possible, use
			segment->type = str("RAW")
		instead.

	B)
		The maximum line length is defined in seer_internal.h
		through the MAXLENGTH define. Exceeding this length will
		cause a crash.

	C)
		Seer does not allow to cast (char) to int's for
		a function call automatically.
			void some_func ( unsigned char val )
		can NOT be called with
			some_func ( 1 )
		since the 1 is treaten as an integer (int) during compilation.

	D)
		The given C script MUST end with a CR, else we would get
		a weird compile error,

	E)
		Functions must not be declared as
			func ( void )
		use
			func (  )
		instead

    F)
        some operators like |= &= ^= are not supported.
        use x = x | y instead.

*/

SEER_PLUG_INIT;


void
MyDispatcher ( int *result, void *function, int *params, int paramcount, unsigned int options )
{
	if ( options & scDispatch_Member )
		options ^= scDispatch_Member;

	scStandardDispatcher ( result, function, params, paramcount, options );
}

/*
    char *str (char *str)
   -----------------------------------------------------

    Internal:
		No

    Description:
        Wrapper for Strings since Seer has problem when
		directly assigning strings.

    Return value:
        returns *str
*/
char *
str (char *str)
{
	return str;
}

/*
    int seer_init (  )
   -----------------------------------------------------

    Internal:
		Yes

    Description:
        Init Seer-Subsystem and register the trix includes.

    Return value:
        returns E_OK on success E_FAIL on error
*/
unsigned int
seer_init (  )
{
	scInit_SeeR ();
	// scOpen_Debug ( "debug.txt" );

	//************************ STDIO ***************************
	scAdd_Internal_Header ( "stdio", ""	);

	seer_init_trix ();

	scAddExtSym ( printf );
	scAddExtSym ( sprintf );
	scAddExtSym ( rand );
	scAddExtSym ( srand );
	scAddExtSym ( strlen );
	scAddExtSym ( scanf );
	scAddExtSym ( sscanf );
	scAddExtSym ( getchar );
	scAddExtSym ( strcpy );
	scAddExtSym ( memcmp );
	scAddExtSym ( memcpy );
	scAddExtSym ( memmove );
	scAddExtSym ( memset );
	scAddExtSym ( strcat );
	scAddExtSym ( strcmp );
	scAddExtSym ( strncmp );
	scAddExtSym ( strtoul );
	scAddExtSym ( strtol );
	scAddExtSym ( strtod );
	scAddExtSym ( atoi );
	scAddExtSym ( atof );
	scAddExtSym ( time );
	scAddExtSym ( system );
	scAddExtSym ( http_put );
	scAddExtSym ( http_parse_url );

	scAdd_External_Symbol ( "say", printf );
	scAdd_External_Symbol ( "realloc", mem_reallocate_seer );
	scAdd_External_Symbol ( "calloc", mem_calloc_seer );
	scAdd_External_Symbol ( "malloc", mem_allocate_seer );
	scAdd_External_Symbol ( "strdup", mem_strduplicate_seer );
	scAdd_External_Symbol ( "free", mem_release_seer );

	scAddExtSym ( str );

	scExport_Kernel();
	scSeeR_Set ( scSeeR_Dispatcher, 1, MyDispatcher );


	options_add_core_option ( OPT_BOOL, "seer", seer_memory_check, "Make TriX crashsafe ;)" );
	options_add_core_option ( OPT_BOOL, "seer", skip_signature_fail, "Skip warning dialog if signature failed" );
	options_add_core_option ( OPT_INT, "seer", signature_details, "Detail level when showing signature (0-4)" );
	options_add_core_option ( OPT_INT, "seer", seer_stack_size, "Script stack size" );
	options_add_core_option ( OPT_INT, "seer", seer_debug_ins, "Insert debug information in compiled scripts. 0=off, 1=file:line, 2=file:line:code (may cause bugs!!)" );
#ifdef DEBUG
	options_add_core_option ( OPT_BOOL, "seer", debug_flag, "verbose debug information" );
#endif
	return E_OK;
}


/*
    int seer_execute ( scScript sc )
   -----------------------------------------------------

    Internal:
		Yes

    Description:
        Run the given C-script.

    Return value:
        returns E_OK on success E_FAIL on error
*/
unsigned int
seer_execute ( scScript sc )
{
	int i;
	unsigned int retval = E_OK;
	t_workspace **ws = NULL;
	t_options *opt = NULL;
	scInstance *in = NULL;

	HEAP_CHECK;
	in = scCreate_Instance ( sc, "" );

	if ( in )
	{
		if ( scGet_Symbol ( in, "trix_workspace" ) != E_FAIL )
		{
			ws = scPtr ( in, scGet_Symbol ( in, "trix_workspace" ), t_workspace* );
			if ( ws )
				*ws = get_workspace ();
		}


		if ( scGet_Symbol ( in, "main" ) == E_FAIL )
		{
			ui_dlg_msg ( " Symbol 'main' is not defined, please create a 'main' routine.", 0 );
			ERR ( 1, " Symbol 'main' is not defined, please create a 'main' routine." );
			scFree_Instance ( in );
			return E_FAIL;
		}


		if ( scGet_Symbol ( in, "script_options" ) != E_FAIL )
		{
			opt = scPtr ( in, scGet_Symbol ( in, "script_options" ), t_options );
			if ( opt )
				options_import_script_options ( "directly_called", opt );
		}

		i = scCall_Instance ( in, scGet_Symbol ( in, "main" ) );

		if ( opt )
			options_delete_script_options ( "directly_called", opt );

		if ( !scErrorNo && i )
		{
			printf ( "\nReturncode: %i\n", i );
			retval = E_FAIL;
		}
	}

	if ( scErrorNo )
	{
		printf ( "\nScript execution failed:\n%s\n", scErrorMsg );
		retval = E_FAIL;
	}


	scFree_Instance ( in );

	return retval;
}

/* function to be called during compilation to display progress bar */
unsigned int
seer_progresscallback ( int percent )
{
	return 1;
}

unsigned int
seer_handle_include ( char **script )
{
	t_crypt_key pub_key[] = TRIX_PUBLIC_KEYS;
	char *signature = NULL;

	signature = crypto_check_integrity ( script, strlen ( *script ), pub_key );
	if ( signature )
	{
		if ( signature_details >= 1 )
			printf ( "[TriX]  Signed by '%s' ", crypto_get_signer ( signature ) );
		if ( signature_details >= 2 )
			printf ( "for '%s' ", crypto_get_signee ( signature ) );
		if ( signature_details >= 3 )
			printf ( "key '%s' ", crypto_get_signkey ( signature ) );
		if ( signature_details >= 4 )
			printf ( "opt %08X", crypto_get_signopt ( signature ) );
		if ( signature_details >=1 )
			printf ( "\n" );
	}
	else
	{
		if ( !skip_signature_fail )
		{
			if ( ui_dlg_bool ( "This script is not signed correctly.\nAre you sure you want to execute it?\n" ) != E_OK )
				return E_FAIL;
		}
	}

	return E_OK;
}
/*
    int seer_run ( char *script )
   -----------------------------------------------------

    Internal:
		Yes

    Description:
        Run the given C-script.

    Return value:
        returns E_OK on success E_FAIL on error
*/
unsigned int
seer_run ( char *script )
{
	int length = 0;
	scScript sc;
	char *new_script = NULL;
	unsigned int oldstate = 0;
	unsigned char oldmark[2];

	seer_allow_script ();

	mem_get_state_mark ( &oldstate, oldmark );
	mem_set_state_mark ( 0xDEADBEEF, "sc" );
	// check signature first
	R(seer_handle_include ( &script ));

	new_script = malloc ( strlen ( script ) + 2 );
	strcpy ( new_script, script );
	strcat ( new_script, "\n" );

	sc = scCompile_Text ( new_script, &length );
	free ( new_script );

	if (!sc)
	{
		printf ( "[TriX]  Error during compilation:\n%s\nIn line:%s\n", scErrorMsg, scErrorLine );
		return E_FAIL;
	}

	printf ( "----------------------------------------------------\n\n" );
 	seer_execute ( sc );
	printf ( "\n\n----------------------------------------------------\n" );
	scFree_Script ( sc );

//	mem_release_tagged ();
	mem_set_state_mark ( oldstate, oldmark );

	return E_OK;
}


unsigned int seer_add_header ( unsigned char *section, unsigned char *header )
{
	scAdd_Internal_Header ( section, header );
	return E_OK;
}

unsigned int seer_add_symbol_int ( char *name, void *func )
{
	scAdd_External_Symbol_Int ( name, func );
	return E_OK;
}

unsigned int seer_del_header ( unsigned char *section )
{
	scDel_Internal_Header ( section );
	return E_OK;
}

unsigned int seer_del_symbol_int ( char *name )
{
	scDel_External_Symbol_Int ( name );
	return E_OK;
}

#endif
