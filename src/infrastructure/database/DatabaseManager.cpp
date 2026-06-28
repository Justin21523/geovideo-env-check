#include "src/infrastructure/database/DatabaseManager.h"

#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>

namespace geovideo::infrastructure::database {

bool DatabaseManager::initializeDefault()
{
    return initialize(resolveDefaultDatabasePath());
}

bool DatabaseManager::initialize(const QString& databasePath)
{
    initialized_ = false;
    lastErrorMessage_.clear();
    schemaVersion_ = 0;

    if (databasePath.trimmed().isEmpty()) {
        lastErrorMessage_ = QStringLiteral("Database path is empty.");
        return false;
    }

    if (!ensureParentDirectoryExists(databasePath)) {
        return false;
    }

    if (!connection_.open(databasePath)) {
        lastErrorMessage_ = connection_.lastErrorMessage();
        return false;
    }

    if (!migrator_.migrate(connection_)) {
        lastErrorMessage_ = migrator_.lastErrorMessage();
        return false;
    }

    const int detectedVersion = connection_.userVersion();

    if (detectedVersion < 0) {
        lastErrorMessage_ = connection_.lastErrorMessage();
        return false;
    }

    databasePath_ = databasePath;
    schemaVersion_ = detectedVersion;
    initialized_ = true;

    qInfo().noquote()
        << "Database manager initialized:"
        << databasePath_
        << "| schema version:"
        << schemaVersion_;

    return true;
}

bool DatabaseManager::isInitialized() const
{
    return initialized_;
}

QString DatabaseManager::databasePath() const
{
    return databasePath_;
}

QString DatabaseManager::lastErrorMessage() const
{
    return lastErrorMessage_;
}

int DatabaseManager::schemaVersion() const
{
    return schemaVersion_;
}

DatabaseConnection& DatabaseManager::connection()
{
    return connection_;
}

QString DatabaseManager::resolveDefaultDatabasePath() const
{
    // AppLocalDataLocation 會依作業系統放到使用者資料目錄，不污染 repo。
    QString basePath = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);

    if (basePath.isEmpty()) {
        basePath = QDir::currentPath();
    }

    return QDir(basePath).filePath(QStringLiteral("data/geovideo.sqlite3"));
}

bool DatabaseManager::ensureParentDirectoryExists(const QString& databasePath)
{
    const QFileInfo fileInfo(databasePath);
    const QString directoryPath = fileInfo.absolutePath();

    if (QDir(directoryPath).exists()) {
        return true;
    }

    if (QDir().mkpath(directoryPath)) {
        return true;
    }

    lastErrorMessage_ =
        QStringLiteral("Failed to create database directory: %1").arg(directoryPath);
    return false;
}

} // namespace geovideo::infrastructure::database