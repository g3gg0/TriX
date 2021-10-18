/// ui_main.cpp
 
#include "ui_plug.h"

#ifdef WIN32
#include <Windows.h>
#endif
extern "C"
{
#include "defines.h"
#include "trixplug.h"
#include "mutex.h"
};

#include "ui_Plugins.h"


t_mutex *plug_refresh_mutex = NULL;

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

void plug_update_changes ( void *parm )
{
	PluginWindow *win = (PluginWindow*)parm;
	

	if ( !parm )
		return;

	win->needs_update = 1;
}

/**
 ** The constructor initializes the environment for the first application.
 **
 */
PluginWindow::PluginWindow()
{
	/*
	** Build the ui component
	**
	*/
	setupUi(this);
	plug_refresh_mutex = mutex_create();

	unload_allowed = true;

	connect ( okButton, SIGNAL ( clicked() ),this, SLOT( accept() ) );
	connect ( btnUnload, SIGNAL ( clicked() ),this, SLOT( btnUnloadClicked() ) );
	connect ( treeWidget, SIGNAL( itemSelectionChanged() ), SLOT( treeWidgetChanged() ) );

	refresh_content();
	needs_update = 0;

	trixplug_set_notification ( (void*)plug_update_changes, this );
}

void PluginWindow::update_timer()
{
	if ( needs_update )
	{
		needs_update = 0;
		refresh_content ();
	}
}

void PluginWindow::refresh_content()
{
	QTreeWidgetItem *child = NULL;
	QStringList title;
	t_seer_dict *dict = NULL;
	t_plugin_entry *plugins = NULL;

	plugins = trixplug_get_plugins ();
	
	treeWidget->clear();
	while ( plugins )
	{
		QStringList list;
		t_plugin_info *pi = NULL;
		QString information;

		list << QString ( "%1" ).arg(plugins->pluginname);

		information += QString ( "<table cellspacing=\"3\">" );
		information += QString ( "<tr>" );
		information += QString ( "<td><b>Filename:</b></td><td></td><td>%1</td>\r\n" ).arg(plugins->filename);
		information += QString ( "</tr>" );

		information += QString ( "<tr>" );
		pi = trixplug_get ( plugins->info, PLUGIN_INFO_VERSION );
		if ( pi )
			information += QString ( "<td><b>Version:</b></td><td></td><td>v%1.%2\r\n" ).arg((int)pi->param1).arg((int)pi->param2);
		else
			information += QString ( "<td><b>Version:</b></td><td></td><td>unknown\r\n" );
		information += QString ( "</tr>" );

		information += QString ( "<tr>" );
		information += QString ( "<td><b>Headers:</b></td><td></td><td></td>\r\n" );
		information += QString ( "</tr>" );
		dict =  plugins->headers;
		while ( dict )
		{
			information += QString ( "<tr>" );
			information += QString ( "<td></td><td></td><td>%1</td>\r\n" ).arg(dict->name);
			information += QString ( "</tr>" );
			LIST_NEXT ( dict );
		}

		information += QString ( "</tr>" );

		information += QString ( "<tr>" );
		information += QString ( "<td><b>Symbols:</b></td><td></td><td></td>\r\n" );
		information += QString ( "</tr>" );
		dict =  plugins->symbols;
		while ( dict )
		{
			information += QString ( "<tr>" );
			information += QString ( "<td></td><td></td><td>%1</td>\r\n" ).arg(dict->name);
			information += QString ( "</tr>" );
			LIST_NEXT ( dict );
		}
		information += QString ( "</tr>" );
		information += QString ( "</table>" );

		list << information;

		child = new QTreeWidgetItem ( list );
		treeWidget->addTopLevelItem ( child );
		LIST_NEXT ( plugins );
	}

	title << "Plugin";
	treeWidget->setColumnCount ( 1 );
	treeWidget->setHeaderLabels ( title );
	treeWidget->setSortingEnabled ( true );
	treeWidget->sortByColumn ( 0 );
	treeWidget->sortItems ( 0, Qt::AscendingOrder );

	txtDetails->setPlainText ( "" );
	return;
}

void PluginWindow::unloading ( bool state )
{
	unload_allowed = state;

	if ( state )
	{
		QList <QTreeWidgetItem *> items = treeWidget->selectedItems();

		if ( items.count() != 1 || !items.first() )
			btnUnload->setEnabled(false);
		else
			btnUnload->setEnabled(true);

	}
	else
		btnUnload->setEnabled(false);

}
//---------------------------------------------------------------------------

void PluginWindow::btnUnloadClicked()
{
	QList <QTreeWidgetItem *> items = treeWidget->selectedItems();

	if ( items.count() != 1 || !items.first() )
	{
		btnUnload->setEnabled(false);
		return;
	}

	trixplug_unload_plugin ( (unsigned char*) items.first()->text ( 0 ).toLocal8Bit().data() );

}


//---------------------------------------------------------------------------
void PluginWindow::treeWidgetChanged (  )
{
	QList <QTreeWidgetItem *> items = treeWidget->selectedItems();

	if ( items.count() != 1 )
	{
		btnUnload->setEnabled(false);
		return;
	}
	if ( unload_allowed )
		btnUnload->setEnabled(true);

	txtDetails->setHtml ( items.first()->text ( 1 ) );


	return;
}
