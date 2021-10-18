/// ui_main.cpp
 
#include "ui_memviewer_dlg.h"

#ifdef WIN32
#include <Windows.h>
#endif
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

#include "ui_MemoryViewer.h"


//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
unsigned int default_start_callback ( void *priv )
{
	t_workspace *ws = (t_workspace *)priv;

	if ( !ws || !mem_is_valid ( ws ) )
		return E_FAIL;

	return v_get_start ( ws );
}
//---------------------------------------------------------------------------
unsigned int default_end_callback ( void *priv )
{
	t_workspace *ws = (t_workspace *)priv;

	if ( !ws || !mem_is_valid ( ws ) )
		return E_FAIL;

	return v_get_end ( ws );
}
//---------------------------------------------------------------------------
unsigned int default_valid_callback ( void *priv, unsigned int address )
{
	t_workspace *ws = (t_workspace *)priv;

	if ( !ws || !mem_is_valid ( ws ) )
		return E_FAIL;

	return v_valid ( ws, address );
}
//---------------------------------------------------------------------------
unsigned int default_read_callback ( void *priv, unsigned int address, unsigned int bytes, unsigned char *buffer )
{
	t_workspace *ws = (t_workspace *)priv;

	if ( !ws || !mem_is_valid ( ws ) )
		return E_FAIL;

	v_memcpy_get ( ws, buffer, address, bytes );

	return E_OK;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

/**
 ** The constructor initializes the environment for the first application.
 **
 */
MemoryViewer::MemoryViewer ( )
{
	
	/*
	** Build the ui component
	**
	*/
	setupUi(this);


	this->current_addr = E_FAIL;
	this->request_addr = E_FAIL;
	this->disp_width = 0x10;
	this->disp_bytes = 0x200;

	this->callbacks = &default_cb;
	this->callbacks->start_callback = default_start_callback;
	this->callbacks->end_callback = default_end_callback;
	this->callbacks->valid_callback = default_valid_callback;
	this->callbacks->read_callback = default_read_callback;
	this->callbacks->priv = NULL;

	connect ( okButton, SIGNAL ( clicked() ),this, SLOT( btnOkPressed() ) );
	connect ( btnGoto, SIGNAL ( clicked() ),this, SLOT( btnGotoPressed() ) );


	this->active = 0;

	this->update_timer = new QTimer ( this );
	connect(update_timer, SIGNAL(timeout()), this, SLOT(update()));
	this->update_timer->start ( 1000 );
	this->timer_delay = 1000;

	this->last_addr = E_FAIL;

	return;
}

//---------------------------------------------------------------------------
void MemoryViewer::ResetData (  )
{
	this->current_addr = E_FAIL;
}

unsigned int MemoryViewer::Stop ()
{
	this->active = 0;

	return E_OK;
}

unsigned int MemoryViewer::Start ()
{
	if ( this->active )
		return E_OK;

	this->active = 1;

	return E_OK;
}

unsigned int MemoryViewer::SetAddress ( unsigned int address )
{
	// update everything with given address as start

	this->current_addr = address;
	if ( this->current_addr == E_FAIL )
		return E_FAIL;

	this->start_addr = this->callbacks->start_callback ( this->callbacks->priv );
	this->end_addr = this->callbacks->end_callback ( this->callbacks->priv );


	if ( this->start_addr + this->disp_bytes > this->end_addr )
		verticalScrollBar->setMaximum ( this->start_addr / disp_width );
	else
		verticalScrollBar->setMaximum ( (this->end_addr - this->disp_bytes) / disp_width + 2 );

	verticalScrollBar->setMinimum ( this->start_addr / disp_width );
	verticalScrollBar->setPageStep ( this->disp_bytes / disp_width );
	verticalScrollBar->setValue ( this->start_addr / disp_width );

	if ( this->callbacks->valid_callback ( this->callbacks->priv, this->current_addr ) == E_FAIL )
		return E_FAIL;

	verticalScrollBar->setValue ( this->current_addr / this->disp_width );

	return E_OK;
}

unsigned int MemoryViewer::GetAddress ( )
{
	if ( this->request_addr != E_FAIL )
	{
		int addr = this->request_addr;

		this->request_addr = E_FAIL;
		return addr;
	}

	return this->current_addr;
}

void MemoryViewer::btnOkPressed ()
{
	this->Stop ();
	this->accept ();
}

void MemoryViewer::btnGotoPressed ()
{
	if ( this->current_addr != getReadAddress () )
	{
		this->current_addr = getReadAddress ();
		this->request_addr = this->current_addr;

		if ( this->callbacks->valid_callback ( this->callbacks->priv, this->current_addr ) == E_FAIL )
			return;
		verticalScrollBar->setValue ( this->current_addr / this->disp_width );
	}
}

unsigned int MemoryViewer::getReadAddress ()
{
	unsigned int value = 0;
	int err = 0;

	value = util_hex_get ( txtAddress->text().toAscii().data(), &err );

	if ( err )
		return E_FAIL;

	return value;
}

void MemoryViewer::update ()
{
	unsigned char tmp_buffer[0x100];
	unsigned char data[0x2000];
	unsigned int address = 0;

	if ( !this->active )
	{
		txtBox->setHtml ( "" );
		this->update_timer->stop ();
		this->update_timer->start ( 1000 );
		this->timer_delay = 1000;
		this->last_addr = E_FAIL;

		return;
	}
	if ( this->timer_delay != 50 )
	{
		this->update_timer->stop ();
		this->update_timer->start ( 50 );
		this->timer_delay = 50;
	}

	if ( !this->callbacks || !this->callbacks->read_callback )
	{
		QString text ( "no memory callbacks defined" );
		txtBox->setPlainText ( text );
	}

	if ( current_addr == E_FAIL )
	{
		this->current_addr = this->callbacks->start_callback ( this->callbacks->priv ) & ~3;
		if ( this->current_addr == E_FAIL )
			return;

		sprintf ( (char*)tmp_buffer, "%08X", this->current_addr );
		txtAddress->setText ( QString::fromAscii ( (const char*)tmp_buffer ) );

		this->start_addr = this->callbacks->start_callback ( this->callbacks->priv );
		this->end_addr = this->callbacks->end_callback ( this->callbacks->priv );


		if ( this->start_addr + this->disp_bytes > this->end_addr )
			verticalScrollBar->setMaximum ( this->start_addr / disp_width );
		else
			verticalScrollBar->setMaximum ( (this->end_addr - this->disp_bytes) / disp_width + 2 );

		verticalScrollBar->setMinimum ( this->start_addr / disp_width );
		verticalScrollBar->setPageStep ( this->disp_bytes / disp_width );
		verticalScrollBar->setValue ( this->start_addr / disp_width );

	}
	address = this->current_addr;
	QString text ( "" );

	if ( (address != verticalScrollBar->value() * this->disp_width) || (address != this->last_addr) )
	{
		address = verticalScrollBar->value() * this->disp_width;
		this->current_addr = address;
		sprintf ( (char*)tmp_buffer, "%08X", address );
		txtAddress->setText ( QString::fromAscii ( (const char*)tmp_buffer ) );
	}


	int pos = 0;
	int col = 0;


	this->callbacks->read_callback ( this->callbacks->priv, address, this->disp_bytes, data );

	// if no change, skip update
	if ( address == last_addr && !memcmp ( old_data, data, this->disp_bytes ) )
		return;

	while ( pos < this->disp_bytes )
	{
		sprintf ( (char*)tmp_buffer, "<FONT COLOR=#0000B0>%08X:</FONT> ", address+pos );
		text += QString::fromAscii ( (const char*)tmp_buffer );
		col = 0;
		while ( col < this->disp_width  )
		{
			if ( col+pos < this->disp_bytes && ( this->callbacks->valid_callback ( this->callbacks->priv, address + col + pos )  ) )
				sprintf ( (char*)tmp_buffer, "%02X ", data[pos+col] );
			else
				sprintf ( (char*)tmp_buffer, "<FONT COLOR=#0000B0>-- </FONT>" );
			text += QString::fromAscii ( (const char*)tmp_buffer );
			col++;
		}

		sprintf ( (char*)tmp_buffer, "<FONT COLOR=#B00000>" );
		text += QString::fromAscii ( (const char*)tmp_buffer );
		col = 0;
		while ( col < this->disp_width )
		{
			if ( col+pos < this->disp_bytes && ( this->callbacks->valid_callback ( this->callbacks->priv, address + col + pos ) ) )
			{
				if ( data[pos+col] < 0x7F && data[pos+col] >= 0x20 )
				{
					switch (data[pos+col])
					{
						case '>':
							sprintf ( (char*)tmp_buffer, "&gt;" );
							break;
						case '<':
							sprintf ( (char*)tmp_buffer, "&lt;" );
							break;
						case '"':
							sprintf ( (char*)tmp_buffer, "&quot;" );
							break;
						case '&':
							sprintf ( (char*)tmp_buffer, "&amp;" );
							break;
						case ' ':
							sprintf ( (char*)tmp_buffer, "&nbsp;" );
							break;
						case '\'':
							sprintf ( (char*)tmp_buffer, "&apos;" );
							break;
						default:
							sprintf ( (char*)tmp_buffer, "%c", data[pos+col] );
							break;
					}
				}
				else
					sprintf ( (char*)tmp_buffer, "." );
			}
			else
				sprintf ( (char*)tmp_buffer, "&nbsp;" );
			text += QString::fromAscii ( (const char*)tmp_buffer );

			col++;
		}
		sprintf ( (char*)tmp_buffer, "</FONT>" );
		text += QString::fromAscii ( (const char*)tmp_buffer );
		sprintf ( (char*)tmp_buffer, "<BR>\n" );
		text += QString::fromAscii ( (const char*)tmp_buffer );
		pos += this->disp_width;
	}

	txtBox->setHtml ( text );

	this->last_addr = address;
	memcpy ( old_data, data, this->disp_bytes );
	
	return;
}

unsigned int MemoryViewer::SetCallbackFuncs ( t_callbacks *cb )
{
	this->callbacks = cb;

	return E_OK;
}
unsigned int MemoryViewer::SetCallbackPriv ( void *priv )
{
	this->callbacks->priv = priv;

	return E_OK;
}