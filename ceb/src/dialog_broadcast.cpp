#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

#include <history_widget.h>

#include "dialog_broadcast.h"

DialogBroadcast::DialogBroadcast(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Message broadcasting"));
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setMargin(4);

    _historyWidget = new HistoryWidget;
    layout->addWidget(_historyWidget);

    QHBoxLayout *sendLayout = new QHBoxLayout;
    layout->addLayout(sendLayout);
    sendLayout->addStretch();
    QPushButton *sendButton = new QPushButton(tr("&broadcast to all servers"));
    sendLayout->addWidget(sendButton);
    resize(500, 60);
    connect(sendButton, SIGNAL(clicked()), this, SLOT(sendButtonClicked()));
    connect(_historyWidget, SIGNAL(textValidated(const QString&)), this, SLOT(textValidated(const QString&)));
}

void DialogBroadcast::sendButtonClicked()
{
    _text = _historyWidget->toPlainText();
    accept();
}

void DialogBroadcast::textValidated(const QString &text)
{
    _text = text;
    accept();
}
