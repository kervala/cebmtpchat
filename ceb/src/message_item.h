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

#ifndef MESSAGE_ITEM_H
#define MESSAGE_ITEM_H

class MessageItem
{
public:
    MessageItem() {}
    MessageItem(const QString &dateTime, const QString &sender, const QString &message) :
        _dateTime(dateTime),
        _sender(sender),
        _message(message) {}

    QString dateTime() const { return _dateTime; }
    void setDateTime(const QString &dateTime) { _dateTime = dateTime; }
    QString sender() const { return _sender; }
    void setSender(const QString &sender) { _sender = sender; }
    QString message() const { return _message; }
    void setMessage(const QString &message) { _message = message; }

private:
    QString _dateTime;
    QString _sender;
    QString _message;
};

#endif
