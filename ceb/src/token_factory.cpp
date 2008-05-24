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

#include <QVector>
#include <QStringList>
#include <QMessageBox>

#include "token_info.h"

#include "token_factory.h"

#define LOGIN_RE "[a-zA-Z][a-zA-Z\\d]{0,7}"

#define TOKEN_A_SIZE(a) sizeof(a) / sizeof(a[0])

const TokenFactory TokenFactory::defaultFactory;

Token::Type TokenFactory::whoTokens[] = {
    Token::WhoSeparator,
    Token::WhoLine,
    Token::WhoEnd,
    Token::WhoEndNoUser
};

Token::Type TokenFactory::aliasTokens[] = {
    Token::UserAliases,
    Token::AliasesEnd,
    Token::NoUserAlias,
    Token::AliasLine
};

Token::Type TokenFactory::normalTokens[] = {
    Token::IndicatedActiveServer,
    Token::Topic,
    Token::YouSetTopic,
    Token::SomeoneSetTopic,
    Token::SomeoneTellsYou,
    Token::YouTellToSomeone,
    Token::SomeoneAsksYou,
    Token::YouAskToSomeone,
    Token::SomeoneReplies,
    Token::YouReply,
    Token::SomeoneBeepsYou,
    Token::WallBegin,
    Token::WhoBegin,
    Token::HistoryBegin,
    Token::FingerBegin,
    Token::SomeoneShouts,
    Token::YouShout,
    Token::SomeoneAway,
    Token::YouAway,
    Token::SomeoneBack,
    Token::YouBack,
    Token::SomeoneAwayWarning,
    Token::SystemAliases,
    Token::MessageBegin,
    Token::NoMessage,
    Token::MessageReceived,
    Token::AllMessagesCleared,
    Token::MessageCleared,
    Token::MessagesCleared,
    Token::HelpBegin,
    Token::UnknownUser,
    Token::UserLoginRenamed,
    Token::YourLoginRenamed,
    Token::SomeoneComesIn,
    Token::SomeoneLeaves,
    Token::SomeoneDisconnects,
    Token::YouLeave,
    Token::YouJoinChannel,
    Token::YouLeaveChannel,
    Token::SomeoneJoinChannel,
    Token::SomeoneFadesIntoTheShadows,
    Token::SomeoneLeaveChannel,
    Token::SomeoneAppearsFromTheShadows,
    Token::YouKickSomeone,
    Token::SomeoneIsKicked,
    Token::YouAreKicked,
    Token::Date,
    Token::YourClientIs,
    Token::MtpSays,
    Token::SomeoneSays,
    Token::Data,
    Token::Unknown
};

Token::Type TokenFactory::historyTokens[] = {
    Token::HistoryEnd,
    Token::HistoryLine
};

Token::Type TokenFactory::fingerTokens[] = {
    Token::FingerEnd,
    Token::FingerLine
};

Token::Type TokenFactory::wallTokens[] = {
    Token::WallEnd,
    Token::WallLine
};

Token::Type TokenFactory::messageTokens[] = {
    Token::MessageEnd,
    Token::MessageLine
};

Token::Type TokenFactory::helpTokens[] = {
    Token::HelpEndNormal,
    Token::HelpEndNoHelp,
    Token::HelpLine
};

TokenFactory::TokenFactory()
{
    _logged = false;
    _state = State_Normal;
    _away = false;

    _timeRegexp = QRegExp("^(\\d+:\\d+:\\d+) (.*)$");

    // Fill regexp
    QList<int> args;
    args << 3 << 4;
    _tokenRegexp << MtpRegExp("^<Mtp> (Active|Main) server (is|move) at ([^:]+):(.+)$",
                              QList<int>() << 3 << 4,
                              "<Mtp> Active server is at a.b.c:4000"); // Token::IndicatedActiveServer

    _tokenRegexp << MtpRegExp("^<Mtp> Login: $",
                              "<Mtp> Login: "); // Token::LoginAsked
    _tokenRegexp << MtpRegExp("^<Mtp> Invalid login, choose another one$",
                              "<Mtp> Invalid login, choose another one"); // Token::InvalidLogin
    _tokenRegexp << MtpRegExp("<Mtp> Password: $",
                              "<Mtp> Password: "); // Token::PasswordAsked
    _tokenRegexp << MtpRegExp("^<Mtp> Incorrect password$",
                              "<Mtp> Incorrect password"); // Token::IncorrectPassword
    _tokenRegexp << MtpRegExp("^<Mtp> Welcome, ("LOGIN_RE")\\.$",
                              QList<int>() << 1,
                              "<Mtp> Welcome, Fooman."); // Token::Welcome

    _tokenRegexp << MtpRegExp("^<Mtp> ("LOGIN_RE") topic : (.+)$",
                              QList<int>() << 1 << 2,
                              "<Mtp> Hall topic : Salut les poulets !"); // Token::Topic
    _tokenRegexp << MtpRegExp("^<Mtp> You set channel ("LOGIN_RE") topic to (.*)$",
                              QList<int>() << 1 << 2,
                              "<Mtp> You set channel Hall topic to Salut les poulets !"); // Token::YouSetTopic
    _tokenRegexp << MtpRegExp("^<Mtp> ("LOGIN_RE") set channel ("LOGIN_RE") topic to (.*)$",
                              QList<int>() << 1 << 2 << 3,
                              "<Mtp> Fooman set channel Hall topic to Salut les poulets !"); // Token::SomeoneSetTopic

    _tokenRegexp << MtpRegExp("^<Mtp> ("LOGIN_RE") tells you: (.+)$",
                              QList<int>() << 1 << 2,
                              "<Mtp> Fooman tells you: espece de truite"); // Token::SomeoneTellsYou
    _tokenRegexp << MtpRegExp("^<Mtp> You tell ("LOGIN_RE"): (.+)$",
                              QList<int>() << 1 << 2,
                              "<Mtp> You tell Fooman: espece de truite"); // Token::YouTellToSomeone
    _tokenRegexp << MtpRegExp("^<Mtp> ("LOGIN_RE") asks you: (.+)$",
                              QList<int>() << 1 << 2,
                              "<Mtp> Fooman asks you: es-tu un saumon ou une truite ?"); // Token::SomeoneAsksYou
    _tokenRegexp << MtpRegExp("^<Mtp> You ask ("LOGIN_RE"): (.+)$",
                              QList<int>() << 1 << 2,
                              "<Mtp> You ask Fooman: es-tu une truite ou un saumon ?"); // Token::YouAskToSomeone
    _tokenRegexp << MtpRegExp("^<Mtp> ("LOGIN_RE") replies: (.+)$",
                              QList<int>() << 1 << 2,
                              "<Mtp> Fooman replies: vive les levures !"); // Token::SomeoneReplies
    _tokenRegexp << MtpRegExp("^<Mtp> You reply to ("LOGIN_RE"): (.+)$",
                              QList<int>() << 1 << 2,
                              "<Mtp> You reply to Fooman: vive les levures !"); // Token::YouReply

    _tokenRegexp << MtpRegExp("^<Mtp> ("LOGIN_RE") beeps you$",
                              QList<int>() << 1,
                              "<Mtp> Fooman beeps you"); // Token::SomeoneBeepsYou

    _tokenRegexp << MtpRegExp("^<Mtp> Wall :$",
                              "<Mtp> Wall :"); // Token::WallBegin
    _tokenRegexp << MtpRegExp("^<Mtp> End of Wall$",
                              "<Mtp> End of Wall"); // Token::WallEnd
    _tokenRegexp << MtpRegExp("^([^:]*) *(\\d+:\\d+:\\d+) *(\\S+) *(.*)$",
                              QList<int>() << 1 << 2 << 3 << 4,
                              "Fri Feb 04 2005 16:48:01 Fooman      Hello World!"); // Token::WallLine

    _tokenRegexp << MtpRegExp("^ Login    Group   Channel   Idle  On For C              From host$",
                              " Login    Group   Channel   Idle  On For C              From host"); // Token::WhoBegin
    _tokenRegexp << MtpRegExp("^<Mtp> There (are|is) currently (\\d+) users?(( in channel ("LOGIN_RE"))|)$",
                              QList<int>() << 2 << 5,
                              "<Mtp> There are currently 12 users in channel Hall"); // Token::WhoEnd
    _tokenRegexp << MtpRegExp("^<Mtp> There is nobody in channel ("LOGIN_RE")$",
                              QList<int>() << 1,
                              "<Mtp> There is nobody in channel Hall"); // Token::WhoEndNoUser
    _tokenRegexp << MtpRegExp("^-------- -------- -------- ------ ------ - ------------------------------------$",
                              "-------- -------- -------- ------ ------ - ------------------------------------"); // Token::WhoSeparator
    _tokenRegexp << MtpRegExp("^("LOGIN_RE") *(\\w+) *(\\w+) *([^ ]+) *(\\w+) *(\\w+) *(.+)$",
                              QList<int>() << 1 << 2 << 3 << 4 << 5 << 6 << 7); // Token::WhoLine

    _tokenRegexp << MtpRegExp("^<Mtp> History :$",
                              "<Mtp> History :"); // Token::HistoryBegin
    _tokenRegexp << MtpRegExp("^<Mtp> End of History$",
                              "<Mtp> End of History"); // Token::HistoryEnd
    _tokenRegexp << MtpRegExp("^(\\S+) *(\\S+) *(\\S+) *(\\S+) *(\\S+) *(.+)$",
                              QList<int>() << 1 << 2 << 3 << 4 << 5 << 6); // Token::HistoryLine

    _tokenRegexp << MtpRegExp("^Login *: ("LOGIN_RE")$",
                              QList<int>() << 1,
                              "Login  : Fooman"); // Token::FingerBegin
    _tokenRegexp << MtpRegExp("^<Mtp> End of finger$",
                              "<Mtp> End of finger"); // Token::FingerEnd
    _tokenRegexp << MtpRegExp("^(.+)$",
                              "Blablabla xann blablabla"); // Token::FingerLine

    _tokenRegexp << MtpRegExp("^<Mtp> ("LOGIN_RE") shouts: (.+)$",
                              QList<int>() << 1 << 2,
                              "<Mtp> Fooman shouts: je veux etre liiibre"); // Token::SomeoneShouts
    _tokenRegexp << MtpRegExp("^<Mtp> You shout: (.+)$",
                              QList<int>() << 1,
                              "<Mtp> You shout: sortez-moi de ce programme de fou"); // Token::YouShout

    _tokenRegexp << MtpRegExp("^<Mtp> ("LOGIN_RE") is away$",
                              QList<int>() << 1,
                              "<Mtp> Fooman is away"); // Token::SomeoneAway
    _tokenRegexp << MtpRegExp("^(\\d+:\\d+:\\d+) (<Mtp> You are away)$",
                              "<Mtp> You are away"); // Token::YouAway
    _tokenRegexp << MtpRegExp("^<Mtp> ("LOGIN_RE") is back$",
                              QList<int>() << 1,
                              "<Mtp> Fooman is back"); // Token::SomeoneBack
    _tokenRegexp << MtpRegExp("^<Mtp> You are back$",
                              "<Mtp> You are back"); // Token::YouBack
    _tokenRegexp << MtpRegExp("^<Mtp> ("LOGIN_RE") is away and may not be hearing you$",
                              QList<int>() << 1,
                              "<Mtp> Fooman is away and may not be hearing you"); // Token::SomeoneAwayWarning

    _tokenRegexp << MtpRegExp("^<Mtp> System Aliases : $",
                              "<Mtp> System aliases"); // Token::SystemAliases
    _tokenRegexp << MtpRegExp("^<Mtp> User Aliases : $",
                              "<Mtp> User Aliases : "); // Token::UserAliases
    _tokenRegexp << MtpRegExp("^<Mtp> End of aliases$",
                              "<Mtp> End of aliases"); // Token::AliasesEnd
    _tokenRegexp << MtpRegExp("^<Mtp> You don't have any alias !$",
                              "<Mtp> You don't have any alias !"); // Token::NoUserAlias
    _tokenRegexp << MtpRegExp("^([^ ]+)( *.+)$",
                              QList<int>() << 1 << 2,
                              "un_alias    ouais euhhh ok"); // Token::AliasLine

    _tokenRegexp << MtpRegExp("^<Mtp> Your message\\(s\\) :$",
                              "<Mtp> You message(s) :"); // Token::MessageBegin
    _tokenRegexp << MtpRegExp("^ *(\\S+) ([^:]+ \\d+:\\d+:\\d+) ("LOGIN_RE") : (.+)$",
                              QList<int>() << 1 << 2 << 3 << 4); // Token::MessageLine
    _tokenRegexp << MtpRegExp("^<Mtp> You have (\\d+) messages?$",
                              QList<int>() << 1,
                              "<Mtp> You have 14 messages"); // Token::MessageEnd
    _tokenRegexp << MtpRegExp("^<Mtp> You have no message !$",
                              "<Mtp> You have no message"); // Token::NoMessage
    _tokenRegexp << MtpRegExp("^<Mtp> ("LOGIN_RE") sends a message to you: (.+)$",
                              QList<int>() << 1 << 2,
                              "<Mtp> Fooman sends a message to you: huitre"); // Token::MessageReceived
    _tokenRegexp << MtpRegExp("^<Mtp> You clear your message\\(s\\) !$",
                              "<Mtp> You clear your messages"); // Token::AllMessagesCleared
    _tokenRegexp << MtpRegExp("^<Mtp> You clear message (\\d+) !$",
                              QList<int>() << 1); // Token::MessageCleared
    _tokenRegexp << MtpRegExp("^<Mtp> You clear messages (\\d+) to (\\d+) !$",
                              QList<int>() << 1 << 2,
                              "<Mtp> You clear messages 1 to 5 !"); // Token::MessagesCleared

    _tokenRegexp << MtpRegExp("^<Mtp> Help for \"(.+)\" :$",
                              QList<int>() << 1,
                              "<Mtp> Help for ""pouet pouet"""); // Token::HelpBegin
    _tokenRegexp << MtpRegExp("^<Mtp> End of help$",
                              "<Mtp> End of help"); // Token::HelpEndNormal
    _tokenRegexp << MtpRegExp("^<Mtp> No help available for \"(.+)\"$",
                              QList<int>() << 1,
                              "<Mtp> No help available for ""pouet pouet"""); // Token::HelpEndNoHelp
    _tokenRegexp << MtpRegExp("^(.*)$"); // Token::HelpLine

    _tokenRegexp << MtpRegExp("^<Mtp> Unknown or unregistered user \"("LOGIN_RE")\"$",
                              QList<int>() << 1,
                              "<Mtp> Unkown or unregistered user ""Fooman"""); // Token::UnknownUser
    _tokenRegexp << MtpRegExp("^<Mtp> User ("LOGIN_RE") is now known as ("LOGIN_RE")$",
                              QList<int>() << 1 << 2,
                              "<Mtp> User Fooman is now known as Footaise"); // Token::UserLoginRenamed
    _tokenRegexp << MtpRegExp("^<Mtp> Your login name is now ("LOGIN_RE")$",
                              QList<int>() << 1,
                              "<Mtp> You login name is now Fooman"); // Token::YourLoginRenamed

    _tokenRegexp << MtpRegExp("^<Mtp> ("LOGIN_RE") comes in !",
                              QList<int>() << 1,
                              "<Mtp> Fooman comes in !"); // Token::SomeoneComesIn
    _tokenRegexp << MtpRegExp("^<Mtp> ("LOGIN_RE") leaves(( \\((.+))\\)|) !$",
                              QList<int>() << 1 << 4,
                              "<Mtp> Fooman leaves (crustace) !"); // Token::SomeoneLeaves
    _tokenRegexp << MtpRegExp("^<Mtp> ("LOGIN_RE") disconnects(( \\((.+))\\)|) !$",
                              QList<int>() << 1 << 4,
                              "<Mtp> Fooman disconnects (connection reset by huitre)"); // Token::SomeoneDisconnects
    _tokenRegexp << MtpRegExp("^<Mtp> You leave <Mtp> Chat !$",
                              "<Mtp> You leave <Mtp> Chat !"); // Token::YouLeave

    _tokenRegexp << MtpRegExp("^<Mtp> You join channel ("LOGIN_RE")$",
                              QList<int>() << 1,
                              "<Mtp> You join channel Foochan"); // Token::YouJoinChannel
    _tokenRegexp << MtpRegExp("^<Mtp> You leave channel ("LOGIN_RE")$",
                              QList<int>() << 1,
                              "<Mtp> You leave channel Foochan"); // Token::YouLeaveChannel
    _tokenRegexp << MtpRegExp("^<Mtp> ("LOGIN_RE") joined channel ("LOGIN_RE")$",
                              QList<int>() << 1 << 2,
                              "<Mtp> Fooman joined channel Foochan"); // Token::SomeoneJoinChannel
    _tokenRegexp << MtpRegExp("^<Mtp> ("LOGIN_RE") fades into the shadows$",
                              QList<int>() << 1,
                              "<Mtp> Fooman fades into the shadows"); // Token::SomeoneFadesIntoTheShadows
    _tokenRegexp << MtpRegExp("^<Mtp> ("LOGIN_RE") comes in from channel ("LOGIN_RE")$",
                              QList<int>() << 1 << 2,
                              "<Mtp> Fooman coms in from channel Foochan"); // Token::SomeoneLeaveChannel
    _tokenRegexp << MtpRegExp("^<Mtp> ("LOGIN_RE") appears from the shadows$",
                              QList<int>() << 1,
                              "<Mtp> Fooman appears from the shadows"); // Token::SomeoneAppearsFromTheShadows

    _tokenRegexp << MtpRegExp("^<Mtp> You kick ("LOGIN_RE") out(( \\((.*)\\))|) !$",
                              QList<int>() << 1 << 4,
                              "<Mtp> You kick Fooman out (non merci)"); // Token::YouKickSomeone
    _tokenRegexp << MtpRegExp("^<Mtp> ("LOGIN_RE") is kicked out by ("LOGIN_RE")(( \\((.*)\\))|) !$",
                              QList<int>() << 1 << 2 << 5,
                              "<Mtp> Fooman is kicked out by Root (non merci) !"); // Token::SomeoneIsKicked
    _tokenRegexp << MtpRegExp("^<Mtp> You are kicked out by ("LOGIN_RE")(( \\((.*)\\))|) !$",
                              QList<int>() << 1 << 4,
                              "<Mtp> You are kicked out by Root (non merci) !"); // Token::YouAreKicked

    _tokenRegexp << MtpRegExp("^<Mtp> The current date and time are (.+)$",
                              QList<int>() << 1,
                              "<Mtp> The current date and time are Tuesday September 26 2006 19:22:48"); // Token::Date
    _tokenRegexp << MtpRegExp("^<Mtp> Your client is now \"(.+)\"$",
                              QList<int>() << 1,
                              "<Mtp> Your client is now \"upset\""); // Token::YourClientIs

    _tokenRegexp << MtpRegExp("^<Mtp> (.+)$",
                              QList<int>() << 1,
                              "<Mtp> Hi all, I'm system, guys!"); // Token::MtpSays
    _tokenRegexp << MtpRegExp("^<("LOGIN_RE")> (.+)$",
                              QList<int>() << 1 << 2,
                              "<Fooman> I see dead people. Seriously."); // Token::SomeoneSays

    _tokenRegexp << MtpRegExp("^\\|("LOGIN_RE")\\| (.+)$",
                              QList<int>() << 1 << 2,
                              "|Fooman| red pill or blue pill?"); // Token::Data

    _tokenRegexp << MtpRegExp(".*"); // Token::Unknown

    // Fill send regexp
    _sendTokenRegexp << QRegExp("^tell ("LOGIN_RE") (.*)$"); // SendToken::Tell
    _sendTokenRegexp << QRegExp("^reply (.*)$"); // SendToken::Reply
    _sendTokenRegexp << QRegExp("^sendmsg ("LOGIN_RE") (.*)$"); // SendToken::Sendmsg
}

void TokenFactory::dataReceived(const QString &data)
{
    // Analyze
    if (!_logged)
        analyzeBeforeLog(data);
    else
        analyzeAfterLog(data);
}

QStringList TokenFactory::split(const QString &message)
{
    QStringList strings; // Contains the result

    // For each sentence in message
    foreach (QString line, message.split('\n'))
    {
        QString trueMessage = line;
        QString prefix;
        int len = 256;

        // Split the message in function of the recognized form
        for (int i = 0; i < _sendTokenRegexp.count(); i++)
        {
            SendToken token = (SendToken) i;

            if (_sendTokenRegexp[i].exactMatch(message))
            {
                switch(token)
                {
                case SendToken_Tell:
                    trueMessage = _sendTokenRegexp[i].cap(2);
                    prefix = "tell " + _sendTokenRegexp[i].cap(1) + " ";
                    break;
                case SendToken_Reply:
                    trueMessage = _sendTokenRegexp[i].cap(1);
                    prefix = "reply ";
                    break;
                case SendToken_Sendmsg:
                    trueMessage = _sendTokenRegexp[i].cap(2);
                    prefix = "sendmsg " + _sendTokenRegexp[i].cap(1) + " ";
                    break;
                default:;
                }
                len -= prefix.length();
                break;
            }
        }

        while (!trueMessage.isEmpty())
        {
            strings << prefix + trueMessage.left(len);

            // Next
            if (trueMessage.length() > len)
                trueMessage = trueMessage.right(trueMessage.length() - len);
            else
                trueMessage = "";
        }
    }

    return strings;
}

void TokenFactory::analyzeBeforeLog(const QString &data)
{
    QVector<QString> arguments;
    QTime now = QTime::currentTime();
    if (_tokenRegexp[Token::IndicatedActiveServer].exactMatch(data))
        doTokenAnalyzed(Token::IndicatedActiveServer, 0, now);
    else if (_tokenRegexp[Token::LoginAsked].exactMatch(data))
        doTokenAnalyzed(Token::LoginAsked, 0, now);
    else if (_tokenRegexp[Token::PasswordAsked].indexIn(data) >= 0)
        doTokenAnalyzed(Token::PasswordAsked, 0, now);
    else if (data.endsWith('\n'))
    {
        // Remove \r\n
        QString newData = data.left(data.size() - 2);

        // Remove TELNET bytes
        static const QString telnetStr = QString("%1%2%3").arg(QChar(255)).arg(QChar(252)).arg(QChar(1));
        if (newData.indexOf(telnetStr) >= 0)
        {
            newData = newData.replace(telnetStr, "");
            newData = newData.trimmed();
        }
        if (newData.isEmpty())
            return;

        if (_tokenRegexp[Token::InvalidLogin].exactMatch(newData))
            doTokenAnalyzed(Token::InvalidLogin, 0, now);
        else if (_tokenRegexp[Token::IncorrectPassword].exactMatch(newData))
            doTokenAnalyzed(Token::IncorrectPassword, 0, now);
        else if (_tokenRegexp[Token::Welcome].exactMatch(newData))
        {
            _logged = true;
            doTokenAnalyzed(Token::Welcome, 0, now);
        }
        else if (_tokenRegexp[Token::MtpSays].exactMatch(newData))
        {
            doTokenAnalyzed(Token::MtpSays, 0, now);
        }
        else if (_tokenRegexp[Token::Unknown].exactMatch(newData))
            doTokenAnalyzed(Token::Unknown, 0, now);
    }
}

void TokenFactory::analyzeAfterLog(const QString &data)
{
    QVector<QString> arguments;
    int ticketID = -1;

    if (data.endsWith('\n'))
    {
        // Record time stamp
        QTime timeStamp = QTime::currentTime();

        // Remove \r\n
        QString newData = rightTrim(data);

        // Remove TELNET commands
        newData = newData.replace(QString("%1%1").arg(QChar(255)), QChar(255));

        if (newData.indexOf(QChar(7)) >= 0) // Beep char
            newData = newData.replace(QChar(7), "");

        // If away, get the time stamp
        if (_away)
        {
            if (_timeRegexp.exactMatch(newData))
            {
                timeStamp = QTime::fromString(_timeRegexp.cap(1), "hh:mm:ss");
                newData = _timeRegexp.cap(2);
            }
        }

        bool found = false;
        Token::Type token;

        Token::Type *tokens;
        int tokensSize;

        switch(_state)
        {
        case State_Normal:
            tokens = normalTokens;
            tokensSize = TOKEN_A_SIZE(normalTokens);
            break;
        case State_Who:
            tokens = whoTokens;
            tokensSize = TOKEN_A_SIZE(whoTokens);
            break;
        case State_History:
            tokens = historyTokens;
            tokensSize = TOKEN_A_SIZE(historyTokens);
            break;
        case State_Finger:
            tokens = fingerTokens;
            tokensSize = TOKEN_A_SIZE(fingerTokens);
            break;
        case State_Alias:
            tokens = aliasTokens;
            tokensSize = TOKEN_A_SIZE(aliasTokens);
            break;
        case State_Wall:
            tokens = wallTokens;
            tokensSize = TOKEN_A_SIZE(wallTokens);
            break;
        case State_Message:
            tokens = messageTokens;
            tokensSize = TOKEN_A_SIZE(messageTokens);
            break;
        case State_Help:
            tokens = helpTokens;
            tokensSize = TOKEN_A_SIZE(helpTokens);
            break;
        default:
            return;
        }

        for (int i = 0; i < tokensSize; i++)
        {
            token = tokens[i];

            MtpRegExp &regExp = _tokenRegexp[token];
            if (regExp.exactMatch(newData))
            {
                found = true;
                switch(token)
                {
                case Token::WallBegin:
                    _state = State_Wall;
                    break;
                case Token::WallEnd:
                    _state = State_Normal;
                    break;
                case Token::WhoBegin:
                    _state = State_Who;
                    if (tickets[Command_Who].count() > 0)
                    {
                        CommandTicket ticket = tickets[Command_Who][0];
                        ticketID = ticket.ID;
                    }
                    break;
                case Token::WhoEnd:
                    _state = State_Normal;
                    if (tickets[Command_Who].count() > 0)
                    {
                        CommandTicket ticket = tickets[Command_Who][0];
                        ticketID = ticket.ID;
                        tickets[Command_Who].removeAt(0);
                    }
                    break;
                case Token::WhoEndNoUser:
                    _state = State_Normal;
                    if (tickets[Command_Who].count() > 0)
                    {
                        CommandTicket ticket = tickets[Command_Who][0];
                        ticketID = ticket.ID;
                        tickets[Command_Who].removeAt(0);
                    }
                    break;
                case Token::WhoSeparator:
                    if (tickets[Command_Who].count() > 0)
                    {
                        CommandTicket ticket = tickets[Command_Who][0];
                        ticketID = ticket.ID;
                    }
                    break;
                case Token::WhoLine:
                    if (tickets[Command_Who].count() > 0)
                    {
                        CommandTicket ticket = tickets[Command_Who][0];
                        ticketID = ticket.ID;
                    }
                    break;
                case Token::HistoryBegin:
                    _state = State_History;
                    break;
                case Token::HistoryEnd:
                    _state = State_Normal;
                    break;
                case Token::FingerBegin:
                    _state = State_Finger;
                    break;
                case Token::FingerEnd:
                    _state = State_Normal;
                    break;
                case Token::YouAway:
                {
                    _away = true;
                    timeStamp = QTime::fromString(regExp.cap(1), "hh:mm:ss");
                    QStringList strList;
                    strList << regExp.cap(2);
                    QList<int> positions;
                    positions << 0;
                    emit newToken(Token(token, strList, positions,
                                        ticketID, timeStamp));
                    return;
                }
                case Token::YouBack:
                    _away = false;
                    break;
                case Token::SystemAliases:
                    _state = State_Alias;
                    break;
                case Token::UserAliases:
                    break;
                case Token::AliasesEnd:
                    _state = State_Normal;
                    break;
                case Token::MessageBegin:
                    _state = State_Message;
                    if (tickets[Command_ShowMsg].count() > 0)
                    {
                        CommandTicket ticket = tickets[Command_ShowMsg][0];
                        ticketID = ticket.ID;
                    }
                    break;
                case Token::MessageLine:
                    if (tickets[Command_ShowMsg].count() > 0)
                    {
                        CommandTicket ticket = tickets[Command_ShowMsg][0];
                        ticketID = ticket.ID;
                    }
                    break;
                case Token::MessageEnd:
                    if (tickets[Command_ShowMsg].count() > 0)
                    {
                        CommandTicket ticket = tickets[Command_ShowMsg][0];
                        ticketID = ticket.ID;
                        tickets[Command_ShowMsg].removeAt(0);
                    }
                    _state = State_Normal;
                    break;
                case Token::HelpBegin:
                    _state = State_Help;
                    if (tickets[Command_Help].count() > 0)
                    {
                        CommandTicket ticket = tickets[Command_Help][0];
                        ticketID = ticket.ID;
                    }
                    break;
                case Token::HelpEndNormal:
                case Token::HelpEndNoHelp:
                    if (tickets[Command_Help].count() > 0)
                    {
                        CommandTicket ticket = tickets[Command_Help][0];
                        ticketID = ticket.ID;
                        tickets[Command_Help].removeAt(0);
                    }
                    _state = State_Normal;
                    break;
                case Token::HelpLine:
                    if (tickets[Command_Help].count() > 0)
                    {
                        CommandTicket ticket = tickets[Command_Help][0];
                        ticketID = ticket.ID;
                    }
                    break;
                case Token::Date:
                    if (tickets[Command_Date].count() > 0)
                    {
                        CommandTicket ticket = tickets[Command_Date][0];
                        ticketID = ticket.ID;
                        tickets[Command_Date].removeAt(0);
                    }
                    break;
                case Token::YourClientIs:
                    if (tickets[Command_SetClient].count() > 0)
                    {
                        CommandTicket ticket = tickets[Command_SetClient][0];
                        ticketID = ticket.ID;
                        tickets[Command_SetClient].removeAt(0);
                    }
                    break;
                default:;
                }
                if (found)
                    break;
            }
        }

        if (!found)
            token = Token::Unknown;

        doTokenAnalyzed(token, ticketID, timeStamp);
    }
}

void TokenFactory::doTokenAnalyzed(Token::Type tokenType, int reservation, const QTime &timeStamp)
{
    MtpRegExp &regExp = _tokenRegexp[tokenType];

    QList<int> positions;
    for (int i = 0; i < regExp.capturedTexts().count(); ++i)
    {
        if (regExp.cap(i) == "")
            positions << -1;
        else
            positions << regExp.pos(i);
    }
    emit newToken(Token(tokenType, regExp.capturedTexts(), positions,
                        reservation, timeStamp));
}

void TokenFactory::reset()
{
    _logged = false;
    _state = State_Normal;
    _away = false;
    clearTickets();
}

int TokenFactory::requestTicket(Command command)
{
    CommandTicket newTicket = { command, tickets[command].count() };

    for (int i = 0; i < tickets[command].count(); i++)
    {
        bool found = false;
        foreach (CommandTicket ticket, tickets[command])
            if (i == ticket.ID)
            {
                found = true;
                break;
            }

        if (!found)
        {
            newTicket.ID = i;
            break;
        }
    }

    tickets[command] << newTicket;
    return newTicket.ID;
}

void TokenFactory::clearTickets()
{
    for (int i =0; i < Command_Count; i++)
        tickets[i].clear();
}

QString TokenFactory::rightTrim(const QString &str) const
{
    int length = str.length();
    while (length > 0 &&
           (str[length - 1] == '\r' || str[length - 1] == '\n'))
        length--;

    if (length)
        return str.mid(0, length);
    else
        return "";
}
