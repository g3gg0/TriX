/********************************************************************************
** Form generated from reading ui file 'BuildPPMProgress.ui'
**
** Created: Thu 21. Feb 15:53:13 2008
**      by: Qt User Interface Compiler version 4.3.2
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_BUILDPPMPROGRESS_H
#define UI_BUILDPPMPROGRESS_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QProgressBar>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QWidget>

class Ui_BuildDialog
{
public:
    QWidget *layoutWidget;
    QHBoxLayout *hboxLayout;
    QSpacerItem *spacerItem;
    QPushButton *cancelButton;
    QLabel *progressLabel;
    QProgressBar *progressBar;

    void setupUi(QDialog *BuildDialog)
    {
    if (BuildDialog->objectName().isEmpty())
        BuildDialog->setObjectName(QString::fromUtf8("BuildDialog"));
    BuildDialog->resize(370, 106);
    layoutWidget = new QWidget(BuildDialog);
    layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
    layoutWidget->setGeometry(QRect(10, 70, 351, 33));
    hboxLayout = new QHBoxLayout(layoutWidget);
#ifndef Q_OS_MAC
    hboxLayout->setSpacing(6);
#endif
    hboxLayout->setMargin(0);
    hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
    hboxLayout->setContentsMargins(0, 0, 0, 0);
    spacerItem = new QSpacerItem(131, 31, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hboxLayout->addItem(spacerItem);

    cancelButton = new QPushButton(layoutWidget);
    cancelButton->setObjectName(QString::fromUtf8("cancelButton"));

    hboxLayout->addWidget(cancelButton);

    progressLabel = new QLabel(BuildDialog);
    progressLabel->setObjectName(QString::fromUtf8("progressLabel"));
    progressLabel->setGeometry(QRect(30, 10, 291, 20));
    QFont font;
    font.setFamily(QString::fromUtf8("MS Shell Dlg 2"));
    font.setPointSize(12);
    font.setBold(false);
    font.setItalic(false);
    font.setUnderline(false);
    font.setWeight(50);
    font.setStrikeOut(false);
    progressLabel->setFont(font);
    progressBar = new QProgressBar(BuildDialog);
    progressBar->setObjectName(QString::fromUtf8("progressBar"));
    progressBar->setGeometry(QRect(30, 40, 331, 23));
    progressBar->setValue(24);
    progressBar->setOrientation(Qt::Horizontal);

    retranslateUi(BuildDialog);
    QObject::connect(cancelButton, SIGNAL(clicked()), BuildDialog, SLOT(reject()));

    QMetaObject::connectSlotsByName(BuildDialog);
    } // setupUi

    void retranslateUi(QDialog *BuildDialog)
    {
    BuildDialog->setWindowTitle(QApplication::translate("BuildDialog", "Building PPM", 0, QApplication::UnicodeUTF8));
    cancelButton->setText(QApplication::translate("BuildDialog", "Abort", 0, QApplication::UnicodeUTF8));
    progressLabel->setText(QApplication::translate("BuildDialog", "Unknown State", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(BuildDialog);
    } // retranslateUi

};

namespace Ui {
    class BuildDialog: public Ui_BuildDialog {};
} // namespace Ui

#endif // UI_BUILDPPMPROGRESS_H
