#include <QCoreApplication>
#include <QDir>
#include <QFile>

#include "global.h"

#define DEV_FILENAME "dev"

bool Global::devMode()
{
    QDir appDir(QCoreApplication::applicationDirPath());
    return QFile(appDir.filePath(DEV_FILENAME)).exists();
}
