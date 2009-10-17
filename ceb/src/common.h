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

#include <QtGui/QtGui>
#include <QtNetwork/QtNetwork>
#include <QtXml/QtXml>

extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

#if defined(WIN32) && defined(_DEBUG)
	#define _CRTDBG_MAP_ALLOC
	#ifdef _malloca
		#undef _malloca
	#endif
	#include <stdlib.h>
	#include <crtdbg.h>
	#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
	#define new DEBUG_NEW
	#ifdef realloc
		#undef realloc
	#endif
	#ifdef free
		#undef free
	#endif
#endif

#endif
