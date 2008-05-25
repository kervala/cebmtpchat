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

#include <QString>
#include <QDateTime>
#include <QMap>

#include "token.h"

namespace Script
{
    class LuaScript
    {
    public:
        QString filePath;
        QDateTime fileDateTime;
        lua_State *l;
    };

    static QMap<Token::Type, LuaScript> luaScripts;

    void closeModifiers();
};

#endif
