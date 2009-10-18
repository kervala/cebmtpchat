TEMPLATE = lib
CONFIG += staticlib release precompile_header
QT += xml

# Use Precompiled headers (PCH)
PRECOMPILED_HEADER = src/common.h

VERSION = 0.1

HEADERS += \
    include/chat_line_widget.h \
    include/dialog_basic.h \
    include/dialog_config.h \
    include/filter_widget.h \
    include/generic_sort_model.h \
    include/history_widget.h \
    include/multi_tab_widget.h \
    include/my_tabbar.h \
    include/my_tabwidget.h \
    include/xml_handler.h

SOURCES += \
    src/chat_line_widget.cpp \
    src/dialog_basic.cpp \
    src/dialog_config.cpp \
    src/filter_widget.cpp \
    src/generic_sort_model.cpp \
    src/history_widget.cpp \
    src/multi_tab_widget.cpp \
    src/my_tabbar.cpp \
    src/my_tabwidget.cpp \
    src/xml_handler.cpp

INCLUDEPATH += include
DESTDIR = lib
TARGET = max

CONFIG(debug, debug|release) {
    TARGET = maxd
}

OBJECTS_DIR = obj
MOC_DIR = moc

macx{
    CONFIG += x86
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.4
    QMAKE_MAC_SDK = /Developer/SDKs/MacOSX10.4u.sdk
    QMAKE_CXXFLAGS += -mmacosx-version-min=10.4
}
