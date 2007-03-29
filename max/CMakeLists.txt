PROJECT(MAX)

SET(QT_USE_QTXML TRUE)

INCLUDE(${QT_USE_FILE})

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src)

SET(MAX_SRCS
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
)

SET(MAX_MOCH
	src/dialog_basic.h
	src/dialog_config.h
	src/history_widget.h
	src/chat_line_widget.h
	src/multi_tab_widget.h
	src/my_tabwidget.h
	src/my_tabbar.h
	src/filter_widget.h
	src/generic_sort_model.h
)

ADD_DEFINITIONS(${QT_DEFINITIONS})

QT4_WRAP_CPP(MAX_MOC ${MAX_MOCH})

SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

ADD_LIBRARY(max ${MAX_SRCS} ${MAX_MOC})

