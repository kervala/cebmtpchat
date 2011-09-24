#include <QObject>

#include "../include/cebinterface.h"

class CeBPlugin : public QObject, public CeBInterface
{
    Q_OBJECT
    Q_INTERFACES(CeBInterface)

public:
    QMainWindow *getMainWindow() const;

    void init() const;
    void deinit() const;
};
