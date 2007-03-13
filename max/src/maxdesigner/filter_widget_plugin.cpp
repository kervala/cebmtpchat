#include "../filter_widget.h"
#include "filter_widget_plugin.h"

#include <QtPlugin>

FilterWidgetPlugin::FilterWidgetPlugin(QObject *parent)
    : QObject(parent)
{
    initialized = false;
}

void FilterWidgetPlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
    if (initialized)
        return;

    initialized = true;
}

bool FilterWidgetPlugin::isInitialized() const
{
    return initialized;
}

QWidget *FilterWidgetPlugin::createWidget(QWidget *parent)
{
    return new FilterWidget(parent);
}

QString FilterWidgetPlugin::name() const
{
    return "FilterWidget";
}

QString FilterWidgetPlugin::group() const
{
    return "Max Widgets";
}

QIcon FilterWidgetPlugin::icon() const
{
    return QIcon();
}

QString FilterWidgetPlugin::toolTip() const
{
    return "";
}

QString FilterWidgetPlugin::whatsThis() const
{
    return "";
}

bool FilterWidgetPlugin::isContainer() const
{
    return false;
}

QString FilterWidgetPlugin::domXml() const
{
    return "<widget class=\"FilterWidget\" name=\"filterWidget\">\n"
           " <property name=\"geometry\">\n"
           "  <rect>\n"
           "   <x>0</x>\n"
           "   <y>0</y>\n"
           "   <width>200</width>\n"
           "   <height>30</height>\n"
           "  </rect>\n"
           " </property>\n"
           " <property name=\"toolTip\" >\n"
           "  <string>A filter</string>\n"
           " </property>\n"
           " <property name=\"whatsThis\" >\n"
           "  <string>The filter widget is used to filter datas "
           "in a tree.</string>\n"
           " </property>\n"
           "</widget>\n";
}

QString FilterWidgetPlugin::includeFile() const
{
    return "filter_widget.h";
}

Q_EXPORT_PLUGIN2(customwidgetplugin, FilterWidgetPlugin)
