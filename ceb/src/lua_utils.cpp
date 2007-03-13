#include <QApplication>
#include <QMessageBox>

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

#include "lua_utils.h"

QString executeLuaFilter(const QString &filterName, const QString &line)
{
	QString result;
	QString fileName = QApplication::applicationDirPath() + "/scripts/" +
		filterName + ".lua";

	lua_State *L = lua_open();
	luaL_openlibs(L);

	if (luaL_loadfile(L, fileName.toLatin1()) || lua_pcall(L, 0, 0, 0))
	{
		QMessageBox::critical(0, "LUA", "error in script loading");
		return "";
	}

	const char *res;

	// Push functions and arguments
	lua_getglobal(L, "InputFilter");  // function to be called
	lua_pushstring(L, line.toLatin1());

	// do the call (1 arguments, 1 result)
	if (lua_pcall(L, 1, 1, 0))
	{
		QMessageBox::critical(0, "LUA", "error running function");
		return "";
	}

	// retrieve result
	if (!lua_isstring(L, -1))
	{
		QMessageBox::warning(0, "LUA", "function must return a string");
		return "";
	}
	res = lua_tostring(L, -1);
	result = QString(res);
	lua_pop(L, 1);  // Pop returned value
	lua_close(L);

	return result;
}
