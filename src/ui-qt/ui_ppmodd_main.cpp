/// ui_main.cpp
 
#include "ui_build_ppm_progress.h"
#include "ui_ppmodd_main.h"
#include "ui_commandref.h"
#include "ui_object_tree.h"
#include "ui_plug.h"
#include "ui_opt.h"
#include "ui_workspace_dlg.h"
#include "ui_text_box.h"
#include "ui_text_edit.h"

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

#include "mem.h"

#define DEFAULT_SCRIPT \
	"#include trix\n" \
	"\n" \
	"int main ( )\n" \
	"{\n"\
	"   char *buffer = NULL;\n"\
	"	UiDlgString ( \"Just press enter if you're bored ;).\", &buffer );\n" \
	"}\n"


unsigned int seer_run ( char *script );
int qInitResources();

extern t_project *project;

extern t_workspace *trix_workspace;
int ppm_start_address = E_FAIL;

char *trix_script_filenames[2][10];
QApplication *trix_main_app = NULL;
PPModdMainWindow *trix_main_win = NULL;

t_treenode *ppm_source = NULL;
t_treenode *ppm_data = NULL;

t_treenode *current_treenode = NULL;
QTreeWidgetItem *current_item = NULL;
t_treenode *copied_treenode = NULL;
QTreeWidgetItem *copied_item = NULL;

unsigned char *server_username = NULL;
unsigned char *server_password = NULL;

unsigned char *last_filename = NULL;
//
//
//

#define QT_BOX_CLEAR   0
#define QT_BOX_APPEND  1
#define QT_BOX_RELEASE 2

typedef struct s_qt_box_queue t_qt_box_queue;
struct s_qt_box_queue
{
	STRUCT_HEADER_LIST(t_qt_box_queue);
	unsigned int type;
	unsigned char *message;
};

typedef struct s_qt_box_entry t_qt_box_entry;
struct s_qt_box_entry
{
	STRUCT_HEADER_LIST(t_qt_box_entry);
	unsigned char *title;
	unsigned int width;
	unsigned int height;
	unsigned int id;
	unsigned int release;
	t_qt_box_queue *queue;
	t_mutex *mutex;
	TextBox *box;
};

t_qt_box_entry *qt_box_list = NULL;
t_mutex *qt_box_mutex = NULL;


//
// Plugin Symbols
//
//////////////////////////////////////////////////////////////////////////////////////////

void *tempsymbol;
char errstring[1024];

#define LOAD_PLUGIN_SYMBOL(plugin,symbol,dest)\
	tempsymbol = trixplug_get_plugin_symbol ( plugin, symbol ); \
	if ( !tempsymbol )\
	{\
		sprintf ( errstring, "[ERROR] Missing symbol '%s' in Plugin '%s'\n", symbol, plugin );\
		printf ( errstring );\
		ui_dlg_msg ( errstring, 0 );\
		return E_FAIL;\
	}\
	else\
		memcpy ( &dest, &tempsymbol, sizeof (tempsymbol) );\




#ifdef TRIX_THREADED
char trix_out_buffer[TRIX_PRINTF_BUFSIZE+10];
QTimer *trix_thread_timer = NULL;
int trix_out_buffer_updated = 0;
int trix_output_pause = 0;
int trix_output_flush = 0;
t_mutex *trix_thread_lock = NULL;

//------------------------------------------

#define DLG_NONE 0
#define DLG_BOOL 1
#define DLG_STR  2

char *dialog_text = NULL;
char *dialog_message = NULL;
int dialog_bool = 0;
int dialog_type = DLG_NONE;
char *dialog_default = NULL;

t_mutex *trix_dialog_lock = NULL;
int dlg_locked = 0;

//------------------------------------------

#endif
};

#ifdef E_FAIL
#undef E_FAIL
#endif

#define E_FAIL (-1)
#define E_OK    0

#define MENU_EXPORT_XML  1
#define MENU_IMPORT_XML  2
#define MENU_CUT         3
#define MENU_COPY        4
#define MENU_PASTE       5
#define MENU_DELETE      6
#define MENU_HEXEDIT     7
#define MENU_EXPORT_BIN  8
#define MENU_IMPORT_BIN  9
#define MENU_UP          10
#define MENU_DOWN        11


typedef struct s_scriptlist t_scriptlist;
struct s_scriptlist
{
	STRUCT_HEADER_LIST(t_scriptlist);
	char *filename;
	char *title;
	char *settings;
	t_options *options;
	scScript script;
} ;

typedef struct s_script_entry t_script_entry;
struct s_script_entry
{
	STRUCT_HEADER_LIST(t_script_entry);
	char *title;
	char *content;
	t_options *options;
	char *settings;
};

t_scriptlist *scriptlist = NULL;

WorkspaceWindow *modeless_ws_win = NULL;
options *opts = NULL;
PluginWindow *modeless_plug_win = NULL;
CommandReference *modeless_ref_win = NULL;


unsigned int execute_scripts ( t_script_entry *script_entries )
{
	char *current_settings = NULL;
	t_script_entry *entry = NULL;

	entry = script_entries;
	// execute scripts
	while ( entry )
	{
		// save current global settings
		current_settings = options_read_all_settings();

		// register script's options
		if ( entry->options )
			options_import_script_options ( entry->title, entry->options );

		// clear all settings
//		options_default_all ();

		// restore the custom settings
		if ( entry->settings )
			options_apply_settings ( entry->settings );

		seer_run ( entry->content );

		// unregister script's options
		if ( entry->options )
			options_delete_script_options ( entry->title, entry->options );

		// restore the original settings
		options_apply_settings ( current_settings );

		free ( current_settings );

		CHECK_AND_FREE ( entry->title );
		CHECK_AND_FREE ( entry->settings );
		CHECK_AND_FREE ( entry->content );

		LIST_NEXT(entry);
	}
	return E_OK;
}



#ifdef TRIX_THREADED

class TriX_Script_Thread : public QThread 
{
public:
	TriX_Script_Thread ();
    virtual void prepare ( char *script, t_workspace *ws );
    virtual void prepare_multi ( t_script_entry *scripts, t_workspace *ws );
    virtual int is_active ();
    virtual int is_finished ();
    virtual void run ();
    virtual void reset ();
    virtual void abort ();
private:
	int thread_active;
	int thread_finished;
	t_script_entry *script_entries;
	t_workspace *workspace;
};

TriX_Script_Thread::TriX_Script_Thread ()
{
	thread_active = 0;
	thread_finished = 0;
	script_entries = NULL;
	workspace = NULL;
}

int TriX_Script_Thread::is_active ()
{
	return thread_active;
}
int TriX_Script_Thread::is_finished ()
{
	return thread_finished;
}


void TriX_Script_Thread::prepare ( char *script, t_workspace *ws )
{
	if ( is_active() )
	{
		ui_dlg_msg ( "Already running a Script!", 0 );
		return;
	}


	CHECK_AND_FREE ( script_entries );

	script_entries = LIST_MALLOC ( t_script_entry );
	if ( !script_entries )
		return;

	script_entries->title = "Script Editor";
	script_entries->content = script;
	script_entries->options = NULL;
	script_entries->settings = NULL;

	workspace = ws;
}

void TriX_Script_Thread::prepare_multi ( t_script_entry *scripts, t_workspace *ws )
{
	if ( is_active() )
	{
		ui_dlg_msg ( "Already running a Script!", 0 );
		return;
	}

	if ( script_entries )
		LIST_FREE ( script_entries );
	script_entries = scripts;
	workspace = ws;
}

void TriX_Script_Thread::abort()
{
	if ( is_active() && !is_finished() )
		seer_abort_script ();
}

void TriX_Script_Thread::reset()
{
	if ( is_active() && !is_finished() )
	{
		ui_dlg_msg ( "Still running a Script!", 0 );
		return;
	}

	thread_active = 0;
	thread_finished = 0;
	script_entries = NULL;
	workspace = NULL;
}
void TriX_Script_Thread::run()
{
	int pos = 0;
	int file_pos = 0;
	t_fileinfo *fi = NULL;

	if ( is_active() )
	{
		ui_dlg_msg ( "Already running a Script!", 0 );
		return;
	}

	thread_active = 1;

	
	trix_workspace = workspace;

	// update workspace window
	if ( modeless_ws_win )
		modeless_ws_win->Update ( trix_workspace );

	// prevent plugin unloading
	if ( modeless_plug_win )
		modeless_plug_win->unloading ( false );


	execute_scripts ( script_entries );

	// allow plugin unloading
	if ( modeless_plug_win )
		modeless_plug_win->unloading ( true );

	// save files
	fi = trix_workspace->fileinfo;
	while ( fi )
	{
		if ( fi )
		{
			if ( trix_script_filenames[1][file_pos] && (strlen ( trix_script_filenames[1][file_pos] ) > 0) )
			{
				printf ( "[ofile #%i: %s ]\n", file_pos, trix_script_filenames[1][file_pos]);
				file_write ( trix_script_filenames[1][file_pos], fi );
			}

			if ( trix_script_filenames[0][file_pos] )
				free ( trix_script_filenames[0][file_pos] );
			if ( trix_script_filenames[1][file_pos] )
				free ( trix_script_filenames[1][file_pos] );
			LIST_NEXT(fi);
			file_pos++;
		}
	}
	// save original workspace and "hide" the trix_workspace
	workspace = trix_workspace;
	trix_workspace = NULL;

	// update workspace window
	if ( modeless_ws_win )
		modeless_ws_win->Update ( trix_workspace );

	// free (hidden) workspace
	workspace_release ( workspace );

	LIST_FREE ( script_entries );

//	mem_check_all (  );
	thread_finished = 1;
}

static TriX_Script_Thread script_thread;

#endif

//---------------------------------------------------------------------------

static QTextEdit *_textOutput;


void PPModdMainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    event->acceptProposedAction();
}

void PPModdMainWindow::dropEvent(QDropEvent *event)
{
	QString name = event->mimeData()->urls().first().toString();
	event->acceptProposedAction();

	LoadFile ( (char*)(name.toAscii().constData()) );
}
/*
//---------------------------------------------------------------------------
// if we are closed, also remove the workspace window
void PPModdMainWindow::setVisible (bool visible)
{
	
	if ( !visible && modeless_ws_win )
		delete modeless_ws_win;
	if ( !visible && modeless_plug_win )
		delete modeless_plug_win;
	if ( !visible && modeless_ref_win )
		delete modeless_ref_win;
	if ( !visible && opts )
		delete opts;

	modeless_ws_win = NULL;
	modeless_plug_win = NULL;
	modeless_ref_win = NULL;
	opts = NULL;

	QWidget::setVisible ( visible );
	return;
}
*/
/**
 ** The constructor initializes the environment for the first application.
 **
 */
PPModdMainWindow::PPModdMainWindow()
{
	int ret = 0;
   /*
   ** Build the ui component
   **
   */
   setupUi(this);
   _textOutput = textOutput; // save as global variable to make it accessible by qt_vprintf()

   /*
   ** Connect custom slots to signals
   **
   */
#ifdef TRIX_THREADED
	trix_thread_lock = mutex_create();
	trix_dialog_lock = mutex_create();

	trix_thread_timer = new QTimer ();
	ret = connect ( trix_thread_timer, SIGNAL(timeout()), this, SLOT(evtTimerElapsed()));

	trix_thread_timer->start(100);
#endif

/*
	ret = connect(cmbSaveMode,SIGNAL(activated(int)),this,SLOT(cmbSaveModeActivated(int)));
	ret = connect(cmbFileType,SIGNAL(activated(int)),this,SLOT(cmbFileTypeActivated(int)));

	ret = connect(btnEditImport,SIGNAL(clicked()),this,SLOT(btnEditImportClicked()));
	ret = connect(btnEditExport,SIGNAL(clicked()),this,SLOT(btnEditExportClicked()));

	ret = connect(btnScriptsAdd,SIGNAL(clicked()),this,SLOT(btnScriptsAddClicked()));
	ret = connect(btnScriptsRemove,SIGNAL(clicked()),this,SLOT(btnScriptsRemoveClicked()));
	ret = connect(btnScriptsManual,SIGNAL(clicked()),this,SLOT(btnScriptsManualClicked()));
	ret = connect(btnScriptsOptions,SIGNAL(clicked()),this,SLOT(btnScriptsOptionsClicked()));
	ret = connect(btnScriptsLoad,SIGNAL(clicked()),this,SLOT(btnScriptsLoadClicked()));

	ret = connect(btnSimpleInput,SIGNAL(clicked()),this,SLOT(btnSimpleInputClicked()));
	ret = connect(btnSimpleOutput,SIGNAL(clicked()),this,SLOT(btnSimpleOutputClicked()));

	ret = connect(btnDCT3InputMCU,SIGNAL(clicked()),this,SLOT(btnDCT3InputMCUClicked()));
	ret = connect(btnDCT3InputPPM,SIGNAL(clicked()),this,SLOT(btnDCT3InputPPMClicked()));
	ret = connect(btnDCT3OutputMCU,SIGNAL(clicked()),this,SLOT(btnDCT3OutputMCUClicked()));
	ret = connect(btnDCT3OutputPPM,SIGNAL(clicked()),this,SLOT(btnDCT3OutputPPMClicked()));

	ret = connect(boxDCT3SeparatePPM,SIGNAL(toggled(bool)),this,SLOT(boxDCT3SeparatePPMToggled(bool)));
	ret = connect(boxDCT3CustomPPM,SIGNAL(toggled(bool)),this,SLOT(boxDCT3CustomPPMToggled(bool)));

	ret = connect(grpSimpleInput,SIGNAL(toggled(bool)),this,SLOT(grpSimpleInputToggled(bool)));
	ret = connect(grpSimpleOutput,SIGNAL(toggled(bool)),this,SLOT(grpSimpleOutputToggled(bool)));

	ret = connect(grpDCT3Input,SIGNAL(toggled(bool)),this,SLOT(grpDCT3InputToggled(bool)));
	ret = connect(grpDCT3Output,SIGNAL(toggled(bool)),this,SLOT(grpDCT3OutputToggled(bool)));

	ret = connect(grpDCT4Input,SIGNAL(toggled(bool)),this,SLOT(grpDCT4InputToggled(bool)));
	ret = connect(grpDCT4Output,SIGNAL(toggled(bool)),this,SLOT(grpDCT4OutputToggled(bool)));

	ret = connect(editSimpleInput,SIGNAL(textEdited ( const QString & )),this,SLOT(editSimpleInputEdited ( const QString & )));
	ret = connect(editSimpleOutput,SIGNAL(textEdited ( const QString & )),this,SLOT(editSimpleOutputEdited ( const QString & )));

	ret = connect(editDCT3InputMCU,SIGNAL(textEdited ( const QString & )),this,SLOT(editDCT3InputMCUEdited ( const QString & )));
	ret = connect(editDCT3OutputMCU,SIGNAL(textEdited ( const QString & )),this,SLOT(editDCT3OutputMCUEdited ( const QString & )));
	ret = connect(editDCT3InputPPM,SIGNAL(textEdited ( const QString & )),this,SLOT(editDCT3InputPPMEdited ( const QString & )));
	ret = connect(editDCT3OutputPPM,SIGNAL(textEdited ( const QString & )),this,SLOT(editDCT3OutputPPMEdited ( const QString & )));
	ret = connect(editDCT3CustomPPM,SIGNAL(textEdited ( const QString & )),this,SLOT(editDCT3CustomPPMEdited ( const QString & )));

	ret = connect(editDCT4InputMCU,SIGNAL(textEdited ( const QString & )),this,SLOT(editDCT4InputMCUEdited ( const QString & )));
	ret = connect(editDCT4OutputMCU,SIGNAL(textEdited ( const QString & )),this,SLOT(editDCT4OutputMCUEdited ( const QString & )));
	ret = connect(editDCT4InputPPM,SIGNAL(textEdited ( const QString & )),this,SLOT(editDCT4InputPPMEdited ( const QString & )));
	ret = connect(editDCT4OutputPPM,SIGNAL(textEdited ( const QString & )),this,SLOT(editDCT4OutputPPMEdited ( const QString & )));



	ret = connect(btnDCT4InputMCU,SIGNAL(clicked()),this,SLOT(btnDCT4InputMCUClicked()));
	ret = connect(btnDCT4InputPPM,SIGNAL(clicked()),this,SLOT(btnDCT4InputPPMClicked()));
	ret = connect(btnDCT4OutputMCU,SIGNAL(clicked()),this,SLOT(btnDCT4OutputMCUClicked()));
	ret = connect(btnDCT4OutputPPM,SIGNAL(clicked()),this,SLOT(btnDCT4OutputPPMClicked()));

	// connect actions
	ret = connect(actPlugins,SIGNAL(triggered()),this,SLOT(actPluginsTriggered()));
	ret = connect(actCommandRef,SIGNAL(triggered()),this,SLOT(actCommandRefTriggered()));
	ret = connect(actNew,SIGNAL(triggered()),this,SLOT(actNewTriggered()));
	ret = connect(actQuit,SIGNAL(triggered()),this,SLOT(actQuitTriggered()));
	ret = connect(actStart,SIGNAL(triggered()),this,SLOT(actStartTriggered()));
	ret = connect(actStop,SIGNAL(triggered()),this,SLOT(actStopTriggered()));
	ret = connect(actOptions,SIGNAL(triggered()),this,SLOT(actOptionsTriggered()));

	ret = connect(textInput,SIGNAL(returnPressed()), this, SLOT(txtEnterPressed ()));

	ret = connect(actShowWorkspace,SIGNAL(triggered()),this,SLOT(actShowWorkspaceTriggered()));

	*/

	ret = connect(btnEdit,SIGNAL(clicked()),this,SLOT(btnEditClicked()));

	ret = connect(actionQuit,SIGNAL(triggered()),this,SLOT(actQuitTriggered()));
	ret = connect(actionOpen,SIGNAL(triggered()),this,SLOT(actOpenTriggered()));
	ret = connect(actionWorkspace,SIGNAL(triggered()),this,SLOT(actWorkspaceTriggered()));
	ret = connect(actionOptions,SIGNAL(triggered()),this,SLOT(actOptionsTriggered()));
	ret = connect(actionPlugins,SIGNAL(triggered()),this,SLOT(actPluginsTriggered()));

	ret = connect(actionSave,SIGNAL(triggered()),this,SLOT(actSaveTriggered()));
	ret = connect(actionSave_as,SIGNAL(triggered()),this,SLOT(actSaveAsTriggered()));
	ret = connect(actionClose,SIGNAL(triggered()),this,SLOT(actCloseTriggered()));

	ret = connect(actionExport_XML,SIGNAL(triggered()),this,SLOT(actExportTriggered()));
	ret = connect(actionImport_XML,SIGNAL(triggered()),this,SLOT(actImportTriggered()));

	ret = connect(actionCreate_PPM,SIGNAL(triggered()),this,SLOT(actBuildPPMTriggered()));

	ret = connect ( treeWidget, SIGNAL( itemClicked(QTreeWidgetItem *, int) ), this, SLOT ( treeItemClicked (QTreeWidgetItem *, int) ) );
	ret = connect ( treeWidget, SIGNAL( itemSelectionChanged() ), SLOT( treeItemChanged() ) );
	ret = connect ( treeWidget, SIGNAL( customContextMenuRequested ( const QPoint & ) ), SLOT( treeItemContext(const QPoint &) ) );

	ret = connect ( treeWidget, SIGNAL( currentItemChanged ( QTreeWidgetItem *, QTreeWidgetItem * ) ), SLOT( treeCurrentItemChanged( QTreeWidgetItem *, QTreeWidgetItem *) ) );

/*
	// create and add the "View" menu
	QMenu *menuView = createPopupMenu();
	menuView->setTitle(tr("View"));
	menubar->insertMenu(menuFile->menuAction(),menuView);

	// add the "Output" action to the toolbar
	QAction *actOutput=menuView->actions().first();
	actOutput->setIcon(QIcon(QString::fromUtf8(":/icons/icons/22x22-gartoon/chat.png")));
*/

	//	toolbar->insertAction(actStart,actOutput);
//	toolbar->insertSeparator(actStart);
	/*

	// simulate "New" action
	actNewTriggered();
*/
	/*
	editScriptBox->setPlainText( QString ( DEFAULT_SCRIPT ) );
	cmbFileType->setCurrentIndex(1);
	cmbFileTypeActivated(1);

	btnScriptsManual->setDisabled ( false );
	btnScriptsManual->setText ( QString ( "Reload" ) );
*/
}
//---------------------------------------------------------------------------


void PPModdMainWindow::evtTimerElapsed()
{
	int pos = 0;
	char buf[TRIX_PRINTF_BUFSIZE];
#ifdef TRIX_THREADED
/*
	if ( modeless_plug_win )
		modeless_plug_win->update_timer();
	if ( opts )
		opts->update_timer();
*/
	/*
	if ( dialog_type == DLG_STR )
	{
		dialog_type = DLG_NONE;

		textInputSymbol->setPixmap ( QPixmap(QString::fromUtf8(":/icons/icons/22x22-gartoon/chat.png")) );
		textInputMessage->setText ( dialog_message );
		if ( dialog_default )
			textInput->setText ( dialog_default );
		textInput->selectAll(); 
		textInput->setFocus ( Qt::OtherFocusReason );
	}
	if ( dialog_type == DLG_BOOL )
	{
		dialog_type = DLG_NONE;

		QMessageBox mb ( "TriX", QString( dialog_message ), QMessageBox::Question, QMessageBox::Yes, QMessageBox::No, 0, 0 );
		mb.setIcon ( QMessageBox::Warning );
		mb.setWindowIcon ( QIcon(QString::fromUtf8(":/icons/icons/22x22-gartoon/calculator.png")) );
		mb.focusWidget();

		switch ( mb.exec() )
		{
			case QMessageBox::Yes:
				dialog_bool = E_OK;
				break;
			case QMessageBox::No:
				dialog_bool = E_FAIL;
				break;
			default:
				dialog_bool = E_FAIL;
		}
		mutex_unlock (trix_dialog_lock);
	}
*/
	if ( trix_out_buffer && trix_out_buffer_updated > 0  )
	{
		mutex_lock ( trix_thread_lock );

		pos = strlen ( trix_out_buffer );
		if ( pos >= TRIX_PRINTF_BUFSIZE - 1 )
			pos = TRIX_PRINTF_BUFSIZE - 2;
		while ( pos && trix_out_buffer[pos] != '\n' )
			pos--;

		if ( trix_out_buffer[pos] == '\n' || trix_output_pause || trix_output_flush )
		{
			trix_output_flush = 0;
			memcpy ( buf, trix_out_buffer, strlen ( trix_out_buffer ) );
			buf[pos] = '\000';
			if ( !trix_output_pause )
				_textOutput->append ( buf );
			memmove ( trix_out_buffer, trix_out_buffer+pos+1, strlen ( trix_out_buffer+pos+1 ) + 1 );
		}

		trix_out_buffer_updated = 0;
		mutex_unlock(trix_thread_lock);
	}
	/*
	if ( script_thread.is_active () && script_thread.is_finished () )
	{
		trix_output_pause = 0;
		script_thread.reset ();
		actStart->setEnabled ( true );
		actStop->setEnabled ( false );
	}*/
#endif
	
}

void PPModdMainWindow::txtEnterPressed ( )
{
	/*
	if ( mutex_locked(trix_dialog_lock ) )
	{
		dialog_text = (char*)strdup(textInput->text().toAscii().data());
		mutex_unlock ( trix_dialog_lock );

		textInput->selectAll(); 
		textInputMessage->setText ( "" );
		textInputSymbol->setPixmap ( QPixmap() );
	}
*/
	return;
}


void PPModdMainWindow::btnEditClicked()
{
	TextEdit dlg( current_treenode );

	dlg.exec ();

	SetData ( ppm_data );
//	UpdatePPMData ( ppm_data );

}

//---------------------------------------------------------------------------
unsigned int PPModdMainWindow::UpdatePPMData ( t_treenode *treenode )
{
	ppm_source = treenode;

//	if ( ppm_data )
//		treenode_release ( ppm_data );
	ppm_data = NULL;

	// reformat PPM data

	ppm_data = ppm_source;

	return E_OK;
}

unsigned int PPModdMainWindow::SetDataSubcall ( t_treenode *treenode, QTreeWidgetItem *tree )
{
	QTreeWidgetItem *item;
	t_treenode *infonode = NULL;

	while ( treenode )
	{
		if ( treenode_get_name ( treenode ) && !strcmp ( (const char*)treenode_get_name ( treenode ), "PPM" ) )
		{
			// that will be the root node

			if ( tree )
				item = new QTreeWidgetItem ( tree );
			else
				item = new QTreeWidgetItem ( treeWidget );
				
			QVariant user_data;
			qVariantSetValue ( user_data, (void*)treenode );
			item->setData ( 1, Qt::UserRole, user_data );

			if ( treenode_get_name ( treenode ) )
				item->setText ( 0, QString((char*)treenode_get_name ( treenode )) );
			else
				item->setText ( 0, tr("(unnamed)") );

			if ( treenode_children ( treenode ) )
				SetDataSubcall ( treenode_children ( treenode ), item );
		}

		if ( treenode_get_name ( treenode ) && !strcmp (  (const char*)treenode_get_name ( treenode ), "CHUNK" ) )
		{
			infonode = treenode_get_byname ( treenode_children ( treenode ), (unsigned char*)"INFORMATION" );
			infonode = treenode_get_byname ( treenode_children ( infonode ), (unsigned char*)"NAME" );

			if ( tree )
				item = new QTreeWidgetItem ( tree );
			else
				item = new QTreeWidgetItem ( treeWidget );
				
			QVariant user_data;
			qVariantSetValue ( user_data, (void*)treenode );
			item->setData ( 1, Qt::UserRole, user_data );

			if ( strlen ( (const char*)treenode_get_content_ascii ( infonode ) ) )
				item->setText ( 0, QString((char*)treenode_get_content_ascii ( infonode )) );
			else
				item->setText ( 0, tr("(unnamed)") );

			if ( treenode_children ( treenode ) )
				SetDataSubcall ( treenode_children ( treenode ), item );
		}

		if ( treenode_get_name ( treenode ) && !strcmp (  (const char*)treenode_get_name ( treenode ), "SUBCHUNK" ) )
		{
			infonode = treenode_get_byname ( treenode_children ( treenode ), (unsigned char*)"INFORMATION" );
			infonode = treenode_get_byname ( treenode_children ( infonode ), (unsigned char*)"NAME" );

			if ( tree )
				item = new QTreeWidgetItem ( tree );
			else
				item = new QTreeWidgetItem ( treeWidget );
				
			QVariant user_data;
			qVariantSetValue ( user_data, (void*)treenode );
			item->setData ( 1, Qt::UserRole, user_data );

			if ( strlen ( (const char*)treenode_get_content_ascii ( infonode ) ) )
				item->setText ( 0, QString((char*)treenode_get_content_ascii ( infonode )) );
			else
				item->setText ( 0, tr("(unnamed)") );

			if ( treenode_children ( treenode ) )
				SetDataSubcall ( treenode_children ( treenode ), item );
		}

		if ( treenode_get_name ( treenode ) && !strcmp (  (const char*)treenode_get_name ( treenode ), "STRINGS" ) )
		{
			if ( treenode_children ( treenode ) )
				SetDataSubcall ( treenode_children ( treenode ), tree );
		}

		if ( treenode_get_name ( treenode ) && !strcmp (  (const char*)treenode_get_name ( treenode ), "STRING" ) )
		{
			infonode = treenode_get_byname ( treenode_children ( treenode ), (unsigned char*)"UTF8" );

			if ( tree )
				item = new QTreeWidgetItem ( tree );
			else
				item = new QTreeWidgetItem ( treeWidget );
				
			QVariant user_data;
			qVariantSetValue ( user_data, (void*)treenode );
			item->setData ( 1, Qt::UserRole, user_data );

			item->setText ( 0, QString::fromUtf8((char*)treenode_get_content_ascii ( infonode )) );

			if ( treenode_children ( treenode ) )
				SetDataSubcall ( treenode_children ( treenode ), item );
		}

		treenode = treenode_next ( treenode );
	}

	return E_OK;
}

unsigned int PPModdMainWindow::SetData ( t_treenode *data )
{
	QTreeWidgetItem *item;

	UpdatePPMData ( data );

	QStringList sl; 
	sl << tr( "Tree" );
    treeWidget->setHeaderLabels( sl );
    treeWidget->setColumnCount( 1 );

	treeWidget->clear ();
	SetDataSubcall ( ppm_data, NULL );

	return E_OK;
}

//---------------------------------------------------------------------------
void PPModdMainWindow::treeItemChanged (  )
{
	return;
}

//---------------------------------------------------------------------------
void PPModdMainWindow::treeCurrentItemChanged ( QTreeWidgetItem * current, QTreeWidgetItem * previous )
{
	if ( !current ) 
		current_treenode = NULL;
	else
		current_treenode = (t_treenode *)qVariantValue <void *>( current->data ( 1, Qt::UserRole) );

	current_item = current;
	return;
}


//---------------------------------------------------------------------------

t_treenode *PPModdMainWindow::LoadFromXML ()
{
	t_treenode *node = NULL;
	QString name=QFileDialog::getOpenFileName(
		this,
		QString("Load Tree from"),
		"", 
		"XML File (*.xml);;Compressed XML File (*.xml.bz2);;All Files (*.*)", 0, 0 );

   if(!name.isNull()) 
   {
		t_workspace *ws = NULL;

		ws = workspace_startup ( (char*)(name.toAscii().constData()) );
		if ( !ws )
		{
			ui_dlg_msg ( "Script loading failed", 0 );
			return NULL;
		}
		node = xml_connector_parse ( v_get_str ( ws, 0 ) );
		workspace_release ( ws );
   }
   return node;
}

//---------------------------------------------------------------------------
unsigned int PPModdMainWindow::SaveAsXML ( t_treenode *node )
{
	int state = E_OK;
	char *filename = NULL;
	QFileDialog dlg;
    QString name = dlg.getSaveFileName( this, 
		QString("Save this Tree as"),
		"",
		"XML File (*.xml);;Compressed XML File (*.xml.bz2);;All Files (*.*)", 0, 0);

   if ( !name.isNull() )
   {
		t_workspace *ws = NULL;
		unsigned char *buffer = NULL; 

		buffer = xml_connector_create ( node, NULL );
		if ( !buffer )
			return E_FAIL;

		filename = (char*)strdup(name.toAscii().data());

		ws = workspace_create_file_from_buffer ( (unsigned char*)buffer, strlen ( (char*)buffer ) );

		if ( ws && (strlen (filename) > 4) && !strncasecmp ( filename+strlen(filename)-4, ".bz2", 4 ) )
			state = file_format ( ws->fileinfo, "BZ2" );

		if ( ws && (state == E_OK) )
		{
			/* override the DRYMODE options */
			int old_options = file_set_options ( FILE_NORMAL );

			state = file_write ( filename, ws->fileinfo );
			file_set_options ( old_options );
		}
		free ( filename );

		if ( !ws || (state != E_OK) )
		{
			ui_dlg_msg ( "Failed saving the script", 0 );
			return E_FAIL;
		}

		workspace_release ( ws );

   }

	return E_OK;
}
//---------------------------------------------------------------------------

unsigned int PPModdMainWindow::LoadFromBIN ( t_treenode *node )
{
	int length = 0;
	t_treenode *datanode = NULL;
	unsigned char *buffer = NULL; 

	datanode = treenode_get_byname ( treenode_children ( node ), (unsigned char*)"DUMP" );
	buffer = treenode_get_content_data_binary ( datanode, &length );

	if ( !buffer )
		return E_FAIL;

	free ( buffer );
	//
	// supports DUMPs, proceed
	//

	QString name=QFileDialog::getOpenFileName(
		this,
		QString("Load subchunk data from"),
		"", 
		"BIN File (*.bin);;Compressed BIN File (*.bin.bz2);;All Files (*.*)", 0, 0);

   if(!name.isNull()) 
   {
		t_workspace *ws = NULL;

		ws = workspace_startup ( (char*)(name.toAscii().constData()) );
		if ( !ws )
		{
			ui_dlg_msg ( "Script loading failed", 0 );
			return E_FAIL;
		}
		if ( treenode_set_content_data_binary ( datanode, v_get_ptr (ws,0), v_get_size ( ws ) ) != E_OK )
		{
			workspace_release ( ws );
			ui_dlg_msg ( "Error reading that data", 0 );
			return E_FAIL;
		}
//		datanode = treenode_get_byname ( treenode_children ( node ), (unsigned char*)"INFORMATION" );
//		datanode = treenode_get_byname ( treenode_children ( datanode ), (unsigned char*)"LENGTH" );
//		treenode_set_content_data_hexval ( datanode, v_get_size ( ws ) );

		workspace_release ( ws );
   }
   return E_OK;
}

//---------------------------------------------------------------------------
unsigned int PPModdMainWindow::SaveAsBIN ( t_treenode *node )
{
	int pos = 0;
	int state = E_OK;
	int length = 0;
	int loop = 0;
	char *filename_template = NULL;
	char *filename = NULL;
	t_treenode *datanode = NULL;
	t_treenode *current_node = NULL;
	t_treenode *multinode = NULL;
	unsigned char *buffer = NULL; 
	t_workspace *ws = NULL;
	QFileDialog dlg;

	node = treenode_clone ( node );

	multinode = treenode_children ( node );
	multinode = treenode_next ( multinode );
	multinode = treenode_children ( multinode );
	multinode = treenode_next ( multinode );
	multinode = treenode_children ( multinode );
	datanode = treenode_get_byname ( treenode_children ( multinode ), (unsigned char*)"DUMP" );

	// check if subchunks have DUMP entries, then use multinode mode
	if ( datanode )
	{
		//
		// supports DUMPs, proceed
		//
		QString name = dlg.getSaveFileName( this, 
			QString("Save these subchunks as"),
			"chunk.bin",
			"BIN File (*.bin);;Compressed BIN File (*.bin.bz2);;All Files (*.*)", 0, 0);

		if ( !name.isNull() )
		{
			filename_template = (char*)strdup(name.toAscii().data());
			filename = (char*)malloc ( 8192 );

			//
			// translate test.bin to test_%04i.bin
			//
			pos = strlen ( filename_template );
			do
			{ 
				pos--;
			} while ( pos >= 0 && filename_template[pos] != '.' && filename_template[pos] != '\\' && filename_template[pos] != '/' );

			filename_template = (char*) realloc ( filename_template, strlen ( filename_template ) + 100 );

			if ( pos < 0 || filename_template[pos] == '\\' || filename_template[pos] == '/' )
			{
				strcpy ( filename, filename_template );
				strcat ( filename, "_%04i" );
			}
			else
			{
				memcpy ( filename, filename_template, pos );
				filename[pos] = '\000';
				strcat ( filename, "_%04i" );
				strcat ( filename, filename_template + pos );
			}
			strcpy ( filename_template, filename );

			current_node = multinode;
			while ( current_node )
			{
				datanode = treenode_get_byname ( treenode_children ( current_node ), (unsigned char*)"DUMP" );

				sprintf ( filename, filename_template, loop++ );
				buffer = treenode_get_content_data_binary ( datanode, &length );
				ws = workspace_create_file_from_buffer ( (unsigned char*)buffer, length );

				if ( ws && (strlen (filename) > 4) && !strncasecmp ( filename+strlen(filename)-4, ".bz2", 4 ) )
					state = file_format ( ws->fileinfo, "BZ2" );

				if ( ws && (state == E_OK) )
				{
					/* override the DRYMODE options */
					int old_options = file_set_options ( FILE_NORMAL );

					state = file_write ( filename, ws->fileinfo );
					file_set_options ( old_options );
				}

				if ( !ws || (state != E_OK) )
				{
					free ( buffer );
					free ( filename );
					treenode_release ( node );
					ui_dlg_msg ( "Failed saving the dump", 0 );
					return E_FAIL;
				}

				//
				// get plain filename from c:\temp\test_0000.bin
				//
				pos = strlen ( filename );
				do
				{ 
					pos--;
				} while ( pos >= 0 && filename[pos] != '\\' && filename[pos] != '/' );

				datanode = treenode_addchild ( current_node );
				treenode_set_name ( datanode, (unsigned char*)"FILE" );
				treenode_set_content_data ( datanode, (unsigned char*)&filename[pos+1] );


				workspace_release ( ws );

				current_node = treenode_next ( current_node );
			}

			// saved all chunk entries, now save XML info file
			buffer = xml_connector_create ( node, NULL );
			if ( !buffer )
			{
				treenode_release ( node );
				return E_FAIL;
			}

			strcpy ( filename, name.toAscii().data() );
			strcat ( filename, ".xml" );

			ws = workspace_create_file_from_buffer ( (unsigned char*)buffer, strlen ( (char*)buffer ) );

			if ( ws && (strlen (filename) > 4) && !strncasecmp ( filename+strlen(filename)-4, ".bz2", 4 ) )
				state = file_format ( ws->fileinfo, "BZ2" );

			if ( ws && (state == E_OK) )
			{
				/* override the DRYMODE options */
				int old_options = file_set_options ( FILE_NORMAL );

				state = file_write ( filename, ws->fileinfo );
				file_set_options ( old_options );
			}

			if ( !ws || (state != E_OK) )
			{
				ui_dlg_msg ( "Failed saving the script", 0 );
				treenode_release ( node );
				return E_FAIL;
			}

			free ( filename );
			free ( filename_template );
			workspace_release ( ws );
		}

		treenode_release ( node );
		return E_OK;
	}

	datanode = treenode_get_byname ( treenode_children ( node ), (unsigned char*)"DUMP" );
	if ( !datanode )
	{
		treenode_release ( node );
		return E_FAIL;
	}
	//
	// supports DUMPs, proceed
	//
	QString name = dlg.getSaveFileName( this, 
		QString("Save this subchunk data as"),
		"chunk.bin",
		"BIN File (*.bin);;Compressed BIN File (*.bin.bz2);;All Files (*.*)", 0, 0);

	if ( !name.isNull() )
	{
		filename = (char*)strdup(name.toAscii().data());

		buffer = treenode_get_content_data_binary ( datanode, &length );
		ws = workspace_create_file_from_buffer ( (unsigned char*)buffer, length );

		if ( ws && (strlen (filename) > 4) && !strncasecmp ( filename+strlen(filename)-4, ".bz2", 4 ) )
			state = file_format ( ws->fileinfo, "BZ2" );

		if ( ws && (state == E_OK) )
		{
			/* override the DRYMODE options */
			int old_options = file_set_options ( FILE_NORMAL );

			state = file_write ( filename, ws->fileinfo );
			file_set_options ( old_options );
		}
		free ( filename );

		if ( !ws || (state != E_OK) )
		{
			ui_dlg_msg ( "Failed saving the dump", 0 );
			treenode_release ( node );
			return E_FAIL;
		}
		workspace_release ( ws );
	}
	treenode_release ( node );
	return E_OK;
}


//---------------------------------------------------------------------------
void PPModdMainWindow::treeItemContext ( const QPoint &pos )
{
	int action_id = 0;
	unsigned char *buffer = NULL;
	QMenu menu(this);
	QMap<QAction*, int> menuMap;
	t_treenode *tempnode = NULL;
	QClipboard *clipboard = QApplication::clipboard();
	QString tempstr;

	if ( !current_treenode )
		return;

	menuMap[menu.addAction("Export to XML...")] = MENU_EXPORT_XML;
	menuMap[menu.addAction("Import from XML...")] = MENU_IMPORT_XML;
	if ( strcmp ( (const char*)treenode_get_name ( current_treenode ), "PPM" ))
	{
		menu.addSeparator ();
		menuMap[menu.addAction("Export to Binary...")] = MENU_EXPORT_BIN;
		if ( strcmp ( (const char*)treenode_get_name ( current_treenode ), "CHUNK" ) ) 
			menuMap[menu.addAction("Import from Binary...")] = MENU_IMPORT_BIN;
	}
	menu.addSeparator ();

	if ( strcmp ( (const char*)treenode_get_name ( current_treenode ), "PPM" ) )
		menuMap[menu.addAction("Cut")] = MENU_CUT;
	menuMap[menu.addAction("Copy")] = MENU_COPY;
	menuMap[menu.addAction("Paste")] = MENU_PASTE;
	if ( strcmp ( (const char*)treenode_get_name ( current_treenode ), "PPM" ) )
	{
		menu.addSeparator ();
		menuMap[menu.addAction("Delete")] = MENU_DELETE;
	}

	menu.addSeparator ();
	menuMap[menu.addAction("Up")] = MENU_UP;
	menuMap[menu.addAction("Down")] = MENU_DOWN;

	if ( !strcmp ( (const char*)treenode_get_name ( current_treenode ), "SUBCHUNK" ) )
	{
		menu.addSeparator ();
		menuMap[menu.addAction("Hex-Edit")] = MENU_HEXEDIT;
	}

	QAction* action = menu.exec ( treeWidget->mapToGlobal ( pos ) );

	if ( !action )
		return;

	action_id = menuMap[action];

	QString str ( "plain" );
	
	switch ( action_id )
	{
		case MENU_EXPORT_XML:
			copied_treenode = treenode_clone ( current_treenode );

			SaveAsXML ( copied_treenode );

			treenode_release ( copied_treenode );
			break;

		case MENU_IMPORT_XML:
			copied_treenode = LoadFromXML();
			if ( copied_treenode )
				copied_treenode = copied_treenode->children;
			if ( !copied_treenode )
				break;

			if ( treenode_get_name ( current_treenode ) && treenode_get_name ( copied_treenode ) &&
				(
					(
						!strcmp (  (const char*)treenode_get_name ( current_treenode ), "PPM" ) &&
						!strcmp (  (const char*)treenode_get_name ( copied_treenode ), "CHUNK" )
					) 
				||
					(
						!strcmp (  (const char*)treenode_get_name ( current_treenode ), "CHUNK" ) &&
						!strcmp (  (const char*)treenode_get_name ( copied_treenode ), "SUBCHUNK" )
					) 
				)
			   )
			{

				if ( copied_treenode )
					treenode_append_child ( current_treenode, copied_treenode );

				// rebuild that pasted item tree
				SetDataSubcall ( copied_treenode, current_item );

				// prepare again for insertion
				copied_treenode = treenode_clone ( copied_treenode );
			}
			else
				ui_dlg_msg ( "You cannot insert that item there", 0 );
			break;

		case MENU_EXPORT_BIN:
			copied_treenode = treenode_clone ( current_treenode );

			if ( SaveAsBIN ( copied_treenode ) )
				ui_dlg_msg ( "Sorry, this chunk does not support binary dumping.", 0 );

			treenode_release ( copied_treenode );
			break;

		case MENU_IMPORT_BIN:
			if ( LoadFromBIN ( current_treenode ) )
				ui_dlg_msg ( "Sorry, this chunk does not support binary dumping.", 0 );
			break;

		case MENU_CUT:
			if ( !current_treenode || !current_item )
				break;

			if ( !strcmp (  (const char*)treenode_get_name ( current_treenode ), "PPM" ) )
			{
				ui_dlg_msg ( "You cannot delete that item", 0 );
				break;
			}

			copied_treenode = treenode_clone ( current_treenode );
			treenode_release ( current_treenode );
			current_treenode = NULL;

			delete current_item;
			current_item = NULL;
			break;

		case MENU_COPY:
			if ( !current_treenode || !current_item )
				break;

			copied_treenode = treenode_clone ( current_treenode );

			buffer = xml_connector_create ( copied_treenode, NULL );
			if ( buffer )
				clipboard->setText ( QString ( (const char*)buffer ) );

			treenode_release ( copied_treenode );
			copied_treenode = NULL;
			free ( buffer );
			break;

		case MENU_PASTE:
			if ( !current_treenode || !current_item )
				break;

			tempstr = clipboard->text ( str, QClipboard::Clipboard );

			if ( tempstr.toAscii().data() )
				copied_treenode = xml_connector_parse ( (unsigned char*)tempstr.toAscii().data() );
			else
				copied_treenode = NULL;

			if ( copied_treenode )
				copied_treenode = copied_treenode->children;

			if ( !copied_treenode )
				break;

			if ( treenode_get_name ( current_treenode ) && treenode_get_name ( copied_treenode ) &&
				(
					(
						!strcmp (  (const char*)treenode_get_name ( current_treenode ), "PPM" ) &&
						!strcmp (  (const char*)treenode_get_name ( copied_treenode ), "CHUNK" )
					) 
				||
					(
						!strcmp (  (const char*)treenode_get_name ( current_treenode ), "CHUNK" ) &&
						!strcmp (  (const char*)treenode_get_name ( copied_treenode ), "SUBCHUNK" )
					) 
				)
				)
			{

				if ( copied_treenode )
					treenode_append_child ( current_treenode, copied_treenode );

				// rebuild that pasted item tree
				SetDataSubcall ( copied_treenode, current_item );

				// prepare again for insertion
				copied_treenode = treenode_clone ( copied_treenode );
			}
			else
				ui_dlg_msg ( "You cannot copy that item there", 0 );

			break;

		case MENU_DELETE:
			if ( !strcmp ( (const char*)treenode_get_name ( current_treenode ), "PPM" ) )
			{
				ui_dlg_msg ( "You cannot delete that item", 0 );
				break;
			}
			treenode_release ( current_treenode );
			current_treenode = NULL;
			delete current_item;
			current_item = NULL;
			break;

		case MENU_HEXEDIT:
			break;

		case MENU_UP:

			if ( treenode_position ( current_treenode ) > 0 )
			{
				int pos = treenode_position ( current_treenode );
				tempnode = treenode_parent(current_treenode);
				treenode_unlink ( current_treenode );
				treenode_insert_at ( tempnode, current_treenode, pos-1 );
			}


			if ( current_item->parent()->indexOfChild(current_item) > 0 )
			{
				int pos = current_item->parent()->indexOfChild(current_item);
				current_item->parent()->insertChild ( pos-1, current_item->parent()->takeChild ( pos ) ); 
			}
			treeWidget->setItemSelected ( current_item, true );
			treeWidget->setCurrentItem ( current_item );
			treeWidget->update ();
			break;

		case MENU_DOWN:
			if ( treenode_position ( current_treenode ) < treenode_count ( treenode_children ( treenode_parent ( current_treenode ) ) ) - 1 )
			{
				int pos = treenode_position ( current_treenode );
				tempnode = treenode_parent(current_treenode);
				treenode_unlink ( current_treenode );
				treenode_insert_at ( tempnode, current_treenode, pos+1 );
			}

			if ( current_item->parent()->indexOfChild(current_item) < current_item->parent()->childCount() - 1 )
			{
				int pos = current_item->parent()->indexOfChild(current_item);
				current_item->parent()->insertChild ( pos+1, current_item->parent()->takeChild ( pos ) ); 
			}
			treeWidget->setItemSelected ( current_item, true );
			treeWidget->setCurrentItem ( current_item );
			treeWidget->update ();
			break;
	}

//	SetData ( ppm_data );
	return;
}


//---------------------------------------------------------------------------
int PPModdMainWindow::treeItemClicked ( QTreeWidgetItem * item, int column )
{
	t_treenode *infonode = NULL;
	t_treenode *datanode = NULL;
	t_treenode *treenode = (t_treenode *)qVariantValue <void *>( item->data ( 1, Qt::UserRole) );


	textEdit->clear ();
	textEdit->append ( tr("Details of selected Item:") );
	textEdit->append ( tr("") );

	if ( treenode )
	{
		if ( !strcmp ( (const char*)treenode_get_name ( treenode ), "PPM" ) )
		{
			infonode = treenode_get_byname ( treenode_children ( treenode ), (unsigned char*)"INFORMATION" );

			btnEdit->setEnabled ( false );

			textEdit->append ( tr("PPM Version:") );
			datanode = treenode_get_byname ( treenode_children ( infonode ), (unsigned char*)"VERSION" );
			textEdit->append ( QString ( (char*)treenode_get_content_ascii ( datanode ) ) );
			textEdit->append ( tr("") );

			textEdit->append ( tr("PPM Lang:") );
			datanode = treenode_get_byname ( treenode_children ( infonode ), (unsigned char*)"LANG" );
			textEdit->append ( QString ( (char*)treenode_get_content_ascii ( datanode ) ) );
			textEdit->append ( tr("") );

		}
		if ( !strcmp ( (const char*)treenode_get_name ( treenode ), "CHUNK" ) )
		{
			infonode = treenode_get_byname ( treenode_children ( treenode ), (unsigned char*)"INFORMATION" );

			datanode = treenode_get_byname ( treenode_children ( infonode ), (unsigned char*)"NAME" );
			if ( datanode && !strncmp ( (char*)treenode_get_content_ascii ( datanode ), "TEXT", 4 ) )
				btnEdit->setEnabled ( true );
			else
				btnEdit->setEnabled ( false );

			textEdit->append ( tr("CHUNK Version:") );
			datanode = treenode_get_byname ( treenode_children ( infonode ), (unsigned char*)"VERSION" );
			textEdit->append ( QString ( (char*)treenode_get_content_ascii ( datanode ) ) );
			textEdit->append ( tr("") );

			textEdit->append ( tr("CHUNK Length:") );
			datanode = treenode_get_byname ( treenode_children ( infonode ), (unsigned char*)"LENGTH" );
			textEdit->append ( QString ( (char*)treenode_get_content_ascii ( datanode ) ) );
			textEdit->append ( tr("") );

			textEdit->append ( tr("CHUNK Checksum:") );
			datanode = treenode_get_byname ( treenode_children ( infonode ), (unsigned char*)"CHECKSUM" );
			textEdit->append ( QString ( (char*)treenode_get_content_ascii ( datanode ) ) );
			textEdit->append ( tr("") );

		}
		if ( !strcmp ( (const char*)treenode_get_name ( treenode ), "SUBCHUNK" ) )
		{
			infonode = treenode_get_byname ( treenode_children ( treenode ), (unsigned char*)"INFORMATION" );

			btnEdit->setEnabled ( false );

			textEdit->append ( tr("CHUNK ID:") );
			datanode = treenode_get_byname ( treenode_children ( infonode ), (unsigned char*)"ID" );
			textEdit->append ( QString ( (char*)treenode_get_content_ascii ( datanode ) ) );
			textEdit->append ( tr("") );

			textEdit->append ( tr("CHUNK Length:") );
			datanode = treenode_get_byname ( treenode_children ( infonode ), (unsigned char*)"LENGTH" );
			textEdit->append ( QString ( (char*)treenode_get_content_ascii ( datanode ) ) );
			textEdit->append ( tr("") );

			textEdit->append ( tr("CHUNK Flags 1:") );
			datanode = treenode_get_byname ( treenode_children ( infonode ), (unsigned char*)"FLAGS1" );
			textEdit->append ( QString ( (char*)treenode_get_content_ascii ( datanode ) ) );
			textEdit->append ( tr("") );

			textEdit->append ( tr("CHUNK Flags 2:") );
			datanode = treenode_get_byname ( treenode_children ( infonode ), (unsigned char*)"FLAGS2" );
			textEdit->append ( QString ( (char*)treenode_get_content_ascii ( datanode ) ) );
			textEdit->append ( tr("") );

			textEdit->append ( tr("CHUNK Flags 3:") );
			datanode = treenode_get_byname ( treenode_children ( infonode ), (unsigned char*)"FLAGS3" );
			textEdit->append ( QString ( (char*)treenode_get_content_ascii ( datanode ) ) );
			textEdit->append ( tr("") );

			textEdit->append ( tr("CHUNK Flags 4:") );
			datanode = treenode_get_byname ( treenode_children ( infonode ), (unsigned char*)"FLAGS4" );
			textEdit->append ( QString ( (char*)treenode_get_content_ascii ( datanode ) ) );
			textEdit->append ( tr("") );

		}

	}
	
	return 0;
}

void PPModdMainWindow::actOpenTriggered()
{
	QString name=QFileDialog::getOpenFileName ( this,
	QString("Load PPM File"),
	"",
	"All Files (*)");

	if(!name.isNull()) 
		LoadFile ( (char*)(name.toAscii().constData()) );

}

unsigned int PPModdMainWindow::LoadFile ( char *filename )
{
	int found = 0;
	QString status_message;

	ppm_start_address = E_FAIL;
	SetData ( NULL );

	//
	// Open File
	//
	status_message = "Loading PPM";
	statusbar->showMessage ( status_message );

	if( !filename ) 
		return E_FAIL;

	trix_workspace = workspace_startup ( filename );
	if ( !trix_workspace )
	{
		ui_dlg_msg ( "PPM loading failed", 0 );
		status_message = "PPM Load Failed";
		statusbar->showMessage ( status_message );

		workspace_release ( trix_workspace );
		return E_FAIL;
	}

	status_message = "PPM loaded successfully, detecting type...";
	statusbar->showMessage ( status_message );

	//
	// Check for PPM
	//
	int start = v_get_start ( trix_workspace );

	if ( v_get_b ( trix_workspace, start +0) == 'P' &&
	 	 v_get_b ( trix_workspace, start +1) == 'P' &&
	 	 v_get_b ( trix_workspace, start +2) == 'M' &&
	 	 v_get_b ( trix_workspace, start +3) == '\000' )
	{
		printf ( "BE format\n" );

		found = 1;
	}
	else if ( 
		 v_get_b ( trix_workspace, start +0) == '\000' &&
		 v_get_b ( trix_workspace, start +1) == 'M' &&
	 	 v_get_b ( trix_workspace, start +2) == 'P' &&
	 	 v_get_b ( trix_workspace, start +3) == 'P' )
	{
		printf ( "LE format\n" );
		trix_workspace->flags &= ~FLAGS_ENDIANESS;
		trix_workspace->flags |= FLAGS_ENDIANESS_LE;

		found = 1;
	}

	if ( !found )
	{
		// try to find via locate functions
		t_locator loc;

		loc.func_name = (unsigned char*)"PPM Signature";
		loc.length = 5;
		loc.pattern = (unsigned char*)"PPM\x00V";
		loc.mask = (unsigned char*)"\xFF\xFF\xFF\xFF\xFF";

		start = util_find_pattern ( trix_workspace, &loc, MEM_AUTO, MEM_AUTO, LOC_UNIQUE );

		if ( start != E_FAIL )
		{
			printf ( "[i] PPM signature found at 0x%08X.\n", start );
			found = 1;
		}
	}

	if ( !found )
	{
		t_locator loc;

		loc.func_name = (unsigned char*)"PPM Signature (relaxed)";
		loc.length = 4;
		loc.pattern = (unsigned char*)"PPM\x00";
		loc.mask = (unsigned char*)"\xFF\xFF\xFF\xFF";

		start = util_find_pattern ( trix_workspace, &loc, MEM_AUTO, MEM_AUTO, LOC_UNIQUE );

		if ( start != E_FAIL )
		{
			printf ( "[i] PPM signature (relaxed) found at 0x%08X.\n", start );
			found = 1;
		}
	}

	if ( !found )
	{
		ui_dlg_msg ( "This is no PPM file. Maybe it's a file from a nokia model which has PPM not at 32k-boundary.\n\nIn this case first load the MCU file (!) and ignore the error message.\nThen open the options dialog and set 'core.fmt.dct4crypt.skip_autobasecode' to '1'\n\nNow try to load the PPM file again.", 0 );
		status_message = "No PPM File";
		statusbar->showMessage ( status_message );

		workspace_release ( trix_workspace );
		return E_FAIL;
	}
	//
	// Parse PPM
	//
	status_message = "PPM loaded successfully, parsing...";
	statusbar->showMessage ( status_message );

	t_treenode *mainnode = ppmodify_dump_xml ( trix_workspace, start );
	
	if ( !mainnode )
	{
		ui_dlg_msg ( "This is no PPM file. Maybe it's a file from a nokia model which has PPM not at 32k-boundary.\n\nIn this case first load the MCU file (!) and ignore the error message.\nThen open the options dialog and set 'core.fmt.dct4crypt.skip_autobasecode' to '1'\n\nNow try to load the PPM file again.", 0 );

		status_message = "Invalid PPM File";
		statusbar->showMessage ( status_message );

		workspace_release ( trix_workspace );
		trix_workspace = NULL;
		ppm_start_address = E_FAIL;
		return E_FAIL;
	}

	status_message = "PPM loaded successfully, displaying...";
	statusbar->showMessage ( status_message );

	SetData ( mainnode );

	status_message = "PPM loaded successfully";
	statusbar->showMessage ( status_message );

	CHECK_AND_FREE(last_filename);
	last_filename = (unsigned char*)strdup ( filename );
	ppm_start_address = start;

	actionExport_XML->setEnabled ( true );
	actionImport_XML->setEnabled ( true );
//	actionSave->setEnabled ( true );
//	actionSave_as->setEnabled ( true );
	actionClose->setEnabled ( true );

	return E_OK;
}

void PPModdMainWindow::actQuitTriggered()
{
   QCoreApplication::exit(0);
}

void PPModdMainWindow::actOptionsTriggered()
{
	options dlg(false);
	dlg.exec();
}

void PPModdMainWindow::actWorkspaceTriggered()
{
	WorkspaceWindow dlg;
	dlg.Update ( trix_workspace );
	dlg.exec();
}

void PPModdMainWindow::actPluginsTriggered()
{
	PluginWindow dlg;

	dlg.unloading(false);
	dlg.exec ();
}


void PPModdMainWindow::actSaveTriggered()
{
}
void PPModdMainWindow::actSaveAsTriggered()
{
}

void PPModdMainWindow::actCloseTriggered()
{
	actionExport_XML->setEnabled ( false );
	actionImport_XML->setEnabled ( false );
	actionSave->setEnabled ( false );
	actionSave_as->setEnabled ( false );
	actionClose->setEnabled ( false );

	ppm_start_address = E_FAIL;
	SetData ( NULL );

	workspace_release (trix_workspace);
	trix_workspace = NULL;
}

void PPModdMainWindow::actExportTriggered()
{
}


void PPModdMainWindow::actBuildPPMTriggered()
{
#ifdef PPMODD_EMBEDDED_VERSION
	t_treenode *node = NULL;
	t_treenode *datanode = NULL;

	node = treenode_clone ( ppm_data );
	ppmodify_merge_ppm ( node );
	datanode = node;
#else
	BuildPPMProgress *build = new BuildPPMProgress ( ppm_data, server_username, server_password );

	build->UploadPPM();

	// save that created node
	t_treenode *node = build->get_node ();
	t_treenode *datanode = treenode_get_byname ( treenode_children ( node ), (unsigned char*)"PPM-BINARY" );
#endif

	if ( !datanode )
	{
		ui_dlg_msg ( "Failed generating the PPM. Sorry.", 0 );
		treenode_release ( node );
		return;
	}

	//
	// proceed saving
	//
	int pos = 0;
	int state = E_OK;
	int length = 0;
	int loop = 0;
	char *filename_template = NULL;
	char *filename = NULL;
	t_treenode *current_node = NULL;
	t_treenode *multinode = NULL;
	unsigned char *buffer = NULL; 
	t_workspace *ws = NULL;
	QFileDialog dlg;

	QString name = dlg.getSaveFileName( this, 
		QString("Save this PPM as"),
		QString::fromUtf8((const char*)(last_filename?(const char*)last_filename:(const char*)"generated.ppm")),
		"PPM File (*.*);;Compressed PPM File (*.bz2);;All Files (*.*)", 0, 0);

	if ( !name.isNull() )
	{
		filename = (char*)strdup(name.toAscii().data());

		buffer = treenode_get_content_data_binary ( datanode, &length );

		if ( !buffer || length < 0 )
		{
			ui_dlg_msg ( "Failed getting the PPM", 0 );
			treenode_release ( node );
			return ;
		}


		v_memcpy_put ( trix_workspace, ppm_start_address, buffer, length );
//		ws = workspace_create_file_from_buffer ( (unsigned char*)buffer, length );

		if ( trix_workspace && (strlen (filename) > 4) && !strncasecmp ( filename+strlen(filename)-4, ".bz2", 4 ) )
			state = file_format ( trix_workspace->fileinfo, "BZ2" );

		if ( trix_workspace && (state == E_OK) )
		{
			/* override the DRYMODE options */
			int old_options = file_set_options ( FILE_NORMAL );

			state = file_write ( filename, trix_workspace->fileinfo );
			file_set_options ( old_options );
		}
		free ( filename );

		if ( !trix_workspace || (state != E_OK) )
		{
			ui_dlg_msg ( "Failed saving the PPM", 0 );
			treenode_release ( node );
			return ;
		}
		workspace_release ( trix_workspace );
	}
	treenode_release ( node );


	LoadFile ( (char*)last_filename );
	return ;
}

void PPModdMainWindow::actImportTriggered()
{
	QString name=QFileDialog::getOpenFileName ( this,
	QString("Load XML File"),
	"",
	"XML Files (*.xml);;All Files (*)");

	QString status_message;

	//
	// Open File
	//
	status_message = "Loading XML";
	statusbar->showMessage ( status_message );

	if(!name.isNull()) 
	{
		trix_workspace = workspace_startup ( (char*)(name.toAscii().constData()) );
		if ( !trix_workspace )
		{
			ui_dlg_msg ( "XML loading failed", 0 );
			status_message = "XML Load Failed";
			statusbar->showMessage ( status_message );

			workspace_release ( trix_workspace );
			return;
		}
	}

	//
	// Parse File
	//
	status_message = "XML loaded successfully, parsing...";
	statusbar->showMessage ( status_message );

	t_treenode *mainnode = treenode_children ( xml_connector_parse ( v_get_str ( trix_workspace, 0 ) ));
	if ( !mainnode )
	{
		ui_dlg_msg ( "Parsing failed", 0 );
		status_message = "Invalid XML File";
		statusbar->showMessage ( status_message );

		workspace_release ( trix_workspace );
		return;
	}
	SetData ( mainnode );

	status_message = "PPM loaded successfully";
	statusbar->showMessage ( status_message );

}

// ---------------------------------------------------------------------------
// normal bool dialog
unsigned int __cdecl qt_dlg_bool_dlg ( char *string )
{
	QMessageBox mb ( "TriX", QString( string ), QMessageBox::Question, QMessageBox::Yes, QMessageBox::No, 0, 0 );
	mb.focusWidget();

	switch ( mb.exec() )
	{
		case QMessageBox::Yes:
			return E_OK;
			break;
		case QMessageBox::No:
			return E_FAIL;
			break;
	}
	return E_FAIL;
}

// bool dialog answered by UI thread
unsigned int __cdecl qt_dlg_bool ( char *string )
{
#ifndef TRIX_THREADED
	return qt_dlg_bool_dlg ( string );
#else
	// fallback to normal dialog
	if ( dlg_locked || mutex_locked ( trix_dialog_lock ))
		return qt_dlg_bool_dlg ( string );

	mutex_lock ( trix_dialog_lock );
	
	dialog_message = string;
	dialog_type = DLG_BOOL;

	while ( mutex_locked ( trix_dialog_lock ) );

	return dialog_bool;
#endif
}

// normal dialog
char * __cdecl qt_dlg_string_dlg ( char *def, char *string )
{
	bool ok;
	if ( !def )
		def = "";
	QString text = QInputDialog::getText ( NULL, "TriX", QString ( string ), QLineEdit::Normal, QString ( def ), &ok);

	if ( ok && !text.isEmpty())
		return (char*)strdup ( text.toAscii().data());

	return NULL;
}

// string dialog answered by UI thread
char * __cdecl qt_dlg_string ( char *def, char *string )
{
#ifndef TRIX_THREADED
	return qt_dlg_string_dlg ( def, string );
#else
	if ( dlg_locked || mutex_locked ( trix_dialog_lock ) )
		return qt_dlg_string_dlg ( def, string );

	mutex_lock ( trix_dialog_lock );

	dialog_message = string;
	dialog_type = DLG_STR;
	if ( !def )
		dialog_default = NULL;
	else
		dialog_default = (char*)strdup ( def );

	// wait until we get a string
	while ( mutex_locked ( trix_dialog_lock ) )
	{
		//sleep a bit, else it takes full CPU power
		Sleep ( 75 );
	};

	return dialog_text;

#endif
}

unsigned int __cdecl qt_dlg_msg ( char *string, int type )
{
	QMessageBox::warning(NULL, "TriX", QString(string), "Ok", 0, 0, 0, 1);
	return E_OK;
}


unsigned int __cdecl qt_dlg_tree ( t_treenode *tree )
{
	ObjectTree dlg;

	dlg.SetData ( tree );
	dlg.exec ();

	return E_OK;
}


//
//
//

t_qt_box_entry * qt_dlg_box_get_by_id ( unsigned int id )
{
	t_qt_box_entry *boxentry = qt_box_list;

	while ( boxentry )
	{
		if ( id == boxentry->id )
			return boxentry;

		LIST_NEXT ( boxentry );
	}

	return NULL;
}

unsigned int qt_dlg_box_get_free_id ( )
{
	t_qt_box_entry *boxentry = qt_box_list;
	unsigned int id = 0;
	int done = 0;

	while ( 1 )
	{
		if ( qt_dlg_box_get_by_id ( id ) != NULL )
			id++;
		else
			return id;
	}

	return E_FAIL;
}


unsigned int qt_dlg_box_create ( unsigned char *title )
{
	t_qt_box_entry *boxentry = NULL;
	unsigned int oldstate = 0;

	boxentry = LIST_MALLOC (t_qt_box_entry);
	if ( !boxentry )
		return E_FAIL;

	boxentry->id = qt_dlg_box_get_free_id ();
	boxentry->title = (unsigned char*)strdup ( (char*)title );
	boxentry->box = NULL;
	boxentry->queue = NULL;
	boxentry->release = 0;

	if ( !qt_box_mutex )
		qt_box_mutex = mutex_create ();

	//
	//
	mutex_lock ( qt_box_mutex );

	if ( qt_box_list )
		LIST_ADD ( qt_box_list, boxentry );
	else
		qt_box_list = boxentry;

	mutex_unlock ( qt_box_mutex );
	//
	//

	return boxentry->id;
}

unsigned int qt_dlg_box_size ( unsigned int id, unsigned int width, unsigned int height )
{
	t_qt_box_entry *boxentry = NULL;
	t_qt_box_queue *queue = NULL;

	//
	//
	mutex_lock ( qt_box_mutex );
	boxentry = qt_dlg_box_get_by_id ( id );
	if ( !boxentry )
	{
		mutex_unlock ( qt_box_mutex );
		return E_FAIL;
	}

	if ( !boxentry->box )
	{
		boxentry->width = width;
		boxentry->height = height;
	}
	else
		boxentry->box->SetWindowSize ( width, height );

	mutex_unlock ( qt_box_mutex );
	//
	//
	return E_OK;
}

unsigned int qt_dlg_box_clear ( unsigned int id )
{
	t_qt_box_entry *boxentry = NULL;
	t_qt_box_queue *queue = NULL;

	//
	//
	mutex_lock ( qt_box_mutex );
	boxentry = qt_dlg_box_get_by_id ( id );
	if ( !boxentry )
	{
		mutex_unlock ( qt_box_mutex );
		return E_FAIL;
	}

	queue = LIST_MALLOC ( t_qt_box_queue );
	queue->type = QT_BOX_CLEAR;
	queue->message = NULL;


	if ( boxentry->queue )
		LIST_ADD ( boxentry->queue, queue );
	else
		boxentry->queue = queue;

	mutex_unlock ( qt_box_mutex );
	//
	//
	return E_OK;
}


unsigned int qt_dlg_box_msg ( unsigned int id, unsigned char *msg )
{
	t_qt_box_entry *boxentry = NULL;
	t_qt_box_queue *queue = NULL;

	//
	//
	mutex_lock ( qt_box_mutex );
	boxentry = qt_dlg_box_get_by_id ( id );
	if ( !boxentry )
	{
		mutex_unlock ( qt_box_mutex );
		return E_FAIL;
	}

	queue = LIST_MALLOC ( t_qt_box_queue );
	queue->type = QT_BOX_APPEND;
	queue->message = (unsigned char*)strdup ( (char*)msg );



	if ( boxentry->queue )
		LIST_ADD ( boxentry->queue, queue );
	else
		boxentry->queue = queue;

	mutex_unlock ( qt_box_mutex );
	//
	//

	return E_OK;
}

unsigned int qt_dlg_box_release ( unsigned int id )
{
	/*
	t_qt_box_entry *boxentry = NULL;
	//
	//
	mutex_lock ( qt_box_mutex );
	boxentry = qt_dlg_box_get_by_id ( id );
	if ( !boxentry )
	{
		mutex_unlock ( qt_box_mutex );
		return E_FAIL;
	}
	boxentry->release = 1;

	mutex_unlock ( qt_box_mutex );
	//
	//
	return E_OK;
	*/
	t_qt_box_entry *boxentry = NULL;
	t_qt_box_queue *queue = NULL;

	//
	//
	mutex_lock ( qt_box_mutex );
	boxentry = qt_dlg_box_get_by_id ( id );
	if ( !boxentry )
	{
		mutex_unlock ( qt_box_mutex );
		return E_FAIL;
	}

	queue = LIST_MALLOC ( t_qt_box_queue );
	queue->type = QT_BOX_RELEASE;
	queue->message = NULL;


	if ( boxentry->queue )
		LIST_ADD ( boxentry->queue, queue );
	else
		boxentry->queue = queue;

	mutex_unlock ( qt_box_mutex );
	//
	//
	return E_OK;
}

unsigned int qt_dlg_box_release_all ( )
{
	/*
	t_qt_box_entry *boxentry = NULL;
	//
	//
	if ( !qt_box_mutex )
		return E_FAIL;
	mutex_lock ( qt_box_mutex );

	boxentry = qt_box_list;
	while ( boxentry )
	{
		if ( boxentry->box )
			boxentry->release = 1;

		LIST_NEXT ( boxentry );
	}

	mutex_unlock ( qt_box_mutex );
	//
	//
	return E_OK;
	*/
	t_qt_box_entry *boxentry = NULL;
	t_qt_box_queue *queue = NULL;

	//
	//
	mutex_lock ( qt_box_mutex );
	boxentry = qt_box_list;
	while ( boxentry )
	{
		queue = LIST_MALLOC ( t_qt_box_queue );
		queue->type = QT_BOX_RELEASE;
		queue->message = NULL;

		if ( boxentry->queue )
			LIST_ADD ( boxentry->queue, queue );
		else
			boxentry->queue = queue;


		LIST_NEXT ( boxentry );
	}

	mutex_unlock ( qt_box_mutex );
	//
	//
	return E_OK;
}


// running in different context
unsigned int qt_dlg_box_process ( )
{
	t_qt_box_entry *boxentry = NULL;
	t_qt_box_queue *queue = NULL;

	if ( !qt_box_mutex )
		return E_FAIL;

	mutex_lock ( qt_box_mutex );

	boxentry = qt_box_list;
	while ( boxentry )
	{
		if ( !boxentry->box )
		{
			boxentry->box = new TextBox ();

			if ( !boxentry->box )
				return E_FAIL;
			boxentry->box->SetWindowName ( boxentry->title );
			boxentry->box->setModal ( false );
			boxentry->box->setVisible ( true );
			boxentry->box->SetWindowSize ( boxentry->width, boxentry->height );

		}
		if ( boxentry->queue )
		{
			queue = boxentry->queue;
			while ( queue )
			{
				switch ( queue->type )
				{
					case QT_BOX_CLEAR:
						boxentry->box->Clear ();
						break;
					case QT_BOX_APPEND:
						boxentry->box->Append ( queue->message );
						break;
					case QT_BOX_RELEASE:
						boxentry->release = 1;
						break;
				}
				LIST_NEXT(queue);
			}
			LIST_FREE ( boxentry->queue );
		}

		if ( boxentry->release )
		{
			delete boxentry->box;
			free ( boxentry->title );
			boxentry->box = NULL;
		}
		LIST_NEXT(boxentry);
	}

	// free the items to release and the ones for which 'new' failed
	boxentry = qt_box_list;
	while ( boxentry )
	{
		t_qt_box_entry *entry = boxentry;
		
		LIST_NEXT(boxentry);
		if ( !entry->box )
			LIST_DEL_SIMPLE ( entry, qt_box_list );
	}

	mutex_unlock ( qt_box_mutex );

	return E_OK;
}



#ifdef WIN32
int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR szCmdLine,int iCmdShow)
{
	char *argv[1] = { "PPModd.exe" };
	int argc = 1;
#else
int main(int argc, char **argv)
{
#endif
	int ret, r;

	/*
	** Create the Qt application object.
	**
	*/
	QApplication app ( argc,argv );
	PPModdMainWindow win;

	trix_main_app = &app;
	trix_main_win = &win;

	/*
	** Set the organization and application names for QSettings
	**  savage and restoration.
	**
	*/
	app.setOrganizationDomain( ORG_DOMAIN );
	app.setApplicationName(    APP_NAME   );

	win.show();


	/*
	** Initialize TriX core.
	**
	*/
#ifdef TRIX_THREADED
    strcpy ( trix_out_buffer, "" );
#endif

	ret = main_setup();
	if( ret == E_OK )
	{
		trixplug_load_plugin ( "trix_ppmodify" );
		trixplug_load_plugin ( "trix_xml" );

		LOAD_PLUGIN_SYMBOL ( "PPModify", "ppmodify_dump", win.ppmodify_dump );
		LOAD_PLUGIN_SYMBOL ( "PPModify", "ppmodify_dump_xml", win.ppmodify_dump_xml );
		LOAD_PLUGIN_SYMBOL ( "PPModify", "ppmodify_build_ppm", win.ppmodify_build_ppm );
		LOAD_PLUGIN_SYMBOL ( "PPModify", "ppmodify_merge_ppm", win.ppmodify_merge_ppm );

		LOAD_PLUGIN_SYMBOL ( "XML", "xml_connector_parse", win.xml_connector_parse );
		LOAD_PLUGIN_SYMBOL ( "XML", "xml_connector_parse_file", win.xml_connector_parse_file );
		LOAD_PLUGIN_SYMBOL ( "XML", "xml_connector_create", win.xml_connector_create );
		LOAD_PLUGIN_SYMBOL ( "XML", "xml_connector_create_file", win.xml_connector_create_file );

		options_add_core_option ( OPT_STR, "ppmodd", server_username, "PPModd User name" );
		options_add_core_option ( OPT_STR, "ppmodd", server_password, "PPModd User password" );

		options_init ();

		ret = app.exec();
	}

	/*
	** Deinitialize TriX core.
	**
	*/
	r = main_cleanup();
	if ( ret == E_OK && r != E_OK ) ret = r;

	return ret;
}
//---------------------------------------------------------------------------

/* Replacement for standard vprintf(), outputs to TriX's Output dock */
int __cdecl	qt_vprintf ( const char *format, va_list args )
{
	static char buffer[TRIX_PRINTF_BUFSIZE] = "";
	char *ptr;
	int done = 0;
	int buflength = 0;

	vsprintf ( buffer + strlen(buffer), format, args);

	ptr = strrchr ( buffer, '\n' );
	if( ptr )
	{
		*ptr='\0';
#ifdef TRIX_THREADED
		if ( trix_output_pause )
		{
			strcpy ( buffer, "" );
			return 0;
		}

		buflength = strlen ( buffer );
		while ( !done && !trix_output_pause )
		{
			mutex_lock(trix_thread_lock);

			if ( strlen ( trix_out_buffer ) + buflength < TRIX_PRINTF_BUFSIZE-10 )
			{
				strcat ( trix_out_buffer, buffer );
				strcat ( trix_out_buffer, "\n" );
				trix_out_buffer_updated++;
				done = 1;
//				trix_thread_timer->setInterval (200);
			}
			else
				trix_output_flush = 1;
//				trix_thread_timer->setInterval (100);


			mutex_unlock(trix_thread_lock);
			if ( !done )
				Sleep ( 100 );
		}
#else
		_textOutput->append(QObject::tr(buffer));
#endif
		memmove ( buffer, ptr+1, strlen(ptr+1)+1 );
	}

	return 0;
}
//---------------------------------------------------------------------------
