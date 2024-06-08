#include "ghTools.h"
#include <QCoreApplication>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUrl>

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
