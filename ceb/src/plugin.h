#ifndef PLUGIN_H
#define PLUGIN_H

#include "../include/cebinterface.h"

class CeBPlugin : public QObject, public CeBInterface
{
    Q_OBJECT
    Q_INTERFACES(CeBInterface)
#ifdef USE_QT5
//    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QImageIOHandlerFactoryInterface" FILE "gif.json")
#endif

public:
    QMainWindow *getMainWindow() const;

    void init() const;
    void deinit() const;
};

#endif
