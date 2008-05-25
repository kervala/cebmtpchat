#ifndef TOKEN_SCRIPT_H
#define TOKEN_SCRIPT_H

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

class TokenScript
{
public:
    static void executeTokenScript(Session *session, const Token &token);

private:
    static void registerFunctions(lua_State *l);
    static void unregisterFunctions(lua_State *l);
};

#endif
