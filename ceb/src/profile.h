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

#ifndef PROFILE_H
#define PROFILE_H

#include <QString>
#include <QList>
#include <QColor>

#include "session_config.h"
#include "token_display.h"
#include "action.h"

class Profile
{
public:
    enum TimeStampPolicy {
        Policy_Classic,
        Policy_Always,
        Policy_Never
    };

    enum LogFilePolicy {
        LogFilePolicy_Daily,
        LogFilePolicy_Weekly,
        LogFilePolicy_Monthly,
        LogFilePolicy_Uniq
    };

    enum WarningoLocation {
        WarningoLocation_TopLeft,
        WarningoLocation_TopRight,
        WarningoLocation_BottomLeft,
        WarningoLocation_BottomRight
    };

    static Profile &instance();

    bool load();
    void save() const;

    Profile &operator=(const Profile &profile);

    SessionConfig *sessionConfigAt(int i) const;
    SessionConfig *getSessionConfig(const QString &name) const;
    void addSessionConfig(const SessionConfig &config);
    void deleteSessionConfig(const QString &name);
    int mainWidth;
    int mainHeight;
    int mainLeft;
    int mainTop;
    QString language;
    bool checkForUpdate;
    bool hideTabsForOne;
    bool systemLogsVisible;
    int systemLogsLeft;
    int systemLogsTop;
    int systemLogsWidth;
    int systemLogsHeight;
    bool topicWindowVisible;
    TimeStampPolicy timeStampPolicy;
    bool timeStampInTellTabs;
    int keepAlive;
    bool logsEnabled;
    bool logsDefaultDir;
    QString logsDir;
    LogFilePolicy logsFilePolicy;
    bool logsTimeStamp;
    bool tabForWall;
    bool tabForWho;
    bool tabForFinger;
    bool trayEnabled;
    int trayAlwaysVisible;
    int trayHideFromTaskBar;
    bool warningoEnabled;
    int warningoLifeTime;
    WarningoLocation warningoLocation;
    int warningoScreen;
    bool warningoPrivate;
    bool warningoHighlight;
    bool tabsAllInOne;
    bool tabsAllInTop;
    bool tabsSuperOnTop;
    bool tabsOnTop;
    bool tabsIcons;
    int tabsCaptionMode;
    QString linksCustomBrowser;
    bool awaySeparatorLines;
    QColor awaySeparatorColor;
    int awaySeparatorLength;
    QString awaySeparatorPeriod;
    bool copyOnSelection;
    bool autoconnection;
    int autoconnectionDelay;
    bool soundBeepEnabled;
    bool soundBeepDefault;
    QString soundBeepFileName;
    bool soundAboutMeEnabled;
    bool soundAboutMeDefault;
    QString soundAboutMeFileName;
    bool idleAway;
    int idleAwayTimeout;
    QStringList idleAwayBypassExpressions;
    static const QStringList idleAwayBypassDefaultExpressions;
    bool idleQuit;
    int idleQuitTimeout;
    QString clientVersion;
    QStringList persistentProperties;
    QStringList persistentSessionProperties;
    Properties properties;

    bool behindNAT; // If true, CeB considere you are behind a NAT and you're not directly joinable
    int transferPort;
    bool transferInit;

    ActionManager actionManager;

    TextSkin &textSkin() { return _textSkin; }

    const QList<SessionConfig *> sessionConfigs() const;
    SessionConfig *getSessionConfigByName(const QString &name);
    QString getUniqSessionConfigName();
    QString getAwaySeparator();
    QString getAwaySeparator(QString period, int length);
    QString getBeepFileName();
    QString getAboutMeFileName();
    bool matchIdleAwayBypassExpressions(const QString &str) const;

private:
    Profile();
    ~Profile();

    static Profile *_instance;
    QList<SessionConfig *> _sessionConfigList;
    TextSkin _textSkin;

    void clearSessionConfigList();
    void init();
};

#endif
