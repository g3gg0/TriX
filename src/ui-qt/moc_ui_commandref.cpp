/****************************************************************************
** Meta object code from reading C++ file 'ui_commandref.h'
**
** Created: Sun 3. Aug 23:18:29 2014
**      by: The Qt Meta Object Compiler version 59 (Qt 4.3.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "ui_commandref.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ui_commandref.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.3.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_CommandReference[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      18,   17,   17,   17, 0x08,
      44,   38,   17,   17, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_CommandReference[] = {
    "CommandReference\0\0treeWidgetChanged()\0"
    "state\0plainStateChanged(int)\0"
};

const QMetaObject CommandReference::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_CommandReference,
      qt_meta_data_CommandReference, 0 }
};

const QMetaObject *CommandReference::metaObject() const
{
    return &staticMetaObject;
}

void *CommandReference::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CommandReference))
	return static_cast<void*>(const_cast< CommandReference*>(this));
    return QDialog::qt_metacast(_clname);
}

int CommandReference::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: treeWidgetChanged(); break;
        case 1: plainStateChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        }
        _id -= 2;
    }
    return _id;
}
