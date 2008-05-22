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

#include "mtp_token_info.h"

QString MtpTokenInfo::tokenToIDString(MtpToken token)
{
    switch (token)
    {
    case Token_IndicatedActiveServer: return "IndicatedActiveServer";
    case Token_LoginAsked: return "LoginAsked";
    case Token_InvalidLogin: return "InvalidLogin";
    case Token_PasswordAsked: return "PasswordAsked";
    case Token_IncorrectPassword: return "IncorrectPassword";
    case Token_Welcome: return "Welcome";
    case Token_Topic: return "Topic";
    case Token_YouSetTopic: return "YouSetTopic";
    case Token_SomeoneSetTopic: return "SomeoneSetTopic";
    case Token_SomeoneTellsYou: return "SomeoneTellsYou";
    case Token_YouTellToSomeone: return "YouTellToSomeone";
    case Token_SomeoneAsksYou: return "SomeoneAsksYou";
    case Token_YouAskToSomeone: return "YouAskToSomeone";
    case Token_SomeoneReplies: return "SomeoneReplies";
    case Token_YouReply: return "YouReply";

    case Token_SomeoneBeepsYou: return "SomeoneBeepsYou";

    case Token_WallBegin: return "WallBegin";
    case Token_WallEnd: return "WallEnd";
    case Token_WallLine: return "WallLine";

    case Token_WhoBegin: return "WhoBegin";
    case Token_WhoEnd: return "WhoEnd";
    case Token_WhoEndNoUser: return "WhoEndNoUser";
    case Token_WhoSeparator: return "WhoSeparator";
    case Token_WhoLine: return "WhoLine";

    case Token_HistoryBegin: return "HistoryBegin";
    case Token_HistoryEnd: return "HistoryEnd";
    case Token_HistoryLine: return "HistoryLine";

    case Token_FingerBegin: return "FingerBegin";
    case Token_FingerEnd: return "FingerEnd";
    case Token_FingerLine: return "FingerLine";

    case Token_SomeoneShouts: return "SomeoneShouts";
    case Token_YouShout: return "YouShout";

    case Token_SomeoneAway: return "SomeoneAway";
    case Token_YouAway: return "YouAway";
    case Token_SomeoneBack: return "SomeoneBack";
    case Token_YouBack: return "YouBack";
    case Token_SomeoneAwayWarning: return "SomeoneAwayWarning";

    case Token_SystemAliases: return "SystemAliases";
    case Token_UserAliases: return "UserAliases";
    case Token_AliasesEnd: return "AliasesEnd";
    case Token_NoUserAlias: return "NoUserAlias";
    case Token_AliasLine: return "AliasLine";

    case Token_MessageBegin: return "MessageBegin";
    case Token_MessageLine: return "MessageLine";
    case Token_MessageEnd: return "MessageEnd";
    case Token_NoMessage: return "NoMessage";
    case Token_MessageReceived: return "MessageReceived";
    case Token_AllMessagesCleared: return "AllMessagesCleared";
    case Token_MessageCleared: return "MessageCleared";
    case Token_MessagesCleared: return "MessagesCleared";

    case Token_HelpBegin: return "HelpBegin";
    case Token_HelpEndNormal: return "HelpEndNormal";
    case Token_HelpEndNoHelp: return "HelpEndNoHelp";
    case Token_HelpLine: return "HelpLine";

    case Token_UnknownUser: return "UnknownUser";
    case Token_UserLoginRenamed: return "UserLoginRenamed";
    case Token_YourLoginRenamed: return "YourLoginRenamed";

    case Token_SomeoneComesIn: return "SomeoneComesIn";
    case Token_SomeoneLeaves: return "SomeoneLeaves";
    case Token_SomeoneDisconnects: return "SomeoneDisconnects";
    case Token_YouLeave: return "YouLeave";

    case Token_YouJoinChannel: return "YouJoinChannel";
    case Token_YouLeaveChannel: return "YouLeaveChannel";
    case Token_SomeoneJoinChannel: return "SomeoneJoinChannel";
    case Token_SomeoneFadesIntoTheShadows: return "SomeoneFadeIntoTheShadows";
    case Token_SomeoneLeaveChannel: return "SomeoneLeaveChannel";
    case Token_SomeoneAppearsFromTheShadows: return "SomeoneAppearsFromTheShadows";

    case Token_YouKickSomeone: return "YouKickSomeone";
    case Token_SomeoneIsKicked: return "SomeoneIsKicked";
    case Token_YouAreKicked: return "YouAreKicked";

    case Token_Date: return "Date";
    case Token_YourClientIs: return "YourClientIs";

    case Token_MtpSays: return "MtpSays";
    case Token_SomeoneSays: return "SomeoneSays";

    case Token_Data: return "Data";

    case Token_Unknown: return "Unknown";
    default: return "";
    }
}

MtpToken MtpTokenInfo::IDStringToToken(const QString &ID)
{
    for (int i = 0; i < Token_End; ++i)
        if (tokenToIDString((MtpToken) i).toUpper() == ID.toUpper())
            return (MtpToken) i;
    return Token_Unknown;
}

QString MtpTokenInfo::tokenToDisplayString(MtpToken token)
{
    switch (token)
    {
    case Token_IndicatedActiveServer: return QObject::tr("Indicated active server");
    case Token_LoginAsked: return QObject::tr("Login asked");
    case Token_InvalidLogin: return QObject::tr("Invalid login");
    case Token_PasswordAsked: return QObject::tr("Password asked");
    case Token_IncorrectPassword: return QObject::tr("Incorrect password");
    case Token_Welcome: return QObject::tr("Welcome msg.");
    case Token_Topic: return QObject::tr("Indicated topic");
    case Token_YouSetTopic: return QObject::tr("You set topic");
    case Token_SomeoneSetTopic: return QObject::tr("Someone set topic");
    case Token_SomeoneTellsYou: return QObject::tr("Someone tells you");
    case Token_YouTellToSomeone: return QObject::tr("You tell someone");
    case Token_SomeoneAsksYou: return QObject::tr("Someone asks you");
    case Token_YouAskToSomeone: return QObject::tr("You ask someone");
    case Token_SomeoneReplies: return QObject::tr("Someone replies");
    case Token_YouReply: return QObject::tr("You reply");

    case Token_SomeoneBeepsYou: return QObject::tr("Someone beeps you");

    case Token_WallBegin: return QObject::tr("Wall start");
    case Token_WallEnd: return QObject::tr("Wall end");
    case Token_WallLine: return QObject::tr("Wall line");

    case Token_WhoBegin: return QObject::tr("Who start");
    case Token_WhoEnd: return QObject::tr("Who end");
    case Token_WhoEndNoUser: return QObject::tr("Who end [no user]");
    case Token_WhoSeparator: return QObject::tr("Who separator");
    case Token_WhoLine: return QObject::tr("Who line");

    case Token_HistoryBegin: return QObject::tr("History start");
    case Token_HistoryEnd: return QObject::tr("History end");
    case Token_HistoryLine: return QObject::tr("History line");

    case Token_FingerBegin: return QObject::tr("Finger start");
    case Token_FingerEnd: return QObject::tr("Finger end");
    case Token_FingerLine: return QObject::tr("Finger line");

    case Token_SomeoneShouts: return QObject::tr("Someone shouts");
    case Token_YouShout: return QObject::tr("You shout");

    case Token_SomeoneAway: return QObject::tr("Someone's away");
    case Token_YouAway: return QObject::tr("You're away");
    case Token_SomeoneBack: return QObject::tr("Someone's back");
    case Token_YouBack: return QObject::tr("You're back");
    case Token_SomeoneAwayWarning: return QObject::tr("Someone's away warning");

    case Token_SystemAliases: return QObject::tr("System aliases");
    case Token_UserAliases: return QObject::tr("User aliases");
    case Token_AliasesEnd: return QObject::tr("Aliases end");
    case Token_NoUserAlias: return QObject::tr("No user aliases");
    case Token_AliasLine: return QObject::tr("Alias line");

    case Token_MessageBegin: return QObject::tr("Message start");
    case Token_MessageLine: return QObject::tr("Message line");
    case Token_MessageEnd: return QObject::tr("Message end");
    case Token_NoMessage: return QObject::tr("No message");
    case Token_MessageReceived: return QObject::tr("Message received");
    case Token_AllMessagesCleared: return QObject::tr("All messages cleared");
    case Token_MessageCleared: return QObject::tr("Message cleared");
    case Token_MessagesCleared: return QObject::tr("Messages cleared");

    case Token_HelpBegin: return QObject::tr("Help start");
    case Token_HelpEndNormal: return QObject::tr("Help normal end");
    case Token_HelpEndNoHelp: return QObject::tr("No help end");
    case Token_HelpLine: return QObject::tr("Help line");

    case Token_UnknownUser: return QObject::tr("Unknown user");
    case Token_UserLoginRenamed: return QObject::tr("User login renamed");
    case Token_YourLoginRenamed: return QObject::tr("Your login renamed");

    case Token_SomeoneComesIn: return QObject::tr("Someone comes in");
    case Token_SomeoneLeaves: return QObject::tr("Someone leaves");

    case Token_SomeoneDisconnects: return QObject::tr("Someone disconnects");
    case Token_YouLeave: return QObject::tr("You leave");

    case Token_YouJoinChannel: return QObject::tr("You join a channel");
    case Token_YouLeaveChannel: return QObject::tr("You leave a channel");
    case Token_SomeoneJoinChannel: return QObject::tr("Someone joins a channel");
    case Token_SomeoneFadesIntoTheShadows: return QObject::tr("Someone fades into the shadows");
    case Token_SomeoneLeaveChannel: return QObject::tr("Someone leaves a channel");
    case Token_SomeoneAppearsFromTheShadows: return QObject::tr("Someone appears from the shadows");

    case Token_YouKickSomeone: return QObject::tr("You kick someone");
    case Token_SomeoneIsKicked: return QObject::tr("Someone is kicked");
    case Token_YouAreKicked: return QObject::tr("You are kicked");

    case Token_Date: return QObject::tr("Date msg.");
    case Token_YourClientIs: return QObject::tr("Your client is");

    case Token_MtpSays: return QObject::tr("Mtp says (other system msg.)");
    case Token_SomeoneSays: return QObject::tr("Someone says");

    case Token_Data: return QObject::tr("Data sent");

    case Token_Unknown: return QObject::tr("Unknown msg.");
    default: return "";
    }
}
