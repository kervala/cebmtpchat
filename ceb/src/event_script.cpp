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

#include <QDir>
#include <QMessageBox>
#include <QFileInfo>

#include "paths.h"

#include "event_script.h"

Script::LuaScript adminScript;
Script::LuaScript userScript;

QString g_entryText;

void EventScript::focused()
{
    executeFunction("focused");
}

void EventScript::unfocused()
{
    executeFunction("unfocused");
}

QString EventScript::newEntry(Session *session, const QString &text)
{
	// Set global stuff
	Script::setSession(session);
	g_entryText = text;

	executeFunction("newEntry");
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
                QMessageBox::critical(0, "LUA", lua_tostring(l, -1));
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
        QMessageBox::critical(0, "LUA", lua_tostring(l, -1));
}

lua_State *EventScript::getScript(const QString &filePath, Script::LuaScript &luaScript)
{
    if (!QFile(filePath).exists())
        return 0;

    QFileInfo fileInfo(filePath);

    if (luaScript.fileDateTime.isValid() && fileInfo.lastModified() != luaScript.fileDateTime)
        lua_close(luaScript.l); // Close the old

    bool error;
    luaScript.l = Script::loadScript(filePath, false, error);
    if (error)
    {
        QMessageBox::critical(0, "LUA", "error in loading script " + filePath);
        return 0;
    }
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
