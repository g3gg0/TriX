/****************************************************************************
** Meta object code from reading C++ file 'ui_object_tree.h'
**
** Created: Sun 3. Aug 23:18:29 2014
**      by: The Qt Meta Object Compiler version 59 (Qt 4.3.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "ui_object_tree.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ui_object_tree.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.3.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_ObjectTree[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      28,   16,   12,   11, 0x08,
      66,   11,   11,   11, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_ObjectTree[] = {
    "ObjectTree\0\0int\0item,column\0"
    "treeItemClicked(QTreeWidgetItem*,int)\0"
    "treeItemChanged()\0"
};

const QMetaObject ObjectTree::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_ObjectTree,
      qt_meta_data_ObjectTree, 0 }
};

const QMetaObject *ObjectTree::metaObject() const
{
    return &staticMetaObject;
}

void *ObjectTree::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ObjectTree))
	return static_cast<void*>(const_cast< ObjectTree*>(this));
    return QDialog::qt_metacast(_clname);
}

int ObjectTree::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: { int _r = treeItemClicked((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 1: treeItemChanged(); break;
        }
        _id -= 2;
    }
    return _id;
}
