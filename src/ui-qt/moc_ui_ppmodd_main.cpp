/****************************************************************************
** Meta object code from reading C++ file 'ui_ppmodd_main.h'
**
** Created: Wed 21. Aug 22:04:24 2013
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "ui_ppmodd_main.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ui_ppmodd_main.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_PPModdMainWindow[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
      20,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      30,   18,   17,   17, 0x08,
      85,   68,   17,   17, 0x08,
     147,  143,   17,   17, 0x08,
     177,  171,   17,   17, 0x0a,
     202,  171,   17,   17, 0x0a,
     235,  171,   17,   17, 0x0a,
     258,   17,   17,   17, 0x0a,
     276,   17,   17,   17, 0x0a,
     294,   17,   17,   17, 0x0a,
     313,   17,   17,   17, 0x0a,
     332,   17,   17,   17, 0x0a,
     354,   17,   17,   17, 0x0a,
     378,   17,   17,   17, 0x0a,
     400,   17,   17,   17, 0x0a,
     419,   17,   17,   17, 0x0a,
     440,   17,   17,   17, 0x0a,
     460,   17,   17,   17, 0x0a,
     477,   17,   17,   17, 0x0a,
     498,   17,   17,   17, 0x0a,
     519,   17,   17,   17, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_PPModdMainWindow[] = {
    "PPModdMainWindow\0\0item,column\0"
    "treeItemClicked(QTreeWidgetItem*,int)\0"
    "current,previous\0"
    "treeCurrentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)\0"
    "pos\0treeItemContext(QPoint)\0event\0"
    "closeEvent(QCloseEvent*)\0"
    "dragEnterEvent(QDragEnterEvent*)\0"
    "dropEvent(QDropEvent*)\0evtTimerElapsed()\0"
    "txtEnterPressed()\0actOpenTriggered()\0"
    "actQuitTriggered()\0actOptionsTriggered()\0"
    "actWorkspaceTriggered()\0actPluginsTriggered()\0"
    "actSaveTriggered()\0actSaveAsTriggered()\0"
    "actCloseTriggered()\0btnEditClicked()\0"
    "actImportTriggered()\0actExportTriggered()\0"
    "actBuildPPMTriggered()\0"
};

const QMetaObject PPModdMainWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_PPModdMainWindow,
      qt_meta_data_PPModdMainWindow, 0 }
};

const QMetaObject *PPModdMainWindow::metaObject() const
{
    return &staticMetaObject;
}

void *PPModdMainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_PPModdMainWindow))
        return static_cast<void*>(const_cast< PPModdMainWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int PPModdMainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: treeItemClicked((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 1: treeCurrentItemChanged((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])),(*reinterpret_cast< QTreeWidgetItem*(*)>(_a[2]))); break;
        case 2: treeItemContext((*reinterpret_cast< const QPoint(*)>(_a[1]))); break;
        case 3: closeEvent((*reinterpret_cast< QCloseEvent*(*)>(_a[1]))); break;
        case 4: dragEnterEvent((*reinterpret_cast< QDragEnterEvent*(*)>(_a[1]))); break;
        case 5: dropEvent((*reinterpret_cast< QDropEvent*(*)>(_a[1]))); break;
        case 6: evtTimerElapsed(); break;
        case 7: txtEnterPressed(); break;
        case 8: actOpenTriggered(); break;
        case 9: actQuitTriggered(); break;
        case 10: actOptionsTriggered(); break;
        case 11: actWorkspaceTriggered(); break;
        case 12: actPluginsTriggered(); break;
        case 13: actSaveTriggered(); break;
        case 14: actSaveAsTriggered(); break;
        case 15: actCloseTriggered(); break;
        case 16: btnEditClicked(); break;
        case 17: actImportTriggered(); break;
        case 18: actExportTriggered(); break;
        case 19: actBuildPPMTriggered(); break;
        default: ;
        }
        _id -= 20;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
