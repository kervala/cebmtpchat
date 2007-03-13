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

#include "general_config.h"

#include <xml_handler.h>

#include <QApplication>
#include <QDomDocument>
#include <QDir>
#include <QTextStream>

GeneralConfig *GeneralConfig::_instance = 0;

GeneralConfig &GeneralConfig::instance()
{
	if (!_instance)
		_instance = new GeneralConfig;
	return *_instance;
}

const QString &GeneralConfig::lastProfileName()
{
	return _lastProfileName;
}

void GeneralConfig::setLastProfileName(const QString &name)
{
	_lastProfileName = name;
}

GeneralConfig::GeneralConfig()
{
	QDir dir(QApplication::applicationDirPath());
	_configFile = new QFile(dir.absoluteFilePath("config.xml"));
	_lastProfileName = "default";
	load();
}

GeneralConfig::~GeneralConfig()
{
	save();
	delete _configFile;
}

void GeneralConfig::load()
{
	QFile &file = *_configFile;
	if (!file.exists())
		return;

	if (!file.open(QFile::ReadOnly | QFile::Text))
		return;

	QDomDocument document;

	if (!document.setContent(&file))
		return;

	file.close();

	QDomElement rootElem = document.documentElement();

	// General parameters
	QDomElement configElem = rootElem.firstChildElement("general");
	if (!configElem.isNull())
	{
		_lastProfileName = XmlHandler::read(configElem, "last_profile_name", "default");
	}
}

void GeneralConfig::save()
{
	QFile &file = *_configFile;

	if (!file.open(QFile::WriteOnly | QFile::Text))
		return;

	QDomDocument document;

	QDomElement rootElem = document.createElement("config");
	document.appendChild(rootElem);

	// General
	QDomElement generalElem = document.createElement("general");
	rootElem.appendChild(generalElem);

	XmlHandler::write(generalElem, "last_profile_name", _lastProfileName);

	// Save
	QString xml = document.toString();
	QTextStream out(&file);
	out << xml.toLatin1();
}

