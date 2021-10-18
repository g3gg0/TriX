/**
 ** @mainpage TriX Graphical User Interface
 **
 ** @author Arkadiusz [Yak] Wahlig
 **
 */
 
#ifndef UI_MAIN_H
#define UI_MAIN_H
//---------------------------------------------------------------------------

/**
 ** Define application globals and constants here...
 **
 */
#define ORG_DOMAIN "g3gg0.de"
#define APP_NAME   "TriX"
//---------------------------------------------------------------------------

/*
** Include any stock Qt files here...
**
*/
#include <Qt/qapplication.h>
#include <QtGui/QtGui>
#include <Qt/qmainwindow.h>
#include <Qt/qdebug.h>
#include <Qt/qevent.h>
//---------------------------------------------------------------------------
 
/*
** Include any application specific headers here.
**
*/
#include "ui_MainWindow.h"
//---------------------------------------------------------------------------
 
/*
** Forward-declare any required classes here.  We do not include the
**  header files for these classes here because it just increases the
**  compile times unnecessarily.  This method is considered more
**  practical - these are typical.
**
*/
class QAction;
class QAction;
class QListWidget;
class QMenu;
class QTextEdit;
//---------------------------------------------------------------------------

/**
 ** The Main Window is the main user interface screen for the
 **  application.  It is implemented as a MIDI type window.
 **
 */
class TriXMainWindow : public QMainWindow, private Ui::MainWindow
{
  Q_OBJECT
 
public:
	TriXMainWindow();
	void update_scriptlist();

public slots:

	void dragEnterEvent(QDragEnterEvent *event);
	void dropEvent(QDropEvent *event);

	void setVisible (bool);
	void evtTimerElapsed();
	void cmbSaveModeActivated(int Pos);
	void cmbFileTypeActivated(int Pos);

	void editSimpleInputEdited ( const QString &  );
	void editSimpleOutputEdited ( const QString &  );
	void editDCT3InputMCUEdited ( const QString &  );
	void editDCT3OutputMCUEdited ( const QString &  );
	void editDCT3InputPPMEdited ( const QString &  );
	void editDCT3OutputPPMEdited ( const QString &  );
	void editDCT3CustomPPMEdited ( const QString &  );
	void editDCT4InputMCUEdited ( const QString &  );
	void editDCT4OutputMCUEdited ( const QString &  );
	void editDCT4InputPPMEdited ( const QString &  );
	void editDCT4OutputPPMEdited ( const QString &  );

	void grpDCT3InputToggled(bool Checked);
	void grpDCT3OutputToggled(bool Checked);
	void grpDCT4InputToggled(bool Checked);
	void grpDCT4OutputToggled(bool Checked);
	void grpSimpleInputToggled(bool Checked);
	void grpSimpleOutputToggled(bool Checked);
	void boxDCT3CustomPPMToggled(bool Checked);
	void boxDCT3SeparatePPMToggled(bool Checked);
	void btnEditImportClicked();
	void btnEditExportClicked();
	void btnSimpleInputClicked();
	void btnSimpleOutputClicked();
	void btnDCT3InputMCUClicked();
	void btnDCT3InputPPMClicked();
	void btnDCT3OutputMCUClicked();
	void btnDCT3OutputPPMClicked();
	void btnDCT4InputMCUClicked();
	void btnDCT4InputPPMClicked();
	void btnDCT4OutputMCUClicked();
	void btnDCT4OutputPPMClicked();
	void actShowWorkspaceTriggered();
	void actNewTriggered();
	void actQuitTriggered();
	void actPluginsTriggered();
	void actStartTriggered();
	void actStopTriggered();
	void btnScriptsAddClicked();
	void btnScriptsRemoveClicked();
	void btnScriptsManualClicked();
	void btnScriptsOptionsClicked();
	void btnScriptsLoadClicked();
	void actCommandRefTriggered();
	void txtEnterPressed ();
	void actOptionsTriggered();
	void actDebugInfoTriggered();

protected:
	void timerEvent(QTimerEvent *event);
	
};
//---------------------------------------------------------------------------
 
#endif // #ifndef UI_MAIN_H
