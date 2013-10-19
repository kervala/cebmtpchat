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

#include "common.h"
#include "system_widget.h"
#include "paths.h"
#include "script.h"
#include "lua_utils.h"

#ifdef DEBUG_NEW
#define new DEBUG_NEW
#endif

int showLuaError(lua_State *l, const QString &error)
{
    SystemWidget::error("Lua : " + error + " " + QString(l ? lua_tostring(l, -1):"not initialized"));
    return 0;
}

QString executeLuaFilter(const QString &filterName, const QString &line)
{
    QString result;
    QString fileName = QDir(Paths::scriptsPath()).filePath(filterName + ".lua");

#if LUA_VERSION_NUM >= 502
    lua_State *L = luaL_newstate();
#else
    lua_State *L = lua_open();
#endif

    luaL_openlibs(L);

    if (luaL_loadfile(L, fileName.toLatin1()) || lua_pcall(L, 0, 0, 0))
    {
        showLuaError(L, "error in script loading");
        return "";
    }

    const char *res;

    // Push functions and arguments
    lua_getglobal(L, "InputFilter");  // function to be called
    lua_pushstring(L, line.toUtf8().data());

    // do the call (1 arguments, 1 result)
    if (lua_pcall(L, 1, 1, 0))
    {
        showLuaError(L, "error running function");
        return "";
    }

    // retrieve result
    if (!lua_isstring(L, -1))
    {
        showLuaError(L, "function must return a string");
        return "";
    }
    res = lua_tostring(L, -1);
    result = QString::fromUtf8(res);
    lua_pop(L, 1);  // Pop returned value
    lua_close(L);

    return result;
}
