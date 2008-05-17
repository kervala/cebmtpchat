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

#ifndef MTP_ANALYZER_H
#define MTP_ANALYZER_H

#include <QObject>
#include <QRegExp>

#include "token_event.h"
#include "mtp_token.h"

class MtpRegExp : public QRegExp
{
public:
	MtpRegExp(const QString &pattern, const QList<int> arguments, const QString &example);
	MtpRegExp(const QString &pattern, const QString &example);
	MtpRegExp(const QString &pattern, const QList<int> arguments);
	MtpRegExp(const QString &pattern) : QRegExp(pattern) {}

	const QList<int> &arguments() const { return m_arguments; }
	const QString &example() const { return m_example; }

private:
	QList<int> m_arguments;
	QString m_example;
};

class MtpAnalyzer : public QObject
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

	MtpAnalyzer();

	bool logged() const;
	State state() const;
	bool away() const;

	QStringList split(const QString &message);
	void reset();

	int requestTicket(Command command);

	const QList<MtpRegExp> &tokenRegexp() const { return m_tokenRegexp; }

	static const MtpAnalyzer defaultAnalyzer;

public slots:
	void dataReceived(const QString &data);

signals:
	void tokenAnalyzed(const TokenEvent &event);

private:
	static MtpToken whoTokens[];
	static MtpToken aliasTokens[];
	static MtpToken normalTokens[];
	static MtpToken historyTokens[];
	static MtpToken fingerTokens[];
	static MtpToken wallTokens[];
	static MtpToken messageTokens[];
	static MtpToken helpTokens[];

	bool m_logged;
	State m_state;
	bool m_away;

	QList<MtpRegExp> m_tokenRegexp;
	QList<QRegExp> sendTokenRegexp;
	QRegExp timeRegexp;

	QList<CommandTicket> tickets[Command_Count];

	void analyzeBeforeLog(const QString &data);
	void analyzeAfterLog(const QString &data);

	// !!! Uses <tokenRegexp> state to work !!!
	void doTokenAnalyzed(MtpToken token, int ticketID, const QTime &timeStamp);

	void clearTickets();

	inline QString rightTrim(const QString &str) const;
};

#endif
