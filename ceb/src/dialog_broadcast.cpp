#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>

#include "session_manager.h"
#include "dialog_broadcast.h"

DialogBroadcast::DialogBroadcast(QWidget *parent)
    : QDialog(parent, Qt::Dialog | Qt::WindowTitleHint | Qt::WindowSystemMenuHint)
{
    setWindowTitle(tr("Message broadcasting"));
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setMargin(4);

    _historyWidget = new HistoryWidget;
    layout->addWidget(_historyWidget);

    QHBoxLayout *sendLayout = new QHBoxLayout;
    layout->addLayout(sendLayout);

    QVBoxLayout *serversLayout = new QVBoxLayout;
    serversLayout->setSpacing(0);

    _signalMapper = new QSignalMapper(this);

    QList<Session*> &sessions = SessionManager::instance().sessionsList();

    foreach (Session *session, sessions)
    {
        QCheckBox *checkbox = new QCheckBox(session->caption());
        checkbox->setChecked(session->config().broadcast());
        serversLayout->addWidget(checkbox);
        connect(checkbox, SIGNAL(clicked()), _signalMapper, SLOT (map()));
        _signalMapper->setMapping(checkbox, session);
    }

    sendLayout->addLayout(serversLayout);

    sendLayout->addStretch();
    QPushButton *sendButton = new QPushButton(tr("&broadcast to all servers"));
    sendLayout->addWidget(sendButton, 0, Qt::AlignBottom);
    resize(500, 60);
    connect(sendButton, SIGNAL(clicked()), this, SLOT(sendButtonClicked()));
    connect(_historyWidget, SIGNAL(textValidated(const QString&)), this, SLOT(textValidated(const QString&)));
    connect(_signalMapper, SIGNAL(mapped(QObject *)), this, SLOT(selectServer(QObject *)));
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

void DialogBroadcast::selectServer(QObject *object)
{
    Session *session = dynamic_cast<Session*>(object);
    if (!session) return;
    SessionConfig config = session->config();
    config.setBroadcast(!config.broadcast());
    session->setConfig(config);
}
