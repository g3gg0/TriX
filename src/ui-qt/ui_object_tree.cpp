/// ui_main.cpp
 
#include "ui_object_tree.h"

#ifdef WIN32
#include <Windows.h>
#endif
extern "C"
{
  
#include "defines.h"
#include "treenode.h"

#include "mem.h"

};

#include "ui_ObjectTree.h"


//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

/**
 ** The constructor initializes the environment for the first application.
 **
 */
ObjectTree::ObjectTree()
{
   /*
   ** Build the ui component
   **
   */
	setupUi(this);

	connect ( okButton, SIGNAL ( clicked() ),this, SLOT( accept() ) );
	connect ( treeObjects, SIGNAL( itemClicked(QTreeWidgetItem *, int) ), this, SLOT ( treeItemClicked (QTreeWidgetItem *, int) ) );
	connect ( treeObjects, SIGNAL( itemSelectionChanged() ), SLOT( treeItemChanged() ) );
	
   return;
}
//---------------------------------------------------------------------------

unsigned int ObjectTree::SetDataSubcall ( t_treenode *treenode, QTreeWidgetItem *tree )
{
	QTreeWidgetItem *item;

	while ( treenode )
	{
		if ( tree )
			item = new QTreeWidgetItem ( tree );
		else
			item = new QTreeWidgetItem ( treeObjects );
			
		QVariant user_data;
		qVariantSetValue ( user_data, (void*)treenode );
		item->setData ( 1, Qt::UserRole, user_data );

		if ( treenode_get_name ( treenode ) )
			item->setText ( 0, QString((char*)treenode_get_name ( treenode )) );
		else
			item->setText ( 0, tr("(unnamed)") );

		if ( treenode_children ( treenode ) )
			SetDataSubcall ( treenode_children ( treenode ), item );

		treenode = treenode_next ( treenode );
	}

	return E_OK;
}

unsigned int ObjectTree::SetData ( t_treenode *data )
{
	QTreeWidgetItem *item;
	t_treenode *treenodes = (t_treenode*)data;

	QStringList sl; 
	sl << tr( "Tree" );
    treeObjects->setHeaderLabels( sl );
    treeObjects->setColumnCount( 1 );

	treeObjects->clear ();
	SetDataSubcall ( treenodes, NULL );

	return E_OK;
}

//---------------------------------------------------------------------------
void ObjectTree::treeItemChanged (  )
{
	return;
}

//---------------------------------------------------------------------------
int ObjectTree::treeItemClicked ( QTreeWidgetItem * item, int column )
{

	t_treenode *treenode = (t_treenode *)qVariantValue <void *>( item->data ( 1, Qt::UserRole) );


	textEdit->clear ();
	textEdit->append ( tr("Details of selected Item:") );
	textEdit->append ( tr("") );

	if ( treenode )
	{
		textEdit->append ( tr("Content Type:") );
		textEdit->append ( QString ( (char*)treenode_get_content_type_raw (treenode) ) );
		textEdit->append ( tr("") );

		textEdit->append ( tr("Content Data:") );
		textEdit->append ( QString ( (char*)treenode_get_content_data (treenode) ) );
		textEdit->append ( tr("") );

		textEdit->append ( tr("Attributes:") );
		textEdit->append ( QString ( (char*)treenode_get_attributes (treenode) ) );
		textEdit->append ( tr("") );
	}
	
	return 0;
}
