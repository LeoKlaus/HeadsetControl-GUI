#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QDialog>
#include "settings.h"

namespace Ui {
class settingswindow;
}

class SettingsWindow : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsWindow(const Settings &programSettings, QWidget *parent = nullptr);
    ~SettingsWindow();

    Settings getSettings();

private:
    Ui::settingswindow *ui;

    void setRunOnStartup();
};

#endif // SETTINGSWINDOW_H
