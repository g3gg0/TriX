/****************************************************************************
** Meta object code from reading C++ file 'ui_memviewer_dlg.h'
**
** Created: Fri 7. Mar 21:45:30 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.3.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "ui_memviewer_dlg.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ui_memviewer_dlg.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.3.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_MemoryViewer[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      14,   13,   13,   13, 0x08,
      23,   13,   13,   13, 0x08,
      40,   13,   13,   13, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_MemoryViewer[] = {
    "MemoryViewer\0\0update()\0btnGotoPressed()\0"
    "btnOkPressed()\0"
};

const QMetaObject MemoryViewer::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_MemoryViewer,
      qt_meta_data_MemoryViewer, 0 }
};

const QMetaObject *MemoryViewer::metaObject() const
{
    return &staticMetaObject;
}

void *MemoryViewer::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MemoryViewer))
	return static_cast<void*>(const_cast< MemoryViewer*>(this));
    return QDialog::qt_metacast(_clname);
}

int MemoryViewer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: update(); break;
        case 1: btnGotoPressed(); break;
        case 2: btnOkPressed(); break;
        }
        _id -= 3;
    }
    return _id;
}
