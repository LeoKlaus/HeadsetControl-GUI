#include "settingswindow.h"
#include "ui_settingswindow.h"
#include "utils.h"


SettingsWindow::SettingsWindow(const Settings& programSettings, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::settingswindow)
{
    setModal(true);
    ui->setupUi(this);

    connect(ui->runonstartupCheckBox, &QCheckBox::clicked, this, &SettingsWindow::setRunOnStartup);

    ui->runonstartupCheckBox->setChecked(programSettings.runOnstartup);
    ui->batterylowtresholdSpinBox->setValue(programSettings.batteryLowThreshold);
    ui->updateintervaltimeDoubleSpinBox->setValue((double)programSettings.msecUpdateIntervalTime/1000);
}

Settings SettingsWindow::getSettings(){
    Settings settings;
    settings.runOnstartup=ui->runonstartupCheckBox->isChecked();
    settings.batteryLowThreshold=ui->batterylowtresholdSpinBox->value();
    settings.msecUpdateIntervalTime=ui->updateintervaltimeDoubleSpinBox->value()*1000;

    return settings;
}

void SettingsWindow::setRunOnStartup(){
    bool enabled = setOSRunOnStartup(ui->runonstartupCheckBox->isChecked());
    ui->runonstartupCheckBox->setChecked(enabled);
}

SettingsWindow::~SettingsWindow()
{
    delete ui;
}
