/********************************************************************************
** Form generated from reading ui file 'TextBox.ui'
**
** Created: Sun 3. Aug 23:16:31 2014
**      by: Qt User Interface Compiler version 4.3.2
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_TEXTBOX_H
#define UI_TEXTBOX_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QTextEdit>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

class Ui_dlgTextBox
{
public:
    QVBoxLayout *vboxLayout;
    QTextEdit *textEdit;

    void setupUi(QWidget *dlgTextBox)
    {
    if (dlgTextBox->objectName().isEmpty())
        dlgTextBox->setObjectName(QString::fromUtf8("dlgTextBox"));
    dlgTextBox->resize(400, 126);
    dlgTextBox->setContextMenuPolicy(Qt::NoContextMenu);
    dlgTextBox->setWindowIcon(QIcon(QString::fromUtf8(":/icons/icons/22x22-gartoon/amusement.png")));
    vboxLayout = new QVBoxLayout(dlgTextBox);
    vboxLayout->setSpacing(0);
    vboxLayout->setMargin(0);
    vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
    textEdit = new QTextEdit(dlgTextBox);
    textEdit->setObjectName(QString::fromUtf8("textEdit"));
    QFont font;
    font.setFamily(QString::fromUtf8("Lucida Console"));
    font.setPointSize(8);
    font.setBold(false);
    font.setItalic(false);
    font.setUnderline(false);
    font.setWeight(50);
    font.setStrikeOut(false);
    textEdit->setFont(font);

    vboxLayout->addWidget(textEdit);


    retranslateUi(dlgTextBox);

    QMetaObject::connectSlotsByName(dlgTextBox);
    } // setupUi

    void retranslateUi(QWidget *dlgTextBox)
    {
    dlgTextBox->setWindowTitle(QApplication::translate("dlgTextBox", "TextBox", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(dlgTextBox);
    } // retranslateUi

};

namespace Ui {
    class dlgTextBox: public Ui_dlgTextBox {};
} // namespace Ui

#endif // UI_TEXTBOX_H
