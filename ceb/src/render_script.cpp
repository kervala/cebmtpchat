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

#include "render_script.h"

class LuaScript
{
public:
    QString filePath;
    QDateTime fileDateTime;
    lua_State *l;
};

static QMap<Token::Type, LuaScript> luaScripts;
static Session *_session;
static Token::Type _token;
static QList<RenderSegment> *_segments;

int segmentsCount(lua_State *l)
{
    lua_pushinteger(l, _segments->count());
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

    if (segNum < _segments->count())
        while (_segments->count() > segNum)
            _segments->removeLast();
    else if (segNum > _segments->count())
        while (_segments->count() < segNum)
            (*_segments) << RenderSegment("toto", QFont(), Qt::black);

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

    (*_segments)[argNum].setText(lua_tostring(l, 2));

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

    lua_pushstring(l, (*_segments)[argNum].text().toLatin1());
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

    (*_segments)[argNum].setColor("#" + QString(lua_tostring(l, 2)));

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

    lua_pushstring(l, (*_segments)[argNum].text().toLatin1());
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

    QFont font = (*_segments)[argNum].font();
    font.setFamily(QString(lua_tostring(l, 2)));
    (*_segments)[argNum].setFont(font);

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

    lua_pushstring(l, (*_segments)[argNum].font().family().toLatin1());
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

    QFont font = (*_segments)[argNum].font();
    font.setPointSize((int) lua_tonumber(l, 2));
    (*_segments)[argNum].setFont(font);

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

    lua_pushinteger(l, (*_segments)[argNum].font().pointSize());
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

    QFont font = (*_segments)[argNum].font();
    font.setItalic(lua_toboolean(l, 2));
    (*_segments)[argNum].setFont(font);

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

    lua_pushboolean(l, (*_segments)[argNum].font().italic());
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

    QFont font = (*_segments)[argNum].font();
    font.setBold(lua_toboolean(l, 2));
    (*_segments)[argNum].setFont(font);

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

    lua_pushboolean(l, (*_segments)[argNum].font().bold());
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

    QFont font = (*_segments)[argNum].font();
    font.setUnderline(lua_toboolean(l, 2));
    (*_segments)[argNum].setFont(font);

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

    lua_pushboolean(l, (*_segments)[argNum].font().underline());
    return 1;
}

int getSessionInfo(lua_State *l)
{
    int n = lua_gettop(l); // Arguments number
    if (n != 1)
        return 0;

    if (!lua_isstring(l, 1))
        return 0;

    QString infoToken(lua_tostring(l, 1));

    if (infoToken.toLower() == "login")
    {
        lua_pushstring(l, _session->serverLogin().toLatin1());
    } else
        lua_pushstring(l, "");

    return 1;
}

int sessionSend(lua_State *l)
{
    int n = lua_gettop(l); // Arguments number
    if (n != 1)
        return 0;

    if (!lua_isstring(l, 1))
        return 0;

    _session->send(lua_tostring(l, 1));

    return 0;
}

lua_State *loadLuaScript(const QString &filePath, bool &error)
{
    lua_State *l = lua_open();
    luaL_openlibs(l);

    if (luaL_loadfile(l, filePath.toLocal8Bit()) || lua_pcall(l, 0, 0, 0))
    {
        QMessageBox::critical(0, "LUA", "error in loading script " + filePath);
        error = true;
        return 0;
    }

    lua_register(l, "getSessionInfo", getSessionInfo);
    lua_register(l, "sessionSend", sessionSend);
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

    error = false;
    return l;
}

void Script::executeRenderScript(Session *session, Token::Type tokenType, QList<RenderSegment> &segments)
{
    if (luaScripts.find(tokenType) == luaScripts.end())
    {
        // File exists now?
        QDir modifiersDir(QDir(Paths::sharePath()).filePath("modifiers"));
        QString fileName = modifiersDir.filePath(
            TokenInfo::tokenToIDString(tokenType).toLower() + ".lua");
        if (QFile(fileName).exists())
        {
            bool error;
            lua_State *l = loadLuaScript(fileName, error);
            if (error)
                return;

            LuaScript script;
            script.filePath = fileName;
            script.fileDateTime = QFileInfo(fileName).lastModified();
            script.l = l;
            luaScripts[tokenType] = script;
        }
        else
            return;
    }

    LuaScript &script = luaScripts[tokenType];

    // File already exists?
    if (!QFile(script.filePath).exists())
    {
        // Remove the script
        lua_close(script.l);
        luaScripts.remove(tokenType);
        return;
    }

    // Date changed?
    QFileInfo fileInfo(script.filePath);
    if (fileInfo.lastModified() != script.fileDateTime)
    {
        // Reload it
        lua_close(script.l);
        bool error;
        script.l = loadLuaScript(script.filePath, error);
        if (error)
        {
            QMessageBox::critical(0, "LUA", "error in loading script " + script.filePath);
            luaScripts.remove(tokenType);
            return;
        }
        script.fileDateTime = fileInfo.lastModified();
    }

    // Init global variables
    _session = session;
    _token = tokenType;
    _segments = &segments;

    lua_State *l = script.l;
    lua_getglobal(l, "render"); // Function to be called

    if (lua_pcall(l, 0, 1, 0))
    {
        const char *res = lua_tostring(l, -1);
        QMessageBox::critical(0, "LUA", res);
        return;
    }
}

void Script::closeModifiers()
{
    foreach (const LuaScript &script, luaScripts)
        lua_close(script.l);
    luaScripts.clear();
}
