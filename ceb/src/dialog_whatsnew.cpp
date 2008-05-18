#include <QApplication>
#include <QDir>
#include <QMessageBox>

#include "dialog_whatsnew.h"

DialogWhatsNew::DialogWhatsNew(QWidget *parent) : DialogBasic(true, parent)
{
    _textBrowser = new QTextBrowser;
    mainLayout->insertWidget(0, _textBrowser);

    QDir dir(QApplication::applicationDirPath());
    QUrl url = QUrl::fromLocalFile(dir.absoluteFilePath("whatsnew.html"));
    _textBrowser->setSource(url);
    resize(400, 250);
}
