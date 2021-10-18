/********************************************************************************
** Form generated from reading ui file 'TextEdit.ui'
**
** Created: Wed 21. Aug 22:19:57 2013
**      by: Qt User Interface Compiler version 4.5.1
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_TEXTEDIT_H
#define UI_TEXTEDIT_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QTableWidget>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_dlgTextEdit
{
public:
    QVBoxLayout *vboxLayout;
    QTableWidget *tableWidget;
    QHBoxLayout *hboxLayout;
    QSpacerItem *spacerItem;
    QPushButton *okButton;
    QPushButton *cancelButton;

    void setupUi(QDialog *dlgTextEdit)
    {
        if (dlgTextEdit->objectName().isEmpty())
            dlgTextEdit->setObjectName(QString::fromUtf8("dlgTextEdit"));
        dlgTextEdit->resize(691, 678);
        dlgTextEdit->setModal(true);
        vboxLayout = new QVBoxLayout(dlgTextEdit);
#ifndef Q_OS_MAC
        vboxLayout->setSpacing(6);
#endif
#ifndef Q_OS_MAC
        vboxLayout->setMargin(9);
#endif
        vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
        tableWidget = new QTableWidget(dlgTextEdit);
        tableWidget->setObjectName(QString::fromUtf8("tableWidget"));

        vboxLayout->addWidget(tableWidget);

        hboxLayout = new QHBoxLayout();
#ifndef Q_OS_MAC
        hboxLayout->setSpacing(6);
#endif
        hboxLayout->setMargin(0);
        hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
        spacerItem = new QSpacerItem(131, 31, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hboxLayout->addItem(spacerItem);

        okButton = new QPushButton(dlgTextEdit);
        okButton->setObjectName(QString::fromUtf8("okButton"));

        hboxLayout->addWidget(okButton);

        cancelButton = new QPushButton(dlgTextEdit);
        cancelButton->setObjectName(QString::fromUtf8("cancelButton"));

        hboxLayout->addWidget(cancelButton);


        vboxLayout->addLayout(hboxLayout);


        retranslateUi(dlgTextEdit);
        QObject::connect(okButton, SIGNAL(clicked()), dlgTextEdit, SLOT(accept()));
        QObject::connect(cancelButton, SIGNAL(clicked()), dlgTextEdit, SLOT(reject()));

        QMetaObject::connectSlotsByName(dlgTextEdit);
    } // setupUi

    void retranslateUi(QDialog *dlgTextEdit)
    {
        dlgTextEdit->setWindowTitle(QApplication::translate("dlgTextEdit", "Text Edit", 0, QApplication::UnicodeUTF8));
        okButton->setText(QApplication::translate("dlgTextEdit", "OK", 0, QApplication::UnicodeUTF8));
        cancelButton->setText(QApplication::translate("dlgTextEdit", "Cancel", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(dlgTextEdit);
    } // retranslateUi

};

namespace Ui {
    class dlgTextEdit: public Ui_dlgTextEdit {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TEXTEDIT_H
