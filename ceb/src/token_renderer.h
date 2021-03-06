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
#include "token.h"
#include "render_segment.h"

class TokenRenderer
{
public:
    TokenRenderer()
        : _session(0), _textEdit(0), _grayMode(false) {}

    void displayToken(const Token &token, bool timeStamp = false);

    void setTextEdit(MyTextEdit *value) { _textEdit = value; }
    void setSession(Session *session) { _session = session; }

    bool grayMode() const { return _grayMode; }
    void setGrayMode(bool value) { _grayMode = value; }

private:
    Session *_session;
    MyTextEdit *_textEdit;
    bool _grayMode;
};

#endif // TOKEN_RENDERER_H
