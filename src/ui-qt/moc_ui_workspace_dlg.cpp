/****************************************************************************
** Meta object code from reading C++ file 'ui_workspace_dlg.h'
**
** Created: Sun 3. Aug 23:18:29 2014
**      by: The Qt Meta Object Compiler version 59 (Qt 4.3.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "ui_workspace_dlg.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ui_workspace_dlg.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.3.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_WorkspaceWindow[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      17,   16,   16,   16, 0x08,
      36,   16,   16,   16, 0x08,
      55,   16,   16,   16, 0x08,
      88,   76,   72,   16, 0x08,
     131,   16,   16,   16, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_WorkspaceWindow[] = {
    "WorkspaceWindow\0\0btnExportClicked()\0"
    "btnImportClicked()\0btnEditClicked()\0"
    "int\0item,column\0"
    "treeWorkspaceClicked(QTreeWidgetItem*,int)\0"
    "treeWorkspaceChanged()\0"
};

const QMetaObject WorkspaceWindow::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_WorkspaceWindow,
      qt_meta_data_WorkspaceWindow, 0 }
};

const QMetaObject *WorkspaceWindow::metaObject() const
{
    return &staticMetaObject;
}

void *WorkspaceWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_WorkspaceWindow))
	return static_cast<void*>(const_cast< WorkspaceWindow*>(this));
    return QDialog::qt_metacast(_clname);
}

int WorkspaceWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: btnExportClicked(); break;
        case 1: btnImportClicked(); break;
        case 2: btnEditClicked(); break;
        case 3: { int _r = treeWorkspaceClicked((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 4: treeWorkspaceChanged(); break;
        }
        _id -= 5;
    }
    return _id;
}
