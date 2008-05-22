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

#include <QMessageBox>
#include <QDir>

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

#include "paths.h"

#include "lua_utils.h"

QString executeLuaFilter(const QString &filterName, const QString &line)
{
    QString result;
    QString fileName = QDir(QDir(Paths::sharePath()).filePath("scripts")).filePath(filterName + ".lua");

    lua_State *L = lua_open();
    luaL_openlibs(L);

    if (luaL_loadfile(L, fileName.toLatin1()) || lua_pcall(L, 0, 0, 0))
    {
        QMessageBox::critical(0, "LUA", "error in script loading");
        return "";
    }

    const char *res;

    // Push functions and arguments
    lua_getglobal(L, "InputFilter");  // function to be called
    lua_pushstring(L, line.toLatin1());

    // do the call (1 arguments, 1 result)
    if (lua_pcall(L, 1, 1, 0))
    {
        QMessageBox::critical(0, "LUA", "error running function");
        return "";
    }

    // retrieve result
    if (!lua_isstring(L, -1))
    {
        QMessageBox::warning(0, "LUA", "function must return a string");
        return "";
    }
    res = lua_tostring(L, -1);
    result = QString(res);
    lua_pop(L, 1);  // Pop returned value
    lua_close(L);

    return result;
}
