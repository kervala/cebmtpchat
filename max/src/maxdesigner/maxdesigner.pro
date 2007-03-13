CONFIG      += designer plugin release
TEMPLATE    = lib

CONFIG(debug, debug|release) {
    unix: TARGET = $$join(TARGET,,,_debug)
    else: TARGET = $$join(TARGET,,d)
}

HEADERS     = ../filter_widget.h \
              ../generic_sort_model.h \
              filter_widget_plugin.h
SOURCES     = ../filter_widget.cpp \
              ../generic_sort_model.cpp \
              filter_widget_plugin.cpp

# install
target.path = $$[QT_INSTALL_PLUGINS]/designer
INSTALLS += target

INCLUDEPATH += ..