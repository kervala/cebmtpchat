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

#ifndef TOKEN_FACTORY_H
#define TOKEN_FACTORY_H

#include <QObject>
#include <QRegExp>

#include "token.h"

class MtpRegExp : public QRegExp
{
public:
    MtpRegExp(const QString &pattern, const QList<int> arguments, const QString &example) :
        QRegExp(pattern),
        _arguments(arguments),
        _example(example) {}
    MtpRegExp(const QString &pattern, const QString &example) :
        QRegExp(pattern),
        _example(example) {}
    MtpRegExp(const QString &pattern, const QList<int> arguments) :
        QRegExp(pattern),
        _arguments(arguments) {}
    MtpRegExp(const QString &pattern) : QRegExp(pattern) {}

    const QList<int> &arguments() const { return _arguments; }
    const QString &example() const { return _example; }

private:
    QList<int> _arguments;
    QString _example;
};

class TokenFactory : public QObject
{
    Q_OBJECT

public:
    enum SendToken {
        SendToken_Tell,
        SendToken_Reply,
        SendToken_Sendmsg
    };

    enum State {
        State_Normal,
        State_Who,
        State_History,
        State_Finger,
        State_Alias,
        State_Wall,
        State_Message,
        State_Help
    };

    enum Command {
        Command_Who = 0,
        Command_Wall,
        Command_Date,
        Command_SetClient,
        Command_ShowMsg,
        Command_Help,
        Command_Count
    };

    struct CommandTicket {
        Command command;
        int ID;
    };

    TokenFactory();

    bool logged() const { return _logged; }
    State state() const { return _state; }
    bool away() const { return _away; }

    QStringList split(const QString &message);
    void reset();

    int requestTicket(Command command);

    const QList<MtpRegExp> &tokenRegexp() const { return _tokenRegexp; }

    static const TokenFactory defaultFactory;

public slots:
    void dataReceived(const QString &data);

signals:
    void newToken(const Token &token);

private:
    static Token::Type whoTokens[];
    static Token::Type aliasTokens[];
    static Token::Type normalTokens[];
    static Token::Type historyTokens[];
    static Token::Type fingerTokens[];
    static Token::Type wallTokens[];
    static Token::Type messageTokens[];
    static Token::Type helpTokens[];

    bool _logged;
    State _state;
    bool _away;

    QList<MtpRegExp> _tokenRegexp;
    QList<QRegExp> _sendTokenRegexp;
    QRegExp _timeRegexp;

    QList<CommandTicket> tickets[Command_Count];

    void analyzeBeforeLog(const QString &data);
    void analyzeAfterLog(const QString &data);

    // !!! Uses <tokenRegexp> state to work !!!
    void doTokenAnalyzed(Token::Type tokenType, int ticketID, const QTime &timeStamp);

    void clearTickets();

    inline QString rightTrim(const QString &str) const;
};

#endif
