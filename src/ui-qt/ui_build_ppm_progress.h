
/*
** Include any stock Qt files here...
**
*/
#include <QtGui/QApplication>
#include <QtGui/QDialog>
#include <QtGui/QTreeWidget>
#include <QtCore/QDebug>
#include <QtCore/QObject>
#include <QtCore/QThread>
#include <QtCore/QTimer>
//---------------------------------------------------------------------------
 
/*
** Include any application specific headers here.
**
*/
#include "ui_BuildPPMProgress.h"

extern "C"
{
#include "defines.h"
#include "treenode.h"

};

class QTreeWidget;

class PPModd_Build_Thread : public QThread 
{
public:
	PPModd_Build_Thread ( t_treenode *node, unsigned char *username, unsigned char *password );
    virtual int is_active ();
    virtual int is_finished ();
    virtual void run ();
    virtual void reset ();
    virtual void abort ();
	unsigned char* TriX_Site_Login ( unsigned char *username, unsigned char *password );
	const char *process_msg ();
	unsigned int process_pct ();
	t_treenode *get_node ();

private:
	int thread_active;
	int thread_finished;
	int thread_abort;
	int encrypted;
	unsigned char login_id[33];
	t_treenode *treenode;
	t_treenode *result_treenode;
	unsigned char *user;
	unsigned char *pass;

	unsigned int process_percent;
	const char *process_message;
	unsigned char server_message[1024];

public:
	unsigned char *(*xml_connector_create) ( void *treenode, char *stylesheet );                                    
	t_treenode *(*xml_connector_parse) ( unsigned char *xml_data );                                                 
 
};

//---------------------------------------------------------------------------
class BuildPPMProgress : public QDialog, private Ui::BuildDialog
{
	Q_OBJECT

public:
	BuildPPMProgress( t_treenode *node, unsigned char *username, unsigned char *password  );
	int UploadPPM ( );
	t_treenode *get_node ();

public slots:
	void update ();
	void cancel ();

private:
	unsigned char *username;
	unsigned char *password;
	PPModd_Build_Thread *this_thread;
	t_treenode *finished_node;


};
//---------------------------------------------------------------------------
 
