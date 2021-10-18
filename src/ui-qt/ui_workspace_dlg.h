#ifndef _UI_WORKSPACE_DLG_H_
#define _UI_WORKSPACE_DLG_H_

/*
** Include any stock Qt files here...
**
*/
#include <Qt/qapplication.h>
#include <QtGui/QtGui>
#include <Qt/qdialog.h>
#include <Qt/qtreewidget.h>
#include <Qt/qdebug.h>
#include <Qt/qobject.h>
//---------------------------------------------------------------------------
 
/*
** Include any application specific headers here.
**
*/
#include "ui_Workspace.h"
#ifdef TRIX
#include "ui_memviewer_dlg.h"
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

#include "mem.h"

};

class QTreeWidget;

#define ACTION_READ_MEM 0
#define ACTION_START    1
#define ACTION_END      2
#define ACTION_VALID    3
//---------------------------------------------------------------------------
class WorkspaceWindow : public QDialog, private Ui::dlgWorkspaceDisplay
{
	Q_OBJECT

public:
	WorkspaceWindow();
	~WorkspaceWindow();
	unsigned int Update ( t_workspace *ws );
	unsigned int UpdateWorkspaces ( t_workspace *ws );
	unsigned int UpdateFileinfo ( t_fileinfo *f, QTreeWidgetItem *item, unsigned char *parent_loc );
	unsigned int UpdateStages ( t_stage *s, QTreeWidgetItem *item, unsigned char *parent_loc );
	unsigned int UpdateSegments ( t_segment *s, QTreeWidgetItem *item, unsigned char *parent_loc );
	unsigned int viewerStartAction (  );
	unsigned int viewerEndAction (  );
	unsigned int viewerValidAction ( unsigned int address );
	unsigned int viewerReadAction ( unsigned int address, unsigned int bytes, unsigned char *buffer);
	unsigned int viewerAction ( int action, unsigned int address, unsigned int bytes, unsigned char *buffer);

private slots:
	void btnExportClicked (  );
	void btnEditClicked ( );
	int  treeWorkspaceClicked ( QTreeWidgetItem * item, int column );
	void treeWorkspaceChanged (  );

private:
#ifdef TRIX
	MemoryViewer *memView;
	t_callbacks cb;
#endif

};
//---------------------------------------------------------------------------


#endif
