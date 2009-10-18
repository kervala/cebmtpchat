TEMPLATE = app
CONFIG += debug_and_release precompile_header
QT += xml network

# Use Precompiled headers (PCH)
PRECOMPILED_HEADER = src/common.h

HEADERS += \
    src/action.h \
    src/actions_model.h \
    src/autoupdate.h \
    src/channel_widget.h \
    src/cmd_output_widget.h \
    src/detailed_fonts_settings_widget.h \
    src/dialog_about.h \
    src/dialog_broadcast.h \
    src/dialog_session_config.h \
    src/dialog_settings.h \
    src/dialog_shortcut.h \
    src/dialog_update.h \
    src/dialog_warningo.h \
    src/dialog_whatsnew.h \
    src/event_script.h \
    src/fonts_settings_widget.h \
    src/general_settings_widget.h \
    src/global.h \
    src/idle_settings_widget.h \
    src/language_manager.h \
    src/links_settings_widget.h \
    src/logger.h \
    src/logs_settings_widget.h \
    src/lua_utils.h \
    src/main_window.h \
    src/message_item.h \
    src/message_model.h \
    src/message_widget.h \
    src/misc_settings_widget.h \
    src/mtpchat_handler.h \
    src/my_application.h \
    src/my_menubar.h \
    src/my_textedit.h \
    src/output_settings_widget.h \
    src/paths.h \
    src/profile.h \
    src/property.h \
    src/render_script.h \
    src/render_segment.h \
    src/script.h \
    src/search_widget.h \
    src/server_group.h \
    src/session.h \
    src/session_config.h \
    src/session_config_widget.h \
    src/session_manager.h \
    src/session_widget.h \
    src/settings_widget.h \
    src/shortcuts_settings_widget.h \
    src/sound_settings_widget.h \
    src/system_widget.h \
    src/tabs_settings_widget.h \
    src/tell_widget.h \
    src/token.h \
    src/token_display.h \
    src/token_factory.h \
    src/token_info.h \
    src/token_renderer.h \
    src/token_script.h \
    src/transfers_manager.h \
    src/transfers_widget.h \
    src/tray_settings_widget.h \
    src/url_textedit.h \
    src/version.h \
    src/warningo_settings_widget.h \
    src/who_model.h \
    src/who_user.h

SOURCES += \
    src/action.cpp \
    src/actions_model.cpp \
    src/autoupdate.cpp \
    src/channel_widget.cpp \
    src/cmd_output_widget.cpp \
    src/detailed_fonts_settings_widget.cpp \
    src/dialog_about.cpp \
    src/dialog_broadcast.cpp \
    src/dialog_session_config.cpp \
    src/dialog_settings.cpp \
    src/dialog_shortcut.cpp \
    src/dialog_update.cpp \
    src/dialog_warningo.cpp \
    src/dialog_whatsnew.cpp \
    src/event_script.cpp \
    src/fonts_settings_widget.cpp \
    src/general_settings_widget.cpp \
    src/global.cpp \
    src/idle_settings_widget.cpp \
    src/language_manager.cpp \
    src/links_settings_widget.cpp \
    src/logger.cpp \
    src/logs_settings_widget.cpp \
    src/lua_utils.cpp \
    src/main.cpp \
    src/main_window.cpp \
    src/message_model.cpp \
    src/message_widget.cpp \
    src/misc_settings_widget.cpp \
    src/mtpchat_handler.cpp \
    src/my_application.cpp \
    src/my_menubar.cpp \
    src/my_textedit.cpp \
    src/output_settings_widget.cpp \
    src/paths.cpp \
    src/profile.cpp \
    src/property.cpp \
    src/render_script.cpp \
    src/render_segment.cpp \
    src/script.cpp \
    src/search_widget.cpp \
    src/server_group.cpp \
    src/session.cpp \
    src/session_config.cpp \
    src/session_config_widget.cpp \
    src/session_manager.cpp \
    src/session_widget.cpp \
    src/shortcuts_settings_widget.cpp \
    src/sound_settings_widget.cpp \
    src/system_widget.cpp \
    src/tabs_settings_widget.cpp \
    src/tell_widget.cpp \
    src/token.cpp \
    src/token_display.cpp \
    src/token_factory.cpp \
    src/token_info.cpp \
    src/token_renderer.cpp \
    src/token_script.cpp \
    src/transfers_manager.cpp \
    src/transfers_widget.cpp \
    src/tray_settings_widget.cpp \
    src/url_textedit.cpp \
    src/warningo_settings_widget.cpp \
    src/who_model.cpp \
    src/who_user.cpp

FORMS += \
    ui/idle_settings.ui \
    ui/detailed_fonts_settings.ui \
    ui/dialog_about.ui \
    ui/fonts_settings.ui \
    ui/general_settings.ui \
    ui/links_settings.ui \
    ui/logs_settings.ui \
    ui/misc_settings.ui \
    ui/output_settings.ui \
    ui/session_config_widget.ui \
    ui/shortcuts_settings.ui \
    ui/sound_settings.ui \
    ui/tabs_settings.ui \
    ui/tray_settings.ui \
    ui/warningo_settings.ui

TRANSLATIONS = \
    ceb_fr.ts \
    ceb_nl.ts \
    ceb_pt-br.ts \
    ceb_us.ts \
    ceb_en.ts

RESOURCES = ceb.qrc

INCLUDEPATH += ../max/include
DEPENDPATH += uic

LIBMAX = max
TARGET = ceb

CONFIG(debug, debug|release) {
    TARGET = cebd
    LIBMAX = maxd
}

message(Using configuration $$LIBMAX)

win32{
    RC_FILE = ceb.rc
    win32-msvc*{
        LIBS += ../max/lib/$${LIBMAX}.lib lua51.lib ole32.lib shell32.lib
    }
    win32-g++{
        LIBS += ../max/lib/lib$${LIBMAX}.a lua51.dll
    }
}

unix{
    LIBS += ../max/lib/lib$${LIBMAX}.a -llua
}

linux{
    CONFIG += link_pkgconfig
    PKGCONFIG += lua
}

macx{
    CONFIG += x86 ppc
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.4
    QMAKE_MAC_SDK = /Developer/SDKs/MacOSX10.4u.sdk
    RC_FILE = misc/ceb.icns
    QMAKE_INFO_PLIST = misc/Info.plist
    QMAKE_CXXFLAGS_x86 += -mmacosx-version-min=10.4
}

DESTDIR = bin
OBJECTS_DIR = obj
MOC_DIR = moc
UI_DIR = uic
