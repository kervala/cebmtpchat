/* This file is part of CeB.
 * Copyright (C) 2005  Guillaume Denry
 *
 * CeB is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * any later version.
 *
 * CeB is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with CeB; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef CHANNEL_WIDGET_H
#define CHANNEL_WIDGET_H

#include <QLineEdit>
#include <QSplitter>
#include <QTreeView>
#include <QLabel>
#include <QTimer>
#include <QStackedWidget>
#include <QToolBar>
#include <QComboBox>

#include <history_widget.h>
#include <chat_line_widget.h>

#include "session_widget.h"
#include "session_config.h"
#include "session.h"
#include "my_textedit.h"
#include "token_renderer.h"
#include "search_widget.h"
#include "who_model.h"

class ChannelWidget : public SessionWidget
{
    Q_OBJECT

public:
    ChannelWidget(Session *session, QWidget *parent = 0);

    void applyFirstShow();
    bool topicWindowVisible();
    void showTopicWindow();
    void hideTopicWindow();
    void refreshKeepAlivePolicy();
    void refreshFonts();
    void refreshWhoColumn();

    virtual QString widgetCaption() const;

    void search(); // Inherited

protected:
    bool eventFilter(QObject *obj, QEvent *event);

private:
    QWidget *_widgetTopic;
    QTextEdit *_lineEditTopic;
    QSplitter *_splitterOutIn;
    QSplitter *_splitterOutWho;
    MyTextEdit *_textEditOutput;
    SearchWidget *_searchWidget;
    QLabel *_labelWhoTitle;
    QTreeView *_treeViewWho;
    QStackedWidget *_stackedWidgetEntry;
    ChatLineWidget *_lineEditWidget;
    QString _manualPassword;
    HistoryWidget *_historyWidget;
    QComboBox *_comboBoxFilter;
    TokenRenderer _tokenRenderer;
    int _whoTicketID;
    int _groupsTicketID;
    int _dateTicketID;
    int _setClientTicketID;
    int _helpTicketID;
    QTimer _timerKeepAlive;
    QTimer _timerWhoBlinking;
    QMap<QString,int> _userToWhoBlinkTime;
    QMap<QString,bool> _userToWhoAscendingOrder;
    QRegExp _regExpAboutMe;
    bool _firstShow;
    QList<BackupServer> _backupServers;
    bool _backupServersHelp; // is true if the current help is about serverlist
    WhoModel *_whoModel;
    WhoSortModel *_whoSortModel;

    void init();
    void colorizeChatItems(const QColor &color);
    void initScriptComboBox();
    void toggleSearchWidgetVisibility();
    void refreshWhoLabel();

private slots:
    void sendLineEditText(const QString &text);
    void sendText(const QString &text);
    void newToken(const Token &token);
    void sessionConnecting();
    void sessionConnected();
    void sessionLogged();
    void sessionDisconnected();
    void sessionSocketError(const QString &errorStr);
    void sessionCleared();
    void outputKeyPressed(const QKeyEvent &e);
    void splitterInOutMoved(int pos, int index);
    void splitterOutWhoMoved(int pos, int index);
    void historyPageUp();
    void historyPageDown();
    void keepAliveTimeout();
    void whoDoubleClicked(const QModelIndex &index);
    void loginChanged(const QString &oldLogin, const QString &newLogin);
    void finger();
    void beep();
    void kick();
    void initiateTellSession();
    void sendAFile();
    void filterActivated(int index);
    void outputFilterSendToChat(const QString &text);
    void whoBlinking();
    void hideSearchWidget();
    void whoModelRowsInserted(const QModelIndex &parent, int start, int end);
    void whoModelRowsRemoved(const QModelIndex &parent, int start, int end);
    void whoModelReset();

signals:
    void whoUserDoubleClicked(const QString &login);
    void tellSessionAsked(const QString &login);
    void showFileTransfers();
};

#endif
