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

#ifndef PROFILE_MANAGER_H
#define PROFILE_MANAGER_H

#include "profile.h"

class ProfileManager
{
public:
    static ProfileManager &instance();
    static void free();

    Profile *defaultProfile();
    Profile *currentProfile();

    Profile *getProfile(const QString &profileName) const;

    void selectProfile(const QString &profileName);

private:
    static ProfileManager *m_instance;
    QList<Profile *> m_profilesList;
    Profile *m_defaultProfile;
    Profile *m_currentProfile;

    ProfileManager();
    ~ProfileManager();

    void clearProfilesList();
    void loadAll();
};

#endif
