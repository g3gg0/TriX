
//#define USE_THREAD_FOR_TEXTBOX


#include "ui_text_edit.h"

#ifdef WIN32
#include <Windows.h>
#endif
extern "C"
{
  
#include "defines.h"
#include "treenode.h"

#include "mem.h"

};

#include "ui_TextEdit.h"


int fill_table ( t_treenode *node, QTableWidget *tableWidget )
{
	//
	unsigned int current_row = 0;
	unsigned int rows = 0;
	unsigned int columns = 0;
	t_treenode *infonode = NULL;
	t_treenode *stringnode = NULL;
	t_treenode *chunknode = NULL;
	t_treenode *worknode = NULL;
	QStringList hheader;
	int comm_entries = 0;


	infonode = treenode_get_byname ( treenode_children ( node ), (unsigned char*)"INFORMATION" );
	infonode = treenode_get_byname ( treenode_children ( infonode ), (unsigned char*)"NAME" );
	if ( !infonode || strcmp ( (const char *) treenode_get_content_ascii ( infonode ), "TEXT" ) )
		return 0;

	chunknode = treenode_get_byname ( treenode_children ( node ), (unsigned char*)"SUBCHUNK" );
	while ( chunknode )
	{
		infonode = treenode_get_byname ( treenode_children ( chunknode ), (unsigned char*)"INFORMATION" );
		infonode = treenode_get_byname ( treenode_children ( infonode ), (unsigned char*)"NAME" );
		if ( !infonode )
			return 0;

		if ( strlen ( (const char*)treenode_get_content_ascii ( infonode ) ) )
		{
			current_row = comm_entries;

			hheader << QString::fromAscii ( (const char*)treenode_get_content_ascii ( infonode ));

			columns++;
			tableWidget->setColumnCount ( columns );

			stringnode = treenode_get_byname ( treenode_children ( chunknode ), (unsigned char*)"STRINGS" );
			stringnode = treenode_get_byname ( treenode_children ( stringnode ), (unsigned char*)"STRING" );
			while ( stringnode )
			{
				worknode = treenode_get_byname ( treenode_children ( stringnode ), (unsigned char*)"UTF8" );
				current_row++;
				if ( current_row > rows )
				{
					rows = current_row;
					tableWidget->setRowCount ( rows );
				}

				QTableWidgetItem *current_item = new QTableWidgetItem;
				if ( current_item )
				{
					current_item->setText ( QString::fromUtf8 ( (const char*)treenode_get_content_ascii ( worknode ) ) );
					tableWidget->setItem ( current_row-1, columns-1, current_item );
				}
			
				stringnode = treenode_next ( stringnode );
				
			}
		}

		if ( !comm_entries )
			comm_entries = current_row;

		chunknode = treenode_next ( chunknode );
	}

	tableWidget->setHorizontalHeaderLabels( hheader );
	//

	return comm_entries;
}


#ifdef USE_THREAD_FOR_TEXTBOX
//---------------------------------------------------------------------------
PPModd_TextEdit_Thread::PPModd_TextEdit_Thread ( t_treenode *treenode, QTableWidget *widget )
{
	thread_abort = 0;
	thread_active = 0;
	thread_finished = 0;
	comm_entries = 0;

	node = treenode;
	tableWidget = widget;
}

int PPModd_TextEdit_Thread::is_active ()
{
	return thread_active;
}
int PPModd_TextEdit_Thread::is_finished ()
{
	return thread_finished;
}

void PPModd_TextEdit_Thread::abort()
{
	if ( is_active() && !is_finished() )
		thread_abort = 1;
}
int PPModd_TextEdit_Thread::get_comm_entries()
{
	return comm_entries;
}

void PPModd_TextEdit_Thread::reset()
{
	if ( is_active() && !is_finished() )
	{
		return;
	}

	thread_active = 0;
	thread_finished = 0;
}

void PPModd_TextEdit_Thread::run()
{
	int pos = 0;
	int file_pos = 0;

	if ( is_active() )
	{
		return;
	}

	thread_active = 1;

	comm_entries = fill_table ( node, tableWidget );

	thread_active = 0;
	thread_finished = 1;
}

#endif

//---------------------------------------------------------------------------

/**
 ** The constructor initializes the environment for the first application.
 **
 */
TextEdit::TextEdit( t_treenode *node )
{

	/*
	** Build the ui component
	**
	*/
	setupUi(this);

	connect ( okButton, SIGNAL ( clicked() ),this, SLOT( accept() ) );
	
	sourcenode = node;
#ifdef USE_THREAD_FOR_TEXTBOX
	tableWidget->setUpdatesEnabled(true);
	thread = new PPModd_TextEdit_Thread ( node, tableWidget );
	thread->start();
	

	// wanted the list to live-update, but it doesnt update when displayed :(
	while ( !thread->is_finished() && thread->isRunning () )
	{
		Sleep ( 100 );
		tableWidget->repaint();
		QCoreApplication::processEvents ( );
	}

	
#else
	comm_entries = fill_table ( node, tableWidget );
#endif


	return;
}
//---------------------------------------------------------------------------

void TextEdit::accept (  )
{
	unsigned int current_row = 0;
	unsigned int rows = 0;
	unsigned int columns = 0;
	unsigned int current_comm_entries = 0;

	t_treenode *infonode = NULL;
	t_treenode *stringnode = NULL;
	t_treenode *chunknode = NULL;
	t_treenode *worknode = NULL;

#ifdef USE_THREAD_FOR_TEXTBOX
	if ( !thread->is_finished() )
		return;

	comm_entries = thread->get_comm_entries();
#endif


	infonode = treenode_get_byname ( treenode_children ( sourcenode ), (unsigned char*)"INFORMATION" );
	infonode = treenode_get_byname ( treenode_children ( infonode ), (unsigned char*)"NAME" );
	if ( !infonode || strcmp ( (const char *) treenode_get_content_ascii ( infonode ), "TEXT" ) )
		return;

	chunknode = treenode_get_byname ( treenode_children ( sourcenode ), (unsigned char*)"SUBCHUNK" );
	while ( chunknode )
	{
		infonode = treenode_get_byname ( treenode_children ( chunknode ), (unsigned char*)"INFORMATION" );
		infonode = treenode_get_byname ( treenode_children ( infonode ), (unsigned char*)"NAME" );
		if ( !infonode )
			return;

		if ( strlen ( (const char*)treenode_get_content_ascii ( infonode ) ) )
		{
			current_row = current_comm_entries;

			columns++;
			stringnode = treenode_get_byname ( treenode_children ( chunknode ), (unsigned char*)"STRINGS" );
			stringnode = treenode_get_byname ( treenode_children ( stringnode ), (unsigned char*)"STRING" );
			while ( stringnode )
			{
				worknode = treenode_get_byname ( treenode_children ( stringnode ), (unsigned char*)"UTF8" );
				current_row++;

				QTableWidgetItem *current_item = tableWidget->item ( current_row-1, columns-1 );
				if ( current_item )
				{
					// just update source data if something was changed
					if ( strcmp ( (const char*)current_item->text().toUtf8().data(), (const char*)treenode_get_content_ascii ( worknode ) ) )
					{
						treenode_set_content_data ( worknode, (unsigned char*)current_item->text().toUtf8().data() );

						// also delete UTF16 and TOKEN if existing
						treenode_release ( treenode_get_byname ( treenode_children ( stringnode ), (unsigned char*)"TOKEN" ) );
						treenode_release ( treenode_get_byname ( treenode_children ( stringnode ), (unsigned char*)"UTF16" ) );
					}
				}
			
				stringnode = treenode_next ( stringnode );
			}
		}

		if ( !current_comm_entries )
			current_comm_entries = comm_entries;

		chunknode = treenode_next ( chunknode );
	}



	close ();
	return;
}
