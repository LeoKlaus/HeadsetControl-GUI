#include "utils.h"
#include <QCoreApplication>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUrl>
#include <QFileInfo>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QEventLoop>
#include <QFile>
#include <QUrl>
#include <QProcess>
#include <QDesktopServices>
#include <QDir>

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

void setOSRunOnStartup(bool enable){
    //TO BE IMPLEMENTED
}

