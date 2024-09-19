#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "device.h"
#include "dialoginfo.h"
#include "headsetcontrolapi.h"
#include "loaddevicewindow.h"
#include "settingswindow.h"
#include "utils.h"

#include <QFile>
#include <QFileDialog>
#include <QScreen>
#include <QStyleHints>
#include <QtConcurrent/QtConcurrent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , trayIcon(new QSystemTrayIcon(this))
    , trayMenu(new QMenu(this))
    , timerGUI(new QTimer(this))
    , API(HeadsetControlAPI(HEADSETCONTROL_FILE_PATH))
{
    QDir().mkpath(PROGRAM_CONFIG_PATH);
    settings = loadSettingsFromFile(PROGRAM_SETTINGS_FILEPATH);
    defaultStyle = styleSheet();

    setupTrayIcon();
    ui->setupUi(this);
    bindEvents();

    updateIconsTheme();
    updateStyle();

    resetGUI();

    updateGUI();

    connect(&API, &HeadsetControlAPI::actionSuccesful, this, &::MainWindow::saveDevicesSettings);

    connect(timerGUI, &QTimer::timeout, this, &::MainWindow::updateGUI);
    timerGUI->start(settings.msecUpdateIntervalTime);

    //Small trick to make work theme style change (Won't work unless you show window once)
    show();
    hide();
}

void MainWindow::deleteDevices(QList<Device *> deviceList)
{
    for (Device *device : deviceList) {
        delete device;
    }
    deviceList.clear();
}

MainWindow::~MainWindow()
{
    timerGUI->stop();
    delete timerGUI;
    delete trayMenu;
    delete trayIcon;
    delete ui;
}

void MainWindow::changeEvent(QEvent *e)
{
    switch (e->type()) {
    case QEvent::ThemeChange:
        updateIconsTheme();
        break;
    case QEvent::WindowStateChange:
        if (windowState() == Qt::WindowMinimized) {
            hide();
        }
        break;
    default:
        break;
    }

    QMainWindow::changeEvent(e);
}

void MainWindow::bindEvents()
{
    // Tool Bar
    connect(ui->actionSettings, &QAction::triggered, this, &MainWindow::editProgramSetting);
    connect(ui->actionLoad_Device, &QAction::triggered, this, &MainWindow::selectDevice);
    connect(ui->actionCheck_Updates, &QAction::triggered, this, &MainWindow::checkForUpdates);

    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::showAbout);
    connect(ui->actionCredits, &QAction::triggered, this, &MainWindow::showCredits);

    //Error frames
    connect(ui->openfolderPushButton, &QPushButton::clicked, this, [=]() {
        openFileExplorer(PROGRAM_APP_PATH);
    });

    // Other Section
    connect(ui->onlightButton, &QPushButton::clicked, &API, [=]() {
        API.setLights(selectedDevice, true);
    });
    connect(ui->offlightButton, &QPushButton::clicked, &API, [=]() {
        API.setLights(selectedDevice, false);
    });
    connect(ui->sidetoneSlider, &QSlider::sliderReleased, &API, [=]() {
        API.setSidetone(selectedDevice, ui->sidetoneSlider->value());
    });
    connect(ui->voiceOnButton, &QPushButton::clicked, &API, [=]() {
        API.setVoicePrompts(selectedDevice, true);
    });
    connect(ui->voiceOffButton, &QPushButton::clicked, &API, [=]() {
        API.setVoicePrompts(selectedDevice, true);
    });
    connect(ui->notification0Button, &QPushButton::clicked, &API, [=]() {
        API.playNotificationSound(selectedDevice, 0);
    });
    connect(ui->notification1Button, &QPushButton::clicked, &API, [=]() {
        API.playNotificationSound(selectedDevice, 1);
    });
    connect(ui->inactivitySlider, &QSlider::sliderReleased, &API, [=]() {
        API.setInactiveTime(selectedDevice, ui->inactivitySlider->value());
    });

    // Equalizer Section
    connect(ui->equalizerPresetcomboBox,
            &QComboBox::activated,
            this,
            &MainWindow::equalizerPresetChanged);
    connect(ui->applyEqualizer, &QPushButton::clicked, this, &MainWindow::applyEqualizer);
    connect(ui->volumelimiterOffButton, &QPushButton::clicked, &API, [=]() {
        API.setVolumeLimiter(selectedDevice, false);
    });
    connect(ui->volumelimiterOnButton, &QPushButton::clicked, &API, [=]() {
        API.setVolumeLimiter(selectedDevice, true);
    });

    // Microphone Section
    connect(ui->muteledbrightnessSlider, &QSlider::sliderReleased, &API, [=]() {
        API.setMuteLedBrightness(selectedDevice, ui->muteledbrightnessSlider->value());
    });
    connect(ui->micvolumeSlider, &QSlider::sliderReleased, &API, [=]() {
        API.setMicrophoneVolume(selectedDevice, ui->micvolumeSlider->value());
    });
    connect(ui->rotateOn, &QPushButton::clicked, &API, [=]() {
        API.setRotateToMute(selectedDevice, true);
    });
    connect(ui->rotateOff, &QPushButton::clicked, &API, [=]() {
        API.setRotateToMute(selectedDevice, false);
    });

    // Bluetooth Section
    connect(ui->btwhenonOffButton, &QPushButton::clicked, &API, [=]() {
        API.setBluetoothWhenPoweredOn(selectedDevice, false);
    });
    connect(ui->btwhenonOnButton, &QPushButton::clicked, &API, [=]() {
        API.setBluetoothWhenPoweredOn(selectedDevice, true);
    });
    connect(ui->btbothRadioButton, &QRadioButton::clicked, &API, [=]() {
        API.setBluetoothCallVolume(selectedDevice, 0);
    });
    connect(ui->btpcdbRadioButton, &QRadioButton::clicked, &API, [=]() {
        API.setBluetoothCallVolume(selectedDevice, 1);
    });
    connect(ui->btonlyRadioButton, &QRadioButton::clicked, &API, [=]() {
        API.setBluetoothCallVolume(selectedDevice, 2);
    });
}

//Tray Icon Section
void MainWindow::changeTrayIconTo(QString iconName)
{
    trayIconName = iconName;
    trayIcon->setIcon(QIcon::fromTheme(iconName));
}

void MainWindow::setupTrayIcon()
{
    changeTrayIconTo("headphones");
    trayIcon->setToolTip("HeadsetControl");

    trayMenu->addAction(tr("Hide/Show"), this, &MainWindow::toggleWindow);
    ledOn = trayMenu->addAction(tr("Turn Lights On"), &API, [=]() {
        API.setLights(selectedDevice, true);
    });
    ledOff = trayMenu->addAction(tr("Turn Lights Off"), &API, [=]() {
        API.setLights(selectedDevice, false);
    });
    trayMenu->addAction(tr("Exit"), this, &QApplication::quit);

    trayIcon->setContextMenu(trayMenu);
    trayIcon->connect(trayIcon,
                      SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
                      this,
                      SLOT(trayIconActivated(QSystemTrayIcon::ActivationReason)));
    trayIcon->show();
}

void MainWindow::trayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::ActivationReason::Trigger) {
        toggleWindow();
    }
}

//Theme mode Section
bool MainWindow::isAppDarkMode()
{
    Qt::ColorScheme scheme = qApp->styleHints()->colorScheme();
    if (scheme == Qt::ColorScheme::Dark)
        return true;
    return false;
}

void MainWindow::updateIconsTheme()
{
    if (isAppDarkMode()) {
        QIcon::setThemeName("light");
    } else {
        QIcon::setThemeName("dark");
    }
    setWindowIcon(QIcon::fromTheme("headphones"));
    changeTrayIconTo(trayIconName);
}

void MainWindow::updateStyle()
{
    if (settings.styleName != "Default") {
        QString destination = PROGRAM_STYLES_PATH + "/" + settings.styleName;
        QFile file(destination);
        if (file.open(QFile::ReadOnly)) {
            QString styleSheet = QLatin1String(file.readAll());
            setStyleSheet(styleSheet);
        }
    } else {
        setStyleSheet(defaultStyle);
    }
    minimizeWindowSize();
    moveToBottomRight();
}

//Window Position and Size Section
void MainWindow::toggleWindow()
{
    if (isHidden()) {
        show();
        minimizeWindowSize();
        moveToBottomRight();
        if (firstShow) {
            checkForUpdates(firstShow);
            firstShow = false;
        }
    } else {
        hide();
    }
}

void MainWindow::minimizeWindowSize()
{
    resize(sizeHint());
}

void MainWindow::moveToBottomRight()
{
    QScreen *screen = QGuiApplication::primaryScreen();
    QSize screenSize = screen->availableSize();
    QSize finalPosition = screenSize - sizeHint();
    move(finalPosition.width() - 5, finalPosition.height() - 35);
}

void MainWindow::resetGUI()
{
    ledOn->setEnabled(false);
    ledOff->setEnabled(false);

    ui->missingheadsetcontrolFrame->setHidden(false);
    ui->notSupportedFrame->setHidden(false);

    ui->deviceinfoFrame->setHidden(true);
    ui->batteryFrame->setHidden(true);

    ui->tabWidget->hide();
    ui->tabWidget->setTabEnabled(3, false);
    ui->tabWidget->setTabEnabled(2, false);
    ui->tabWidget->setTabEnabled(1, false);
    ui->tabWidget->setTabEnabled(0, false);

    ui->lightFrame->setHidden(true);
    ui->voicepromptFrame->setHidden(true);
    ui->notificationFrame->setHidden(true);
    ui->sidetoneFrame->setHidden(true);
    ui->inactivityFrame->setHidden(true);
    ui->chatmixFrame->setHidden(true);
    ui->volumelimiterFrame->setHidden(true);

    ui->equalizerpresetFrame->setHidden(true);
    ui->equalizerFrame->setHidden(true);
    ui->applyEqualizer->setEnabled(false);
    clearEqualizerSliders(ui->equalizerLayout);

    ui->rotatetomuteFrame->setHidden(true);
    ui->muteledbrightnessFrame->setHidden(true);
    ui->micvolumeFrame->setHidden(true);

    ui->btwhenonFrame->setHidden(true);
    ui->btcallvolumeFrame->setHidden(true);
}

//Devices Managing Section
void MainWindow::loadDevices()
{
    deleteDevices(connectedDevices);
    QList<Device *> saved = getSavedDevices();
    connectedDevices = API.getConnectedDevices();
    updateDevicesFromSource(connectedDevices, saved);

    deleteDevices(saved);
}

void MainWindow::loadDevice(int deviceIndex)
{
    resetGUI();

    if (deviceIndex < 0) {
        selectedDevice = nullptr;
        return;
    }

    selectedDevice = connectedDevices.value(deviceIndex);
    QSet<QString> &capabilities = selectedDevice->capabilities;

    ui->missingheadsetcontrolFrame->setHidden(true);
    ui->notSupportedFrame->setHidden(true);

    qDebug() << selectedDevice->capabilities;

    // Info section
    ui->deviceinfovalueLabel->setText(selectedDevice->device + "<br/>" + selectedDevice->vendor
                                      + "<br/>" + selectedDevice->product);
    ui->deviceinfoFrame->setHidden(false);
    if (capabilities.contains("CAP_BATTERY_STATUS")) {
        ui->batteryFrame->setHidden(false);
        setBatteryStatus();
    }

    ui->tabWidget->show();
    // Other Section
    if (capabilities.contains("CAP_LIGHTS")) {
        ui->lightFrame->setHidden(false);
        ui->tabWidget->setTabEnabled(0, true);
        ledOn->setEnabled(true);
        ledOff->setEnabled(true);
    }
    if (capabilities.contains("CAP_SIDETONE")) {
        ui->sidetoneFrame->setHidden(false);
        ui->tabWidget->setTabEnabled(0, true);
    }
    if (capabilities.contains("CAP_VOICE_PROMPTS")) {
        ui->voicepromptFrame->setHidden(false);
        ui->tabWidget->setTabEnabled(0, true);
    }
    if (capabilities.contains("CAP_NOTIFICATION_SOUND")) {
        ui->notificationFrame->setHidden(false);
        ui->tabWidget->setTabEnabled(0, true);
    }
    if (capabilities.contains("CAP_INACTIVE_TIME")) {
        ui->inactivityFrame->setHidden(false);
        ui->tabWidget->setTabEnabled(0, true);
    }
    if (capabilities.contains("CAP_CHATMIX_STATUS")) {
        ui->chatmixFrame->setHidden(false);
        ui->tabWidget->setTabEnabled(0, true);
        setChatmixStatus();
    }
    // Equalizer Section
    if (capabilities.contains("CAP_EQUALIZER_PRESET") && !selectedDevice->presets_list.empty()) {
        ui->equalizerpresetFrame->setHidden(false);
        ui->tabWidget->setTabEnabled(1, true);
    }
    if (capabilities.contains("CAP_EQUALIZER") && selectedDevice->equalizer.bands_number > 0) {
        ui->equalizerFrame->setHidden(false);
        ui->tabWidget->setTabEnabled(1, true);
    }
    if (capabilities.contains("CAP_VOLUME_LIMITER")) {
        ui->volumelimiterFrame->setHidden(false);
        ui->tabWidget->setTabEnabled(1, true);
    }
    // Microphone Section
    if (capabilities.contains("CAP_ROTATE_TO_MUTE")) {
        ui->rotatetomuteFrame->setHidden(false);
        ui->tabWidget->setTabEnabled(2, true);
    }
    if (capabilities.contains("CAP_MICROPHONE_MUTE_LED_BRIGHTNESS")) {
        ui->muteledbrightnessFrame->setHidden(false);
        ui->tabWidget->setTabEnabled(2, true);
    }
    if (capabilities.contains("CAP_MICROPHONE_VOLUME")) {
        ui->micvolumeFrame->setHidden(false);
        ui->tabWidget->setTabEnabled(2, true);
    }
    // Bluetooth Section
    if (capabilities.contains("CAP_BT_WHEN_POWERED_ON")) {
        ui->btwhenonFrame->setHidden(false);
        ui->tabWidget->setTabEnabled(3, true);
    }
    if (capabilities.contains("CAP_BT_CALL_VOLUME")) {
        ui->btcallvolumeFrame->setHidden(false);
        ui->tabWidget->setTabEnabled(3, true);
    }

    loadGUIValues();
    minimizeWindowSize();
    moveToBottomRight();
}

void MainWindow::loadGUIValues()
{
    if (selectedDevice->lights >= 0) {
        ui->onlightButton->setChecked(selectedDevice->lights);
        ui->offlightButton->setChecked(!selectedDevice->lights);
    }
    if (selectedDevice->sidetone >= 0) {
        ui->sidetoneSlider->setSliderPosition(selectedDevice->sidetone);
    }
    if (selectedDevice->voice_prompts >= 0) {
        ui->voiceOnButton->setChecked(selectedDevice->voice_prompts);
        ui->voiceOffButton->setChecked(!selectedDevice->voice_prompts);
    }
    if (selectedDevice->inactive_time >= 0) {
        ui->inactivitySlider->setSliderPosition(selectedDevice->inactive_time);
    }

    QHBoxLayout *equalizerLayout = ui->equalizerLayout;
    clearEqualizerSliders(equalizerLayout);
    createEqualizerSliders(equalizerLayout);

    ui->equalizerPresetcomboBox->clear();
    for (int i = 0; i < selectedDevice->presets_list.size(); ++i) {
        ui->equalizerPresetcomboBox->addItem(selectedDevice->presets_list.at(i).name);
    }
    ui->equalizerPresetcomboBox->setCurrentIndex(-1);
    if (selectedDevice->equalizer_preset >= 0) {
        ui->equalizerPresetcomboBox->setCurrentIndex(selectedDevice->equalizer_preset);
    } else if (selectedDevice->equalizer_curve.length() == selectedDevice->equalizer.bands_number) {
        setEqualizerSliders(selectedDevice->equalizer_curve);
    }

    if (selectedDevice->volume_limiter >= 0) {
        ui->volumelimiterOnButton->setChecked(selectedDevice->volume_limiter);
        ui->volumelimiterOffButton->setChecked(!selectedDevice->volume_limiter);
    }

    if (selectedDevice->rotate_to_mute >= 0) {
        ui->rotateOn->setChecked(selectedDevice->rotate_to_mute);
        ui->rotateOff->setChecked(!selectedDevice->rotate_to_mute);
    }
    if (selectedDevice->mic_mute_led_brightness >= 0) {
        ui->muteledbrightnessSlider->setSliderPosition(selectedDevice->mic_mute_led_brightness);
    }
    if (selectedDevice->mic_volume >= 0) {
        ui->micvolumeSlider->setSliderPosition(selectedDevice->mic_volume);
    }

    if (selectedDevice->bt_call_volume >= 0) {
        switch (selectedDevice->bt_call_volume) {
        case 0:
            ui->btbothRadioButton->setChecked(true);
            break;
        case 1:
            ui->btpcdbRadioButton->setChecked(true);
            break;
        case 2:
            ui->btonlyRadioButton->setChecked(true);
            break;
        default:
            break;
        }
    }
    if (selectedDevice->bt_when_powered_on >= 0) {
        ui->btwhenonOnButton->setChecked(selectedDevice->bt_when_powered_on);
        ui->btwhenonOffButton->setChecked(!selectedDevice->bt_when_powered_on);
    }
}

void MainWindow::saveDevicesSettings()
{
    QList<Device *> toSave = getSavedDevices();
    updateDevicesFromSource(toSave, connectedDevices);

    serializeDevices(toSave, DEVICES_SETTINGS_FILEPATH);

    deleteDevices(toSave);
}

QList<Device *> MainWindow::getSavedDevices()
{
    return deserializeDevices(DEVICES_SETTINGS_FILEPATH);
}

void MainWindow::updateDevice()
{
    QList<Device *> newDl = API.getConnectedDevices();
    if (!selectedDevice->updateDevice(newDl)) {
        selectedDevice = nullptr;
    }

    deleteDevices(newDl);
}

//Update GUI Section
void MainWindow::updateGUI()
{
    if (!fileExists(HEADSETCONTROL_FILE_PATH)) {
        resetGUI();
        ui->notSupportedFrame->setHidden(true);
        selectedDevice = nullptr;
    } else {
        if (selectedDevice == nullptr) {
            loadDevices();
            if (connectedDevices.isEmpty()) {
                ui->missingheadsetcontrolFrame->setHidden(true);
            } else {
                loadDevice();
            }
        } else {
            updateDevice();
        }
    }
    setBatteryStatus();
    setChatmixStatus();
}

// Info Section Events
void MainWindow::setBatteryStatus()
{
    if (selectedDevice == nullptr) {
        changeTrayIconTo("headphones");
        return;
    }

    QString status = selectedDevice->battery.status;
    int batteryLevel = selectedDevice->battery.level;
    QString level = QString::number(batteryLevel);

    if (batteryLevel >= 0) {
        ui->batteryProgressBar->show();
        ui->batteryProgressBar->setValue(batteryLevel);
    } else {
        ui->batteryProgressBar->hide();
    }

    if (status == "BATTERY_UNAVAILABLE") {
        ui->batteryPercentage->setText(tr("Headset Off"));
        trayIcon->setToolTip(tr("HeadsetControl \r\nHeadset Off"));
        changeTrayIconTo("headphones");
    } else if (status == "BATTERY_CHARGING") {
        ui->batteryPercentage->setText(level + tr("% - Charging"));
        trayIcon->setToolTip(tr("HeadsetControl \r\nBattery Charging"));
        changeTrayIconTo("battery-charging");
    } else if (status == "BATTERY_AVAILABLE") {
        ui->batteryPercentage->setText(level + tr("% - Descharging"));
        trayIcon->setToolTip(tr("HeadsetControl \r\nBattery: ") + level + "%");
        if (level.toInt() > 75) {
            changeTrayIconTo("battery-level-full");
            notified = false;
        } else if (level.toInt() > settings.batteryLowThreshold) {
            changeTrayIconTo("battery-medium");
            notified = false;
        } else {
            changeTrayIconTo("battery-low");
            if (!notified) {
                trayIcon->showMessage(tr("Battery Alert!"),
                                      tr("The battery of your headset is running low"),
                                      QIcon("battery-low"));
                notified = true;
            }
        }
    } else {
        ui->batteryPercentage->setText(tr("No battery info"));
        trayIcon->setToolTip("HeadsetControl");
        changeTrayIconTo("headphones");
    }
}

void MainWindow::setChatmixStatus()
{
    QString chatmixStatus = tr("None");

    if (selectedDevice == nullptr) {
        ui->chatmixvalueLabel->setText(chatmixStatus);
        return;
    }

    int chatmix = selectedDevice->chatmix;
    QString chatmixValue = QString::number(chatmix);
    if (chatmix < 65)
        chatmixStatus = tr("Game");
    else if (chatmix > 65)
        chatmixStatus = tr("Chat");

    ui->chatmixvalueLabel->setText(chatmixValue);
    ui->chatmixstatusLabel->setText(chatmixStatus);
}

// Equalizer Section Events
void MainWindow::equalizerPresetChanged()
{
    int index = ui->equalizerPresetcomboBox->currentIndex();
    setEqualizerSliders(selectedDevice->presets_list.value(index).values);
    API.setEqualizerPreset(selectedDevice, index);
}

void MainWindow::applyEqualizer()
{
    ui->equalizerPresetcomboBox->setCurrentIndex(-1);
    QList<double> values;
    for (QSlider *slider : slidersEq) {
        values.append(slider->value() * selectedDevice->equalizer.band_step);
    }
    API.setEqualizer(selectedDevice, values);
}

//Equalizer Slidesrs Section
void MainWindow::createEqualizerSliders(QHBoxLayout *layout)
{
    if (selectedDevice->equalizer.bands_number > 0) {
        int i;
        for (i = 0; i < selectedDevice->equalizer.bands_number; ++i) {
            QLabel *l = new QLabel(QString::number(i));
            l->setAlignment(Qt::AlignHCenter);

            QSlider *s = new QSlider(Qt::Vertical);
            s->setMaximum(selectedDevice->equalizer.band_max / selectedDevice->equalizer.band_step);
            s->setMinimum(selectedDevice->equalizer.band_min / selectedDevice->equalizer.band_step);
            s->setSingleStep(1);
            s->setTickInterval(1 / selectedDevice->equalizer.band_step);
            s->setTickPosition(QSlider::TicksBothSides);
            if (selectedDevice->equalizer_curve.size() == selectedDevice->equalizer.bands_number) {
                s->setValue(selectedDevice->equalizer_curve.value(i));
            } else {
                s->setValue(selectedDevice->equalizer.band_baseline);
            }

            QVBoxLayout *lb = new QVBoxLayout();
            lb->addWidget(l);
            lb->addWidget(s);

            slidersEq.append(s);
            layout->addLayout(lb);
        }
        ui->applyEqualizer->setEnabled(true);
    }
}

void MainWindow::setEqualizerSliders(double value)
{
    for (QSlider *slider : slidersEq) {
        slider->setValue(value / selectedDevice->equalizer.band_step);
    }
}

void MainWindow::setEqualizerSliders(QList<double> values)
{
    int i = 0;
    if (values.length() == selectedDevice->equalizer.bands_number) {
        for (QSlider *slider : slidersEq) {
            slider->setValue((int) (values[i++] / selectedDevice->equalizer.band_step));
        }
    } else {
        qDebug() << "ERROR: Bad Equalizer Preset";
    }
}

void MainWindow::clearEqualizerSliders(QLayout *layout)
{
    QLayoutItem *item;
    while (!layout->isEmpty()) {
        item = layout->takeAt(0);
        delete item->widget();
        delete item;
        slidersEq.removeFirst();
    }
}

// Tool Bar Events
void MainWindow::selectDevice()
{
    this->loadDevices();

    QStringList devices = QStringList();
    for (Device *device : connectedDevices) {
        if (device->connected) {
            devices << device->device;
        }
    }

    LoaddeviceWindow *loadDevWindow = new LoaddeviceWindow(devices, this);
    if (loadDevWindow->exec() == QDialog::Accepted) {
        int index = loadDevWindow->getDeviceIndex();
        if (index >= 0 && index < devices.length()) {
            if (index == 0) {
                ui->tabWidget->setDisabled(false);
            } else {
                ui->tabWidget->setDisabled(true);
            }
            loadDevice(index);
        }
    }
    delete (loadDevWindow);
}

void MainWindow::editProgramSetting()
{
    SettingsWindow *settingsW = new SettingsWindow(settings, this);
    if (settingsW->exec() == QDialog::Accepted) {
        settings = settingsW->getSettings();
        saveSettingstoFile(settings, PROGRAM_SETTINGS_FILEPATH);
        timerGUI->setInterval(settings.msecUpdateIntervalTime);
        updateStyle();
    }
    delete (settingsW);
}

void MainWindow::checkForUpdates(bool firstStart)
{
    bool needsUpdate = false;

    const QVersionNumber &local_hc = API.getVersion();
    const QVersionNumber local_gui = QVersionNumber::fromString(qApp->applicationVersion());
    QString v1 = getLatestGitHubReleaseVersion("Sapd", "HeadsetControl");
    QString v2 = getLatestGitHubReleaseVersion("LeoKlaus", "HeadsetControl-GUI");
    QVersionNumber remote_hc = QVersionNumber::fromString(v1);
    QVersionNumber remote_gui = QVersionNumber::fromString(v2);
    QString s1 = tr("up-to date v") + local_hc.toString();
    QString s2 = tr("up-to date v") + local_gui.toString();
    if (!(v1 == "") && remote_hc > local_hc) {
        s1 = tr("Newer version")
             + " -> <a "
               "href=\"https://github.com/Sapd/HeadsetControl/releases/latest\">"
             + remote_hc.toString() + "</a>";
        needsUpdate = true;
    }
    if (!(v2 == "") && remote_gui > local_gui) {
        s2 = tr("Newer version")
             + " -> <a "
               "href=\"https://github.com/LeoKlaus/HeadsetControl-GUI/releases/"
               "latest\">"
             + remote_gui.toString() + "</a>";
        needsUpdate = true;
    }

    if ((needsUpdate && firstStart) || !firstStart) {
        DialogInfo *dialogWindow = new DialogInfo(this);
        dialogWindow->setTitle(tr("Check for updates"));
        QString text = "HeadesetControl: " + s1 + "<br/>HeadesetControl-GUI: " + s2;
        dialogWindow->setLabel(text);

        dialogWindow->exec();
        delete (dialogWindow);
    }
}

void MainWindow::showAbout()
{
    DialogInfo *dialogWindow = new DialogInfo(this);
    dialogWindow->setTitle(tr("About this program"));
    QString text = tr("You can find HeadsetControl-GUI source code on "
                      "<a href='https://github.com/LeoKlaus/HeadsetControl-GUI'>GitHub</a>.<br/>"
                      "Made by:<br/>"
                      " - <a href='https://github.com/LeoKlaus'>LeoKlaus</a><br/>"
                      " - <a href='https://github.com/nicola02nb'>nicola02nb</a><br/>"
                      "Version: ")
                   + qApp->applicationVersion();
    dialogWindow->setLabel(text);

    dialogWindow->exec();

    delete (dialogWindow);
}

void MainWindow::showCredits()
{
    DialogInfo *dialogWindow = new DialogInfo(this);
    dialogWindow->setTitle(tr("Credits"));
    QString text = tr("Big shout-out to:<br/>"
                      " - <a href='https://github.com/Sapd'>Sapd</a> for <a "
                      "href='https://github.com/Sapd/HeadsetControl'>HeadsetCoontrol");
    dialogWindow->setLabel(text);

    dialogWindow->exec();

    delete (dialogWindow);
}
