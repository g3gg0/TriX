/****************************************************************************
** Meta object code from reading C++ file 'ui_text_box.h'
**
** Created: Sun 6. Jan 17:44:39 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.3.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "ui_text_box.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ui_text_box.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.3.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_TextBox[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets

       0        // eod
};

static const char qt_meta_stringdata_TextBox[] = {
    "TextBox\0"
};

const QMetaObject TextBox::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_TextBox,
      qt_meta_data_TextBox, 0 }
};

const QMetaObject *TextBox::metaObject() const
{
    return &staticMetaObject;
}

void *TextBox::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_TextBox))
	return static_cast<void*>(const_cast< TextBox*>(this));
    return QDialog::qt_metacast(_clname);
}

int TextBox::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
