#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ghTools.h"
#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QJsonObject>
#include <QJsonArray>
#include <QSlider>
#include <QVersionNumber>

const QVersionNumber GUI_VERSION = QVersionNumber::fromString("0.2.0");

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    bool notified = false;
    QSystemTrayIcon *tray = new QSystemTrayIcon(this);

private:
    QMenu *menu;
    Ui::MainWindow *uix;

    QJsonObject jsonInfo;
    QJsonArray deviceList;
    QJsonObject usingDevice;

    QSet<QString> capabilities;
    QVector<QSlider*> slidersEq;

    QVector<int> flat={0,0,0,0,0,0,0,0,0,0};

private slots:
    void changeEvent(QEvent *e);

    void RestoreWindowTrigger(QSystemTrayIcon::ActivationReason RW);

    void checkForUpdates();

    QString sendCommand(QStringList args);

    void loadDevices();

    void disableFrames();

    void loadFeatures(int deviceIndex=0);

    void on_onButton_clicked();

    void on_offButton_clicked();

    void on_voiceOnButton_clicked();

    void on_voiceOffButton_clicked();

    void on_rotateOn_clicked();

    void on_rotateOff_clicked();

    void setBatteryStatus();

    void on_sidetoneSlider_valueChanged();

    void on_inactivitySlider_valueChanged();

    void setChatmixStatus();

    void on_equalizerPresetcomboBox_currentIndexChanged();

    void on_applyEqualizer_clicked();

    void setSliders(QVector<int> values);

    void on_muteledbrightnessSlider_valueChanged();

    void on_micvolumeSlider_valueChanged();

    void showDialog(QString title, QLayout* layout);

    void showAbout();

    void showCredits();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
