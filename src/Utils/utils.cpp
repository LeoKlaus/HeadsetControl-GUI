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
    QString linkPath = startupPath + QDir::separator() + appName + ".lnk";
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
