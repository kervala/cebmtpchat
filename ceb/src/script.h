#ifndef SCRIPT_H
#define SCRIPT_H

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

#include "token.h"
#include "session.h"

namespace Script
{
    class LuaScript
    {
    public:
        QString filePath;
        QDateTime fileDateTime;
        lua_State *l;
    };

    Session *getSession();
    void setSession(Session *session);

    QColor colorOnStack(lua_State *l, int index);

    lua_State *loadScript(const QString &filePath, bool userScript, bool &error);

    lua_State *getUserScript(Token::Type tokenType);
    lua_State *getScript(Token::Type tokenType);

    void unregisterFunction(lua_State *l, const char *name);
    void closeModifiers();
}

#endif
