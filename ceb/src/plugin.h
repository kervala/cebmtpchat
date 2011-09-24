#include <QObject>

#include "../include/plugininterface.h"

class CeBPlugin : public QObject, public PluginInterface
{
    Q_OBJECT
    Q_INTERFACES(PluginInterface)

public:
    QMainWindow *getMainWindow() const;

    void init() const;
    void deinit() const;
};
