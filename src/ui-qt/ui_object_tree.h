
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
#include "ui_ObjectTree.h"

extern "C"
{
#include "defines.h"
#include "treenode.h"

};

class QTreeWidget;

//---------------------------------------------------------------------------
class ObjectTree : public QDialog, private Ui::dlgObjectTreeDisplay
{
	Q_OBJECT

public:
	ObjectTree();
	unsigned int SetData ( t_treenode *data );
	unsigned int SetDataSubcall ( t_treenode *treenode, QTreeWidgetItem *tree );

private slots:
	int  treeItemClicked ( QTreeWidgetItem * item, int column );
	void treeItemChanged (  );

};
//---------------------------------------------------------------------------
 