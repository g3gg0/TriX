
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
#include "ui_TextEdit.h"

extern "C"
{
#include "defines.h"
#include "treenode.h"

};

class QTreeWidget;

class PPModd_TextEdit_Thread : public QThread 
{
public:
	PPModd_TextEdit_Thread ( t_treenode *treenode, QTableWidget *widget );
    virtual int is_active ();
    virtual int is_finished ();
    virtual void run ();
    virtual void reset ();
    virtual void abort ();
	int get_comm_entries();

private:
	int thread_active;
	int thread_finished;
	int thread_abort;
	int comm_entries;
	t_treenode *node;
	QTableWidget *tableWidget;
};

//---------------------------------------------------------------------------
class TextEdit : public QDialog, private Ui::dlgTextEdit
{
	Q_OBJECT

public:
	TextEdit( t_treenode *node );

public slots:
	void accept ();

private:
	t_treenode *sourcenode;
	unsigned int comm_entries;
	PPModd_TextEdit_Thread *thread;



};
//---------------------------------------------------------------------------
 