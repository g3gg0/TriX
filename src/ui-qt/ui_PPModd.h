/********************************************************************************
** Form generated from reading ui file 'PPModd.ui'
**
** Created: Wed 21. Aug 22:19:50 2013
**      by: Qt User Interface Compiler version 4.5.1
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_PPMODD_H
#define UI_PPMODD_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDockWidget>
#include <QtGui/QFrame>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QStatusBar>
#include <QtGui/QTextEdit>
#include <QtGui/QToolBar>
#include <QtGui/QTreeWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionOpen;
    QAction *actionQuit;
    QAction *actionOptions;
    QAction *actionWorkspace;
    QAction *actionPlugins;
    QAction *actionSave;
    QAction *actionSave_as;
    QAction *actionClose;
    QAction *actionExport_XML;
    QAction *actionImport_XML;
    QAction *actionCreate_PPM;
    QAction *actionStatus;
    QAction *actionInformation;
    QAction *actionLogfile;
    QWidget *centralwidget;
    QGridLayout *gridLayout;
    QVBoxLayout *vboxLayout;
    QHBoxLayout *hboxLayout;
    QDockWidget *dockWidgetTree;
    QWidget *dockWidgetContents_2;
    QHBoxLayout *hboxLayout1;
    QTreeWidget *treeWidget;
    QFrame *line;
    QDockWidget *dockWidget;
    QWidget *dockWidgetContents;
    QGridLayout *gridLayout1;
    QHBoxLayout *hboxLayout2;
    QSpacerItem *spacerItem;
    QPushButton *btnEdit;
    QTextEdit *textEdit;
    QFrame *line_2;
    QTextEdit *textOutput;
    QMenuBar *menubar;
    QMenu *menuAccount;
    QMenu *menuOptions;
    QMenu *menuFile;
    QStatusBar *statusbar;
    QToolBar *toolBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(620, 520);
        MainWindow->setAcceptDrops(true);
        const QIcon icon = QIcon(QString::fromUtf8(":/icons/icons/22x22-gartoon/configuration.png"));
        MainWindow->setWindowIcon(icon);
        actionOpen = new QAction(MainWindow);
        actionOpen->setObjectName(QString::fromUtf8("actionOpen"));
        const QIcon icon1 = QIcon(QString::fromUtf8(":/icons/icons/22x22-gartoon/folder-open.png"));
        actionOpen->setIcon(icon1);
        actionQuit = new QAction(MainWindow);
        actionQuit->setObjectName(QString::fromUtf8("actionQuit"));
        const QIcon icon2 = QIcon(QString::fromUtf8(":/icons/icons/22x22-gartoon/quit.png"));
        actionQuit->setIcon(icon2);
        actionOptions = new QAction(MainWindow);
        actionOptions->setObjectName(QString::fromUtf8("actionOptions"));
        actionOptions->setIcon(icon);
        actionWorkspace = new QAction(MainWindow);
        actionWorkspace->setObjectName(QString::fromUtf8("actionWorkspace"));
        const QIcon icon3 = QIcon(QString::fromUtf8(":/icons/icons/22x22-gartoon/information.png"));
        actionWorkspace->setIcon(icon3);
        actionPlugins = new QAction(MainWindow);
        actionPlugins->setObjectName(QString::fromUtf8("actionPlugins"));
        const QIcon icon4 = QIcon(QString::fromUtf8(":/icons/icons/22x22-gartoon/viewer.png"));
        actionPlugins->setIcon(icon4);
        actionSave = new QAction(MainWindow);
        actionSave->setObjectName(QString::fromUtf8("actionSave"));
        actionSave->setEnabled(false);
        const QIcon icon5 = QIcon(QString::fromUtf8(":/icons/icons/22x22-gartoon/disk-floppy.png"));
        actionSave->setIcon(icon5);
        actionSave_as = new QAction(MainWindow);
        actionSave_as->setObjectName(QString::fromUtf8("actionSave_as"));
        actionSave_as->setEnabled(false);
        const QIcon icon6 = QIcon(QString::fromUtf8(":/icons/icons/22x22-gartoon/disk.png"));
        actionSave_as->setIcon(icon6);
        actionClose = new QAction(MainWindow);
        actionClose->setObjectName(QString::fromUtf8("actionClose"));
        actionClose->setEnabled(false);
        const QIcon icon7 = QIcon(QString::fromUtf8(":/icons/icons/22x22-gartoon/choice-no.png"));
        actionClose->setIcon(icon7);
        actionExport_XML = new QAction(MainWindow);
        actionExport_XML->setObjectName(QString::fromUtf8("actionExport_XML"));
        actionExport_XML->setEnabled(false);
        const QIcon icon8 = QIcon(QString::fromUtf8(":/icons/icons/22x22-gartoon/www.png"));
        actionExport_XML->setIcon(icon8);
        actionImport_XML = new QAction(MainWindow);
        actionImport_XML->setObjectName(QString::fromUtf8("actionImport_XML"));
        actionImport_XML->setEnabled(false);
        const QIcon icon9 = QIcon(QString::fromUtf8(":/icons/icons/22x22-gartoon/editor.png"));
        actionImport_XML->setIcon(icon9);
        actionCreate_PPM = new QAction(MainWindow);
        actionCreate_PPM->setObjectName(QString::fromUtf8("actionCreate_PPM"));
        const QIcon icon10 = QIcon(QString::fromUtf8(":/icons/icons/22x22-gartoon/science.png"));
        actionCreate_PPM->setIcon(icon10);
        actionStatus = new QAction(MainWindow);
        actionStatus->setObjectName(QString::fromUtf8("actionStatus"));
        actionStatus->setEnabled(false);
        actionStatus->setIcon(icon8);
        actionInformation = new QAction(MainWindow);
        actionInformation->setObjectName(QString::fromUtf8("actionInformation"));
        actionInformation->setEnabled(false);
        actionLogfile = new QAction(MainWindow);
        actionLogfile->setObjectName(QString::fromUtf8("actionLogfile"));
        actionLogfile->setEnabled(false);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        gridLayout = new QGridLayout(centralwidget);
        gridLayout->setSpacing(6);
        gridLayout->setMargin(9);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        vboxLayout = new QVBoxLayout();
        vboxLayout->setSpacing(6);
        vboxLayout->setMargin(0);
        vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
        hboxLayout = new QHBoxLayout();
        hboxLayout->setSpacing(6);
        hboxLayout->setMargin(0);
        hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
        dockWidgetTree = new QDockWidget(centralwidget);
        dockWidgetTree->setObjectName(QString::fromUtf8("dockWidgetTree"));
        dockWidgetTree->setFeatures(QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetMovable|QDockWidget::NoDockWidgetFeatures);
        dockWidgetContents_2 = new QWidget();
        dockWidgetContents_2->setObjectName(QString::fromUtf8("dockWidgetContents_2"));
        hboxLayout1 = new QHBoxLayout(dockWidgetContents_2);
        hboxLayout1->setSpacing(6);
        hboxLayout1->setMargin(9);
        hboxLayout1->setObjectName(QString::fromUtf8("hboxLayout1"));
        treeWidget = new QTreeWidget(dockWidgetContents_2);
        treeWidget->setObjectName(QString::fromUtf8("treeWidget"));
        treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
        treeWidget->setSelectionBehavior(QAbstractItemView::SelectItems);

        hboxLayout1->addWidget(treeWidget);

        dockWidgetTree->setWidget(dockWidgetContents_2);

        hboxLayout->addWidget(dockWidgetTree);

        line = new QFrame(centralwidget);
        line->setObjectName(QString::fromUtf8("line"));
        line->setFrameShape(QFrame::VLine);
        line->setFrameShadow(QFrame::Sunken);

        hboxLayout->addWidget(line);

        dockWidget = new QDockWidget(centralwidget);
        dockWidget->setObjectName(QString::fromUtf8("dockWidget"));
        dockWidget->setFeatures(QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetMovable|QDockWidget::NoDockWidgetFeatures);
        dockWidgetContents = new QWidget();
        dockWidgetContents->setObjectName(QString::fromUtf8("dockWidgetContents"));
        gridLayout1 = new QGridLayout(dockWidgetContents);
        gridLayout1->setSpacing(6);
        gridLayout1->setMargin(9);
        gridLayout1->setObjectName(QString::fromUtf8("gridLayout1"));
        hboxLayout2 = new QHBoxLayout();
        hboxLayout2->setSpacing(6);
        hboxLayout2->setMargin(0);
        hboxLayout2->setObjectName(QString::fromUtf8("hboxLayout2"));
        spacerItem = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hboxLayout2->addItem(spacerItem);

        btnEdit = new QPushButton(dockWidgetContents);
        btnEdit->setObjectName(QString::fromUtf8("btnEdit"));
        btnEdit->setEnabled(false);

        hboxLayout2->addWidget(btnEdit);


        gridLayout1->addLayout(hboxLayout2, 1, 0, 1, 1);

        textEdit = new QTextEdit(dockWidgetContents);
        textEdit->setObjectName(QString::fromUtf8("textEdit"));
        textEdit->setEnabled(false);
        textEdit->setReadOnly(true);

        gridLayout1->addWidget(textEdit, 0, 0, 1, 1);

        dockWidget->setWidget(dockWidgetContents);

        hboxLayout->addWidget(dockWidget);


        vboxLayout->addLayout(hboxLayout);

        line_2 = new QFrame(centralwidget);
        line_2->setObjectName(QString::fromUtf8("line_2"));
        line_2->setFrameShape(QFrame::HLine);
        line_2->setFrameShadow(QFrame::Sunken);

        vboxLayout->addWidget(line_2);

        textOutput = new QTextEdit(centralwidget);
        textOutput->setObjectName(QString::fromUtf8("textOutput"));
        QSizePolicy sizePolicy(static_cast<QSizePolicy::Policy>(7), static_cast<QSizePolicy::Policy>(0));
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(textOutput->sizePolicy().hasHeightForWidth());
        textOutput->setSizePolicy(sizePolicy);
        textOutput->setMaximumSize(QSize(16777215, 100));
        QFont font;
        font.setFamily(QString::fromUtf8("Courier"));
        font.setPointSize(8);
        font.setBold(false);
        font.setItalic(false);
        font.setUnderline(false);
        font.setWeight(50);
        font.setStrikeOut(false);
        textOutput->setFont(font);

        vboxLayout->addWidget(textOutput);


        gridLayout->addLayout(vboxLayout, 0, 0, 1, 1);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 717, 24));
        menubar->setContextMenuPolicy(Qt::NoContextMenu);
        menuAccount = new QMenu(menubar);
        menuAccount->setObjectName(QString::fromUtf8("menuAccount"));
        menuAccount->setEnabled(true);
        menuOptions = new QMenu(menubar);
        menuOptions->setObjectName(QString::fromUtf8("menuOptions"));
        menuFile = new QMenu(menubar);
        menuFile->setObjectName(QString::fromUtf8("menuFile"));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);
        toolBar = new QToolBar(MainWindow);
        toolBar->setObjectName(QString::fromUtf8("toolBar"));
        toolBar->setEnabled(true);
        toolBar->setOrientation(Qt::Horizontal);
        toolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        MainWindow->addToolBar(static_cast<Qt::ToolBarArea>(4), toolBar);

        menubar->addAction(menuFile->menuAction());
        menubar->addAction(menuOptions->menuAction());
        menubar->addAction(menuAccount->menuAction());
        menuAccount->addAction(actionStatus);
        menuAccount->addAction(actionInformation);
        menuAccount->addAction(actionLogfile);
        menuOptions->addAction(actionOptions);
        menuOptions->addAction(actionWorkspace);
        menuOptions->addAction(actionPlugins);
        menuFile->addAction(actionOpen);
        menuFile->addAction(actionSave);
        menuFile->addAction(actionSave_as);
        menuFile->addAction(actionClose);
        menuFile->addSeparator();
        menuFile->addAction(actionExport_XML);
        menuFile->addAction(actionImport_XML);
        menuFile->addSeparator();
        menuFile->addAction(actionQuit);
        toolBar->addAction(actionOpen);
        toolBar->addAction(actionSave);
        toolBar->addSeparator();
        toolBar->addAction(actionCreate_PPM);
        toolBar->addSeparator();
        toolBar->addAction(actionStatus);
        toolBar->addSeparator();
        toolBar->addAction(actionOptions);
        toolBar->addAction(actionWorkspace);
        toolBar->addAction(actionPlugins);
        toolBar->addSeparator();
        toolBar->addAction(actionQuit);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "PPModd", 0, QApplication::UnicodeUTF8));
        actionOpen->setText(QApplication::translate("MainWindow", "Open", 0, QApplication::UnicodeUTF8));
        actionQuit->setText(QApplication::translate("MainWindow", "Quit", 0, QApplication::UnicodeUTF8));
        actionOptions->setText(QApplication::translate("MainWindow", "Options", 0, QApplication::UnicodeUTF8));
        actionWorkspace->setText(QApplication::translate("MainWindow", "Workspace", 0, QApplication::UnicodeUTF8));
        actionPlugins->setText(QApplication::translate("MainWindow", "Plugins", 0, QApplication::UnicodeUTF8));
        actionSave->setText(QApplication::translate("MainWindow", "Save", 0, QApplication::UnicodeUTF8));
        actionSave_as->setText(QApplication::translate("MainWindow", "Save as...", 0, QApplication::UnicodeUTF8));
        actionClose->setText(QApplication::translate("MainWindow", "Close", 0, QApplication::UnicodeUTF8));
        actionExport_XML->setText(QApplication::translate("MainWindow", "Export XML...", 0, QApplication::UnicodeUTF8));
        actionImport_XML->setText(QApplication::translate("MainWindow", "Import XML...", 0, QApplication::UnicodeUTF8));
        actionCreate_PPM->setText(QApplication::translate("MainWindow", "Create PPM", 0, QApplication::UnicodeUTF8));
        actionStatus->setText(QApplication::translate("MainWindow", "Account Status", 0, QApplication::UnicodeUTF8));
        actionInformation->setText(QApplication::translate("MainWindow", "User Information", 0, QApplication::UnicodeUTF8));
        actionLogfile->setText(QApplication::translate("MainWindow", "Logfile", 0, QApplication::UnicodeUTF8));
        dockWidgetTree->setWindowTitle(QApplication::translate("MainWindow", "PPM Tree", 0, QApplication::UnicodeUTF8));
        dockWidget->setWindowTitle(QApplication::translate("MainWindow", "Details", 0, QApplication::UnicodeUTF8));
        btnEdit->setText(QApplication::translate("MainWindow", "Edit content", 0, QApplication::UnicodeUTF8));
        menuAccount->setTitle(QApplication::translate("MainWindow", "Account", 0, QApplication::UnicodeUTF8));
        menuOptions->setTitle(QApplication::translate("MainWindow", "Extras", 0, QApplication::UnicodeUTF8));
        menuFile->setTitle(QApplication::translate("MainWindow", "File", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PPMODD_H
