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
#include "paths.h"
#include "event_script.h"
#include "lua_utils.h"

#ifdef LUA_QT
#include "tolua++.h"
#include "lua_qt.h"
#endif

#ifdef DEBUG_NEW
#define new DEBUG_NEW
#endif

Script::LuaScript adminScript;
Script::LuaScript userScript;

QString g_entryText;
QColor g_userBackgroundColor;

int getEntry(lua_State *l)
{
    lua_pushstring(l, g_entryText.toUtf8().data());

    return 1;
}

int setEntry(lua_State *l)
{
    int n = lua_gettop(l);
    if (!n)
        return 0;

    if (lua_isstring(l, 1))
        g_entryText = QString::fromUtf8(lua_tostring(l, 1));

    return 0;
}

int setUserBackgroundColor(lua_State *l)
{
    int n = lua_gettop(l);
    if (n < 1)
        return 0;

    QColor c = Script::colorOnStack(l, 1);
    if (c.isValid())
        g_userBackgroundColor = c;

    return 0;
}

void EventScript::focused()
{
    executeFunction("focused");
}

void EventScript::unfocused()
{
    executeFunction("unfocused");
}

QColor EventScript::getWhoUserBackgroundColor(Session *session, const WhoUser &user)
{
    // Set global stuff
    Script::setSession(session);

    g_userBackgroundColor = QColor(0, 0, 0, 0);

    bool bypassAncestor = false;

    lua_State *l = getUserScript();
    if (l)
    {
        lua_register(l, "setUserBackgroundColor", setUserBackgroundColor);

        // Init global variables
        lua_getglobal(l, "userBackgroundColor"); // Function to be called

        int top = lua_gettop(l);
        if (!lua_isnil(l, top))
        {
            lua_pushstring(l, user.login().toLatin1());
            if (lua_pcall(l, 1, 1, 0))
                showLuaError(l, "error executing user userBackgroundColor");
        }

        int n = lua_gettop(l); // Arguments number
        if (n && lua_isboolean(l, 2))
            bypassAncestor = lua_toboolean(l, 2);

        Script::unregisterFunction(l, "setUserBackgroundColor");
    }

    if (bypassAncestor)
        return g_userBackgroundColor;

    l = getAdminScript();

    if (!l)
        return g_userBackgroundColor;

    lua_register(l, "setUserBackgroundColor", setUserBackgroundColor);

    // Init global variables
    lua_getglobal(l, "userBackgroundColor"); // Function to be called

    int top = lua_gettop(l);
    if (lua_isnil(l, top))
        return g_userBackgroundColor;

    lua_pushstring(l, user.login().toLatin1());
    if (lua_pcall(l, 1, 1, 0))
        showLuaError(l, "error executing admin userBackgroundColor ");

    Script::unregisterFunction(l, "setUserBackgroundColor");

    return g_userBackgroundColor;
}

QString EventScript::newEntry(Session *session, const QString &text)
{
    // Set global stuff
    Script::setSession(session);
    g_entryText = text;

    bool bypassAncestor = false;

    lua_State *l = getUserScript();
    if (l)
    {
        lua_register(l, "getEntry", getEntry);
        lua_register(l, "setEntry", setEntry);

#ifdef LUA_QT
        lua_register(l, "tolua_lua_qt_open", tolua_lua_qt_open);
#endif

        // Init global variables
        lua_getglobal(l, "newEntry"); // Function to be called

        int top = lua_gettop(l);
        if (!lua_isnil(l, top))
        {
            if (lua_pcall(l, 0, 1, 0))
                showLuaError(l, "error executing user newEntry");
        }

        int n = lua_gettop(l); // Arguments number
        if (n && lua_isboolean(l, 2))
            bypassAncestor = lua_toboolean(l, 2);

        Script::unregisterFunction(l, "getEntry");
        Script::unregisterFunction(l, "setEntry");

#ifdef LUA_QT
        Script::unregisterFunction(l, "tolua_lua_qt_open");
#endif
    }

    if (bypassAncestor)
        return g_entryText;

    l = getAdminScript();

    if (!l)
        return g_entryText;

    lua_register(l, "getEntry", getEntry);
    lua_register(l, "setEntry", setEntry);

#ifdef LUA_QT
    lua_register(l, "tolua_lua_qt_open", tolua_lua_qt_open);
#endif

    // Init global variables
    lua_getglobal(l, "newEntry"); // Function to be called

    int top = lua_gettop(l);
    if (lua_isnil(l, top))
        return g_entryText;

    if (lua_pcall(l, 0, 1, 0))
        showLuaError(l, "error executing admin newEntry");

    Script::unregisterFunction(l, "getEntry");
    Script::unregisterFunction(l, "setEntry");

#ifdef LUA_QT
    Script::unregisterFunction(l, "tolua_lua_qt_open");
#endif

    return g_entryText;
}

void EventScript::executeFunction(const QString &function)
{
    bool bypassAncestor = false;

    lua_State *l = getUserScript();
    if (l)
    {
        // Init global variables
        lua_getglobal(l, function.toLatin1()); // Function to be called

        int top = lua_gettop(l);
        if (!lua_isnil(l, top))
        {
            if (lua_pcall(l, 0, 1, 0))
                showLuaError(l, "error executing user function " + function);
        }

        int n = lua_gettop(l); // Arguments number
        if (n && lua_isboolean(l, 2))
            bypassAncestor = lua_toboolean(l, 2);
    }

    if (bypassAncestor)
        return;

    l = getAdminScript();

    if (!l)
        return;

    // Init global variables
    lua_getglobal(l, function.toLatin1()); // Function to be called

    int top = lua_gettop(l);
    if (lua_isnil(l, top))
        return;

    if (lua_pcall(l, 0, 1, 0))
        showLuaError(l, "error executing admin function " + function);
}

lua_State *EventScript::getScript(const QString &filePath, Script::LuaScript &luaScript)
{
    if (!QFile(filePath).exists())
        return 0;

    QFileInfo fileInfo(filePath);

    bool tryToLoad = true, error = false;
    if (!luaScript.fileDateTime.isValid()) // First load?
        luaScript.l = Script::loadScript(filePath, false, error);
    else if (fileInfo.lastModified() != luaScript.fileDateTime) // File changed?
    {
        lua_close(luaScript.l); // Close the old
        luaScript.l = Script::loadScript(filePath, false, error);
    } else // Do nothing
        tryToLoad = false;

    if (error)
    {
        luaScript.fileDateTime = QDateTime();
        return 0;
    } else if (tryToLoad)
        luaScript.fileDateTime = fileInfo.lastModified();

    return luaScript.l;
}

lua_State *EventScript::getAdminScript()
{
    QDir modifiersDir(QDir(Paths::sharePath()).filePath("modifiers"));
    return getScript(modifiersDir.filePath("event.lua"), adminScript);
}

lua_State *EventScript::getUserScript()
{
    QDir modifiersDir(QDir(Paths::profilePath()).filePath("modifiers"));
    return getScript(modifiersDir.filePath("event.lua"), userScript);
}
