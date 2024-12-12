#include "device.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QProcess>

Battery::Battery() {}

Battery::Battery(QString stat, int lev)
{
    status = stat;
    level = lev;
}

Equalizer::Equalizer() {}

Equalizer::Equalizer(int bands, int baseline, double step, int min, int max)
{
    bands_number = bands;
    band_baseline = baseline;
    band_min = min;
    band_step = step;
    band_max = max;
}

Device::Device() {}

Device::Device(const QJsonObject &jsonObj, QString jsonData)
{
    status = jsonObj["status"].toString();

    device = jsonObj["device"].toString();
    vendor = jsonObj["vendor"].toString();
    product = jsonObj["product"].toString();
    id_vendor = jsonObj["id_vendor"].toString();
    id_product = jsonObj["id_product"].toString();

    QJsonArray caps = jsonObj["capabilities"].toArray();
    for (const QJsonValue &value : caps) {
        capabilities.insert(value.toString());
    }
    if (capabilities.contains("CAP_BATTERY_STATUS")) {
        QJsonObject jEq = jsonObj["battery"].toObject();
        battery = Battery(jEq["status"].toString(), jEq["level"].toInt());
    }
    if (capabilities.contains("CAP_CHATMIX_STATUS")) {
        chatmix = jsonObj["chatmix"].toInt();
    }

    if (capabilities.contains("CAP_EQUALIZER_PRESET")) {
        if (jsonObj.contains("equalizer_presets") && jsonObj["equalizer_presets"].isObject()) {
            QJsonObject equalizerPresets = jsonObj["equalizer_presets"].toObject();

            // Parse the original JSON string to find the order of keys
            static QRegularExpression re("\"(\\w+)\":\\s*\\[");
            QRegularExpressionMatchIterator i = re.globalMatch(jsonData);
            while (i.hasNext()) {
                QRegularExpressionMatch match = i.next();
                QString presetName = match.captured(1);
                if (equalizerPresets.contains(presetName)) {
                    EqualizerPreset preset;
                    preset.name = presetName;

                    QJsonArray valuesArray = equalizerPresets[presetName].toArray();
                    for (const QJsonValue &value : valuesArray) {
                        preset.values.append(value.toDouble());
                    }

                    presets_list.append(preset);
                }
            }
        }
    }
    if (capabilities.contains("CAP_EQUALIZER")) {
        QJsonObject jEq = jsonObj["equalizer"].toObject();
        if (!jEq.isEmpty()) {
            equalizer = Equalizer(jEq["bands"].toInt(),
                                  jEq["baseline"].toInt(),
                                  jEq["step"].toDouble(),
                                  jEq["min"].toInt(),
                                  jEq["max"].toInt());
            equalizer_curve = QVector<double>(equalizer.bands_number, equalizer.band_baseline);
        }
    }
}

// Helper functions
bool Device::operator!=(const Device &d) const
{
    return this->id_vendor != d.id_vendor || this->id_product != d.id_product;
}

bool Device::operator==(const Device &d) const
{
    return this->id_vendor == d.id_vendor && this->id_product == d.id_product;
}

bool Device::operator==(const Device *d) const
{
    return this->id_vendor == d->id_vendor && this->id_product == d->id_product;
}

void Device::updateDevice(const Device *new_device)
{
    this->battery = new_device->battery;
    this->chatmix = new_device->chatmix;
}

bool Device::updateDevice(const QList<Device *> &new_device_list)
{
    foreach (Device *new_device, new_device_list) {
        if (*this == new_device) {
            this->updateDevice(new_device);
            return true;
        }
    }
    return false;
}

QJsonObject Device::toJson() const
{
    QJsonObject json;
    json["device"] = device;
    json["vendor"] = vendor;
    json["product"] = product;
    json["id_vendor"] = id_vendor;
    json["id_product"] = id_product;

    json["lights"] = lights;
    json["sidetone"] = sidetone;
    json["voice_prompts"] = voice_prompts;
    json["inactive_time"] = inactive_time;
    json["equalizer_preset"] = equalizer_preset;
    json["equalizer_curve"] = QJsonArray::fromVariantList(
        QVariantList(equalizer_curve.begin(), equalizer_curve.end()));
    json["volume_limiter"] = volume_limiter;
    json["rotate_to_mute"] = rotate_to_mute;
    json["mic_mute_led_brightness"] = mic_mute_led_brightness;
    json["mic_volume"] = mic_volume;
    json["bt_when_powered_on"] = bt_when_powered_on;
    json["bt_call_volume"] = bt_call_volume;

    return json;
}

Device *Device::fromJson(
    const QJsonObject &json)
{
    Device *device = new Device();
    device->device = json["device"].toString();
    device->vendor = json["vendor"].toString();
    device->product = json["product"].toString();
    device->id_vendor = json["id_vendor"].toString();
    device->id_product = json["id_product"].toString();

    device->lights = json["lights"].toInt();
    device->sidetone = json["sidetone"].toInt();
    device->voice_prompts = json["voice_prompts"].toInt();
    device->inactive_time = json["inactive_time"].toInt();
    device->equalizer_preset = json["equalizer_preset"].toInt();

    QJsonArray curveArray = json["equalizer_curve"].toArray();
    for (const auto &value : curveArray) {
        device->equalizer_curve.append(value.toDouble());
    }

    device->volume_limiter = json["volume_limiter"].toInt();
    device->rotate_to_mute = json["rotate_to_mute"].toInt();
    device->mic_mute_led_brightness = json["mic_mute_led_brightness"].toInt();
    device->mic_volume = json["mic_volume"].toInt();
    device->bt_when_powered_on = json["bt_when_powered_on"].toInt();
    device->bt_call_volume = json["bt_call_volume"].toInt();

    return device;
}

void updateDeviceFromSource(QList<Device *> &devicesToUpdate, const Device *sourceDevice)
{
    bool found = false;
    for (Device *toUpdateDevice : devicesToUpdate) {
        if (*toUpdateDevice == sourceDevice) {
            // Update the saved device with connected device's information
            *toUpdateDevice = *sourceDevice;
            found = true;
        }
    };
    if (!found) {
        Device *toAppend = new Device();
        *toAppend = *sourceDevice;
        devicesToUpdate.append(toAppend);
    }
}

void updateDevicesFromSource(QList<Device *> &devicesToUpdate, const QList<Device *> &sourceDevices)
{
    for (Device *toUpdateDevice : devicesToUpdate) {
        for (Device *sourceDevice : sourceDevices) {
            if (*toUpdateDevice == sourceDevice) {
                // Update the connected device with saved device's information
                *toUpdateDevice = *sourceDevice;
            }
        }
    };
}

void serializeDevices(const QList<Device *> &devices, const QString &filePath)
{
    QJsonArray jsonArray;
    for (const auto *device : devices) {
        jsonArray.append(device->toJson());
    }

    QJsonDocument doc(jsonArray);
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson());
        file.close();
        qDebug() << "Devices Serialized" << jsonArray;
        qDebug();
    }
}

QList<Device *> deserializeDevices(const QString &filePath)
{
    QList<Device *> devices;
    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly)) {
        QByteArray data = file.readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        QJsonArray jsonArray = doc.array();

        for (const auto &value : jsonArray) {
            Device *device = Device::fromJson(value.toObject());
            devices.append(device);
        }

        file.close();
        qDebug() << "Devices Deserialized" << jsonArray;
        qDebug();
    }
    return devices;
}

void deleteDevices(
    QList<Device *> deviceList)
{
    for (Device *device : deviceList) {
        delete device;
    }
    deviceList.clear();
}
