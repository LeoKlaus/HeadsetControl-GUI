#include "settings.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

Settings::Settings() {}

Settings loadSettingsFromFile(const QString &filePath)
{
    Settings s;

    QFile file(filePath);

    if (file.open(QIODevice::ReadOnly)) {
        QByteArray saveData = file.readAll();
        file.close();

        QJsonDocument doc(QJsonDocument::fromJson(saveData));
        QJsonObject json = doc.object();

        if (json.contains("runOnStartup")) {
            s.runOnstartup = json["runOnStartup"].toBool();
        }
        if (json.contains("batteryLowThreshold")) {
            s.batteryLowThreshold = json["batteryLowThreshold"].toInt();
        }
        if (json.contains("msecUpdateIntervalTime")) {
            s.msecUpdateIntervalTime = json["msecUpdateIntervalTime"].toInt();
        }
        if (json.contains("styleName")) {
            s.styleName = json["styleName"].toString();
        }
        qDebug() << "Settings Loaded:\t" << json;
    }

    return s;
}

void saveSettingstoFile(const Settings &settings, const QString &filePath)
{
    QJsonObject json;
    json["runOnStartup"] = settings.runOnstartup;
    json["batteryLowThreshold"] = settings.batteryLowThreshold;
    json["msecUpdateIntervalTime"] = settings.msecUpdateIntervalTime;
    json["styleName"] = settings.styleName;

    QJsonDocument doc(json);
    QFile file(filePath);

    if (!file.open(QIODevice::WriteOnly)) {
        qWarning("Couldn't open save file.");
    }

    file.write(doc.toJson());
    file.close();
    qDebug() << "Settings Saved:\t" << json;
}
