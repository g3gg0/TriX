
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
#include "ui_CommandReference.h"


class QTreeWidget;

//---------------------------------------------------------------------------
class CommandReference : public QDialog, private Ui::dlgCommandReference
{
	Q_OBJECT

public:
	CommandReference();

private slots:
	void treeWidgetChanged ( );
	void plainStateChanged ( int state );

};
//---------------------------------------------------------------------------
 