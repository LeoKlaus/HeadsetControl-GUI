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
    QString getVersion();
    QString getApiVersion();
    QString getHidApiVersion();

    int getSelectedDevice();
    void setSelectedDevice(const int &deviceIndex);

    Device *getDevice();
    int getDeviceIndex(const QString &vendorID, const QString &productID);
    QList<Device *> getConnectedDevices();

private:
    QString headsetcontrolFilePath;
    QFile headsetcontrol;

    int selectedDevice = 0;
    QString name;
    QString version;
    QString api_version;
    QString hidapi_version;

    QString sendCommand(const QStringList &args_list);
    QJsonObject parseOutput(const QString &output);
    Action sendAction(const QStringList &args_list);

public slots:
    void setSidetone(Device *device, int level, bool emitSignal = true);
    void setLights(Device *device, bool enabled, bool emitSignal = true);
    void setVoicePrompts(Device *device, bool enabled, bool emitSignal = true);
    void setInactiveTime(Device *device, int time, bool emitSignal = true);
    void playNotificationSound(Device *device, int id, bool emitSignal = true);
    void setVolumeLimiter(Device *device, bool enabled, bool emitSignal = true);
    void setEqualizer(Device *device, QList<double> equalizerValues, bool emitSignal = true);
    void setEqualizerPreset(Device *device, int number, bool emitSignal = true);

    void setRotateToMute(Device *device, bool enabled, bool emitSignal = true);
    void setMuteLedBrightness(Device *device, int brightness, bool emitSignal = true);
    void setMicrophoneVolume(Device *device, int volume, bool emitSignal = true);

    void setBluetoothWhenPoweredOn(Device *device, bool enabled, bool emitSignal = true);
    void setBluetoothCallVolume(Device *device, int option, bool emitSignal = true);

signals:
    void actionSuccesful();
};

#endif // HEADSETCONTROLAPI_H
