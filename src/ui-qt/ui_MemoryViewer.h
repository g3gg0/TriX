/********************************************************************************
** Form generated from reading ui file 'MemoryViewer.ui'
**
** Created: Thu 21. Feb 15:53:13 2008
**      by: Qt User Interface Compiler version 4.3.2
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_MEMORYVIEWER_H
#define UI_MEMORYVIEWER_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QScrollBar>
#include <QtGui/QSpacerItem>
#include <QtGui/QTextBrowser>
#include <QtGui/QVBoxLayout>

class Ui_dlgMemView
{
public:
    QHBoxLayout *hboxLayout;
    QVBoxLayout *vboxLayout;
    QHBoxLayout *hboxLayout1;
    QGroupBox *groupBox;
    QHBoxLayout *hboxLayout2;
    QTextBrowser *txtBox;
    QScrollBar *verticalScrollBar;
    QHBoxLayout *hboxLayout3;
    QSpacerItem *spacerItem;
    QLineEdit *txtAddress;
    QPushButton *btnGoto;
    QSpacerItem *spacerItem1;
    QPushButton *okButton;

    void setupUi(QDialog *dlgMemView)
    {
    if (dlgMemView->objectName().isEmpty())
        dlgMemView->setObjectName(QString::fromUtf8("dlgMemView"));
    dlgMemView->resize(680, 663);
    hboxLayout = new QHBoxLayout(dlgMemView);
#ifndef Q_OS_MAC
    hboxLayout->setSpacing(6);
#endif
#ifndef Q_OS_MAC
    hboxLayout->setMargin(9);
#endif
    hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
    vboxLayout = new QVBoxLayout();
#ifndef Q_OS_MAC
    vboxLayout->setSpacing(6);
#endif
#ifndef Q_OS_MAC
    vboxLayout->setMargin(0);
#endif
    vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
    hboxLayout1 = new QHBoxLayout();
#ifndef Q_OS_MAC
    hboxLayout1->setSpacing(6);
#endif
#ifndef Q_OS_MAC
    hboxLayout1->setMargin(0);
#endif
    hboxLayout1->setObjectName(QString::fromUtf8("hboxLayout1"));
    groupBox = new QGroupBox(dlgMemView);
    groupBox->setObjectName(QString::fromUtf8("groupBox"));
    hboxLayout2 = new QHBoxLayout(groupBox);
#ifndef Q_OS_MAC
    hboxLayout2->setSpacing(6);
#endif
#ifndef Q_OS_MAC
    hboxLayout2->setMargin(9);
#endif
    hboxLayout2->setObjectName(QString::fromUtf8("hboxLayout2"));
    txtBox = new QTextBrowser(groupBox);
    txtBox->setObjectName(QString::fromUtf8("txtBox"));
    QFont font;
    font.setFamily(QString::fromUtf8("Courier New"));
    font.setPointSize(10);
    font.setBold(false);
    font.setItalic(false);
    font.setUnderline(false);
    font.setWeight(50);
    font.setStrikeOut(false);
    txtBox->setFont(font);

    hboxLayout2->addWidget(txtBox);

    verticalScrollBar = new QScrollBar(groupBox);
    verticalScrollBar->setObjectName(QString::fromUtf8("verticalScrollBar"));
    verticalScrollBar->setOrientation(Qt::Vertical);

    hboxLayout2->addWidget(verticalScrollBar);


    hboxLayout1->addWidget(groupBox);


    vboxLayout->addLayout(hboxLayout1);

    hboxLayout3 = new QHBoxLayout();
#ifndef Q_OS_MAC
    hboxLayout3->setSpacing(6);
#endif
    hboxLayout3->setMargin(0);
    hboxLayout3->setObjectName(QString::fromUtf8("hboxLayout3"));
    spacerItem = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hboxLayout3->addItem(spacerItem);

    txtAddress = new QLineEdit(dlgMemView);
    txtAddress->setObjectName(QString::fromUtf8("txtAddress"));
    QSizePolicy sizePolicy(static_cast<QSizePolicy::Policy>(0), static_cast<QSizePolicy::Policy>(0));
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(txtAddress->sizePolicy().hasHeightForWidth());
    txtAddress->setSizePolicy(sizePolicy);

    hboxLayout3->addWidget(txtAddress);

    btnGoto = new QPushButton(dlgMemView);
    btnGoto->setObjectName(QString::fromUtf8("btnGoto"));
    btnGoto->setDefault(true);

    hboxLayout3->addWidget(btnGoto);

    spacerItem1 = new QSpacerItem(131, 31, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hboxLayout3->addItem(spacerItem1);

    okButton = new QPushButton(dlgMemView);
    okButton->setObjectName(QString::fromUtf8("okButton"));

    hboxLayout3->addWidget(okButton);


    vboxLayout->addLayout(hboxLayout3);


    hboxLayout->addLayout(vboxLayout);


    retranslateUi(dlgMemView);
    QObject::connect(okButton, SIGNAL(clicked()), dlgMemView, SLOT(accept()));

    QMetaObject::connectSlotsByName(dlgMemView);
    } // setupUi

    void retranslateUi(QDialog *dlgMemView)
    {
    dlgMemView->setWindowTitle(QApplication::translate("dlgMemView", "Memory Viewer", 0, QApplication::UnicodeUTF8));
    groupBox->setTitle(QApplication::translate("dlgMemView", "GroupBox", 0, QApplication::UnicodeUTF8));
    btnGoto->setText(QApplication::translate("dlgMemView", "Goto Address", 0, QApplication::UnicodeUTF8));
    okButton->setText(QApplication::translate("dlgMemView", "OK", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(dlgMemView);
    } // retranslateUi

};

namespace Ui {
    class dlgMemView: public Ui_dlgMemView {};
} // namespace Ui

#endif // UI_MEMORYVIEWER_H
