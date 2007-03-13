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

#ifndef MTP_TOKEN_H
#define MTP_TOKEN_H

enum MtpToken {
	Token_IndicatedActiveServer,
	Token_LoginAsked,
	Token_InvalidLogin,
	Token_PasswordAsked,
	Token_IncorrectPassword,
	Token_Welcome,
	Token_Topic,
	Token_YouSetTopic,
	Token_SomeoneSetTopic,
	Token_SomeoneTellsYou,
	Token_YouTellToSomeone,
	Token_SomeoneAsksYou,
	Token_YouAskToSomeone,
	Token_SomeoneReplies,
	Token_YouReply,

	Token_SomeoneBeepsYou,

	Token_WallBegin,
	Token_WallEnd,
	Token_WallLine,

	Token_WhoBegin,
	Token_WhoEnd,
	Token_WhoEndNoUser,
	Token_WhoSeparator,
	Token_WhoLine,

	Token_HistoryBegin,
	Token_HistoryEnd,
	Token_HistoryLine,

	Token_FingerBegin,
	Token_FingerEnd,
	Token_FingerLine,

	Token_SomeoneShouts,
	Token_YouShout,

	Token_SomeoneAway,
	Token_YouAway,
	Token_SomeoneBack,
	Token_YouBack,
	Token_SomeoneAwayWarning,

	Token_SystemAliases,
	Token_UserAliases,
	Token_AliasesEnd,
	Token_NoUserAlias,
	Token_AliasLine,

	Token_MessageBegin,
	Token_MessageLine,
	Token_MessageEnd,
	Token_NoMessage,
	Token_MessageReceived,
	Token_AllMessagesCleared,
	Token_MessageCleared,
	Token_MessagesCleared,

	Token_HelpBegin,
	Token_HelpEndNormal,
	Token_HelpEndNoHelp,
	Token_HelpLine,

	Token_UnknownUser,
	Token_UserLoginRenamed,
	Token_YourLoginRenamed,

	Token_SomeoneComesIn,
	Token_SomeoneLeaves,
	Token_SomeoneDisconnects,
	Token_YouLeave,

	Token_YouJoinChannel,
	Token_YouLeaveChannel,
	Token_SomeoneJoinChannel,
	Token_SomeoneFadesIntoTheShadows,
	Token_SomeoneLeaveChannel,
	Token_SomeoneAppearsFromTheShadows,

	Token_YouKickSomeone,
	Token_SomeoneIsKicked,
	Token_YouAreKicked,

	Token_Date,
	Token_YourClientIs,
	
	Token_MtpSays,
	Token_SomeoneSays,

	Token_Data,

	Token_Unknown,
	Token_End = Token_Unknown
};

#endif
