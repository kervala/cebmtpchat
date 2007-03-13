/*
    This file is part of Max.

    Max is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    any later version.

    Max is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Max; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef XML_HANDLER_H
#define XML_HANDLER_H

#include <QDomElement>

class XmlHandler
{
public:
    // Read functions
    static QString read(const QDomElement &father, const QString &name, const QString &defaultValue);
    static QString read(const QDomElement &father, const QString &name, const char *defaultValue);
    static int read(const QDomElement &father, const QString &name, const int defaultValue);
    static bool read(const QDomElement &father, const QString &name, const bool defaultValue);

    // Write functions
    static void write(QDomElement &father, const QString &name, const QString &value);
    static void write(QDomElement &father, const QString &name, char *value);
    static void write(QDomElement &father, const QString &name, int value);
    static void write(QDomElement &father, const QString &name, bool value);
};

#endif
