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

#include "common.h"
#include "xml_handler.h"

#ifdef DEBUG_NEW
#define new DEBUG_NEW
#endif

QString XmlHandler::read(const QDomElement &father, const QString &name, const QString &defaultValue)
{
	QDomElement elem = father.firstChildElement(name);

	if (elem.isNull())
		return defaultValue;

	return elem.text();
}

QString XmlHandler::read(const QDomElement &father, const QString &name, const char *defaultValue)
{
    QString defaultStr(defaultValue);
    return read(father, name, defaultStr);
}

int XmlHandler::read(const QDomElement &father, const QString &name, const int defaultValue)
{
    QString defaultStr = QString::number(defaultValue);
    QString strValue = read(father, name, defaultStr);
    bool ok;
    int val = strValue.toInt(&ok);
    if (ok)
        return val;
    else
        return defaultValue;
}

int XmlHandler::read(const QDomElement &father, const QString &name, const long int defaultValue)
{
    QString defaultStr = QString::number(defaultValue);
    QString strValue = read(father, name, defaultStr);
    bool ok;
    long int val = strValue.toLong(&ok);
    if (ok)
        return val;
    else
        return defaultValue;
}

bool XmlHandler::read(const QDomElement &father, const QString &name, const bool defaultValue)
{
    QString defaultStr = QString::number((int) defaultValue);
    QString strValue = read(father, name, defaultStr);
    bool ok;
    int val = strValue.toInt(&ok);
    if (ok)
        return val;
    else
        return defaultValue;
}

void XmlHandler::write(QDomElement &father, const QString &name, const QString &value)
{
    QDomDocument document = father.ownerDocument();

    QDomElement elem = document.createElement(name);
    father.appendChild(elem);

    QDomText t = document.createTextNode(value);
    elem.appendChild(t);
}

void XmlHandler::write(QDomElement &father, const QString &name, char *value)
{
    QString strValue(value);
    write(father, name, strValue);
}

void XmlHandler::write(QDomElement &father, const QString &name, int value)
{
    QString valueStr = QString::number(value);
    write(father, name, valueStr);
}

void XmlHandler::write(QDomElement &father, const QString &name, long int value)
{
    QString valueStr = QString::number(value);
    write(father, name, valueStr);
}

void XmlHandler::write(QDomElement &father, const QString &name, bool value)
{
    QString valueStr = QString::number((int) value);
    write(father, name, valueStr);
}
