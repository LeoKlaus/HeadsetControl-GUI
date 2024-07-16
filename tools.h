#ifndef TOOLS_H
#define TOOLS_H

#include <QString>

QString getLatestGitHubReleaseVersion(const QString& owner, const QString& repo);

bool downloadAndUnzipGithubRepo(const QString& user, const QString& repo, const QString& savePath);

bool fileExists(const QString& filepath);

bool openFileExplorer(const QString& path);

#endif // TOOLS_H
