/****************************************************************************
** Meta object code from reading C++ file 'ui_opt.h'
**
** Created: Thu 21. Feb 15:53:14 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.3.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "ui_opt.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ui_opt.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.3.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_options[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      18,    9,    8,    8, 0x08,
      57,   52,    8,    8, 0x08,
      79,    8,    8,    8, 0x08,
      86,    8,    8,    8, 0x08,
     103,    8,    8,    8, 0x08,
     120,    8,    8,    8, 0x08,
     136,    8,    8,    8, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_options[] = {
    "options\0\0item,col\0sel_changed(QTreeWidgetItem*,int)\0"
    "text\0value_edited(QString)\0test()\0"
    "default_option()\0accept_options()\0"
    "apply_options()\0resize_all()\0"
};

const QMetaObject options::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_options,
      qt_meta_data_options, 0 }
};

const QMetaObject *options::metaObject() const
{
    return &staticMetaObject;
}

void *options::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_options))
	return static_cast<void*>(const_cast< options*>(this));
    return QDialog::qt_metacast(_clname);
}

int options::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: sel_changed((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 1: value_edited((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: test(); break;
        case 3: default_option(); break;
        case 4: accept_options(); break;
        case 5: apply_options(); break;
        case 6: resize_all(); break;
        }
        _id -= 7;
    }
    return _id;
}
