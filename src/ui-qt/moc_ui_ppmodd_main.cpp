/****************************************************************************
** Meta object code from reading C++ file 'ui_ppmodd_main.h'
**
** Created: So 5. Aug 14:55:17 2007
**      by: The Qt Meta Object Compiler version 59 (Qt 4.1.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "ui_ppmodd_main.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ui_ppmodd_main.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.1.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_PPModdMainWindow[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
      20,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      34,   22,   18,   17, 0x08,
      89,   72,   17,   17, 0x08,
     147,   17,   17,   17, 0x08,
     169,  165,   17,   17, 0x08,
     199,  193,   17,   17, 0x0a,
     232,  193,   17,   17, 0x0a,
     255,   17,   17,   17, 0x0a,
     273,   17,   17,   17, 0x0a,
     291,   17,   17,   17, 0x0a,
     310,   17,   17,   17, 0x0a,
     329,   17,   17,   17, 0x0a,
     351,   17,   17,   17, 0x0a,
     375,   17,   17,   17, 0x0a,
     397,   17,   17,   17, 0x0a,
     416,   17,   17,   17, 0x0a,
     437,   17,   17,   17, 0x0a,
     457,   17,   17,   17, 0x0a,
     474,   17,   17,   17, 0x0a,
     495,   17,   17,   17, 0x0a,
     516,   17,   17,   17, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_PPModdMainWindow[] = {
    "PPModdMainWindow\0\0int\0item,column\0"
    "treeItemClicked(QTreeWidgetItem*,int)\0current,previous\0"
    "treeCurrentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)\0"
    "treeItemChanged()\0pos\0treeItemContext(QPoint)\0event\0"
    "dragEnterEvent(QDragEnterEvent*)\0dropEvent(QDropEvent*)\0"
    "evtTimerElapsed()\0txtEnterPressed()\0actOpenTriggered()\0"
    "actQuitTriggered()\0actOptionsTriggered()\0actWorkspaceTriggered()\0"
    "actPluginsTriggered()\0actSaveTriggered()\0actSaveAsTriggered()\0"
    "actCloseTriggered()\0btnEditClicked()\0actImportTriggered()\0"
    "actExportTriggered()\0actBuildPPMTriggered()\0"
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
	return static_cast<void*>(const_cast<PPModdMainWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int PPModdMainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: { int _r = treeItemClicked((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 1: treeCurrentItemChanged((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])),(*reinterpret_cast< QTreeWidgetItem*(*)>(_a[2]))); break;
        case 2: treeItemChanged(); break;
        case 3: treeItemContext((*reinterpret_cast< const QPoint(*)>(_a[1]))); break;
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
        }
        _id -= 20;
    }
    return _id;
}
