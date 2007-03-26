find_package(Qt4 REQUIRED) # find and setup Qt4 for this project

set(QT_USE_QTXML TRUE)

include(${QT_USE_FILE})

set(max_SRCS
	dialog_basic.cpp
	dialog_config.cpp
	xml_handler.cpp
	history_widget.cpp
	chat_line_widget.cpp
	multi_tab_widget.cpp
	my_tabwidget.cpp
	my_tabbar.cpp
	filter_widget.cpp
	generic_sort_model.cpp
)

set(max_MOCH
	dialog_basic.h
	dialog_config.h
	history_widget.h
	chat_line_widget.h
	multi_tab_widget.h
	my_tabwidget.h
	my_tabbar.h
	filter_widget.h
	generic_sort_model.h
)

QT4_WRAP_CPP(max_MOC ${max_MOCH})

add_library(max ${max_SRCS} ${max_MOC})
