

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
	LastCharPtr = NULL;

	textEdit->setReadOnly(true);

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

int TextBox::GetLastKeypress ()
{
	return LastChar;
}

void TextBox::SetKeypressPtr (int *ptr)
{
	LastCharPtr = ptr;
}

unsigned int TextBox::AppendLine ( unsigned char *msg )
{
	textEdit->append ( QString ((const char*)msg) );
	return E_OK;
}

void QTextEdit::keyReleaseEvent(QKeyEvent *e)
{
	TextBox* box = dynamic_cast<TextBox*>(this->parentWidget());
	if(box != NULL) 
	{
		QString cmd = e->text().toAscii();
		if(cmd.length() > 0)
		{
			box->LastChar = cmd.at(0).toAscii();
			if(box->LastCharPtr)
			{
				*(box->LastCharPtr) = box->LastChar;
			}
		}
	}
	else
	{
	}
}


unsigned int TextBox::AppendChar ( unsigned char *msg )
{
	QTextCursor cursor(textEdit->textCursor());

	cursor.movePosition(QTextCursor::End);
	textEdit->setTextCursor(cursor);
	textEdit->ensureCursorVisible();

	QString str = QString ((const char*)msg);

	for(int pos = 0; pos < str.length(); pos++)
	{
		unsigned char c = str.at(pos).toAscii();
		if(c == 0x07 || c == 0x0D || c == 0x0A || (c >= 0x20 && c <= 0x7F))
		{
			textEdit->insertPlainText (QString (c));
		}
		else if(c == 8)
		{
			textEdit->textCursor().deletePreviousChar();
		}
	}

	QScrollBar *sb = textEdit->verticalScrollBar();
	sb->setValue(sb->maximum());

	cursor.movePosition(QTextCursor::End);
	textEdit->setTextCursor(cursor);
	textEdit->ensureCursorVisible();
	return E_OK;
}



