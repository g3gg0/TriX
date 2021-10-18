
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
	unsigned int AppendLine ( unsigned char *msg );
	unsigned int AppendChar ( unsigned char *msg );
	unsigned int SetWindowName ( unsigned char *title );
	unsigned int SetWindowSize ( unsigned int width, unsigned int height );
	int LastChar;
	int *LastCharPtr;
	int GetLastKeypress ();
	void SetKeypressPtr (int *ptr);

private:



};
//---------------------------------------------------------------------------
 