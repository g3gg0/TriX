#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QDockWidget>
#include <QtGui/QFrame>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QListWidget>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QStatusBar>
#include <QtGui/QTabWidget>
#include <QtGui/QTextEdit>
#include <QtGui/QToolBar>
#include <QtGui/QToolButton>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

class Ui_MainWindow
{
public:
    QAction *actOpen;
    QAction *actSave;
    QAction *actSaveAs;
    QAction *actQuit;
    QAction *actNew;
    QAction *actAbout;
    QAction *actStart;
    QAction *actStop;
    QAction *actCut;
    QAction *actCopy;
    QAction *actPaste;
    QAction *actVerbose;
    QWidget *centralwidget;
    QVBoxLayout *vboxLayout;
    QTabWidget *tabWidget;
    QWidget *tab;
    QVBoxLayout *vboxLayout1;
    QGroupBox *groupBox_4;
    QVBoxLayout *vboxLayout2;
    QComboBox *cmbFileType;
    QFrame *frmSimple;
    QVBoxLayout *vboxLayout3;
    QGroupBox *grpSimpleInput;
    QHBoxLayout *hboxLayout;
    QLineEdit *editSimpleInput;
    QToolButton *btnSimpleInput;
    QGroupBox *grpSimpleOutput;
    QHBoxLayout *hboxLayout1;
    QLineEdit *editSimpleOutput;
    QToolButton *btnSimpleOutput;
    QFrame *frmDCT3;
    QVBoxLayout *vboxLayout4;
    QGroupBox *grpDCT3Input;
    QVBoxLayout *vboxLayout5;
    QGridLayout *gridLayout;
    QCheckBox *boxDCT3SeparatePPM;
    QLineEdit *editDCT3InputMCU;
    QLineEdit *editDCT3InputPPM;
    QLabel *label_10;
    QToolButton *btnDCT3InputPPM;
    QLabel *label_11;
    QHBoxLayout *hboxLayout2;
    QCheckBox *boxDCT3CustomPPM;
    QLineEdit *editDCT3CustomPPM;
    QSpacerItem *spacerItem;
    QToolButton *btnDCT3InputMCU;
    QGroupBox *grpDCT3Output;
    QVBoxLayout *vboxLayout6;
    QGridLayout *gridLayout1;
    QToolButton *btnDCT3OutputPPM;
    QLineEdit *editDCT3OutputMCU;
    QToolButton *btnDCT3OutputMCU;
    QLineEdit *editDCT3OutputPPM;
    QLabel *label_12;
    QLabel *label_13;
    QFrame *frmDCT4;
    QVBoxLayout *vboxLayout7;
    QGroupBox *grpDCT4Input;
    QGridLayout *gridLayout2;
    QLabel *label_15;
    QLabel *label_14;
    QToolButton *btnDCT4InputMCU;
    QLineEdit *editDCT4InputPPM;
    QLineEdit *editDCT4InputMCU;
    QToolButton *btnDCT4InputPPM;
    QGroupBox *grpDCT4Output;
    QGridLayout *gridLayout3;
    QLabel *label_17;
    QLineEdit *editDCT4OutputMCU;
    QToolButton *btnDCT4OutputMCU;
    QLineEdit *editDCT4OutputPPM;
    QLabel *label_16;
    QToolButton *btnDCT4OutputPPM;
    QGroupBox *groupBox;
    QHBoxLayout *hboxLayout3;
    QRadioButton *radioModeList;
    QRadioButton *radioModeEditor;
    QSpacerItem *spacerItem1;
    QWidget *tab_2;
    QVBoxLayout *vboxLayout8;
    QListWidget *listWidget;
    QHBoxLayout *hboxLayout4;
    QPushButton *pushButton;
    QPushButton *pushButton_2;
    QPushButton *pushButton_3;
    QPushButton *pushButton_4;
    QHBoxLayout *hboxLayout5;
    QLabel *label_5;
    QLineEdit *lineEdit_5;
    QToolButton *toolButton_5;
    QWidget *tab_3;
    QVBoxLayout *vboxLayout9;
    QTextEdit *editScriptBox;
    QHBoxLayout *hboxLayout6;
    QPushButton *btnEditImport;
    QPushButton *btnEditExport;
    QMenuBar *menubar;
    QMenu *menuProject;
    QMenu *menuNew;
    QMenu *menuEdit;
    QMenu *menuHelp;
    QMenu *menuView;
    QMenu *menuProcessing;
    QMenu *menuVerbose;
    QStatusBar *statusbar;
    QToolBar *toolbar;
    QDockWidget *dockOutput;
    QWidget *dockWidgetContents;
    QVBoxLayout *vboxLayout10;
    QTextEdit *textOutput;

    void setupUi(QMainWindow *MainWindow)
    {
    MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
    MainWindow->resize(QSize(759, 825).expandedTo(MainWindow->minimumSizeHint()));
    QSizePolicy sizePolicy(static_cast<QSizePolicy::Policy>(1), static_cast<QSizePolicy::Policy>(1));
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(MainWindow->sizePolicy().hasHeightForWidth());
    MainWindow->setSizePolicy(sizePolicy);
    MainWindow->setWindowIcon(QIcon(QString::fromUtf8(":/icons/icons/22x22-gartoon/calculator.png")));
    MainWindow->setToolButtonStyle(Qt::ToolButtonIconOnly);
    actOpen = new QAction(MainWindow);
    actOpen->setObjectName(QString::fromUtf8("actOpen"));
    actOpen->setIcon(QIcon(QString::fromUtf8(":/icons/icons/22x22-gartoon/folder-open.png")));
    actSave = new QAction(MainWindow);
    actSave->setObjectName(QString::fromUtf8("actSave"));
    actSave->setIcon(QIcon(QString::fromUtf8(":/icons/icons/22x22-gartoon/disk-floppy.png")));
    actSaveAs = new QAction(MainWindow);
    actSaveAs->setObjectName(QString::fromUtf8("actSaveAs"));
    actSaveAs->setIcon(QIcon(QString::fromUtf8(":/icons/icons/22x22-gartoon/disk.png")));
    actQuit = new QAction(MainWindow);
    actQuit->setObjectName(QString::fromUtf8("actQuit"));
    actQuit->setIcon(QIcon(QString::fromUtf8(":/icons/icons/22x22-gartoon/quit.png")));
    actNew = new QAction(MainWindow);
    actNew->setObjectName(QString::fromUtf8("actNew"));
    actNew->setIcon(QIcon(QString::fromUtf8(":/icons/icons/22x22-gartoon/file.png")));
    actAbout = new QAction(MainWindow);
    actAbout->setObjectName(QString::fromUtf8("actAbout"));
    actAbout->setIcon(QIcon(QString::fromUtf8(":/icons/icons/22x22-gartoon/information.png")));
    actStart = new QAction(MainWindow);
    actStart->setObjectName(QString::fromUtf8("actStart"));
    actStart->setIcon(QIcon(QString::fromUtf8(":/icons/icons/22x22-gartoon/choice-yes.png")));
    actStop = new QAction(MainWindow);
    actStop->setObjectName(QString::fromUtf8("actStop"));
    actStop->setEnabled(false);
    actStop->setIcon(QIcon(QString::fromUtf8(":/icons/icons/22x22-gartoon/choice-no.png")));
    actCut = new QAction(MainWindow);
    actCut->setObjectName(QString::fromUtf8("actCut"));
    actCopy = new QAction(MainWindow);
    actCopy->setObjectName(QString::fromUtf8("actCopy"));
    actPaste = new QAction(MainWindow);
    actPaste->setObjectName(QString::fromUtf8("actPaste"));
    actVerbose = new QAction(MainWindow);
    actVerbose->setObjectName(QString::fromUtf8("actVerbose"));
    actVerbose->setCheckable(true);
    actVerbose->setIcon(QIcon(QString::fromUtf8(":/icons/icons/22x22-gartoon/ghostview.png")));
    centralwidget = new QWidget(MainWindow);
    centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
    vboxLayout = new QVBoxLayout(centralwidget);
    vboxLayout->setSpacing(6);
    vboxLayout->setMargin(9);
    vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
    tabWidget = new QTabWidget(centralwidget);
    tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
    QSizePolicy sizePolicy1(static_cast<QSizePolicy::Policy>(1), static_cast<QSizePolicy::Policy>(1));
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(tabWidget->sizePolicy().hasHeightForWidth());
    tabWidget->setSizePolicy(sizePolicy1);
    tabWidget->setTabPosition(QTabWidget::North);
    tab = new QWidget();
    tab->setObjectName(QString::fromUtf8("tab"));
    vboxLayout1 = new QVBoxLayout(tab);
    vboxLayout1->setSpacing(6);
    vboxLayout1->setMargin(9);
    vboxLayout1->setObjectName(QString::fromUtf8("vboxLayout1"));
    groupBox_4 = new QGroupBox(tab);
    groupBox_4->setObjectName(QString::fromUtf8("groupBox_4"));
    groupBox_4->setFlat(true);
    vboxLayout2 = new QVBoxLayout(groupBox_4);
    vboxLayout2->setSpacing(6);
    vboxLayout2->setMargin(9);
    vboxLayout2->setObjectName(QString::fromUtf8("vboxLayout2"));
    cmbFileType = new QComboBox(groupBox_4);
    cmbFileType->setObjectName(QString::fromUtf8("cmbFileType"));

    vboxLayout2->addWidget(cmbFileType);


    vboxLayout1->addWidget(groupBox_4);

    frmSimple = new QFrame(tab);
    frmSimple->setObjectName(QString::fromUtf8("frmSimple"));
    frmSimple->setFrameShape(QFrame::NoFrame);
    frmSimple->setFrameShadow(QFrame::Raised);
    vboxLayout3 = new QVBoxLayout(frmSimple);
    vboxLayout3->setSpacing(6);
    vboxLayout3->setMargin(0);
    vboxLayout3->setObjectName(QString::fromUtf8("vboxLayout3"));
    grpSimpleInput = new QGroupBox(frmSimple);
    grpSimpleInput->setObjectName(QString::fromUtf8("grpSimpleInput"));
    grpSimpleInput->setCheckable(true);
    grpSimpleInput->setChecked(true);
    hboxLayout = new QHBoxLayout(grpSimpleInput);
    hboxLayout->setSpacing(6);
    hboxLayout->setMargin(9);
    hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
    editSimpleInput = new QLineEdit(grpSimpleInput);
    editSimpleInput->setObjectName(QString::fromUtf8("editSimpleInput"));

    hboxLayout->addWidget(editSimpleInput);

    btnSimpleInput = new QToolButton(grpSimpleInput);
    btnSimpleInput->setObjectName(QString::fromUtf8("btnSimpleInput"));
    btnSimpleInput->setIcon(QIcon(QString::fromUtf8(":/icons/icons/22x22-gartoon/viewer.png")));

    hboxLayout->addWidget(btnSimpleInput);


    vboxLayout3->addWidget(grpSimpleInput);

    grpSimpleOutput = new QGroupBox(frmSimple);
    grpSimpleOutput->setObjectName(QString::fromUtf8("grpSimpleOutput"));
    grpSimpleOutput->setCheckable(true);
    hboxLayout1 = new QHBoxLayout(grpSimpleOutput);
    hboxLayout1->setSpacing(6);
    hboxLayout1->setMargin(9);
    hboxLayout1->setObjectName(QString::fromUtf8("hboxLayout1"));
    editSimpleOutput = new QLineEdit(grpSimpleOutput);
    editSimpleOutput->setObjectName(QString::fromUtf8("editSimpleOutput"));

    hboxLayout1->addWidget(editSimpleOutput);

    btnSimpleOutput = new QToolButton(grpSimpleOutput);
    btnSimpleOutput->setObjectName(QString::fromUtf8("btnSimpleOutput"));
    btnSimpleOutput->setIcon(QIcon(QString::fromUtf8(":/icons/icons/22x22-gartoon/viewer.png")));

    hboxLayout1->addWidget(btnSimpleOutput);


    vboxLayout3->addWidget(grpSimpleOutput);


    vboxLayout1->addWidget(frmSimple);

    frmDCT3 = new QFrame(tab);
    frmDCT3->setObjectName(QString::fromUtf8("frmDCT3"));
    frmDCT3->setFrameShape(QFrame::NoFrame);
    frmDCT3->setFrameShadow(QFrame::Raised);
    vboxLayout4 = new QVBoxLayout(frmDCT3);
    vboxLayout4->setSpacing(6);
    vboxLayout4->setMargin(0);
    vboxLayout4->setObjectName(QString::fromUtf8("vboxLayout4"));
    grpDCT3Input = new QGroupBox(frmDCT3);
    grpDCT3Input->setObjectName(QString::fromUtf8("grpDCT3Input"));
    grpDCT3Input->setCheckable(true);
    grpDCT3Input->setChecked(true);
    vboxLayout5 = new QVBoxLayout(grpDCT3Input);
    vboxLayout5->setSpacing(6);
    vboxLayout5->setMargin(9);
    vboxLayout5->setObjectName(QString::fromUtf8("vboxLayout5"));
    gridLayout = new QGridLayout();
    gridLayout->setSpacing(6);
    gridLayout->setMargin(0);
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    boxDCT3SeparatePPM = new QCheckBox(grpDCT3Input);
    boxDCT3SeparatePPM->setObjectName(QString::fromUtf8("boxDCT3SeparatePPM"));
    boxDCT3SeparatePPM->setEnabled(true);
    boxDCT3SeparatePPM->setChecked(true);

    gridLayout->addWidget(boxDCT3SeparatePPM, 1, 1, 1, 1);

    editDCT3InputMCU = new QLineEdit(grpDCT3Input);
    editDCT3InputMCU->setObjectName(QString::fromUtf8("editDCT3InputMCU"));

    gridLayout->addWidget(editDCT3InputMCU, 0, 1, 1, 1);

    editDCT3InputPPM = new QLineEdit(grpDCT3Input);
    editDCT3InputPPM->setObjectName(QString::fromUtf8("editDCT3InputPPM"));

    gridLayout->addWidget(editDCT3InputPPM, 2, 1, 1, 1);

    label_10 = new QLabel(grpDCT3Input);
    label_10->setObjectName(QString::fromUtf8("label_10"));

    gridLayout->addWidget(label_10, 0, 0, 1, 1);

    btnDCT3InputPPM = new QToolButton(grpDCT3Input);
    btnDCT3InputPPM->setObjectName(QString::fromUtf8("btnDCT3InputPPM"));
    btnDCT3InputPPM->setIcon(QIcon(QString::fromUtf8(":/icons/icons/22x22-gartoon/viewer.png")));

    gridLayout->addWidget(btnDCT3InputPPM, 2, 2, 1, 1);

    label_11 = new QLabel(grpDCT3Input);
    label_11->setObjectName(QString::fromUtf8("label_11"));

    gridLayout->addWidget(label_11, 2, 0, 1, 1);

    hboxLayout2 = new QHBoxLayout();
    hboxLayout2->setSpacing(6);
    hboxLayout2->setMargin(0);
    hboxLayout2->setObjectName(QString::fromUtf8("hboxLayout2"));
    boxDCT3CustomPPM = new QCheckBox(grpDCT3Input);
    boxDCT3CustomPPM->setObjectName(QString::fromUtf8("boxDCT3CustomPPM"));

    hboxLayout2->addWidget(boxDCT3CustomPPM);

    editDCT3CustomPPM = new QLineEdit(grpDCT3Input);
    editDCT3CustomPPM->setObjectName(QString::fromUtf8("editDCT3CustomPPM"));
    editDCT3CustomPPM->setEnabled(false);
    QSizePolicy sizePolicy2(static_cast<QSizePolicy::Policy>(0), static_cast<QSizePolicy::Policy>(0));
    sizePolicy2.setHorizontalStretch(0);
    sizePolicy2.setVerticalStretch(0);
    sizePolicy2.setHeightForWidth(editDCT3CustomPPM->sizePolicy().hasHeightForWidth());
    editDCT3CustomPPM->setSizePolicy(sizePolicy2);

    hboxLayout2->addWidget(editDCT3CustomPPM);

    spacerItem = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hboxLayout2->addItem(spacerItem);


    gridLayout->addLayout(hboxLayout2, 3, 1, 1, 1);

    btnDCT3InputMCU = new QToolButton(grpDCT3Input);
    btnDCT3InputMCU->setObjectName(QString::fromUtf8("btnDCT3InputMCU"));
    btnDCT3InputMCU->setIcon(QIcon(QString::fromUtf8(":/icons/icons/22x22-gartoon/viewer.png")));

    gridLayout->addWidget(btnDCT3InputMCU, 0, 2, 1, 1);


    vboxLayout5->addLayout(gridLayout);


    vboxLayout4->addWidget(grpDCT3Input);

    grpDCT3Output = new QGroupBox(frmDCT3);
    grpDCT3Output->setObjectName(QString::fromUtf8("grpDCT3Output"));
    grpDCT3Output->setCheckable(true);
    vboxLayout6 = new QVBoxLayout(grpDCT3Output);
    vboxLayout6->setSpacing(6);
    vboxLayout6->setMargin(9);
    vboxLayout6->setObjectName(QString::fromUtf8("vboxLayout6"));
    gridLayout1 = new QGridLayout();
    gridLayout1->setSpacing(6);
    gridLayout1->setMargin(0);
    gridLayout1->setObjectName(QString::fromUtf8("gridLayout1"));
    btnDCT3OutputPPM = new QToolButton(grpDCT3Output);
    btnDCT3OutputPPM->setObjectName(QString::fromUtf8("btnDCT3OutputPPM"));
    btnDCT3OutputPPM->setIcon(QIcon(QString::fromUtf8(":/icons/icons/22x22-gartoon/viewer.png")));

    gridLayout1->addWidget(btnDCT3OutputPPM, 1, 2, 1, 1);

    editDCT3OutputMCU = new QLineEdit(grpDCT3Output);
    editDCT3OutputMCU->setObjectName(QString::fromUtf8("editDCT3OutputMCU"));

    gridLayout1->addWidget(editDCT3OutputMCU, 0, 1, 1, 1);

    btnDCT3OutputMCU = new QToolButton(grpDCT3Output);
    btnDCT3OutputMCU->setObjectName(QString::fromUtf8("btnDCT3OutputMCU"));
    btnDCT3OutputMCU->setIcon(QIcon(QString::fromUtf8(":/icons/icons/22x22-gartoon/viewer.png")));

    gridLayout1->addWidget(btnDCT3OutputMCU, 0, 2, 1, 1);

    editDCT3OutputPPM = new QLineEdit(grpDCT3Output);
    editDCT3OutputPPM->setObjectName(QString::fromUtf8("editDCT3OutputPPM"));

    gridLayout1->addWidget(editDCT3OutputPPM, 1, 1, 1, 1);

    label_12 = new QLabel(grpDCT3Output);
    label_12->setObjectName(QString::fromUtf8("label_12"));

    gridLayout1->addWidget(label_12, 0, 0, 1, 1);

    label_13 = new QLabel(grpDCT3Output);
    label_13->setObjectName(QString::fromUtf8("label_13"));

    gridLayout1->addWidget(label_13, 1, 0, 1, 1);


    vboxLayout6->addLayout(gridLayout1);


    vboxLayout4->addWidget(grpDCT3Output);


    vboxLayout1->addWidget(frmDCT3);

    frmDCT4 = new QFrame(tab);
    frmDCT4->setObjectName(QString::fromUtf8("frmDCT4"));
    frmDCT4->setFrameShape(QFrame::NoFrame);
    frmDCT4->setFrameShadow(QFrame::Raised);
    vboxLayout7 = new QVBoxLayout(frmDCT4);
    vboxLayout7->setSpacing(6);
    vboxLayout7->setMargin(0);
    vboxLayout7->setObjectName(QString::fromUtf8("vboxLayout7"));
    grpDCT4Input = new QGroupBox(frmDCT4);
    grpDCT4Input->setObjectName(QString::fromUtf8("grpDCT4Input"));
    grpDCT4Input->setCheckable(true);
    gridLayout2 = new QGridLayout(grpDCT4Input);
    gridLayout2->setSpacing(6);
    gridLayout2->setMargin(9);
    gridLayout2->setObjectName(QString::fromUtf8("gridLayout2"));
    label_15 = new QLabel(grpDCT4Input);
    label_15->setObjectName(QString::fromUtf8("label_15"));

    gridLayout2->addWidget(label_15, 1, 0, 1, 1);

    label_14 = new QLabel(grpDCT4Input);
    label_14->setObjectName(QString::fromUtf8("label_14"));

    gridLayout2->addWidget(label_14, 0, 0, 1, 1);

    btnDCT4InputMCU = new QToolButton(grpDCT4Input);
    btnDCT4InputMCU->setObjectName(QString::fromUtf8("btnDCT4InputMCU"));
    btnDCT4InputMCU->setIcon(QIcon(QString::fromUtf8(":/icons/icons/22x22-gartoon/viewer.png")));

    gridLayout2->addWidget(btnDCT4InputMCU, 0, 2, 1, 1);

    editDCT4InputPPM = new QLineEdit(grpDCT4Input);
    editDCT4InputPPM->setObjectName(QString::fromUtf8("editDCT4InputPPM"));

    gridLayout2->addWidget(editDCT4InputPPM, 1, 1, 1, 1);

    editDCT4InputMCU = new QLineEdit(grpDCT4Input);
    editDCT4InputMCU->setObjectName(QString::fromUtf8("editDCT4InputMCU"));

    gridLayout2->addWidget(editDCT4InputMCU, 0, 1, 1, 1);

    btnDCT4InputPPM = new QToolButton(grpDCT4Input);
    btnDCT4InputPPM->setObjectName(QString::fromUtf8("btnDCT4InputPPM"));
    btnDCT4InputPPM->setIcon(QIcon(QString::fromUtf8(":/icons/icons/22x22-gartoon/viewer.png")));

    gridLayout2->addWidget(btnDCT4InputPPM, 1, 2, 1, 1);


    vboxLayout7->addWidget(grpDCT4Input);

    grpDCT4Output = new QGroupBox(frmDCT4);
    grpDCT4Output->setObjectName(QString::fromUtf8("grpDCT4Output"));
    grpDCT4Output->setCheckable(true);
    gridLayout3 = new QGridLayout(grpDCT4Output);
    gridLayout3->setSpacing(6);
    gridLayout3->setMargin(9);
    gridLayout3->setObjectName(QString::fromUtf8("gridLayout3"));
    label_17 = new QLabel(grpDCT4Output);
    label_17->setObjectName(QString::fromUtf8("label_17"));

    gridLayout3->addWidget(label_17, 1, 0, 1, 1);

    editDCT4OutputMCU = new QLineEdit(grpDCT4Output);
    editDCT4OutputMCU->setObjectName(QString::fromUtf8("editDCT4OutputMCU"));

    gridLayout3->addWidget(editDCT4OutputMCU, 0, 1, 1, 1);

    btnDCT4OutputMCU = new QToolButton(grpDCT4Output);
    btnDCT4OutputMCU->setObjectName(QString::fromUtf8("btnDCT4OutputMCU"));
    btnDCT4OutputMCU->setIcon(QIcon(QString::fromUtf8(":/icons/icons/22x22-gartoon/viewer.png")));

    gridLayout3->addWidget(btnDCT4OutputMCU, 0, 2, 1, 1);

    editDCT4OutputPPM = new QLineEdit(grpDCT4Output);
    editDCT4OutputPPM->setObjectName(QString::fromUtf8("editDCT4OutputPPM"));

    gridLayout3->addWidget(editDCT4OutputPPM, 1, 1, 1, 1);

    label_16 = new QLabel(grpDCT4Output);
    label_16->setObjectName(QString::fromUtf8("label_16"));

    gridLayout3->addWidget(label_16, 0, 0, 1, 1);

    btnDCT4OutputPPM = new QToolButton(grpDCT4Output);
    btnDCT4OutputPPM->setObjectName(QString::fromUtf8("btnDCT4OutputPPM"));
    btnDCT4OutputPPM->setIcon(QIcon(QString::fromUtf8(":/icons/icons/22x22-gartoon/viewer.png")));

    gridLayout3->addWidget(btnDCT4OutputPPM, 1, 2, 1, 1);


    vboxLayout7->addWidget(grpDCT4Output);


    vboxLayout1->addWidget(frmDCT4);

    groupBox = new QGroupBox(tab);
    groupBox->setObjectName(QString::fromUtf8("groupBox"));
    hboxLayout3 = new QHBoxLayout(groupBox);
    hboxLayout3->setSpacing(6);
    hboxLayout3->setMargin(9);
    hboxLayout3->setObjectName(QString::fromUtf8("hboxLayout3"));
    radioModeList = new QRadioButton(groupBox);
    radioModeList->setObjectName(QString::fromUtf8("radioModeList"));

    hboxLayout3->addWidget(radioModeList);

    radioModeEditor = new QRadioButton(groupBox);
    radioModeEditor->setObjectName(QString::fromUtf8("radioModeEditor"));

    hboxLayout3->addWidget(radioModeEditor);


    vboxLayout1->addWidget(groupBox);

    spacerItem1 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    vboxLayout1->addItem(spacerItem1);

    tabWidget->addTab(tab, QIcon(QString::fromUtf8(":/icons/icons/22x22-gartoon/configuration.png")), QApplication::translate("MainWindow", "General", 0, QApplication::UnicodeUTF8));
    tab_2 = new QWidget();
    tab_2->setObjectName(QString::fromUtf8("tab_2"));
    tab_2->setCursor(QCursor(static_cast<Qt::CursorShape>(0)));
    vboxLayout8 = new QVBoxLayout(tab_2);
    vboxLayout8->setSpacing(6);
    vboxLayout8->setMargin(9);
    vboxLayout8->setObjectName(QString::fromUtf8("vboxLayout8"));
    listWidget = new QListWidget(tab_2);
    listWidget->setObjectName(QString::fromUtf8("listWidget"));

    vboxLayout8->addWidget(listWidget);

    hboxLayout4 = new QHBoxLayout();
    hboxLayout4->setSpacing(6);
    hboxLayout4->setMargin(0);
    hboxLayout4->setObjectName(QString::fromUtf8("hboxLayout4"));
    pushButton = new QPushButton(tab_2);
    pushButton->setObjectName(QString::fromUtf8("pushButton"));
    pushButton->setIcon(QIcon(QString::fromUtf8(":/icons/icons/22x22-gartoon/modules.png")));

    hboxLayout4->addWidget(pushButton);

    pushButton_2 = new QPushButton(tab_2);
    pushButton_2->setObjectName(QString::fromUtf8("pushButton_2"));
    pushButton_2->setIcon(QIcon(QString::fromUtf8(":/icons/icons/22x22-gartoon/choice-no.png")));

    hboxLayout4->addWidget(pushButton_2);

    pushButton_3 = new QPushButton(tab_2);
    pushButton_3->setObjectName(QString::fromUtf8("pushButton_3"));
    pushButton_3->setIcon(QIcon(QString::fromUtf8(":/icons/icons/22x22-gartoon/information.png")));

    hboxLayout4->addWidget(pushButton_3);

    pushButton_4 = new QPushButton(tab_2);
    pushButton_4->setObjectName(QString::fromUtf8("pushButton_4"));
    pushButton_4->setIcon(QIcon(QString::fromUtf8(":/icons/icons/22x22-gartoon/configuration.png")));

    hboxLayout4->addWidget(pushButton_4);


    vboxLayout8->addLayout(hboxLayout4);

    hboxLayout5 = new QHBoxLayout();
    hboxLayout5->setSpacing(6);
    hboxLayout5->setMargin(0);
    hboxLayout5->setObjectName(QString::fromUtf8("hboxLayout5"));
    label_5 = new QLabel(tab_2);
    label_5->setObjectName(QString::fromUtf8("label_5"));

    hboxLayout5->addWidget(label_5);

    lineEdit_5 = new QLineEdit(tab_2);
    lineEdit_5->setObjectName(QString::fromUtf8("lineEdit_5"));

    hboxLayout5->addWidget(lineEdit_5);

    toolButton_5 = new QToolButton(tab_2);
    toolButton_5->setObjectName(QString::fromUtf8("toolButton_5"));
    toolButton_5->setIcon(QIcon(QString::fromUtf8(":/icons/icons/22x22-gartoon/viewer.png")));

    hboxLayout5->addWidget(toolButton_5);


    vboxLayout8->addLayout(hboxLayout5);

    tabWidget->addTab(tab_2, QIcon(QString::fromUtf8(":/icons/icons/22x22-gartoon/modules.png")), QApplication::translate("MainWindow", "Scripts", 0, QApplication::UnicodeUTF8));
    tab_3 = new QWidget();
    tab_3->setObjectName(QString::fromUtf8("tab_3"));
    vboxLayout9 = new QVBoxLayout(tab_3);
    vboxLayout9->setSpacing(6);
    vboxLayout9->setMargin(9);
    vboxLayout9->setObjectName(QString::fromUtf8("vboxLayout9"));
    editScriptBox = new QTextEdit(tab_3);
    editScriptBox->setObjectName(QString::fromUtf8("editScriptBox"));
    QFont font;
    font.setFamily(QString::fromUtf8("Lucida Console"));
    font.setPointSize(8);
    font.setBold(false);
    font.setItalic(false);
    font.setUnderline(false);
    font.setWeight(50);
    font.setStrikeOut(false);
    editScriptBox->setFont(font);
    editScriptBox->setLineWrapMode(QTextEdit::NoWrap);
    editScriptBox->setTabStopWidth(21);
    editScriptBox->setAcceptRichText(false);

    vboxLayout9->addWidget(editScriptBox);

    hboxLayout6 = new QHBoxLayout();
    hboxLayout6->setSpacing(6);
    hboxLayout6->setMargin(0);
    hboxLayout6->setObjectName(QString::fromUtf8("hboxLayout6"));
    btnEditImport = new QPushButton(tab_3);
    btnEditImport->setObjectName(QString::fromUtf8("btnEditImport"));
    btnEditImport->setIcon(QIcon(QString::fromUtf8(":/icons/icons/22x22-gartoon/folder-open.png")));

    hboxLayout6->addWidget(btnEditImport);

    btnEditExport = new QPushButton(tab_3);
    btnEditExport->setObjectName(QString::fromUtf8("btnEditExport"));
    btnEditExport->setIcon(QIcon(QString::fromUtf8(":/icons/icons/22x22-gartoon/disk.png")));

    hboxLayout6->addWidget(btnEditExport);


    vboxLayout9->addLayout(hboxLayout6);

    tabWidget->addTab(tab_3, QIcon(QString::fromUtf8(":/icons/icons/22x22-gartoon/editor.png")), QApplication::translate("MainWindow", "Editor", 0, QApplication::UnicodeUTF8));

    vboxLayout->addWidget(tabWidget);

    MainWindow->setCentralWidget(centralwidget);
    menubar = new QMenuBar(MainWindow);
    menubar->setObjectName(QString::fromUtf8("menubar"));
    menubar->setGeometry(QRect(0, 0, 759, 24));
    menuProject = new QMenu(menubar);
    menuProject->setObjectName(QString::fromUtf8("menuProject"));
    menuNew = new QMenu(menuProject);
    menuNew->setObjectName(QString::fromUtf8("menuNew"));
    menuEdit = new QMenu(menubar);
    menuEdit->setObjectName(QString::fromUtf8("menuEdit"));
    menuHelp = new QMenu(menubar);
    menuHelp->setObjectName(QString::fromUtf8("menuHelp"));
    menuView = new QMenu(menubar);
    menuView->setObjectName(QString::fromUtf8("menuView"));
    menuProcessing = new QMenu(menubar);
    menuProcessing->setObjectName(QString::fromUtf8("menuProcessing"));
    menuVerbose = new QMenu(menuProcessing);
    menuVerbose->setObjectName(QString::fromUtf8("menuVerbose"));
    MainWindow->setMenuBar(menubar);
    statusbar = new QStatusBar(MainWindow);
    statusbar->setObjectName(QString::fromUtf8("statusbar"));
    MainWindow->setStatusBar(statusbar);
    toolbar = new QToolBar(MainWindow);
    toolbar->setObjectName(QString::fromUtf8("toolbar"));
    toolbar->setOrientation(Qt::Horizontal);
    toolbar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    MainWindow->addToolBar(static_cast<Qt::ToolBarArea>(4), toolbar);
    dockOutput = new QDockWidget(MainWindow);
    dockOutput->setObjectName(QString::fromUtf8("dockOutput"));
    dockOutput->setWindowIcon(QIcon(QString::fromUtf8(":/icons/icons/22x22-gartoon/chat.png")));
    dockOutput->setAllowedAreas(Qt::AllDockWidgetAreas);
    dockWidgetContents = new QWidget(dockOutput);
    dockWidgetContents->setObjectName(QString::fromUtf8("dockWidgetContents"));
    vboxLayout10 = new QVBoxLayout(dockWidgetContents);
    vboxLayout10->setSpacing(6);
    vboxLayout10->setMargin(9);
    vboxLayout10->setObjectName(QString::fromUtf8("vboxLayout10"));
    textOutput = new QTextEdit(dockWidgetContents);
    textOutput->setObjectName(QString::fromUtf8("textOutput"));
    QFont font1;
    font1.setFamily(QString::fromUtf8("Lucida Console"));
    font1.setPointSize(8);
    font1.setBold(false);
    font1.setItalic(false);
    font1.setUnderline(false);
    font1.setWeight(50);
    font1.setStrikeOut(false);
    textOutput->setFont(font1);
    textOutput->setUndoRedoEnabled(false);
    textOutput->setReadOnly(true);
    textOutput->setAcceptRichText(false);

    vboxLayout10->addWidget(textOutput);

    dockOutput->setWidget(dockWidgetContents);
    MainWindow->addDockWidget(static_cast<Qt::DockWidgetArea>(2), dockOutput);
    QWidget::setTabOrder(cmbFileType, editSimpleInput);
    QWidget::setTabOrder(editSimpleInput, btnSimpleInput);
    QWidget::setTabOrder(btnSimpleInput, editSimpleOutput);
    QWidget::setTabOrder(editSimpleOutput, btnSimpleOutput);
    QWidget::setTabOrder(btnSimpleOutput, editDCT3InputMCU);
    QWidget::setTabOrder(editDCT3InputMCU, btnDCT3InputMCU);
    QWidget::setTabOrder(btnDCT3InputMCU, boxDCT3SeparatePPM);
    QWidget::setTabOrder(boxDCT3SeparatePPM, editDCT3InputPPM);
    QWidget::setTabOrder(editDCT3InputPPM, btnDCT3InputPPM);
    QWidget::setTabOrder(btnDCT3InputPPM, boxDCT3CustomPPM);
    QWidget::setTabOrder(boxDCT3CustomPPM, editDCT3CustomPPM);
    QWidget::setTabOrder(editDCT3CustomPPM, editDCT3OutputMCU);
    QWidget::setTabOrder(editDCT3OutputMCU, btnDCT3OutputMCU);
    QWidget::setTabOrder(btnDCT3OutputMCU, editDCT3OutputPPM);
    QWidget::setTabOrder(editDCT3OutputPPM, btnDCT3OutputPPM);
    QWidget::setTabOrder(btnDCT3OutputPPM, editDCT4InputMCU);
    QWidget::setTabOrder(editDCT4InputMCU, btnDCT4InputMCU);
    QWidget::setTabOrder(btnDCT4InputMCU, editDCT4InputPPM);
    QWidget::setTabOrder(editDCT4InputPPM, btnDCT4InputPPM);
    QWidget::setTabOrder(btnDCT4InputPPM, editDCT4OutputMCU);
    QWidget::setTabOrder(editDCT4OutputMCU, btnDCT4OutputMCU);
    QWidget::setTabOrder(btnDCT4OutputMCU, editDCT4OutputPPM);
    QWidget::setTabOrder(editDCT4OutputPPM, btnDCT4OutputPPM);
    QWidget::setTabOrder(btnDCT4OutputPPM, tabWidget);
    QWidget::setTabOrder(tabWidget, listWidget);
    QWidget::setTabOrder(listWidget, pushButton);
    QWidget::setTabOrder(pushButton, pushButton_2);
    QWidget::setTabOrder(pushButton_2, pushButton_3);
    QWidget::setTabOrder(pushButton_3, pushButton_4);
    QWidget::setTabOrder(pushButton_4, lineEdit_5);
    QWidget::setTabOrder(lineEdit_5, toolButton_5);
    QWidget::setTabOrder(toolButton_5, editScriptBox);
    QWidget::setTabOrder(editScriptBox, btnEditImport);
    QWidget::setTabOrder(btnEditImport, btnEditExport);
    QWidget::setTabOrder(btnEditExport, textOutput);

    menubar->addAction(menuProject->menuAction());
    menubar->addAction(menuEdit->menuAction());
    menubar->addAction(menuProcessing->menuAction());
    menubar->addAction(menuHelp->menuAction());
    menuProject->addAction(actNew);
    menuProject->addSeparator();
    menuProject->addAction(actOpen);
    menuProject->addAction(actSave);
    menuProject->addAction(actSaveAs);
    menuProject->addSeparator();
    menuProject->addAction(actQuit);
    menuEdit->addAction(actCut);
    menuEdit->addAction(actCopy);
    menuEdit->addAction(actPaste);
    menuHelp->addAction(actAbout);
    menuProcessing->addAction(actStart);
    menuProcessing->addAction(actStop);
    menuProcessing->addSeparator();
    menuProcessing->addAction(actVerbose);
    toolbar->addAction(actNew);
    toolbar->addAction(actOpen);
    toolbar->addAction(actSave);
    toolbar->addSeparator();
    toolbar->addAction(actStart);
    toolbar->addAction(actStop);
    toolbar->addSeparator();
    toolbar->addAction(actAbout);
    retranslateUi(MainWindow);
    QObject::connect(grpSimpleInput, SIGNAL(toggled(bool)), grpSimpleOutput, SLOT(setEnabled(bool)));
    QObject::connect(grpDCT3Input, SIGNAL(toggled(bool)), grpDCT3Output, SLOT(setEnabled(bool)));
    QObject::connect(grpDCT4Input, SIGNAL(toggled(bool)), grpDCT4Output, SLOT(setEnabled(bool)));
    QObject::connect(boxDCT3SeparatePPM, SIGNAL(toggled(bool)), editDCT3InputPPM, SLOT(setEnabled(bool)));
    QObject::connect(boxDCT3SeparatePPM, SIGNAL(toggled(bool)), boxDCT3CustomPPM, SLOT(setEnabled(bool)));
    QObject::connect(boxDCT3CustomPPM, SIGNAL(toggled(bool)), editDCT3CustomPPM, SLOT(setEnabled(bool)));
    QObject::connect(boxDCT3SeparatePPM, SIGNAL(toggled(bool)), btnDCT3InputPPM, SLOT(setEnabled(bool)));

    tabWidget->setCurrentIndex(0);


    QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
    MainWindow->setWindowTitle(QApplication::translate("MainWindow", "TriX", 0, QApplication::UnicodeUTF8));
    MainWindow->setStatusTip(QApplication::translate("MainWindow", "", 0, QApplication::UnicodeUTF8));
    actOpen->setText(QApplication::translate("MainWindow", "Open...", 0, QApplication::UnicodeUTF8));
    actSave->setText(QApplication::translate("MainWindow", "Save", 0, QApplication::UnicodeUTF8));
    actSaveAs->setText(QApplication::translate("MainWindow", "Save As...", 0, QApplication::UnicodeUTF8));
    actQuit->setText(QApplication::translate("MainWindow", "Quit", 0, QApplication::UnicodeUTF8));
    actQuit->setShortcut(QApplication::translate("MainWindow", "Alt+F4", 0, QApplication::UnicodeUTF8));
    actNew->setText(QApplication::translate("MainWindow", "New", 0, QApplication::UnicodeUTF8));
    actNew->setStatusTip(QApplication::translate("MainWindow", "Creates a new project", 0, QApplication::UnicodeUTF8));
    actAbout->setText(QApplication::translate("MainWindow", "About", 0, QApplication::UnicodeUTF8));
    actStart->setText(QApplication::translate("MainWindow", "Start", 0, QApplication::UnicodeUTF8));
    actStop->setText(QApplication::translate("MainWindow", "Stop", 0, QApplication::UnicodeUTF8));
    actCut->setText(QApplication::translate("MainWindow", "Cut", 0, QApplication::UnicodeUTF8));
    actCut->setShortcut(QApplication::translate("MainWindow", "Ctrl+X", 0, QApplication::UnicodeUTF8));
    actCopy->setText(QApplication::translate("MainWindow", "Copy", 0, QApplication::UnicodeUTF8));
    actCopy->setShortcut(QApplication::translate("MainWindow", "Ctrl+C", 0, QApplication::UnicodeUTF8));
    actPaste->setText(QApplication::translate("MainWindow", "Paste", 0, QApplication::UnicodeUTF8));
    actPaste->setShortcut(QApplication::translate("MainWindow", "Ctrl+V", 0, QApplication::UnicodeUTF8));
    actVerbose->setText(QApplication::translate("MainWindow", "Verbose", 0, QApplication::UnicodeUTF8));
    groupBox_4->setTitle(QApplication::translate("MainWindow", "Project type", 0, QApplication::UnicodeUTF8));
    cmbFileType->clear();
    cmbFileType->addItem(QIcon(QString::fromUtf8(":/icons/icons/22x22-gartoon/item.png")), QApplication::translate("MainWindow", "Simple", 0, QApplication::UnicodeUTF8));
    cmbFileType->addItem(QIcon(QString::fromUtf8(":/icons/icons/22x22-gartoon/calculator.png")), QApplication::translate("MainWindow", "DCT3 Phone Firmware", 0, QApplication::UnicodeUTF8));
    cmbFileType->addItem(QIcon(QString::fromUtf8(":/icons/icons/22x22-gartoon/calculator.png")), QApplication::translate("MainWindow", "DCT4 Phone Firmware", 0, QApplication::UnicodeUTF8));
    grpSimpleInput->setTitle(QApplication::translate("MainWindow", "Input File", 0, QApplication::UnicodeUTF8));
    btnSimpleInput->setText(QApplication::translate("MainWindow", "...", 0, QApplication::UnicodeUTF8));
    grpSimpleOutput->setTitle(QApplication::translate("MainWindow", "Output File", 0, QApplication::UnicodeUTF8));
    btnSimpleOutput->setText(QApplication::translate("MainWindow", "...", 0, QApplication::UnicodeUTF8));
    grpDCT3Input->setTitle(QApplication::translate("MainWindow", "Input Files", 0, QApplication::UnicodeUTF8));
    boxDCT3SeparatePPM->setText(QApplication::translate("MainWindow", "Separate PPM File", 0, QApplication::UnicodeUTF8));
    label_10->setText(QApplication::translate("MainWindow", "MCU", 0, QApplication::UnicodeUTF8));
    btnDCT3InputPPM->setText(QApplication::translate("MainWindow", "...", 0, QApplication::UnicodeUTF8));
    label_11->setText(QApplication::translate("MainWindow", "PPM", 0, QApplication::UnicodeUTF8));
    boxDCT3CustomPPM->setText(QApplication::translate("MainWindow", "Custom PPM address (hex)", 0, QApplication::UnicodeUTF8));
    btnDCT3InputMCU->setText(QApplication::translate("MainWindow", "...", 0, QApplication::UnicodeUTF8));
    grpDCT3Output->setTitle(QApplication::translate("MainWindow", "Output Files", 0, QApplication::UnicodeUTF8));
    btnDCT3OutputPPM->setText(QApplication::translate("MainWindow", "...", 0, QApplication::UnicodeUTF8));
    btnDCT3OutputMCU->setText(QApplication::translate("MainWindow", "...", 0, QApplication::UnicodeUTF8));
    label_12->setText(QApplication::translate("MainWindow", "MCU", 0, QApplication::UnicodeUTF8));
    label_13->setText(QApplication::translate("MainWindow", "PPM", 0, QApplication::UnicodeUTF8));
    grpDCT4Input->setTitle(QApplication::translate("MainWindow", "Input Files", 0, QApplication::UnicodeUTF8));
    label_15->setText(QApplication::translate("MainWindow", "PPM", 0, QApplication::UnicodeUTF8));
    label_14->setText(QApplication::translate("MainWindow", "MCU", 0, QApplication::UnicodeUTF8));
    btnDCT4InputMCU->setText(QApplication::translate("MainWindow", "...", 0, QApplication::UnicodeUTF8));
    btnDCT4InputPPM->setText(QApplication::translate("MainWindow", "...", 0, QApplication::UnicodeUTF8));
    grpDCT4Output->setTitle(QApplication::translate("MainWindow", "Output Files", 0, QApplication::UnicodeUTF8));
    label_17->setText(QApplication::translate("MainWindow", "PPM", 0, QApplication::UnicodeUTF8));
    btnDCT4OutputMCU->setText(QApplication::translate("MainWindow", "...", 0, QApplication::UnicodeUTF8));
    label_16->setText(QApplication::translate("MainWindow", "MCU", 0, QApplication::UnicodeUTF8));
    btnDCT4OutputPPM->setText(QApplication::translate("MainWindow", "...", 0, QApplication::UnicodeUTF8));
    groupBox->setTitle(QApplication::translate("MainWindow", "TriX mode", 0, QApplication::UnicodeUTF8));
    radioModeList->setText(QApplication::translate("MainWindow", "Scripts processing list", 0, QApplication::UnicodeUTF8));
    radioModeEditor->setText(QApplication::translate("MainWindow", "Direct script editor", 0, QApplication::UnicodeUTF8));
    tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("MainWindow", "General", 0, QApplication::UnicodeUTF8));
    tab_2->setToolTip(QApplication::translate("MainWindow", "", 0, QApplication::UnicodeUTF8));
    tab_2->setStatusTip(QApplication::translate("MainWindow", "", 0, QApplication::UnicodeUTF8));
    tab_2->setWhatsThis(QApplication::translate("MainWindow", "", 0, QApplication::UnicodeUTF8));
    tab_2->setAccessibleName(QApplication::translate("MainWindow", "", 0, QApplication::UnicodeUTF8));
    tab_2->setAccessibleDescription(QApplication::translate("MainWindow", "", 0, QApplication::UnicodeUTF8));
    pushButton->setText(QApplication::translate("MainWindow", "Add...", 0, QApplication::UnicodeUTF8));
    pushButton_2->setText(QApplication::translate("MainWindow", "Remove", 0, QApplication::UnicodeUTF8));
    pushButton_3->setText(QApplication::translate("MainWindow", "Manual...", 0, QApplication::UnicodeUTF8));
    pushButton_4->setText(QApplication::translate("MainWindow", "Options...", 0, QApplication::UnicodeUTF8));
    label_5->setText(QApplication::translate("MainWindow", "<html><head><meta name=\"qrichtext\" content=\"1\" /></head><body style=\" white-space: pre-wrap; font-family:MS Shell Dlg; font-size:8.25pt; font-weight:400; font-style:normal; text-decoration:none;\"><p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:8pt;\">Script</span></p></body></html>", 0, QApplication::UnicodeUTF8));
    toolButton_5->setText(QApplication::translate("MainWindow", "...", 0, QApplication::UnicodeUTF8));
    tabWidget->setTabText(tabWidget->indexOf(tab_2), QApplication::translate("MainWindow", "Scripts", 0, QApplication::UnicodeUTF8));
    editScriptBox->setDocumentTitle(QApplication::translate("MainWindow", "", 0, QApplication::UnicodeUTF8));
    btnEditImport->setText(QApplication::translate("MainWindow", "Import...", "Load a Script from a File", QApplication::UnicodeUTF8));
    btnEditExport->setText(QApplication::translate("MainWindow", "Export...", "Save the Script to a File", QApplication::UnicodeUTF8));
    tabWidget->setTabText(tabWidget->indexOf(tab_3), QApplication::translate("MainWindow", "Editor", 0, QApplication::UnicodeUTF8));
    menuProject->setTitle(QApplication::translate("MainWindow", "Project", 0, QApplication::UnicodeUTF8));
    menuNew->setTitle(QApplication::translate("MainWindow", "New", 0, QApplication::UnicodeUTF8));
    menuEdit->setTitle(QApplication::translate("MainWindow", "Edit", 0, QApplication::UnicodeUTF8));
    menuHelp->setTitle(QApplication::translate("MainWindow", "Help", 0, QApplication::UnicodeUTF8));
    menuView->setTitle(QApplication::translate("MainWindow", "View", 0, QApplication::UnicodeUTF8));
    menuProcessing->setTitle(QApplication::translate("MainWindow", "Processing", 0, QApplication::UnicodeUTF8));
    menuVerbose->setTitle(QApplication::translate("MainWindow", "Verbose", 0, QApplication::UnicodeUTF8));
    toolbar->setWindowTitle(QApplication::translate("MainWindow", "Toolbar", 0, QApplication::UnicodeUTF8));
    dockOutput->setWindowTitle(QApplication::translate("MainWindow", "Output", 0, QApplication::UnicodeUTF8));
    textOutput->setHtml(QApplication::translate("MainWindow", "<html><head><meta name=\"qrichtext\" content=\"1\" /></head><body style=\" white-space: pre-wrap; font-family:Lucida Console; font-size:8pt; font-weight:400; font-style:normal; text-decoration:none;\"><p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"></p></body></html>", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

#endif // UI_MAINWINDOW_H
