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

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &settingsWindow::updateSettings);
    connect(ui->runonstartupCheckBox, &QCheckBox::checkStateChanged, this, &settingsWindow::setRunOnStartup);
}

void settingsWindow::updateSettings(){
    temporarySettings.runOnstartup=ui->runonstartupCheckBox->isChecked();
    temporarySettings.batteryLowThreshold=ui->batterylowtresholdSpinBox->value();
    temporarySettings.msecUpdateIntervalTime=ui->updateintervaltimeDoubleSpinBox->value()*1000;
}

void settingsWindow::setRunOnStartup(){
    setOSRunOnStartup(ui->runonstartupCheckBox->isChecked());
}

settingsWindow::~settingsWindow()
{
    delete ui;
}
