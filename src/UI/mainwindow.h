#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QHBoxLayout>
#include <QJsonArray>
#include <QJsonObject>
#include <QMainWindow>
#include <QSlider>
#include <QSystemTrayIcon>
#include <QTimer>
#include <QVersionNumber>
#include "device.h"
#include "settings.h"

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

private:
    const QString FILE_DEVICES_SETTINGS = "devices.json";
    const QString PROGRAM_SETTINGS_FILENAME = "settings.json";
#ifdef Q_OS_WIN
    const QString HEADSETCONTROL = "headsetcontrol.exe";
#elif
    const QString HEADSETCONTROL = "headsetcontrol";
#endif
    bool firstShow = true;
    bool notified = false;
    bool savedDevices = true;

    QSystemTrayIcon *tray = new QSystemTrayIcon(this);
    QString trayIconPath;
    QMenu *trayMenu;
    QAction *ledOn;
    QAction *ledOff;

    Settings settings;

    QTimer *timerGUI = nullptr;

    int n_connected = 0, n_saved = 0;

    Device *selectedDevice;
    QList<Device *> deviceList;
    QList<QSlider *> slidersEq;

    void bindEvents();

    //Tray Icon Section
    void createTrayIcon();

    //Theme mode Section
    bool isAppDarkMode();
    void updateIconsTheme();

    void resetGUI();

    //Window Position and Size Section
    void minimizeWindowSize();
    void moveToBottomRight();
    void toggleWindow();

    //Devices Managing Section
    void updateDevice();
    void loadDevice(int deviceIndex = 0);
    void loadDevices();
    void loadGUIValues();

    // Info Section Events
    void setBatteryStatus();

    //Equalizer Slidesrs Section
    void createEqualizerSliders(QHBoxLayout *layout);
    void setEqualizerSliders(double value);
    void setEqualizerSliders(QList<double> values);
    void clearEqualizerSliders(QLayout *layout);

private slots:
    void changeEvent(QEvent *e);

    //Tray Icon Section
    void trayIconActivated(QSystemTrayIcon::ActivationReason reason);

    //Devices Managing Section
    void saveDevicesSettings();
    QList<Device *> getSavedDevices();

    //Update GUI Section
    void updateGUI();

    // Other Section Events
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

    // Equalizer Section Events
    void equalizerPresetcomboBox_currentIndexChanged();
    void applyEqualizer_clicked();

    void volumelimiterOffButton_clicked();
    void volumelimiterOnButton_clicked();

    // Microphone Section Events
    void muteledbrightnessSlider_sliderReleased();
    void micvolumeSlider_sliderReleased();

    // Bluetooth Section Events
    void btwhenonOffButton_clicked();
    void btwhenonOnButton_clicked();

    void btbothRadioButton_clicked();
    void btpcdbRadioButton_clicked();
    void btonlyRadioButton_clicked();

    // Tool Bar Events
    void selectDevice();
    void editProgramSetting();
    void checkForUpdates(bool firstStart = false);
    void showAbout();
    void showCredits();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
