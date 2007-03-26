find_package(Qt4 REQUIRED) # find and setup Qt4 for this project

set(QT_USE_QTXML TRUE)
set(QT_USE_QTNETWORK TRUE)

include(${QT_USE_FILE})

include_directories(${CEB_SOURCE_DIR}/max/src)
include_directories(/usr/include/lua5.1)

set(ceb_SRCS
	src/main.cpp
	src/my_application.cpp
    src/form_main.cpp
    src/dialog_settings.cpp
    src/profile.cpp
    src/session_config.cpp
    src/profile_manager.cpp
    src/general_config.cpp
    src/session_widget.cpp
    src/channel_widget.cpp
    src/tell_widget.cpp
    src/dialog_about.cpp
    src/dialog_system.cpp
    src/session.cpp
    src/session_manager.cpp
    src/session_config_widget.cpp
    src/dialog_session_config.cpp
    src/mtp_token_info.cpp
    src/mtp_analyzer.cpp
	src/my_textedit.cpp
	src/token_event.cpp
	src/logger.cpp
    src/cmd_output_widget.cpp
    src/autoupdate.cpp
    src/dialog_update.cpp
    src/dialog_warningo.cpp
    src/language_manager.cpp
    src/message_model.cpp
    src/message_widget.cpp
    src/message_item.cpp
    src/my_menubar.cpp
    src/settings_widget.cpp
    src/sound_settings_widget.cpp
    src/idle_settings_widget.cpp
    src/warningo_settings_widget.cpp
    src/dialog_whatsnew.cpp
    src/lua_utils.cpp
	src/detailed_fonts_settings_widget.cpp
    src/token_display.cpp
    src/token_renderer.cpp
    src/fonts_settings_widget.cpp
    src/render_segment.cpp
    src/modifier.cpp
    src/links_settings_widget.cpp
    src/transfers_widget.cpp
    src/transfers_manager.cpp
)

set(ceb_UIS
	ui/sound_settings.ui
	ui/idle_settings.ui
	ui/warningo_settings.ui
	ui/session_config_widget.ui
	ui/detailed_fonts_settings.ui
	ui/fonts_settings.ui
	ui/links_settings.ui
	ui/dialog_about.ui
)

QT4_WRAP_UI(ceb_UIS_H ${ceb_UIS})

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(ceb_MOCH
	src/form_main.h
	src/my_application.h
	src/dialog_settings.h
	src/session_widget.h
	src/channel_widget.h
	src/tell_widget.h
	src/dialog_about.h
	src/dialog_system.h
	src/session.h
	src/session_manager.h
	src/session_config_widget.h
	src/dialog_session_config.h
	src/mtp_analyzer.h
	src/my_textedit.h
	src/logger.h
	src/cmd_output_widget.h
	src/autoupdate.h
	src/dialog_update.h
	src/dialog_warningo.h
	src/message_widget.h
	src/my_menubar.h
	src/settings_widget.h
	src/sound_settings_widget.h
	src/idle_settings_widget.h
	src/warningo_settings_widget.h
	src/dialog_whatsnew.h
	src/detailed_fonts_settings_widget.h
	src/fonts_settings_widget.h
	src/links_settings_widget.h
	src/transfers_widget.h
	src/transfers_manager.h
)

QT4_WRAP_CPP(ceb_MOC ${ceb_MOCH})

QT4_ADD_RESOURCES(ceb_RES ceb.qrc)

add_executable(ceb ${ceb_SRCS} ${ceb_MOC} ${ceb_RES})

TARGET_LINK_LIBRARIES(
 ceb ${QT_LIBRARIES} ${CEB_SOURCE_DIR}/max/libmax.a /usr/lib/liblua5.1.so
)
