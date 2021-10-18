

#include "ui_build_ppm_progress.h"

#ifdef WIN32
#include <Windows.h>
#endif
extern "C"
{
  

#include "defines.h"
#include "segment.h"
#include "stage.h"
#include "symbols.h"

#include "file_io.h"
#include "file.h"
#include "fmt.h"
#include "workspace.h"
#include "util.h"
#include "main.h"
#include "ui.h"
#include "crypto.h"
#include "seer.h"
#include "options.h"
#include "mutex.h"
#include "treenode.h"
#include "trixplug.h"

#include "http_lib.h"
#include "md5.h"

#include "project.h"
#include "ppmodd_net.h"

#include "mem.h"

//
// Plugin Symbols
//
//////////////////////////////////////////////////////////////////////////////////////////

void *ppm_tempsymbol;
char ppm_errstring[1024];

#define LOAD_PLUGIN_SYMBOL(plugin,symbol,dest)\
	ppm_tempsymbol = trixplug_get_plugin_symbol ( plugin, symbol ); \
	if ( !ppm_tempsymbol )\
	{\
		sprintf ( ppm_errstring, "[ERROR] Missing symbol '%s' in Plugin '%s'\n", symbol, plugin );\
		printf ( ppm_errstring );\
		ui_dlg_msg ( ppm_errstring, 0 );\
		return;\
	}\
	else\
		memcpy ( &dest, &ppm_tempsymbol, sizeof (ppm_tempsymbol) );\

};

#include "ui_BuildPPMProgress.h"



PPModd_Build_Thread::PPModd_Build_Thread ( t_treenode *node, unsigned char *username, unsigned char *password )
{
	thread_abort = 0;
	thread_active = 0;
	thread_finished = 0;
	treenode = node;
	result_treenode = NULL;
	user = username;
	pass = password;
	encrypted = 0;

	process_message = "Idle";
	process_percent = 0;
}

const char *PPModd_Build_Thread::process_msg ()
{
	return process_message;
}

unsigned int PPModd_Build_Thread::process_pct ()
{
	return process_percent;
}
t_treenode *PPModd_Build_Thread::get_node ()
{
	return result_treenode;
}

unsigned char* PPModd_Build_Thread::TriX_Site_Login ( unsigned char *username, unsigned char *password )
{
	unsigned char *id = ppmodd_net_login ( username, password );

	if ( !id )
		return NULL;

	memcpy ( this->login_id, id, 33 );
	free ( id );

	return this->login_id;
}


int PPModd_Build_Thread::is_active ()
{
	return thread_active;
}
int PPModd_Build_Thread::is_finished ()
{
	return thread_finished;
}

void PPModd_Build_Thread::abort()
{
	if ( is_active() && !is_finished() )
		thread_abort = 1;
}

void PPModd_Build_Thread::reset()
{
	if ( is_active() && !is_finished() )
	{
		ui_dlg_msg ( "Still running!", 0 );
		return;
	}

	thread_active = 0;
	thread_finished = 0;

}

void PPModd_Build_Thread::run()
{
	int pos = 0;
	int file_pos = 0;
	t_fileinfo *fi = NULL;

	if ( is_active() )
	{
		ui_dlg_msg ( "Already running!", 0 );
		return;
	}

	thread_active = 1;

	//
	unsigned char *buffer = NULL;
	unsigned char *login_id = NULL;
	unsigned char *request = NULL;
	t_treenode *copied_treenode = NULL;
	t_workspace *ws = NULL;
	int data_ready = 0;

	// for get requests
	unsigned char *ret_buffer = NULL;
	unsigned int ret_length = 0;

	if ( !treenode )
	{
		process_message = "No data given";
		process_percent = 100;
		thread_active = 0;
		return;
	}

	process_message = "Logging in...";
	process_percent = 1;

	login_id = this->TriX_Site_Login ( this->user, this->pass );
	if ( !login_id )
	{
		process_message = "Login failed!";
		process_percent = 100;
		ui_dlg_msg ( "Unable to log in. Please try again later.", 0 );
		return;
	}

	process_message = "Preparing...";
	process_percent = 2;

	if ( thread_abort )
	{
		thread_active = 0;
		return;
	}

	copied_treenode = treenode_clone ( treenode );

	if ( thread_abort )
	{
		process_message = "No data given";
		process_percent = 100;
		thread_active = 0;
		return;
	}
	process_message = "Preparing...";
	process_percent = 3;

	buffer = xml_connector_create ( copied_treenode, NULL );
	if ( buffer )
	{
		if ( thread_abort )
		{
			process_message = "Aborted";
			process_percent = 100;
			thread_active = 0;
			return;
		}

		process_message = "Preparing...";
		process_percent = 4;
		ws = workspace_create_file_from_buffer ( buffer, strlen ( (const char*)buffer ) );

		process_message = "Packing...";
		process_percent = 5;
		file_format ( ws->fileinfo, "BZ2" );

		if ( encrypted )
		{
			process_message = "Encrypting...";
			process_percent = 6;
			file_format ( ws->fileinfo, "TRIXCRYPT" );
		}

		if ( thread_abort )
		{
			process_message = "Aborted";
			process_percent = 100;
			thread_active = 0;
			return;
		}

		process_message = "Uploading data...";
		process_percent = 7;
		if ( ws && ws->fileinfo && ws->fileinfo->stages && ws->fileinfo->stages->segments && ws->fileinfo->stages->segments->length > 0 )
		{

			http_parse_url ( "http://ppmodd.g3gg0.de/", NULL );
			buffer = (unsigned char *)malloc ( 1024 );
			if ( !buffer )
				return;
			sprintf ( (char*)buffer, "request?%s", login_id );

			http_set_progress ( process_percent, 50, &process_percent );
			http_put ( (char*)buffer, (char*)ws->fileinfo->stages->segments->data, ws->fileinfo->stages->segments->length, 0, NULL );
			http_set_progress ( 0, 100, NULL );

			if ( thread_abort )
			{
				process_message = "Aborted";
				process_percent = 100;
				thread_active = 0;
				return;
			}
			process_message = "Waiting for Server to process";
			process_percent = 50;

			//
			// retrieve again
			//
			data_ready = 0;
			while ( !thread_abort && !data_ready )
			{
				sprintf ( (char*)buffer, "ready?%s", login_id );
				http_get ( (char*)buffer, (char**)&ret_buffer, &ret_length, NULL );
				if ( !ret_buffer )
				{
					process_message = "Check failed!!!!";
					process_percent = 100;
					return;
				}
				if ( !strcmp ( "OK", (const char*)ret_buffer ))
					data_ready = 1;
				else if ( strlen ( (const char*)ret_buffer ) > 0 && strlen ( (const char*)ret_buffer ) < 256 )
				{
					strcpy ( (char*)server_message, (const char*)ret_buffer );
					process_message = (const char*)server_message;
					Sleep ( 1000 );
				}
				else
					Sleep ( 1000 );

			}

			process_message = "Downloading generated data";
			process_percent = 51;

			if ( thread_abort )
			{
				process_message = "Aborted";
				process_percent = 100;
				thread_active = 0;
				return;
			}

			sprintf ( (char*)buffer, "retrieve?%s", login_id );

			http_set_progress ( process_percent, 99, &process_percent );
			http_get ( (char*)buffer, (char**)&ret_buffer, &ret_length, NULL );
			http_set_progress ( 0, 100, NULL );

			if ( !ret_buffer )
			{
				process_message = "Receiving failed!!!!";
				process_percent = 100;
				return;
			}

			process_message = "Rebuilding Data";
			process_percent = 99;

			t_workspace *ws2 = workspace_create_file_from_buffer ( ret_buffer, ret_length );
			file_sync ( ws2->fileinfo->stages );
			workspace_update_memmap ( ws2 );

			result_treenode = xml_connector_parse ( v_get_str ( ws2, 0 ) );
			free ( ret_buffer );

			process_message = "Finished";
			process_percent = 100;

			CHECK_AND_FREE ( buffer );
		}

		workspace_release ( ws );
	}

	treenode_release ( copied_treenode );
	copied_treenode = NULL;
	//

	thread_active = 0;
	thread_finished = 1;
}



//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

/**
 ** The constructor initializes the environment for the first application.
 **
 */
BuildPPMProgress::BuildPPMProgress( t_treenode *node, unsigned char *username, unsigned char *password )
{

	/*
	** Build the ui component
	**
	*/
	setupUi(this);

	this->username = username;
	this->password = password;
	finished_node = NULL;
	this_thread = new PPModd_Build_Thread ( node, username, password );

	LOAD_PLUGIN_SYMBOL ( "XML", "xml_connector_create", this_thread->xml_connector_create );
	LOAD_PLUGIN_SYMBOL ( "XML", "xml_connector_parse", this_thread->xml_connector_parse );


	connect ( cancelButton, SIGNAL ( clicked() ),this, SLOT( cancel() ) );
	return;
}
//---------------------------------------------------------------------------

t_treenode *BuildPPMProgress::get_node ()
{
	return finished_node;
}

void BuildPPMProgress::cancel ()
{
	this_thread->abort ();

	reject();
	return;
}

void BuildPPMProgress::update ()
{
	progressLabel->setText ( QString::fromUtf8 ( this_thread->process_msg() ) );
	progressBar->setValue ( this_thread->process_pct() );

	if ( this_thread->is_finished() )
		accept();

	return;
}

int BuildPPMProgress::UploadPPM ( )
{
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(250);

	progressBar->setRange ( 0, 100 );
	progressLabel->setText ( QString::fromUtf8 ( "Initializing" ) );
	progressBar->setValue ( 0 );

	this_thread->start ();

	this->exec();

	finished_node = this_thread->get_node ();

	return E_OK;
}
