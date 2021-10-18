/********************************************************************************
** Form generated from reading ui file 'Workspace.ui'
**
** Created: Sun 3. Aug 23:16:31 2014
**      by: Qt User Interface Compiler version 4.3.2
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_WORKSPACE_H
#define UI_WORKSPACE_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QTextEdit>
#include <QtGui/QTreeWidget>

class Ui_dlgWorkspaceDisplay
{
public:
    QGridLayout *gridLayout;
    QGroupBox *groupBox;
    QGridLayout *gridLayout1;
    QGridLayout *gridLayout2;
    QTextEdit *textItemInfo;
    QHBoxLayout *hboxLayout;
    QSpacerItem *spacerItem;
    QPushButton *btnEdit;
    QPushButton *btnImport;
    QPushButton *btnExport;
    QGroupBox *groupBox_2;
    QHBoxLayout *hboxLayout1;
    QTreeWidget *treeWorkspace;
    QHBoxLayout *hboxLayout2;
    QSpacerItem *spacerItem1;
    QPushButton *okButton;

    void setupUi(QDialog *dlgWorkspaceDisplay)
    {
    if (dlgWorkspaceDisplay->objectName().isEmpty())
        dlgWorkspaceDisplay->setObjectName(QString::fromUtf8("dlgWorkspaceDisplay"));
    dlgWorkspaceDisplay->resize(734, 462);
    dlgWorkspaceDisplay->setWindowIcon(QIcon(QString::fromUtf8(":/icons/icons/22x22-gartoon/modules.png")));
    gridLayout = new QGridLayout(dlgWorkspaceDisplay);
#ifndef Q_OS_MAC
    gridLayout->setSpacing(6);
#endif
#ifndef Q_OS_MAC
    gridLayout->setMargin(9);
#endif
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    groupBox = new QGroupBox(dlgWorkspaceDisplay);
    groupBox->setObjectName(QString::fromUtf8("groupBox"));
    gridLayout1 = new QGridLayout(groupBox);
#ifndef Q_OS_MAC
    gridLayout1->setSpacing(6);
#endif
#ifndef Q_OS_MAC
    gridLayout1->setMargin(9);
#endif
    gridLayout1->setObjectName(QString::fromUtf8("gridLayout1"));
    gridLayout2 = new QGridLayout();
#ifndef Q_OS_MAC
    gridLayout2->setSpacing(6);
#endif
#ifndef Q_OS_MAC
    gridLayout2->setMargin(0);
#endif
    gridLayout2->setObjectName(QString::fromUtf8("gridLayout2"));
    textItemInfo = new QTextEdit(groupBox);
    textItemInfo->setObjectName(QString::fromUtf8("textItemInfo"));
    textItemInfo->setEnabled(false);
    textItemInfo->setAcceptDrops(false);
    textItemInfo->setFrameShape(QFrame::Box);
    textItemInfo->setFrameShadow(QFrame::Plain);
    textItemInfo->setLineWidth(0);

    gridLayout2->addWidget(textItemInfo, 0, 0, 2, 1);

    hboxLayout = new QHBoxLayout();
#ifndef Q_OS_MAC
    hboxLayout->setSpacing(6);
#endif
    hboxLayout->setMargin(0);
    hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
    spacerItem = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hboxLayout->addItem(spacerItem);

    btnEdit = new QPushButton(groupBox);
    btnEdit->setObjectName(QString::fromUtf8("btnEdit"));

    hboxLayout->addWidget(btnEdit);

    btnImport = new QPushButton(groupBox);
    btnImport->setObjectName(QString::fromUtf8("btnImport"));
    btnImport->setEnabled(true);

    hboxLayout->addWidget(btnImport);

    btnExport = new QPushButton(groupBox);
    btnExport->setObjectName(QString::fromUtf8("btnExport"));

    hboxLayout->addWidget(btnExport);


    gridLayout2->addLayout(hboxLayout, 2, 0, 1, 1);


    gridLayout1->addLayout(gridLayout2, 0, 0, 1, 1);


    gridLayout->addWidget(groupBox, 0, 1, 1, 1);

    groupBox_2 = new QGroupBox(dlgWorkspaceDisplay);
    groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
    hboxLayout1 = new QHBoxLayout(groupBox_2);
#ifndef Q_OS_MAC
    hboxLayout1->setSpacing(6);
#endif
#ifndef Q_OS_MAC
    hboxLayout1->setMargin(9);
#endif
    hboxLayout1->setObjectName(QString::fromUtf8("hboxLayout1"));
    treeWorkspace = new QTreeWidget(groupBox_2);
    treeWorkspace->setObjectName(QString::fromUtf8("treeWorkspace"));
    treeWorkspace->setAutoFillBackground(false);

    hboxLayout1->addWidget(treeWorkspace);


    gridLayout->addWidget(groupBox_2, 0, 0, 1, 1);

    hboxLayout2 = new QHBoxLayout();
#ifndef Q_OS_MAC
    hboxLayout2->setSpacing(6);
#endif
    hboxLayout2->setMargin(0);
    hboxLayout2->setObjectName(QString::fromUtf8("hboxLayout2"));
    spacerItem1 = new QSpacerItem(131, 31, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hboxLayout2->addItem(spacerItem1);

    okButton = new QPushButton(dlgWorkspaceDisplay);
    okButton->setObjectName(QString::fromUtf8("okButton"));

    hboxLayout2->addWidget(okButton);


    gridLayout->addLayout(hboxLayout2, 1, 0, 1, 2);


    retranslateUi(dlgWorkspaceDisplay);

    QMetaObject::connectSlotsByName(dlgWorkspaceDisplay);
    } // setupUi

    void retranslateUi(QDialog *dlgWorkspaceDisplay)
    {
    dlgWorkspaceDisplay->setWindowTitle(QApplication::translate("dlgWorkspaceDisplay", "Show workspace", 0, QApplication::UnicodeUTF8));
    groupBox->setTitle(QApplication::translate("dlgWorkspaceDisplay", "Information", 0, QApplication::UnicodeUTF8));
    btnEdit->setText(QApplication::translate("dlgWorkspaceDisplay", "Edit Memory", 0, QApplication::UnicodeUTF8));
    btnImport->setText(QApplication::translate("dlgWorkspaceDisplay", "Import...", 0, QApplication::UnicodeUTF8));
    btnExport->setText(QApplication::translate("dlgWorkspaceDisplay", "Export...", 0, QApplication::UnicodeUTF8));
    groupBox_2->setTitle(QApplication::translate("dlgWorkspaceDisplay", "Workspace Tree", 0, QApplication::UnicodeUTF8));
    okButton->setText(QApplication::translate("dlgWorkspaceDisplay", "OK", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(dlgWorkspaceDisplay);
    } // retranslateUi

};

namespace Ui {
    class dlgWorkspaceDisplay: public Ui_dlgWorkspaceDisplay {};
} // namespace Ui

#endif // UI_WORKSPACE_H
