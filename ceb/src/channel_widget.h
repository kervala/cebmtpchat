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
#include <QListWidget>
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

class ChannelWidget : public SessionWidget
{
	Q_OBJECT

public:
	ChannelWidget(Session *session,	QWidget *parent = 0);
	
	void applyFirstShow();
	bool topicWindowVisible();
	void showTopicWindow();
	void hideTopicWindow();
	void refreshKeepAlivePolicy();
	void refreshFonts();
	void refreshWhoColumn();

	virtual QString widgetCaption() const;

protected:
	bool eventFilter(QObject *obj, QEvent *event);
	
private:
	QWidget *widgetTopic;
	QLineEdit *lineEditTopic;
	QSplitter *splitterOutIn;
	QSplitter *splitterOutWho;
	MyTextEdit *textEditOutput;
	QLabel *labelWhoTitle;
	QListWidget *listWidgetWho;
	QStackedWidget *stackedWidgetEntry;
	ChatLineWidget *lineEditWidget;
	HistoryWidget *historyWidget;
	QComboBox *comboBoxFilter;
	TokenRenderer m_tokenRenderer;

	int whoTicketID;
	int dateTicketID;
	int setClientTicketID;
	int helpTicketID;
	QTimer timerKeepAlive;
	QTimer timerWhoBlinking;
	QMap<QString,int> userToWhoBlinkTime;
	QMap<QString,bool> userToWhoAscendingOrder;
	QRegExp regExpAboutMe;
	bool firstShow;
	QList<BackupServer> m_backupServers;
	bool m_backupServersHelp; // Is true if the current help is about serverlist
	
	void init();
	QListWidgetItem *getWhoItemByNickname(const QString &nickname);
	void colorizeChatItems(const QColor &color);
	void changeLoginInWhoColumn(const QString &oldLogin,
					const QString &newLogin);
	void initScriptComboBox();
	
private slots:
	void sendText(const QString &text);
	void newTokenFromSession(const TokenEvent &event);
	void sessionConnecting();
	void sessionConnected();
	void sessionLogged();
	void sessionDisconnected();
	void sessionSocketError(const QString &errorStr);
	void outputKeyPressed(const QKeyEvent &e);
	void splitterInOutMoved(int pos, int index);
	void splitterOutWhoMoved(int pos, int index);
	void historyPageUp();
	void historyPageDown();
	void timerTimeout();
	void whoItemDoubleClicked(QListWidgetItem *item);
	void loginChanged(const QString &oldLogin, const QString &newLogin);
	void finger();
	void beep();
	void kick();
	void initiateTellSession();
	void sendAFile();
	void filterActivated(int index);
	void outputFilterSendToChat(const QString &text);
	void whoBlinking();
	
signals:
	void whoUserDoubleClicked(const QString &login);
	void tellSessionAsked(const QString &login);
	void showFileTransfers();
};

#endif
