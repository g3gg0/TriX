/// ui_main.cpp
 
#include "ui_commandref.h"

#ifdef WIN32
#include <Windows.h>
#endif
extern "C"
{
//#include "mem.h"
};

#include "ui_CommandReference.h"

#include "seer_auto.h"


//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

/**
 ** The constructor initializes the environment for the first application.
 **
 */
CommandReference::CommandReference()
{
	HEADER_DEFINE(headers);
	int pos = 0;
	QTreeWidgetItem *child = NULL;
	QStringList title;

	/*
	** Build the ui component
	**
	*/
	setupUi(this);

	connect ( okButton, SIGNAL ( clicked() ),this, SLOT( accept() ) );
	connect ( treeWidget, SIGNAL( itemSelectionChanged() ), SLOT( treeWidgetChanged() ) );
	connect ( chkPlain, SIGNAL( stateChanged ( int ) ), SLOT( plainStateChanged ( int ) ) );


	while ( headers[pos] && headers[pos+1] && headers[pos+2]  )
	{
		QStringList list;
		list << QString ( "%1" ).arg(headers[pos]);
		list << QString ( "%1" ).arg(headers[pos+1]);
		list << QString ( "%1" ).arg(headers[pos+2]);

		child = new QTreeWidgetItem ( list );
		treeWidget->addTopLevelItem ( child );
		pos+=3;
	}

	title << "Header File";
	treeWidget->setColumnCount ( 1 );
	treeWidget->setHeaderLabels ( title );
	treeWidget->setSortingEnabled ( true );
	treeWidget->sortByColumn ( 0 );
	treeWidget->sortItems ( 0, Qt::AscendingOrder );

	plainStateChanged ( chkPlain->checkState () );

	return;
}
//---------------------------------------------------------------------------

void CommandReference::plainStateChanged ( int state )
{
	if ( state == Qt::Checked )
	{
		textEdit->setHidden ( false );
		textBrowser->setHidden ( true );
	}
	else
	{
		textEdit->setHidden ( true );
		textBrowser->setHidden ( false );
	}

	return;
}

//---------------------------------------------------------------------------
void CommandReference::treeWidgetChanged (  )
{
	QList <QTreeWidgetItem *> items = treeWidget->selectedItems();

	if ( items.count() != 1 )
		return;

	textEdit->setPlainText ( items.first()->text ( 1 ) );

	QTextDocument *editorDocument = textBrowser->document();
	editorDocument->setHtml ( items.first()->text ( 2 ) );

	return;
}
