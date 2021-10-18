/****************************************************************************
** Meta object code from reading C++ file 'ui_main.h'
**
** Created: Mo 3. Jul 13:22:55 2006
**      by: The Qt Meta Object Compiler version 59 (Qt 4.1.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/ui-qt/ui_main.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ui_main.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.1.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_TriXMainWindow[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
      18,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      20,   16,   15,   15, 0x08,
      54,   46,   15,   15, 0x08,
      86,   46,   15,   15, 0x08,
     113,   15,   15,   15, 0x08,
     136,   15,   15,   15, 0x08,
     159,   15,   15,   15, 0x08,
     183,   15,   15,   15, 0x08,
     208,   15,   15,   15, 0x08,
     233,   15,   15,   15, 0x08,
     258,   15,   15,   15, 0x08,
     284,   15,   15,   15, 0x08,
     310,   15,   15,   15, 0x08,
     335,   15,   15,   15, 0x08,
     360,   15,   15,   15, 0x08,
     386,   15,   15,   15, 0x08,
     412,   15,   15,   15, 0x08,
     430,   15,   15,   15, 0x08,
     449,   15,   15,   15, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_TriXMainWindow[] = {
    "TriXMainWindow\0\0Pos\0cmbFileTypeActivated(int)\0Checked\0"
    "boxDCT3SeparatePPMToggled(bool)\0radioModeListToggled(bool)\0"
    "btnEditImportClicked()\0btnEditExportClicked()\0btnSimpleInputClicked()\0"
    "btnSimpleOutputClicked()\0btnDCT3InputMCUClicked()\0"
    "btnDCT3InputPPMClicked()\0btnDCT3OutputMCUClicked()\0"
    "btnDCT3OutputPPMClicked()\0btnDCT4InputMCUClicked()\0"
    "btnDCT4InputPPMClicked()\0btnDCT4OutputMCUClicked()\0"
    "btnDCT4OutputPPMClicked()\0actNewTriggered()\0actQuitTriggered()\0"
    "actStartTriggered()\0"
};

const QMetaObject TriXMainWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_TriXMainWindow,
      qt_meta_data_TriXMainWindow, 0 }
};

const QMetaObject *TriXMainWindow::metaObject() const
{
    return &staticMetaObject;
}

void *TriXMainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_TriXMainWindow))
	return static_cast<void*>(const_cast<TriXMainWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int TriXMainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: cmbFileTypeActivated((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: boxDCT3SeparatePPMToggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: radioModeListToggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: btnEditImportClicked(); break;
        case 4: btnEditExportClicked(); break;
        case 5: btnSimpleInputClicked(); break;
        case 6: btnSimpleOutputClicked(); break;
        case 7: btnDCT3InputMCUClicked(); break;
        case 8: btnDCT3InputPPMClicked(); break;
        case 9: btnDCT3OutputMCUClicked(); break;
        case 10: btnDCT3OutputPPMClicked(); break;
        case 11: btnDCT4InputMCUClicked(); break;
        case 12: btnDCT4InputPPMClicked(); break;
        case 13: btnDCT4OutputMCUClicked(); break;
        case 14: btnDCT4OutputPPMClicked(); break;
        case 15: actNewTriggered(); break;
        case 16: actQuitTriggered(); break;
        case 17: actStartTriggered(); break;
        }
        _id -= 18;
    }
    return _id;
}
