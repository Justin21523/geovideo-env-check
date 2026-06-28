#pragma once

#include "src/infrastructure/database/DatabaseConnection.h"
#include "src/infrastructure/database/SchemaMigrator.h"

#include <QString>

namespace geovideo::infrastructure::database {

class DatabaseManager final {
public:
    DatabaseManager() = default;

    bool initializeDefault();
    bool initialize(const QString& databasePath);

    bool isInitialized() const;

    QString databasePath() const;
    QString lastErrorMessage() const;
    int schemaVersion() const;

    DatabaseConnection& connection();

private:
    QString resolveDefaultDatabasePath() const;
    bool ensureParentDirectoryExists(const QString& databasePath);

private:
    DatabaseConnection connection_;
    SchemaMigrator migrator_;

    QString databasePath_;
    QString lastErrorMessage_;
    int schemaVersion_ = 0;
    bool initialized_ = false;
};

} // namespace geovideo::infrastructure::database