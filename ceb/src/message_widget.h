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

#ifndef MESSAGE_WIDGET_H
#define MESSAGE_WIDGET_H

#include <QWidget>
#include <QCheckBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QSplitter>
#include <QPushButton>
#include <QTreeView>

#include <generic_sort_model.h>

#include "session_widget.h"
#include "my_textedit.h"
#include "message_model.h"

class MessageWidget : public SessionWidget
{
    Q_OBJECT

public:
    MessageWidget(Session *session, QWidget *parent = 0);

    virtual QString widgetCaption() const;

protected:
    void showEvent(QShowEvent *event);

private:
    bool m_firstShow;
    QSplitter *splitterMain;
    QTreeView *treeViewMessages;
    QPushButton *pushButtonRefresh;
    QPushButton *pushButtonRemove;
    MyTextEdit *textEditMessage;
    GenericSortModel *m_sortModel;
    MessageModel *messageModel;
    int showMsgTicketID;

    void init();
    void focusLastItem();

private slots:
    void currentMessageChanged(const QModelIndex & current, const QModelIndex & previous);
    void removeSelectedMessage();
    void newTokenFromSession(const TokenEvent &event);
    void refreshMessages();
};

#endif
