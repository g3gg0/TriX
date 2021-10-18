
/*
** Include any stock Qt files here...
**
*/
#include <Qt/qapplication.h>
#include <QtGui/QtGui>
#include <Qt/qdialog.h>
#include <Qt/qtreewidget.h>
#include <Qt/qdebug.h>
#include <Qt/qobject.h>
//---------------------------------------------------------------------------
 
/*
** Include any application specific headers here.
**
*/
#include "ui_TextBox.h"

extern "C"
{
#include "defines.h"

};

//---------------------------------------------------------------------------
class TextBox : public QDialog, private Ui::dlgTextBox
{
	Q_OBJECT

public:
	TextBox(  );
	unsigned int Clear ();
	unsigned int Append ( unsigned char *msg );
	unsigned int SetWindowName ( unsigned char *title );
	unsigned int SetWindowSize ( unsigned int width, unsigned int height );


private:



};
//---------------------------------------------------------------------------
 