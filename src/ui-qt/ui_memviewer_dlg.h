#ifndef _UI_MEMVIEWER_DLG_H_
#define _UI_MEMVIEWER_DLG_H_

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
#include "ui_MemoryViewer.h"

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

typedef struct s_callbacks
{
	unsigned int (*read_callback) ( void *priv, unsigned int address, unsigned int bytes, unsigned char *buffer );
	unsigned int (*valid_callback) ( void *priv, unsigned int address );
	unsigned int (*start_callback) ( void *priv );
	unsigned int (*end_callback) ( void *priv );
	void *priv;
} t_callbacks;

//---------------------------------------------------------------------------
class MemoryViewer : public QDialog, private Ui::dlgMemView
{
	Q_OBJECT

public:
	MemoryViewer (  );
	unsigned int SetCallbackFuncs ( t_callbacks *cb );
	unsigned int SetCallbackPriv ( void *priv );
	void ResetData ();
	unsigned int SetAddress ( unsigned int address );
	unsigned int GetAddress ( );
	unsigned int Stop ();
	unsigned int Start ();


private slots:
	void update ();
	void btnGotoPressed ();
	void btnOkPressed ();

private:
	unsigned int getReadAddress ();
	unsigned int request_addr;
	unsigned int current_addr;
	unsigned int start_addr;
	unsigned int end_addr;
	unsigned int disp_width;
	unsigned int disp_bytes;
	t_callbacks *callbacks;
	t_callbacks default_cb;
	QTimer *update_timer;
	unsigned int active;
	unsigned int timer_delay;
	unsigned char old_data[0x2000];
	unsigned int last_addr;


};
//---------------------------------------------------------------------------
 
#endif
