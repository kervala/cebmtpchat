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

#ifndef TELL_WIDGET_H
#define TELL_WIDGET_H

#include <history_widget.h>

#include "session_widget.h"
#include "session_config.h"
#include "session.h"
#include "my_textedit.h"
#include "token_renderer.h"
#include "search_widget.h"

class TellWidget : public SessionWidget
{
	Q_OBJECT

public:
    TellWidget(Session *session, const QString &login, QWidget *parent = 0);

    const QString &login() const;
    void setLogin(const QString &newLogin);
    void applyFirstShow();
    void refreshFonts();

    virtual QString widgetCaption() const;

    void search(); // Inherited from SessionWidget

public slots:
    void newTokenFromSession(const Token &token);

protected:
    bool eventFilter(QObject *obj, QEvent *event);

private:
    static const int chatBlockNoOne = 0;
    static const int chatBlockYou = 1;
    static const int chatBlockHim = 2;

    bool _firstShow;
    QString _login;
    MyTextEdit *_textEditOutput;
    HistoryWidget *_historyWidget;
    QComboBox *_comboBoxFilter;
    TokenRenderer _tokenRenderer;
    int _chatBlock;
    SearchWidget *_searchWidget;

    void init();
    void scrollOutputToBottom();
    void setTextColor(int r, int g, int b);
    void initScriptComboBox();
    void toggleSearchWidgetVisibility();
    void replayPreviousConversation();

    void displayToken(const Token &token, bool old = false);

private slots:
    void sendText(const QString &text);
    void outputKeyPressed(const QKeyEvent &e);
    void historyPageUp();
    void historyPageDown();
    void outputFilterSendToChat(const QString &text);
    void filterActivated(int index);
    void hideSearchWidget();
};

#endif
