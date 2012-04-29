#include "token_display.h"
#include "script.h"
#include "profile.h"
#include "main_window.h"
#include "plugin.h"

#ifdef DEBUG_NEW
#define new DEBUG_NEW
#endif

QMainWindow *CeBPlugin::getMainWindow() const {
    return MainWindow::instance();
}

void CeBPlugin::init() const {
    TextSkin::createDefaultSkin(); // Used to create fonts after QApplication (otherwise, it failed and gives wrong fonts)
    Profile::instance().load();
}

void CeBPlugin::deinit() const {
    Script::closeModifiers(); // Close all lua chunks

    Profile::instance().save();
	Profile::free();

	MainWindow::free();
	TextSkin::freeDefaultSkin();
}

Q_EXPORT_PLUGIN2(cebplugin, CeBPlugin)
