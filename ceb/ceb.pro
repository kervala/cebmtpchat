CONFIG += qt debug_and_release

QT += xml network

HEADERS += src/main_window.h \
	src/my_application.h \
    src/dialog_settings.h \
    src/profile.h \
    src/session_config.h \
    src/profile_manager.h \
    src/general_config.h \
    src/session_widget.h \
    src/channel_widget.h \
    src/tell_widget.h \
    src/dialog_about.h \
    src/dialog_system.h \
    src/session.h \
    src/session_manager.h \
    src/session_config_widget.h \
    src/dialog_session_config.h \
    src/mtp_token.h \
    src/mtp_token_info.h \
    src/mtp_analyzer.h \
    src/my_textedit.h \
    src/token_event.h \
    src/logger.h \
    src/cmd_output_widget.h \
    src/autoupdate.h \
    src/version.h \
    src/dialog_update.h \
    src/dialog_warningo.h \
    src/language_manager.h \
    src/message_model.h \
    src/message_widget.h \
    src/message_item.h \
    src/my_menubar.h \
    src/settings_widget.h \
    src/sound_settings_widget.h \
    src/idle_settings_widget.h \
    src/warningo_settings_widget.h \
    src/dialog_whatsnew.h \
    src/lua_utils.h \
    src/detailed_fonts_settings_widget.h \
    src/token_display.h \
    src/token_renderer.h \
    src/fonts_settings_widget.h \
    src/render_segment.h \
    src/modifier.h \
    src/links_settings_widget.h \
    src/transfers_widget.h \
    src/transfers_manager.h \
    src/shortcuts_settings_widget.h
      
SOURCES += src/main.cpp \
	src/my_application.cpp \
    src/main_window.cpp \
    src/dialog_settings.cpp \
    src/profile.cpp \
    src/session_config.cpp \
    src/profile_manager.cpp \
    src/general_config.cpp \
    src/session_widget.cpp \
    src/channel_widget.cpp \
    src/tell_widget.cpp \
    src/dialog_about.cpp \
    src/dialog_system.cpp \
    src/session.cpp \
    src/session_manager.cpp \
    src/session_config_widget.cpp \
    src/dialog_session_config.cpp \
    src/mtp_token_info.cpp \
    src/mtp_analyzer.cpp \
    src/my_textedit.cpp \
    src/token_event.cpp \
    src/logger.cpp \
    src/cmd_output_widget.cpp \
    src/autoupdate.cpp \
    src/dialog_update.cpp \
    src/dialog_warningo.cpp \
    src/language_manager.cpp \
    src/message_model.cpp \
    src/message_widget.cpp \
    src/message_item.cpp \
    src/my_menubar.cpp \
    src/settings_widget.cpp \
    src/sound_settings_widget.cpp \
    src/idle_settings_widget.cpp \
    src/warningo_settings_widget.cpp \
    src/dialog_whatsnew.cpp \
    src/lua_utils.cpp \
    src/detailed_fonts_settings_widget.cpp \
    src/token_display.cpp \
    src/token_renderer.cpp \
    src/fonts_settings_widget.cpp \
    src/render_segment.cpp \
    src/modifier.cpp \
    src/links_settings_widget.cpp \
    src/transfers_widget.cpp \
    src/transfers_manager.cpp \
    src/shortcuts_settings_widget.cpp

FORMS += ui/sound_settings.ui \
	ui/idle_settings.ui \
    ui/warningo_settings.ui \
    ui/session_config_widget.ui \
    ui/detailed_fonts_settings.ui \
    ui/fonts_settings.ui \
    ui/links_settings.ui \
    ui/shortcuts_settings.ui \
    ui/dialog_about.ui

RESOURCES = ceb.qrc

RC_FILE = ceb.rc

LIBS += ../max/lib/libmax.a

win32: LIBS += C:/MinGW/lib/libgdi32.a

win32: LIBS += ../CONTRIB/lua-5.1/src/lua51.dll

unix: LIBS += -llua5.1

INCLUDEPATH += ../max/src

unix: INCLUDEPATH += /usr/include/lua5.1

win32: INCLUDEPATH += ../CONTRIB/lua-5.1/src

DESTDIR = bin

TARGET = ceb

TRANSLATIONS = ceb_fr.ts \
               ceb_nl.ts \
	       ceb_pt-br.ts \
           ceb_us.ts \
           ceb_en.ts

unix {
  OBJECTS_DIR=.obj
  MOC_DIR=.moc
}

win32 {
  OBJECTS_DIR=obj
  MOC_DIR=moc
}
