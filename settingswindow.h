#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include "settings.h"
#include <QDialog>

namespace Ui {
class settings;
}

class settingsWindow : public QDialog
{
    Q_OBJECT

public:
    explicit settingsWindow(const Settings& programSettings, QWidget *parent = nullptr);
    ~settingsWindow();

    Settings temporarySettings;

private:
    Ui::settings *ui;

    void updateSettings();
    void setRunOnStartup();
};

#endif // SETTINGSWINDOW_H
