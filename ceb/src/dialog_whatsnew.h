#ifndef DIALOG_WHATSNEW_H
#define DIALOG_WHATSNEW_H

#include <dialog_basic.h>

#include <QTextBrowser>

class DialogWhatsNew : public DialogBasic
{
    Q_OBJECT

public:
    DialogWhatsNew(QWidget *parent = 0);

private:
    QTextBrowser *_textBrowser;
};

#endif
