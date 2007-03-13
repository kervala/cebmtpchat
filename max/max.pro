TEMPLATE=lib

CONFIG += qt staticlib

QT += xml

VERSION=0.1

HEADERS += src/dialog_basic.h \
           src/dialog_config.h \
           src/xml_handler.h \
           src/history_widget.h \
           src/chat_line_widget.h \
           src/multi_tab_widget.h \
           src/my_tabwidget.h \
           src/my_tabbar.h \
           src/filter_widget.h \
           src/generic_sort_model.h

SOURCES += src/dialog_basic.cpp \
           src/dialog_config.cpp \
           src/xml_handler.cpp \
           src/history_widget.cpp \
           src/chat_line_widget.cpp \
           src/multi_tab_widget.cpp \
           src/my_tabwidget.cpp \
           src/my_tabbar.cpp \
           src/filter_widget.cpp \
           src/generic_sort_model.cpp

INCLUDEPATH += include
INCLUDEPATH += ./src

DESTDIR=lib

TARGET=max

unix {
  OBJECTS_DIR=.obj
  MOC_DIR=.moc
}

win32 {
  OBJECTS_DIR=obj
  MOC_DIR=moc
}
