#include "settingswindow.h"
#include "ui_settingswindow.h"
#include "settings.h"
#include "utils.h"

settingsWindow::settingsWindow(const Settings& programSettings, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::settings)
{
    setModal(true);
    ui->setupUi(this);

    ui->runonstartupCheckBox->setChecked(programSettings.runOnstartup);
    ui->batterylowtresholdSpinBox->setValue(programSettings.batteryLowThreshold);
    ui->updateintervaltimeDoubleSpinBox->setValue((double)programSettings.msecUpdateIntervalTime/1000);
}

Settings settingsWindow::getSettings(){
    Settings settings;
    settings.runOnstartup=ui->runonstartupCheckBox->isChecked();
    settings.batteryLowThreshold=ui->batterylowtresholdSpinBox->value();
    settings.msecUpdateIntervalTime=ui->updateintervaltimeDoubleSpinBox->value()*1000;

    return settings;
}

void settingsWindow::setRunOnStartup(){
    setOSRunOnStartup(ui->runonstartupCheckBox->isChecked());
}

settingsWindow::~settingsWindow()
{
    delete ui;
}
