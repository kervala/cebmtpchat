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

#include "dialog_settings.h"
#include "profile_manager.h"
#include "session_config_widget.h"
#include "language_manager.h"
#include "sound_settings_widget.h"
#include "idle_settings_widget.h"
#include "warningo_settings_widget.h"
#include "fonts_settings_widget.h"
#include "detailed_fonts_settings_widget.h"
#include "links_settings_widget.h"

DialogSettings::DialogSettings(QWidget *parent): DialogConfig(parent)
{
	treeMain->setColumnCount(1);
	treeMain->setHeaderLabels(QStringList(tr("Categories")));
	treeMain->header()->setMovable(false);
	treeMain->header()->setResizeMode(QHeaderView::Stretch);
	createNode(0, createGeneralWidget(), tr("General", "Settings node"));
	createConnectionsNodes();
	QTreeWidgetItem *item = createNode(0, fontsWidget = createFontsWidget(), tr("Fonts", "Settings node"));
	createNode(item, detailedFontsWidget = createDetailedFontsWidget(), tr("Detailed", "Settings node"));
	createNode(0, createLogsWidget(), tr("Logs", "Settings node"));
	createNode(0, createTrayWidget(), tr("Tray", "Settings node"));
	createNode(0, warningoWidget = createWarningoWidget(), tr("Warningo", "Settings node"));
	createNode(0, soundsWidget = createSoundsWidget(), tr("Sounds", "Settings node"), QIcon(":/images/sounds.png"));
	createNode(0, idleWidget = createIdleWidget(), tr("Idle", "Settings node"));
	createNode(0, createTabsWidget(), tr("Tabs", "Settings node"));
	createNode(0, linksWidget = createLinksWidget(), tr("Links", "Settings node"));
	createNode(0, createOutputWidget(), tr("Output", "Settings node"));
	createNode(0, createMiscWidget(), tr("Misc", "Settings node"));

	// Save old things
	Profile &profile = *ProfileManager::instance().currentProfile();
	m_oldTextSkin = profile.textSkin();

	// Focus on first node
	treeMain->setCurrentItem(treeMain->topLevelItem(0));

	resize(600, 400);
}

QWidget *DialogSettings::createGeneralWidget()
{
	Profile &profile = *ProfileManager::instance().currentProfile();
	QWidget *mainWidget = new QWidget;
	QVBoxLayout *mainLayout = new QVBoxLayout(mainWidget);

	QLabel *label = new QLabel(tr("Application language:"));
	mainLayout->addWidget(label);

	comboBoxLanguage = new QComboBox;
	mainLayout->addWidget(comboBoxLanguage);

	// End spacer
	QSpacerItem *spacer = new QSpacerItem(0, 0, QSizePolicy::Fixed, QSizePolicy::Expanding);
	mainLayout->addItem(spacer);

	////// INIT
	QStringList languages = LanguageManager::getAvailableLanguages();
	comboBoxLanguage->addItem(tr("<Default language>"));
	foreach (QString language, languages)
	{
		comboBoxLanguage->addItem(getLanguageDisplay(language));
		displayToLanguage.insert(getLanguageDisplay(language), language);
	}

	if (!profile.language.isEmpty())
	{
		int index = comboBoxLanguage->findText(getLanguageDisplay(profile.language));
		if (index >= 0)
			comboBoxLanguage->setCurrentIndex(index);
	}

	return  mainWidget;
}

void DialogSettings::createConnectionsNodes()
{
	Profile &profile = *ProfileManager::instance().currentProfile();

	itemConnections = createNode(0, createConnectionsWidget(), "Connections");

	for (int i = 0; i < profile.sessionConfigs().count(); i++)
	{
		const SessionConfig &config = *profile.sessionConfigs().value(i);

		// Create a widget for the connection
		SessionConfigWidget *sessionConfigWidget = new SessionConfigWidget(0, true);
		connect(sessionConfigWidget, SIGNAL(removeMe()),
			this, SLOT(removeSessionConfig()));
		sessionConfigWidget->init(config);
		
		// Create a node for the config
		createNode(itemConnections, sessionConfigWidget, config.name());
	}
}

QWidget *DialogSettings::createFontsWidget()
{
	FontsSettingsWidget *widget = new FontsSettingsWidget;
	widget->applyProfile(*ProfileManager::instance().currentProfile());
	return widget;
}

QWidget *DialogSettings::createDetailedFontsWidget()
{
	DetailedFontsSettingsWidget *widget = new DetailedFontsSettingsWidget;
	widget->applyProfile(*ProfileManager::instance().currentProfile());
	return widget;
}

QWidget *DialogSettings::createLogsWidget()
{
	Profile &profile = *ProfileManager::instance().currentProfile();
	QWidget *mainWidget = new QWidget;
	QVBoxLayout *mainLayout = new QVBoxLayout(mainWidget);
	mainLayout->setMargin(2);
	
	groupBoxLogs = new QGroupBox(tr("Enable logs"));
	mainLayout->addWidget(groupBoxLogs);
	groupBoxLogs->setCheckable(true);

	QVBoxLayout *logsLayout = new QVBoxLayout(groupBoxLogs);

	QTabWidget *tabWidgetLogs = new QTabWidget;
	logsLayout->addWidget(tabWidgetLogs);

	QWidget *widgetDir = new QWidget;
	tabWidgetLogs->addTab(widgetDir, tr("Logs directory"));
	QVBoxLayout *logsDirLayout = new QVBoxLayout(widgetDir);

	// Default directory
	radioButtonLogsDefaultDir = new QRadioButton(tr("Default directory: "));
	logsDirLayout->addWidget(radioButtonLogsDefaultDir);
	QLineEdit *lineEditDefaultDir = new QLineEdit;
	lineEditDefaultDir->setReadOnly(true);
	QString defaultDir = QDir::convertSeparators(QApplication::applicationDirPath() + "/logs");
	lineEditDefaultDir->setText(defaultDir);

	QHBoxLayout *hLayout = new QHBoxLayout;
	logsDirLayout->addLayout(hLayout);
	QSpacerItem *spacer = new QSpacerItem(20, 0, QSizePolicy::Fixed,
		QSizePolicy::Fixed);
	hLayout->addItem(spacer);
	hLayout->addWidget(lineEditDefaultDir);
	connect(radioButtonLogsDefaultDir, SIGNAL(toggled(bool)),
			lineEditDefaultDir, SLOT(setEnabled(bool)));

	// Custom directory
	radioButtonLogsCustomDir = new QRadioButton(tr("Custom directory: "));
	logsDirLayout->addWidget(radioButtonLogsCustomDir);
	lineEditLogsCustomDir = new QLineEdit;
	lineEditLogsCustomDir->setText(profile.logsDir);
	hLayout = new QHBoxLayout;
	hLayout->setSpacing(0);
	logsDirLayout->addLayout(hLayout);
	spacer = new QSpacerItem(20, 0, QSizePolicy::Fixed,
							QSizePolicy::Fixed);
	hLayout->addItem(spacer);
	hLayout->addWidget(lineEditLogsCustomDir);
	QPushButton *fileButton = new QPushButton("...");
	fileButton->setFixedWidth(25);
	hLayout->addWidget(fileButton);
	connect(fileButton, SIGNAL(clicked()), this, SLOT(logsCustomDirClicked()));
	connect(radioButtonLogsCustomDir, SIGNAL(toggled(bool)),
			lineEditLogsCustomDir, SLOT(setEnabled(bool)));
	connect(radioButtonLogsCustomDir, SIGNAL(toggled(bool)),
			fileButton, SLOT(setEnabled(bool)));

	if (profile.logsDefaultDir)
		radioButtonLogsDefaultDir->setChecked(true);
	else
		radioButtonLogsCustomDir->setChecked(true);
	lineEditDefaultDir->setEnabled(profile.logsDefaultDir);
	lineEditLogsCustomDir->setEnabled(!profile.logsDefaultDir);
	fileButton->setEnabled(!profile.logsDefaultDir);

	// Logs file policy
	QWidget *widgetFile = new QWidget;
	tabWidgetLogs->addTab(widgetFile, tr("Logs file policy"));
	QVBoxLayout *logsFileLayout = new QVBoxLayout(widgetFile);
	QLabel *label = new QLabel(tr("Save your logs in a file:"));
	logsFileLayout->addWidget(label);
	radioButtonLogsDaily = new QRadioButton(tr("Daily"));
	logsFileLayout->addWidget(radioButtonLogsDaily);
	radioButtonLogsWeekly = new QRadioButton(tr("Weekly (from monday to sunday)"));
	logsFileLayout->addWidget(radioButtonLogsWeekly);
	radioButtonLogsMonthly = new QRadioButton(tr("Monthly"));
	logsFileLayout->addWidget(radioButtonLogsMonthly);
	radioButtonLogsUniq = new QRadioButton(tr("Uniq"));
	logsFileLayout->addWidget(radioButtonLogsUniq);	

	switch (profile.logsFilePolicy)
	{
	case Profile::LogFilePolicy_Daily:
		radioButtonLogsDaily->setChecked(true);
		break;
	case Profile::LogFilePolicy_Weekly:
		radioButtonLogsWeekly->setChecked(true);
		break;
	case Profile::LogFilePolicy_Monthly:
		radioButtonLogsMonthly->setChecked(true);
		break;
	case Profile::LogFilePolicy_Uniq:
		radioButtonLogsUniq->setChecked(true);
		break;
	}

	groupBoxLogs->setChecked(profile.logsEnabled);

	// Misc
	QWidget *widgetMisc = new QWidget;
	tabWidgetLogs->addTab(widgetMisc, "Misc");
	checkBoxLogsTimeStamp = new QCheckBox(tr("Alway timestamps"));
	QVBoxLayout *miscLayout = new QVBoxLayout(widgetMisc);
	miscLayout->addWidget(checkBoxLogsTimeStamp);
	checkBoxLogsTimeStamp->setChecked(profile.logsTimeStamp);
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
	Profile &profile = *ProfileManager::instance().currentProfile();

	QWidget *mainWidget = new QWidget;
	QVBoxLayout *mainLayout = new QVBoxLayout(mainWidget);
	mainLayout->setMargin(2);

	groupBoxTray = new QGroupBox(tr("Enable tray icon"));
	mainLayout->addWidget(groupBoxTray);
	groupBoxTray->setCheckable(true);

	QVBoxLayout *trayLayout = new QVBoxLayout(groupBoxTray);

	checkBoxTrayAlwaysVisible = new QCheckBox(tr("Always visible"));
	trayLayout->addWidget(checkBoxTrayAlwaysVisible);
	checkBoxTrayAlwaysVisible->setChecked(profile.trayAlwaysVisible);

	checkBoxTrayHideFromTaskBar = new QCheckBox(tr("Hide from task bar when minimized"));
	trayLayout->addWidget(checkBoxTrayHideFromTaskBar);
	checkBoxTrayHideFromTaskBar->setChecked(profile.trayHideFromTaskBar);

	groupBoxTray->setChecked(profile.trayEnabled);

	// End spacer
	QSpacerItem *spacer = new QSpacerItem(0, 0, QSizePolicy::Fixed,
						QSizePolicy::Expanding);
	mainLayout->addItem(spacer);

	return mainWidget;
}

QWidget *DialogSettings::createWarningoWidget()
{
	WarningoSettingsWidget *widget = new WarningoSettingsWidget;
	widget->applyProfile(*ProfileManager::instance().currentProfile());
	return widget;
}

QWidget *DialogSettings::createSoundsWidget()
{
	SoundSettingsWidget *widget = new SoundSettingsWidget;
	widget->applyProfile(*ProfileManager::instance().currentProfile());
	return widget;
}

QWidget *DialogSettings::createIdleWidget()
{
	IdleSettingsWidget *widget = new IdleSettingsWidget;
	widget->applyProfile(*ProfileManager::instance().currentProfile());
	return widget;
}

QWidget *DialogSettings::createTabsWidget()
{
	Profile &profile = *ProfileManager::instance().currentProfile();

	QWidget *mainWidget = new QWidget;
	QVBoxLayout *mainLayout = new QVBoxLayout(mainWidget);
	mainLayout->setMargin(2);

	checkBoxTabsIcons = new QCheckBox(tr("Display icons in tabs"));
	mainLayout->addWidget(checkBoxTabsIcons);
	checkBoxTabsIcons->setChecked(profile.tabsIcons);

	QGroupBox *groupBoxType = new QGroupBox(tr("Tabs disposition"));
	QVBoxLayout *typeLayout = new QVBoxLayout(groupBoxType);
	mainLayout->addWidget(groupBoxType);

	radioButtonTabsAllInOne = new QRadioButton(tr("All tabs in one row"));
	connect(radioButtonTabsAllInOne, SIGNAL(clicked(bool)), this, 
		SLOT(refreshTabExample(bool)));
	typeLayout->addWidget(radioButtonTabsAllInOne);

	radioButtonTabsSuper = new QRadioButton(tr("Two tabs ranges"));
	connect(radioButtonTabsSuper, SIGNAL(clicked(bool)), this, 
		SLOT(refreshTabExample(bool)));
	typeLayout->addWidget(radioButtonTabsSuper);

	// Stack widget
	stackedWidgetTabs = new QStackedWidget;
	mainLayout->addWidget(stackedWidgetTabs);
	QSizePolicy sizePolicy = stackedWidgetTabs->sizePolicy();
	sizePolicy.setVerticalPolicy(QSizePolicy::Fixed);
	stackedWidgetTabs->setSizePolicy(sizePolicy);

	QGroupBox *groupBoxAllInOne = new QGroupBox(tr("Tabs location"));
	QVBoxLayout *allInOneLocationLayout = new QVBoxLayout(groupBoxAllInOne);
	radioButtonTabsAllInTop = new QRadioButton(tr("On the top"));
	connect(radioButtonTabsAllInTop, SIGNAL(clicked(bool)), this, 
		SLOT(refreshTabExample(bool)));
	allInOneLocationLayout->addWidget(radioButtonTabsAllInTop);	

	radioButtonTabsAllInBottom = new QRadioButton(tr("On the bottom"));
	connect(radioButtonTabsAllInBottom, SIGNAL(clicked(bool)), this, 
		SLOT(refreshTabExample(bool)));
	allInOneLocationLayout->addWidget(radioButtonTabsAllInBottom);

	if (profile.tabsAllInTop)
		radioButtonTabsAllInTop->setChecked(true);
	else
		radioButtonTabsAllInBottom->setChecked(true); 

	stackedWidgetTabs->addWidget(groupBoxAllInOne);

	/////////////////

	QWidget *superWidget = new QWidget;

	QHBoxLayout *superLayout = new QHBoxLayout(superWidget);
	superLayout->setMargin(0);

	QGroupBox *groupBoxSuper = new QGroupBox(tr("Servers tabs location"));
	superLayout->addWidget(groupBoxSuper);	
	QVBoxLayout *superLocationLayout = new QVBoxLayout(groupBoxSuper);
	radioButtonTabsSuperOnTop = new QRadioButton(tr("On the top"));
	connect(radioButtonTabsSuperOnTop, SIGNAL(clicked(bool)), this, 
		SLOT(refreshTabExample(bool)));
	superLocationLayout->addWidget(radioButtonTabsSuperOnTop);
	radioButtonTabsSuperOnBottom = new QRadioButton(tr("On the bottom"));
	connect(radioButtonTabsSuperOnBottom, SIGNAL(clicked(bool)), this, 
		SLOT(refreshTabExample(bool)));
	superLocationLayout->addWidget(radioButtonTabsSuperOnBottom);
	if (profile.tabsSuperOnTop)
		radioButtonTabsSuperOnTop->setChecked(true);
	else
		radioButtonTabsSuperOnBottom->setChecked(true);

	QGroupBox *groupBoxNormal = new QGroupBox(tr("Sub-tabs location"));
	superLayout->addWidget(groupBoxNormal);
	QVBoxLayout *normalLocationLayout = new QVBoxLayout(groupBoxNormal);
	radioButtonTabsOnTop = new QRadioButton(tr("On the top"));
	connect(radioButtonTabsOnTop, SIGNAL(clicked(bool)), this, 
		SLOT(refreshTabExample(bool)));
	normalLocationLayout->addWidget(radioButtonTabsOnTop);
	radioButtonTabsOnBottom = new QRadioButton(tr("On the bottom"));
	connect(radioButtonTabsOnBottom, SIGNAL(clicked(bool)), this, 
		SLOT(refreshTabExample(bool)));
	normalLocationLayout->addWidget(radioButtonTabsOnBottom);
	if (profile.tabsOnTop)
		radioButtonTabsOnTop->setChecked(true);
	else
		radioButtonTabsOnBottom->setChecked(true);

	stackedWidgetTabs->addWidget(superWidget);

	connect(radioButtonTabsAllInOne, SIGNAL(toggled(bool)), this, SLOT(tabsTypeAllInOneToggled(bool)));
	connect(radioButtonTabsSuper, SIGNAL(toggled(bool)), this, SLOT(tabsTypeSuperToggled(bool)));

	if (profile.tabsAllInOne)
		radioButtonTabsAllInOne->setChecked(true);
	else
		radioButtonTabsSuper->setChecked(true);	

	mtwExample = new MultiTabWidget;
	mainLayout->addWidget(mtwExample);


	QLabel *label = new QLabel("Hall");
	mtwExample->addWidget("Server 1", label, "Hall");

	label = new QLabel("Foo");
	mtwExample->addWidget("Server 1", label, "Foo");

	label = new QLabel("Hall");
	mtwExample->addWidget("Server 2", label, "Hall");

	label = new QLabel("Bar");
	mtwExample->addWidget("Server 2", label, "Bar");
	refreshTabExample();

	return mainWidget;
}

QWidget *DialogSettings::createLinksWidget()
{
	LinksSettingsWidget *widget = new LinksSettingsWidget;
	widget->applyProfile(*ProfileManager::instance().currentProfile());
	return widget;
}

QWidget *DialogSettings::createOutputWidget()
{
	Profile &profile = *ProfileManager::instance().currentProfile();

	QWidget *mainWidget = new QWidget;
	QVBoxLayout *mainLayout = new QVBoxLayout(mainWidget);
	mainLayout->setMargin(2);

	groupBoxAwaySeparator = new QGroupBox(tr("Enable away/back separator lines"));
	mainLayout->addWidget(groupBoxAwaySeparator);
	groupBoxAwaySeparator->setCheckable(true);
	groupBoxAwaySeparator->setChecked(profile.awaySeparatorLines);

	QVBoxLayout *groupBoxAwayLayout = new QVBoxLayout(groupBoxAwaySeparator);

	QHBoxLayout *colorLayout = new QHBoxLayout;
	groupBoxAwayLayout->addLayout(colorLayout);
	lineEditAwaySeparatorColor = new QLineEdit;
	lineEditAwaySeparatorColor->setReadOnly(true);
	colorLayout->addWidget(lineEditAwaySeparatorColor);
	pushButtonAwaySeparatorColor = new QPushButton(tr("Pick a color"));
	QSizePolicy policy = pushButtonAwaySeparatorColor->sizePolicy();
	policy.setHorizontalPolicy(QSizePolicy::Fixed);
	pushButtonAwaySeparatorColor->setSizePolicy(policy);
	connect(pushButtonAwaySeparatorColor, SIGNAL(clicked()),
		this, SLOT(changeAwaySeparatorColor()));
	colorLayout->addWidget(pushButtonAwaySeparatorColor);

	QPalette palette = lineEditAwaySeparatorColor->palette();
	palette.setColor(QPalette::Text, profile.awaySeparatorColor);
	palette.setColor(QPalette::Base, QColor(255, 255, 240));
	lineEditAwaySeparatorColor->setPalette(palette);

	QHBoxLayout *lengthLayout = new QHBoxLayout;
	groupBoxAwayLayout->addLayout(lengthLayout);
	QLabel *label = new QLabel(tr("Lines length: "));
	lengthLayout->addWidget(label);	
	spinBoxAwaySeparatorLength = new QSpinBox;
	policy = spinBoxAwaySeparatorLength->sizePolicy();
	policy.setHorizontalPolicy(QSizePolicy::Fixed);
	spinBoxAwaySeparatorLength->setSizePolicy(policy);
	lengthLayout->addWidget(spinBoxAwaySeparatorLength);
	spinBoxAwaySeparatorLength->setMinimum(1);
	spinBoxAwaySeparatorLength->setMaximum(200);
	spinBoxAwaySeparatorLength->setValue(profile.awaySeparatorLength);
	connect(spinBoxAwaySeparatorLength, SIGNAL(valueChanged(int)),
		this, SLOT(refreshAwaySeparatorPreview()));

	QHBoxLayout *periodLayout = new QHBoxLayout;
	groupBoxAwayLayout->addLayout(periodLayout);
	label = new QLabel(tr("Periodic separator string:"));
	periodLayout->addWidget(label);
	lineEditAwaySeparatorPeriod = new QLineEdit;
	periodLayout->addWidget(lineEditAwaySeparatorPeriod);
	lineEditAwaySeparatorPeriod->setText(profile.awaySeparatorPeriod);
	connect(lineEditAwaySeparatorPeriod, SIGNAL(textChanged(const QString &)),
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
	Profile &profile = *ProfileManager::instance().currentProfile();

	QWidget *mainWidget = new QWidget;
	QVBoxLayout *mainLayout = new QVBoxLayout(mainWidget);
	mainLayout->setMargin(2);

	QGroupBox *groupBox = new QGroupBox(tr("Timestamp policy"));
	QVBoxLayout *timeStampLayout = new QVBoxLayout(groupBox);
	mainLayout->addWidget(groupBox);

	radioButtonTimeStampClassic = new QRadioButton(tr("Classic display (server choice)"));
	radioButtonTimeStampAlways = new QRadioButton(tr("Always display timestamp"));
	radioButtonTimeStampNever = new QRadioButton(tr("Never display timestamp"));
	checkBoxTimeStampInTell = new QCheckBox(tr("Display timestamp in tell tabs"));
	timeStampLayout->addWidget(radioButtonTimeStampClassic);
	timeStampLayout->addWidget(radioButtonTimeStampAlways);
	timeStampLayout->addWidget(radioButtonTimeStampNever);
	timeStampLayout->addWidget(checkBoxTimeStampInTell);

	switch(profile.timeStampPolicy)
	{
	case Profile::Policy_Classic:
		radioButtonTimeStampClassic->setChecked(true);
		break;
	case Profile::Policy_Always:
		radioButtonTimeStampAlways->setChecked(true);
		break;
	case Profile::Policy_Never:
		radioButtonTimeStampNever->setChecked(true);
		break;
	default:;
	}
	checkBoxTimeStampInTell->setChecked(profile.timeStampInTellTabs);

	checkBoxKeepAlive = new QCheckBox(tr("Keep Alive (the \"date\" command is sent frequently)"));
	mainLayout->addWidget(checkBoxKeepAlive);
	connect(checkBoxKeepAlive, SIGNAL(stateChanged(int)),
			this, SLOT(keepAliveChecked(int)));

	QHBoxLayout *keepAliveLayout = new QHBoxLayout;
	mainLayout->addLayout(keepAliveLayout);

	QSpacerItem *spacer = new QSpacerItem(20, 0, QSizePolicy::Fixed,
		QSizePolicy::Fixed);
	keepAliveLayout->addItem(spacer);
	spinBoxKeepAlive = new QSpinBox;
	spinBoxKeepAlive->setFixedWidth(100);
	spinBoxKeepAlive->setRange(1, 1000);
	keepAliveLayout->addWidget(spinBoxKeepAlive);
	labelKeepAliveSeconds = new QLabel("sec");
	keepAliveLayout->addWidget(labelKeepAliveSeconds);

	checkBoxKeepAlive->setChecked(profile.keepAlive);
	if (checkBoxKeepAlive->isChecked())
		spinBoxKeepAlive->setValue(profile.keepAlive);
	else
		spinBoxKeepAlive->setValue(60);

	// Tabs for...
	groupBox = new QGroupBox("Tabs for");
	mainLayout->addWidget(groupBox);
	checkBoxTabForWho = new QCheckBox(tr("who command"));
	checkBoxTabForWall = new QCheckBox(tr("wall command"));
	checkBoxTabForFinger = new QCheckBox(tr("finger command"));

	QVBoxLayout *tabsForLayout = new QVBoxLayout(groupBox);
	tabsForLayout->addWidget(checkBoxTabForWho);
	tabsForLayout->addWidget(checkBoxTabForWall);
	tabsForLayout->addWidget(checkBoxTabForFinger);

	checkBoxTabForWho->setChecked(profile.tabForWho);
	checkBoxTabForWall->setChecked(profile.tabForWall);
	checkBoxTabForFinger->setChecked(profile.tabForFinger);

	checkBoxCopyOnSelection = new QCheckBox(tr("Copy on selection (unix style)"));
	mainLayout->addWidget(checkBoxCopyOnSelection);
	checkBoxCopyOnSelection->setChecked(profile.copyOnSelection);

	// End spacer
	spacer = new QSpacerItem(0, 0, QSizePolicy::Fixed, QSizePolicy::Expanding);
	mainLayout->addItem(spacer);

	return mainWidget;
}

QWidget *DialogSettings::createConnectionsWidget()
{
	QWidget *mainWidget = new QWidget;
	QVBoxLayout *mainLayout = new QVBoxLayout(mainWidget);

	pushButtonNewConnection = new QPushButton("New connection");
	mainLayout->addWidget(pushButtonNewConnection);
	connect(pushButtonNewConnection, SIGNAL(clicked()),
		this, SLOT(newSessionConfig()));

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
	for (int i = 0; i < itemConnections->childCount(); i++)
	{
		SessionConfigWidget *widget =
			qobject_cast<SessionConfigWidget*>(getWidgetByNode(itemConnections->child(i)));

		if (!widget->check())
		{
			// Focus on widget
			treeMain->setCurrentItem(itemConnections->child(i));
			return false;
		}
	}

	return true;
}

void DialogSettings::getControlsDatas()
{
	getGeneralControlsDatas();
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
	Profile &profile = *ProfileManager::instance().currentProfile();
	if (comboBoxLanguage->currentIndex() == 0)
		profile.language = "";
	else
		profile.language = displayToLanguage[comboBoxLanguage->currentText()];
}

void DialogSettings::getConnectionsControlsDatas()
{
	Profile &profile = *ProfileManager::instance().currentProfile();
	for (int i = 0; i < itemConnections->childCount(); i++)
	{
		SessionConfigWidget *widget =
			qobject_cast<SessionConfigWidget*>(getWidgetByNode(itemConnections->child(i)));

		// Refresh config in profile
		SessionConfig *config = profile.getSessionConfig(widget->oldName());
		widget->get(*config);
	}
}

void DialogSettings::getFontsControlsDatas()
{
	qobject_cast<FontsSettingsWidget*>(fontsWidget)->feedProfile(
		*ProfileManager::instance().currentProfile());
}

void DialogSettings::getDetailedFontsControlsDatas()
{
	qobject_cast<DetailedFontsSettingsWidget*>(detailedFontsWidget)->feedProfile(
		*ProfileManager::instance().currentProfile());
}

void DialogSettings::getLogsControlsDatas()
{
	Profile &profile = *ProfileManager::instance().currentProfile();

	profile.logsEnabled = groupBoxLogs->isChecked();
	profile.logsDefaultDir = radioButtonLogsDefaultDir->isChecked();
	profile.logsDir = lineEditLogsCustomDir->text();
	if (radioButtonLogsDaily->isChecked())
		profile.logsFilePolicy = Profile::LogFilePolicy_Daily;
	else if (radioButtonLogsWeekly->isChecked())
		profile.logsFilePolicy = Profile::LogFilePolicy_Weekly;
	else if (radioButtonLogsMonthly->isChecked())
		profile.logsFilePolicy = Profile::LogFilePolicy_Monthly;
	else if (radioButtonLogsUniq->isChecked())
		profile.logsFilePolicy = Profile::LogFilePolicy_Uniq;
	profile.logsTimeStamp = checkBoxLogsTimeStamp->isChecked();
}

void DialogSettings::getTrayControlsDatas()
{
	Profile &profile = *ProfileManager::instance().currentProfile();

	profile.trayEnabled = groupBoxTray->isChecked();
	profile.trayAlwaysVisible = checkBoxTrayAlwaysVisible->isChecked();
	profile.trayHideFromTaskBar = checkBoxTrayHideFromTaskBar->isChecked();
}

void DialogSettings::getWarningoControlsDatas()
{
	qobject_cast<WarningoSettingsWidget*>(warningoWidget)->feedProfile(
		*ProfileManager::instance().currentProfile());
}

void DialogSettings::getSoundsControlsDatas()
{
	qobject_cast<SoundSettingsWidget*>(soundsWidget)->feedProfile(
		*ProfileManager::instance().currentProfile());
}

void DialogSettings::getIdleControlsDatas()
{
	qobject_cast<IdleSettingsWidget*>(idleWidget)->feedProfile(
		*ProfileManager::instance().currentProfile());
}

void DialogSettings::getTabsControlsDatas()
{
	Profile &profile = *ProfileManager::instance().currentProfile();

	profile.tabsIcons = checkBoxTabsIcons->isChecked();
	profile.tabsAllInOne = radioButtonTabsAllInOne->isChecked();
	profile.tabsAllInTop = radioButtonTabsAllInTop->isChecked();
	profile.tabsSuperOnTop = radioButtonTabsSuperOnTop->isChecked();
	profile.tabsOnTop = radioButtonTabsOnTop->isChecked();
}

void DialogSettings::getLinksControlsDatas()
{
	qobject_cast<LinksSettingsWidget*>(linksWidget)->feedProfile(
		*ProfileManager::instance().currentProfile());
}

void DialogSettings::getOutputControlsDatas()
{
	Profile &profile = *ProfileManager::instance().currentProfile();

	profile.awaySeparatorLines = groupBoxAwaySeparator->isChecked();
	QPalette palette = lineEditAwaySeparatorColor->palette();
	profile.awaySeparatorColor = palette.color(QPalette::Text);
	profile.awaySeparatorLength = spinBoxAwaySeparatorLength->value();
	profile.awaySeparatorPeriod = lineEditAwaySeparatorPeriod->text();
}

void DialogSettings::getMiscControlsDatas()
{
	Profile &profile = *ProfileManager::instance().currentProfile();

	// Time stamp
	if (radioButtonTimeStampClassic->isChecked())
		profile.timeStampPolicy = Profile::Policy_Classic;
	else if (radioButtonTimeStampAlways->isChecked())
		profile.timeStampPolicy = Profile::Policy_Always;
	else if (radioButtonTimeStampNever->isChecked())
		profile.timeStampPolicy = Profile::Policy_Never;

	profile.timeStampInTellTabs = checkBoxTimeStampInTell->isChecked();

	// Keep alive
	if (checkBoxKeepAlive->isChecked())
		profile.keepAlive = spinBoxKeepAlive->value();
	else
		profile.keepAlive = 0;

	// Tabs for...
	profile.tabForWho = checkBoxTabForWho->isChecked();
	profile.tabForWall = checkBoxTabForWall->isChecked();
	profile.tabForFinger = checkBoxTabForFinger->isChecked();

	profile.copyOnSelection = checkBoxCopyOnSelection->isChecked();
}

void DialogSettings::newSessionConfig()
{
	Profile &profile = *ProfileManager::instance().currentProfile();

	// Widget
	SessionConfigWidget *sessionConfigWidget = new SessionConfigWidget(0, true);

	SessionConfig newConfig = SessionConfig::getTemplate();
	newConfig.setName(profile.getUniqSessionConfigName());
	profile.addSessionConfig(newConfig);
	sessionConfigWidget->init(newConfig);

	// Node
	QTreeWidgetItem *item = createNode(itemConnections, sessionConfigWidget, newConfig.name());
	treeMain->setCurrentItem(item);
}

void DialogSettings::removeSessionConfig()
{
	Profile &profile = *ProfileManager::instance().currentProfile();

	// Remove the session config
	SessionConfigWidget *widget = qobject_cast<SessionConfigWidget*>(sender());

	// Remove internal datas
	profile.deleteSessionConfig(widget->oldName());

	// Destroy the node and widget
	deleteNode(getNodeByWidget(widget));

	// Focus on the connections node
	treeMain->setCurrentItem(itemConnections);
}

void DialogSettings::keepAliveChecked(int state)
{
	spinBoxKeepAlive->setEnabled(state == Qt::Checked);
	labelKeepAliveSeconds->setEnabled(state == Qt::Checked);
}

void DialogSettings::logsCustomDirClicked()
{
	QString dir = QFileDialog::getExistingDirectory(this, tr("Choose a logs directory"));
	lineEditLogsCustomDir->setText(dir);
}

void DialogSettings::tabsTypeAllInOneToggled(bool checked)
{
	if (checked)
		stackedWidgetTabs->setCurrentIndex(0);
}

void DialogSettings::tabsTypeSuperToggled(bool checked)
{
	if (checked)
		stackedWidgetTabs->setCurrentIndex(1);
}

void DialogSettings::refreshTabExample(bool)
{
	if (radioButtonTabsAllInOne->isChecked())
	{
		mtwExample->setDisplayMode(MultiTabWidget::DisplayMode_AllInOneRow);
		
		if (radioButtonTabsAllInTop->isChecked())
			mtwExample->setAllInOneRowLocation(MultiTabWidget::TabLocation_North);
		else
			mtwExample->setAllInOneRowLocation(MultiTabWidget::TabLocation_South);
	}
	else
	{
		mtwExample->setDisplayMode(MultiTabWidget::DisplayMode_Hierarchical);
		if (radioButtonTabsSuperOnTop->isChecked())
			mtwExample->setSuperLocation(MultiTabWidget::TabLocation_North);
		else
			mtwExample->setSuperLocation(MultiTabWidget::TabLocation_South);

		if (radioButtonTabsOnTop->isChecked())
			mtwExample->setSubLocation(MultiTabWidget::TabLocation_North);
		else
			mtwExample->setSubLocation(MultiTabWidget::TabLocation_South);
	}

}

void DialogSettings::changeAwaySeparatorColor()
{
	Profile &profile = *ProfileManager::instance().currentProfile();

	QColor color = QColorDialog::getColor(profile.awaySeparatorColor);
	if (color.isValid())
	{
		QPalette palette = lineEditAwaySeparatorColor->palette();
		palette.setColor(QPalette::Text, color);
		lineEditAwaySeparatorColor->setPalette(palette);
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
	Profile &profile = *ProfileManager::instance().currentProfile();

	QString period = lineEditAwaySeparatorPeriod->text();
	int length = spinBoxAwaySeparatorLength->value();
	lineEditAwaySeparatorColor->setText(profile.getAwaySeparator(period, length));
}

void DialogSettings::reject()
{
	// Recopy old profile into the new
	Profile &profile = *ProfileManager::instance().currentProfile();
	TextSkin &newTextSkin = profile.textSkin();
	newTextSkin = m_oldTextSkin;

	DialogConfig::reject();
}
