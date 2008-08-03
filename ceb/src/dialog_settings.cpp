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

#include <QApplication>
#include <QPushButton>
#include <QHeaderView>
#include <QTreeWidgetItem>

#include "profile.h"
#include "session_config_widget.h"
#include "language_manager.h"
#include "general_settings_widget.h"
#include "shortcuts_settings_widget.h"
#include "fonts_settings_widget.h"
#include "sound_settings_widget.h"
#include "idle_settings_widget.h"
#include "warningo_settings_widget.h"
#include "detailed_fonts_settings_widget.h"
#include "links_settings_widget.h"
#include "misc_settings_widget.h"
#include "tabs_settings_widget.h"
#include "logs_settings_widget.h"
#include "output_settings_widget.h"
#include "logger.h"

#include "dialog_settings.h"

DialogSettings::DialogSettings(QWidget *parent): DialogConfig(parent)
{
    treeMain->setColumnCount(1);
    treeMain->setHeaderLabels(QStringList(tr("Categories")));
    treeMain->header()->setMovable(false);
    treeMain->header()->setResizeMode(QHeaderView::Stretch);
    createNode(0, addSettingsWidget(new GeneralSettingsWidget), tr("General", "Settings node"), QIcon(":/images/transparent.png"));
    createNode(0, addSettingsWidget(new ShortcutsSettingsWidget), tr("Shortcuts", "Settings node"), QIcon(":/images/transparent.png"));
    createConnectionsNodes();
    QTreeWidgetItem *item = createNode(0, addSettingsWidget(new FontsSettingsWidget), tr("Fonts", "Settings node"), QIcon(":/images/transparent.png"));
    createNode(item, addSettingsWidget(new DetailedFontsSettingsWidget), tr("Detailed", "Settings node"), QIcon(":/images/transparent.png"));
    createNode(0, addSettingsWidget(new LogsSettingsWidget), tr("Logs", "Settings node"), QIcon(":/images/logs.png"));
    createNode(0, createTrayWidget(), tr("Tray", "Settings node"), QIcon(":/images/transparent.png"));
    createNode(0, addSettingsWidget(new WarningoSettingsWidget), tr("Warningo", "Settings node"), QIcon(":/images/warningo.png"));
    createNode(0, addSettingsWidget(new SoundSettingsWidget), tr("Sounds", "Settings node"), QIcon(":/images/sounds.png"));
    createNode(0, addSettingsWidget(new IdleSettingsWidget), tr("Idle", "Settings node"), QIcon(":/images/transparent.png"));
    createNode(0, addSettingsWidget(new TabsSettingsWidget), tr("Tabs", "Settings node"), QIcon(":/images/tabs.png"));
    createNode(0, addSettingsWidget(new LinksSettingsWidget), tr("Links", "Settings node"), QIcon(":/images/links.png"));
    createNode(0, addSettingsWidget(new OutputSettingsWidget), tr("Output", "Settings node"), QIcon(":/images/transparent.png"));
    createNode(0, addSettingsWidget(new MiscSettingsWidget), tr("Misc", "Settings node"), QIcon(":/images/transparent.png"));

    // Apply profile on every SettingsWidget
    foreach (SettingsWidget *widget, _settingsWidgets)
        widget->applyProfile(Profile::instance());

    // Save old things
    _oldTextSkin = Profile::instance().textSkin();

    // Focus on first node
    treeMain->setCurrentItem(treeMain->topLevelItem(0));

    resize(662, 479);
}

SettingsWidget *DialogSettings::addSettingsWidget(SettingsWidget *widget)
{
    _settingsWidgets << widget;
    return widget;
}

void DialogSettings::createConnectionsNodes()
{
    _itemConnections = createNode(0, createConnectionsWidget(), tr("Connections"), QIcon(":/images/connect.png"));

    for (int i = 0; i < Profile::instance().sessionConfigs().count(); i++)
    {
        const SessionConfig &config = *Profile::instance().sessionConfigs().value(i);

        // Create a widget for the connection
        SessionConfigWidget *sessionConfigWidget = new SessionConfigWidget(0, true);
        connect(sessionConfigWidget, SIGNAL(removeMe()),
                this, SLOT(removeSessionConfig()));
        sessionConfigWidget->init(config);

        // Create a node for the config
        createNode(_itemConnections, sessionConfigWidget, config.name(), QIcon(":/images/transparent.png"));
    }
}

QWidget *DialogSettings::createTrayWidget()
{
    QWidget *mainWidget = new QWidget;
    QVBoxLayout *mainLayout = new QVBoxLayout(mainWidget);
    mainLayout->setMargin(2);

    _groupBoxTray = new QGroupBox(tr("Enable tray icon"));
    mainLayout->addWidget(_groupBoxTray);
    _groupBoxTray->setCheckable(true);

    QVBoxLayout *trayLayout = new QVBoxLayout(_groupBoxTray);

    _checkBoxTrayAlwaysVisible = new QCheckBox(tr("Always visible"));
    trayLayout->addWidget(_checkBoxTrayAlwaysVisible);
    _checkBoxTrayAlwaysVisible->setChecked(Profile::instance().trayAlwaysVisible);

    _checkBoxTrayHideFromTaskBar = new QCheckBox(tr("Hide from task bar when minimized"));
    trayLayout->addWidget(_checkBoxTrayHideFromTaskBar);
    _checkBoxTrayHideFromTaskBar->setChecked(Profile::instance().trayHideFromTaskBar);

    _groupBoxTray->setChecked(Profile::instance().trayEnabled);

    // End spacer
    QSpacerItem *spacer = new QSpacerItem(0, 0, QSizePolicy::Fixed,
                                          QSizePolicy::Expanding);
    mainLayout->addItem(spacer);

    return mainWidget;
}

QWidget *DialogSettings::createConnectionsWidget()
{
    QWidget *mainWidget = new QWidget;
    QVBoxLayout *mainLayout = new QVBoxLayout(mainWidget);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch();
    _pushButtonNewConnection = new QPushButton(tr("New connection"));
    _pushButtonNewConnection->setIcon(QIcon(":/images/add.png"));
    buttonLayout->addWidget(_pushButtonNewConnection);
    connect(_pushButtonNewConnection, SIGNAL(clicked()),
            this, SLOT(newSessionConfig()));
    buttonLayout->addStretch();
    mainLayout->addLayout(buttonLayout);

    return mainWidget;
}

void DialogSettings::checkValues()
{
    if (verifyControlsDatas())
    {
        getControlsDatas();
        accept();
    }
}

bool DialogSettings::verifyControlsDatas()
{
    // Connections
    for (int i = 0; i < _itemConnections->childCount(); i++)
    {
        SessionConfigWidget *widget =
            qobject_cast<SessionConfigWidget*>(getWidgetByNode(_itemConnections->child(i)));

        if (!widget->check())
        {
            // Focus on widget
            treeMain->setCurrentItem(_itemConnections->child(i));
            return false;
        }
    }

    return true;
}

void DialogSettings::getControlsDatas()
{
    foreach (SettingsWidget *widget, _settingsWidgets)
        widget->feedProfile(Profile::instance());

    getConnectionsControlsDatas();
    getTrayControlsDatas();
}

void DialogSettings::getConnectionsControlsDatas()
{
    for (int i = 0; i < _itemConnections->childCount(); i++)
    {
        SessionConfigWidget *widget =
            qobject_cast<SessionConfigWidget*>(getWidgetByNode(_itemConnections->child(i)));

        // Refresh config in profile
        SessionConfig *config = Profile::instance().getSessionConfig(widget->oldName());
        widget->get(*config);
    }
}

void DialogSettings::getTrayControlsDatas()
{
    Profile::instance().trayEnabled = _groupBoxTray->isChecked();
    Profile::instance().trayAlwaysVisible = _checkBoxTrayAlwaysVisible->isChecked();
    Profile::instance().trayHideFromTaskBar = _checkBoxTrayHideFromTaskBar->isChecked();
}

void DialogSettings::newSessionConfig()
{
    // Widget
    SessionConfigWidget *sessionConfigWidget = new SessionConfigWidget(0, true);

    SessionConfig newConfig = SessionConfig::getTemplate();
    newConfig.setName(Profile::instance().getUniqSessionConfigName());
    Profile::instance().addSessionConfig(newConfig);
    sessionConfigWidget->init(newConfig);
    connect(sessionConfigWidget, SIGNAL(removeMe()),
            this, SLOT(removeSessionConfig()));

    // Node
    QTreeWidgetItem *item = createNode(_itemConnections, sessionConfigWidget, newConfig.name(), QIcon(":/images/transparent.png"));
    treeMain->setCurrentItem(item);
}

void DialogSettings::removeSessionConfig()
{
    // Remove the session config
    SessionConfigWidget *widget = qobject_cast<SessionConfigWidget*>(sender());

    // Remove internal datas
    Profile::instance().deleteSessionConfig(widget->oldName());

    // Destroy the node and widget
    deleteNode(getNodeByWidget(widget));

    // Focus on the connections node
    treeMain->setCurrentItem(_itemConnections);
}

QString DialogSettings::getLanguageDisplay(const QString &language)
{
    return LanguageManager::getLanguageDisplayName(language) + " (" + language + ")";
}

void DialogSettings::reject()
{
    // Recopy old profile into the new
    TextSkin &newTextSkin = Profile::instance().textSkin();
    newTextSkin = _oldTextSkin;

    DialogConfig::reject();
}
