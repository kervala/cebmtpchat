#include "main_window.h"
#include "plugin.h"

QMainWindow *CeBPlugin::getMainWindow() const {
    return MainWindow::instance();
}

void CeBPlugin::init() const {
}

void CeBPlugin::deinit() const {
}

Q_EXPORT_PLUGIN2(cebplugin, CeBPlugin)
