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

#include <QCoreApplication>
#include <QDir>
#include <QUrl>
#include <QTextStream>
#include <QFile>
#include <QMessageBox>
#include <QFileInfo>
#include <QDomDocument>
#include <QTextStream>
#include <QTranslator>

#include <xml_handler.h>

#include "version.h"
#include "global.h"
#include "paths.h"
#include "language_manager.h"

#include "profile.h"

Profile *Profile::_instance = 0;
const QStringList Profile::idleAwayBypassDefaultExpressions = QStringList() << "^who(|( .*))$" << "^wall$" << "^history$" << "^set away off$" << "^users(|( .*))$" << "^get .* group$";

Profile &Profile::instance()
{
    if (!_instance)
        _instance = new Profile;

    return *_instance;
}

Profile::Profile()
{
    init();
}

void Profile::init()
{
    mainWidth = -1;
    mainHeight = -1;
    mainLeft = -1;
    mainTop = -1;
#ifdef Q_OS_WIN32
    keepAboveOtherWindows = false;
#endif
    language = "";
    checkForUpdate = true;
    menuBarVisible = true;
    statusBarVisible = true;
    hideTabsForOne = true;
    systemLogsVisible = false;
    topicWindowVisible = true;
    usersWindowVisible = true;
    timeStampPolicy = Policy_Classic;
    timeStampInTellTabs = true;
    keepAlive = 0;
    logsEnabled = false;
    logsDefaultDir = true;
    logsFilePolicy = LogFilePolicy_Daily;
    logsTimeStamp = false;
    tabForWho = false;
    tabForWall = true;
    tabForFinger = true;
    trayEnabled = true;
    trayAlwaysVisible = true;
    trayHideFromTaskBar = true;
    warningoEnabled = true;
    warningoLifeTime = 3000;
    warningoLocation = WarningoLocation_TopLeft;
    warningoScreen = 0;
    warningoPrivate = true;
    warningoHighlight = true;
    tabsPosition = QTabWidget::South;
    tabsChannelCaptionMode = 0;
    linksCustomBrowser = "";
    awaySeparatorLines = true;
    awaySeparatorColor = QColor(0, 225, 0);
    awaySeparatorLength = 80;
    awaySeparatorPeriod = "\\_o<..........";
    copyOnSelection = false;
    autoconnection = true;
    autoconnectionDelay = 15;
    soundBeepEnabled = true;
    soundBeepDefault = true;
    soundBeepFileName = "";
    soundAboutMeEnabled = false;
    soundAboutMeDefault = true;
    soundAboutMeFileName = "";
    idleAway = true;
    idleAwayTimeout = 60;
    idleAwayBypassExpressions = idleAwayBypassDefaultExpressions;
    idleQuit = true;
    idleQuitTimeout = 600;
    clientVersion = "";
    behindNAT = false;
    transferPort = 4001;
    transferInit = false;
	uploadUrl = "";
	downloadUrl = "";
    prefixDate = true;
}

Profile::~Profile()
{
    clearSessionConfigList();
}

void Profile::clearSessionConfigList()
{
    while (_sessionConfigList.count() > 0)
    {
        SessionConfig *config = _sessionConfigList.value(0);
        delete config;
        _sessionConfigList.removeAt(0);
    }
}

bool Profile::load()
{
    // Load and parse
    QFile file(QDir(Paths::profilePath()).filePath("settings.xml"));

    if (!file.exists())
    {
        // Search for the "old school" file
        QDir profilesDir(QDir(QCoreApplication::applicationDirPath()).filePath("profiles"));
        file.setFileName(profilesDir.filePath("default.xml"));
    }

    if (!file.open(QFile::ReadOnly))
        return false;

    QDomDocument document;

    QString errorMsg;
    int errorLine, errorColumn;
    if (!document.setContent(&file, &errorMsg, &errorLine, &errorColumn))
    {
        qDebug("%s at line %d, column %d", qPrintable(errorMsg),
               errorLine, errorColumn);
        return false;
    }

    file.close();

    QDomElement rootElem = document.documentElement();

    // General parameters
    mainWidth = XmlHandler::read(rootElem, "main_width", -1);
    mainHeight = XmlHandler::read(rootElem, "main_height", -1);
    mainLeft = XmlHandler::read(rootElem, "main_left", -1);
    mainTop = XmlHandler::read(rootElem, "main_top", -1);
#ifdef Q_OS_WIN32
    keepAboveOtherWindows = XmlHandler::read(rootElem, "keep_above", false);
#endif
    language = XmlHandler::read(rootElem, "language", "");
    checkForUpdate = XmlHandler::read(rootElem, "check_for_update", true);
    menuBarVisible = XmlHandler::read(rootElem, "menu_bar_visible", true);
    statusBarVisible = XmlHandler::read(rootElem, "status_bar_visible", true);
    hideTabsForOne = XmlHandler::read(rootElem, "hide_tabs_for_one", true);

    // Load the language
    if (language != "")
    {
        QString fileName = LanguageManager::getLanguageFileName(language);
        if (!fileName.isEmpty())
        {
            QTranslator *translator = new QTranslator(qApp);
            translator->load(fileName);
            qApp->installTranslator(translator);
        }
    }

    const QDomElement systemLogsElem = rootElem.firstChildElement("system_logs");
    if (!systemLogsElem.isNull())
        systemLogsVisible = XmlHandler::read(systemLogsElem, "visible", false);

    // Topic
    topicWindowVisible = XmlHandler::read(rootElem, "topic_window_visible", true);
    usersWindowVisible = XmlHandler::read(rootElem, "users_window_visible", true);

    // Time stamp
    timeStampPolicy = (TimeStampPolicy) XmlHandler::read(rootElem, "time_stamp_policy", 0);
    timeStampInTellTabs = XmlHandler::read(rootElem, "time_stamp_in_tell_tabs", true);

    // Actions
    actionManager.load(rootElem);

    // Keep alive
    keepAlive = XmlHandler::read(rootElem, "keep_alive", 0);

    // Logs
    const QDomElement logsElem = rootElem.firstChildElement("logs");
    if (!logsElem.isNull())
    {
        logsEnabled = XmlHandler::read(logsElem, "enabled", false);
        logsDefaultDir = XmlHandler::read(logsElem, "default_dir", true);
        logsDir = XmlHandler::read(logsElem, "dir", "");
        logsFilePolicy = (LogFilePolicy) XmlHandler::read(logsElem, "file_policy", 0);
        logsTimeStamp = XmlHandler::read(logsElem, "time_stamp", false);
    }

    // Tab for...
    tabForWho = XmlHandler::read(rootElem, "tab_for_who", false);
    tabForWall = XmlHandler::read(rootElem, "tab_for_wall", true);
    tabForFinger = XmlHandler::read(rootElem, "tab_for_finger", true);

    // Version of old client recorded in profile
    clientVersion = XmlHandler::read(rootElem, "client_version", "");

    // Tray
    const QDomElement trayElem = rootElem.firstChildElement("tray");
    if (!trayElem.isNull())
    {
        trayEnabled = XmlHandler::read(trayElem, "enabled", true);
        trayAlwaysVisible = XmlHandler::read(trayElem, "always_visible", true);
        trayHideFromTaskBar = XmlHandler::read(trayElem, "hide_from_task_bar", true);
    }

    // Warningo
    const QDomElement warningoElem = rootElem.firstChildElement("warningo");
    if (!warningoElem.isNull())
    {
        warningoEnabled = XmlHandler::read(warningoElem, "enabled", true);
        warningoLifeTime = XmlHandler::read(warningoElem, "life_time", 3000);
        warningoLocation = (WarningoLocation) XmlHandler::read(warningoElem, "location", 0);
        warningoScreen = XmlHandler::read(warningoElem, "screen", 0);
        warningoPrivate = XmlHandler::read(warningoElem, "private", true);
        warningoHighlight = XmlHandler::read(warningoElem, "highlight", true);
    }

    // Tabs
    const QDomElement tabsElem = rootElem.firstChildElement("tabs");
    if (!tabsElem.isNull())
    {
        tabsPosition = (QTabWidget::TabPosition) XmlHandler::read(tabsElem, "tabs_position", (int) QTabWidget::South);
    }

    // Links
    linksCustomBrowser = XmlHandler::read(rootElem, "links_custom_browser", "");

    // Away Separator line
    const QDomElement awaySepElem = rootElem.firstChildElement("away_separator");
    if (!awaySepElem.isNull())
    {
        awaySeparatorLines = XmlHandler::read(awaySepElem, "enabled", true);
        awaySeparatorColor.setNamedColor(XmlHandler::read(awaySepElem, "color", "#00E100"));
        awaySeparatorLength = XmlHandler::read(awaySepElem, "length", 80);
        awaySeparatorPeriod = XmlHandler::read(awaySepElem, "period", "-8<--");
    }

    copyOnSelection = XmlHandler::read(rootElem, "copy_on_selection", false);

    // Autoconnect
    const QDomElement autoconnectionSepElem = rootElem.firstChildElement("autoconnection");
    if (!autoconnectionSepElem.isNull())
    {
        autoconnection = XmlHandler::read(autoconnectionSepElem, "enabled", true);
        autoconnectionDelay = XmlHandler::read(autoconnectionSepElem, "delay", 15);
    }

    // Sound
    const QDomElement soundElem = rootElem.firstChildElement("sound");
    if (!soundElem.isNull())
    {
        const QDomElement beepElem = soundElem.firstChildElement("beep");
        if (!beepElem.isNull())
        {
            soundBeepEnabled = XmlHandler::read(beepElem, "enabled", true);
            soundBeepDefault = XmlHandler::read(beepElem, "default", true);
            soundBeepFileName = XmlHandler::read(beepElem, "filename", "");
        }
        const QDomElement aboutMeElem = soundElem.firstChildElement("about_me");
        if (!aboutMeElem.isNull())
        {
            soundAboutMeEnabled = XmlHandler::read(aboutMeElem, "enabled", true);
            soundAboutMeDefault = XmlHandler::read(aboutMeElem, "default", true);
            soundAboutMeFileName = XmlHandler::read(aboutMeElem, "filename", "");
        }
    }

    // Idle
    const QDomElement idleElem = rootElem.firstChildElement("idle");
    if (!idleElem.isNull())
    {
        idleAway = XmlHandler::read(idleElem, "away", idleAway);
        idleAwayTimeout = XmlHandler::read(idleElem, "away_timeout", idleAwayTimeout);
        const QDomElement awayBypassExpressionsElem = idleElem.firstChildElement("away_bypass_expressions");
        if (!awayBypassExpressionsElem.isNull())
        {
            idleAwayBypassExpressions.clear();
            QDomElement expressionElem = awayBypassExpressionsElem.firstChildElement("expression");
            while (!expressionElem.isNull())
            {
                idleAwayBypassExpressions << expressionElem.text();
                expressionElem = expressionElem.nextSiblingElement("expression");
            }
        }
        idleQuit = XmlHandler::read(idleElem, "quit", idleQuit);
        idleQuitTimeout = XmlHandler::read(idleElem, "quit_timeout", idleQuitTimeout);
    }

    // Session configurations
    clearSessionConfigList();

    const QDomElement sessionsElem = rootElem.firstChildElement("sessions");
    if (!sessionsElem.isNull())
    {
        QDomElement sessionElem = sessionsElem.firstChildElement("session");
        while (!sessionElem.isNull())
        {
            SessionConfig *config = new SessionConfig;
            _sessionConfigList << config;
            config->load(sessionElem);
            sessionElem = sessionElem.nextSiblingElement("session");
        }
    }

    behindNAT = XmlHandler::read(rootElem, "behind_nat", true);
    transferPort = XmlHandler::read(rootElem, "transfer_port", 4001);
    transferInit = XmlHandler::read(rootElem, "transfer_init", false);

    // Text skin
    _textSkin.load(rootElem);

    // Load persistent names
    persistentProperties.clear();
    QDir modifiersDir(QDir(Paths::sharePath()).filePath("modifiers"));
    QFile propertiesFile(modifiersDir.filePath("properties"));
    if (propertiesFile.open(QIODevice::ReadOnly))
    {
        QTextStream stream(&propertiesFile);

        QString line;
        do
        {
            line = stream.readLine().trimmed();
            if (line != "")
                persistentProperties << line;
        } while (!line.isNull());

        propertiesFile.close();
    }

    persistentSessionProperties.clear();
    QFile sessionPropertiesFile(modifiersDir.filePath("session_properties"));
    if (sessionPropertiesFile.open(QIODevice::ReadOnly))
    {
        QTextStream stream(&sessionPropertiesFile);

        QString line;
        do
        {
            line = stream.readLine().trimmed();
            if (line != "")
                persistentSessionProperties << line;
        } while (!line.isNull());

        sessionPropertiesFile.close();
    }


    // Persistent properties
    QDomElement propertyElem = rootElem.firstChildElement("properties").firstChildElement("property");
    while (!propertyElem.isNull())
    {
        properties << Property(propertyElem);
        propertyElem = propertyElem.nextSiblingElement("property");
    }

	// ftp upload
	uploadUrl = XmlHandler::read(rootElem, "ftp_upload_url", "");
	downloadUrl = XmlHandler::read(rootElem, "http_download_url", "");
	prefixDate = XmlHandler::read(rootElem, "prefix_date", true);

    return true;
}

void Profile::save() const
{
    // Save
    QDir dataDir(Paths::profilePath());

    dataDir.mkpath(".");
    QFile file(dataDir.filePath("settings.xml"));

    if (!file.open(QFile::WriteOnly | QFile::Text))
    {
        qDebug("failed to save the profile!");
        return;
    }

    QDomDocument document;

    QDomElement rootElem = document.createElement("profile");
    document.appendChild(rootElem);

    // General parameters
    XmlHandler::write(rootElem, "main_width", mainWidth);
    XmlHandler::write(rootElem, "main_height", mainHeight);
    XmlHandler::write(rootElem, "main_left", mainLeft);
    XmlHandler::write(rootElem, "main_top", mainTop);
#ifdef Q_OS_WIN32
    XmlHandler::write(rootElem, "keep_above", keepAboveOtherWindows);
#endif
    XmlHandler::write(rootElem, "hide_tabs_for_one", hideTabsForOne);
    XmlHandler::write(rootElem, "language", language);
    XmlHandler::write(rootElem, "check_for_update", checkForUpdate);
    XmlHandler::write(rootElem, "menu_bar_visible", menuBarVisible);
    XmlHandler::write(rootElem, "status_bar_visible", statusBarVisible);

    // System logs
    QDomElement systemLogsElem = document.createElement("system_logs");
    rootElem.appendChild(systemLogsElem);
    XmlHandler::write(systemLogsElem, "visible", systemLogsVisible);

    // Topic window
    XmlHandler::write(rootElem, "topic_window_visible", topicWindowVisible);
    XmlHandler::write(rootElem, "users_window_visible", usersWindowVisible);

    // Time stamp
    XmlHandler::write(rootElem, "time_stamp_policy", (int) timeStampPolicy);
    XmlHandler::write(rootElem, "time_stamp_in_tell_tabs", timeStampInTellTabs);

    // Actions
    actionManager.save(rootElem);

    // Keep alive
    XmlHandler::write(rootElem, "keep_alive", keepAlive);

    // Logs
    QDomElement logsElem = document.createElement("logs");
    rootElem.appendChild(logsElem);
    XmlHandler::write(logsElem, "enabled", logsEnabled);
    XmlHandler::write(logsElem, "default_dir", logsDefaultDir);
    XmlHandler::write(logsElem, "dir", logsDefaultDir ? "" : logsDir);
    XmlHandler::write(logsElem, "file_policy", (int) logsFilePolicy);
    XmlHandler::write(logsElem, "time_stamp", logsTimeStamp);

    // Tab for...
    XmlHandler::write(rootElem, "tab_for_who", tabForWho);
    XmlHandler::write(rootElem, "tab_for_wall", tabForWall);
    XmlHandler::write(rootElem, "tab_for_finger", tabForFinger);

    // Client version
    XmlHandler::write(rootElem, "client_version", QString(VERSION));

    // Tray
    QDomElement trayElem = document.createElement("tray");
    rootElem.appendChild(trayElem);
    XmlHandler::write(trayElem, "enabled", trayEnabled);
    XmlHandler::write(trayElem, "always_visible", trayAlwaysVisible);
    XmlHandler::write(trayElem, "hide_from_task_bar", trayHideFromTaskBar);

    // Warningo
    QDomElement warningoElem = document.createElement("warningo");
    rootElem.appendChild(warningoElem);
    XmlHandler::write(warningoElem, "enabled", warningoEnabled);
    XmlHandler::write(warningoElem, "life_time", warningoLifeTime);
    XmlHandler::write(warningoElem, "location", (int) warningoLocation);
    XmlHandler::write(warningoElem, "screen", warningoScreen);
    XmlHandler::write(warningoElem, "private", warningoPrivate);
    XmlHandler::write(warningoElem, "highlight", warningoHighlight);

    // Tabs
    QDomElement tabsElem = document.createElement("tabs");
    rootElem.appendChild(tabsElem);
    XmlHandler::write(tabsElem, "tabs_position", tabsPosition);

    // Links
    XmlHandler::write(rootElem, "links_custom_browser", linksCustomBrowser);

    // Away Separator line
    QDomElement awaySepElem = document.createElement("away_separator");
    rootElem.appendChild(awaySepElem);
    XmlHandler::write(awaySepElem, "enabled", awaySeparatorLines);
    XmlHandler::write(awaySepElem, "color", awaySeparatorColor.name());
    XmlHandler::write(awaySepElem, "length", awaySeparatorLength);
    XmlHandler::write(awaySepElem, "period", awaySeparatorPeriod);

    XmlHandler::write(rootElem, "copy_on_selection", copyOnSelection);

    // Autoconnection
    QDomElement autoconnectionElem = document.createElement("autoconnection");
    rootElem.appendChild(autoconnectionElem);
    XmlHandler::write(autoconnectionElem, "enabled", autoconnection);
    XmlHandler::write(autoconnectionElem, "delay", autoconnectionDelay);

    // Sound
    QDomElement soundElem = document.createElement("sound");
    rootElem.appendChild(soundElem);
    QDomElement beepElem = document.createElement("beep");
    soundElem.appendChild(beepElem);
    XmlHandler::write(beepElem, "enabled", soundBeepEnabled);
    XmlHandler::write(beepElem, "default", soundBeepDefault);
    XmlHandler::write(beepElem, "filename", soundBeepFileName);
    QDomElement aboutMeElem = document.createElement("about_me");
    soundElem.appendChild(aboutMeElem);
    XmlHandler::write(aboutMeElem, "enabled", soundAboutMeEnabled);
    XmlHandler::write(aboutMeElem, "default", soundAboutMeDefault);
    XmlHandler::write(aboutMeElem, "filename", soundAboutMeFileName);

    // Idle
    QDomElement idleElem = document.createElement("idle");
    rootElem.appendChild(idleElem);
    XmlHandler::write(idleElem, "away", idleAway);
    XmlHandler::write(idleElem, "away_timeout", idleAwayTimeout);
    QDomElement expressionsElem = document.createElement("away_bypass_expressions");
    idleElem.appendChild(expressionsElem);
    foreach (const QString &str, idleAwayBypassExpressions)
    {
        QDomElement expressionElem = document.createElement("expression");
        expressionsElem.appendChild(expressionElem);
        QDomText t = document.createTextNode(str);
        expressionElem.appendChild(t);
    }
    XmlHandler::write(idleElem, "quit", idleQuit);
    XmlHandler::write(idleElem, "quit_timeout", idleQuitTimeout);

    // Session config
    QDomElement sessionsElem = document.createElement("sessions");
    rootElem.appendChild(sessionsElem);
    for (int i = 0; i < _sessionConfigList.count(); i++)
    {
        QDomElement elem = document.createElement("session");
        sessionsElem.appendChild(elem);
        _sessionConfigList[i]->save(elem);
    }

    XmlHandler::write(rootElem, "behind_nat", behindNAT);
    XmlHandler::write(rootElem, "transfer_port", transferPort);
    XmlHandler::write(rootElem, "transfer_init", transferInit);

    // Skin
    _textSkin.save(rootElem);

    // Properties
    QDomElement propertiesElem = rootElem.ownerDocument().createElement("properties");
    foreach (const Property &property, properties)
    {
        if (persistentProperties.contains(property.name(), Qt::CaseInsensitive))
        {
            QDomElement propertyElem = rootElem.ownerDocument().createElement("property");
            propertiesElem.appendChild(propertyElem);
            property.save(propertyElem);
        }
    }
    if (!propertiesElem.firstChildElement().isNull())
        rootElem.appendChild(propertiesElem);

	// ftp upload
	XmlHandler::write(rootElem, "ftp_upload_url", uploadUrl);
	XmlHandler::write(rootElem, "http_download_url", downloadUrl);
	XmlHandler::write(rootElem, "prefix_date", prefixDate);

    // Save
    QString xml = document.toString();
    QTextStream out(&file);
    out << xml.toLatin1();
}

SessionConfig *Profile::sessionConfigAt(int i) const
{
    return _sessionConfigList.value(i);;
}

SessionConfig *Profile::getSessionConfig(const QString &name) const
{
    foreach (SessionConfig *pConfig, _sessionConfigList)
        if (pConfig->name() == name)
            return pConfig;
    return 0;
}

void Profile::addSessionConfig(const SessionConfig &config)
{
    SessionConfig *newConfig = new SessionConfig(config);
    _sessionConfigList << newConfig;
}

SessionConfig Profile::addSessionUrl(const QUrl &url)
{
    SessionConfig config;

    config.setName(QObject::tr("New connection"));
    config.setAddress(url.host());
    config.setPort(url.port(4000));
    config.setLogin(url.userName());
    config.setPassword(url.password());
    config.setAutoconnect(true);
    config.setBroadcast(true);
    config.setEncodingMib(111);

    for (int j = 0; j < _sessionConfigList.count(); ++j)
        if (_sessionConfigList[j] && _sessionConfigList[j]->address() == config.address() && _sessionConfigList[j]->port() == config.port() && (_sessionConfigList[j]->login() == config.login() || config.login().isEmpty()))
            return SessionConfig();

    addSessionConfig(config);
    return config;
}

void Profile::deleteSessionConfig(const QString &name)
{
    SessionConfig *config = getSessionConfig(name);
    if (config)
    {
        _sessionConfigList.removeAt(_sessionConfigList.indexOf(config));
        delete config;
    }
}

const QList<SessionConfig *> Profile::sessionConfigs() const
{
    return _sessionConfigList;
}

SessionConfig *Profile::getSessionConfigByName(const QString &name)
{
    foreach (SessionConfig *config, _sessionConfigList)
        if (!config->name().compare(name))
            return config;
    return 0;
}

Profile &Profile::operator=(const Profile &profile)
{
    mainWidth = profile.mainWidth;
    mainHeight = profile.mainHeight;
    mainLeft = profile.mainLeft;
    mainTop = profile.mainTop;
#ifdef Q_OS_WIN32
    keepAboveOtherWindows = profile.keepAboveOtherWindows;
#endif
    language = profile.language;
    checkForUpdate = profile.checkForUpdate;
    menuBarVisible = profile.menuBarVisible;
    statusBarVisible = profile.statusBarVisible;
    hideTabsForOne = profile.hideTabsForOne;
    systemLogsVisible = profile.systemLogsVisible;
    topicWindowVisible = profile.topicWindowVisible;
    usersWindowVisible = profile.usersWindowVisible;
    timeStampPolicy = profile.timeStampPolicy;
    timeStampInTellTabs = profile.timeStampInTellTabs;
    keepAlive = profile.keepAlive;
    logsEnabled = profile.logsEnabled;
    logsDefaultDir = profile.logsDefaultDir;
    logsDir = profile.logsDir;
    logsFilePolicy = profile.logsFilePolicy;
    logsTimeStamp = profile.logsTimeStamp;
    tabForWho = profile.tabForWho;
    tabForWall = profile.tabForWall;
    tabForFinger = profile.tabForFinger;
    trayEnabled = profile.trayEnabled;
    trayAlwaysVisible = profile.trayAlwaysVisible;
    trayHideFromTaskBar = profile.trayHideFromTaskBar;
    warningoEnabled = profile.warningoEnabled;
    warningoLifeTime = profile.warningoLifeTime;
    warningoLocation = profile.warningoLocation;
    warningoScreen = profile.warningoScreen;
    warningoPrivate = profile.warningoPrivate;
    warningoHighlight = profile.warningoHighlight;
    tabsPosition = profile.tabsPosition;
    linksCustomBrowser = profile.linksCustomBrowser;
    awaySeparatorLines = profile.awaySeparatorLines;
    awaySeparatorColor = profile.awaySeparatorColor;
    awaySeparatorLength = profile.awaySeparatorLength;
    awaySeparatorPeriod = profile.awaySeparatorPeriod;
    copyOnSelection = profile.copyOnSelection;
    autoconnection = profile.autoconnection;
    autoconnectionDelay = profile.autoconnectionDelay;
    soundBeepEnabled = profile.soundBeepEnabled;
    soundBeepDefault = profile.soundBeepDefault;
    soundBeepFileName = profile.soundBeepFileName;
    soundAboutMeEnabled = profile.soundAboutMeEnabled;
    soundAboutMeDefault = profile.soundAboutMeDefault;
    soundAboutMeFileName = profile.soundAboutMeFileName;
    idleAway = profile.idleAway;
    idleAwayTimeout = profile.idleAwayTimeout;
    idleAwayBypassExpressions = profile.idleAwayBypassExpressions;
    idleQuit = profile.idleQuit;
    idleQuitTimeout = profile.idleQuitTimeout;
    clientVersion = profile.clientVersion;
    behindNAT = profile.behindNAT;
    transferPort = profile.transferPort;
    transferInit = profile.transferInit;
	uploadUrl = profile.uploadUrl;
	downloadUrl = profile.downloadUrl;
    prefixDate = profile.prefixDate;

    return *this;
}

QString Profile::getUniqSessionConfigName()
{
    for (int i = 0; i <= _sessionConfigList.count(); i++)
    {
        QString tempName = "connection_" + QString::number(i);
        bool found = false;

        for (int j = 0; j < _sessionConfigList.count(); j++)
        {
            const QString &sessionName = _sessionConfigList.value(j)->name();
            if (!sessionName.localeAwareCompare(tempName, sessionName))
            {
                found = true;
                break;
            }
        }
        if (!found)
            return tempName;
    }
    return "connection_" + QString::number(_sessionConfigList.count());
}

QString Profile::getAwaySeparator()
{
    return getAwaySeparator(awaySeparatorPeriod, awaySeparatorLength);
}

QString Profile::getAwaySeparator(QString period, int length)
{
    if (period == "")
        return "";
    QString str;
    int periodPos = 0;
    for (int i = 0; i < length; i++)
    {
        str += period[periodPos];
        periodPos = (periodPos + 1) % period.length();
    }
    return str;
}

QString Profile::getBeepFileName()
{
    if (soundBeepDefault)
    {
        QDir resourcesDir(QDir(QCoreApplication::applicationDirPath()).filePath("resources"));
        return resourcesDir.filePath("notify.wav");
    }
    else
        return soundBeepFileName;
    return "";
}

QString Profile::getAboutMeFileName()
{
    if (soundAboutMeDefault)
    {
        QDir resourcesDir(QDir(QCoreApplication::applicationDirPath()).filePath("resources"));
        return resourcesDir.filePath("notify.wav");
    }
    else
        return soundAboutMeFileName;
    return "";
}

bool Profile::matchIdleAwayBypassExpressions(const QString &str) const
{
    foreach (const QString &expr, idleAwayBypassExpressions)
    {
        QRegExp regExp(expr, Qt::CaseInsensitive);
        if (regExp.isValid() && regExp.exactMatch(str))
            return true;
    }
    return false;
}
