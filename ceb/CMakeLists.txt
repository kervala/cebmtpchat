PROJECT(CEB_LOCAL)

CMAKE_MINIMUM_REQUIRED(VERSION 2.4.0)

SET(QT_USE_QTXML TRUE)
SET(QT_USE_QTNETWORK TRUE)

INCLUDE(${QT_USE_FILE})

INCLUDE_DIRECTORIES(${CEB_SOURCE_DIR}/max/src)
IF(WIN32)
	INCLUDE_DIRECTORIES(${CEB_SOURCE_DIR}/CONTRIB/lua-5.1/src)
ELSE(WIN32)
	INCLUDE_DIRECTORIES(/usr/include/lua5.1)
ENDIF(WIN32)

SET(CEB_SRCS
	src/main.cpp
	src/my_application.cpp
	src/main_window.cpp
	src/dialog_settings.cpp
	src/profile.cpp
	src/session_config.cpp
	src/session_widget.cpp
	src/channel_widget.cpp
	src/tell_widget.cpp
	src/dialog_about.cpp
	src/session.cpp
	src/session_manager.cpp
	src/session_config_widget.cpp
	src/dialog_session_config.cpp
	src/token_info.cpp
	src/token_factory.cpp
	src/my_textedit.cpp
	src/token.cpp
	src/logger.cpp
	src/cmd_output_widget.cpp
	src/autoupdate.cpp
	src/dialog_update.cpp
	src/dialog_warningo.cpp
	src/language_manager.cpp
	src/message_model.cpp
	src/message_widget.cpp
	src/my_menubar.cpp
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
	src/render_script.cpp
	src/links_settings_widget.cpp
	src/transfers_widget.cpp
	src/transfers_manager.cpp
	src/shortcuts_settings_widget.cpp
	src/action.cpp
	src/actions_model.cpp
	src/dialog_shortcut.cpp
	src/search_widget.cpp
        src/global.cpp
        src/paths.cpp
        src/script.cpp
        src/token_script.cpp
        src/property.cpp
	src/event_script.cpp
	src/who_user.cpp
	src/who_model.cpp
	src/server_group.cpp
	src/misc_settings_widget.cpp
	src/tabs_settings_widget.cpp
	src/general_settings_widget.cpp
	src/logs_settings_widget.cpp
	src/output_settings_widget.cpp
	src/tray_settings_widget.cpp
	src/system_widget.cpp
	src/mtpchat_handler.cpp
	src/url_textedit.cpp
	src/dialog_broadcast.cpp
)

SET(CEB_UIS
	ui/sound_settings.ui
	ui/idle_settings.ui
	ui/warningo_settings.ui
	ui/session_config_widget.ui
	ui/detailed_fonts_settings.ui
	ui/fonts_settings.ui
	ui/links_settings.ui
	ui/shortcuts_settings.ui
	ui/dialog_about.ui
	ui/misc_settings.ui
	ui/tabs_settings.ui
	ui/general_settings.ui
	ui/logs_settings.ui
	ui/output_settings.ui
	ui/tray_settings.ui
)

QT4_WRAP_UI(CEB_UIS_H ${CEB_UIS})

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

SET(CEB_MOCH
	src/main_window.h
	src/my_application.h
	src/dialog_settings.h
	src/session_widget.h
	src/channel_widget.h
	src/tell_widget.h
	src/dialog_about.h
	src/session.h
	src/session_manager.h
	src/session_config_widget.h
	src/dialog_session_config.h
	src/token_factory.h
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
	src/shortcuts_settings_widget.h
	src/actions_model.h
	src/dialog_shortcut.h
	src/search_widget.h
	src/who_user.h
	src/who_model.h
	src/server_group.h
	src/message_model.h
	src/misc_settings_widget.h
	src/tabs_settings_widget.h
	src/general_settings_widget.h
	src/logs_settings_widget.h
	src/output_settings_widget.h
	src/tray_settings_widget.h
	src/system_widget.h
	src/mtpchat_handler.h
	src/url_textedit.h
	src/dialog_broadcast.h
)

ADD_DEFINITIONS(${QT_DEFINITIONS} -DQT_NO_DEBUG)

QT4_WRAP_CPP(CEB_MOC ${CEB_MOCH})

QT4_ADD_RESOURCES(CEB_RES ceb.qrc)

IF(MINGW)
	# resource compilation for MinGW
	ADD_CUSTOM_COMMAND(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/ceb_rc.o
		COMMAND windres.exe -I${CMAKE_CURRENT_SOURCE_DIR} -i${CMAKE_CURRENT_SOURCE_DIR}/ceb.rc 
		-o ${CMAKE_CURRENT_BINARY_DIR}/ceb_rc.o )
	SET(CEB_SRCS ${CEB_SRCS} ${CMAKE_CURRENT_BINARY_DIR}/ceb_rc.o)
ELSE(MINGW)
	SET(CEB_SRCS ${CEB_SRCS} ceb.rc)
ENDIF(MINGW)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

IF(WIN32)
	ADD_DEFINITIONS(-DZLIB_WINAPI)
ENDIF(WIN32)

IF(WIN32)
	ADD_EXECUTABLE(ceb WIN32 ${CEB_SRCS} ${CEB_UIS_H} ${CEB_MOC} ${CEB_RES})
ELSE(WIN32)
	ADD_EXECUTABLE(ceb ${CEB_SRCS} ${CEB_UIS_H} ${CEB_MOC} ${CEB_RES})
ENDIF(WIN32)

IF(UNIX)
	TARGET_LINK_LIBRARIES(
		ceb ${CEB_SOURCE_DIR}/max/lib/libmax.a ${QT_LIBRARIES} /usr/lib/liblua5.1.so
	)
ELSEIF(WIN32)
	TARGET_LINK_LIBRARIES(
		ceb ${CEB_SOURCE_DIR}/max/lib/Release/max.lib ${QT_LIBRARIES} ${QT_QTMAIN_LIBRARY} ${CEB_SOURCE_DIR}/CONTRIB/lua-5.1/src/lua51.lib
	)
ENDIF(UNIX)

INSTALL(TARGETS ceb
	RUNTIME DESTINATION bin)
INSTALL(DIRECTORY share/ceb
        DESTINATION share
        PATTERN .svn
        EXCLUDE)
INSTALL(DIRECTORY share/applications
        DESTINATION share
        PATTERN .svn
        EXCLUDE)
INSTALL(DIRECTORY share/pixmaps
        DESTINATION share
        PATTERN .svn
        EXCLUDE)
