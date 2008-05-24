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

#include <QObject>

#include "token_info.h"

QString TokenInfo::tokenToIDString(Token::Type token)
{
    switch (token)
    {
    case Token::IndicatedActiveServer: return "IndicatedActiveServer";
    case Token::LoginAsked: return "LoginAsked";
    case Token::InvalidLogin: return "InvalidLogin";
    case Token::PasswordAsked: return "PasswordAsked";
    case Token::IncorrectPassword: return "IncorrectPassword";
    case Token::Welcome: return "Welcome";
    case Token::Topic: return "Topic";
    case Token::YouSetTopic: return "YouSetTopic";
    case Token::SomeoneSetTopic: return "SomeoneSetTopic";
    case Token::SomeoneTellsYou: return "SomeoneTellsYou";
    case Token::YouTellToSomeone: return "YouTellToSomeone";
    case Token::SomeoneAsksYou: return "SomeoneAsksYou";
    case Token::YouAskToSomeone: return "YouAskToSomeone";
    case Token::SomeoneReplies: return "SomeoneReplies";
    case Token::YouReply: return "YouReply";

    case Token::SomeoneBeepsYou: return "SomeoneBeepsYou";

    case Token::WallBegin: return "WallBegin";
    case Token::WallEnd: return "WallEnd";
    case Token::WallLine: return "WallLine";

    case Token::WhoBegin: return "WhoBegin";
    case Token::WhoEnd: return "WhoEnd";
    case Token::WhoEndNoUser: return "WhoEndNoUser";
    case Token::WhoSeparator: return "WhoSeparator";
    case Token::WhoLine: return "WhoLine";

    case Token::HistoryBegin: return "HistoryBegin";
    case Token::HistoryEnd: return "HistoryEnd";
    case Token::HistoryLine: return "HistoryLine";

    case Token::FingerBegin: return "FingerBegin";
    case Token::FingerEnd: return "FingerEnd";
    case Token::FingerLine: return "FingerLine";

    case Token::SomeoneShouts: return "SomeoneShouts";
    case Token::YouShout: return "YouShout";

    case Token::SomeoneAway: return "SomeoneAway";
    case Token::YouAway: return "YouAway";
    case Token::SomeoneBack: return "SomeoneBack";
    case Token::YouBack: return "YouBack";
    case Token::SomeoneAwayWarning: return "SomeoneAwayWarning";

    case Token::SystemAliases: return "SystemAliases";
    case Token::UserAliases: return "UserAliases";
    case Token::AliasesEnd: return "AliasesEnd";
    case Token::NoUserAlias: return "NoUserAlias";
    case Token::AliasLine: return "AliasLine";

    case Token::MessageBegin: return "MessageBegin";
    case Token::MessageLine: return "MessageLine";
    case Token::MessageEnd: return "MessageEnd";
    case Token::NoMessage: return "NoMessage";
    case Token::MessageReceived: return "MessageReceived";
    case Token::AllMessagesCleared: return "AllMessagesCleared";
    case Token::MessageCleared: return "MessageCleared";
    case Token::MessagesCleared: return "MessagesCleared";

    case Token::HelpBegin: return "HelpBegin";
    case Token::HelpEndNormal: return "HelpEndNormal";
    case Token::HelpEndNoHelp: return "HelpEndNoHelp";
    case Token::HelpLine: return "HelpLine";

    case Token::UnknownUser: return "UnknownUser";
    case Token::UserLoginRenamed: return "UserLoginRenamed";
    case Token::YourLoginRenamed: return "YourLoginRenamed";

    case Token::SomeoneComesIn: return "SomeoneComesIn";
    case Token::SomeoneLeaves: return "SomeoneLeaves";
    case Token::SomeoneDisconnects: return "SomeoneDisconnects";
    case Token::YouLeave: return "YouLeave";

    case Token::YouJoinChannel: return "YouJoinChannel";
    case Token::YouLeaveChannel: return "YouLeaveChannel";
    case Token::SomeoneJoinChannel: return "SomeoneJoinChannel";
    case Token::SomeoneFadesIntoTheShadows: return "SomeoneFadeIntoTheShadows";
    case Token::SomeoneLeaveChannel: return "SomeoneLeaveChannel";
    case Token::SomeoneAppearsFromTheShadows: return "SomeoneAppearsFromTheShadows";

    case Token::YouKickSomeone: return "YouKickSomeone";
    case Token::SomeoneIsKicked: return "SomeoneIsKicked";
    case Token::YouAreKicked: return "YouAreKicked";

    case Token::Date: return "Date";
    case Token::YourClientIs: return "YourClientIs";

    case Token::MtpSays: return "MtpSays";
    case Token::SomeoneSays: return "SomeoneSays";

    case Token::Data: return "Data";

    case Token::Unknown: return "Unknown";
    default: return "";
    }
}

Token::Type TokenInfo::IDStringToToken(const QString &ID)
{
    for (int i = 0; i < Token::End; ++i)
        if (tokenToIDString((Token::Type) i).toUpper() == ID.toUpper())
            return (Token::Type) i;
    return Token::Unknown;
}

QString TokenInfo::tokenToDisplayString(Token::Type token)
{
    switch (token)
    {
    case Token::IndicatedActiveServer: return QObject::tr("Indicated active server");
    case Token::LoginAsked: return QObject::tr("Login asked");
    case Token::InvalidLogin: return QObject::tr("Invalid login");
    case Token::PasswordAsked: return QObject::tr("Password asked");
    case Token::IncorrectPassword: return QObject::tr("Incorrect password");
    case Token::Welcome: return QObject::tr("Welcome msg.");
    case Token::Topic: return QObject::tr("Indicated topic");
    case Token::YouSetTopic: return QObject::tr("You set topic");
    case Token::SomeoneSetTopic: return QObject::tr("Someone set topic");
    case Token::SomeoneTellsYou: return QObject::tr("Someone tells you");
    case Token::YouTellToSomeone: return QObject::tr("You tell someone");
    case Token::SomeoneAsksYou: return QObject::tr("Someone asks you");
    case Token::YouAskToSomeone: return QObject::tr("You ask someone");
    case Token::SomeoneReplies: return QObject::tr("Someone replies");
    case Token::YouReply: return QObject::tr("You reply");

    case Token::SomeoneBeepsYou: return QObject::tr("Someone beeps you");

    case Token::WallBegin: return QObject::tr("Wall start");
    case Token::WallEnd: return QObject::tr("Wall end");
    case Token::WallLine: return QObject::tr("Wall line");

    case Token::WhoBegin: return QObject::tr("Who start");
    case Token::WhoEnd: return QObject::tr("Who end");
    case Token::WhoEndNoUser: return QObject::tr("Who end [no user]");
    case Token::WhoSeparator: return QObject::tr("Who separator");
    case Token::WhoLine: return QObject::tr("Who line");

    case Token::HistoryBegin: return QObject::tr("History start");
    case Token::HistoryEnd: return QObject::tr("History end");
    case Token::HistoryLine: return QObject::tr("History line");

    case Token::FingerBegin: return QObject::tr("Finger start");
    case Token::FingerEnd: return QObject::tr("Finger end");
    case Token::FingerLine: return QObject::tr("Finger line");

    case Token::SomeoneShouts: return QObject::tr("Someone shouts");
    case Token::YouShout: return QObject::tr("You shout");

    case Token::SomeoneAway: return QObject::tr("Someone's away");
    case Token::YouAway: return QObject::tr("You're away");
    case Token::SomeoneBack: return QObject::tr("Someone's back");
    case Token::YouBack: return QObject::tr("You're back");
    case Token::SomeoneAwayWarning: return QObject::tr("Someone's away warning");

    case Token::SystemAliases: return QObject::tr("System aliases");
    case Token::UserAliases: return QObject::tr("User aliases");
    case Token::AliasesEnd: return QObject::tr("Aliases end");
    case Token::NoUserAlias: return QObject::tr("No user aliases");
    case Token::AliasLine: return QObject::tr("Alias line");

    case Token::MessageBegin: return QObject::tr("Message start");
    case Token::MessageLine: return QObject::tr("Message line");
    case Token::MessageEnd: return QObject::tr("Message end");
    case Token::NoMessage: return QObject::tr("No message");
    case Token::MessageReceived: return QObject::tr("Message received");
    case Token::AllMessagesCleared: return QObject::tr("All messages cleared");
    case Token::MessageCleared: return QObject::tr("Message cleared");
    case Token::MessagesCleared: return QObject::tr("Messages cleared");

    case Token::HelpBegin: return QObject::tr("Help start");
    case Token::HelpEndNormal: return QObject::tr("Help normal end");
    case Token::HelpEndNoHelp: return QObject::tr("No help end");
    case Token::HelpLine: return QObject::tr("Help line");

    case Token::UnknownUser: return QObject::tr("Unknown user");
    case Token::UserLoginRenamed: return QObject::tr("User login renamed");
    case Token::YourLoginRenamed: return QObject::tr("Your login renamed");

    case Token::SomeoneComesIn: return QObject::tr("Someone comes in");
    case Token::SomeoneLeaves: return QObject::tr("Someone leaves");

    case Token::SomeoneDisconnects: return QObject::tr("Someone disconnects");
    case Token::YouLeave: return QObject::tr("You leave");

    case Token::YouJoinChannel: return QObject::tr("You join a channel");
    case Token::YouLeaveChannel: return QObject::tr("You leave a channel");
    case Token::SomeoneJoinChannel: return QObject::tr("Someone joins a channel");
    case Token::SomeoneFadesIntoTheShadows: return QObject::tr("Someone fades into the shadows");
    case Token::SomeoneLeaveChannel: return QObject::tr("Someone leaves a channel");
    case Token::SomeoneAppearsFromTheShadows: return QObject::tr("Someone appears from the shadows");

    case Token::YouKickSomeone: return QObject::tr("You kick someone");
    case Token::SomeoneIsKicked: return QObject::tr("Someone is kicked");
    case Token::YouAreKicked: return QObject::tr("You are kicked");

    case Token::Date: return QObject::tr("Date msg.");
    case Token::YourClientIs: return QObject::tr("Your client is");

    case Token::MtpSays: return QObject::tr("Mtp says (other system msg.)");
    case Token::SomeoneSays: return QObject::tr("Someone says");

    case Token::Data: return QObject::tr("Data sent");

    case Token::Unknown: return QObject::tr("Unknown msg.");
    default: return "";
    }
}
