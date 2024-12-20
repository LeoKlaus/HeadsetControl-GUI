#include "headsetcontrolapi.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QProcess>

HeadsetControlAPI::HeadsetControlAPI(QString headsetcontrolFilePath)
{
    this->headsetcontrolFilePath = headsetcontrolFilePath;
    parseOutput(sendCommand(QStringList()));
}

int HeadsetControlAPI::getSelectedDevice()
{
    return selectedDevice;
}

void HeadsetControlAPI::setSelectedDevice(const int &deviceIndex)
{
    if (deviceIndex >= 0)
        selectedDevice = deviceIndex;
    else
        selectedDevice = 0;
}

QString HeadsetControlAPI::getName()
{
    return name;
}

QString HeadsetControlAPI::getVersion()
{
    return version;
}

QString HeadsetControlAPI::getApiVersion()
{
    return api_version;
}

QString HeadsetControlAPI::getHidApiVersion()
{
    return hidapi_version;
}

Device *HeadsetControlAPI::getDevice()
{
    QList<Device *> connectedList = getConnectedDevices();
    Device *connectedDevice = connectedList.value(selectedDevice);
    Device *device = nullptr;
    if (connectedDevice != nullptr) {
        device = new Device();
        *device = *connectedDevice;
    }
    deleteDevices(connectedList);

    return device;
}

int HeadsetControlAPI::getDeviceIndex(
    const QString &vendorID, const QString &productID)
{
    QList<Device *> connectedDevices = getConnectedDevices();
    int i = 0;
    foreach (Device *device, connectedDevices) {
        if (device->id_vendor == vendorID && device->id_product == productID) {
            deleteDevices(connectedDevices);
            return i;
        }
        i++;
    }
    deleteDevices(connectedDevices);

    return 0;
}

QList<Device *> HeadsetControlAPI::getConnectedDevices()
{
    QStringList args = QStringList();
    QString output = sendCommand(args);
    QJsonObject jsonInfo = parseOutput(output);

    int device_number = jsonInfo["device_count"].toInt();
    qDebug() << "Found" << device_number << "devices:";

    QList<Device *> devices;
    QJsonArray jsonDevices = jsonInfo["devices"].toArray();
    for (int i = 0; i < device_number; ++i) {
        Device *device = new Device(jsonDevices[i].toObject(), output);
        devices.append(device);
        qDebug() << "[" + QString::number(i) + "] " + device->device;
    }
    qDebug();

    return devices;
}

// HC rleated functions
QString HeadsetControlAPI::sendCommand(const QStringList &args_list)
{
    QProcess *proc = new QProcess();
    QStringList args = QStringList();
    args << QString("--output") << QString("JSON");
    args << QString("--device") << QString::number(selectedDevice);
    //args << QString("--test-device"); //Uncomment this to enable all "modules"
    args << args_list;

    proc->start(headsetcontrolFilePath, args);
    proc->waitForFinished();
    QString output = proc->readAllStandardOutput();
    qDebug() << "Command:\t" << headsetcontrolFilePath;
    qDebug() << "Args:\t" << args;
    qDebug() << "Error:\t" << proc->exitStatus();
    //qDebug() << "Output:" << output;
    qDebug();

    delete (proc);

    return output;
}

QJsonObject HeadsetControlAPI::parseOutput(
    const QString &output)
{
    QJsonDocument jsonDoc = QJsonDocument::fromJson(output.toUtf8());
    QJsonObject jsonInfo = jsonDoc.object();

    name = jsonInfo["name"].toString();
    version = jsonInfo["version"].toString();
    api_version = jsonInfo["api_version"].toString();
    hidapi_version = jsonInfo["hidapi_version"].toString();

    return jsonInfo;
}

Action HeadsetControlAPI::sendAction(const QStringList &args_list)
{
    QString output = sendCommand(args_list);
    QJsonObject jsonInfo = parseOutput(output);
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
        qDebug();
    }

    return action;
}

void HeadsetControlAPI::setSidetone(
    Device *device, int level, bool emitSignal)
{
    QStringList args = QStringList() << QString("--sidetone") << QString::number(level);
    Action a = sendAction(args);
    if (emitSignal && a.success) {
        device->sidetone = level;
        emit actionSuccesful();
    }
}

void HeadsetControlAPI::setLights(
    Device *device, bool enabled, bool emitSignal)
{
    QStringList args = QStringList() << QString("--light") << QString::number(enabled);
    Action a = sendAction(args);
    if (emitSignal && a.success) {
        device->lights = enabled;
        emit actionSuccesful();
    }
}

void HeadsetControlAPI::setVoicePrompts(
    Device *device, bool enabled, bool emitSignal)
{
    QStringList args = QStringList() << QString("--voice-prompt") << QString::number(enabled);
    Action a = sendAction(args);
    if (emitSignal && a.success) {
        device->voice_prompts = enabled;
        emit actionSuccesful();
    }
}

void HeadsetControlAPI::setInactiveTime(
    Device *device, int time, bool emitSignal)
{
    QStringList args = QStringList() << QString("--inactive-time") << QString::number(time);
    Action a = sendAction(args);
    if (emitSignal && a.success) {
        device->inactive_time = time;
        emit actionSuccesful();
    }
}

void HeadsetControlAPI::playNotificationSound(
    Device *device, int id, bool emitSignal)
{
    QStringList args = QStringList() << QString("--notificate") << QString::number(id);
    Action a = sendAction(args);
    if (emitSignal && a.success) {
        device->notification_sound = id;
        emit actionSuccesful();
    }
}

void HeadsetControlAPI::setVolumeLimiter(
    Device *device, bool enabled, bool emitSignal)
{
    QStringList args = QStringList() << QString("--volume-limiter") << QString::number(enabled);
    Action a = sendAction(args);
    if (emitSignal && a.success) {
        device->volume_limiter = enabled;
        emit actionSuccesful();
    }
}

void HeadsetControlAPI::setEqualizer(
    Device *device, QList<double> equalizerValues, bool emitSignal)
{
    QString equalizer = "";
    for (double value : equalizerValues) {
        equalizer += QString::number(value) + ",";
    }
    equalizer.removeLast();
    QStringList args = QStringList() << QString("--equalizer") << equalizer;
    Action a = sendAction(args);
    if (emitSignal && a.success) {
        device->equalizer_curve = equalizerValues;
        device->equalizer_preset = -1;
        emit actionSuccesful();
    }
}

void HeadsetControlAPI::setEqualizerPreset(
    Device *device, int number, bool emitSignal)
{
    QStringList args = QStringList() << QString("--equalizer-preset") << QString::number(number);
    Action a = sendAction(args);
    if (emitSignal && a.success) {
        device->equalizer_preset = number;
        emit actionSuccesful();
    }
}

void HeadsetControlAPI::setRotateToMute(
    Device *device, bool enabled, bool emitSignal)
{
    QStringList args = QStringList() << QString("--rotate-to-mute") << QString::number(enabled);
    Action a = sendAction(args);
    if (emitSignal && a.success) {
        device->rotate_to_mute = enabled;
        emit actionSuccesful();
    }
}

void HeadsetControlAPI::setMuteLedBrightness(
    Device *device, int brightness, bool emitSignal)
{
    QStringList args = QStringList() << QString("--microphone-mute-led-brightness")
                                     << QString::number(brightness);
    Action a = sendAction(args);
    if (emitSignal && a.success) {
        device->mic_mute_led_brightness = brightness;
        emit actionSuccesful();
    }
}

void HeadsetControlAPI::setMicrophoneVolume(
    Device *device, int volume, bool emitSignal)
{
    QStringList args = QStringList() << QString("--microphone-volume") << QString::number(volume);
    Action a = sendAction(args);
    if (emitSignal && a.success) {
        device->mic_volume = volume;
        emit actionSuccesful();
    }
}

void HeadsetControlAPI::setBluetoothWhenPoweredOn(
    Device *device, bool enabled, bool emitSignal)
{
    QStringList args = QStringList() << QString("--bt-when-powered-on") << QString::number(enabled);
    Action a = sendAction(args);
    if (emitSignal && a.success) {
        device->bt_when_powered_on = enabled;
        emit actionSuccesful();
    }
}

void HeadsetControlAPI::setBluetoothCallVolume(
    Device *device, int option, bool emitSignal)
{
    QStringList args = QStringList() << QString("--bt-call-volume") << QString::number(option);
    Action a = sendAction(args);
    if (emitSignal && a.success) {
        device->bt_call_volume = option;
        emit actionSuccesful();
    }
}
