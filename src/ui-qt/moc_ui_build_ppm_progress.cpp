/****************************************************************************
** Meta object code from reading C++ file 'ui_build_ppm_progress.h'
**
** Created: Mo 11. Jun 18:43:01 2007
**      by: The Qt Meta Object Compiler version 59 (Qt 4.1.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "ui_build_ppm_progress.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ui_build_ppm_progress.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.1.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_BuildPPMProgress[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      18,   17,   17,   17, 0x0a,
      27,   17,   17,   17, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_BuildPPMProgress[] = {
    "BuildPPMProgress\0\0update()\0cancel()\0"
};

const QMetaObject BuildPPMProgress::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_BuildPPMProgress,
      qt_meta_data_BuildPPMProgress, 0 }
};

const QMetaObject *BuildPPMProgress::metaObject() const
{
    return &staticMetaObject;
}

void *BuildPPMProgress::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_BuildPPMProgress))
	return static_cast<void*>(const_cast<BuildPPMProgress*>(this));
    return QDialog::qt_metacast(_clname);
}

int BuildPPMProgress::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: update(); break;
        case 1: cancel(); break;
        }
        _id -= 2;
    }
    return _id;
}
