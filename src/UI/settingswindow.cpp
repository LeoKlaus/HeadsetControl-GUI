#include "settingswindow.h"
#include "ui_settingswindow.h"

#include "utils.h"

#include <QFileDialog>
#include <QStyleHints>

SettingsWindow::SettingsWindow(const Settings &programSettings, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::settingswindow)
{
    setModal(true);
    ui->setupUi(this);

    connect(ui->runonstartupCheckBox, &QCheckBox::clicked, this, &SettingsWindow::setRunOnStartup);
    connect(ui->selectstyleComboBox,
            &QComboBox::currentTextChanged,
            this,
            [this](const QString &text) {
                this->ui->removestylePushButton->setEnabled(text != "Default");
            });
    connect(ui->loadstylePushButton, &QPushButton::clicked, this, &SettingsWindow::saveStyle);
    connect(ui->removestylePushButton, &QPushButton::clicked, this, &SettingsWindow::removeStyle);

    ui->runonstartupCheckBox->setChecked(programSettings.runOnstartup);
    ui->batterylowtresholdSpinBox->setValue(programSettings.batteryLowThreshold);
    ui->updateintervaltimeDoubleSpinBox->setValue((double) programSettings.msecUpdateIntervalTime
                                                  / 1000);
    loadStyles();
    ui->selectstyleComboBox->setCurrentIndex(
        ui->selectstyleComboBox->findText(programSettings.styleName));
}

Settings SettingsWindow::getSettings()
{
    Settings settings;
    settings.runOnstartup = ui->runonstartupCheckBox->isChecked();
    settings.batteryLowThreshold = ui->batterylowtresholdSpinBox->value();
    settings.msecUpdateIntervalTime = ui->updateintervaltimeDoubleSpinBox->value() * 1000;
    settings.styleName = ui->selectstyleComboBox->currentText();

    return settings;
}

void SettingsWindow::setRunOnStartup()
{
    bool enabled = setOSRunOnStartup(ui->runonstartupCheckBox->isChecked());
    ui->runonstartupCheckBox->setChecked(enabled);
}

void SettingsWindow::loadStyles()
{
    QString destination = PROGRAM_STYLES_PATH;
    QDir directory = QDir(destination);
    QStringList list = directory.entryList(QStringList() << "*.qss", QDir::Files);
    ui->selectstyleComboBox->clear();
    ui->selectstyleComboBox->addItem("Default");
    ui->selectstyleComboBox->addItems(list);
}

void SettingsWindow::saveStyle()
{
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setNameFilter(tr("QStyle (*.qss)"));

    QUrl fileUrl = dialog.getOpenFileUrl();
    if (fileUrl.isValid()) {
        QString source = fileUrl.path().removeFirst();
        QString destination = PROGRAM_STYLES_PATH;
        QDir().mkpath(destination);
        destination += "/" + fileUrl.fileName();
        QFile file(destination);
        if (file.exists()) {
            file.remove();
        }
        QFile::copy(source, destination);

        loadStyles();
    }
}

void SettingsWindow::removeStyle()
{
    QString stylePath = PROGRAM_STYLES_PATH + "/" + ui->selectstyleComboBox->currentText();
    QFile file(stylePath);
    if (file.exists()) {
        file.remove();
    }

    loadStyles();
}

SettingsWindow::~SettingsWindow()
{
    delete ui;
}
