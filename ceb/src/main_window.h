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

#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QMenuBar>
#include <QStatusBar>
#include <QMenu>
#include <QAction>
#include <QTabWidget>
#include <QSignalMapper>
#include <QSystemTrayIcon>
#include <QShortcut>

#include <my_tabwidget.h>

#include "session.h"
#include "system_widget.h"
#include "autoupdate.h"
#include "channel_widget.h"
#include "tell_widget.h"
#include "cmd_output_widget.h"
#include "message_widget.h"
#include "transfers_widget.h"
#include "my_menubar.h"
#include "action.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    static MainWindow *instance();

    //! For Lua scripts
    QWidget *getTab(Session *session, const QString &category, const QString &argument);
    QColor getTabColor(QWidget *widget) const;
    void setTabColor(QWidget *widget, const QColor &color);
    bool isTabFocused(QWidget *widget) const;

protected:
    void closeEvent(QCloseEvent *event);
#if defined(Q_OS_WIN32)
    bool winEvent(MSG *message, long *result);
#endif
    bool event(QEvent *e);
    void showEvent(QShowEvent *event);
    bool eventFilter(QObject *watched, QEvent *event);

private:
    static MainWindow *_instance;

    // Menu bar
    MyMenuBar *mbMain;
    // Connections menu
    QMenu *mConnectTo;
    QAction *actionReconnect;
    QAction *actionCloseConnection;
    QSignalMapper *connectionsSignalMapper;
    // Edit menu
    QAction *actionEditConnectionConfig;
    // Help menu
#ifdef Q_OS_WIN32
    QAction *actionCheckForUpdate;
#endif // Q_OS_WIN32
    // Status bar
    QStatusBar *sbMain;
    MyTabWidget *tabWidgetMain;
    QTimer animationTimer;
    QMap<QWidget*,int> tabToBlinkTime;
    QMap<QWidget*,bool> tabToAscendingOrder;
    QSystemTrayIcon *trayIcon;
    QMenu *trayMenu;
    bool trayTalkAboutMe;
    AutoUpdate autoUpdate;
    QSignalMapper *actionSignalMapper;
    QList<QShortcut*> actionShortcuts;

    QAction *_actionToggleMenuBarVisibility;
    QAction *_actionToggleStatusBarVisibility;
    QAction *_actionToggleSystemLogsVisibility;
    QAction *_actionToggleTopicVisibility;
    QAction *_actionToggleUsersVisibility;

    MainWindow();
    ~MainWindow();

    void makeMenuBar();
    void makeToolBar();
    void makeConnectionsActions();

    void recordCurrentProfileDatas();
    Session *getCurrentSession();
    void closeCurrentSession();

    // Channel things
    ChannelWidget *getChannelWidget(Session *session);
    ChannelWidget *getChannelWidget(const SessionConfig &config) const;

    // Tell things
    TellWidget *getTellWidget(Session *session, const QString &login);
    TellWidget *newTellWidget(Session *session, const QString &login);

    // Cmd output things
    CmdOutputWidget *getCmdOutputWidget(Session *session, const QString &cmdName);
    CmdOutputWidget *newCmdOutputWidget(Session *session, const QString &cmdName);

    // File transfers things
    TransfersWidget *getTransfersWidget(Session *session);
    TransfersWidget *newTransfersWidget(Session *session);

    // Message things
    MessageWidget *getMessageWidget(Session *session);
    MessageWidget *newMessageWidget(Session *session);

    void renameWidget(QWidget *widget, const QString &text = "");
    void changeWidgetColor(QWidget *widget, const QColor &color);
    void removeWidget(QWidget *widget);
    void removeSessionWidgets(Session *session);

    void refreshProfileSettings(); // Refresh profile settings on every widget
    void applyProfileOnTabWidget();
    void refreshWidgets();
    void createActionShortcuts();

    Session *getCurrentSession() const;
    ChannelWidget *getChannelWidget(Session *session) const;

    ChannelWidget *connectTo(SessionConfig &config);

    QShortcut *shortcutByActionType(Action::ActionType type) const;

private:
    bool m_firstShow;

private slots:
    void newConnection();
    void reconnect();
    void closeConnection();
    void editConnectionConfig();
    void editSettings();
    void showSystemLogs();
    void checkForUpdate();
    void about();
    void aboutQt();
    void connectTo(const QString &configName);
    void connectToFromMenu(const QString &configName);
    void aboutToShowConnectionsActions();
    void aboutToShowConfigurationMenu();
    void aboutToShowWindowsMenu();
    void aboutToShowEditMenu();
    void newSessionTokenForActivity(Session *session, const Token &token);
    void newSessionToken(Session *session, const Token &token);
    void sessionLoginChanged(Session *session, const QString &oldLogin, const QString &newLogin);
    void trayActivated(QSystemTrayIcon::ActivationReason reason);
    void newProgramVersion(const QDate &);
    void whoItemDblClicked(const QString &login);
    void closeTabWidget();
//  void highlightSessionWidget();
    void tabWidgetMainCurrentChanged(int index);
    void whoUserDoubleClicked(const QString &login);
    void showLogsDir();
    void showMessages();
    void showTransfers();
    void launchWhatsNew();
    void launchBugReport();
    void launchFeatureReport();
    void menuIconClicked();
    void updateAccepted();
    void tellSessionAsked(const QString &login);
//    void animationTimeout();
    void showFileTransfers();
    void newTransfer(Transfer *transfer);
    void executeAction(int action);
    void searchActionTriggered();
    void captionChanged();
    void previousTab();
    void nextTab();
    void toggleMenuBarVisibility();
    void toggleStatusBarVisibility();
    void toggleTopicVisibility();
    void toggleUsersVisibility();
};

#endif // MAIN_WINDOW_H
