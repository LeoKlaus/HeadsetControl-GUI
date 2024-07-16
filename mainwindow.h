#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "tools.h"
#include "Device.h"
#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QJsonObject>
#include <QJsonArray>
#include <QSlider>
#include <QVersionNumber>

const QVersionNumber GUI_VERSION = QVersionNumber::fromString("0.11.0");

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
    const int UPDATE_TIME=30000;

    bool darkMode;
    QString trayIconPath;

    QMenu *menu;

    QAction* ledOn;
    QAction* ledOff;

    Device* selectedDevice;
    QList<Device*> deviceList;
    QList<QSlider*> slidersEq;

    QTimer* timerGUI;

    QList<int> flat={0,0,0,0,0,0,0,0,0,0};

private slots:
    void changeEvent(QEvent *e);

    void RestoreWindowTrigger(QSystemTrayIcon::ActivationReason RW);

    void trayIconActivated(QSystemTrayIcon::ActivationReason reason);

    bool isOsDarkMode();

    void disableFrames();

    void loadDevices();

    void loadGUIValues();

    void updateDevice();

    void updateIcons();

    void updateGUI();

    void loadDevice(int deviceIndex=0);

    void setBatteryStatus();

    void on_savesettingsButton_clicked();

    //Other Section Events
    void on_onlightButton_clicked();

    void on_offlightButton_clicked();

    void on_sidetoneSlider_sliderReleased();

    void on_voiceOnButton_clicked();

    void on_voiceOffButton_clicked();

    void on_notification0Button_clicked();

    void on_notification1Button_clicked();

    void on_inactivitySlider_sliderReleased();

    void on_rotateOn_clicked();

    void on_rotateOff_clicked();

    void setChatmixStatus();

    //Equalizer Section Events
    void on_equalizerPresetcomboBox_currentIndexChanged();

    void on_applyEqualizer_clicked();

    void setSliders(int value);

    void setSliders(QList<double> values);

    void clearLayout(QLayout* layout);

    void on_volumelimiterOffButton_clicked();

    void on_volumelimiterOnButton_clicked();

    //Microphone Section Events
    void on_muteledbrightnessSlider_sliderReleased();

    void on_micvolumeSlider_sliderReleased();

    //Bluetooth Section Events
    void on_btwhenonOffButton_clicked();

    void on_btwhenonOnButton_clicked();

    void on_btbothRadioButton_clicked();

    void on_btpcdbRadioButton_clicked();

    void on_btonlyRadioButton_clicked();

    //Tool Bar Events
    void checkForUpdates();

    void selectDevice();

    void showAbout();

    void showCredits();

private:
    Ui::MainWindow* ui;
};
#endif // MAINWINDOW_H
