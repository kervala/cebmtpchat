#include <QString>
#include <QDateTime>
#include <QMap>
#include <QDir>

#include "token_info.h"
#include "paths.h"
#include "profile.h"
#include "dialog_system.h"
#include "main_window.h"
#include "my_textedit.h"

#include "script.h"
#include "lua_utils.h"

namespace Script
{
    static QMap<Token::Type, LuaScript> luaScripts;
    static QMap<Token::Type, LuaScript> userLuaScripts;

    Session *g_session = 0;

    Session *getSession() { return g_session; }
    void setSession(Session *session) { g_session = session; }

    QColor colorOnStack(lua_State *l, int index)
    {
        if (lua_isnumber(l, index))
            return QColor::fromRgba(lua_tonumber(l, index));
        else if (lua_isstring(l, index))
            return QColor(QString(lua_tostring(l, index)));
        else
            return QColor();
    }

    int message(lua_State *l)
    {
        int n = lua_gettop(l);
        if (n != 1)
            return 0;

        if (lua_isstring(l, 1))
            DialogSystem::warning("LUA : " + QString(lua_tostring(l, 1)));
        else if (lua_isnumber(l, 1))
            DialogSystem::error("LUA : " + QString::number(lua_tonumber(l, 1)));
        else if (lua_isboolean(l, 1))
            switch (lua_toboolean(l, 1))
            {
            case true:
                DialogSystem::warning("LUA : true");
                break;
            case false:
                DialogSystem::warning("LUA : false");
                break;
            }
        return 0;
    }

    int debug(lua_State *l)
    {
        int n = lua_gettop(l);
        if (n != 1)
            return 0;

        if (lua_isstring(l, 1))
            qDebug(lua_tostring(l, 1));
        else if (lua_isnumber(l, 1))
            qDebug("%f", lua_tonumber(l, 1));
        else if (lua_isboolean(l, 1))
            switch (lua_toboolean(l, 1))
            {
            case true:
                qDebug("true");
                break;
            case false:
                qDebug("false");
                break;
            }
        return 0;
    }

    //! \brief returns a argb value in function of a string and an alpha value
    int getColor(lua_State *l)
    {
        int n = lua_gettop(l);
        if (n < 1)
            return 0;

        if (!lua_isstring(l, 1))
            return 0;

        QColor c(QString(lua_tostring(l, 1)));

        if (n >= 2 && lua_isnumber(l, 2))
        {
            int alpha = lua_tonumber(l, 2);
            c.setAlpha(alpha);
        }

        lua_pushnumber(l, c.rgba());

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
            properties.setValue(propName, (long int) lua_tonumber(l, 2));
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

    int propertyExists(lua_State *l)
    {
        int n = lua_gettop(l);
        if (n != 1)
            return 0;

        if (!lua_isstring(l, 1))
            return 0;

        QString propName = lua_tostring(l, 1);

        lua_pushboolean(l, Profile::instance().properties.exists(propName));

        return 1;
    }

    int getTab(lua_State *l)
    {
        int n = lua_gettop(l); // Arguments number
        if (n < 1 || n > 2)
            return 0;

        if (!lua_isstring(l, 1))
            return 0;

        QString category(lua_tostring(l, 1));

        QString arg;
        if (n == 2)
            if (!lua_isstring(l, 2))
                return 0;
            else
                arg = QString(lua_tostring(l, 2));

        lua_pushlightuserdata(l, MainWindow::instance()->getTab(Script::getSession(), category, arg));

        return 1;
    }

    int getTabColor(lua_State *l)
    {
        int n = lua_gettop(l);
        if (!n)
            return 0;

        if (!lua_islightuserdata(l, 1))
            return 0;

        QWidget *tab = (QWidget *) lua_topointer(l, 1);

        lua_pushnumber(l, MainWindow::instance()->getTabColor(tab).rgba());

        return 1;
    }

    int setTabColor(lua_State *l)
    {
        int n = lua_gettop(l);
        if (n != 2)
            return 0;

        if (!lua_islightuserdata(l, 1))
            return 0;

        QWidget *tab = (QWidget *) lua_topointer(l, 1);

        QColor c = Script::colorOnStack(l, 2);
        if (c.isValid())
            MainWindow::instance()->setTabColor(tab, c);

        return 0;
    }

    int getTextBackgroundColor(lua_State *l)
    {
        lua_pushnumber(l, MyTextEdit::getTextBackgroundColor().rgba());
        return 1;
    }

    int setTextBackgroundColor(lua_State *l)
    {
        int n = lua_gettop(l);
        if (!n)
            return 0;

        QColor c = Script::colorOnStack(l, 1);
        if (c.isValid())
            MyTextEdit::setTextBackgroundColor(c);

        return 0;
    }

    int isTabFocused(lua_State *l)
    {
        int n = lua_gettop(l); // Arguments number
        if (n != 1)
            return 0;

        if (!lua_islightuserdata(l, 1))
            return 0;

        QWidget *tab = (QWidget *) lua_topointer(l, 1);

        lua_pushboolean(l, MainWindow::instance()->isTabFocused(tab));

        return 1;
    }

    int isSuperTabFocused(lua_State *l)
    {
        int n = lua_gettop(l); // Arguments number
        if (n != 1)
            return 0;

        if (!lua_islightuserdata(l, 1))
            return 0;

        QWidget *tab = (QWidget *) lua_topointer(l, 1);

        lua_pushboolean(l, MainWindow::instance()->isSuperTabFocused(tab));

        return 1;
    }

    int getSuperTabColor(lua_State *l)
    {
        int n = lua_gettop(l);
        if (!n)
            return 0;

        if (!lua_islightuserdata(l, 1))
            return 0;

        QWidget *tab = (QWidget *) lua_topointer(l, 1);

        lua_pushnumber(l, MainWindow::instance()->getSuperTabColor(tab).rgba());

        return 1;
    }

    int setSuperTabColor(lua_State *l)
    {
        int n = lua_gettop(l);
        if (n != 2)
            return 0;

        if (!lua_islightuserdata(l, 1))
            return 0;

        QWidget *tab = (QWidget *) lua_topointer(l, 1);

        QColor c = Script::colorOnStack(l, 2);
        if (c.isValid())
            MainWindow::instance()->setSuperTabColor(tab, c);

        return 0;
    }

    int getUserCount(lua_State *l)
    {
        if (!g_session)
            lua_pushnumber(l, 0);
        else
            lua_pushnumber(l, g_session->whoPopulation().users().count());
        return 1;
    }

    int getUserLogin(lua_State *l)
    {
        int n = lua_gettop(l);
        if (!g_session || n < 1 || !lua_isnumber(l, 1))
        {
            lua_pushstring(l, "");
            return 1;
        }

        int index = lua_tonumber(l, 1);
        if (index < 0 || index >= g_session->whoPopulation().users().count())
        {
            lua_pushstring(l, "");
            return 1;
        }

        lua_pushstring(l, g_session->whoPopulation().users()[index].login().toLatin1());
        return 1;
    }

    int getUserData(lua_State *l)
    {
        int n = lua_gettop(l);
        if (!g_session || n < 2 || !lua_isstring(l, 1) || !lua_isstring(l, 2))
        {
            lua_pushstring(l, "");
            return 1;
        }

        QString login(lua_tostring(l, 1));

        WhoUser user = g_session->whoPopulation().userForLogin(login);
        if (!user.isValid())
        {
            lua_pushstring(l, "");
            return 1;
        }

        QString property(lua_tostring(l, 2));

        lua_pushstring(l, user.propertyByName(property).toLatin1());

        return 1;
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
            showLuaError(l, "error in loading script");
            error = true;
            return 0;
        }

        // Register some basic functions
        lua_register(l, "message", message);
        lua_register(l, "debug", debug);
        lua_register(l, "getColor", getColor);
        lua_register(l, "getSessionInfo", getSessionInfo);
        lua_register(l, "sessionSend", sessionSend);
        lua_register(l, "getProperty", getProperty);
        lua_register(l, "setProperty", setProperty);
        lua_register(l, "getSessionProperty", getSessionProperty);
        lua_register(l, "setSessionProperty", setSessionProperty);
        lua_register(l, "propertyExists", propertyExists);
        lua_register(l, "getTab", getTab);
        lua_register(l, "getTabColor", getTabColor);
        lua_register(l, "setTabColor", setTabColor);
        lua_register(l, "isTabFocused", isTabFocused);
        lua_register(l, "isSuperTabFocused", isSuperTabFocused);
        lua_register(l, "getSuperTabColor", getSuperTabColor);
        lua_register(l, "setSuperTabColor", setSuperTabColor);
        lua_register(l, "getTextBackgroundColor", getTextBackgroundColor);
        lua_register(l, "setTextBackgroundColor", setTextBackgroundColor);
        lua_register(l, "getUserCount", getUserCount);
        lua_register(l, "getUserLogin", getUserLogin);
        lua_register(l, "getUserData", getUserData);

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
                showLuaError(script.l, "error in loading script");
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
                showLuaError(script.l, "error in loading script");
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
