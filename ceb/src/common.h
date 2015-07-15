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

#ifndef COMMON_H
#define COMMON_H

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0510
#endif

#ifndef APSTUDIO_READONLY_SYMBOLS

#if defined(_MSC_VER) && defined(_DEBUG)
	#define _CRTDBG_MAP_ALLOC
	#ifdef _malloca
		#undef _malloca
	#endif
	#include <stdlib.h>
	#include <crtdbg.h>
	#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
	#ifdef realloc
		#undef realloc
	#endif
#endif

#include <QtCore/QtCore>
#include <QtGui/QtGui>
#include <QtNetwork/QtNetwork>
#include <QtXml/QtXml>

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#define USE_QT5
#endif

#ifdef USE_QT5
#include <QtWidgets/QtWidgets>
#include <QtConcurrent/QtConcurrent>
#include <QtMultimedia/QtMultimedia>
#endif

extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

#endif

#endif
