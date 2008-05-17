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

#ifndef TOKEN_RENDERER_H
#define TOKEN_RENDERER_H

#include "session.h"
#include "my_textedit.h"
#include "token_event.h"

class TokenRenderer
{
public:
	TokenRenderer() : m_session(0), m_textEdit(0) {}
	TokenRenderer(MyTextEdit *textEdit) : m_session(0) { m_textEdit = textEdit; }

	void displayToken(const TokenEvent &event, bool timeStamp = false);

	MyTextEdit *textEdit() const { return m_textEdit; }
	void setTextEdit(MyTextEdit *value) { m_textEdit = value; }

	Session *session() const { return m_session; }
	void setSession(Session *session) { m_session = session; }

private:
	Session *m_session;
	MyTextEdit *m_textEdit;
};

#endif // TOKEN_RENDERER_H
