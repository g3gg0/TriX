/********************************************************************************
** Form generated from reading ui file 'ObjectTree.ui'
**
** Created: Sun 3. Aug 23:16:31 2014
**      by: Qt User Interface Compiler version 4.3.2
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_OBJECTTREE_H
#define UI_OBJECTTREE_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QTextEdit>
#include <QtGui/QTreeWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

class Ui_dlgObjectTreeDisplay
{
public:
    QWidget *verticalLayout;
    QVBoxLayout *vboxLayout;
    QHBoxLayout *hboxLayout;
    QGroupBox *ObjectTree;
    QTreeWidget *treeObjects;
    QGroupBox *Description;
    QTextEdit *textEdit;
    QHBoxLayout *hboxLayout1;
    QSpacerItem *spacerItem;
    QPushButton *okButton;

    void setupUi(QDialog *dlgObjectTreeDisplay)
    {
    if (dlgObjectTreeDisplay->objectName().isEmpty())
        dlgObjectTreeDisplay->setObjectName(QString::fromUtf8("dlgObjectTreeDisplay"));
    dlgObjectTreeDisplay->resize(584, 573);
    dlgObjectTreeDisplay->setWindowIcon(QIcon(QString::fromUtf8(":/icons/icons/22x22-gartoon/viewer.png")));
    verticalLayout = new QWidget(dlgObjectTreeDisplay);
    verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
    verticalLayout->setGeometry(QRect(0, 0, 581, 571));
    vboxLayout = new QVBoxLayout(verticalLayout);
    vboxLayout->setSpacing(0);
    vboxLayout->setMargin(0);
    vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
    vboxLayout->setContentsMargins(0, 0, 0, 0);
    hboxLayout = new QHBoxLayout();
    hboxLayout->setSpacing(0);
#ifndef Q_OS_MAC
    hboxLayout->setMargin(0);
#endif
    hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
    ObjectTree = new QGroupBox(verticalLayout);
    ObjectTree->setObjectName(QString::fromUtf8("ObjectTree"));
    treeObjects = new QTreeWidget(ObjectTree);
    treeObjects->setObjectName(QString::fromUtf8("treeObjects"));
    treeObjects->setGeometry(QRect(10, 20, 271, 501));

    hboxLayout->addWidget(ObjectTree);

    Description = new QGroupBox(verticalLayout);
    Description->setObjectName(QString::fromUtf8("Description"));
    textEdit = new QTextEdit(Description);
    textEdit->setObjectName(QString::fromUtf8("textEdit"));
    textEdit->setEnabled(false);
    textEdit->setGeometry(QRect(10, 20, 271, 501));

    hboxLayout->addWidget(Description);


    vboxLayout->addLayout(hboxLayout);

    hboxLayout1 = new QHBoxLayout();
#ifndef Q_OS_MAC
    hboxLayout1->setSpacing(6);
#endif
    hboxLayout1->setMargin(0);
    hboxLayout1->setObjectName(QString::fromUtf8("hboxLayout1"));
    spacerItem = new QSpacerItem(131, 31, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hboxLayout1->addItem(spacerItem);

    okButton = new QPushButton(verticalLayout);
    okButton->setObjectName(QString::fromUtf8("okButton"));

    hboxLayout1->addWidget(okButton);


    vboxLayout->addLayout(hboxLayout1);


    retranslateUi(dlgObjectTreeDisplay);
    QObject::connect(okButton, SIGNAL(clicked()), dlgObjectTreeDisplay, SLOT(accept()));

    QMetaObject::connectSlotsByName(dlgObjectTreeDisplay);
    } // setupUi

    void retranslateUi(QDialog *dlgObjectTreeDisplay)
    {
    dlgObjectTreeDisplay->setWindowTitle(QApplication::translate("dlgObjectTreeDisplay", "Object Tree", 0, QApplication::UnicodeUTF8));
    ObjectTree->setTitle(QApplication::translate("dlgObjectTreeDisplay", "ObjectTree", 0, QApplication::UnicodeUTF8));
    Description->setTitle(QApplication::translate("dlgObjectTreeDisplay", "Description", 0, QApplication::UnicodeUTF8));
    okButton->setText(QApplication::translate("dlgObjectTreeDisplay", "OK", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(dlgObjectTreeDisplay);
    } // retranslateUi

};

namespace Ui {
    class dlgObjectTreeDisplay: public Ui_dlgObjectTreeDisplay {};
} // namespace Ui

#endif // UI_OBJECTTREE_H
