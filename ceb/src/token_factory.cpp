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

#include "common.h"
#include <QVector>
#include <QStringList>
#include <QMessageBox>

#include "token_info.h"

#include "token_factory.h"

//#define LOGIN_RE "[a-zA-Z][a-zA-Z\\d]*" // MTP style
#define LOGIN_RE "[a-zA-Z\\d]+"
#define ID_RE "[a-zA-Z][a-zA-Z\\d]+"
#define SRV_RE QString("%1%2%3").arg("^<").arg(_serverName).arg("> ")

#define TOKEN_A_SIZE(a) sizeof(a) / sizeof(a[0])

const TokenFactory TokenFactory::defaultFactory;

Token::Type TokenFactory::whoTokens[] = {
    Token::WhoSeparator,
    Token::WhoLine,
    Token::WhoEnd,
    Token::WhoEndNoUser
};

Token::Type TokenFactory::groupsTokens[] = {
    Token::GroupsSeparator,
    Token::GroupsEnd,
    Token::GroupsLine
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
    Token::GroupsBegin,
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
    Token::SomeoneGroup,
    Token::UnregisteredUser,
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
    : _logged(false), _state(State_Normal), _away(false),
      _serverType(Mtp), _serverName("Mtp"),
      _timeRegexp(QRegExp("^(\\d+:\\d+:\\d+) (.*)$"))
{
    createTokenRegularExpressions();
}

void TokenFactory::createTokenRegularExpressions()
{
    _tokenRegexp.clear();

    // Pre-login tokens
    _tokenRegexp.insert(Token::IndicatedActiveServer,
                        MtpRegExp("^<"ID_RE"> (Active|Main) server (is|move) at ([^:]+):(.+)$",
                                  QList<int>() << 1 << 3 << 4,
                                  "<Mtp> Active server is at a.b.c:4000"));
    _tokenRegexp.insert(Token::LoginAsked,
                        MtpRegExp("^<"ID_RE"> Login: $",
                                  "<Mtp> Login: "));
    _tokenRegexp.insert(Token::InvalidLogin,
                        MtpRegExp("^<"ID_RE"> Invalid login, choose another one$",
                                  "<Mtp> Invalid login, choose another one"));
    _tokenRegexp.insert(Token::OnlyRegisteredUsers,
                        MtpRegExp("^<"ID_RE"> Only registered users are allowed to login at the moment...$",
                                  "<Mtp> Only registered users are allowed to login at the moment..."));
    _tokenRegexp.insert(Token::PasswordAsked,
                        MtpRegExp("<"ID_RE"> Password: $", // no ^, because there are telnet code before <Mtp> Password:
                                  "<Mtp> Password: "));
    _tokenRegexp.insert(Token::IncorrectPassword,
                        MtpRegExp("^<"ID_RE"> Incorrect password$",
                                  "<Mtp> Incorrect password"));
    _tokenRegexp.insert(Token::Welcome,
                        MtpRegExp("^<("ID_RE")> Welcome, ("LOGIN_RE")\\.$",
                                  QList<int>() << 1 << 2,
                                  "<Mtp> Welcome, Fooman."));

    // Post-login tokens
    _tokenRegexp.insert(Token::Topic,
                        MtpRegExp(SRV_RE + "("LOGIN_RE") topic : (.+)$",
                                  QList<int>() << 1 << 2,
                                  "<Mtp> Hall topic : Salut les poulets !"));
    _tokenRegexp.insert(Token::YouSetTopic,
                        MtpRegExp(SRV_RE + "You set channel ("LOGIN_RE") topic to (.*)$",
                                  QList<int>() << 1 << 2,
                                  "<Mtp> You set channel Hall topic to Salut les poulets !"));
    _tokenRegexp.insert(Token::SomeoneSetTopic,
                        MtpRegExp(SRV_RE + "("LOGIN_RE") set channel ("LOGIN_RE") topic to (.*)$",
                                  QList<int>() << 1 << 2 << 3,
                                  "<Mtp> Fooman set channel Hall topic to Salut les poulets !"));

    _tokenRegexp.insert(Token::SomeoneTellsYou,
                        MtpRegExp(SRV_RE + "("LOGIN_RE") tells you: (.+)$",
                                  QList<int>() << 1 << 2,
                                  "<Mtp> Fooman tells you: espece de truite"));
    _tokenRegexp.insert(Token::YouTellToSomeone,
                        MtpRegExp(SRV_RE + "You tell ("LOGIN_RE"): (.+)$",
                                  QList<int>() << 1 << 2,
                                  "<Mtp> You tell Fooman: espece de truite"));
    _tokenRegexp.insert(Token::SomeoneAsksYou,
                        MtpRegExp(SRV_RE + "("LOGIN_RE") asks you: (.+)$",
                                  QList<int>() << 1 << 2,
                                  "<Mtp> Fooman asks you: es-tu un saumon ou une truite ?"));
    _tokenRegexp.insert(Token::YouAskToSomeone,
                        MtpRegExp(SRV_RE + "You ask ("LOGIN_RE"): (.+)$",
                                  QList<int>() << 1 << 2,
                                  "<Mtp> You ask Fooman: es-tu une truite ou un saumon ?"));
    _tokenRegexp.insert(Token::SomeoneReplies,
                        MtpRegExp(SRV_RE + "("LOGIN_RE") replies: (.+)$",
                                  QList<int>() << 1 << 2,
                                  "<Mtp> Fooman replies: vive les levures !"));
    _tokenRegexp.insert(Token::YouReply,
                        MtpRegExp(SRV_RE + "You reply to ("LOGIN_RE"): (.+)$",
                                  QList<int>() << 1 << 2,
                                  "<Mtp> You reply to Fooman: vive les levures !"));

    _tokenRegexp.insert(Token::SomeoneBeepsYou,
                        MtpRegExp(SRV_RE + "("LOGIN_RE") beeps you$",
                                  QList<int>() << 1,
                                  "<Mtp> Fooman beeps you"));

    _tokenRegexp.insert(Token::WallBegin,
                        MtpRegExp(SRV_RE + "Wall :$",
                                  "<Mtp> Wall :"));
    _tokenRegexp.insert(Token::WallEnd,
                        MtpRegExp(SRV_RE + "End of Wall$",
                                  "<Mtp> End of Wall"));
    _tokenRegexp.insert(Token::WallLine,
                        MtpRegExp("^([^:]*) *(\\d+:\\d+:\\d+) *(\\S+) *(.*)$",
                                  QList<int>() << 1 << 2 << 3 << 4,
                                  "Fri Feb 04 2005 16:48:01 Fooman      Hello World!"));

    _tokenRegexp.insert(Token::WhoBegin,
                        MtpRegExp("^ Login +Group +Channel +Idle +On +For +C +From +host$",
                                  " Login    Group   Channel   Idle  On For C              From host"));
    _tokenRegexp.insert(Token::WhoEnd,
                        MtpRegExp(SRV_RE + "There (are|is) currently (\\d+) users?(( in channel ("LOGIN_RE"))|)$",
                                  QList<int>() << 2 << 5,
                                  "<Mtp> There are currently 12 users in channel Hall"));
    _tokenRegexp.insert(Token::WhoEndNoUser,
                        MtpRegExp(SRV_RE + "There is nobody in channel ("LOGIN_RE")$",
                                  QList<int>() << 1,
                                  "<Mtp> There is nobody in channel Hall"));
    _tokenRegexp.insert(Token::WhoSeparator,
                        MtpRegExp("^-+ -+ -+ -+ -+ -+ -+$",
                                  "-------- -------- -------- ------ ------ - ------------------------------------"));
    _tokenRegexp.insert(Token::WhoLine,
                        MtpRegExp("^("LOGIN_RE") *(\\w+) *(\\w+) *([^ ]+) *([^ ]+) *([^ ]+) *(.+)$",
                                  QList<int>() << 1 << 2 << 3 << 4 << 5 << 6 << 7));

    _tokenRegexp.insert(Token::GroupsBegin,
                        MtpRegExp("^ Group +Leader +Lv +Full Name$",
                                  " Group      Leader    Lv            Full Name"));
    _tokenRegexp.insert(Token::GroupsSeparator,
                        MtpRegExp("^-+ -+ -+ -+$",
                                  "-------- -------- -- --------------------------------"));
    _tokenRegexp.insert(Token::GroupsLine,
                        MtpRegExp("^("LOGIN_RE") *("LOGIN_RE") *(\\-?\\d+) *(.+)$",
                                  QList<int>() << 1 << 2 << 3 << 4,
                                  "System   Root         12 Administrators"));
    _tokenRegexp.insert(Token::GroupsEnd,
                        MtpRegExp(SRV_RE + "There are (\\d+) groups$",
                                  QList<int>() << 1,
                                  "<Mtp> There are 9 groups"));

    _tokenRegexp.insert(Token::HistoryBegin,
                        MtpRegExp(SRV_RE + "History :$",
                                  "<Mtp> History :"));
    _tokenRegexp.insert(Token::HistoryEnd,
                        MtpRegExp(SRV_RE + "End of History$",
                                  "<Mtp> End of History"));
    _tokenRegexp.insert(Token::HistoryLine,
                        MtpRegExp("^(\\S+) *(\\S+) *(\\S+) *(\\S+) *(\\S+) *(.+)$",
                                  QList<int>() << 1 << 2 << 3 << 4 << 5 << 6));

    _tokenRegexp.insert(Token::FingerBegin,
                        MtpRegExp("^Login *: ("LOGIN_RE")$",
                                  QList<int>() << 1,
                                  "Login  : Fooman"));
    _tokenRegexp.insert(Token::FingerEnd,
                        MtpRegExp(SRV_RE + "End of finger$",
                                  "<Mtp> End of finger"));
    _tokenRegexp.insert(Token::FingerLine,
                        MtpRegExp("^(.+)$",
                                  "Blablabla xann blablabla"));

    _tokenRegexp.insert(Token::SomeoneShouts,
                        MtpRegExp(SRV_RE + "("LOGIN_RE") shouts: (.+)$",
                                  QList<int>() << 1 << 2,
                                  "<Mtp> Fooman shouts: I want to be freeee!"));
    _tokenRegexp.insert(Token::YouShout,
                        MtpRegExp(SRV_RE + "You shout: (.+)$",
                                  QList<int>() << 1,
                                  "<Mtp> You shout: get me out of this crazy software!"));

    _tokenRegexp.insert(Token::SomeoneAway,
                        MtpRegExp(SRV_RE + "("LOGIN_RE") is away$",
                                  QList<int>() << 1,
                                  "<Mtp> Fooman is away"));
    _tokenRegexp.insert(Token::YouAway,
                        MtpRegExp("^(\\d+:\\d+:\\d+) (" + QString("%1%2%3").arg("<").arg(_serverName).arg("> ") + "You are away)$",
                                  "<Mtp> You are away"));
    _tokenRegexp.insert(Token::SomeoneBack,
                        MtpRegExp(SRV_RE + "("LOGIN_RE") is back$",
                                  QList<int>() << 1,
                                  "<Mtp> Fooman is back"));
    _tokenRegexp.insert(Token::YouBack,
                        MtpRegExp(SRV_RE + "You are back$",
                                  "<Mtp> You are back"));
    _tokenRegexp.insert(Token::SomeoneAwayWarning,
                        MtpRegExp(SRV_RE + "("LOGIN_RE") is away and may not be hearing you$",
                                  QList<int>() << 1,
                                  "<Mtp> Fooman is away and may not be hearing you"));

    _tokenRegexp.insert(Token::SystemAliases,
                        MtpRegExp(SRV_RE + "System Aliases : $",
                                  "<Mtp> System aliases"));
    _tokenRegexp.insert(Token::UserAliases,
                        MtpRegExp(SRV_RE + "User Aliases : $",
                                  "<Mtp> User Aliases : "));
    _tokenRegexp.insert(Token::AliasesEnd,
                        MtpRegExp(SRV_RE + "End of aliases$",
                                  "<Mtp> End of aliases"));
    _tokenRegexp.insert(Token::NoUserAlias,
                        MtpRegExp(SRV_RE + "You don't have any alias !$",
                                  "<Mtp> You don't have any alias !"));
    _tokenRegexp.insert(Token::AliasLine,
                        MtpRegExp("^([^ ]+)( *.+)$",
                                  QList<int>() << 1 << 2,
                                  "wtf    wtf o_O"));

    _tokenRegexp.insert(Token::MessageBegin,
                        MtpRegExp(SRV_RE + "Your message\\(s\\) :$",
                                  "<Mtp> You message(s) :"));
    _tokenRegexp.insert(Token::MessageLine,
                        MtpRegExp("^ *(\\S+) ([^:]+ \\d+:\\d+:\\d+) ("LOGIN_RE") : (.+)$",
                                  QList<int>() << 1 << 2 << 3 << 4));
    _tokenRegexp.insert(Token::MessageEnd,
                        MtpRegExp(SRV_RE + "You have (\\d+) messages?$",
                                  QList<int>() << 1,
                                  "<Mtp> You have 14 messages"));
    _tokenRegexp.insert(Token::NoMessage,
                        MtpRegExp(SRV_RE + "You have no message !$",
                                  "<Mtp> You have no message"));
    _tokenRegexp.insert(Token::MessageReceived,
                        MtpRegExp(SRV_RE + "("LOGIN_RE") sends a message to you: (.+)$",
                                  QList<int>() << 1 << 2,
                                  "<Mtp> Fooman sends a message to you: huitre"));
    _tokenRegexp.insert(Token::AllMessagesCleared,
                        MtpRegExp(SRV_RE + "You clear your message\\(s\\) !$",
                                  "<Mtp> You clear your messages"));
    _tokenRegexp.insert(Token::MessageCleared,
                        MtpRegExp(SRV_RE + "You clear message (\\d+) !$",
                                  QList<int>() << 1));
    _tokenRegexp.insert(Token::MessagesCleared,
                        MtpRegExp(SRV_RE + "You clear messages (\\d+) to (\\d+) !$",
                                  QList<int>() << 1 << 2,
                                  "<Mtp> You clear messages 1 to 5 !"));

    _tokenRegexp.insert(Token::HelpBegin,
                        MtpRegExp(SRV_RE + "Help for \"(.+)\" :$",
                                  QList<int>() << 1,
                                  "<Mtp> Help for ""pouet pouet"""));
    _tokenRegexp.insert(Token::HelpEndNormal,
                        MtpRegExp(SRV_RE + "End of help$",
                                  "<Mtp> End of help"));
    _tokenRegexp.insert(Token::HelpEndNoHelp,
                        MtpRegExp(SRV_RE + "No help available for \"(.+)\"$",
                                  QList<int>() << 1,
                                  "<Mtp> No help available for ""pouet pouet"""));
    _tokenRegexp.insert(Token::HelpLine, MtpRegExp("^(.*)$"));

    _tokenRegexp.insert(Token::UnknownUser,
                        MtpRegExp(SRV_RE + "Unknown or unregistered user \"("LOGIN_RE")\"$",
                                  QList<int>() << 1,
                                  "<Mtp> Unkown or unregistered user ""Fooman"""));
    _tokenRegexp.insert(Token::UserLoginRenamed,
                        MtpRegExp(SRV_RE + "User ("LOGIN_RE") is now known as ("LOGIN_RE")$",
                                  QList<int>() << 1 << 2,
                                  "<Mtp> User Fooman is now known as Footaise"));
    _tokenRegexp.insert(Token::YourLoginRenamed,
                        MtpRegExp(SRV_RE + "Your login name is now ("LOGIN_RE")$",
                                  QList<int>() << 1,
                                  "<Mtp> You login name is now Fooman"));

    _tokenRegexp.insert(Token::SomeoneComesIn,
                        MtpRegExp(SRV_RE + "("LOGIN_RE") comes in !",
                                  QList<int>() << 1,
                                  "<Mtp> Fooman comes in !"));
    _tokenRegexp.insert(Token::SomeoneLeaves,
                        MtpRegExp(SRV_RE + "("LOGIN_RE") leaves(( \\((.+))\\)|) !$",
                                  QList<int>() << 1 << 4,
                                  "<Mtp> Fooman leaves (crustace) !"));
    _tokenRegexp.insert(Token::SomeoneDisconnects,
                        MtpRegExp(SRV_RE + "("LOGIN_RE") disconnects(( \\((.+))\\)|) !$",
                                  QList<int>() << 1 << 4,
                                  "<Mtp> Fooman disconnects (connection reset by huitre)"));
    _tokenRegexp.insert(Token::YouLeave,
                        MtpRegExp(SRV_RE + "You leave " + QString("%1%2%3").arg("<").arg(_serverName).arg("> ") + "Chat !$",
                                  "<Mtp> You leave <Mtp> Chat !"));

    _tokenRegexp.insert(Token::YouJoinChannel,
                        MtpRegExp(SRV_RE + "You join channel ("LOGIN_RE")$",
                                  QList<int>() << 1,
                                  "<Mtp> You join channel Foochan"));
    _tokenRegexp.insert(Token::YouLeaveChannel,
                        MtpRegExp(SRV_RE + "You leave channel ("LOGIN_RE")$",
                                  QList<int>() << 1,
                                  "<Mtp> You leave channel Foochan"));
    _tokenRegexp.insert(Token::SomeoneJoinChannel,
                        MtpRegExp(SRV_RE + "("LOGIN_RE") joined channel ("LOGIN_RE")$",
                                  QList<int>() << 1 << 2,
                                  "<Mtp> Fooman joined channel Foochan"));
    _tokenRegexp.insert(Token::SomeoneFadesIntoTheShadows,
                        MtpRegExp(SRV_RE + "("LOGIN_RE") fades into the shadows$",
                                  QList<int>() << 1,
                                  "<Mtp> Fooman fades into the shadows"));
    _tokenRegexp.insert(Token::SomeoneLeaveChannel,
                        MtpRegExp(SRV_RE + "("LOGIN_RE") comes in from channel ("LOGIN_RE")$",
                                  QList<int>() << 1 << 2,
                                  "<Mtp> Fooman coms in from channel Foochan"));
    _tokenRegexp.insert(Token::SomeoneAppearsFromTheShadows,
                        MtpRegExp(SRV_RE + "("LOGIN_RE") appears from the shadows$",
                                  QList<int>() << 1,
                                  "<Mtp> Fooman appears from the shadows"));

    _tokenRegexp.insert(Token::YouKickSomeone,
                        MtpRegExp(SRV_RE + "You kick ("LOGIN_RE") out(( \\((.*)\\))|) !$",
                                  QList<int>() << 1 << 4,
                                  "<Mtp> You kick Fooman out (non merci) !"));
    _tokenRegexp.insert(Token::SomeoneIsKicked,
                        MtpRegExp(SRV_RE + "("LOGIN_RE") is kicked out by ("LOGIN_RE")(( \\((.*)\\))|) !$",
                                  QList<int>() << 1 << 2 << 5,
                                  "<Mtp> Fooman is kicked out by Root (non merci) !"));
    _tokenRegexp.insert(Token::YouAreKicked,
                        MtpRegExp(SRV_RE + "You are kicked out by ("LOGIN_RE")(( \\((.*)\\))|) !$",
                                  QList<int>() << 1 << 4,
                                  "<Mtp> You are kicked out by Root (non merci) !"));

    _tokenRegexp.insert(Token::Date,
                        MtpRegExp(SRV_RE + "The current date and time are (.+)$",
                                  QList<int>() << 1,
                                  "<Mtp> The current date and time are Tuesday September 26 2006 19:22:48"));
    _tokenRegexp.insert(Token::YourClientIs,
                        MtpRegExp(SRV_RE + "Your client is now \"(.+)\"$",
                                  QList<int>() << 1,
                                  "<Mtp> Your client is now \"CeB\""));

    _tokenRegexp.insert(Token::SomeoneGroup,
                        MtpRegExp(SRV_RE + "("LOGIN_RE")'group is ("LOGIN_RE")$",
                                  QList<int>() << 1 << 2,
                                  "<Mtp> Foo'group is Tell"));

    _tokenRegexp.insert(Token::UnregisteredUser,
                        MtpRegExp(SRV_RE + "Unregistered user ("LOGIN_RE")$",
                                  QList<int>() << 1,
                                  "<Mtp> Unregistered user Foo"));

    _tokenRegexp.insert(Token::MtpSays,
                        MtpRegExp(SRV_RE + "(.+)$",
                                  QList<int>() << 1,
                                  "<Mtp> Hi all, I'm system, guys!"));
    _tokenRegexp.insert(Token::SomeoneSays,
                        MtpRegExp("^<("LOGIN_RE")> (.+)$",
                                  QList<int>() << 1 << 2,
                                  "<Fooman> I see dead people. Seriously."));

    _tokenRegexp.insert(Token::Data,
                        MtpRegExp("^\\|("LOGIN_RE")\\| (.+)$",
                                  QList<int>() << 1 << 2,
                                  "|Fooman| red pill or blue pill?"));

    _tokenRegexp.insert(Token::Unknown, MtpRegExp(".*"));

    // Fill send regexp
    _sendTokenRegexp.clear();
    _sendTokenRegexp << QRegExp("^" + serverCommand("tell") + " ("LOGIN_RE") (.*)$"); // SendToken::Tell
    _sendTokenRegexp << QRegExp("^" + serverCommand("reply") + " (.*)$"); // SendToken::Reply
    _sendTokenRegexp << QRegExp("^" + serverCommand("sendmsg") + " ("LOGIN_RE") (.*)$"); // SendToken::Sendmsg
}

void TokenFactory::dataReceived(const QString &data)
{
    // Analyze
    if (!_logged)
        analyzeBeforeLogin(data);
    else
        analyzeAfterLogin(data);
}

QString TokenFactory::serverCommand(const QString &command) const
{
    if (_serverType == Mtp)
        return command;
    else
        return "." + command;
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
                    prefix = serverCommand("tell " + _sendTokenRegexp[i].cap(1) + " ");
                    break;
                case SendToken_Reply:
                    trueMessage = _sendTokenRegexp[i].cap(1);
                    prefix = serverCommand("reply ");
                    break;
                case SendToken_Sendmsg:
                    trueMessage = _sendTokenRegexp[i].cap(2);
                    prefix = serverCommand("sendmsg " + _sendTokenRegexp[i].cap(1) + " ");
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

void TokenFactory::analyzeBeforeLogin(const QString &data)
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
        else if (_tokenRegexp[Token::OnlyRegisteredUsers].exactMatch(newData))
            doTokenAnalyzed(Token::OnlyRegisteredUsers, 0, now);
        else if (_tokenRegexp[Token::IncorrectPassword].exactMatch(newData))
            doTokenAnalyzed(Token::IncorrectPassword, 0, now);
        else if (_tokenRegexp[Token::Welcome].exactMatch(newData))
        {
            _logged = true;
            doTokenAnalyzed(Token::Welcome, 0, now);
        }
        else if (_tokenRegexp[Token::MtpSays].exactMatch(newData))
            doTokenAnalyzed(Token::MtpSays, 0, now);
        else if (_tokenRegexp[Token::Unknown].exactMatch(newData))
            doTokenAnalyzed(Token::Unknown, 0, now);
    }
}

void TokenFactory::analyzeAfterLogin(const QString &data)
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

        // If away, remove the timestamp
        if (_away && _timeRegexp.exactMatch(newData))
            newData = _timeRegexp.cap(2);

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
        case State_Groups:
            tokens = groupsTokens;
            tokensSize = TOKEN_A_SIZE(groupsTokens);
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
                // set state for begin tokens and do other treatments
                switch(token)
                {
                case Token::WallBegin:
                    _state = State_Wall;
                    break;
                case Token::WhoBegin:
                    _state = State_Who;
                    break;
                case Token::GroupsBegin:
                    _state = State_Groups;
                    break;
                case Token::HistoryBegin:
                    _state = State_History;
                    break;
                case Token::FingerBegin:
                    _state = State_Finger;
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
                case Token::MessageBegin:
                    _state = State_Message;
                    break;
                case Token::HelpBegin:
                    _state = State_Help;
                    break;
                default:;
                }

                // Ticket managing
                switch (_state)
                {
                case State_Normal:
                    // Stateless tickets
                    switch (token)
                    {
                    case Token::Date:
                        if (tickets[Command_Date].count() > 0)
                            ticketID = tickets[Command_Date].takeFirst().ID;
                        break;
                    case Token::YourClientIs:
                        if (tickets[Command_SetClient].count() > 0)
                            ticketID = tickets[Command_SetClient].takeFirst().ID;
                        break;
                    case Token::SomeoneGroup:
                    case Token::UnregisteredUser:
                        if (tickets[Command_GetGroup].count() > 0)
                            ticketID = tickets[Command_GetGroup].takeFirst().ID;
                        break;
                    default:;
                    }
                    break;
                case State_Who:
                    if (tickets[Command_Who].count() > 0)
                    {
                        if (token == Token::WhoEnd || token == Token::WhoEndNoUser)
                            ticketID = tickets[Command_Who].takeFirst().ID;
                        else
                            ticketID = tickets[Command_Who].first().ID;
                    }
                    break;
                case State_Groups:
                    if (tickets[Command_Groups].count() > 0)
                    {
                        if (token == Token::GroupsEnd)
                            ticketID = tickets[Command_Groups].takeFirst().ID;
                        else
                            ticketID = tickets[Command_Groups].first().ID;
                    }
                    break;
                case State_Message:
                    if (tickets[Command_ShowMsg].count() > 0)
                    {
                        if (token = Token::MessageEnd)
                            ticketID = tickets[Command_ShowMsg].takeFirst().ID;
                        else
                            ticketID = tickets[Command_ShowMsg].first().ID;
                    }
                    break;
                case State_Help:
                    if (tickets[Command_Help].count() > 0)
                    {
                        if (token == Token::HelpEndNormal || token == Token::HelpEndNoHelp)
                            ticketID = tickets[Command_Help].takeFirst().ID;
                        else
                            ticketID = tickets[Command_Help].first().ID;
                    }
                    break;
                default:;
                }

                // Set state for end tokens
                switch(token)
                {
                case Token::WallEnd:
                case Token::WhoEnd:
                case Token::WhoEndNoUser:
                case Token::GroupsEnd:
                case Token::HistoryEnd:
                case Token::FingerEnd:
                case Token::AliasesEnd:
                case Token::MessageEnd:
                case Token::HelpEndNormal:
                case Token::HelpEndNoHelp:
                    _state = State_Normal;
                    break;
                default:;
                }

                break;
            }
        }

        if (!found)
            token = Token::Unknown;

        doTokenAnalyzed(token, ticketID, timeStamp);
    }
}

void TokenFactory::doTokenAnalyzed(Token::Type tokenType, int ticketID, const QTime &timeStamp)
{
    MtpRegExp &regExp = _tokenRegexp[tokenType];

    QStringList capturedTexts = regExp.capturedTexts();
    QList<int> positions;
    for (int i = 0; i < regExp.capturedTexts().count(); ++i)
    {
        if (regExp.cap(i) == "")
            positions << -1;
        else
            positions << regExp.pos(i);
    }
    // We reconstruct all regular expressions
    if (tokenType == Token::Welcome)
    {
        _serverName = regExp.cap(1);
        if (!_serverName.compare("SoR", Qt::CaseInsensitive))
            _serverType = Ryzom;
        else // Other server type => considerate it like a Mtp server
            _serverType = Mtp;
        createTokenRegularExpressions();
    }

    emit newToken(Token(tokenType, capturedTexts, positions,
                        ticketID, timeStamp));
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
