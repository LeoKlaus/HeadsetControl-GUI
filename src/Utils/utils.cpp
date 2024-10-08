#include "utils.h"

#include <QCoreApplication>
#include <QDesktopServices>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QProcess>
#include <QStandardPaths>

QString getLatestGitHubReleaseVersion(const QString &owner, const QString &repo)
{
    QEventLoop loop;
    QNetworkAccessManager manager;
    QNetworkRequest request(
        QUrl(QString("https://api.github.com/repos/%1/%2/releases/latest").arg(owner, repo)));
    request.setHeader(QNetworkRequest::UserAgentHeader, "Mozilla/5.0");

    QNetworkReply *reply = manager.get(request);
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    if (reply->error() == QNetworkReply::NoError) {
        QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
        QJsonObject jsonObj = doc.object();
        QString latestVersion = jsonObj.value("tag_name").toString();
        reply->deleteLater();
        return latestVersion.removeFirst();
    } else {
        qDebug() << "Error:" << reply->errorString();
        reply->deleteLater();
        return QString();
    }
}

bool fileExists(const QString &filePath)
{
    QFileInfo checkFile(filePath);
    return checkFile.exists();
}

bool openFileExplorer(const QString &path)
{
    QDir dir(path);
    if (!dir.exists()) {
        qDebug() << "Path does not exist:" << path;
        return false;
    }

    QUrl url = QUrl::fromLocalFile(dir.absolutePath());
    return QDesktopServices::openUrl(url);
}

bool setOSRunOnStartup(bool enable)
{
    QString appName = QCoreApplication::applicationName();
    QString appPath = QCoreApplication::applicationFilePath();

#ifdef Q_OS_WIN
    QString startupPath = QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation)
                          + QDir::separator() + "Startup";
    QString linkPath = startupPath + "\\" + appName + ".lnk";
    if (enable) {
        QFile::remove(linkPath);
        return QFile::link(appPath, linkPath);
    }
    QFile::remove(linkPath);
    return false;

#elif defined(Q_OS_LINUX)
    QString appDir = QCoreApplication::applicationDirPath();
    QString autostartPath = QDir::homePath() + "/.config/autostart/";
    QString desktopFilePath = autostartPath + appName + ".desktop";

    if (enable) {
        QFile::remove(desktopFilePath);
        QFile desktopFile(desktopFilePath);
        if (desktopFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&desktopFile);
            out << "[Desktop Entry]\n";
            out << "Path=" + appDir + "\n";
            out << "Type=Application\n";
            out << "Exec=" << appPath << "\n";
            out << "Name=" << appName << "\n";
            out << "Comment=Auto-starts " << appName << " on boot\n";
            desktopFile.close();
            return true;
        }
    }
    QFile::remove(desktopFilePath);

    return false;
#endif
}

bool createStartMenuShortcut()
{
    QString appName = QCoreApplication::applicationName();
    QString appPath = QCoreApplication::applicationFilePath();

#ifdef Q_OS_WIN
    QString startupPath = QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation);
    QString linkPath = startupPath + QDir::separator() + appName + ".lnk";
    QFile::remove(linkPath);
    return QFile::link(appPath, linkPath);

#elif defined(Q_OS_LINUX)
    // Get the applications directory
    QString applicationsDir = QDir::homePath() + "/.local/share/applications/";

    // Create applications directory if it doesn't exist
    QDir().mkpath(applicationsDir);

    // Create .desktop file
    QString desktopFilePath = applicationsDir + appName.toLower() + ".desktop";
    QFile desktopFile(desktopFilePath);

    if (!desktopFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Failed to create .desktop file";
        return false;
    }

    // Desktop entry content
    QString desktopContent = QString("[Desktop Entry]\n"
                                     "Version=1.0\n"
                                     "Type=Application\n"
                                     "Name=%1\n"
                                     "Exec=%2\n"
                                     "Terminal=false\n"
                                     "Categories=Utility;\n")
                                 .arg(appName, appPath);
    desktopFile.write(desktopContent.toUtf8());
    desktopFile.close();

    // Make the .desktop file executable
    QFile::setPermissions(desktopFilePath,
                          QFile::ReadOwner | QFile::WriteOwner | QFile::ExeOwner | QFile::ReadGroup
                              | QFile::ReadOther);

    return true;

#else
    qDebug() << "Unsupported operating system";
    return false;
#endif
}
