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
#include <QDateTime>

#include "lua_utils.h"
#include "token_info.h"
#include "paths.h"
#include "script.h"

#include "render_script.h"

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

    (*gSegments)[argNum].setText(QString::fromUtf8(lua_tostring(l, 2)));

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

    lua_pushstring(l, (*gSegments)[argNum].text().toUtf8().data());
    return 1;
}

int setSegmentColor(lua_State *l)
{
    int n = lua_gettop(l); // Arguments number
    if (n < 2)
        return 0;

    if (!lua_isnumber(l, 1))
        return 0;
    int argNum = (int) lua_tonumber(l, 1);

    QColor c = Script::colorOnStack(l, 2);
    if (c.isValid())
        (*gSegments)[argNum].setColor(c);

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

    lua_pushnumber(l, (*gSegments)[argNum].color().rgba());
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

void RenderScript::registerFunctions(lua_State *l)
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

void RenderScript::unregisterFunctions(lua_State *l)
{
    Script::unregisterFunction(l, "segmentsCount");
    Script::unregisterFunction(l, "setSegmentsCount");
    Script::unregisterFunction(l, "getSegmentText");
    Script::unregisterFunction(l, "setSegmentText");
    Script::unregisterFunction(l, "getSegmentColor");
    Script::unregisterFunction(l, "setSegmentColor");
    Script::unregisterFunction(l, "getSegmentFont");
    Script::unregisterFunction(l, "setSegmentFont");
    Script::unregisterFunction(l, "getSegmentSize");
    Script::unregisterFunction(l, "setSegmentSize");
    Script::unregisterFunction(l, "getSegmentItalic");
    Script::unregisterFunction(l, "setSegmentItalic");
    Script::unregisterFunction(l, "getSegmentBold");
    Script::unregisterFunction(l, "setSegmentBold");
    Script::unregisterFunction(l, "getSegmentUnderline");
    Script::unregisterFunction(l, "setSegmentUnderline");
}

void RenderScript::executeRenderScript(Session *session, Token::Type tokenType, QList<RenderSegment> &segments)
{
    bool bypassAncestor = false;
    lua_State *l = Script::getUserScript(tokenType);
    if (l)
    {
        registerFunctions(l);

        // Init global variables
        Script::setSession(session);
        gSegments = &segments;

        lua_getglobal(l, "render"); // Function to be called

        int top = lua_gettop(l);
        if (!lua_isnil(l, top))
        {
            if (lua_pcall(l, 0, 1, 0))
                showLuaError(l, "error executing user render");
        }

        int n = lua_gettop(l); // Arguments number
        if (n && lua_isboolean(l, 2))
            bypassAncestor = lua_toboolean(l, 2);

        unregisterFunctions(l);
    }

    if (bypassAncestor)
        return;

    l = Script::getScript(tokenType);

    if (!l)
        return;

    registerFunctions(l);

    // Init global variables
    Script::setSession(session);
    gSegments = &segments;

    lua_getglobal(l, "render"); // Function to be called

    int top = lua_gettop(l);
    if (lua_isnil(l, top))
        return;

    if (lua_pcall(l, 0, 1, 0))
        showLuaError(l, "error executing render");

    unregisterFunctions(l);
}
