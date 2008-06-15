#include <QDir>
#include <QMessageBox>
#include <QFileInfo>

#include "paths.h"

#include "event_script.h"

Script::LuaScript adminScript;
Script::LuaScript userScript;

void EventScript::focused()
{
    executeFunction("focused");
}

void EventScript::unfocused()
{
    executeFunction("unfocused");
}

void EventScript::executeFunction(const QString &function)
{
    bool bypassAncestor = false;

    lua_State *l = getUserScript();
    if (l)
    {
        // Init global variables
        lua_getglobal(l, ""); // Function to be called

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
