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
#define APP_NAME   "PPModd"
//---------------------------------------------------------------------------

/*
** Include any stock Qt files here...
**
*/
#include <QtGui/QApplication>
#include <QtGui/QtGui>
#include <QtGui/QMainWindow>
#include <QtCore/QDebug>
#include <QtCore/QEvent>
//---------------------------------------------------------------------------
 
/*
** Include any application specific headers here.
**
*/
#include "ui_PPModd.h"
//---------------------------------------------------------------------------
extern "C"
{
#include "defines.h"
#include "defines.h"                                                                                            
#include "segment.h"                                                                                            
#include "stage.h"                                                                                              
#include "symbols.h"                                                                                            
                                                                                                                
#include "file_io.h"                                                                                            
#include "file.h"                                                                                               
#include "fmt.h"                                                                                                
#include "workspace.h"           
#include "treenode.h"

}; 
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
class PPModdMainWindow : public QMainWindow, private Ui::MainWindow
{
  Q_OBJECT
 
public:
	PPModdMainWindow();
	unsigned int SetData ( t_treenode *data );
	unsigned int SetDataSubcall ( t_treenode *treenode, QTreeWidgetItem *tree );
	unsigned int UpdatePPMData ( t_treenode *treenode );
	unsigned int LoadFile ( char *filename );
	unsigned int SaveAsXML ( t_treenode *node );
	t_treenode *LoadFromXML ();
	unsigned int SaveAsBIN ( t_treenode *node );
	unsigned int LoadFromBIN ( t_treenode *node );

private slots:
	void treeItemClicked ( QTreeWidgetItem * item, int column );
	void treeCurrentItemChanged ( QTreeWidgetItem * current, QTreeWidgetItem * previous );
	//void treeItemChanged (  );
	void treeItemContext(const QPoint &pos);

public slots:

   void closeEvent ( QCloseEvent *event );

	void dragEnterEvent(QDragEnterEvent *event);
	void dropEvent(QDropEvent *event);
/*
	void setVisible (bool);
*/
	void evtTimerElapsed();
	void txtEnterPressed ();
	void actOpenTriggered();
	void actQuitTriggered();
	void actOptionsTriggered();
	void actWorkspaceTriggered();
	void actPluginsTriggered();
	void actSaveTriggered();
	void actSaveAsTriggered();
	void actCloseTriggered();
	void btnEditClicked();
	void actImportTriggered();
	void actExportTriggered();
	void actBuildPPMTriggered();
	
public:
    //                                                                                                              
    // PPModify                                                                                                     
    //                                                                                                              
    int (*ppmodify_dump) ( t_workspace *ws, int start );                                                            
    t_treenode * (*ppmodify_dump_xml) ( t_workspace *ws, int start );                                               
    int (*ppmodify_build_ppm) ( t_workspace *ws, int start, int length, t_treenode *node );                         
    int (*ppmodify_merge_ppm) ( t_treenode *node );                                                                 
                                                                                                                
    //                                                                                                              
    // XML                                                                                                          
    //                                                                                                              
    t_treenode *(*xml_connector_parse) ( unsigned char *xml_data );                                                 
    t_treenode *(*xml_connector_parse_file) ( unsigned char *filename );                                            
    unsigned char *(*xml_connector_create) ( void *treenode, char *stylesheet );                                    
    int (*xml_connector_create_file) ( void *node, char *filename, char *stylesheet );                              
    //////////////////////////////////////////////////////////////////////////////////////////                      
 
};
//---------------------------------------------------------------------------
 
#endif // #ifndef UI_MAIN_H
