/// ui_main.cpp
 
#include "ui_opt.h"

#ifdef WIN32
#include <Windows.h>
#endif
extern "C"
{
#include "defines.h"

#include "mem.h"

	
#include "segment.h"
#include "stage.h"
#include "symbols.h"

#include "fmt.h"
#include "file_io.h"
#include "file.h"
#include "workspace.h"
#include "util.h"
#include "mutex.h"

#include "options.h"

};

#include "ui_options.h"

t_mutex *refresh_mutex = NULL;


void update_changes ( void *parm )
{
	options *opt = (options*)parm;
	
	if ( !parm )
		return;

	opt->needs_update = 1;
}

//#include "seer_auto.h"


//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

/**
 ** The constructor initializes the environment for the first application.
 **
 */
options::options( bool modal )
{
	//HEADER_DEFINE(headers);


	setupUi(this);
	refresh_mutex = mutex_create();

	//connect default button
	connect ( pushButton_Ok, SIGNAL ( clicked() ), this, SLOT ( accept_options() ) );
	connect ( pushButton_Apply, SIGNAL ( clicked() ), this, SLOT ( apply_options() ) );
	connect ( pushButton_cancel, SIGNAL ( clicked() ), this, SLOT ( close() ) );
	connect ( pushButton_default, SIGNAL ( clicked() ), this, SLOT ( default_option() ) );
	connect ( treeWidget_options, SIGNAL ( itemClicked(QTreeWidgetItem *, int) ), this, SLOT ( sel_changed(QTreeWidgetItem *, int) ) );
	connect ( lineEdit_current_option, SIGNAL ( textEdited ( const QString& ) ), this, SLOT ( value_edited ( const QString& ) ) );

	refresh_content ();


	if ( !modal )
		options_set_notification ( (void*)update_changes, (void*)this );

	setModal ( modal );
}

options::~options()
{
	if ( !isModal () ) 
		options_set_notification ( NULL, NULL );
}

void options::update_timer()
{
	if ( needs_update )
	{
		needs_update = 0;
		refresh_content ();
	}
}

void options::refresh_content ()
{
	QTreeWidgetItem *entry = NULL;
	QTreeWidgetItem *parent = NULL;
	QTreeWidgetItem *tmp = NULL;
	QList<QTreeWidgetItem*> items;

	char *allopts = NULL;
	char *oneopt = NULL;
	char *opt_part = NULL;
	char *opt_part_tmp = NULL;
	int i = 0;
	int line = 0;
	static int updating = 0;

	mutex_lock ( refresh_mutex );

	treeWidget_options->clear();

	allopts = options_get_options ();

	while ( oneopt = util_get_line ( allopts, line++ ) )
	{
		parent = NULL;

		//create QT tree structure
		i = 0;
		while ( opt_part = option_get_part ( oneopt, i ) )
		{
			items = treeWidget_options->findItems ( opt_part, Qt::MatchExactly | Qt::MatchRecursive, 0 );
			opt_part_tmp = option_get_part ( oneopt, i + 1 );
			if ( !items.count() || !opt_part_tmp )
			{
				//create a parent for this "option"
				if ( !parent )
					parent = new QTreeWidgetItem ( treeWidget_options );
				else
				{
					tmp = parent;
					parent = new QTreeWidgetItem ( parent );
					treeWidget_options->setItemExpanded ( tmp, true );
					
				}
				parent->setText ( 0, opt_part );
				
			}
			else
				parent = items.at(0);

			CHECK_AND_FREE ( opt_part_tmp );
			CHECK_AND_FREE ( opt_part );
			i++;
		}

		entry = parent;

		switch ( options_get_type ( oneopt ) )
		{
		case OPT_BOOL:
			entry->setText ( 1, "bool" );
			entry->setText ( 2, QString ( "%1" ).arg ( options_get_bool( oneopt ) ) );
			break;
		case OPT_INT:
			entry->setText ( 1, "int" );
			entry->setText ( 2, QString ( "%1" ).arg ( options_get_int( oneopt ) ) );
			break;
		case OPT_HEX:
			entry->setText ( 1, "hex" );
			entry->setText ( 2, QString ( "0x%1" ).arg ( options_get_int( oneopt ), 0, 16 ) );
			break;
		case OPT_STR:
			entry->setText ( 1, "string" );
			entry->setText ( 2, QString ( "%1" ).arg ( options_get_string( oneopt ) ) );
			break;
		default:
			entry->setText ( 1, "unknown" );
		}

		QFont boldFont = QFont ();
		boldFont.setBold ( 1 );

		if ( options_has_default ( oneopt ) )
			entry->setText ( 3, "no" );
		else
		{
			entry->setText ( 3, "yes" );
			entry->setFont ( 0, boldFont );
			entry->setFont ( 1, boldFont );
			entry->setFont ( 2, boldFont );
			entry->setFont ( 3, boldFont );
			entry->setFont ( 4, boldFont );
		}

		entry->setText ( 4, QString ( "%1" ).arg ( options_get_description( oneopt ) ) );
		//todo: get default and add to a new column
		CHECK_AND_FREE ( oneopt );
	};

	CHECK_AND_FREE ( allopts );
	resize_all ();

	mutex_unlock ( refresh_mutex );
	return;
}

void options::resize_all (  )
{
	treeWidget_options->header()->setResizeMode(QHeaderView::Interactive); 

	treeWidget_options->resizeColumnToContents(0);
	treeWidget_options->resizeColumnToContents(1);
	treeWidget_options->resizeColumnToContents(2);
	treeWidget_options->resizeColumnToContents(4);

	treeWidget_options->header()->resizeSection(0,treeWidget_options->columnWidth(0) + 30);
	treeWidget_options->header()->resizeSection(1,treeWidget_options->columnWidth(1) + 10);
	treeWidget_options->header()->resizeSection(2,treeWidget_options->columnWidth(2) + 10);
	treeWidget_options->header()->resizeSection(3, 30); 
	treeWidget_options->header()->resizeSection(4,treeWidget_options->columnWidth(4) + 10);
}

char *options::option_get_part ( char * option, int part )
{
	char *tmp = NULL;
	char *tmp2 = NULL;
	int i = 0;
	int length = 0;

	if ( !option )
		return NULL;

	tmp = option;

	while ( i < part && strchr ( tmp, '.' ) )
	{
		tmp = strchr ( tmp, '.' ) + 1;
		i++;
	}

	if ( i == part )
	{
		if ( tmp2 = strchr ( tmp, '.' ) )
		{
			length = tmp2 - tmp;
			tmp2 = (char*)malloc ( length + 1 );
			strncpy ( tmp2, tmp, length );
			tmp2[length] = '\x00';
		}
		else
			tmp2 = (char*)strdup ( tmp );

		return tmp2;
	}

	return NULL;
}

void options::sel_changed(QTreeWidgetItem *item, int col)
{

	resize_all ();
	lineEdit_current_option->setText ( item->text ( 2 ) );
}

void options::test()
{
	lineEdit_current_option->setText ( "test" );
}

void options::value_edited ( const QString& text )
{
	QFont boldFont = QFont ();
	boldFont.setBold ( 1 );

	resize_all ();

	if ( !treeWidget_options->currentItem() )
		return;

	if ( treeWidget_options->currentItem()->text(1).length() == 0 )
		return;

	treeWidget_options->currentItem()->setText ( 2, text );
	treeWidget_options->currentItem()->setText ( 3, "yes" );
	treeWidget_options->currentItem()->setFont ( 0, boldFont );
	treeWidget_options->currentItem()->setFont ( 1, boldFont );
	treeWidget_options->currentItem()->setFont ( 2, boldFont );
	treeWidget_options->currentItem()->setFont ( 3, boldFont );
	treeWidget_options->currentItem()->setFont ( 4, boldFont );
}

void options::default_option()
{
	char *opt = NULL;
	QTreeWidgetItem *item = treeWidget_options->currentItem();
	QString option;

	if ( !item )
		return;

	option = item->text ( 0 );
	while ( item->parent() )
	{
		option = item->parent()->text(0) + QString ( "." ) + option;
		item = item->parent();
	}

	options_set_default ( option.toAscii().data() );

	item = treeWidget_options->currentItem();
	
	switch ( options_get_type ( option.toAscii().data() ) )
	{
		case OPT_BOOL:
			item->setText ( 1, "bool" );
			item->setText ( 2, QString ( "%1" ).arg ( options_get_bool( option.toAscii().data() ) ) );
			break;
		case OPT_INT:
			item->setText ( 1, "int" );
			item->setText ( 2, QString ( "%1" ).arg ( options_get_int( option.toAscii().data() ) ) );
			break;
		case OPT_HEX:
			item->setText ( 1, "hex" );
			item->setText ( 2, QString ( "0x%1" ).arg ( options_get_int( option.toAscii().data() ), 0, 16 ) );
			break;
		case OPT_STR:
			item->setText ( 1, "string" );
			item->setText ( 2, QString ( "%1" ).arg ( options_get_string( option.toAscii().data() ) ) );
			break;
		default:
			item->setText ( 1, "unknown" );
	}

	QFont normFont = QFont ();

	item->setText ( 3, "no" );
	item->setFont ( 0, normFont );
	item->setFont ( 1, normFont );
	item->setFont ( 2, normFont );
	item->setFont ( 3, normFont );
	item->setFont ( 4, normFont );

	sel_changed( item, 0 );
}

void options::accept_options()
{
	apply_options();
	close();
}
void options::apply_options()
{
	QTreeWidgetItem* tmp;
	QString option;

	//only update options with "changed"-indicator - still a bit overhead
	QList<QTreeWidgetItem*> items = treeWidget_options->findItems ( "yes", Qt::MatchRecursive, 3 );

#ifdef WIN32
	for each ( QTreeWidgetItem * item in items )
	{
		//lineEdit_current_option->setText ( lineEdit_current_option->text() + QString("*%1").arg(item->childCount()) );

		//additional check if that item has no childs -> then it's an option
		if ( item->childCount() )
			continue;
		//lineEdit_current_option->setText ( lineEdit_current_option->text() + QString ( "?" ) );

		//rebuild option from path
		tmp = item;
		option = tmp->text ( 0 );
		while ( tmp->parent() )
		{
			//option += QString ( "+" );
			option = tmp->parent()->text(0) + QString ( "." ) + option;
			tmp = tmp->parent();
		}

		//lineEdit_current_option->setText ( lineEdit_current_option->text() + QString("|") + option );

		//decide on option type which function to use to safe option
		if ( !strcmp ( item->text ( 1 ).toAscii().data(), "bool" ) )
		{
			if ( item->text ( 2 ).toAscii().at ( 0 ) == '0' )
				options_set_bool ( option.toAscii().data(), 0 );
			else
				options_set_bool ( option.toAscii().data(), 1 );
			continue;
		}
		if ( !strcmp ( item->text ( 1 ).toAscii().data(), "int" ) )
		{
			options_set_int ( option.toAscii().data(), item->text( 2 ).toInt() );
			continue;
		}
		if ( !strcmp ( item->text ( 1 ).toAscii().data(), "hex" ) )
		{
			options_set_int ( option.toAscii().data(), item->text ( 2 ).remove ( 0, 2 ).toInt(0, 16 ) );
			continue;
		}
		if ( !strcmp ( item->text ( 1 ).toAscii().data(), "string" ) )
		{
			options_set_string ( option.toAscii().data(), item->text ( 2 ).toAscii().data() );
			continue;
		}
	}
#endif
}

