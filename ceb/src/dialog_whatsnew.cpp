#include <QApplication>
#include <QDir>
#include <QMessageBox>

#include "dialog_whatsnew.h"

DialogWhatsNew::DialogWhatsNew(QWidget *parent) : DialogBasic(true, parent)
{
	m_textBrowser = new QTextBrowser;
	mainLayout->insertWidget(0, m_textBrowser);

	QDir dir(QApplication::applicationDirPath());
	QUrl url = QUrl::fromLocalFile(dir.absoluteFilePath("whatsnew.html"));
	m_textBrowser->setSource(url);
	resize(400, 250);
}
