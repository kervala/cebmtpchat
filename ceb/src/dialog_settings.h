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

#ifndef DIALOG_SETTINGS_H
#define DIALOG_SETTINGS_H

#include <QRadioButton>
#include <QCheckBox>
#include <QSpinBox>
#include <QLabel>
#include <QGroupBox>
#include <QStackedWidget>
#include <QComboBox>
#include <QSignalMapper>

#include <multi_tab_widget.h>
#include <dialog_config.h>

#include "token_display.h"

class DialogSettings : public DialogConfig
{
    Q_OBJECT

public:
    DialogSettings(QWidget *parent = 0);

    void reject();

protected slots:
    virtual void checkValues();

private:
    // General
    QComboBox *comboBoxLanguage;
    QCheckBox *checkBoxCheckForUpdate;

    // Shortcuts
    QWidget *shortcutsWidget;

    // Connections
    QTreeWidgetItem *itemConnections;
    QPushButton *pushButtonNewConnection;

    // Fonts
    QWidget *fontsWidget;

    // Detailed fonts
    QWidget *detailedFontsWidget;

    // Logs
    QGroupBox *groupBoxLogs;
    QRadioButton *radioButtonLogsDefaultDir;
    QRadioButton *radioButtonLogsCustomDir;
    QLineEdit *lineEditLogsCustomDir;
    QRadioButton *radioButtonLogsDaily;
    QRadioButton *radioButtonLogsWeekly;
    QRadioButton *radioButtonLogsMonthly;
    QRadioButton *radioButtonLogsUniq;
    QCheckBox *checkBoxLogsTimeStamp;

    // Tray
    QGroupBox *groupBoxTray;
    QCheckBox *checkBoxTrayAlwaysVisible;
    QCheckBox *checkBoxTrayHideFromTaskBar;

    // Warningo
    QWidget *warningoWidget;

    // Sounds
    QWidget *soundsWidget;

    // Idle
    QWidget *idleWidget;

    // Tabs
    QCheckBox *checkBoxTabsIcons;
    QStackedWidget *stackedWidgetTabs;
    QRadioButton *radioButtonTabsAllInOne;
    QRadioButton *radioButtonTabsAllInTop;
    QRadioButton *radioButtonTabsAllInBottom;
    QRadioButton *radioButtonTabsSuper;
    QRadioButton *radioButtonTabsSuperOnTop;
    QRadioButton *radioButtonTabsSuperOnBottom;
    QRadioButton *radioButtonTabsOnTop;
    QRadioButton *radioButtonTabsOnBottom;
    MultiTabWidget *mtwExample;

    // Links
    QWidget *linksWidget;

    // Output
    QGroupBox *groupBoxAwaySeparator;
    QLineEdit *lineEditAwaySeparatorColor;
    QPushButton *pushButtonAwaySeparatorColor;
    QSpinBox *spinBoxAwaySeparatorLength;
    QLineEdit *lineEditAwaySeparatorPeriod;

    // Misc
    QRadioButton *radioButtonTimeStampClassic;
    QRadioButton *radioButtonTimeStampAlways;
    QRadioButton *radioButtonTimeStampNever;
    QCheckBox *checkBoxTimeStampInTell;
    QCheckBox *checkBoxKeepAlive;
    QSpinBox *spinBoxKeepAlive;
    QLabel *labelKeepAliveSeconds;
    QCheckBox *checkBoxTabForWho;
    QCheckBox *checkBoxTabForWall;
    QCheckBox *checkBoxTabForFinger;
    QCheckBox *checkBoxCopyOnSelection;

    QWidget *createGeneralWidget();
    QWidget *createShortcutsWidget();
    void createConnectionsNodes();
    QWidget *createConnectionsWidget();
    QWidget *createFontsWidget();
    QWidget *createDetailedFontsWidget();
    QWidget *createLogsWidget();
    QWidget *createTrayWidget();
    QWidget *createWarningoWidget();
    QWidget *createSoundsWidget();
    QWidget *createIdleWidget();
    QWidget *createTabsWidget();
    QWidget *createLinksWidget();
    QWidget *createOutputWidget();
    QWidget *createMiscWidget();

    bool verifyControlsDatas();

    // Get datas
    void getGeneralControlsDatas();
    void getShortcutsControlsDatas();
    void getConnectionsControlsDatas(); // Collect all datas concerning connections
    void getLogsControlsDatas();
    void getFontsControlsDatas();
    void getDetailedFontsControlsDatas();
    void getTrayControlsDatas();
    void getWarningoControlsDatas();
    void getSoundsControlsDatas();
    void getIdleControlsDatas();
    void getTabsControlsDatas();
    void getLinksControlsDatas();
    void getOutputControlsDatas();
    void getMiscControlsDatas();
    void getControlsDatas();

    QMap<QString, QString> displayToLanguage;
    QString getLanguageDisplay(const QString &language);

    TextSkin m_oldTextSkin;

private slots:
    void newSessionConfig();
    void removeSessionConfig();
    void keepAliveChecked(int state);
    void logsCustomDirClicked();
    void tabsTypeAllInOneToggled(bool checked);
    void tabsTypeSuperToggled(bool checked);
    void refreshTabExample(bool checked = false);
    void changeAwaySeparatorColor();
    void refreshAwaySeparatorPreview();
    void awaySeparatorLengthChanged(int a);

};


#endif // DIALOG_SETTINGS_H
