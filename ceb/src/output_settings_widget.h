#ifndef OUTPUT_SETTINGS_WIDGET_H
#define OUTPUT_SETTINGS_WIDGET_H

#include "settings_widget.h"

#include "ui_output_settings.h"

class OutputSettingsWidget : public SettingsWidget, private Ui::OutputSettings
{
    Q_OBJECT

public:
    OutputSettingsWidget(QWidget *parent = 0);

    void applyProfile(const Profile &profile);
    void feedProfile(Profile &profile);

private slots:
    void refreshAwaySeparatorPreview();
    void on_pushButtonIdleColor_clicked();
};

#endif
