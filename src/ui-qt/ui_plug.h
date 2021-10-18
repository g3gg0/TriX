
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
#include "ui_Plugins.h"


class QTreeWidget;

//---------------------------------------------------------------------------
class PluginWindow : public QDialog, private Ui::dlgPlugins
{
	Q_OBJECT

public:
	PluginWindow();
	void refresh_content();
	bool unload_allowed;
	void unloading ( bool state );
	void update_timer();
	int needs_update;

private slots:
	void treeWidgetChanged ( );
	void btnUnloadClicked();

};
//---------------------------------------------------------------------------
 