/********************************************************************************
** Form generated from reading ui file 'options.ui'
**
** Created: Sun 3. Aug 23:16:31 2014
**      by: Qt User Interface Compiler version 4.3.2
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_OPTIONS_H
#define UI_OPTIONS_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QTreeWidget>
#include <QtGui/QVBoxLayout>

class Ui_dlgOptions
{
public:
    QVBoxLayout *vboxLayout;
    QTreeWidget *treeWidget_options;
    QHBoxLayout *hboxLayout;
    QLineEdit *lineEdit_current_option;
    QPushButton *pushButton_default;
    QHBoxLayout *hboxLayout1;
    QPushButton *pushButton_cancel;
    QSpacerItem *spacerItem;
    QPushButton *pushButton_Apply;
    QPushButton *pushButton_Ok;

    void setupUi(QDialog *dlgOptions)
    {
    if (dlgOptions->objectName().isEmpty())
        dlgOptions->setObjectName(QString::fromUtf8("dlgOptions"));
    dlgOptions->resize(663, 472);
    dlgOptions->setWindowIcon(QIcon(QString::fromUtf8(":/icons/icons/22x22-gartoon/editor.png")));
    vboxLayout = new QVBoxLayout(dlgOptions);
#ifndef Q_OS_MAC
    vboxLayout->setSpacing(6);
#endif
#ifndef Q_OS_MAC
    vboxLayout->setMargin(9);
#endif
    vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
    treeWidget_options = new QTreeWidget(dlgOptions);
    treeWidget_options->setObjectName(QString::fromUtf8("treeWidget_options"));
    treeWidget_options->setAlternatingRowColors(true);
    treeWidget_options->setColumnCount(5);

    vboxLayout->addWidget(treeWidget_options);

    hboxLayout = new QHBoxLayout();
#ifndef Q_OS_MAC
    hboxLayout->setSpacing(6);
#endif
    hboxLayout->setMargin(0);
    hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
    lineEdit_current_option = new QLineEdit(dlgOptions);
    lineEdit_current_option->setObjectName(QString::fromUtf8("lineEdit_current_option"));

    hboxLayout->addWidget(lineEdit_current_option);

    pushButton_default = new QPushButton(dlgOptions);
    pushButton_default->setObjectName(QString::fromUtf8("pushButton_default"));
    pushButton_default->setEnabled(true);
    pushButton_default->setAutoDefault(false);

    hboxLayout->addWidget(pushButton_default);


    vboxLayout->addLayout(hboxLayout);

    hboxLayout1 = new QHBoxLayout();
#ifndef Q_OS_MAC
    hboxLayout1->setSpacing(6);
#endif
    hboxLayout1->setMargin(0);
    hboxLayout1->setObjectName(QString::fromUtf8("hboxLayout1"));
    pushButton_cancel = new QPushButton(dlgOptions);
    pushButton_cancel->setObjectName(QString::fromUtf8("pushButton_cancel"));
    pushButton_cancel->setAutoDefault(false);
    pushButton_cancel->setDefault(false);

    hboxLayout1->addWidget(pushButton_cancel);

    spacerItem = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hboxLayout1->addItem(spacerItem);

    pushButton_Apply = new QPushButton(dlgOptions);
    pushButton_Apply->setObjectName(QString::fromUtf8("pushButton_Apply"));
    pushButton_Apply->setAutoDefault(true);
    pushButton_Apply->setDefault(true);

    hboxLayout1->addWidget(pushButton_Apply);

    pushButton_Ok = new QPushButton(dlgOptions);
    pushButton_Ok->setObjectName(QString::fromUtf8("pushButton_Ok"));
    pushButton_Ok->setAutoDefault(false);
    pushButton_Ok->setDefault(false);

    hboxLayout1->addWidget(pushButton_Ok);


    vboxLayout->addLayout(hboxLayout1);


    retranslateUi(dlgOptions);

    QMetaObject::connectSlotsByName(dlgOptions);
    } // setupUi

    void retranslateUi(QDialog *dlgOptions)
    {
    dlgOptions->setWindowTitle(QApplication::translate("dlgOptions", "Options", 0, QApplication::UnicodeUTF8));
    treeWidget_options->headerItem()->setText(0, QApplication::translate("dlgOptions", "option", 0, QApplication::UnicodeUTF8));
    treeWidget_options->headerItem()->setText(1, QApplication::translate("dlgOptions", "type", 0, QApplication::UnicodeUTF8));
    treeWidget_options->headerItem()->setText(2, QApplication::translate("dlgOptions", "value", 0, QApplication::UnicodeUTF8));
    treeWidget_options->headerItem()->setText(3, QApplication::translate("dlgOptions", "chgd", 0, QApplication::UnicodeUTF8));
    treeWidget_options->headerItem()->setText(4, QApplication::translate("dlgOptions", "description", 0, QApplication::UnicodeUTF8));
    pushButton_default->setText(QApplication::translate("dlgOptions", "set default", 0, QApplication::UnicodeUTF8));
    pushButton_cancel->setText(QApplication::translate("dlgOptions", "Cancel", 0, QApplication::UnicodeUTF8));
    pushButton_Apply->setText(QApplication::translate("dlgOptions", "Apply", 0, QApplication::UnicodeUTF8));
    pushButton_Ok->setText(QApplication::translate("dlgOptions", "OK", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(dlgOptions);
    } // retranslateUi

};

namespace Ui {
    class dlgOptions: public Ui_dlgOptions {};
} // namespace Ui

#endif // UI_OPTIONS_H
