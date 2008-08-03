#ifndef LOGS_SETTINGS_WIDGET_H
#define LOGS_SETTINGS_WIDGET_H

#include "settings_widget.h"

#include "ui_logs_settings.h"

class LogsSettingsWidget : public SettingsWidget, private Ui::LogsSettings
{
    Q_OBJECT

public:
    LogsSettingsWidget(QWidget *parent = 0);

    void applyProfile(const Profile &profile);
    void feedProfile(Profile &profile);

private:
    QString chooseDirectory();

private slots:
    void on_toolButtonCustomDirectory_clicked();
    void on_radioButtonCustomDirectory_toggled(bool checked);
};

#endif
