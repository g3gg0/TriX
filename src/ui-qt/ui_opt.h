
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

#include "ui_options.h"

//forward declaration - so class needn't be included (compiles faster)
class QTreeWidget;

class options : public QDialog, private Ui::dlgOptions
{
	Q_OBJECT

public:
	options( bool modal );
	~options();

public:
	char *option_get_part ( char * option, int part );
	void refresh_content ();
	void update_timer();
	int needs_update;

private slots:
	void sel_changed(QTreeWidgetItem *item, int col);
	void item_dbl_clicked(QTreeWidgetItem *item, int col);
	void value_edited ( const QString& text );
	void test();
	void default_option();
	void accept_options();
	void apply_options();
	void resize_all ();

};

 