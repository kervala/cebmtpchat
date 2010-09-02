PROJECT(MAX)

SET(QT_USE_QTXML TRUE)

INCLUDE(${QT_USE_FILE})

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src ${PROJECT_SOURCE_DIR}/include)

SET(CMAKE_BUILD_TYPE Release)

SET(MAX_SRCS
	src/common.cpp
	src/dialog_basic.cpp
	src/dialog_config.cpp
	src/xml_handler.cpp
	src/history_widget.cpp
	src/chat_line_widget.cpp
	src/multi_tab_widget.cpp
	src/my_tabwidget.cpp
	src/my_tabbar.cpp
	src/filter_widget.cpp
	src/generic_sort_model.cpp
	src/icon_editor.cpp
)

SET(MAX_MOCH
	include/dialog_basic.h
	include/dialog_config.h
	include/history_widget.h
	include/chat_line_widget.h
	include/multi_tab_widget.h
	include/my_tabwidget.h
	include/my_tabbar.h
	include/filter_widget.h
	include/generic_sort_model.h
	include/icon_editor.h
)

ADD_DEFINITIONS(${QT_DEFINITIONS} -DQT_NO_DEBUG)

QT4_WRAP_CPP(MAX_MOC ${MAX_MOCH})

SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

ADD_LIBRARY(max ${MAX_SRCS} ${MAX_MOC})

ADD_NATIVE_PRECOMPILED_HEADER(max ${CMAKE_CURRENT_SOURCE_DIR}/src/common.h ${CMAKE_CURRENT_SOURCE_DIR}/src/common.cpp)
