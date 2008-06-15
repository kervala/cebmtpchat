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
    static void focused();
    static void unfocused();

private:
    static lua_State *getScript(const QString &filePath, Script::LuaScript &luaScript);
    static lua_State *getAdminScript();
    static lua_State *getUserScript();

    static void executeFunction(const QString &function);
};


#endif
