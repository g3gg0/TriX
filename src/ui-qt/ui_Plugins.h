/********************************************************************************
** Form generated from reading ui file 'Plugins.ui'
**
** Created: Thu 21. Feb 15:53:13 2008
**      by: Qt User Interface Compiler version 4.3.2
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_PLUGINS_H
#define UI_PLUGINS_H

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

class Ui_dlgPlugins
{
public:
    QVBoxLayout *vboxLayout;
    QHBoxLayout *hboxLayout;
    QGroupBox *groupBox;
    QHBoxLayout *hboxLayout1;
    QTreeWidget *treeWidget;
    QGroupBox *groupBox_2;
    QHBoxLayout *hboxLayout2;
    QTextEdit *txtDetails;
    QHBoxLayout *hboxLayout3;
    QSpacerItem *spacerItem;
    QPushButton *okButton;
    QPushButton *btnUnload;

    void setupUi(QDialog *dlgPlugins)
    {
    if (dlgPlugins->objectName().isEmpty())
        dlgPlugins->setObjectName(QString::fromUtf8("dlgPlugins"));
    dlgPlugins->resize(478, 312);
    dlgPlugins->setWindowIcon(QIcon(QString::fromUtf8(":/icons/icons/22x22-gartoon/modules.png")));
    vboxLayout = new QVBoxLayout(dlgPlugins);
#ifndef Q_OS_MAC
    vboxLayout->setSpacing(6);
#endif
#ifndef Q_OS_MAC
    vboxLayout->setMargin(9);
#endif
    vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
    hboxLayout = new QHBoxLayout();
#ifndef Q_OS_MAC
    hboxLayout->setSpacing(6);
#endif
#ifndef Q_OS_MAC
    hboxLayout->setMargin(0);
#endif
    hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
    groupBox = new QGroupBox(dlgPlugins);
    groupBox->setObjectName(QString::fromUtf8("groupBox"));
    hboxLayout1 = new QHBoxLayout(groupBox);
#ifndef Q_OS_MAC
    hboxLayout1->setSpacing(6);
#endif
#ifndef Q_OS_MAC
    hboxLayout1->setMargin(9);
#endif
    hboxLayout1->setObjectName(QString::fromUtf8("hboxLayout1"));
    treeWidget = new QTreeWidget(groupBox);
    treeWidget->setObjectName(QString::fromUtf8("treeWidget"));
    treeWidget->setAlternatingRowColors(true);
    treeWidget->setItemsExpandable(false);

    hboxLayout1->addWidget(treeWidget);


    hboxLayout->addWidget(groupBox);

    groupBox_2 = new QGroupBox(dlgPlugins);
    groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
    hboxLayout2 = new QHBoxLayout(groupBox_2);
#ifndef Q_OS_MAC
    hboxLayout2->setSpacing(6);
#endif
#ifndef Q_OS_MAC
    hboxLayout2->setMargin(9);
#endif
    hboxLayout2->setObjectName(QString::fromUtf8("hboxLayout2"));
    txtDetails = new QTextEdit(groupBox_2);
    txtDetails->setObjectName(QString::fromUtf8("txtDetails"));
    txtDetails->setEnabled(true);
    txtDetails->setReadOnly(true);

    hboxLayout2->addWidget(txtDetails);


    hboxLayout->addWidget(groupBox_2);


    vboxLayout->addLayout(hboxLayout);

    hboxLayout3 = new QHBoxLayout();
#ifndef Q_OS_MAC
    hboxLayout3->setSpacing(6);
#endif
    hboxLayout3->setMargin(0);
    hboxLayout3->setObjectName(QString::fromUtf8("hboxLayout3"));
    spacerItem = new QSpacerItem(131, 31, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hboxLayout3->addItem(spacerItem);

    okButton = new QPushButton(dlgPlugins);
    okButton->setObjectName(QString::fromUtf8("okButton"));

    hboxLayout3->addWidget(okButton);

    btnUnload = new QPushButton(dlgPlugins);
    btnUnload->setObjectName(QString::fromUtf8("btnUnload"));
    btnUnload->setEnabled(false);

    hboxLayout3->addWidget(btnUnload);


    vboxLayout->addLayout(hboxLayout3);


    retranslateUi(dlgPlugins);
    QObject::connect(okButton, SIGNAL(clicked()), dlgPlugins, SLOT(accept()));

    QMetaObject::connectSlotsByName(dlgPlugins);
    } // setupUi

    void retranslateUi(QDialog *dlgPlugins)
    {
    dlgPlugins->setWindowTitle(QApplication::translate("dlgPlugins", "Plugins", 0, QApplication::UnicodeUTF8));
    groupBox->setTitle(QApplication::translate("dlgPlugins", "Plugins", 0, QApplication::UnicodeUTF8));
    groupBox_2->setTitle(QApplication::translate("dlgPlugins", "Details", 0, QApplication::UnicodeUTF8));
    okButton->setText(QApplication::translate("dlgPlugins", "Close", 0, QApplication::UnicodeUTF8));
    btnUnload->setText(QApplication::translate("dlgPlugins", "Unload", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(dlgPlugins);
    } // retranslateUi

};

namespace Ui {
    class dlgPlugins: public Ui_dlgPlugins {};
} // namespace Ui

#endif // UI_PLUGINS_H
