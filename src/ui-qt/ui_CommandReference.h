/********************************************************************************
** Form generated from reading ui file 'CommandReference.ui'
**
** Created: Sun 3. Aug 23:16:31 2014
**      by: Qt User Interface Compiler version 4.3.2
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_COMMANDREFERENCE_H
#define UI_COMMANDREFERENCE_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QDialog>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QTextBrowser>
#include <QtGui/QTextEdit>
#include <QtGui/QTreeWidget>
#include <QtGui/QWidget>

class Ui_dlgCommandReference
{
public:
    QGroupBox *groupBox_2;
    QTextBrowser *textBrowser;
    QTextEdit *textEdit;
    QWidget *layoutWidget;
    QHBoxLayout *hboxLayout;
    QSpacerItem *spacerItem;
    QPushButton *okButton;
    QGroupBox *groupBox;
    QTreeWidget *treeWidget;
    QCheckBox *chkPlain;

    void setupUi(QDialog *dlgCommandReference)
    {
    if (dlgCommandReference->objectName().isEmpty())
        dlgCommandReference->setObjectName(QString::fromUtf8("dlgCommandReference"));
    dlgCommandReference->resize(830, 589);
    dlgCommandReference->setContextMenuPolicy(Qt::NoContextMenu);
    dlgCommandReference->setWindowIcon(QIcon(QString::fromUtf8(":/icons/icons/22x22-gartoon/ghostview.png")));
    dlgCommandReference->setModal(false);
    groupBox_2 = new QGroupBox(dlgCommandReference);
    groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
    groupBox_2->setGeometry(QRect(150, 10, 671, 531));
    textBrowser = new QTextBrowser(groupBox_2);
    textBrowser->setObjectName(QString::fromUtf8("textBrowser"));
    textBrowser->setGeometry(QRect(10, 20, 651, 501));
    textEdit = new QTextEdit(groupBox_2);
    textEdit->setObjectName(QString::fromUtf8("textEdit"));
    textEdit->setEnabled(true);
    textEdit->setGeometry(QRect(10, 20, 651, 501));
    QSizePolicy sizePolicy(static_cast<QSizePolicy::Policy>(7), static_cast<QSizePolicy::Policy>(7));
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(textEdit->sizePolicy().hasHeightForWidth());
    textEdit->setSizePolicy(sizePolicy);
    textEdit->setAutoFillBackground(true);
    textEdit->setLineWrapMode(QTextEdit::NoWrap);
    layoutWidget = new QWidget(dlgCommandReference);
    layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
    layoutWidget->setGeometry(QRect(0, 550, 821, 33));
    hboxLayout = new QHBoxLayout(layoutWidget);
#ifndef Q_OS_MAC
    hboxLayout->setSpacing(6);
#endif
    hboxLayout->setMargin(0);
    hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
    hboxLayout->setContentsMargins(0, 0, 0, 0);
    spacerItem = new QSpacerItem(131, 31, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hboxLayout->addItem(spacerItem);

    okButton = new QPushButton(layoutWidget);
    okButton->setObjectName(QString::fromUtf8("okButton"));

    hboxLayout->addWidget(okButton);

    groupBox = new QGroupBox(dlgCommandReference);
    groupBox->setObjectName(QString::fromUtf8("groupBox"));
    groupBox->setGeometry(QRect(10, 10, 141, 531));
    treeWidget = new QTreeWidget(groupBox);
    treeWidget->setObjectName(QString::fromUtf8("treeWidget"));
    treeWidget->setGeometry(QRect(10, 19, 120, 471));
    chkPlain = new QCheckBox(groupBox);
    chkPlain->setObjectName(QString::fromUtf8("chkPlain"));
    chkPlain->setGeometry(QRect(10, 500, 111, 18));

    retranslateUi(dlgCommandReference);

    QMetaObject::connectSlotsByName(dlgCommandReference);
    } // setupUi

    void retranslateUi(QDialog *dlgCommandReference)
    {
    dlgCommandReference->setWindowTitle(QApplication::translate("dlgCommandReference", "Command Reference", 0, QApplication::UnicodeUTF8));
    groupBox_2->setTitle(QApplication::translate("dlgCommandReference", "Headers", 0, QApplication::UnicodeUTF8));
    okButton->setText(QApplication::translate("dlgCommandReference", "OK", 0, QApplication::UnicodeUTF8));
    groupBox->setTitle(QApplication::translate("dlgCommandReference", "Engine", 0, QApplication::UnicodeUTF8));
    chkPlain->setText(QApplication::translate("dlgCommandReference", "PlainText Headers", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(dlgCommandReference);
    } // retranslateUi

};

namespace Ui {
    class dlgCommandReference: public Ui_dlgCommandReference {};
} // namespace Ui

#endif // UI_COMMANDREFERENCE_H
