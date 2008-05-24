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

#ifndef CMD_OUTPUT_WIDGET_H
#define CMD_OUTPUT_WIDGET_H

#include "session_widget.h"
#include "my_textedit.h"
#include "session.h"
#include "token_renderer.h"

class CmdOutputWidget : public SessionWidget
{
    Q_OBJECT

public:
    CmdOutputWidget(Session *session, const QString &cmdName, QWidget *parent = 0);

    const QString &cmdName() const;

    virtual QString widgetCaption() const;

public slots:
    void newTokenFromSession(const Token &token);

private:
    QString m_cmdName;

    MyTextEdit *m_textEditOutput;
    TokenRenderer m_tokenRenderer;

    void init();
    void scrollOutputToBottom();
};


#endif
