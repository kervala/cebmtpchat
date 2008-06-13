#include <QString>
#include <QDateTime>
#include <QMap>
#include <QDir>
#include <QMessageBox>

#include "token_info.h"
#include "paths.h"
#include "profile.h"

#include "script.h"

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
    static QMap<Token::Type, LuaScript> userLuaScripts;

    Session *g_session = 0;

    Session *getSession() { return g_session; }
    void setSession(Session *session) { g_session = session; }

    int message(lua_State *l)
    {
        int n = lua_gettop(l);
        if (n != 1)
            return 0;

        if (lua_isstring(l, 1))
            QMessageBox::warning(0, "Lua", QString(lua_tostring(l, 1)));
        else if (lua_isnumber(l, 1))
            QMessageBox::warning(0, "Lua", QString::number(lua_tonumber(l, 1)));
        else if (lua_isboolean(l, 1))
            switch (lua_toboolean(l, 1))
            {
            case true:
                QMessageBox::warning(0, "Lua", "true");
                break;
            case false:
                QMessageBox::warning(0, "Lua", "false");
                break;
            }
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
            lua_pushstring(l, g_session->serverLogin().toLatin1());
        } else if (infoToken.toLower() == "server_address")
        {
            lua_pushstring(l, g_session->serverAddress().toLatin1());
        } else if (infoToken.toLower() == "server_port")
        {
            lua_pushnumber(l, g_session->serverPort());
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

        g_session->send(lua_tostring(l, 1));

        return 0;
    }

    int getPropertyGeneric(lua_State *l, const Properties &properties)
    {
        int n = lua_gettop(l);
        if (n < 1 || n > 2)
            return 0;

        if (!lua_isstring(l, 1))
            return 0;

        QString propName = lua_tostring(l, 1);

        // Default value?
        bool found = false;
        foreach (const Property &prop, properties)
        {
            if (!prop.name().compare(propName))
            {
                found = true;
                switch (prop.type())
                {
                case Property::IntegerProperty:
                    lua_pushnumber(l, prop.intValue());
                    break;
                case Property::BooleanProperty:
                    lua_pushboolean(l, prop.boolValue());
                    break;
                default:
                    lua_pushstring(l, prop.strValue().toLatin1());
                }
                break;
            }
        }
        if (!found)
        {
            if (lua_isnumber(l, 2))
                lua_pushnumber(l, lua_tonumber(l, 2));
            else if (lua_isboolean(l, 2))
                lua_pushboolean(l, lua_toboolean(l, 2));
            else if (lua_isstring(l, 2))
                lua_pushstring(l, lua_tostring(l, 2));
            else
                return 0;
        }

        return 1;
    }

    int getSessionProperty(lua_State *l)
    {
        return getPropertyGeneric(l, g_session->properties());
    }

    int getProperty(lua_State *l)
    {
        return getPropertyGeneric(l, Profile::instance().properties);
    }

    int setPropertyGeneric(lua_State *l, Properties &properties)
    {
        int n = lua_gettop(l);
        if (n != 2)
            return 0;

        if (!lua_isstring(l, 1))
            return 0;

        QString propName = lua_tostring(l, 1);

        if (lua_isnumber(l, 2))
            properties.setValue(propName, (int) lua_tonumber(l, 2));
        else if (lua_isboolean(l, 2))
            properties.setValue(propName, (bool) lua_toboolean(l, 2));
        else if (lua_isstring(l, 2))
            properties.setValue(propName, QString(lua_tostring(l, 2)));

        return 0;
    }

    int setSessionProperty(lua_State *l)
    {
        return setPropertyGeneric(l, g_session->properties());
    }

    int setProperty(lua_State *l)
    {
        return setPropertyGeneric(l, Profile::instance().properties);
    }

    lua_State *loadScript(const QString &filePath, bool userScript, bool &error)
    {
        lua_State *l = luaL_newstate();
        luaL_openlibs(l);

        lua_getfield(l, LUA_GLOBALSINDEX, "package");   /* table to be indexed */
        lua_getfield(l, -1, "path");        /* push result of t.x (2nd arg) */
        lua_remove(l, -2);
        QString path = QDir(QDir(Paths::sharePath()).filePath("modifiers")).filePath("?.lua") + ";";
        if (userScript)
            path += QDir(QDir(Paths::profilePath()).filePath("modifiers")).filePath("?.lua") + ";";
        path += QString(lua_tostring(l, -1));
        lua_getfield(l, LUA_GLOBALSINDEX, "package");
        lua_pushstring(l, path.toLatin1());
        lua_setfield(l, -2, "path");
        lua_remove(l, -1);

        if (luaL_loadfile(l, filePath.toLocal8Bit()) || lua_pcall(l, 0, 0, 0))
        {
            QMessageBox::critical(0, "LUA", "error in loading script " + filePath);
            error = true;
            return 0;
        }

        // Register some basic functions
        lua_register(l, "message", message);
        lua_register(l, "getSessionInfo", getSessionInfo);
        lua_register(l, "sessionSend", sessionSend);
        lua_register(l, "getProperty", getProperty);
        lua_register(l, "setProperty", setProperty);
        lua_register(l, "getSessionProperty", getSessionProperty);
        lua_register(l, "setSessionProperty", setSessionProperty);

        error = false;
        return l;
    }

    lua_State *getScript(Token::Type tokenType)
    {
        if (luaScripts.find(tokenType) == luaScripts.end())
        {
            QDir modifiersDir(QDir(Paths::sharePath()).filePath("modifiers"));
            QString fileName = modifiersDir.filePath(
                TokenInfo::tokenToIDString(tokenType).toLower() + ".lua");
            if (QFile(fileName).exists())
            {
                bool error;
                lua_State *l = loadScript(fileName, false, error);
                if (error)
                    return 0;

                LuaScript script;
                script.filePath = fileName;
                script.fileDateTime = QFileInfo(fileName).lastModified();
                script.l = l;
                luaScripts[tokenType] = script;
            } else
                return 0;
        }

        LuaScript &script = luaScripts[tokenType];

        // If the file doesn't exist anymore, the intention of the remover is to remove the behavior too => remove the script
        if (!QFile(script.filePath).exists())
        {
            // Remove the script
            lua_close(script.l);
            luaScripts.remove(tokenType);
            return 0;
        }

        // Date changed?
        QFileInfo fileInfo(script.filePath);
        if (fileInfo.lastModified() != script.fileDateTime)
        {
            // Reload it
            lua_close(script.l);
            bool error;
            script.l = loadScript(script.filePath, false, error);
            if (error)
            {
                QMessageBox::critical(0, "LUA", "error in loading script " + script.filePath);
                luaScripts.remove(tokenType);
                return 0;
            }
            script.fileDateTime = fileInfo.lastModified();
        }

        return script.l;
    }

    lua_State *getUserScript(Token::Type tokenType)
    {
        if (userLuaScripts.find(tokenType) == userLuaScripts.end())
        {
            QDir modifiersDir(QDir(Paths::profilePath()).filePath("modifiers"));
            QString fileName = modifiersDir.filePath(
                TokenInfo::tokenToIDString(tokenType).toLower() + ".lua");
            if (QFile(fileName).exists())
            {
                bool error;
                lua_State *l = loadScript(fileName, true, error);
                if (error)
                    return 0;

                LuaScript script;
                script.filePath = fileName;
                script.fileDateTime = QFileInfo(fileName).lastModified();
                script.l = l;
                userLuaScripts[tokenType] = script;
            } else
                return 0;
        }

        LuaScript &script = userLuaScripts[tokenType];

        // If the file doesn't exist anymore, the intention of the remover is to remove the behavior too => remove the script
        if (!QFile(script.filePath).exists())
        {
            // Remove the script
            lua_close(script.l);
            userLuaScripts.remove(tokenType);
            return 0;
        }

        // Date changed?
        QFileInfo fileInfo(script.filePath);
        if (fileInfo.lastModified() != script.fileDateTime)
        {
            // Reload it
            lua_close(script.l);
            bool error;
            script.l = loadScript(script.filePath, true, error);
            if (error)
            {
                QMessageBox::critical(0, "LUA", "error in loading script " + script.filePath);
                userLuaScripts.remove(tokenType);
                return 0;
            }
            script.fileDateTime = fileInfo.lastModified();
        }

        return script.l;
    }

    void closeModifiers()
    {
        foreach (const LuaScript &script, luaScripts)
            lua_close(script.l);
        luaScripts.clear();
        foreach (const LuaScript &script, userLuaScripts)
            lua_close(script.l);
        userLuaScripts.clear();
    }

    void unregisterFunction(lua_State *l, const char *name)
    {
        lua_pushnil(l);
        lua_setglobal(l, name);
    }
}
