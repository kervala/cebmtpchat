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

#include "token_event.h"

TokenEvent::TokenEvent(MtpToken token, const QStringList &arguments,
					   const QList<int> &positions, int ticketID, const QTime &timeStamp)
{
	init(token, arguments, positions, ticketID, timeStamp);
}

TokenEvent::TokenEvent(MtpToken token, const QStringList &arguments,
					   const QList<int> &positions, int ticketID)
{
	init(token, arguments, positions, ticketID, QTime::currentTime());
}

void TokenEvent::init(MtpToken token, const QStringList &arguments,
					  const QList<int> &positions, int ticketID, const QTime &timeStamp)
{
	m_token = token;
	m_arguments = arguments;
	m_positions = positions;
	m_timeStamp = timeStamp;
	m_ticketID = ticketID;
}
