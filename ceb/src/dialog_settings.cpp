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
#include <QFontDialog>
#include <QFileDialog>
#include <QScrollArea>
#include <QColorDialog>
#include <QMessageBox>
#include <QSound>

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
    createNode(0, createLogsWidget(), tr("Logs", "Settings node"), QIcon(":/images/logs.png"));
    createNode(0, createTrayWidget(), tr("Tray", "Settings node"), QIcon(":/images/transparent.png"));
    createNode(0, addSettingsWidget(new WarningoSettingsWidget), tr("Warningo", "Settings node"), QIcon(":/images/warningo.png"));
    createNode(0, addSettingsWidget(new SoundSettingsWidget), tr("Sounds", "Settings node"), QIcon(":/images/sounds.png"));
    createNode(0, addSettingsWidget(new IdleSettingsWidget), tr("Idle", "Settings node"), QIcon(":/images/transparent.png"));
    createNode(0, addSettingsWidget(new TabsSettingsWidget), tr("Tabs", "Settings node"), QIcon(":/images/tabs.png"));
    createNode(0, addSettingsWidget(new LinksSettingsWidget), tr("Links", "Settings node"), QIcon(":/images/links.png"));
    createNode(0, createOutputWidget(), tr("Output", "Settings node"), QIcon(":/images/transparent.png"));
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

QWidget *DialogSettings::createLogsWidget()
{
    QWidget *mainWidget = new QWidget;
    QVBoxLayout *mainLayout = new QVBoxLayout(mainWidget);
    mainLayout->setMargin(2);

    _groupBoxLogs = new QGroupBox(tr("Enable logs"));
    mainLayout->addWidget(_groupBoxLogs);
    _groupBoxLogs->setCheckable(true);

    QVBoxLayout *logsLayout = new QVBoxLayout(_groupBoxLogs);

    QTabWidget *tabWidgetLogs = new QTabWidget;
    logsLayout->addWidget(tabWidgetLogs);

    QWidget *widgetDir = new QWidget;
    tabWidgetLogs->addTab(widgetDir, tr("Logs directory"));
    QVBoxLayout *logsDirLayout = new QVBoxLayout(widgetDir);

    // Default directory
    _radioButtonLogsDefaultDir = new QRadioButton(tr("Default directory: "));
    logsDirLayout->addWidget(_radioButtonLogsDefaultDir);
    QLineEdit *lineEditDefaultDir = new QLineEdit;
    lineEditDefaultDir->setReadOnly(true);
    QString defaultDir = Logger::getDefaultLogsDir();
    lineEditDefaultDir->setText(defaultDir);

    QHBoxLayout *hLayout = new QHBoxLayout;
    logsDirLayout->addLayout(hLayout);
    QSpacerItem *spacer = new QSpacerItem(20, 0, QSizePolicy::Fixed,
                                          QSizePolicy::Fixed);
    hLayout->addItem(spacer);
    hLayout->addWidget(lineEditDefaultDir);
    connect(_radioButtonLogsDefaultDir, SIGNAL(toggled(bool)),
            lineEditDefaultDir, SLOT(setEnabled(bool)));

    // Custom directory
    _radioButtonLogsCustomDir = new QRadioButton(tr("Custom directory: "));
    logsDirLayout->addWidget(_radioButtonLogsCustomDir);
    _lineEditLogsCustomDir = new QLineEdit;
    _lineEditLogsCustomDir->setText(Profile::instance().logsDir);
    hLayout = new QHBoxLayout;
    hLayout->setSpacing(0);
    logsDirLayout->addLayout(hLayout);
    spacer = new QSpacerItem(20, 0, QSizePolicy::Fixed,
                             QSizePolicy::Fixed);
    hLayout->addItem(spacer);
    hLayout->addWidget(_lineEditLogsCustomDir);
    QPushButton *fileButton = new QPushButton("...");
    fileButton->setFixedWidth(25);
    hLayout->addWidget(fileButton);
    connect(fileButton, SIGNAL(clicked()), this, SLOT(logsCustomDirClicked()));
    connect(_radioButtonLogsCustomDir, SIGNAL(toggled(bool)),
            _lineEditLogsCustomDir, SLOT(setEnabled(bool)));
    connect(_radioButtonLogsCustomDir, SIGNAL(toggled(bool)),
            fileButton, SLOT(setEnabled(bool)));

    if (Profile::instance().logsDefaultDir)
        _radioButtonLogsDefaultDir->setChecked(true);
    else
        _radioButtonLogsCustomDir->setChecked(true);
    lineEditDefaultDir->setEnabled(Profile::instance().logsDefaultDir);
    _lineEditLogsCustomDir->setEnabled(!Profile::instance().logsDefaultDir);
    fileButton->setEnabled(!Profile::instance().logsDefaultDir);

    // Logs file policy
    QWidget *widgetFile = new QWidget;
    tabWidgetLogs->addTab(widgetFile, tr("Logs file policy"));
    QVBoxLayout *logsFileLayout = new QVBoxLayout(widgetFile);
    QLabel *label = new QLabel(tr("Save your logs in a file:"));
    logsFileLayout->addWidget(label);
    _radioButtonLogsDaily = new QRadioButton(tr("Daily"));
    logsFileLayout->addWidget(_radioButtonLogsDaily);
    _radioButtonLogsWeekly = new QRadioButton(tr("Weekly (from monday to sunday)"));
    logsFileLayout->addWidget(_radioButtonLogsWeekly);
    _radioButtonLogsMonthly = new QRadioButton(tr("Monthly"));
    logsFileLayout->addWidget(_radioButtonLogsMonthly);
    _radioButtonLogsUniq = new QRadioButton(tr("Uniq"));
    logsFileLayout->addWidget(_radioButtonLogsUniq);

    switch (Profile::instance().logsFilePolicy)
    {
    case Profile::LogFilePolicy_Daily:
        _radioButtonLogsDaily->setChecked(true);
        break;
    case Profile::LogFilePolicy_Weekly:
        _radioButtonLogsWeekly->setChecked(true);
        break;
    case Profile::LogFilePolicy_Monthly:
        _radioButtonLogsMonthly->setChecked(true);
        break;
    case Profile::LogFilePolicy_Uniq:
        _radioButtonLogsUniq->setChecked(true);
        break;
    }

    _groupBoxLogs->setChecked(Profile::instance().logsEnabled);

    // Misc
    QWidget *widgetMisc = new QWidget;
    tabWidgetLogs->addTab(widgetMisc, tr("Misc"));
    _checkBoxLogsTimeStamp = new QCheckBox(tr("Alway timestamps"));
    QVBoxLayout *miscLayout = new QVBoxLayout(widgetMisc);
    miscLayout->addWidget(_checkBoxLogsTimeStamp);
    _checkBoxLogsTimeStamp->setChecked(Profile::instance().logsTimeStamp);
    spacer = new QSpacerItem(0, 0, QSizePolicy::Fixed, QSizePolicy::Expanding);
    miscLayout->addItem(spacer);

    // End spacer
    spacer = new QSpacerItem(0, 0, QSizePolicy::Fixed,
                             QSizePolicy::Expanding);
    mainLayout->addItem(spacer);
    return mainWidget;
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

QWidget *DialogSettings::createOutputWidget()
{
    QWidget *mainWidget = new QWidget;
    QVBoxLayout *mainLayout = new QVBoxLayout(mainWidget);
    mainLayout->setMargin(2);

    _groupBoxAwaySeparator = new QGroupBox(tr("Enable away/back separator lines"));
    mainLayout->addWidget(_groupBoxAwaySeparator);
    _groupBoxAwaySeparator->setCheckable(true);
    _groupBoxAwaySeparator->setChecked(Profile::instance().awaySeparatorLines);

    QVBoxLayout *groupBoxAwayLayout = new QVBoxLayout(_groupBoxAwaySeparator);

    QHBoxLayout *colorLayout = new QHBoxLayout;
    groupBoxAwayLayout->addLayout(colorLayout);
    _lineEditAwaySeparatorColor = new QLineEdit;
    _lineEditAwaySeparatorColor->setReadOnly(true);
    colorLayout->addWidget(_lineEditAwaySeparatorColor);
    _pushButtonAwaySeparatorColor = new QPushButton(tr("Pick a color"));
    QSizePolicy policy = _pushButtonAwaySeparatorColor->sizePolicy();
    policy.setHorizontalPolicy(QSizePolicy::Fixed);
    _pushButtonAwaySeparatorColor->setSizePolicy(policy);
    connect(_pushButtonAwaySeparatorColor, SIGNAL(clicked()),
            this, SLOT(changeAwaySeparatorColor()));
    colorLayout->addWidget(_pushButtonAwaySeparatorColor);

    QPalette palette = _lineEditAwaySeparatorColor->palette();
    palette.setColor(QPalette::Text, Profile::instance().awaySeparatorColor);
    palette.setColor(QPalette::Base, QColor(255, 255, 240));
    _lineEditAwaySeparatorColor->setPalette(palette);

    QHBoxLayout *lengthLayout = new QHBoxLayout;
    groupBoxAwayLayout->addLayout(lengthLayout);
    QLabel *label = new QLabel(tr("Lines length: "));
    lengthLayout->addWidget(label);
    _spinBoxAwaySeparatorLength = new QSpinBox;
    policy = _spinBoxAwaySeparatorLength->sizePolicy();
    policy.setHorizontalPolicy(QSizePolicy::Fixed);
    _spinBoxAwaySeparatorLength->setSizePolicy(policy);
    lengthLayout->addWidget(_spinBoxAwaySeparatorLength);
    _spinBoxAwaySeparatorLength->setMinimum(1);
    _spinBoxAwaySeparatorLength->setMaximum(200);
    _spinBoxAwaySeparatorLength->setValue(Profile::instance().awaySeparatorLength);
    connect(_spinBoxAwaySeparatorLength, SIGNAL(valueChanged(int)),
            this, SLOT(refreshAwaySeparatorPreview()));

    QHBoxLayout *periodLayout = new QHBoxLayout;
    groupBoxAwayLayout->addLayout(periodLayout);
    label = new QLabel(tr("Periodic separator string:"));
    periodLayout->addWidget(label);
    _lineEditAwaySeparatorPeriod = new QLineEdit;
    periodLayout->addWidget(_lineEditAwaySeparatorPeriod);
    _lineEditAwaySeparatorPeriod->setText(Profile::instance().awaySeparatorPeriod);
    connect(_lineEditAwaySeparatorPeriod, SIGNAL(textChanged(const QString &)),
            this, SLOT(refreshAwaySeparatorPreview()));

    refreshAwaySeparatorPreview();

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
    getLogsControlsDatas();
    getTrayControlsDatas();
    getOutputControlsDatas();
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

void DialogSettings::getLogsControlsDatas()
{
    Profile::instance().logsEnabled = _groupBoxLogs->isChecked();
    Profile::instance().logsDefaultDir = _radioButtonLogsDefaultDir->isChecked();
    Profile::instance().logsDir = _lineEditLogsCustomDir->text();
    if (_radioButtonLogsDaily->isChecked())
        Profile::instance().logsFilePolicy = Profile::LogFilePolicy_Daily;
    else if (_radioButtonLogsWeekly->isChecked())
        Profile::instance().logsFilePolicy = Profile::LogFilePolicy_Weekly;
    else if (_radioButtonLogsMonthly->isChecked())
        Profile::instance().logsFilePolicy = Profile::LogFilePolicy_Monthly;
    else if (_radioButtonLogsUniq->isChecked())
        Profile::instance().logsFilePolicy = Profile::LogFilePolicy_Uniq;
    Profile::instance().logsTimeStamp = _checkBoxLogsTimeStamp->isChecked();
}

void DialogSettings::getTrayControlsDatas()
{
    Profile::instance().trayEnabled = _groupBoxTray->isChecked();
    Profile::instance().trayAlwaysVisible = _checkBoxTrayAlwaysVisible->isChecked();
    Profile::instance().trayHideFromTaskBar = _checkBoxTrayHideFromTaskBar->isChecked();
}

void DialogSettings::getOutputControlsDatas()
{
    Profile::instance().awaySeparatorLines = _groupBoxAwaySeparator->isChecked();
    QPalette palette = _lineEditAwaySeparatorColor->palette();
    Profile::instance().awaySeparatorColor = palette.color(QPalette::Text);
    Profile::instance().awaySeparatorLength = _spinBoxAwaySeparatorLength->value();
    Profile::instance().awaySeparatorPeriod = _lineEditAwaySeparatorPeriod->text();
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

void DialogSettings::logsCustomDirClicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Choose a logs directory"));
    _lineEditLogsCustomDir->setText(dir);
}

void DialogSettings::changeAwaySeparatorColor()
{
    QColor color = QColorDialog::getColor(Profile::instance().awaySeparatorColor);
    if (color.isValid())
    {
        QPalette palette = _lineEditAwaySeparatorColor->palette();
        palette.setColor(QPalette::Text, color);
        _lineEditAwaySeparatorColor->setPalette(palette);
    }
}

QString DialogSettings::getLanguageDisplay(const QString &language)
{
    return LanguageManager::getLanguageDisplayName(language) + " (" + language + ")";
}

void DialogSettings::awaySeparatorLengthChanged(int)
{
    refreshAwaySeparatorPreview();
}

void DialogSettings::refreshAwaySeparatorPreview()
{
    QString period = _lineEditAwaySeparatorPeriod->text();
    int length = _spinBoxAwaySeparatorLength->value();
    _lineEditAwaySeparatorColor->setText(Profile::instance().getAwaySeparator(period, length));
}

void DialogSettings::reject()
{
    // Recopy old profile into the new
    TextSkin &newTextSkin = Profile::instance().textSkin();
    newTextSkin = _oldTextSkin;

    DialogConfig::reject();
}
