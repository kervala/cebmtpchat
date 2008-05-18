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

#ifndef TOKEN_EVENT_H
#define TOKEN_EVENT_H

#include <QVector>
#include <QStringList>
#include <QTime>

#include "mtp_token.h"

class TokenEvent
{
public:
    TokenEvent(MtpToken token, const QStringList &arguments,
               const QList<int> &positions,
               int ticketID, const QTime &timeStamp);
    TokenEvent(MtpToken token, const QStringList &arguments,
               const QList<int> &positions,
               int ticketID);

    const QString &line() const { return m_arguments[0]; }
    const MtpToken token() const { return m_token; }
    const QStringList &arguments() const { return m_arguments; }
    const QList<int> &positions() const { return m_positions; }
    const QTime &timeStamp() const { return m_timeStamp; }
    int ticketID() const { return m_ticketID; }

private:
    MtpToken m_token;
    QStringList m_arguments;
    QList<int> m_positions;
    QTime m_timeStamp;
    int m_ticketID;

    void init(MtpToken token, const QStringList &arguments,
              const QList<int> &positions,
              int ticketID, const QTime &timeStamp);
};

#endif
