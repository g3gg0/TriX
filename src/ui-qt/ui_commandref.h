
/*
** Include any stock Qt files here...
**
*/
#include <QtGui/QApplication>
#include <QtGui/QtGui>
#include <QtGui/QDialog>
#include <QtGui/QTreeWidget>
#include <QtCore/QDebug>
#include <QtCore/QObject>
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
 