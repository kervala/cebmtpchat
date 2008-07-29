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
    QComboBox *_comboBoxLanguage;
    QCheckBox *_checkBoxCheckForUpdate;
    QCheckBox *_checkBoxKeepAboveOtherWindows;

    // Shortcuts
    QWidget *_shortcutsWidget;

    // Connections
    QTreeWidgetItem *_itemConnections;
    QPushButton *_pushButtonNewConnection;

    // Fonts
    QWidget *_fontsWidget;

    // Detailed fonts
    QWidget *_detailedFontsWidget;

    // Logs
    QGroupBox *_groupBoxLogs;
    QRadioButton *_radioButtonLogsDefaultDir;
    QRadioButton *_radioButtonLogsCustomDir;
    QLineEdit *_lineEditLogsCustomDir;
    QRadioButton *_radioButtonLogsDaily;
    QRadioButton *_radioButtonLogsWeekly;
    QRadioButton *_radioButtonLogsMonthly;
    QRadioButton *_radioButtonLogsUniq;
    QCheckBox *_checkBoxLogsTimeStamp;

    // Tray
    QGroupBox *_groupBoxTray;
    QCheckBox *_checkBoxTrayAlwaysVisible;
    QCheckBox *_checkBoxTrayHideFromTaskBar;

    // Warningo
    QWidget *_warningoWidget;

    // Sounds
    QWidget *_soundsWidget;

    // Idle
    QWidget *_idleWidget;

    // Tabs
    QCheckBox *_checkBoxTabsIcons;
    QStackedWidget *_stackedWidgetTabs;
    QComboBox *_comboBoxTabsCaptionMode;
    QRadioButton *_radioButtonTabsAllInOne;
    QRadioButton *_radioButtonTabsAllInTop;
    QRadioButton *_radioButtonTabsAllInBottom;
    QRadioButton *_radioButtonTabsSuper;
    QRadioButton *_radioButtonTabsSuperOnTop;
    QRadioButton *_radioButtonTabsSuperOnBottom;
    QRadioButton *_radioButtonTabsOnTop;
    QRadioButton *_radioButtonTabsOnBottom;
    MultiTabWidget *_mtwExample;

    // Links
    QWidget *_linksWidget;

    // Output
    QGroupBox *_groupBoxAwaySeparator;
    QLineEdit *_lineEditAwaySeparatorColor;
    QPushButton *_pushButtonAwaySeparatorColor;
    QSpinBox *_spinBoxAwaySeparatorLength;
    QLineEdit *_lineEditAwaySeparatorPeriod;

    // Misc
    QWidget *_miscWidget;

    // Creation functions
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

    TextSkin _oldTextSkin;

    bool verifyControlsDatas();

    // Get datas
    void getGeneralControlsDatas();
    void getShortcutsControlsDatas();
    void getConnectionsControlsDatas(); //!< Collect all datas concerning connections
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

    void fillMultiTabWidget();

private slots:
    void newSessionConfig();
    void removeSessionConfig();
    void logsCustomDirClicked();
    void tabsTypeAllInOneToggled(bool checked);
    void tabsTypeSuperToggled(bool checked);
    void currentTabCaptionModeChanged(int index);
    void refreshTabExample(bool checked = false);
    void changeAwaySeparatorColor();
    void refreshAwaySeparatorPreview();
    void awaySeparatorLengthChanged(int a);
};


#endif // DIALOG_SETTINGS_H
