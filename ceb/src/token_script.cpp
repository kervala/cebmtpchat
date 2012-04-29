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
#include "lua_utils.h"
#include "script.h"
#include "paths.h"

#include "token_script.h"

#ifdef DEBUG_NEW
#define new DEBUG_NEW
#endif

static Token g_token;

int tokenArgumentCount(lua_State *l)
{
    lua_pushinteger(l, g_token.arguments().count());
    return 1;
}

int tokenArgument(lua_State *l)
{
    int n = lua_gettop(l); // Arguments number
    if (n != 1)
        return 0;

    if (!lua_isnumber(l, 1))
        return 0;

    int argument = (int) lua_tonumber(l, 1);

    lua_pushstring(l, g_token.arguments()[argument].toLatin1());

    return 1;
}

void TokenScript::registerFunctions(lua_State *l)
{
    lua_register(l, "tokenArgumentCount", tokenArgumentCount);
    lua_register(l, "tokenArgument", tokenArgument);
}

void TokenScript::unregisterFunctions(lua_State *l)
{
    Script::unregisterFunction(l, "tokenArgumentCount");
    Script::unregisterFunction(l, "tokenArgument");
}

void TokenScript::executeScript(Session *session, const Token &token)
{
    bool bypassAncestor = false;

    lua_State *l = Script::getUserScript(token.type());
    if (l)
    {
        registerFunctions(l);

        // Init global variables
        Script::setSession(session);
        g_token = token;

        lua_getglobal(l, "newToken"); // Function to be called

        int top = lua_gettop(l);
        if (!lua_isnil(l, top))
        {
            if (lua_pcall(l, 0, 1, 0))
                showLuaError(l, "error executing user newToken");
        }

        int n = lua_gettop(l); // Arguments number
        if (n && lua_isboolean(l, 2))
            bypassAncestor = lua_toboolean(l, 2);

        unregisterFunctions(l);
    }

    if (bypassAncestor)
        return;

    l = Script::getScript(token.type());

    if (!l)
        return;

    registerFunctions(l);

    // Init global variables
    Script::setSession(session);
    g_token = token;

    lua_getglobal(l, "newToken"); // Function to be called

    int top = lua_gettop(l);
    if (lua_isnil(l, top))
        return;

    if (lua_pcall(l, 0, 1, 0))
        showLuaError(l, "error executing admin newToken");

    unregisterFunctions(l);
}
