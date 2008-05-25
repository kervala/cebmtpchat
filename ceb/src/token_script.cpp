#include <QMessageBox>
#include <QDir>

#include "script.h"
#include "main_window.h"
#include "paths.h"

#include "token_script.h"

static Token g_token;

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

int tokenArgumentCount(lua_State *l)
{
    lua_pushinteger(l, g_token.arguments().count());
    return 1;
}

int tokenArgument(lua_State *l)
{
    int n = lua_gettop(l); // Arguments number
    if (n != 1)
        return 0;

    if (!lua_isnumber(l, 1))
        return 0;

    int argument = (int) lua_tonumber(l, 1);

    lua_pushstring(l, g_token.arguments()[argument].toLatin1());

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

    if (!lua_isstring(l, 2))
        return 0;

    MainWindow::instance()->setTabColor(tab, QColor("#" + QString(lua_tostring(l, 2))));

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

void TokenScript::registerFunctions(lua_State *l)
{
    lua_register(l, "tokenArgumentCount", tokenArgumentCount);
    lua_register(l, "tokenArgument", tokenArgument);
    lua_register(l, "getTab", getTab);
    lua_register(l, "setTabColor", setTabColor);
    lua_register(l, "isTabFocused", isTabFocused);
}

void TokenScript::unregisterFunctions(lua_State *l)
{
    Script::unregisterFunction(l, "tokenArgumentCount");
    Script::unregisterFunction(l, "tokenArgument");
    Script::unregisterFunction(l, "getTab");
    Script::unregisterFunction(l, "setTabColor");
    Script::unregisterFunction(l, "isTabFocused");
}

void TokenScript::executeTokenScript(Session *session, const Token &token)
{
    lua_State *l = Script::getScript(token.type());

    if (!l)
        return;

    registerFunctions(l);

    // Init global variables
    Script::setSession(session);
    g_token = token;

    lua_getglobal(l, "newToken"); // Function to be called

    int top = lua_gettop(l);
    if (lua_isnil(l, top))
        return;

    if (lua_pcall(l, 0, 1, 0))
        QMessageBox::critical(0, "LUA", lua_tostring(l, -1));

    unregisterFunctions(l);
}
