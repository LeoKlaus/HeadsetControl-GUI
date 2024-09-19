#include "headsetcontrolapi.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QProcess>

HeadsetControlAPI::HeadsetControlAPI(QString headsetcontrolFilePath)
{
    this->headsetcontrolFilePath = headsetcontrolFilePath;
    sendCommand(QStringList());
}

QString HeadsetControlAPI::getName()
{
    return name;
}

QVersionNumber HeadsetControlAPI::getVersion()
{
    return version;
}

QVersionNumber HeadsetControlAPI::getApiVersion()
{
    return api_version;
}

QVersionNumber HeadsetControlAPI::getHidApiVersion()
{
    return hidapi_version;
}

QList<Device *> HeadsetControlAPI::getConnectedDevices()
{
    QStringList args = QStringList() << QString("--output") << QString("JSON");
    QString output = sendCommand(args);
    QJsonDocument jsonDoc = QJsonDocument::fromJson(output.toUtf8());
    QJsonObject jsonInfo = jsonDoc.object();

    name = jsonInfo["name"].toString();
    version = QVersionNumber::fromString(jsonInfo["version"].toString());
    api_version = QVersionNumber::fromString(jsonInfo["api_version"].toString());
    hidapi_version = QVersionNumber::fromString(jsonInfo["hidapi_version"].toString());

    int device_number = jsonInfo["device_count"].toInt();
    qDebug() << "Found" << device_number << "devices:";

    QList<Device *> devices;
    QJsonArray jsonDevices = jsonInfo["devices"].toArray();
    if (!jsonDoc.isNull()) {
        for (int i = 0; i < device_number; ++i) {
            Device *device = new Device(jsonDevices[i].toObject(), output);
            devices.append(device);
            qDebug() << "\t" << device->device;
        }
    }

    return devices;
}

// HC rleated functions
QString HeadsetControlAPI::sendCommand(const QStringList &args_list)
{
    QProcess *proc = new QProcess();
    QStringList args = QStringList() << QString("--output") << QString("JSON");
    //args << QString("--test-device"); //Uncomment this to enable all "modules"
    args << args_list;

    proc->start(headsetcontrolFilePath, args);
    proc->waitForFinished();
    QString output = proc->readAllStandardOutput();
    qDebug() << "Command: \t" << headsetcontrolFilePath;
    qDebug() << "\tArgs: \theadsetcontrol " << args;
    // qDebug() << output;
    qDebug() << "Error: \t" << proc->error();

    delete (proc);

    return output;
}

Action HeadsetControlAPI::sendAction(const QStringList &args_list)
{
    QString output = sendCommand(args_list);
    QJsonDocument jsonDoc = QJsonDocument::fromJson(output.toUtf8());
    QJsonObject jsonInfo = jsonDoc.object();
    QJsonArray actions = jsonInfo["actions"].toArray();
    Action action;
    if (!actions.isEmpty()) {
        QJsonObject jaction = actions[0].toObject();

        action.device = jaction["device"].toString();
        action.capability = jaction["capability"].toString();
        action.status = jaction["status"].toString();
        action.error_message = jaction["error_message"].toString();

        action.success = action.status == "success";

        qDebug() << "Device:\t" << action.device;
        qDebug() << "Capability:" << action.capability;
        qDebug() << "Status:\t" << action.status;
        if (!action.success) {
            qDebug() << "Error:\t" << action.error_message;
        }
    }

    return action;
}

void HeadsetControlAPI::setSidetone(Device *device, int level)
{
    QStringList args = QStringList() << QString("--sidetone") << QString::number(level);
    Action a = sendAction(args);
    if (a.success) {
        device->sidetone = level;
        emit actionSuccesful();
    }
}

void HeadsetControlAPI::setLights(Device *device, bool enabled)
{
    QStringList args = QStringList() << QString("--light") << QString::number(enabled);
    Action a = sendAction(args);
    if (a.success) {
        device->lights = enabled;
        emit actionSuccesful();
    }
}

void HeadsetControlAPI::setVoicePrompts(Device *device, bool enabled)
{
    QStringList args = QStringList() << QString("--voice-prompt") << QString::number(enabled);
    Action a = sendAction(args);
    if (a.success) {
        device->voice_prompts = enabled;
        emit actionSuccesful();
    }
}

void HeadsetControlAPI::setInactiveTime(Device *device, int time)
{
    QStringList args = QStringList() << QString("--inactive-time") << QString::number(time);
    Action a = sendAction(args);
    if (a.success) {
        device->inactive_time = time;
        emit actionSuccesful();
    }
}

void HeadsetControlAPI::playNotificationSound(Device *device, int id)
{
    QStringList args = QStringList() << QString("--notificate") << QString::number(id);
    Action a = sendAction(args);
    if (a.success) {
        device->notification_sound = id;
        emit actionSuccesful();
    }
}

void HeadsetControlAPI::setVolumeLimiter(Device *device, bool enabled)
{
    QStringList args = QStringList() << QString("--volume-limiter") << QString::number(enabled);
    Action a = sendAction(args);
    if (a.success) {
        device->volume_limiter = enabled;
        emit actionSuccesful();
    }
}

void HeadsetControlAPI::setEqualizer(Device *device, QList<double> equalizerValues)
{
    QString equalizer = "";
    for (double value : equalizerValues) {
        equalizer += QString::number(value) + ",";
    }
    equalizer.removeLast();
    QStringList args = QStringList() << QString("--equalizer") << equalizer;
    Action a = sendAction(args);
    if (a.success) {
        device->equalizer_curve = equalizerValues;
        device->equalizer_preset = -1;
        emit actionSuccesful();
    }
}

void HeadsetControlAPI::setEqualizerPreset(Device *device, int number)
{
    QStringList args = QStringList() << QString("--equalizer-preset") << QString::number(number);
    Action a = sendAction(args);
    if (a.success) {
        device->equalizer_preset = number;
        emit actionSuccesful();
    }
}

void HeadsetControlAPI::setRotateToMute(Device *device, bool enabled)
{
    QStringList args = QStringList() << QString("--rotate-to-mute") << QString::number(enabled);
    Action a = sendAction(args);
    if (a.success) {
        device->rotate_to_mute = enabled;
        emit actionSuccesful();
    }
}

void HeadsetControlAPI::setMuteLedBrightness(Device *device, int brightness)
{
    QStringList args = QStringList() << QString("--microphone-mute-led-brightness")
                                     << QString::number(brightness);
    Action a = sendAction(args);
    if (a.success) {
        device->mic_mute_led_brightness = brightness;
        emit actionSuccesful();
    }
}

void HeadsetControlAPI::setMicrophoneVolume(Device *device, int volume)
{
    QStringList args = QStringList() << QString("--microphone-volume") << QString::number(volume);
    Action a = sendAction(args);
    if (a.success) {
        device->mic_volume = volume;
        emit actionSuccesful();
    }
}

void HeadsetControlAPI::setBluetoothWhenPoweredOn(Device *device, bool enabled)
{
    QStringList args = QStringList() << QString("--bt-when-powered-on") << QString::number(enabled);
    Action a = sendAction(args);
    if (a.success) {
        device->bt_when_powered_on = enabled;
        emit actionSuccesful();
    }
}

void HeadsetControlAPI::setBluetoothCallVolume(Device *device, int option)
{
    QStringList args = QStringList() << QString("--bt-call-volume") << QString::number(option);
    Action a = sendAction(args);
    if (a.success) {
        device->bt_call_volume = option;
        emit actionSuccesful();
    }
}
