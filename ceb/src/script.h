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

namespace Script
{
    lua_State *getScript(Token::Type tokenType);
    void closeModifiers();
};

#endif
