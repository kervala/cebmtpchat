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

#include <QString>

class MessageItem
{
public:
	MessageItem() {}
	MessageItem(const QString &dateTime, const QString &sender, const QString &message);

	QString dateTime() const { return m_dateTime; }
	void setDateTime(const QString &dateTime) { m_dateTime = dateTime; }
	QString sender() const { return m_sender; }
	void setSender(const QString &sender) { m_sender = sender; }
	QString message() const { return m_message; }
	void setMessage(const QString &message) { m_message = message; }

private:
	QString m_dateTime;
	QString m_sender;
	QString m_message;
};

#endif
