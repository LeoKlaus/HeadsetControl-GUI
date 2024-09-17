#ifndef HEADSETCONTROLAPI_H
#define HEADSETCONTROLAPI_H

#include "device.h"

#include <QFile>
#include <QObject>
#include <QVersionNumber>

class Action
{
public:
    bool success;
    QString capability;
    QString device;
    QString status;
    QString error_message;
};

class HeadsetControlAPI : public QObject
{
    Q_OBJECT

public:
    HeadsetControlAPI(QString headsetcontrolFilePath);

    QString getName();
    QVersionNumber getVersion();
    QVersionNumber getApiVersion();
    QVersionNumber getHidApiVersion();

    QList<Device *> getConnectedDevices();

private:
    QString headsetcontrolFilePath;
    QFile headsetcontrol;

    QString name;
    QVersionNumber version;
    QVersionNumber api_version;
    QVersionNumber hidapi_version;

    QString sendCommand(const QStringList &args_list);
    Action sendAction(const QStringList &args_list);

public slots:
    void setSidetone(Device *device, int level);
    void setLights(Device *device, bool enabled);
    void setVoicePrompts(Device *device, bool enabled);
    void setInactiveTime(Device *device, int time);
    void playNotificationSound(Device *device, int id);
    void setVolumeLimiter(Device *device, bool enabled);
    void setEqualizer(Device *device, QList<double> equalizerValues);
    void setEqualizerPreset(Device *device, int number);

    void setRotateToMute(Device *device, bool enabled);
    void setMuteLedBrightness(Device *device, int brightness);
    void setMicrophoneVolume(Device *device, int volume);

    void setBluetoothWhenPoweredOn(Device *device, bool enabled);
    void setBluetoothCallVolume(Device *device, int option);

signals:
    void actionSuccesful();
};

#endif // HEADSETCONTROLAPI_H
