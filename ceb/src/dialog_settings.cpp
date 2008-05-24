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
#include "sound_settings_widget.h"
#include "idle_settings_widget.h"
#include "warningo_settings_widget.h"
#include "fonts_settings_widget.h"
#include "detailed_fonts_settings_widget.h"
#include "links_settings_widget.h"
#include "shortcuts_settings_widget.h"
#include "logger.h"

#include "dialog_settings.h"

DialogSettings::DialogSettings(QWidget *parent): DialogConfig(parent)
{
    treeMain->setColumnCount(1);
    treeMain->setHeaderLabels(QStringList(tr("Categories")));
    treeMain->header()->setMovable(false);
    treeMain->header()->setResizeMode(QHeaderView::Stretch);
    createNode(0, createGeneralWidget(), tr("General", "Settings node"), QIcon(":/images/transparent.png"));
    createNode(0, _shortcutsWidget = createShortcutsWidget(), tr("Shortcuts", "Settings node"), QIcon(":/images/transparent.png"));
    createConnectionsNodes();
    QTreeWidgetItem *item = createNode(0, _fontsWidget = createFontsWidget(), tr("Fonts", "Settings node"), QIcon(":/images/transparent.png"));
    createNode(item, _detailedFontsWidget = createDetailedFontsWidget(), tr("Detailed", "Settings node"), QIcon(":/images/transparent.png"));
    createNode(0, createLogsWidget(), tr("Logs", "Settings node"), QIcon(":/images/logs.png"));
    createNode(0, createTrayWidget(), tr("Tray", "Settings node"), QIcon(":/images/transparent.png"));
    createNode(0, _warningoWidget = createWarningoWidget(), tr("Warningo", "Settings node"), QIcon(":/images/warningo.png"));
    createNode(0, _soundsWidget = createSoundsWidget(), tr("Sounds", "Settings node"), QIcon(":/images/sounds.png"));
    createNode(0, _idleWidget = createIdleWidget(), tr("Idle", "Settings node"), QIcon(":/images/transparent.png"));
    createNode(0, createTabsWidget(), tr("Tabs", "Settings node"), QIcon(":/images/tabs.png"));
    createNode(0, _linksWidget = createLinksWidget(), tr("Links", "Settings node"), QIcon(":/images/links.png"));
    createNode(0, createOutputWidget(), tr("Output", "Settings node"), QIcon(":/images/transparent.png"));
    createNode(0, createMiscWidget(), tr("Misc", "Settings node"), QIcon(":/images/transparent.png"));

    // Save old things
    _oldTextSkin = Profile::instance().textSkin();

    // Focus on first node
    treeMain->setCurrentItem(treeMain->topLevelItem(0));

    resize(600, 400);
}

QWidget *DialogSettings::createGeneralWidget()
{
    QWidget *mainWidget = new QWidget;
    QVBoxLayout *mainLayout = new QVBoxLayout(mainWidget);

    // Language
    QLabel *label = new QLabel(tr("Application language:"));
    mainLayout->addWidget(label);

    _comboBoxLanguage = new QComboBox;
    mainLayout->addWidget(_comboBoxLanguage);

    // Frame
    QFrame *frame = new QFrame;
    frame->setFrameShape(QFrame::HLine);
    mainLayout->addWidget(frame);

    // Update
    _checkBoxCheckForUpdate = new QCheckBox("Check for CeB update at startup");
    mainLayout->addWidget(_checkBoxCheckForUpdate);
    _checkBoxCheckForUpdate->setChecked(Profile::instance().checkForUpdate);

    // End spacer
    mainLayout->addStretch();

    ////// INIT
    QStringList languages = LanguageManager::getAvailableLanguages();
    _comboBoxLanguage->addItem(tr("<Default language>"));
    foreach (QString language, languages)
    {
        _comboBoxLanguage->addItem(getLanguageDisplay(language));
        displayToLanguage.insert(getLanguageDisplay(language), language);
    }

    if (!Profile::instance().language.isEmpty())
    {
        int index = _comboBoxLanguage->findText(getLanguageDisplay(Profile::instance().language));
        if (index >= 0)
            _comboBoxLanguage->setCurrentIndex(index);
    }

    return  mainWidget;
}

QWidget *DialogSettings::createShortcutsWidget()
{
    ShortcutsSettingsWidget *widget = new ShortcutsSettingsWidget;
    widget->applyProfile(Profile::instance());
    return widget;
}

void DialogSettings::createConnectionsNodes()
{
    _itemConnections = createNode(0, createConnectionsWidget(), "Connections", QIcon(":/images/connect.png"));

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

QWidget *DialogSettings::createFontsWidget()
{
    FontsSettingsWidget *widget = new FontsSettingsWidget;
    widget->applyProfile(Profile::instance());
    return widget;
}

QWidget *DialogSettings::createDetailedFontsWidget()
{
    DetailedFontsSettingsWidget *widget = new DetailedFontsSettingsWidget;
    widget->applyProfile(Profile::instance());
    return widget;
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
    tabWidgetLogs->addTab(widgetMisc, "Misc");
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

QWidget *DialogSettings::createWarningoWidget()
{
    WarningoSettingsWidget *widget = new WarningoSettingsWidget;
    widget->applyProfile(Profile::instance());
    return widget;
}

QWidget *DialogSettings::createSoundsWidget()
{
    SoundSettingsWidget *widget = new SoundSettingsWidget;
    widget->applyProfile(Profile::instance());
    return widget;
}

QWidget *DialogSettings::createIdleWidget()
{
    IdleSettingsWidget *widget = new IdleSettingsWidget;
    widget->applyProfile(Profile::instance());
    return widget;
}

QWidget *DialogSettings::createTabsWidget()
{
    QWidget *mainWidget = new QWidget;
    QVBoxLayout *mainLayout = new QVBoxLayout(mainWidget);
    mainLayout->setMargin(2);

    _checkBoxTabsIcons = new QCheckBox(tr("Display icons in tabs"));
    mainLayout->addWidget(_checkBoxTabsIcons);
    _checkBoxTabsIcons->setChecked(Profile::instance().tabsIcons);

    QGroupBox *groupBoxType = new QGroupBox(tr("Tabs disposition"));
    QVBoxLayout *typeLayout = new QVBoxLayout(groupBoxType);
    mainLayout->addWidget(groupBoxType);

    _radioButtonTabsAllInOne = new QRadioButton(tr("All tabs in one row"));
    connect(_radioButtonTabsAllInOne, SIGNAL(clicked(bool)), this,
            SLOT(refreshTabExample(bool)));
    typeLayout->addWidget(_radioButtonTabsAllInOne);

    _radioButtonTabsSuper = new QRadioButton(tr("Two tabs ranges"));
    connect(_radioButtonTabsSuper, SIGNAL(clicked(bool)), this,
            SLOT(refreshTabExample(bool)));
    typeLayout->addWidget(_radioButtonTabsSuper);

    // Stack widget
    _stackedWidgetTabs = new QStackedWidget;
    mainLayout->addWidget(_stackedWidgetTabs);
    QSizePolicy sizePolicy = _stackedWidgetTabs->sizePolicy();
    sizePolicy.setVerticalPolicy(QSizePolicy::Fixed);
    _stackedWidgetTabs->setSizePolicy(sizePolicy);

    QGroupBox *groupBoxAllInOne = new QGroupBox(tr("Tabs location"));
    QVBoxLayout *allInOneLocationLayout = new QVBoxLayout(groupBoxAllInOne);
    _radioButtonTabsAllInTop = new QRadioButton(tr("On the top"));
    connect(_radioButtonTabsAllInTop, SIGNAL(clicked(bool)), this,
            SLOT(refreshTabExample(bool)));
    allInOneLocationLayout->addWidget(_radioButtonTabsAllInTop);

    _radioButtonTabsAllInBottom = new QRadioButton(tr("On the bottom"));
    connect(_radioButtonTabsAllInBottom, SIGNAL(clicked(bool)), this,
            SLOT(refreshTabExample(bool)));
    allInOneLocationLayout->addWidget(_radioButtonTabsAllInBottom);

    if (Profile::instance().tabsAllInTop)
        _radioButtonTabsAllInTop->setChecked(true);
    else
        _radioButtonTabsAllInBottom->setChecked(true);

    _stackedWidgetTabs->addWidget(groupBoxAllInOne);

    /////////////////

    QWidget *superWidget = new QWidget;

    QHBoxLayout *superLayout = new QHBoxLayout(superWidget);
    superLayout->setMargin(0);

    QGroupBox *groupBoxSuper = new QGroupBox(tr("Servers tabs location"));
    superLayout->addWidget(groupBoxSuper);
    QVBoxLayout *superLocationLayout = new QVBoxLayout(groupBoxSuper);
    _radioButtonTabsSuperOnTop = new QRadioButton(tr("On the top"));
    connect(_radioButtonTabsSuperOnTop, SIGNAL(clicked(bool)), this,
            SLOT(refreshTabExample(bool)));
    superLocationLayout->addWidget(_radioButtonTabsSuperOnTop);
    _radioButtonTabsSuperOnBottom = new QRadioButton(tr("On the bottom"));
    connect(_radioButtonTabsSuperOnBottom, SIGNAL(clicked(bool)), this,
            SLOT(refreshTabExample(bool)));
    superLocationLayout->addWidget(_radioButtonTabsSuperOnBottom);
    if (Profile::instance().tabsSuperOnTop)
        _radioButtonTabsSuperOnTop->setChecked(true);
    else
        _radioButtonTabsSuperOnBottom->setChecked(true);

    QGroupBox *groupBoxNormal = new QGroupBox(tr("Sub-tabs location"));
    superLayout->addWidget(groupBoxNormal);
    QVBoxLayout *normalLocationLayout = new QVBoxLayout(groupBoxNormal);
    _radioButtonTabsOnTop = new QRadioButton(tr("On the top"));
    connect(_radioButtonTabsOnTop, SIGNAL(clicked(bool)), this,
            SLOT(refreshTabExample(bool)));
    normalLocationLayout->addWidget(_radioButtonTabsOnTop);
    _radioButtonTabsOnBottom = new QRadioButton(tr("On the bottom"));
    connect(_radioButtonTabsOnBottom, SIGNAL(clicked(bool)), this,
            SLOT(refreshTabExample(bool)));
    normalLocationLayout->addWidget(_radioButtonTabsOnBottom);
    if (Profile::instance().tabsOnTop)
        _radioButtonTabsOnTop->setChecked(true);
    else
        _radioButtonTabsOnBottom->setChecked(true);

    _stackedWidgetTabs->addWidget(superWidget);

    connect(_radioButtonTabsAllInOne, SIGNAL(toggled(bool)), this, SLOT(tabsTypeAllInOneToggled(bool)));
    connect(_radioButtonTabsSuper, SIGNAL(toggled(bool)), this, SLOT(tabsTypeSuperToggled(bool)));

    if (Profile::instance().tabsAllInOne)
        _radioButtonTabsAllInOne->setChecked(true);
    else
        _radioButtonTabsSuper->setChecked(true);

    _mtwExample = new MultiTabWidget;
    mainLayout->addWidget(_mtwExample);


    QLabel *label = new QLabel("Hall");
    label->setAlignment(Qt::AlignCenter);
    _mtwExample->addWidget("Server 1", label, "Hall");

    label = new QLabel("Foo");
    label->setAlignment(Qt::AlignCenter);
    _mtwExample->addWidget("Server 1", label, "Foo");

    label = new QLabel("Hall");
    label->setAlignment(Qt::AlignCenter);
    _mtwExample->addWidget("Server 2", label, "Hall");

    label = new QLabel("Bar");
    label->setAlignment(Qt::AlignCenter);
    _mtwExample->addWidget("Server 2", label, "Bar");
    refreshTabExample();

    return mainWidget;
}

QWidget *DialogSettings::createLinksWidget()
{
    LinksSettingsWidget *widget = new LinksSettingsWidget;
    widget->applyProfile(Profile::instance());
    return widget;
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

QWidget *DialogSettings::createMiscWidget()
{
    QWidget *mainWidget = new QWidget;
    QVBoxLayout *mainLayout = new QVBoxLayout(mainWidget);
    mainLayout->setMargin(2);

    QGroupBox *groupBox = new QGroupBox(tr("Timestamp policy"));
    QVBoxLayout *timeStampLayout = new QVBoxLayout(groupBox);
    mainLayout->addWidget(groupBox);

    _radioButtonTimeStampClassic = new QRadioButton(tr("Classic display (server choice)"));
    _radioButtonTimeStampAlways = new QRadioButton(tr("Always display timestamp"));
    _radioButtonTimeStampNever = new QRadioButton(tr("Never display timestamp"));
    _checkBoxTimeStampInTell = new QCheckBox(tr("Display timestamp in tell tabs"));
    timeStampLayout->addWidget(_radioButtonTimeStampClassic);
    timeStampLayout->addWidget(_radioButtonTimeStampAlways);
    timeStampLayout->addWidget(_radioButtonTimeStampNever);
    timeStampLayout->addWidget(_checkBoxTimeStampInTell);

    switch(Profile::instance().timeStampPolicy)
    {
    case Profile::Policy_Classic:
        _radioButtonTimeStampClassic->setChecked(true);
        break;
    case Profile::Policy_Always:
        _radioButtonTimeStampAlways->setChecked(true);
        break;
    case Profile::Policy_Never:
        _radioButtonTimeStampNever->setChecked(true);
        break;
    default:;
    }
    _checkBoxTimeStampInTell->setChecked(Profile::instance().timeStampInTellTabs);

    _checkBoxKeepAlive = new QCheckBox(tr("Keep Alive (the \"date\" command is sent frequently)"));
    mainLayout->addWidget(_checkBoxKeepAlive);
    connect(_checkBoxKeepAlive, SIGNAL(stateChanged(int)),
            this, SLOT(keepAliveChecked(int)));

    QHBoxLayout *keepAliveLayout = new QHBoxLayout;
    mainLayout->addLayout(keepAliveLayout);

    QSpacerItem *spacer = new QSpacerItem(20, 0, QSizePolicy::Fixed,
                                          QSizePolicy::Fixed);
    keepAliveLayout->addItem(spacer);
    _spinBoxKeepAlive = new QSpinBox;
    _spinBoxKeepAlive->setFixedWidth(100);
    _spinBoxKeepAlive->setRange(1, 1000);
    keepAliveLayout->addWidget(_spinBoxKeepAlive);
    _labelKeepAliveSeconds = new QLabel("sec");
    keepAliveLayout->addWidget(_labelKeepAliveSeconds);

    _checkBoxKeepAlive->setChecked(Profile::instance().keepAlive);
    if (_checkBoxKeepAlive->isChecked())
        _spinBoxKeepAlive->setValue(Profile::instance().keepAlive);
    else
        _spinBoxKeepAlive->setValue(60);

    // Tabs for...
    groupBox = new QGroupBox("Tabs for");
    mainLayout->addWidget(groupBox);
    _checkBoxTabForWho = new QCheckBox(tr("who command"));
    _checkBoxTabForWall = new QCheckBox(tr("wall command"));
    _checkBoxTabForFinger = new QCheckBox(tr("finger command"));

    QVBoxLayout *tabsForLayout = new QVBoxLayout(groupBox);
    tabsForLayout->addWidget(_checkBoxTabForWho);
    tabsForLayout->addWidget(_checkBoxTabForWall);
    tabsForLayout->addWidget(_checkBoxTabForFinger);

    _checkBoxTabForWho->setChecked(Profile::instance().tabForWho);
    _checkBoxTabForWall->setChecked(Profile::instance().tabForWall);
    _checkBoxTabForFinger->setChecked(Profile::instance().tabForFinger);

    _checkBoxCopyOnSelection = new QCheckBox(tr("Copy on selection (unix style)"));
    mainLayout->addWidget(_checkBoxCopyOnSelection);
    _checkBoxCopyOnSelection->setChecked(Profile::instance().copyOnSelection);

    // End spacer
    spacer = new QSpacerItem(0, 0, QSizePolicy::Fixed, QSizePolicy::Expanding);
    mainLayout->addItem(spacer);

    return mainWidget;
}

QWidget *DialogSettings::createConnectionsWidget()
{
    QWidget *mainWidget = new QWidget;
    QVBoxLayout *mainLayout = new QVBoxLayout(mainWidget);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch();
    _pushButtonNewConnection = new QPushButton("New connection");
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
    getGeneralControlsDatas();
    getShortcutsControlsDatas();
    getConnectionsControlsDatas();
    getLogsControlsDatas();
    getFontsControlsDatas();
    getDetailedFontsControlsDatas();
    getTrayControlsDatas();
    getWarningoControlsDatas();
    getSoundsControlsDatas();
    getIdleControlsDatas();
    getTabsControlsDatas();
    getLinksControlsDatas();
    getOutputControlsDatas();
    getMiscControlsDatas();
}

void DialogSettings::getGeneralControlsDatas()
{
    QString oldLanguage = Profile::instance().language;
    if (_comboBoxLanguage->currentIndex() == 0)
        Profile::instance().language = "";
    else
        Profile::instance().language = displayToLanguage[_comboBoxLanguage->currentText()];

    if (Profile::instance().language != oldLanguage)
        QMessageBox::warning(this, tr("Warning"), tr("You must restart CeB to apply your language changes"));
    Profile::instance().checkForUpdate = _checkBoxCheckForUpdate->isChecked();
}

void DialogSettings::getShortcutsControlsDatas()
{
    qobject_cast<ShortcutsSettingsWidget*>(_shortcutsWidget)->feedProfile(Profile::instance());
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

void DialogSettings::getFontsControlsDatas()
{
    qobject_cast<FontsSettingsWidget*>(_fontsWidget)->feedProfile(Profile::instance());
}

void DialogSettings::getDetailedFontsControlsDatas()
{
    qobject_cast<DetailedFontsSettingsWidget*>(_detailedFontsWidget)->feedProfile(Profile::instance());
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

void DialogSettings::getWarningoControlsDatas()
{
    qobject_cast<WarningoSettingsWidget*>(_warningoWidget)->feedProfile(Profile::instance());
}

void DialogSettings::getSoundsControlsDatas()
{
    qobject_cast<SoundSettingsWidget*>(_soundsWidget)->feedProfile(Profile::instance());
}

void DialogSettings::getIdleControlsDatas()
{
    qobject_cast<IdleSettingsWidget*>(_idleWidget)->feedProfile(Profile::instance());
}

void DialogSettings::getTabsControlsDatas()
{
    Profile::instance().tabsIcons = _checkBoxTabsIcons->isChecked();
    Profile::instance().tabsAllInOne = _radioButtonTabsAllInOne->isChecked();
    Profile::instance().tabsAllInTop = _radioButtonTabsAllInTop->isChecked();
    Profile::instance().tabsSuperOnTop = _radioButtonTabsSuperOnTop->isChecked();
    Profile::instance().tabsOnTop = _radioButtonTabsOnTop->isChecked();
}

void DialogSettings::getLinksControlsDatas()
{
    qobject_cast<LinksSettingsWidget*>(_linksWidget)->feedProfile(Profile::instance());
}

void DialogSettings::getOutputControlsDatas()
{
    Profile::instance().awaySeparatorLines = _groupBoxAwaySeparator->isChecked();
    QPalette palette = _lineEditAwaySeparatorColor->palette();
    Profile::instance().awaySeparatorColor = palette.color(QPalette::Text);
    Profile::instance().awaySeparatorLength = _spinBoxAwaySeparatorLength->value();
    Profile::instance().awaySeparatorPeriod = _lineEditAwaySeparatorPeriod->text();
}

void DialogSettings::getMiscControlsDatas()
{
    // Time stamp
    if (_radioButtonTimeStampClassic->isChecked())
        Profile::instance().timeStampPolicy = Profile::Policy_Classic;
    else if (_radioButtonTimeStampAlways->isChecked())
        Profile::instance().timeStampPolicy = Profile::Policy_Always;
    else if (_radioButtonTimeStampNever->isChecked())
        Profile::instance().timeStampPolicy = Profile::Policy_Never;

    Profile::instance().timeStampInTellTabs = _checkBoxTimeStampInTell->isChecked();

    // Keep alive
    if (_checkBoxKeepAlive->isChecked())
        Profile::instance().keepAlive = _spinBoxKeepAlive->value();
    else
        Profile::instance().keepAlive = 0;

    // Tabs for...
    Profile::instance().tabForWho = _checkBoxTabForWho->isChecked();
    Profile::instance().tabForWall = _checkBoxTabForWall->isChecked();
    Profile::instance().tabForFinger = _checkBoxTabForFinger->isChecked();

    Profile::instance().copyOnSelection = _checkBoxCopyOnSelection->isChecked();
}

void DialogSettings::newSessionConfig()
{
    // Widget
    SessionConfigWidget *sessionConfigWidget = new SessionConfigWidget(0, true);

    SessionConfig newConfig = SessionConfig::getTemplate();
    newConfig.setName(Profile::instance().getUniqSessionConfigName());
    Profile::instance().addSessionConfig(newConfig);
    sessionConfigWidget->init(newConfig);

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

void DialogSettings::keepAliveChecked(int state)
{
    _spinBoxKeepAlive->setEnabled(state == Qt::Checked);
    _labelKeepAliveSeconds->setEnabled(state == Qt::Checked);
}

void DialogSettings::logsCustomDirClicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Choose a logs directory"));
    _lineEditLogsCustomDir->setText(dir);
}

void DialogSettings::tabsTypeAllInOneToggled(bool checked)
{
    if (checked)
        _stackedWidgetTabs->setCurrentIndex(0);
}

void DialogSettings::tabsTypeSuperToggled(bool checked)
{
    if (checked)
        _stackedWidgetTabs->setCurrentIndex(1);
}

void DialogSettings::refreshTabExample(bool)
{
    if (_radioButtonTabsAllInOne->isChecked())
    {
        _mtwExample->setDisplayMode(MultiTabWidget::DisplayMode_AllInOneRow);

        if (_radioButtonTabsAllInTop->isChecked())
            _mtwExample->setAllInOneRowLocation(MultiTabWidget::TabLocation_North);
        else
            _mtwExample->setAllInOneRowLocation(MultiTabWidget::TabLocation_South);
    }
    else
    {
        _mtwExample->setDisplayMode(MultiTabWidget::DisplayMode_Hierarchical);
        if (_radioButtonTabsSuperOnTop->isChecked())
            _mtwExample->setSuperLocation(MultiTabWidget::TabLocation_North);
        else
            _mtwExample->setSuperLocation(MultiTabWidget::TabLocation_South);

        if (_radioButtonTabsOnTop->isChecked())
            _mtwExample->setSubLocation(MultiTabWidget::TabLocation_North);
        else
            _mtwExample->setSubLocation(MultiTabWidget::TabLocation_South);
    }

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
