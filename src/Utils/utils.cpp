#include "utils.h"
#include <QCoreApplication>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QProcess>
#include <QDesktopServices>
#include <QDir>

#include <QStandardPaths>

QString getLatestGitHubReleaseVersion(const QString& owner, const QString& repo)
{
    QEventLoop loop;
    QNetworkAccessManager manager;
    QNetworkRequest request(QUrl(QString("https://api.github.com/repos/%1/%2/releases/latest").arg(owner, repo)));
    request.setHeader(QNetworkRequest::UserAgentHeader, "Mozilla/5.0");

    QNetworkReply *reply = manager.get(request);
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    if (reply->error() == QNetworkReply::NoError) {
        QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
        QJsonObject jsonObj = doc.object();
        QString latestVersion = jsonObj.value("tag_name").toString();
        reply->deleteLater();
        return latestVersion;
    } else {
        qDebug() << "Error:" << reply->errorString();
        reply->deleteLater();
        return QString();
    }
}

bool fileExists(const QString& filepath)
{
    QFileInfo checkFile(filepath);
    return checkFile.exists();
}

bool downloadAndUnzipGithubRepo(const QString& user, const QString& repo, const QString& savePath)
{
    // Step 1: Download the zip file
    QString url = QString("https://github.com/%1/%2/archive/refs/heads/headsetcontrol-windows.zip").arg(user, repo);
    QString zipFilePath = savePath + "/headsetcontrol-windows.zip";

    url= "https://github.com/Sapd/HeadsetControl/releases/download/3.0.0/headsetcontrol-windows.zip";
    QNetworkAccessManager manager;
    QEventLoop loop;
    QNetworkReply *reply = manager.get(QNetworkRequest(QUrl(url)));
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << "Download Error:" << reply->errorString();
        reply->deleteLater();
        return false;
    }

    QFile file(zipFilePath);
    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "Could not open file for writing:" << file.errorString();
        reply->deleteLater();
        return false;
    }

    file.write(reply->readAll());
    file.close();
    reply->deleteLater();

    // Step 2: Unzip the file
    QDir().mkpath(savePath);
    QProcess unzip;
    unzip.setWorkingDirectory(savePath);
    unzip.start("unzip", QStringList() << "-o" << zipFilePath << "-d" << savePath);
    if (!unzip.waitForFinished()) {
        qDebug() << "Unzip Error:" << unzip.errorString();
        return false;
    }

    // Optional: Remove the zip file after extraction
    QFile::remove(zipFilePath);

    return true;
}

bool openFileExplorer(const QString& path)
{
    QDir dir(path);
    if (!dir.exists())
    {
        qDebug() << "Path does not exist:" << path;
        return false;
    }

    QUrl url = QUrl::fromLocalFile(dir.absolutePath());
    return QDesktopServices::openUrl(url);
}

bool setOSRunOnStartup(bool enable){
    QString appName = QCoreApplication::applicationName();
    QString appDir = QCoreApplication::applicationDirPath();
    QString appPath = QCoreApplication::applicationFilePath();

#ifdef Q_OS_WIN
    QString startupPath = QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation) + QDir::separator() + "Startup";
    QString linkPath = startupPath + "\\" + appName + ".lnk";
    if(enable){
        QFile::remove(linkPath);
        return QFile::link(appPath, linkPath);
    }
    QFile::remove(linkPath);
    return false;

#elif defined(Q_OS_LINUX)
    QString autostartPath = QDir::homePath() + "/.config/autostart/";
    QString desktopFilePath = autostartPath + appName + ".desktop";

    if(enable){
        QFile::remove(desktopFilePath);
        QFile desktopFile(desktopFilePath);
        if (desktopFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&desktopFile);
            out << "[Desktop Entry]\n";
            out << "Path=" + appDir + "\n"
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

