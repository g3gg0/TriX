/// ui_main.cpp
 
#include "ui_main.h"
#include "ui_commandref.h"
#include "ui_object_tree.h"
#include "ui_plug.h"
#include "ui_opt.h"
#include "ui_text_box.h"
#include "ui_workspace_dlg.h"
#include "ui_memviewer_dlg.h"

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
char *trix_script_filenames[2][10];
QApplication *trix_main_app = NULL;
TriXMainWindow *trix_main_win = NULL;

//
// options
//
int mem_checking = 0;
//

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

};

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

t_scriptlist *scriptlist = NULL;

WorkspaceWindow *modeless_ws_win = NULL;
options *modeless_opt_win = NULL;
PluginWindow *modeless_plug_win = NULL;
CommandReference *modeless_ref_win = NULL;
MemoryViewer *modeless_mem_win = NULL;


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
//		options_apply_settings ( current_settings );

		free ( current_settings );

		CHECK_AND_FREE ( entry->title );
		CHECK_AND_FREE ( entry->settings );
		CHECK_AND_FREE ( entry->content );

		LIST_NEXT(entry);
	}
	return E_OK;
}



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
    virtual void dump_mem ();
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
void TriX_Script_Thread::dump_mem ()
{
	mem_check_all();
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

	if ( modeless_mem_win )
	{
		modeless_mem_win->Stop ();
		modeless_mem_win->setVisible ( false );
	}

	// release text boxes
	qt_dlg_box_release_all ();

	// free (hidden) workspace
	workspace_release ( workspace );

	LIST_FREE ( script_entries );

	if ( mem_checking )
		mem_check_all (  );
	thread_finished = 1;
}
///////////////////////////////////////////

class TriX_Dump_Thread : public QThread 
{
public:
	TriX_Dump_Thread ();
    virtual int is_active ();
    virtual int is_finished ();
    virtual void run ();
    virtual void reset ();
    virtual void abort ();

private:
	int thread_active;
	int thread_finished;
};

TriX_Dump_Thread::TriX_Dump_Thread ()
{
	thread_active = 0;
	thread_finished = 0;

}

int TriX_Dump_Thread::is_active ()
{
	return thread_active;
}
int TriX_Dump_Thread::is_finished ()
{
	return thread_finished;
}

void TriX_Dump_Thread::abort()
{
	if ( is_active() && !is_finished() )
		seer_abort_script ();
}

void TriX_Dump_Thread::reset()
{
	if ( is_active() && !is_finished() )
	{
		ui_dlg_msg ( "Still running a Dump!", 0 );
		return;
	}

	thread_active = 0;
	thread_finished = 0;

}

void TriX_Dump_Thread::run()
{
	int pos = 0;
	int file_pos = 0;
	t_fileinfo *fi = NULL;

	if ( is_active() )
	{
		ui_dlg_msg ( "Already running a Dump!", 0 );
		return;
	}

	thread_active = 1;
	mem_check_all ();
	thread_active = 0;
	thread_finished = 1;
}



static TriX_Script_Thread script_thread;
static TriX_Dump_Thread dump_thread;


//---------------------------------------------------------------------------

static QTextEdit *_textOutput;

void TriXMainWindow::timerEvent(QTimerEvent *event)
{

}
void TriXMainWindow::dragEnterEvent(QDragEnterEvent *event)
{
	if ( tabWidget->currentIndex() == 1 || tabWidget->currentIndex() == 2 )
        event->acceptProposedAction();
}

void TriXMainWindow::dropEvent(QDropEvent *event)
{
	if ( tabWidget->currentIndex() == 1 )
	{
		QString name = event->mimeData()->urls().first().toString();
		event->acceptProposedAction();

		project_add_file ( project, name.toAscii().data() );

		update_scriptlist ();
		lineScriptsName->setText ( QString ( "" ) );
		btnScriptsRemove->setEnabled(true);

	}
	else if ( tabWidget->currentIndex() == 2 )
	{
		QString name = event->mimeData()->urls().first().toString();
		event->acceptProposedAction();

		t_workspace *ws = NULL;

		ws = workspace_startup ( (char*)(name.toAscii().constData()) );
		if ( !ws )
		{
			ui_dlg_msg ( "Script loading failed", 0 );
			return;
		}
		editScriptBox->setPlainText( QString ( (const char*)v_get_str ( ws, 0 ) ) );
		workspace_release ( ws );
	}
	 
}
//---------------------------------------------------------------------------
// if we are closed, also remove the workspace window
void TriXMainWindow::setVisible (bool visible)
{

	if ( !visible )
	{
		// close all additional boxes
		qt_dlg_box_release_all ();

		if ( modeless_ws_win )
			delete modeless_ws_win;
		if ( modeless_plug_win )
			delete modeless_plug_win;
		if ( modeless_ref_win )
			delete modeless_ref_win;
		if ( modeless_opt_win )
			delete modeless_opt_win;
		if ( modeless_mem_win )
			delete modeless_mem_win;

		modeless_ws_win = NULL;
		modeless_plug_win = NULL;
		modeless_ref_win = NULL;
		modeless_mem_win = NULL;
		modeless_opt_win = NULL;

	}
	QWidget::setVisible ( visible );
	return;
}

/**
 ** The constructor initializes the environment for the first application.
 **
 */
TriXMainWindow::TriXMainWindow()
{
	int ret = 0;
   /*
   ** Build the ui component
   **
   */
   setupUi(this);
   _textOutput = textOutput; // save as global variable to make it accessible by qt_vprintf()

   trix_thread_lock = mutex_create();
   trix_dialog_lock = mutex_create();
   /*
   ** Connect custom slots to signals
   **
   */
	trix_thread_timer = new QTimer ();
	ret = connect ( trix_thread_timer, SIGNAL(timeout()), this, SLOT(evtTimerElapsed()));

	trix_thread_timer->start(100);


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
	ret = connect(actDebugInfo,SIGNAL(triggered()),this,SLOT(actDebugInfoTriggered()));

	ret = connect(textInput,SIGNAL(returnPressed()), this, SLOT(txtEnterPressed ()));

	ret = connect(actShowWorkspace,SIGNAL(triggered()),this,SLOT(actShowWorkspaceTriggered()));

	// create and add the "View" menu
	QMenu *menuView = createPopupMenu();
	menuView->setTitle(tr("View"));
	menubar->insertMenu(menuProcessing->menuAction(),menuView);

	// add the "Output" action to the toolbar
	QAction *actOutput=menuView->actions().first();
	actOutput->setIcon(QIcon(QString::fromUtf8(":/icons/icons/22x22-gartoon/chat.png")));
	toolbar->insertAction(actStart,actOutput);
	toolbar->insertSeparator(actStart);


	// simulate "New" action
	actNewTriggered();

	editScriptBox->setPlainText( QString ( DEFAULT_SCRIPT ) );
	cmbFileType->setCurrentIndex(1);
	cmbFileTypeActivated(1);

	btnScriptsManual->setDisabled ( false );
	btnScriptsManual->setText ( QString ( "Reload" ) );

	modeless_mem_win = new MemoryViewer ( );
	modeless_mem_win->setVisible ( false );
	modeless_mem_win->setModal ( false );
	modeless_mem_win->Stop ();


	// options
	options_add_core_option ( OPT_BOOL, "util", mem_checking, "Check+Dump memory after script exits" );


}
//---------------------------------------------------------------------------
void TriXMainWindow::evtTimerElapsed()
{
	int pos = 0;
	char buf[TRIX_PRINTF_BUFSIZE];

	qt_dlg_box_process ();

	if ( modeless_plug_win )
		modeless_plug_win->update_timer();

	if ( modeless_opt_win )
		modeless_opt_win->update_timer();

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

	if ( trix_out_buffer && (trix_out_buffer_updated > 0)  )
	{
		int len = 0;
		mutex_lock ( trix_thread_lock );

		pos = strlen ( trix_out_buffer );
		while ( pos && (trix_out_buffer[pos] != '\n') )
			pos--;

		if ( (trix_out_buffer[pos] == '\n') || trix_output_pause || trix_output_flush )
		{
			trix_output_flush = 0;
//			buf = (char*)strdup ( trix_out_buffer );
			memcpy ( buf, trix_out_buffer, strlen ( trix_out_buffer ) );
			buf[pos] = '\000';
			if ( !trix_output_pause )
				_textOutput->append ( buf );
//			free ( buf );
			memmove ( trix_out_buffer, trix_out_buffer+pos+1, strlen ( trix_out_buffer ) - pos );
		}

		trix_out_buffer_updated = 0;
		mutex_unlock(trix_thread_lock);
	}

	if ( script_thread.is_active () && script_thread.is_finished () )
	{
		trix_output_pause = 0;
		script_thread.reset ();
		actStart->setEnabled ( true );
		actStop->setEnabled ( false );
	}

}

void TriXMainWindow::txtEnterPressed ( )
{
	if ( mutex_locked(trix_dialog_lock ) )
	{
		CHECK_AND_FREE ( dialog_text );
		dialog_text = (char*)strdup(textInput->text().toAscii().data());
		mutex_unlock ( trix_dialog_lock );

		textInput->selectAll(); 
		textInputMessage->setText ( "" );
		textInputSymbol->setPixmap ( QPixmap() );
	}

	return;
}


// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// ----------------------------------------------------------------------


void TriXMainWindow::cmbSaveModeActivated ( int Pos )
{
	if ( Pos == 0 )
		file_set_options ( FILE_NORMAL );
	else if ( Pos == 1 )
		file_set_options ( FILE_DRYMODE );
	else if ( Pos == 2 )
		file_set_options ( FILE_DIFF );

   PROJ_ASSIGN_INT(savemode, Pos);
}

void TriXMainWindow::cmbFileTypeActivated(int Pos)
{

   if ( Pos == 0 ) 
   {
       frmSimple->hide();
       frmDCT4->hide();
       frmDCT3->hide();
	   tabWidget->widget(1)->setEnabled(false);
	   tabWidget->widget(2)->setEnabled(false);
   }
   else if ( Pos == 1 )  
   {
       frmDCT3->hide();
       frmDCT4->hide();
       frmSimple->show();
	   tabWidget->widget(1)->setEnabled(true);
	   tabWidget->widget(2)->setEnabled(true);
   }
   else if ( Pos == 2 )
   {
       frmSimple->hide();
       frmDCT4->hide();
       frmDCT3->show();
	   tabWidget->widget(1)->setEnabled(true);
	   tabWidget->widget(2)->setEnabled(true);
   }
   else if ( Pos == 3 )
   {
       frmSimple->hide();
       frmDCT3->hide();
       frmDCT4->show();
	   tabWidget->widget(1)->setEnabled(true);
	   tabWidget->widget(2)->setEnabled(true);
   }

   PROJ_ASSIGN_INT(operationmode, Pos);
}
//---------------------------------------------------------------------------
void TriXMainWindow::grpDCT3InputToggled(bool Checked)
{
   PROJ_ASSIGN_BOOL(dct3_in, Checked);
}

//---------------------------------------------------------------------------
void TriXMainWindow::grpDCT3OutputToggled(bool Checked)
{
   PROJ_ASSIGN_BOOL(dct3_out, Checked);
}

//---------------------------------------------------------------------------
void TriXMainWindow::grpDCT4InputToggled(bool Checked)
{
   PROJ_ASSIGN_BOOL(dct4_in, Checked);
}

//---------------------------------------------------------------------------
void TriXMainWindow::grpDCT4OutputToggled(bool Checked)
{
   PROJ_ASSIGN_BOOL(dct4_out, Checked);
}

//---------------------------------------------------------------------------
void TriXMainWindow::grpSimpleInputToggled(bool Checked)
{
   PROJ_ASSIGN_BOOL(simple_in, Checked);
}

//---------------------------------------------------------------------------
void TriXMainWindow::grpSimpleOutputToggled(bool Checked)
{
   PROJ_ASSIGN_BOOL(simple_out, Checked);
}

//---------------------------------------------------------------------------

void TriXMainWindow::boxDCT3CustomPPMToggled(bool Checked)
{
   PROJ_ASSIGN_BOOL(dct3_ppm_address, Checked);
}

//---------------------------------------------------------------------------

void TriXMainWindow::boxDCT3SeparatePPMToggled(bool Checked)
{
   if ( Checked && boxDCT3CustomPPM->isChecked() )
		editDCT3CustomPPM->setEnabled(true);
   else
		editDCT3CustomPPM->setEnabled(false);

   PROJ_ASSIGN_BOOL(dct3_seperate_ppm, Checked);
}

//---------------------------------------------------------------------------

void TriXMainWindow::btnSimpleInputClicked()
{
   QString name=QFileDialog::getOpenFileName(
      this,
      QString(),
      editSimpleInput->text(),
      "Any file (*.*)",
	  0,
	  0 );

   if(!name.isNull()) 
		editSimpleInput->setText(name);

}
//---------------------------------------------------------------------------

void TriXMainWindow::btnSimpleOutputClicked()
{
   QString name=QFileDialog::getSaveFileName(
      this,
      QString(),
      editSimpleOutput->text(),
      "Any file (*.*)");

   if(!name.isNull()) 
		editSimpleOutput->setText(name);
}
//
//
//

void TriXMainWindow::editSimpleInputEdited ( const QString & text )
{
	PROJ_ASSIGN_STR ( simple_input, text );
}
void TriXMainWindow::editSimpleOutputEdited ( const QString & text )
{
	PROJ_ASSIGN_STR ( simple_output, text );
}

void TriXMainWindow::editDCT3InputMCUEdited ( const QString & text )
{
	PROJ_ASSIGN_STR ( dct3_mcu_input, text );
}
void TriXMainWindow::editDCT3OutputMCUEdited ( const QString & text )
{
	PROJ_ASSIGN_STR ( dct3_mcu_output, text );
}
void TriXMainWindow::editDCT3InputPPMEdited ( const QString & text )
{
	PROJ_ASSIGN_STR ( dct3_ppm_input, text );
}
void TriXMainWindow::editDCT3OutputPPMEdited ( const QString & text )
{
	PROJ_ASSIGN_STR ( dct3_ppm_output, text );
}
void TriXMainWindow::editDCT3CustomPPMEdited ( const QString & text )
{
	PROJ_ASSIGN_INT ( dct3_ppm_address, 0 );
}

void TriXMainWindow::editDCT4InputMCUEdited ( const QString & text )
{
	PROJ_ASSIGN_STR ( dct4_mcu_input, text );
}
void TriXMainWindow::editDCT4OutputMCUEdited ( const QString & text )
{
	PROJ_ASSIGN_STR ( dct4_mcu_output, text );
}
void TriXMainWindow::editDCT4InputPPMEdited ( const QString & text )
{
	PROJ_ASSIGN_STR ( dct4_ppm_input, text );
}
void TriXMainWindow::editDCT4OutputPPMEdited ( const QString & text )
{
	PROJ_ASSIGN_STR ( dct4_ppm_output, text );
}

//---------------------------------------------------------------------------

void TriXMainWindow::btnDCT3InputMCUClicked()
{
   QString name=QFileDialog::getOpenFileName(
      this,
      QString(),
      editDCT3InputMCU->text(),
      "DCT3 Phone MCU (*.*)");

   if(!name.isNull()) 
   {
	   editDCT3InputMCU->setText(name);
		if ( project )
			project->dct3_mcu_input = (char*)strdup ( name.toAscii().data () );
   }
}
//---------------------------------------------------------------------------

void TriXMainWindow::btnDCT3InputPPMClicked()
{
   QString name=QFileDialog::getOpenFileName(
      this,
      QString(),
      editDCT3InputPPM->text(),
      "DCT3 Phone PPM (*.*)");

   if(!name.isNull()) 
   {
	    editDCT3InputPPM->setText(name);
		if ( project )
			project->dct3_ppm_input = (char*)strdup ( name.toAscii().data () );
   }
}
//---------------------------------------------------------------------------

void TriXMainWindow::btnDCT3OutputMCUClicked()
{
   QString name=QFileDialog::getSaveFileName(
      this,
      QString(),
      editDCT3OutputMCU->text(),
      "DCT3 Phone MCU (*.*)");

   if(!name.isNull()) 
   {
	   editDCT3OutputMCU->setText(name);
		if ( project )
			project->dct3_mcu_output = (char*)strdup ( name.toAscii().data () );
   }
}
//---------------------------------------------------------------------------

void TriXMainWindow::btnDCT3OutputPPMClicked()
{
   QString name=QFileDialog::getSaveFileName(
      this,
      QString(),
      editDCT3OutputPPM->text(),
      "DCT3 Phone PPM (*.*)");

   if(!name.isNull()) 
   {
	   editDCT3OutputPPM->setText(name);
		if ( project )
			project->dct3_ppm_output = (char*)strdup ( name.toAscii().data () );
   }
}
//---------------------------------------------------------------------------

void TriXMainWindow::btnDCT4InputMCUClicked()
{
   QString name=QFileDialog::getOpenFileName(
      this,
      QString(),
      editDCT4InputMCU->text(),
      "DCT4 Phone MCU (*.*)");

   if(!name.isNull()) 
   {
	   editDCT4InputMCU->setText(name);
		if ( project )
			project->dct4_mcu_input = (char*)strdup ( name.toAscii().data () );
   }
}
//---------------------------------------------------------------------------

void TriXMainWindow::btnDCT4InputPPMClicked()
{
   QString name=QFileDialog::getOpenFileName(
      this,
      QString(),
      editDCT4InputPPM->text(),
      "DCT4 Phone PPM (*.*)");

   if(!name.isNull()) 
   {
	   editDCT4InputPPM->setText(name);
		if ( project )
			project->dct4_ppm_input = (char*)strdup ( name.toAscii().data () );
   }
}
//---------------------------------------------------------------------------

void TriXMainWindow::btnDCT4OutputMCUClicked()
{
   QString name=QFileDialog::getSaveFileName(
      this,
      QString(),
      editDCT4OutputMCU->text(),
      "DCT4 Phone MCU (*.*)");

   if(!name.isNull()) 
   {
	   editDCT4OutputMCU->setText(name);
		if ( project )
			project->dct4_mcu_output = (char*)strdup ( name.toAscii().data () );
   }
}
//---------------------------------------------------------------------------

void TriXMainWindow::btnDCT4OutputPPMClicked()
{
   QString name=QFileDialog::getSaveFileName(
      this,
      QString(),
      editDCT4OutputPPM->text(),
      "DCT4 Phone PPM (*.*)");

   if(!name.isNull()) 
   {
	   editDCT4OutputPPM->setText(name);
		if ( project )
			project->dct4_ppm_output = (char*)strdup ( name.toAscii().data () );
   }
}
//---------------------------------------------------------------------------

void TriXMainWindow::actNewTriggered()
{
   cmbFileType->setCurrentIndex(0); // Simple
   cmbFileTypeActivated(cmbFileType->currentIndex());

}
//---------------------------------------------------------------------------

void TriXMainWindow::actQuitTriggered()
{
   QCoreApplication::exit(0);
}
//---------------------------------------------------------------------------

void TriXMainWindow::actPluginsTriggered()
{
	if ( !modeless_plug_win )
		modeless_plug_win = new PluginWindow ();

	modeless_plug_win->setModal(false);
	modeless_plug_win->setVisible(true);
	modeless_plug_win->show();

	if ( script_thread.is_active () )
		modeless_plug_win->unloading ( false );
	else
		modeless_plug_win->unloading ( true );
}

//---------------------------------------------------------------------------

void TriXMainWindow::actCommandRefTriggered()
{
	if ( !modeless_ref_win )
		modeless_ref_win = new CommandReference ();

	modeless_ref_win->setModal(false);
	modeless_ref_win->setVisible(true);
	modeless_ref_win->show();
}

//---------------------------------------------------------------------------

void TriXMainWindow::update_scriptlist()
{
	int pos = 0;
	t_project_files *pfile = NULL;

	if ( !project )
		return;

	listScripts->clear();

	while ( pos < project->num_files )
	{
		pfile = project_get_file ( project, pos );
		if ( !pfile )
			return;

		QListWidgetItem *item = new QListWidgetItem ();
		if ( pfile->state == PROJ_FILE_SIGNED )
			item->setIcon ( QIcon(QString::fromUtf8(":/icons/icons/22x22-gartoon/item.png")) );
		else if ( pfile->state == PROJ_FILE_OK )
			item->setIcon ( QIcon(QString::fromUtf8(":/icons/icons/22x22-gartoon/choice-yes.png")) );
		else
			item->setIcon ( QIcon(QString::fromUtf8(":/icons/icons/22x22-gartoon/choice-no.png")) );

		item->setText ( pfile->filename );
		listScripts->addItem ( item );

		pos++;
	}
}
//---------------------------------------------------------------------------
void TriXMainWindow::btnScriptsAddClicked()
{
	int length = 0;
	scScript sc;
	scInstance *in = NULL;
	char *signature = NULL;
	char *new_script = NULL;
	char *script = NULL;
	t_workspace *ws = NULL;
	t_crypt_key pub_key[] = TRIX_PUBLIC_KEYS;
	t_options *options = NULL;
	t_scriptlist *scr = NULL;
	int symbol = 0;

	if ( strlen ( (lineScriptsName->text()).toAscii().data() ) > 0 )
	{
		project_add_file ( project, lineScriptsName->text().toAscii().data() );

		update_scriptlist ();
		lineScriptsName->setText ( QString ( "" ) );
		btnScriptsRemove->setEnabled(true);
	}
}
//---------------------------------------------------------------------------


void TriXMainWindow::btnScriptsRemoveClicked()
{
	t_scriptlist *scr = scriptlist;
	char *current_name = NULL;
	int pos = 0;

	if ( !project )
		return;


	while ( pos < project->num_files )
	{
		if ( listScripts->isItemSelected ( listScripts->item ( pos ) ) )
		{
			project_del_file ( project, pos );
			delete ( listScripts->takeItem ( pos ) );
			return;
		}
		pos++;
	}

	update_scriptlist();

	//disable remove button
	if ( !listScripts->count() )
		btnScriptsRemove->setDisabled ( true );
}
//---------------------------------------------------------------------------


void TriXMainWindow::btnScriptsManualClicked()
{
	// has the functional of refreshing selected script
	t_scriptlist *scr = scriptlist;
	char *current_name = NULL;
	int pos = 0;
	int original_script_count = 0;

	if ( !project )
		return;

	original_script_count = project->num_files;


	while ( pos < original_script_count )
	{
		project_add_file ( project, listScripts->item(pos)->text().toAscii().data() );
		pos++;
	}

	pos = 0;
	while ( pos < original_script_count )
	{
		project_del_file ( project, 0 );
		delete ( listScripts->takeItem ( 0 ) );
		pos++;
	}


	update_scriptlist();
}
//---------------------------------------------------------------------------


void TriXMainWindow::actOptionsTriggered()
{
	if ( !modeless_opt_win )
		modeless_opt_win = new options (false);

	modeless_opt_win->show();
}
//---------------------------------------------------------------------------


void TriXMainWindow::btnScriptsOptionsClicked()
{
	char *current_name = NULL;
	char *current_settings = NULL;
	int pos = 0;
	t_project_files *pfile = NULL;

	if ( !project )
		return;

	while ( pos < project->num_files )
	{
		if ( listScripts->isItemSelected ( listScripts->item ( pos ) ) )
		{
			pfile = project_get_file ( project, pos );

			// register script's options
			if ( pfile->options )
				options_import_script_options ( pfile->title, pfile->options );

			// save current global settings
			current_settings = options_read_all_settings();

			// clear all settings
			options_default_all ();

			// restore the custom settings
			if ( pfile->settings )
			{
				/*
				printf ( "restore: ----------------------------\n" );
				printf ( pfile->settings );
				printf ( "-------------------------------------\n" );
				*/
				options_apply_settings ( pfile->settings );
			}

			// let user change settings
			options opts(true);
			opts.exec();

			// save settings for that script
			CHECK_AND_FREE(pfile->settings);
			pfile->settings = options_read_settings ();
/*
			if ( pfile->settings )
			{
				printf ( "changed: ----------------------------\n" );
				printf ( pfile->settings );
				printf ( "-------------------------------------\n" );
			}
*/
			// unregister options
			if ( pfile->options )
				options_delete_script_options ( pfile->title, pfile->options );

			// restore all global settings
			options_apply_settings ( current_settings );

			return;
		}
		pos++;
	}


}
//---------------------------------------------------------------------------


void TriXMainWindow::btnScriptsLoadClicked()
{
   QString name=QFileDialog::getOpenFileName(
      this,
      QString("Load Script from"),
      "scripts\\standalone",
      "TriX Script File (*.trx);;Compressed TriX Script File (*.trz);;All Files (*)");

   if(!name.isNull()) 
   {
	   lineScriptsName->setText ( name );
   }
}
//---------------------------------------------------------------------------


void TriXMainWindow::btnEditImportClicked()
{
   QString name=QFileDialog::getOpenFileName(
      this,
      QString("Load Script from"),
      "scripts\\standalone",
      "TriX Script Files (*.trx *.trz);;All Files (*)");

   if(!name.isNull()) 
   {
		t_workspace *ws = NULL;

		ws = workspace_startup ( (char*)(name.toAscii().constData()) );
		if ( !ws )
		{
			ui_dlg_msg ( "Script loading failed", 0 );
			return;
		}
		editScriptBox->setPlainText( QString ( (const char*)v_get_str ( ws, 0 ) ) );
		workspace_release ( ws );
   }
}
//---------------------------------------------------------------------------

void TriXMainWindow::btnEditExportClicked()
{
	int state = E_OK;
	char *filename = NULL;
	QFileDialog dlg;
    QString name = dlg.getSaveFileName( this, 
		QString("Save this script as"),
		"",
		"TriX Script File (*.trx);;Compressed TriX Script File (*.trz);;All Files (*)");

   if ( !name.isNull() )
   {
		t_workspace *ws = NULL;
		char *buffer = NULL; 

		filename = (char*)strdup(name.toAscii().data());
		buffer = (char*)strdup(editScriptBox->toPlainText().toAscii().data());

		ws = workspace_create_file_from_buffer ( (unsigned char*)buffer, strlen ( buffer ) );

		if ( ws && (strlen (filename) > 3) && !strncasecmp ( filename+strlen(filename)-4, ".trz", 4 ) )
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
			return;
		}

		workspace_release ( ws );

   }
}
//---------------------------------------------------------------------------
void TriXMainWindow::actDebugInfoTriggered()
{
	dump_thread.start ();
}

void TriXMainWindow::actShowWorkspaceTriggered()
{

	if ( !modeless_ws_win )
		modeless_ws_win = new WorkspaceWindow ();

	modeless_ws_win->setModal(false);
	modeless_ws_win->setVisible(true);
	modeless_ws_win->Update ( trix_workspace );
	modeless_ws_win->show();

	actShowWorkspace->setChecked(false);

	return;
}

//---------------------------------------------------------------------------
void TriXMainWindow::actStopTriggered()
{
	trix_output_pause = 1;
	unsigned char *buffer = NULL;
	unsigned char *name = NULL;

	mutex_lock ( trix_thread_lock );
	strcpy ( trix_out_buffer, "" );
	mutex_unlock ( trix_thread_lock );

	_textOutput->append ( "" );
	_textOutput->append ( "################################" );
	_textOutput->append ( "################################" );
	_textOutput->append ( "######## ABORTING SCRIPT #######" );
	_textOutput->append ( "################################" );
	_textOutput->append ( "################################" );

	script_thread.abort ();

	// restore entry box state
	CHECK_AND_FREE ( dialog_text );
	dialog_bool = 0;
	dialog_type = DLG_NONE;
	mutex_unlock ( trix_dialog_lock );

	textInput->selectAll(); 
	textInputMessage->setText ( "" );
	textInputSymbol->setPixmap ( QPixmap() );

	Sleep ( 100 );
	if ( seer_in_system () )
	{
		name = seer_in_system_name ();
		buffer = (unsigned char*)malloc ( strlen ( (char*)name ) + 50 );
		sprintf ( (char*)buffer, "#### Call name: %s", name );

		_textOutput->append ( "################################" );
		_textOutput->append ( "#### Script is currently in ####" );
		_textOutput->append ( "#### a blocking SYSTEM call ####" );
		_textOutput->append ( "########  PLEASE WAIT   ########" );
		_textOutput->append ( "################################" );
		_textOutput->append ( "####" );
		_textOutput->append ( (char*)buffer );
		_textOutput->append ( "####" );
		free ( buffer );
	}

}

void TriXMainWindow::actStartTriggered()
{
	int file_pos = 0;
	QString string;
	t_fileinfo *fi = NULL;
	t_fileinfo *last_fi = NULL;
	t_workspace *file_ws = NULL;
	t_workspace *ws = NULL;

	t_script_entry *entry = NULL;
	t_script_entry *entries = NULL;

	memset ( trix_script_filenames[0], 0x00, 4*sizeof ( char* ) );
	memset ( trix_script_filenames[1], 0x00, 4*sizeof ( char* ) );

	file_pos = 0;
	if ( cmbFileType->currentIndex() == 0 )
	{
	}
	else if ( cmbFileType->currentIndex() == 1 )
	{
		if ( grpSimpleInput->isChecked() && (strlen ( editSimpleInput->text().toAscii().data() ) == 0) )
		{
			ui_dlg_msg ( "No input file specified\nPlease Select one in the 'General' tab\n", 0 );
			return;
		}
		if ( grpSimpleInput->isChecked() && grpSimpleOutput->isChecked() && (strlen ( editSimpleOutput->text().toAscii().data()) == 0) )
		{
			ui_dlg_msg ( "No output file specified\nPlease Select one in the 'General' tab\n", 0 );
			return;
		}

		if ( grpSimpleInput->isChecked() )
		{
			trix_script_filenames[0][file_pos] = (char*)strdup(editSimpleInput->text().toAscii().data());
			if ( grpSimpleOutput->isChecked() )
				trix_script_filenames[1][file_pos] = (char*)strdup(editSimpleOutput->text().toAscii().data());
			file_pos++;
		}
	}
	else if ( cmbFileType->currentIndex() == 2 )
	{
		if ( grpDCT3Input->isChecked() && (strlen ( editDCT3InputMCU->text().toAscii().data()) == 0) )
		{
			ui_dlg_msg ( "No MCU input file specified\nPlease Select one in the 'General' tab\n", 0 );
			return;
		}
		if ( grpDCT3Input->isChecked() && grpDCT3Output->isChecked() && (strlen ( editDCT3OutputMCU->text().toAscii().data()) == 0) )
		{
			ui_dlg_msg ( "No MCU output file specified\nPlease Select one in the 'General' tab\n", 0 );
			return;
		}
		if ( grpDCT3Input->isChecked() && boxDCT3SeparatePPM->isChecked() && (strlen ( editDCT3InputPPM->text().toAscii().data()) == 0) )
		{
			ui_dlg_msg ( "No PPM input file specified\nPlease Select one in the 'General' tab\n", 0 );
			return;
		}
		if ( grpDCT3Input->isChecked() && !boxDCT3SeparatePPM->isChecked() && (strlen ( editDCT3OutputPPM->text().toAscii().data()) != 0) )
		{
			ui_dlg_msg ( "No PPM output file specified\nPlease Select one in the 'General' tab\n", 0 );
			return;
		}
		if ( grpDCT3Input->isChecked() && (strlen(editDCT3InputMCU->text().toAscii().data())) )
		{
			trix_script_filenames[0][file_pos] = (char*)strdup(editDCT3InputMCU->text().toAscii().data() );
			if ( grpDCT3Output->isChecked() && (strlen(editDCT3OutputMCU->text().toAscii().data())) )
				trix_script_filenames[1][file_pos] = (char*)strdup(editDCT3OutputMCU->text().toAscii().data() );
			file_pos++;
		}
		if ( grpDCT3Input->isChecked() && (strlen(editDCT3InputPPM->text().toAscii().data())) )
		{
			trix_script_filenames[0][file_pos] = (char*)strdup(editDCT3InputPPM->text().toAscii().data() );
			if ( grpDCT3Output->isChecked() && (strlen(editDCT3OutputPPM->text().toAscii().data())) )
				trix_script_filenames[1][file_pos] = (char*)strdup(editDCT3OutputPPM->text().toAscii().data() );
			file_pos++;
		}
	}
	else if ( cmbFileType->currentIndex() == 3 )
	{
		if ( grpDCT4Input->isChecked() && (strlen ( editDCT4InputMCU->text().toAscii().data()) == 0) )
		{
			ui_dlg_msg ( "No MCU input file specified\nPlease Select one in the 'General' tab\n", 0 );
			return;
		}
		if ( grpDCT4Input->isChecked() && grpDCT4Output->isChecked() && (strlen ( editDCT4OutputMCU->text().toAscii().data()) == 0) )
		{
			ui_dlg_msg ( "No MCU output file specified\nPlease Select one in the 'General' tab\n", 0 );
			return;
		}
		if ( grpDCT4Input->isChecked() && (strlen ( editDCT4InputPPM->text().toAscii().data()) == 0) && (strlen ( editDCT4OutputPPM->text().toAscii().data()) != 0) )
		{
			ui_dlg_msg ( "No PPM input file specified\nPlease Select one in the 'General' tab\n", 0 );
			return;
		}

		if ( grpDCT4Input->isChecked() && (strlen(editDCT4InputMCU->text().toAscii().data())) )
		{
			trix_script_filenames[0][file_pos] = (char*)strdup(editDCT4InputMCU->text().toAscii().data() );
			if ( grpDCT4Output->isChecked() && (strlen(editDCT4OutputMCU->text().toAscii().data())) )
				trix_script_filenames[1][file_pos] = (char*)strdup(editDCT4OutputMCU->text().toAscii().data() );
			file_pos++;
		}
		if ( grpDCT4Input->isChecked() && (strlen(editDCT4InputPPM->text().toAscii().data())) )
		{
			trix_script_filenames[0][file_pos] = (char*)strdup(editDCT4InputPPM->text().toAscii().data() );
			if ( grpDCT4Output->isChecked() && (strlen(editDCT4OutputPPM->text().toAscii().data())) )
				trix_script_filenames[1][file_pos] = (char*)strdup(editDCT4OutputPPM->text().toAscii().data() );
			file_pos++;
		}
	}

	file_ws = workspace_create ( );

	file_pos = 0;
	while ( trix_script_filenames[0][file_pos] )
	{
		printf ( "[ifile #%i: %s ]\n", file_pos, trix_script_filenames[0][file_pos]);
		fi = file_open ( trix_script_filenames[0][file_pos] );
		if ( !fi )
		{
			QString warning;
			warning = QString("Error opening file #%1 named '%2'\nDo you want to ignore that error?").arg(file_pos).arg( QString (trix_script_filenames[0][file_pos]) );
			QMessageBox mb ( "TriX", warning, QMessageBox::Critical, QMessageBox::No | QMessageBox::Default, QMessageBox::Ignore, 0 );
			mb.setButtonText(QMessageBox::No, "Abort");
			mb.setButtonText(QMessageBox::Ignore, "Ignore");
			
			switch ( mb.exec() )
			{
				case QMessageBox::Ignore:
					break;
				case QMessageBox::No:
					/* free workspace and return from this function */
					return;
					break;
			}
			/* just create an empty fileinfo */
			fi = file_io_create_empty ( );
		}

		if ( !file_ws->fileinfo )
			file_ws->fileinfo = fi;
		else
			LIST_ADD(file_ws->fileinfo, fi);

		last_fi = fi;
		file_pos++;
	}

	workspace_update_memmap ( file_ws );



	if ( cmbFileType->currentIndex() == 0 )
	{
		// Selected NeTriX mode
		ws = workspace_startup ( "http://trix.g3gg0.de/main" );
		if ( !ws )
		{
			ui_dlg_msg ( "Sorry, could not open 'http://trix.g3gg0.de/main'", 0 );
			return;
		}
		entry = LIST_NEW ( entries, t_script_entry );
		if ( !entries )
			entries = entry;

		entry->title = (char*)strdup("NeTriX");
		entry->options = NULL;
		entry->settings = NULL;
		entry->content = (char*)strdup ((char*)v_get_str ( ws, 0 ));

		workspace_release ( ws );
	}
	else
	{
		// any other mode
		if ( tabWidget->currentIndex() == 1 )
		{
			int pos = 0;
			t_project_files *pfile = NULL;

			if ( !project || !project->num_files )
				return;

			while ( pos < project->num_files )
			{
				pfile = project_get_file ( project, pos );
				if ( !pfile )
					return;

				entry = LIST_NEW ( entries, t_script_entry );
				if ( !entries )
					entries = entry;

				if ( pfile->title )
					entry->title = (char*)strdup(pfile->title);
				else
					entry->title = NULL;

				if ( pfile->options )
					entry->options = pfile->options;
				else
					entry->options = NULL;

				if ( pfile->settings )
					entry->settings = (char*)strdup(pfile->settings);
				else
					entry->settings = NULL;

				if ( pfile->content )
					entry->content = (char*)strdup(pfile->content);
				else
					entry->content = NULL;

				pos++;
			}
		}
		else if ( tabWidget->currentIndex() == 2 )
		{
			entry = LIST_NEW ( entries, t_script_entry );
			if ( !entries )
				entries = entry;

			entry->title = (char*)strdup("EditorTab");
			entry->options = NULL;
			entry->settings = NULL;
			entry->content = (char*)strdup(editScriptBox->toPlainText().toAscii().data());

		}
		else
		{
			workspace_release ( file_ws );
			ui_dlg_msg ( "Don't know what to execute.\nPlease Select either the 'Scripts' or the 'Editor' tab\n", 0 );
			return;
		}
	}

	_textOutput->clear();
	actStart->setEnabled ( false );
	actStop->setEnabled ( true );

	script_thread.prepare_multi ( entries, file_ws );

	script_thread.start ();

	return;
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
	// fallback to normal dialog
	if ( dlg_locked || mutex_locked ( trix_dialog_lock ))
		return qt_dlg_bool_dlg ( string );

	mutex_lock ( trix_dialog_lock );
	
	dialog_message = string;
	dialog_type = DLG_BOOL;

	while ( mutex_locked ( trix_dialog_lock ) );

	return dialog_bool;
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

	if ( !dialog_text )
		return NULL;

	return (char*)strdup (dialog_text);
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

unsigned int __cdecl qt_dlg_dumpmem ( t_workspace *ws, int address )
{
	if ( !modeless_mem_win )
		return E_FAIL;

	modeless_mem_win->SetCallbackPriv ( (void*) ws );
	modeless_mem_win->SetAddress ( address );

	modeless_mem_win->setVisible ( true );
	modeless_mem_win->Start ();

	return E_OK;
}

unsigned int __cdecl qt_dlg_dumpmem_set_addr ( int address )
{
	if ( !modeless_mem_win )
		return E_FAIL;

	return modeless_mem_win->SetAddress ( address );
}

unsigned int __cdecl qt_dlg_dumpmem_get_addr (  )
{
	if ( !modeless_mem_win )
		return E_FAIL;

	return modeless_mem_win->GetAddress();
}

unsigned int __cdecl qt_dlg_dumpmem_finish (  )
{
	if ( !modeless_mem_win )
		return E_FAIL;

	modeless_mem_win->setVisible ( false );
	return E_OK;
}

unsigned int __cdecl qt_dlg_dumpmem_visible (  )
{
	if ( !modeless_mem_win )
		return E_FAIL;

	if ( modeless_mem_win->isVisible() )
		return E_OK;

	return E_FAIL;
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
	char *argv[1] = { "TriX.exe" };
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
	TriXMainWindow win;

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
    strcpy ( trix_out_buffer, "" );

	ret = main_setup();
	if( ret == E_OK )
	{
	/*
	** Main loop.
	**
	*/
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
	int loops = 0;

	// just some security
	if ( strlen(buffer) >= TRIX_PRINTF_BUFSIZE/2 )
		sprintf ( buffer, "" );

	vsprintf ( buffer + strlen(buffer), format, args);

	ptr = strrchr ( buffer, '\n' );
	if( ptr )
	{
		*ptr='\0';
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


			mutex_unlock (trix_thread_lock);
			if ( !done )
				Sleep ( 100 );

			if ( ++loops > 10 )
				done = 1;
		}
		memmove ( buffer, ptr+1, strlen(ptr+1)+1 );
	}

	return 0;
}
//---------------------------------------------------------------------------
