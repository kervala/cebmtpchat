PROJECT(CEB_LOCAL)

CMAKE_MINIMUM_REQUIRED(VERSION 2.4.0)

FIND_PACKAGE(Qt4 REQUIRED) # find and setup Qt4 for this project

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
	src/shortcuts_settings_widget.cpp
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
	src/shortcuts_settings_widget.h
)

# ADD_DEFINITIONS(-Wall -DUNICODE -DQT_LARGEFILE_SUPPORT -DQT_NO_DEBUG -DQT_XML_LIB -DQT_GUI_LIB -DQT_NETWORK_LIB -DQT_CORE_LIB -DQT_THREAD_SUPPORT) # -DQT_NEEDS_QMAIN)
ADD_DEFINITIONS(${QT_DEFINITIONS})

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

ADD_EXECUTABLE(ceb ${CEB_SRCS} ${CEB_UIS_H} ${CEB_MOC} ${CEB_RES})

IF(UNIX)
	TARGET_LINK_LIBRARIES(
		ceb ${CEB_SOURCE_DIR}/max/libmax.a ${QT_LIBRARIES} /usr/lib/liblua5.1.so
	)
ELSEIF(WIN32)
	TARGET_LINK_LIBRARIES(
		ceb ${CEB_SOURCE_DIR}/max/libmax.a ${QT_LIBRARIES} ${CEB_SOURCE_DIR}/CONTRIB/lua-5.1/src/lua51.dll -lgdi32 -lcomdlg32 -loleaut32 
            -limm32 -lwinmm -lwinspool -lmsimg32 -lQtCore -lkernel32 -luser32 -lshell32 -luuid -lole32 -ladvapi32 -lws2_32
	)
ENDIF(UNIX)
