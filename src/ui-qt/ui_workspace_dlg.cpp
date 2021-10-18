/// ui_main.cpp
 

#include "ui_workspace_dlg.h"

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
#include "treenode.h"
#include "workspace.h"
#include "util.h"
#include "main.h"
#include "ui.h"
#include "crypto.h"
#include "seer.h"

#include "mem.h"

extern t_workspace *trix_workspace;
};

#include "ui_Workspace.h"

int location[4] = { -1 };
t_workspace *actual_ws = NULL;

//---------------------------------------------------------------------------
unsigned int memviewer_start_callback ( void *priv )
{
	WorkspaceWindow *ws_viewer = (WorkspaceWindow *)priv;

	return ws_viewer->viewerStartAction ( );
}
//---------------------------------------------------------------------------
unsigned int memviewer_end_callback ( void *priv )
{
	WorkspaceWindow *ws_viewer = (WorkspaceWindow *)priv;

	return ws_viewer->viewerEndAction ( );
}
//---------------------------------------------------------------------------
unsigned int memviewer_valid_callback ( void *priv, unsigned int address )
{
	WorkspaceWindow *ws_viewer = (WorkspaceWindow *)priv;

	return ws_viewer->viewerValidAction ( address );
}
//---------------------------------------------------------------------------
unsigned int memviewer_read_callback ( void *priv, unsigned int address, unsigned int bytes, unsigned char *buffer )
{
	WorkspaceWindow *ws_viewer = (WorkspaceWindow *)priv;

	ws_viewer->viewerReadAction ( address, bytes, buffer );

	return E_OK;
}
//---------------------------------------------------------------------------

/**
 ** The constructor initializes the environment for the first application.
 **
 */
WorkspaceWindow::WorkspaceWindow()
{
   /*
   ** Build the ui component
   **
   */
	setupUi(this);

	QStringList list;
	list << "Item";
	treeWorkspace->setColumnCount ( 1 );
	treeWorkspace->setHeaderLabels ( list );

#ifdef TRIX
	this->memView = new MemoryViewer (  );
	this->memView->setModal ( false );
	this->memView->setVisible ( false );


	cb.start_callback = memviewer_start_callback;
	cb.end_callback = memviewer_end_callback;
	cb.valid_callback = memviewer_valid_callback;
	cb.read_callback = memviewer_read_callback;
	cb.priv = (void*)this;

	memView->SetCallbackFuncs ( &cb );
	memView->ResetData ();
#endif


	connect ( okButton, SIGNAL ( clicked() ),this, SLOT( accept() ) );
	connect ( btnExport, SIGNAL ( clicked() ),this, SLOT( btnExportClicked() ) );
	connect ( btnEdit, SIGNAL ( clicked() ),this, SLOT( btnEditClicked() ) );
	connect ( treeWorkspace, SIGNAL( itemClicked(QTreeWidgetItem *, int) ), this, SLOT ( treeWorkspaceClicked (QTreeWidgetItem *, int) ) );
	connect ( treeWorkspace, SIGNAL( itemSelectionChanged() ), SLOT( treeWorkspaceChanged() ) );
	
   return;
}

WorkspaceWindow::~WorkspaceWindow()
{
#ifdef TRIX
	delete memView;
#endif
	return;
}
//---------------------------------------------------------------------------



void WorkspaceWindow::btnEditClicked (  )
{
#ifdef TRIX
	memView->setVisible ( true );
	memView->Start ();
#endif
}

unsigned int WorkspaceWindow::viewerStartAction (  )
{
	unsigned int ret = E_FAIL;

#ifdef TRIX
	ret = viewerAction ( ACTION_START, 0, 0, NULL ) ;
	
	if ( ret == E_FAIL )
	{
		memView->Stop ();
		memView->setVisible ( false );
		return E_FAIL;
	}
#endif
	return ret;
}
unsigned int WorkspaceWindow::viewerEndAction (  )
{
	unsigned int ret = E_FAIL;

#ifdef TRIX
	ret = viewerAction ( ACTION_END, 0, 0, NULL ) ;
	
	if ( ret == E_FAIL )
	{
		memView->Stop ();
		memView->setVisible ( false );
		return E_FAIL;
	}
#endif

	return ret;
}

unsigned int WorkspaceWindow::viewerValidAction ( unsigned int address )
{
	unsigned int ret = E_FAIL;

#ifdef TRIX
	ret = viewerAction ( ACTION_VALID, address, 0, NULL ) ;
	
	if ( ret == E_FAIL )
	{
		memView->Stop ();
		memView->setVisible ( false );
		return E_FAIL;
	}
#endif

	return ret;
}
unsigned int WorkspaceWindow::viewerReadAction ( unsigned int address, unsigned int bytes, unsigned char *buffer)
{
	unsigned int ret = E_FAIL;

#ifdef TRIX
	ret = viewerAction ( ACTION_READ_MEM, address, bytes, buffer ) ;
	
	if ( ret == E_FAIL )
	{
		memView->Stop ();
		memView->setVisible ( false );
		return E_FAIL;
	}
#endif

	return ret;
}

unsigned int WorkspaceWindow::viewerAction ( int action, unsigned int address, unsigned int bytes, unsigned char *buffer )
{
#ifdef TRIX
	int pos = 0;
	t_workspace *ws = actual_ws;
	t_fileinfo *fi = NULL;
	t_stage *stage = NULL;
	t_segment *segment = NULL;
	int state = E_OK;
	char *filename = NULL;
	char *export_buffer = NULL; 
	unsigned long export_length = 0;
	
	if ( !ws )
		return E_FAIL;

	// not a stage or segment
	if ( location[0] == -1 || location[1] == -1 || location[2] == -1  )
		return E_FAIL;

	pos = location[0];
	while ( pos > 0 )
	{
		if ( !ws || !ws->next )
			return E_FAIL;
		ws = ws->next;
		pos--;
	}
	fi = ws->fileinfo;

	pos = location[1];
	while ( pos > 0 )
	{
		if ( !fi || !fi->next )
			return E_FAIL;
		fi = fi->next;
		pos--;
	}
	stage = fi->stages;

	pos = location[2];
	while ( pos > 0 )
	{
		if ( !stage || !stage->next )
			return E_FAIL;
		stage = stage->next;
		pos--;
	}
	segment = stage->segments;

	pos = location[3];
	while ( pos > 0 )
	{
		if ( !segment || !segment->next )
			return E_FAIL;
		segment = segment->next;
		pos--;
	}


	/* if a segment was selected */
	if ( location[3] != -1 && location[2] != -1 && location[1] != -1 && location[0] != -1 )
	{
		if ( action == ACTION_START )
			return segment->start;
		if ( action == ACTION_END )
			return segment->start + segment->length;

		if ( action == ACTION_VALID )
		{
			if ( !v_valid_raw ( (t_priv*)segment, address ) )
				return 0;
			else
				return 1;
		}

		v_memcpy_get_raw ( (t_priv*)segment, buffer, address, bytes );
		return E_OK;
	}
	/* if a stage was selected */
	else if ( location[3] == -1 && location[2] != -1 && location[1] != -1 && location[0] != -1 )
	{
		if ( action == ACTION_START )
			return stage_get_start ( stage );
		if ( action == ACTION_END )
			return stage_get_end ( stage );

		if ( action == ACTION_VALID )
		{
			if ( !v_valid_raw ( (t_priv*)stage, address ) )
				return 0;
			else
				return 1;
		}

		v_memcpy_get_raw ( (t_priv*)stage, buffer, address, bytes );
		return E_OK;
	}

	memView->Stop ();
	memView->setVisible ( false );

#endif

	return E_FAIL;
}

void WorkspaceWindow::btnExportClicked (  )
{
	int pos = 0;
	t_workspace *ws = actual_ws;
	t_fileinfo *fi = NULL;
	t_stage *stage = NULL;
	t_segment *segment = NULL;
	int state = E_OK;
	char *filename = NULL;
	char *export_buffer = NULL; 
	unsigned long export_length = 0;
	
	if ( !ws )
		return;

	if ( location[0] == -1 || location[1] == -1 || location[2] == -1  )
	{
		ui_dlg_msg ( "Can not Export that type. Please select a Stage or Segment.", 0 );
		return;
	}

	pos = location[0];
	while ( pos > 0 )
	{
		if ( !ws || !ws->next )
		{
			ui_dlg_msg ( "ERROR: workspace seems to have changed?", 0 );
			return;
		}
		ws = ws->next;
		pos--;
	}
	fi = ws->fileinfo;

	pos = location[1];
	while ( pos > 0 )
	{
		if ( !fi || !fi->next )
		{
			ui_dlg_msg ( "ERROR: workspace seems to have changed?", 0 );
			return;
		}
		fi = fi->next;
		pos--;
	}
	stage = fi->stages;

	pos = location[2];
	while ( pos > 0 )
	{
		if ( !stage || !stage->next )
		{
			ui_dlg_msg ( "ERROR: workspace seems to have changed?", 0 );
			return;
		}
		stage = stage->next;
		pos--;
	}
	segment = stage->segments;

	pos = location[3];
	while ( pos > 0 )
	{
		if ( !segment || !segment->next )
		{
			ui_dlg_msg ( "ERROR: workspace seems to have changed?", 0 );
			return;
		}
		segment = segment->next;
		pos--;
	}


	/* if a segment was selected */
	if ( location[3] != -1 && location[2] != -1 && location[1] != -1 && location[0] != -1 )
	{
		export_length = segment->length;
		export_buffer = (char*)malloc ( export_length );
		memcpy ( export_buffer, segment->data, export_length );
	}
	/* if a stage was selected */
	else if ( location[3] == -1 && location[2] != -1 && location[1] != -1 && location[0] != -1 )
	{
		int start_address = 0;
		int end_address = 0;
		char *message = (char*)malloc ( 200 );

		if ( stage_has_overlaps ( stage ) )
		{
			ui_dlg_msg ( "Sorry, the segments in this stage overlap. Please save them manually.", 0 );
			return;
		}
		if ( stage_has_unmapped ( stage ) )
			ui_dlg_msg ( "In this stage is one or more segments that will not get saved.\nThese stages do not belong to the memory space.\nYou will find them by looking for segments which start at '0' and also end at '0'.", 0 );

		start_address = stage_get_start ( stage );
		end_address = stage_get_end ( stage );

		if ( start_address == E_FAIL || end_address == E_FAIL )
		{
			ui_dlg_msg ( "Sorry, some error occured while checking the segments.", 0 );
			return;
		}

		sprintf ( message, "The file would occupy %i bytes. Do you want to save it?", end_address-start_address );
		if ( ui_dlg_bool ( message ) != E_OK )
		{
			free ( message );
			return;
		}
		free ( message );

		export_length = end_address-start_address;
		export_buffer = (char*)malloc ( export_length );
		memset ( export_buffer, 0xFF, export_length );

		if ( stage_merge ( stage, (unsigned char*)export_buffer, export_length ) != E_OK )
		{
			ui_dlg_msg ( "Sorry, some error occured while merging the segments.", 0 );
			return;
		}

	}


	/* export now */
	QFileDialog dlg;
    QString name = dlg.getSaveFileName( this, 
		QString("Save this data block as"),
		"",
		"Binary File (*.bin);;Compressed Binary File (*.bin.bz2);;All Files (*)");

	if ( !name.isNull() )
	{
		t_workspace *ws = NULL;
		filename = (char*)strdup(name.toAscii().data());
		
		ws = workspace_create_file_from_buffer ( (unsigned char*)export_buffer, export_length );

		if ( ws && (strlen (filename) > 3) && !strncasecmp ( filename+strlen(filename)-4, ".bz2", 4 ) )
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
			ui_dlg_msg ( "Failed saving the data", 0 );
			return;
		}

		workspace_release ( ws );
	}

	return;
}

//---------------------------------------------------------------------------
void WorkspaceWindow::treeWorkspaceChanged (  )
{
	unsigned char order[] = { 'W', 'F', 'T', 'S' };
	int pos = 0;
	char *id = NULL;
	int done = 0;

	QList <QTreeWidgetItem *> items = treeWorkspace->selectedItems();

	if ( items.count() != 1 )
		return;

	textItemInfo->setPlainText ( items.first()->text ( 1 ) );

	// why doesnt that work with ->toAscii()->data() ?!
	QByteArray byteArray = items.first()->text(2).toAscii();
	id = byteArray.data();

	if ( !id )
		return;


	for ( pos = 0; pos<4; pos++ )
		location[pos] = -1;

	pos = 0;
	while ( !done )
	{
		if ( *id == order[pos] )
		{
			id++;
			while ( *id <= '9' && *id >= '0' )
			{
				if ( location[pos] == -1 )
					location[pos] = 0;

				location[pos] *= 10;
				location[pos] += *id - '0';
				id++;
			}
		}
		else
		{
			done = 1;
		}
		pos++;
	}
#ifdef TRIX
	memView->ResetData ();
#endif

	return;
}

//---------------------------------------------------------------------------
int WorkspaceWindow::treeWorkspaceClicked ( QTreeWidgetItem * item, int column )
{
	return 0;
}

unsigned int WorkspaceWindow::UpdateFileinfo ( t_fileinfo *f, QTreeWidgetItem *item, unsigned char *parent_loc )
{
	QTreeWidgetItem *child;
	int pos = 0;
	unsigned char location[16];

	while ( f )
	{
		QStringList list;
		sprintf ( (char*)location, "%sF%i", (char*)parent_loc, pos );
		list << QString ( "File #%1, %2" ).arg(pos).arg(f->filename);
		list << QString ( "ID:\t%1\nFlags:\t0x%2\nName:\t0x%3\nPath:\t0x%4\nOptions:\t0x%5" ).arg((char*)location).arg((int)f->flags,0,16).arg(f->filename).arg(f->path).arg(f->options,0,16);
		list << QString ( (const char*)location );
		child = new QTreeWidgetItem ( item, list );
		UpdateStages ( f->stages, child, location );
		f = f->next;
		pos++;
	}

	return E_OK;
}
unsigned int WorkspaceWindow::UpdateStages ( t_stage *s, QTreeWidgetItem *item, unsigned char *parent_loc )
{
	QTreeWidgetItem *child;
	int pos = 0;
	unsigned char location[16];

	while ( s )
	{
		QStringList list;
		sprintf ( (char*)location, "%sT%i", (char*)parent_loc, pos );
		list << QString ( "Stage #%1, %2" ).arg(pos).arg(s->name);
//		list << QString ( "Stage #%1, %2 (0x%3)" ).arg(pos).arg(s->name).arg((int)s,0,16);
		list << QString ( "ID:\t%1\nFlags:\t0x%2\nLength:\t0x%3" ).arg((char*)location).arg((int)s->flags,0,16).arg(s->length,0,16);
		list << QString ( (const char*)location );
		child = new QTreeWidgetItem ( item, list );
		UpdateSegments ( s->segments, child, location );
		s = s->next;
		pos++;
	}

	return E_OK;
}
unsigned int WorkspaceWindow::UpdateSegments ( t_segment *s, QTreeWidgetItem *item, unsigned char *parent_loc )
{
	QTreeWidgetItem *child;
	int pos = 0;
	unsigned char location[16];

	while ( s )
	{
		QStringList list;
		sprintf ( (char*)location, "%sS%i", (char*)parent_loc, pos );
		list << QString ( "Segment #%1, %2" ).arg(pos).arg(s->name);
//		list << QString ( "Segment #%1, %2 (0x%3)" ).arg(pos).arg(s->name).arg((int)s,0,16);
		list << QString ( "ID:\t%1\nFlags:\t0x%2\nStart:\t0x%3\nEnd:\t0x%4\nLength:\t0x%5" ).arg((char*)location).arg((int)s->flags,0,16).arg(s->start,0,16).arg(s->end,0,16).arg(s->length,0,16);
		list << QString ( (const char*)location );
		child = new QTreeWidgetItem ( item, list );
		s = s->next;
		pos++;
	}

	return E_OK;
}

unsigned int WorkspaceWindow::UpdateWorkspaces ( t_workspace *ws )
{
	QTreeWidgetItem *item;
	int pos = 0;
	unsigned char location[16];

	actual_ws = ws;
	while ( ws )
	{
		QStringList list;
		sprintf ( (char*)location, "W%i", pos );
		list << QString ( "Workspace #%1" ).arg(pos);
//		list << QString ( "Workspace #%1, (0x%3)" ).arg(pos).arg((int)ws,0,16);
		list << QString ( "ID:\t%1\nFlags:\t0x%2" ).arg((char*)location).arg((int)ws->flags,0,16);
		list << QString ( (const char*)location );

		item = new QTreeWidgetItem ( list );
		UpdateFileinfo ( ws->fileinfo, item, location );
		treeWorkspace->addTopLevelItem ( item );
		ws = ws->next;
		pos++;
	}

	return E_OK;
}

unsigned int WorkspaceWindow::Update ( t_workspace *ws )
{
	int pos = 0;

	for ( pos = 0; pos<4; pos++ )
		location[pos] = -1;

	treeWorkspace->clear();
	if ( !ws )
	{
#ifdef TRIX
		memView->Stop ();
		memView->setVisible ( false );
#endif
		QTreeWidgetItem *item = new QTreeWidgetItem ( QStringList(QString("No Workspace defined")) );
		treeWorkspace->insertTopLevelItem ( 0, item );
		return E_FAIL;
	}

	UpdateWorkspaces ( ws );
#ifdef TRIX
	memView->ResetData ();
#endif

	return E_OK;
}
