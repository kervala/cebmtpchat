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

#include <QMap>
#include <QDir>
#include <QMessageBox>
#include <QDateTime>

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

#include "token_info.h"
#include "paths.h"
#include "script.h"

#include "render_script.h"

namespace Script
{
    static Token::Type gTokenType;
    static QList<RenderSegment> *gSegments;

    int segmentsCount(lua_State *l)
    {
        lua_pushinteger(l, gSegments->count());
        return 1;
    }

    int setSegmentsCount(lua_State *l)
    {
        int n = lua_gettop(l); // Arguments number
        if (n != 1)
            return 0;

        if (!lua_isnumber(l, 1))
            return 0;
        int segNum = (int) lua_tonumber(l, 1);

        if (segNum < gSegments->count())
            while (gSegments->count() > segNum)
                gSegments->removeLast();
        else if (segNum > gSegments->count())
            while (gSegments->count() < segNum)
                (*gSegments) << RenderSegment("toto", QFont(), Qt::black);

        return 0;
    }

    int setSegmentText(lua_State *l)
    {
        int n = lua_gettop(l); // Arguments number
        if (n != 2)
            return 0;

        if (!lua_isnumber(l, 1))
            return 0;
        int argNum = (int) lua_tonumber(l, 1);

        if (!lua_isstring(l, 2))
            return 0;

        (*gSegments)[argNum].setText(lua_tostring(l, 2));

        return 0;
    }

    int getSegmentText(lua_State *l)
    {
        int n = lua_gettop(l); // Arguments number
        if (n != 1)
            return 0;
        if (!lua_isnumber(l, 1))
            return 0;
        int argNum = (int) lua_tonumber(l, 1);

        lua_pushstring(l, (*gSegments)[argNum].text().toLatin1());
        return 1;
    }

    int setSegmentColor(lua_State *l)
    {
        int n = lua_gettop(l); // Arguments number
        if (n != 2)
            return 0;

        if (!lua_isnumber(l, 1))
            return 0;
        int argNum = (int) lua_tonumber(l, 1);

        if (!lua_isstring(l, 2))
            return 0;

        (*gSegments)[argNum].setColor("#" + QString(lua_tostring(l, 2)));

        return 0;
    }

    int getSegmentColor(lua_State *l)
    {
        int n = lua_gettop(l); // Arguments number
        if (n != 1)
            return 0;
        if (!lua_isnumber(l, 1))
            return 0;
        int argNum = (int) lua_tonumber(l, 1);

        lua_pushstring(l, (*gSegments)[argNum].text().toLatin1());
        return 1;
    }

    int setSegmentFont(lua_State *l)
    {
        int n = lua_gettop(l); // Arguments number
        if (n != 2)
            return 0;

        if (!lua_isnumber(l, 1))
            return 0;
        int argNum = (int) lua_tonumber(l, 1);

        if (!lua_isstring(l, 2))
            return 0;

        QFont font = (*gSegments)[argNum].font();
        font.setFamily(QString(lua_tostring(l, 2)));
        (*gSegments)[argNum].setFont(font);

        return 0;
    }

    int getSegmentFont(lua_State *l)
    {
        int n = lua_gettop(l); // Arguments number
        if (n != 1)
            return 0;
        if (!lua_isnumber(l, 1))
            return 0;
        int argNum = (int) lua_tonumber(l, 1);

        lua_pushstring(l, (*gSegments)[argNum].font().family().toLatin1());
        return 1;
    }

    int setSegmentSize(lua_State *l)
    {
        int n = lua_gettop(l); // Arguments number
        if (n != 2)
            return 0;

        if (!lua_isnumber(l, 1))
            return 0;
        int argNum = (int) lua_tonumber(l, 1);

        if (!lua_isnumber(l, 2))
            return 0;

        QFont font = (*gSegments)[argNum].font();
        font.setPointSize((int) lua_tonumber(l, 2));
        (*gSegments)[argNum].setFont(font);

        return 0;
    }

    int getSegmentSize(lua_State *l)
    {
        int n = lua_gettop(l); // Arguments number
        if (n != 1)
            return 0;
        if (!lua_isnumber(l, 1))
            return 0;
        int argNum = (int) lua_tonumber(l, 1);

        lua_pushinteger(l, (*gSegments)[argNum].font().pointSize());
        return 1;
    }

    int setSegmentItalic(lua_State *l)
    {
        int n = lua_gettop(l); // Arguments number
        if (n != 2)
            return 0;

        if (!lua_isnumber(l, 1))
            return 0;
        int argNum = (int) lua_tonumber(l, 1);

        if (!lua_isboolean(l, 2))
            return 0;

        QFont font = (*gSegments)[argNum].font();
        font.setItalic(lua_toboolean(l, 2));
        (*gSegments)[argNum].setFont(font);

        return 0;
    }

    int getSegmentItalic(lua_State *l)
    {
        int n = lua_gettop(l); // Arguments number
        if (n != 1)
            return 0;
        if (!lua_isnumber(l, 1))
            return 0;
        int argNum = (int) lua_tonumber(l, 1);

        lua_pushboolean(l, (*gSegments)[argNum].font().italic());
        return 1;
    }

    int setSegmentBold(lua_State *l)
    {
        int n = lua_gettop(l); // Arguments number
        if (n != 2)
            return 0;

        if (!lua_isnumber(l, 1))
            return 0;
        int argNum = (int) lua_tonumber(l, 1);

        if (!lua_isboolean(l, 2))
            return 0;

        QFont font = (*gSegments)[argNum].font();
        font.setBold(lua_toboolean(l, 2));
        (*gSegments)[argNum].setFont(font);

        return 0;
    }

    int getSegmentBold(lua_State *l)
    {
        int n = lua_gettop(l); // Arguments number
        if (n != 1)
            return 0;
        if (!lua_isnumber(l, 1))
            return 0;
        int argNum = (int) lua_tonumber(l, 1);

        lua_pushboolean(l, (*gSegments)[argNum].font().bold());
        return 1;
    }

    int setSegmentUnderline(lua_State *l)
    {
        int n = lua_gettop(l); // Arguments number
        if (n != 2)
            return 0;

        if (!lua_isnumber(l, 1))
            return 0;
        int argNum = (int) lua_tonumber(l, 1);

        if (!lua_isboolean(l, 2))
            return 0;

        QFont font = (*gSegments)[argNum].font();
        font.setUnderline(lua_toboolean(l, 2));
        (*gSegments)[argNum].setFont(font);

        return 0;
    }

    int getSegmentUnderline(lua_State *l)
    {
        int n = lua_gettop(l); // Arguments number
        if (n != 1)
            return 0;
        if (!lua_isnumber(l, 1))
            return 0;
        int argNum = (int) lua_tonumber(l, 1);

        lua_pushboolean(l, (*gSegments)[argNum].font().underline());
        return 1;
    }

    void registerFunctions(lua_State *l)
    {
        lua_register(l, "segmentsCount", segmentsCount);
        lua_register(l, "setSegmentsCount", setSegmentsCount);
        lua_register(l, "getSegmentText", getSegmentText);
        lua_register(l, "setSegmentText", setSegmentText);
        lua_register(l, "getSegmentColor", getSegmentColor);
        lua_register(l, "setSegmentColor", setSegmentColor);
        lua_register(l, "getSegmentFont", getSegmentFont);
        lua_register(l, "setSegmentFont", setSegmentFont);
        lua_register(l, "getSegmentSize", getSegmentSize);
        lua_register(l, "setSegmentSize", setSegmentSize);
        lua_register(l, "getSegmentItalic", getSegmentItalic);
        lua_register(l, "setSegmentItalic", setSegmentItalic);
        lua_register(l, "getSegmentBold", getSegmentBold);
        lua_register(l, "setSegmentBold", setSegmentBold);
        lua_register(l, "getSegmentUnderline", getSegmentUnderline);
        lua_register(l, "setSegmentUnderline", setSegmentUnderline);
    }

    void unregisterFunctions(lua_State *l)
    {
        unregisterFunction(l, "segmentsCount");
        unregisterFunction(l, "setSegmentsCount");
        unregisterFunction(l, "getSegmentText");
        unregisterFunction(l, "setSegmentText");
        unregisterFunction(l, "getSegmentColor");
        unregisterFunction(l, "setSegmentColor");
        unregisterFunction(l, "getSegmentFont");
        unregisterFunction(l, "setSegmentFont");
        unregisterFunction(l, "getSegmentSize");
        unregisterFunction(l, "setSegmentSize");
        unregisterFunction(l, "getSegmentItalic");
        unregisterFunction(l, "setSegmentItalic");
        unregisterFunction(l, "getSegmentBold");
        unregisterFunction(l, "setSegmentBold");
        unregisterFunction(l, "getSegmentUnderline");
        unregisterFunction(l, "setSegmentUnderline");
    }

    void executeRenderScript(Session *session, Token::Type tokenType, QList<RenderSegment> &segments)
    {
        lua_State *l = getScript(tokenType);

        if (!l)
            return;

        registerFunctions(l);

        // Init global variables
        setSession(session);
        gTokenType = tokenType;
        gSegments = &segments;

        lua_getglobal(l, "render"); // Function to be called

        if (lua_pcall(l, 0, 1, 0))
            QMessageBox::critical(0, "LUA", lua_tostring(l, -1));

        unregisterFunctions(l);
    }
}
