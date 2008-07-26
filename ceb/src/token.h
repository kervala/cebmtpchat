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

#ifndef TOKEN_H
#define TOKEN_H

#include <QStringList>
#include <QTime>

class Token
{
public:
    enum Type {
	IndicatedActiveServer,
	LoginAsked,
	InvalidLogin,
        OnlyRegisteredUsers,
	PasswordAsked,
	IncorrectPassword,
	Welcome,
	Topic,
	YouSetTopic,
	SomeoneSetTopic,
	SomeoneTellsYou,
	YouTellToSomeone,
	SomeoneAsksYou,
	YouAskToSomeone,
	SomeoneReplies,
	YouReply,

	SomeoneBeepsYou,

	WallBegin,
	WallEnd,
	WallLine,

	WhoBegin,
	WhoEnd,
	WhoEndNoUser,
	WhoSeparator,
	WhoLine,

        GroupsBegin,
        GroupsEnd,
        GroupsSeparator,
        GroupsLine,

	HistoryBegin,
	HistoryEnd,
	HistoryLine,

	FingerBegin,
	FingerEnd,
	FingerLine,

	SomeoneShouts,
	YouShout,

	SomeoneAway,
	YouAway,
	SomeoneBack,
	YouBack,
	SomeoneAwayWarning,

	SystemAliases,
	UserAliases,
	AliasesEnd,
	NoUserAlias,
	AliasLine,

	MessageBegin,
	MessageLine,
	MessageEnd,
	NoMessage,
	MessageReceived,
	AllMessagesCleared,
	MessageCleared,
	MessagesCleared,

	HelpBegin,
	HelpEndNormal,
	HelpEndNoHelp,
	HelpLine,


	UnknownUser,
	UserLoginRenamed,
	YourLoginRenamed,

	SomeoneComesIn,
	SomeoneLeaves,
	SomeoneDisconnects,
	YouLeave,

	YouJoinChannel,
	YouLeaveChannel,
	SomeoneJoinChannel,
	SomeoneFadesIntoTheShadows,
	SomeoneLeaveChannel,
	SomeoneAppearsFromTheShadows,

	YouKickSomeone,
	SomeoneIsKicked,
	YouAreKicked,

	Date,
	YourClientIs,

        SomeoneGroup,
        UnregisteredUser,

	MtpSays,
	SomeoneSays,

	Data,

	Unknown,
	End = Unknown
    };

    Token();
    Token(Type type, const QStringList &arguments,
          const QList<int> &positions,
          int ticketID, const QTime &timeStamp);
    Token(Type type, const QStringList &arguments,
          const QList<int> &positions,
          int ticketID);

    bool isValid() const { return _isValid; }
    const QString &line() const { return _arguments[0]; }
    const Type type() const { return _type; }
    const QStringList &arguments() const { return _arguments; }
    const QList<int> &positions() const { return _positions; }
    const QTime &timeStamp() const { return _timeStamp; }
    int ticketID() const { return _ticketID; }

private:
    bool _isValid;
    Type _type;
    QStringList _arguments;
    QList<int> _positions;
    QTime _timeStamp;
    int _ticketID;

    void init(Type type, const QStringList &arguments,
              const QList<int> &positions,
              int ticketID, const QTime &timeStamp);
};

#endif
