

#include "ui_text_box.h"

#ifdef WIN32
#include <Windows.h>
#endif
extern "C"
{
  
#include "defines.h"
#include "mem.h"

};

#include "ui_TextBox.h"

//---------------------------------------------------------------------------

/**
 ** The constructor initializes the environment for the first application.
 **
 */
TextBox::TextBox(  )
{

	/*
	** Build the ui component
	**
	*/
	setupUi(this);

	return;
}

unsigned int TextBox::SetWindowName ( unsigned char *title )
{
	this->setWindowTitle ( QString ((const char*)title) );
	return E_OK;
}

unsigned int TextBox::SetWindowSize ( unsigned int width, unsigned int height )
{
	this->resize(QSize(width, height).expandedTo(this->minimumSizeHint()));	
	return E_OK;
}

unsigned int TextBox::Clear ()
{
	textEdit->clear ();
	return E_OK;
}

unsigned int TextBox::Append ( unsigned char *msg )
{
	textEdit->append ( QString ((const char*)msg) );
	return E_OK;
}



