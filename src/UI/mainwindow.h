#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "utils.h"
#include "device.h"
#include "settings.h"
#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QJsonObject>
#include <QJsonArray>
#include <QSlider>
#include <QVersionNumber>
#include <QTimer>

const QVersionNumber GUI_VERSION = QVersionNumber::fromString("0.12.3");

QT_BEGIN_NAMESPACE
namespace Ui {
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    bool notified = false;
    QSystemTrayIcon* tray = new QSystemTrayIcon(this);

private:
    const QString FILE_DEVICES_SETTINGS = "devices.json";
    const QString PROGRAM_SETTINGS_FILENAME = "settings.json";

    int n_connected = 0, n_saved = 0;

    Settings settings;
    bool darkMode;
    QString trayIconPath;

    QMenu *menu;

    QAction* ledOn;
    QAction* ledOff;

    Device* selectedDevice;
    QList<Device*> deviceList;
    QList<QSlider*> slidersEq;

    QTimer* timerGUI = nullptr;

private slots:
    void changeEvent(QEvent *e);

    void bindEvents();
    void RestoreWindowTrigger(QSystemTrayIcon::ActivationReason RW);
    void trayIconActivated(QSystemTrayIcon::ActivationReason reason);

    bool isOsDarkMode();
    void updateIcons();

    void disableFrames();

    void loadDevices();
    void loadDevice(int deviceIndex=0);
    void loadGUIValues();

    void updateDevice();
    void updateGUI();

    void setBatteryStatus();

    void savesettingsButton_clicked();

    //Other Section Events
    void onlightButton_clicked();
    void offlightButton_clicked();

    void sidetoneSlider_sliderReleased();

    void voiceOnButton_clicked();
    void voiceOffButton_clicked();

    void notification0Button_clicked();
    void notification1Button_clicked();

    void inactivitySlider_sliderReleased();

    void rotateOn_clicked();
    void rotateOff_clicked();

    void setChatmixStatus();

    //Equalizer Section Events
    void equalizerPresetcomboBox_currentIndexChanged();

    void applyEqualizer_clicked();
    void setSliders(int value);
    void setSliders(QList<double> values);
    void clearLayout(QLayout* layout);

    void volumelimiterOffButton_clicked();
    void volumelimiterOnButton_clicked();

    //Microphone Section Events
    void muteledbrightnessSlider_sliderReleased();
    void micvolumeSlider_sliderReleased();

    //Bluetooth Section Events
    void btwhenonOffButton_clicked();
    void btwhenonOnButton_clicked();

    void btbothRadioButton_clicked();
    void btpcdbRadioButton_clicked();
    void btonlyRadioButton_clicked();

    //Tool Bar Events
    void editProgramSetting();
    void checkForUpdates();
    void selectDevice();
    void showAbout();
    void showCredits();

private:
    Ui::MainWindow* ui;
};
#endif // MAINWINDOW_H
