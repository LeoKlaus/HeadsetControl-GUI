#ifndef SETTINGS_H
#define SETTINGS_H

#include <QString>

class Settings
{
public:
    Settings();

    bool runOnstartup = false;
    int batteryLowThreshold = 15;
    int msecUpdateIntervalTime = 30000;
};

Settings loadSettingsFromFile(const QString &filename);
void saveSettingstoFile(const Settings &settings, const QString &filename);

#endif // SETTINGS_H
