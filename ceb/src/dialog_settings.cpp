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

#include "common.h"
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
#include "tray_settings_widget.h"
#include "logger.h"

#include "dialog_settings.h"


DialogSettings::DialogSettings(QWidget *parent): DialogConfig(parent)
{
    QIcon transpIcon(":/images/transparent.png");

    treeMain->setColumnCount(1);
    treeMain->setHeaderLabels(QStringList(tr("Categories")));
    treeMain->header()->setMovable(false);
    treeMain->header()->setResizeMode(QHeaderView::Stretch);
    addSettingsWidget(0, new GeneralSettingsWidget, tr("General", "Settings node"), transpIcon);
    addSettingsWidget(0, new ShortcutsSettingsWidget, tr("Shortcuts", "Settings node"), transpIcon);
    createConnectionsNodes();
    QTreeWidgetItem *item = addSettingsWidget(0, new FontsSettingsWidget, tr("Fonts", "Settings node"), transpIcon);
    addSettingsWidget(item, new DetailedFontsSettingsWidget, tr("Detailed", "Settings node"), transpIcon);
    addSettingsWidget(0, new LogsSettingsWidget, tr("Logs", "Settings node"), QIcon(":/images/logs.png"));
    addSettingsWidget(0, new TraySettingsWidget, tr("Tray", "Settings node"), transpIcon);
    addSettingsWidget(0, new WarningoSettingsWidget, tr("Warningo", "Settings node"), QIcon(":/images/warningo.png"));
    addSettingsWidget(0, new SoundSettingsWidget, tr("Sounds", "Settings node"), QIcon(":/images/sounds.png"));
    addSettingsWidget(0, new IdleSettingsWidget, tr("Idle", "Settings node"), transpIcon);
    addSettingsWidget(0, new TabsSettingsWidget, tr("Tabs", "Settings node"), QIcon(":/images/tabs.png"));
    addSettingsWidget(0, new LinksSettingsWidget, tr("Links", "Settings node"), QIcon(":/images/links.png"));
    addSettingsWidget(0, new OutputSettingsWidget, tr("Output", "Settings node"), transpIcon);
    addSettingsWidget(0, new MiscSettingsWidget, tr("Misc", "Settings node"), transpIcon);

    // Apply profile on every SettingsWidget
    foreach (SettingsWidget *widget, _settingsWidgets)
        widget->applyProfile(Profile::instance());

    // Save old things
    _oldTextSkin = Profile::instance().textSkin();

    // Focus on first node
    treeMain->setCurrentItem(treeMain->topLevelItem(0));

    resize(662, 479);
}

QTreeWidgetItem *DialogSettings::addSettingsWidget(QTreeWidgetItem *father, SettingsWidget *widget,
                                                   const QString &label, const QIcon &icon)
{
    _settingsWidgets << widget;
    return createNode(father, widget, label, icon);
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
