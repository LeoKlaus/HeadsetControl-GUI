#ifndef UTILS_H
#define UTILS_H

#include <QString>

QString getLatestGitHubReleaseVersion(const QString& owner, const QString& repo);

bool downloadAndUnzipGithubRepo(const QString& user, const QString& repo, const QString& savePath);

bool fileExists(const QString& filepath);

bool openFileExplorer(const QString& path);

void setOSRunOnStartup(bool enable);

#endif // UTILS_H
