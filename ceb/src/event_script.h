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

#ifndef EVENT_SCRIPT_H
#define EVENT_SCRIPT_H

extern "C" {
#if defined(Q_OS_FREEBSD)
#include <lua51/lua.h>
#include <lua51/lauxlib.h>
#include <lua51/lualib.h>
#else
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#endif
}

#include <QList>

#include "session.h"
#include "token.h"
#include "script.h"

class EventScript
{
public:
    static void focused(); //!< called when CeB is focused
    static void unfocused(); //!< called when CeB is unfocused

	static QString newEntry(Session *session, const QString &text); //!< called when there is a new entry for a session (the user validated a new text in the channel widget

private:
    static lua_State *getScript(const QString &filePath, Script::LuaScript &luaScript);
    static lua_State *getAdminScript();
    static lua_State *getUserScript();

    static void executeFunction(const QString &function);
};


#endif
