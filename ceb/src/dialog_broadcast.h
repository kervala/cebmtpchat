#ifndef DIALOG_BROADCAST_H
#define DIALOG_BROADCAST_H

#include <QDialog>

#include <history_widget.h>

class DialogBroadcast : public QDialog
{
    Q_OBJECT

public:
    DialogBroadcast(QWidget *parent = 0);

    const QString &text() const { return _text; }

private:
    HistoryWidget *_historyWidget;
    QString _text;

private slots:
    void sendButtonClicked();
    void textValidated(const QString &text);
};

#endif
