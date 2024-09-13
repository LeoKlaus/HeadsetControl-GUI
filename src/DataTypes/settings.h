#ifndef SETTINGS_H
#define SETTINGS_H

#include <QApplication>
#include <QCoreApplication>
#include <QStandardPaths>
#include <QString>

#ifdef QT_DEBUG
const QString PROGRAM_CONFIG_PATH = "./DEBUG-Config";
#else
const QString PROGRAM_CONFIG_PATH = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
                                    + "/HeadsetControl-GUI";
#endif
const QString PROGRAM_STYLES_PATH = PROGRAM_CONFIG_PATH + "/styles";
const QString PROGRAM_SETTINGS_FILEPATH = PROGRAM_CONFIG_PATH + "/settings.json";
const QString DEVICES_SETTINGS_FILEPATH = PROGRAM_CONFIG_PATH + "/devices.json";

class Settings
{
public:
    Settings();

    bool runOnstartup = false;
    int batteryLowThreshold = 15;
    int msecUpdateIntervalTime = 30000;
    QString styleName = "Default";
};

Settings loadSettingsFromFile(const QString &filePath);
void saveSettingstoFile(const Settings &settings, const QString &filePath);

#endif // SETTINGS_H
