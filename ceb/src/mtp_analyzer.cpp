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

#include "mtp_token_info.h"
#include "mtp_analyzer.h"

#define LOGIN_RE "[a-zA-Z][a-zA-Z\\d]{0,7}"

#define TOKEN_A_SIZE(a) sizeof(a) / sizeof(a[0])

MtpRegExp::MtpRegExp(const QString &pattern, const QList<int> arguments, const QString &example) : QRegExp(pattern)
{
	m_arguments = arguments;
	m_example = example;
}

MtpRegExp::MtpRegExp(const QString &pattern, const QList<int> arguments) : QRegExp(pattern)
{
	m_arguments = arguments;
}

MtpRegExp::MtpRegExp(const QString &pattern, const QString &example) : QRegExp(pattern)
{
	m_example = example;
}

////////////////////////////////////////////////////////////////:

const MtpAnalyzer MtpAnalyzer::defaultAnalyzer;

MtpToken MtpAnalyzer::whoTokens[] = {
	Token_WhoSeparator,
	Token_WhoLine,
	Token_WhoEnd,
	Token_WhoEndNoUser
};

MtpToken MtpAnalyzer::aliasTokens[] = {
	Token_UserAliases,
	Token_AliasesEnd,
	Token_NoUserAlias,
	Token_AliasLine
};

MtpToken MtpAnalyzer::normalTokens[] = {
	Token_IndicatedActiveServer,
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
	Token_WhoBegin,
	Token_HistoryBegin,
	Token_FingerBegin,
	Token_SomeoneShouts,
	Token_YouShout,
	Token_SomeoneAway,
	Token_YouAway,
	Token_SomeoneBack,
	Token_YouBack,
	Token_SomeoneAwayWarning,
	Token_SystemAliases,
	Token_MessageBegin,
	Token_NoMessage,
	Token_MessageReceived,
	Token_AllMessagesCleared,
	Token_MessageCleared,
	Token_MessagesCleared,
	Token_HelpBegin,
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
	Token_Unknown
};

MtpToken MtpAnalyzer::historyTokens[] = {
	Token_HistoryEnd,
	Token_HistoryLine
};

MtpToken MtpAnalyzer::fingerTokens[] = {
	Token_FingerEnd,
	Token_FingerLine
};

MtpToken MtpAnalyzer::wallTokens[] = {
	Token_WallEnd,
	Token_WallLine
};

MtpToken MtpAnalyzer::messageTokens[] = {
	Token_MessageEnd,
	Token_MessageLine
};

MtpToken MtpAnalyzer::helpTokens[] = {
	Token_HelpEndNormal,
	Token_HelpEndNoHelp,
	Token_HelpLine
};

MtpAnalyzer::MtpAnalyzer()
{
	m_logged = false;
	m_state = State_Normal;
	m_away = false;

	timeRegexp = QRegExp("^(\\d+:\\d+:\\d+) (.*)$");

	// Fill regexp
	QList<int> args;
	args << 3 << 4;
	m_tokenRegexp << MtpRegExp("^<Mtp> (Active|Main) server (is|move) at ([^:]+):(.+)$",
							   QList<int>() << 3 << 4,
							   "<Mtp> Active server is at a.b.c:4000"); // Token_IndicatedActiveServer

	m_tokenRegexp << MtpRegExp("^<Mtp> Login: $",
							   "<Mtp> Login: "); // Token_LoginAsked
	m_tokenRegexp << MtpRegExp("^<Mtp> Invalid login, choose another one$",
							   "<Mtp> Invalid login, choose another one"); // Token_InvalidLogin
	m_tokenRegexp << MtpRegExp("<Mtp> Password: $",
							   "<Mtp> Password: "); // Token_PasswordAsked
	m_tokenRegexp << MtpRegExp("^<Mtp> Incorrect password$",
							   "<Mtp> Incorrect password"); // Token_IncorrectPassword
	m_tokenRegexp << MtpRegExp("^<Mtp> Welcome, ("LOGIN_RE")\\.$",
							   QList<int>() << 1,
							   "<Mtp> Welcome, Fooman."); // Token_Welcome

	m_tokenRegexp << MtpRegExp("^<Mtp> ("LOGIN_RE") topic : (.+)$",
							   QList<int>() << 1 << 2,
							   "<Mtp> Hall topic : Salut les poulets !"); // Token_Topic
	m_tokenRegexp << MtpRegExp("^<Mtp> You set channel ("LOGIN_RE") topic to (.*)$",
							   QList<int>() << 1 << 2,
							   "<Mtp> You set channel Hall topic to Salut les poulets !"); // Token_YouSetTopic
	m_tokenRegexp << MtpRegExp("^<Mtp> ("LOGIN_RE") set channel ("LOGIN_RE") topic to (.*)$",
							   QList<int>() << 1 << 2 << 3,
							   "<Mtp> Fooman set channel Hall topic to Salut les poulets !"); // Token_SomeoneSetTopic

	m_tokenRegexp << MtpRegExp("^<Mtp> ("LOGIN_RE") tells you: (.+)$",
							   QList<int>() << 1 << 2,
							   "<Mtp> Fooman tells you: espece de truite"); // Token_SomeoneTellsYou
	m_tokenRegexp << MtpRegExp("^<Mtp> You tell ("LOGIN_RE"): (.+)$",
							   QList<int>() << 1 << 2,
							   "<Mtp> You tell Fooman: espece de truite"); // Token_YouTellToSomeone
	m_tokenRegexp << MtpRegExp("^<Mtp> ("LOGIN_RE") asks you: (.+)$",
							   QList<int>() << 1 << 2,
							   "<Mtp> Fooman asks you: es-tu un saumon ou une truite ?"); // Token_SomeoneAsksYou
	m_tokenRegexp << MtpRegExp("^<Mtp> You ask ("LOGIN_RE"): (.+)$",
							   QList<int>() << 1 << 2,
							   "<Mtp> You ask Fooman: es-tu une truite ou un saumon ?"); // Token_YouAskToSomeone
	m_tokenRegexp << MtpRegExp("^<Mtp> ("LOGIN_RE") replies: (.+)$",
							   QList<int>() << 1 << 2,
							   "<Mtp> Fooman replies: vive les levures !"); // Token_SomeoneReplies
	m_tokenRegexp << MtpRegExp("^<Mtp> You reply to ("LOGIN_RE"): (.+)$",
							   QList<int>() << 1 << 2,
							   "<Mtp> You reply to Fooman: vive les levures !"); // Token_YouReply

	m_tokenRegexp << MtpRegExp("^<Mtp> ("LOGIN_RE") beeps you$",
							   QList<int>() << 1,
							   "<Mtp> Fooman beeps you"); // Token_SomeoneBeepsYou

	m_tokenRegexp << MtpRegExp("^<Mtp> Wall :$",
							   "<Mtp> Wall :"); // Token_WallBegin
	m_tokenRegexp << MtpRegExp("^<Mtp> End of Wall$",
							   "<Mtp> End of Wall"); // Token_WallEnd
	m_tokenRegexp << MtpRegExp("^([^:]*) *(\\d+:\\d+:\\d+) *(\\S+) *(.*)$",
							   QList<int>() << 1 << 2 << 3 << 4,
							   "Fri Feb 04 2005 16:48:01 Fooman      Hello World!"); // Token_WallLine

	m_tokenRegexp << MtpRegExp("^ Login    Group   Channel   Idle  On For C              From host$",
							   " Login    Group   Channel   Idle  On For C              From host"); // Token_WhoBegin
	m_tokenRegexp << MtpRegExp("^<Mtp> There (are|is) currently (\\d+) users?(( in channel ("LOGIN_RE"))|)$",
							   QList<int>() << 2 << 5,
							   "<Mtp> There are currently 12 users in channel Hall"); // Token_WhoEnd
	m_tokenRegexp << MtpRegExp("^<Mtp> There is nobody in channel ("LOGIN_RE")$",
							   QList<int>() << 1,
							   "<Mtp> There is nobody in channel Hall"); // Token_WhoEndNoUser
	m_tokenRegexp << MtpRegExp("^-------- -------- -------- ------ ------ - ------------------------------------$",
							   "-------- -------- -------- ------ ------ - ------------------------------------"); // Token_WhoSeparator
	m_tokenRegexp << MtpRegExp("^("LOGIN_RE") *(\\w+) *(\\w+) *([^ ]+) *(\\w+) *(\\w+) *(.+)$",
							   QList<int>() << 1 << 2 << 3 << 4 << 5 << 6 << 7); // Token_WhoLine

	m_tokenRegexp << MtpRegExp("^<Mtp> History :$",
							   "<Mtp> History :"); // Token_HistoryBegin
	m_tokenRegexp << MtpRegExp("^<Mtp> End of History$",
							   "<Mtp> End of History"); // Token_HistoryEnd
	m_tokenRegexp << MtpRegExp("^(\\S+) *(\\S+) *(\\S+) *(\\S+) *(\\S+) *(.+)$",
							   QList<int>() << 1 << 2 << 3 << 4 << 5 << 6); // Token_HistoryLine

	m_tokenRegexp << MtpRegExp("^Login *: ("LOGIN_RE")$",
							   QList<int>() << 1,
							   "Login  : Fooman"); // Token_FingerBegin
	m_tokenRegexp << MtpRegExp("^<Mtp> End of finger$",
							   "<Mtp> End of finger"); // Token_FingerEnd
	m_tokenRegexp << MtpRegExp("^(.+)$",
							   "Blablabla xann blablabla"); // Token_FingerLine

	m_tokenRegexp << MtpRegExp("^<Mtp> ("LOGIN_RE") shouts: (.+)$",
							   QList<int>() << 1 << 2,
							   "<Mtp> Fooman shouts: je veux etre liiibre"); // Token_SomeoneShouts
	m_tokenRegexp << MtpRegExp("^<Mtp> You shout: (.+)$",
							   QList<int>() << 1,
							   "<Mtp> You shout: sortez-moi de ce programme de fou"); // Token_YouShout

	m_tokenRegexp << MtpRegExp("^<Mtp> ("LOGIN_RE") is away$",
							   QList<int>() << 1,
							   "<Mtp> Fooman is away"); // Token_SomeoneAway
	m_tokenRegexp << MtpRegExp("^(\\d+:\\d+:\\d+) (<Mtp> You are away)$",
							   "<Mtp> You are away"); // Token_YouAway
	m_tokenRegexp << MtpRegExp("^<Mtp> ("LOGIN_RE") is back$",
							   QList<int>() << 1,
							   "<Mtp> Fooman is back"); // Token_SomeoneBack
	m_tokenRegexp << MtpRegExp("^<Mtp> You are back$",
							   "<Mtp> You are back"); // Token_YouBack
	m_tokenRegexp << MtpRegExp("^<Mtp> ("LOGIN_RE") is away and may not be hearing you$",
							   QList<int>() << 1,
							   "<Mtp> Fooman is away and may not be hearing you"); // Token_SomeoneAwayWarning

	m_tokenRegexp << MtpRegExp("^<Mtp> System Aliases : $",
							   "<Mtp> System aliases"); // Token_SystemAliases
	m_tokenRegexp << MtpRegExp("^<Mtp> User Aliases : $",
							   "<Mtp> User Aliases : "); // Token_UserAliases
	m_tokenRegexp << MtpRegExp("^<Mtp> End of aliases$",
							   "<Mtp> End of aliases"); // Token_AliasesEnd
	m_tokenRegexp << MtpRegExp("^<Mtp> You don't have any alias !$",
							   "<Mtp> You don't have any alias !"); // Token_NoUserAlias
	m_tokenRegexp << MtpRegExp("^([^ ]+)( *.+)$",
							   QList<int>() << 1 << 2,
							   "un_alias    ouais euhhh ok"); // Token_AliasLine

	m_tokenRegexp << MtpRegExp("^<Mtp> Your message\\(s\\) :$",
							   "<Mtp> You message(s) :"); // Token_MessageBegin
	m_tokenRegexp << MtpRegExp("^ *(\\S+) ([^:]+ \\d+:\\d+:\\d+) ("LOGIN_RE") : (.+)$",
							   QList<int>() << 1 << 2 << 3 << 4); // Token_MessageLine
	m_tokenRegexp << MtpRegExp("^<Mtp> You have (\\d+) messages?$",
							   QList<int>() << 1,
							   "<Mtp> You have 14 messages"); // Token_MessageEnd
	m_tokenRegexp << MtpRegExp("^<Mtp> You have no message !$",
							   "<Mtp> You have no message"); // Token_NoMessage
	m_tokenRegexp << MtpRegExp("^<Mtp> ("LOGIN_RE") sends a message to you: (.+)$",
							   QList<int>() << 1 << 2,
							   "<Mtp> Fooman sends a message to you: huitre"); // Token_MessageReceived
	m_tokenRegexp << MtpRegExp("^<Mtp> You clear your message\\(s\\) !$",
							   "<Mtp> You clear your messages"); // Token_AllMessagesCleared
	m_tokenRegexp << MtpRegExp("^<Mtp> You clear message (\\d+) !$",
							   QList<int>() << 1); // Token_MessageCleared
	m_tokenRegexp << MtpRegExp("^<Mtp> You clear messages (\\d+) to (\\d+) !$",
							   QList<int>() << 1 << 2,
							   "<Mtp> You clear messages 1 to 5 !"); // Token_MessagesCleared

	m_tokenRegexp << MtpRegExp("^<Mtp> Help for \"(.+)\" :$",
							   QList<int>() << 1,
							   "<Mtp> Help for ""pouet pouet"""); // Token_HelpBegin
	m_tokenRegexp << MtpRegExp("^<Mtp> End of help$",
							   "<Mtp> End of help"); // Token_HelpEndNormal
	m_tokenRegexp << MtpRegExp("^<Mtp> No help available for \"(.+)\"$",
							   QList<int>() << 1,
							   "<Mtp> No help available for ""pouet pouet"""); // Token_HelpEndNoHelp
	m_tokenRegexp << MtpRegExp("^(.*)$"); // Token_HelpLine

	m_tokenRegexp << MtpRegExp("^<Mtp> Unknown or unregistered user \"("LOGIN_RE")\"$",
							   QList<int>() << 1,
							   "<Mtp> Unkown or unregistered user ""Fooman"""); // Token_UnknownUser
	m_tokenRegexp << MtpRegExp("^<Mtp> User ("LOGIN_RE") is now known as ("LOGIN_RE")$",
							   QList<int>() << 1 << 2,
							   "<Mtp> User Fooman is now known as Footaise"); // Token_UserLoginRenamed
	m_tokenRegexp << MtpRegExp("^<Mtp> Your login name is now ("LOGIN_RE")$",
							   QList<int>() << 1,
							   "<Mtp> You login name is now Fooman"); // Token_YourLoginRenamed

	m_tokenRegexp << MtpRegExp("^<Mtp> ("LOGIN_RE") comes in !",
							   QList<int>() << 1,
							   "<Mtp> Fooman comes in !"); // Token_SomeoneComesIn
	m_tokenRegexp << MtpRegExp("^<Mtp> ("LOGIN_RE") leaves(( \\((.+))\\)|) !$",
							   QList<int>() << 1 << 4,
							   "<Mtp> Fooman leaves (crustace) !"); // Token_SomeoneLeaves
	m_tokenRegexp << MtpRegExp("^<Mtp> ("LOGIN_RE") disconnects(( \\((.+))\\)|) !$",
							   QList<int>() << 1 << 4,
							   "<Mtp> Fooman disconnects (connection reset by huitre)"); // Token_SomeoneDisconnects
	m_tokenRegexp << MtpRegExp("^<Mtp> You leave <Mtp> Chat !$",
							   "<Mtp> You leave <Mtp> Chat !"); // Token_YouLeave

	m_tokenRegexp << MtpRegExp("^<Mtp> You join channel ("LOGIN_RE")$",
							   QList<int>() << 1,
							   "<Mtp> You join channel Foochan"); // Token_YouJoinChannel
	m_tokenRegexp << MtpRegExp("^<Mtp> You leave channel ("LOGIN_RE")$",
							   QList<int>() << 1,
							   "<Mtp> You leave channel Foochan"); // Token_YouLeaveChannel
	m_tokenRegexp << MtpRegExp("^<Mtp> ("LOGIN_RE") joined channel ("LOGIN_RE")$",
							   QList<int>() << 1 << 2,
							   "<Mtp> Fooman joined channel Foochan"); // Token_SomeoneJoinChannel
	m_tokenRegexp << MtpRegExp("^<Mtp> ("LOGIN_RE") fades into the shadows$",
							   QList<int>() << 1,
							   "<Mtp> Fooman fades into the shadows"); // Token_SomeoneFadesIntoTheShadows
	m_tokenRegexp << MtpRegExp("^<Mtp> ("LOGIN_RE") comes in from channel ("LOGIN_RE")$",
							   QList<int>() << 1 << 2,
							   "<Mtp> Fooman coms in from channel Foochan"); // Token_SomeoneLeaveChannel
	m_tokenRegexp << MtpRegExp("^<Mtp> ("LOGIN_RE") appears from the shadows$",
							   QList<int>() << 1,
							   "<Mtp> Fooman appears from the shadows"); // Token_SomeoneAppearsFromTheShadows

	m_tokenRegexp << MtpRegExp("^<Mtp> You kick ("LOGIN_RE") out(( \\((.*)\\))|) !$",
							   QList<int>() << 1 << 4,
							   "<Mtp> You kick Fooman out (non merci)"); // Token_YouKickSomeone
	m_tokenRegexp << MtpRegExp("^<Mtp> ("LOGIN_RE") is kicked out by ("LOGIN_RE")(( \\((.*)\\))|) !$",
							   QList<int>() << 1 << 2 << 5,
							   "<Mtp> Fooman is kicked out by Root (non merci) !"); // Token_SomeoneIsKicked
	m_tokenRegexp << MtpRegExp("^<Mtp> You are kicked out by ("LOGIN_RE")(( \\((.*)\\))|) !$",
							   QList<int>() << 1 << 4,
							   "<Mtp> You are kicked out by Root (non merci) !"); // Token_YouAreKicked

	m_tokenRegexp << MtpRegExp("^<Mtp> The current date and time are (.+)$",
							   QList<int>() << 1,
							   "<Mtp> The current date and time are Tuesday September 26 2006 19:22:48"); // Token_Date
	m_tokenRegexp << MtpRegExp("^<Mtp> Your client is now \"(.+)\"$",
							   QList<int>() << 1,
							   "<Mtp> Your client is now \"upset\""); // Token_YourClientIs

	m_tokenRegexp << MtpRegExp("^<Mtp> (.+)$",
							   QList<int>() << 1,
							   "<Mtp> Hi all, I'm system, guys!"); // Token_MtpSays
	m_tokenRegexp << MtpRegExp("^<("LOGIN_RE")> (.+)$",
							   QList<int>() << 1 << 2,
							   "<Fooman> I see dead people. Seriously."); // Token_SomeoneSays

	m_tokenRegexp << MtpRegExp("^\\|("LOGIN_RE")\\| (.+)$",
							   QList<int>() << 1 << 2,
							   "|Fooman| red pill or blue pill?"); // Token_Data

	m_tokenRegexp << MtpRegExp(".*"); // Token_Unknown

	// Fill send regexp
	sendTokenRegexp << QRegExp("^tell ("LOGIN_RE") (.*)$"); // SendToken_Tell
	sendTokenRegexp << QRegExp("^reply (.*)$"); // SendToken_Reply
	sendTokenRegexp << QRegExp("^sendmsg ("LOGIN_RE") (.*)$"); // SendToken_Sendmsg
}

bool MtpAnalyzer::logged() const
{
	return m_logged;
}

MtpAnalyzer::State MtpAnalyzer::state() const
{
	return m_state;
}

bool MtpAnalyzer::away() const
{
	return m_away;
}

void MtpAnalyzer::dataReceived(const QString &data)
{
	// Analyze
	if (!m_logged)
		analyzeBeforeLog(data);
	else
		analyzeAfterLog(data);
}

QStringList MtpAnalyzer::split(const QString &message)
{
	QStringList strings; // Contains the result

	// For each sentence in message
	foreach (QString line, message.split('\n'))
	{
		QString trueMessage = line;
		QString prefix;
		int len = 256;

		// Split the message in function of the recognized form
		for (int i = 0; i < sendTokenRegexp.count(); i++)
		{
			SendToken token = (SendToken) i;

			if (sendTokenRegexp[i].exactMatch(message))
			{
				switch(token)
				{
				case SendToken_Tell:
					trueMessage = sendTokenRegexp[i].cap(2);
					prefix = "tell " + sendTokenRegexp[i].cap(1) + " ";
					break;
				case SendToken_Reply:
					trueMessage = sendTokenRegexp[i].cap(1);
					prefix = "reply ";
					break;
				case SendToken_Sendmsg:
					trueMessage = sendTokenRegexp[i].cap(2);
					prefix = "sendmsg " + sendTokenRegexp[i].cap(1) + " ";
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

void MtpAnalyzer::analyzeBeforeLog(const QString &data)
{
	QVector<QString> arguments;
	QTime now = QTime::currentTime();
	if (m_tokenRegexp[Token_IndicatedActiveServer].exactMatch(data))
		doTokenAnalyzed(Token_IndicatedActiveServer, 0, now);
	else if (m_tokenRegexp[Token_LoginAsked].exactMatch(data))
		doTokenAnalyzed(Token_LoginAsked, 0, now);
	else if (m_tokenRegexp[Token_PasswordAsked].indexIn(data) >= 0)
		doTokenAnalyzed(Token_PasswordAsked, 0, now);
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

		if (m_tokenRegexp[Token_InvalidLogin].exactMatch(newData))
			doTokenAnalyzed(Token_InvalidLogin, 0, now);
		else if (m_tokenRegexp[Token_IncorrectPassword].exactMatch(newData))
			doTokenAnalyzed(Token_IncorrectPassword, 0, now);
		else if (m_tokenRegexp[Token_Welcome].exactMatch(newData))
		{
			m_logged = true;
			doTokenAnalyzed(Token_Welcome, 0, now);
		}
		else if (m_tokenRegexp[Token_MtpSays].exactMatch(newData))
		{
			doTokenAnalyzed(Token_MtpSays, 0, now);
		}
		else if (m_tokenRegexp[Token_Unknown].exactMatch(newData))
			doTokenAnalyzed(Token_Unknown, 0, now);
	}
}

void MtpAnalyzer::analyzeAfterLog(const QString &data)
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
		if (m_away)
		{
			if (timeRegexp.exactMatch(newData))
			{
				timeStamp = QTime::fromString(timeRegexp.cap(1), "hh:mm:ss");
				newData = timeRegexp.cap(2);
			}
		}

		bool found = false;
		MtpToken token;

		MtpToken *tokens;
		int tokensSize;

		switch(m_state)
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

			MtpRegExp &regExp = m_tokenRegexp[token];
			if (regExp.exactMatch(newData))
			{
				found = true;
				switch(token)
				{
				case Token_WallBegin:
					m_state = State_Wall;
					break;
				case Token_WallEnd:
					m_state = State_Normal;
					break;
				case Token_WhoBegin:
					m_state = State_Who;
					if (tickets[Command_Who].count() > 0)
					{
						CommandTicket ticket = tickets[Command_Who][0];
						ticketID = ticket.ID;
					}
					break;
				case Token_WhoEnd:
					m_state = State_Normal;
					if (tickets[Command_Who].count() > 0)
					{
						CommandTicket ticket = tickets[Command_Who][0];
						ticketID = ticket.ID;
						tickets[Command_Who].removeAt(0);
					}
					break;
				case Token_WhoEndNoUser:
					m_state = State_Normal;
					if (tickets[Command_Who].count() > 0)
					{
						CommandTicket ticket = tickets[Command_Who][0];
						ticketID = ticket.ID;
						tickets[Command_Who].removeAt(0);
					}
					break;
				case Token_WhoSeparator:
					if (tickets[Command_Who].count() > 0)
					{
						CommandTicket ticket = tickets[Command_Who][0];
						ticketID = ticket.ID;
					}
					break;
				case Token_WhoLine:
					if (tickets[Command_Who].count() > 0)
					{
						CommandTicket ticket = tickets[Command_Who][0];
						ticketID = ticket.ID;
					}
					break;
				case Token_HistoryBegin:
					m_state = State_History;
					break;
				case Token_HistoryEnd:
					m_state = State_Normal;
					break;
				case Token_FingerBegin:
					m_state = State_Finger;
					break;
				case Token_FingerEnd:
					m_state = State_Normal;
					break;
				case Token_YouAway:
				{
					m_away = true;
					timeStamp = QTime::fromString(regExp.cap(1), "hh:mm:ss");
					QStringList strList;
					strList << regExp.cap(2);
					QList<int> positions;
					positions << 0;
					emit tokenAnalyzed(TokenEvent(token, strList, positions,
												  ticketID, timeStamp));
					return;
				}
				case Token_YouBack:
					m_away = false;
					break;
				case Token_SystemAliases:
					m_state = State_Alias;
					break;
				case Token_UserAliases:
					break;
				case Token_AliasesEnd:
					m_state = State_Normal;
					break;
				case Token_MessageBegin:
					m_state = State_Message;
					if (tickets[Command_ShowMsg].count() > 0)
					{
						CommandTicket ticket = tickets[Command_ShowMsg][0];
						ticketID = ticket.ID;
					}
					break;
				case Token_MessageLine:
					if (tickets[Command_ShowMsg].count() > 0)
					{
						CommandTicket ticket = tickets[Command_ShowMsg][0];
						ticketID = ticket.ID;
					}
					break;
				case Token_MessageEnd:
					if (tickets[Command_ShowMsg].count() > 0)
					{
						CommandTicket ticket = tickets[Command_ShowMsg][0];
						ticketID = ticket.ID;
						tickets[Command_ShowMsg].removeAt(0);
					}
					m_state = State_Normal;
					break;
				case Token_HelpBegin:
					m_state = State_Help;
					if (tickets[Command_Help].count() > 0)
					{
						CommandTicket ticket = tickets[Command_Help][0];
						ticketID = ticket.ID;
					}
					break;
				case Token_HelpEndNormal:
				case Token_HelpEndNoHelp:
					if (tickets[Command_Help].count() > 0)
					{
						CommandTicket ticket = tickets[Command_Help][0];
						ticketID = ticket.ID;
						tickets[Command_Help].removeAt(0);
					}
					m_state = State_Normal;
					break;
				case Token_HelpLine:
					if (tickets[Command_Help].count() > 0)
					{
						CommandTicket ticket = tickets[Command_Help][0];
						ticketID = ticket.ID;
					}
					break;
				case Token_Date:
					if (tickets[Command_Date].count() > 0)
					{
						CommandTicket ticket = tickets[Command_Date][0];
						ticketID = ticket.ID;
						tickets[Command_Date].removeAt(0);
					}
					break;
				case Token_YourClientIs:
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
			token = Token_Unknown;

		doTokenAnalyzed(token, ticketID, timeStamp);
	}
}

void MtpAnalyzer::doTokenAnalyzed(MtpToken token, int reservation, const QTime &timeStamp)
{
	MtpRegExp &regExp = m_tokenRegexp[token];

	QList<int> positions;
	for (int i = 0; i < regExp.capturedTexts().count(); ++i)
	{
		if (regExp.cap(i) == "")
			positions << -1;
		else
			positions << regExp.pos(i);
	}
	emit tokenAnalyzed(TokenEvent(token, regExp.capturedTexts(), positions,
								  reservation, timeStamp));
}

void MtpAnalyzer::reset()
{
	m_logged = false;
	m_state = State_Normal;
	m_away = false;
	clearTickets();
}

int MtpAnalyzer::requestTicket(Command command)
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

void MtpAnalyzer::clearTickets()
{
	for (int i =0; i < Command_Count; i++)
		tickets[i].clear();
}

QString MtpAnalyzer::rightTrim(const QString &str) const
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
