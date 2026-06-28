#pragma once

#include <QString>
#include <QStringList>

namespace geovideo::infrastructure::database {

class DatabaseConnection;

class SchemaMigrator final {
public:
    static constexpr int CurrentSchemaVersion = 1;

    bool migrate(DatabaseConnection& connection);

    QString lastErrorMessage() const;
    QStringList appliedSteps() const;

private:
    bool migrateToVersion1(DatabaseConnection& connection);
    bool runInTransaction(DatabaseConnection& connection, const QString& sql, int targetVersion);

private:
    QString lastErrorMessage_;
    QStringList appliedSteps_;
};

} // namespace geovideo::infrastructure::database