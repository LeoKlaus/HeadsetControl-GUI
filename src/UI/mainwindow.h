#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "device.h"
#include "headsetcontrolapi.h"
#include "settings.h"

#include <QHBoxLayout>
#include <QJsonArray>
#include <QJsonObject>
#include <QMainWindow>
#include <QSlider>
#include <QStandardPaths>
#include <QSystemTrayIcon>
#include <QTimer>
#include <QVersionNumber>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    const QString PROGRAM_APP_PATH = qApp->applicationDirPath();
#ifdef Q_OS_WIN
    const QString HEADSETCONTROL_FILE_PATH = PROGRAM_APP_PATH + "/headsetcontrol.exe";
#else
    const QString HEADSETCONTROL_FILE_PATH = PROGRAM_APP_PATH + "/headsetcontrol";
#endif

    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    bool firstShow = true;
    bool notified = false;

    QString defaultStyle;

    Ui::MainWindow *ui;
    QSystemTrayIcon *trayIcon;
    QString trayIconName;
    QMenu *trayMenu;
    QAction *ledOn;
    QAction *ledOff;
    QTimer *timerGUI;

    Settings settings;

    int n_connected = 0, n_saved = 0;

    HeadsetControlAPI API;
    Device *selectedDevice = nullptr;
    QList<Device *> connectedDevices;

    QList<QSlider *> slidersEq;

    void deleteDevices(QList<Device *> deviceList);

    void bindEvents();

    //Tray Icon Section
    void changeTrayIconTo(QString iconName);
    void setupTrayIcon();

    //Theme mode Section
    bool isAppDarkMode();
    void updateIconsTheme();
    void updateStyle();

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
    QList<Device *> getSavedDevices();

    // Info Section Events
    void setBatteryStatus();
    void setChatmixStatus();

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

    //Update GUI Section
    void updateGUI();

    // Equalizer Section Events
    void equalizerPresetChanged();
    void applyEqualizer();

    // Tool Bar Events
    void selectDevice();
    void editProgramSetting();
    void checkForUpdates(bool firstStart = false);
    void showAbout();
    void showCredits();
};
#endif // MAINWINDOW_H
