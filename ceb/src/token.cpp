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

#include "token.h"

Token::Token(Type type, const QStringList &arguments,
             const QList<int> &positions, int ticketID, const QTime &timeStamp)
{
    init(type, arguments, positions, ticketID, timeStamp);
}

Token::Token(Type type, const QStringList &arguments,
             const QList<int> &positions, int ticketID)
{
    init(type, arguments, positions, ticketID, QTime::currentTime());
}

void Token::init(Type type, const QStringList &arguments,
                 const QList<int> &positions, int ticketID, const QTime &timeStamp)
{
    _type = type;
    _arguments = arguments;
    _positions = positions;
    _timeStamp = timeStamp;
    _ticketID = ticketID;
}
