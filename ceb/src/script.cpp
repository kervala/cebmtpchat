#include <QString>
#include <QDateTime>
#include <QMap>
#include <QDir>
#include <QMessageBox>

#include "token_info.h"
#include "paths.h"

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

    Session *g_session = 0;

    Session *getSession() { return g_session; }
    void setSession(Session *session) { g_session = session; }

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

    lua_State *loadScript(const QString &filePath, bool &error)
    {
        lua_State *l = luaL_newstate();
        luaL_openlibs(l);

        if (luaL_loadfile(l, filePath.toLocal8Bit()) || lua_pcall(l, 0, 0, 0))
        {
            QMessageBox::critical(0, "LUA", "error in loading script " + filePath);
            error = true;
            return 0;
        }

        // Register some basic functions
        lua_register(l, "getSessionInfo", getSessionInfo);
        lua_register(l, "sessionSend", sessionSend);

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
                lua_State *l = loadScript(fileName, error);
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
            script.l = loadScript(script.filePath, error);
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

    void closeModifiers()
    {
        foreach (const LuaScript &script, luaScripts)
            lua_close(script.l);
        luaScripts.clear();
    }

    void unregisterFunction(lua_State *l, const char *name)
    {
        lua_pushnil(l);
        lua_setglobal(l, name);
    }
}
