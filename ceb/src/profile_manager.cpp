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
#include "profile_manager.h"

#include <QApplication>
#include <QDir>
#include <QMessageBox>

ProfileManager *ProfileManager::m_instance = 0;

ProfileManager::ProfileManager()
{
    loadAll();

    // Select the last profile
    GeneralConfig &config = GeneralConfig::instance();

    m_defaultProfile = getProfile("default");
    m_currentProfile = getProfile(config.lastProfileName());
    if (!m_currentProfile)
        m_currentProfile = m_defaultProfile;
}

void ProfileManager::loadAll()
{
    QDir profilesDir(QApplication::applicationDirPath());

    if (profilesDir.cd("profiles"))
    {
        clearProfilesList();

        QStringList nameFilters;
        nameFilters << "*.xml";
        QStringList entryList = profilesDir.entryList(nameFilters, QDir::Files);
        for (int i = 0; i < entryList.size(); i++)
        {
            QString fileName = profilesDir.absoluteFilePath(entryList.at(i));
            Profile *profile = new Profile;
            if (profile->load(fileName))
                m_profilesList += profile;
            else
                delete profile;
        }

    } else
        QMessageBox::critical(0, "Error", "The profiles directory does not exists!");
}

ProfileManager &ProfileManager::instance()
{
    if (m_instance == 0)
        m_instance = new ProfileManager();
    return *m_instance;
}

void ProfileManager::free()
{
    if (m_instance)
    {
        delete m_instance;
        m_instance = 0;
    }
}

void ProfileManager::clearProfilesList()
{
    while (m_profilesList.count() > 0)
    {
        Profile *profile = m_profilesList.value(0);
        delete profile;
        m_profilesList.removeAt(0);
    }
}

ProfileManager::~ProfileManager()
{
    clearProfilesList();
}

Profile *ProfileManager::currentProfile()
{
    return m_currentProfile;
}

Profile *ProfileManager::defaultProfile()
{
    return m_defaultProfile;
}

void ProfileManager::selectProfile(const QString &profileName)
{
    Profile *profile = getProfile(profileName);
    if (profile)
        m_currentProfile = profile;
}

Profile *ProfileManager::getProfile(const QString &profileName) const
{
    for (int i = 0; i < m_profilesList.count(); i++)
    {
        Profile *profile = m_profilesList.value(i);
        if (profile->name() == profileName)
            return profile;
    }
    return 0;
}

