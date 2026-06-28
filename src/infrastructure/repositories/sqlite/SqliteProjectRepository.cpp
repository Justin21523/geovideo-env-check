#include "src/infrastructure/repositories/sqlite/SqliteProjectRepository.h"

#include "src/infrastructure/database/DatabaseConnection.h"
#include "src/infrastructure/database/SqliteStatement.h"

namespace geovideo::infrastructure::repositories::sqlite {
namespace {

QString dateTimeToText(const QDateTime& value)
{
    // SQLite 先以 ISO 文字保存時間，後續查詢與除錯都比較直覺。
    return value.toUTC().toString(Qt::ISODateWithMs);
}

QDateTime dateTimeFromText(const QString& value)
{
    return QDateTime::fromString(value, Qt::ISODateWithMs);
}

domain::Project mapProject(database::SqliteStatement& statement)
{
    return domain::Project::reconstitute(
        domain::EntityId::fromString(statement.columnText(0)),
        statement.columnText(1),
        statement.columnText(2),
        statement.columnText(3),
        dateTimeFromText(statement.columnText(4)),
        dateTimeFromText(statement.columnText(5))
    );
}

} // namespace

SqliteProjectRepository::SqliteProjectRepository(database::DatabaseConnection& connection)
    : connection_(connection)
{
}

bool SqliteProjectRepository::save(const domain::Project& project)
{
    database::SqliteStatement statement(
        connection_,
        QStringLiteral(R"SQL(
INSERT INTO projects (
    id,
    name,
    description,
    base_directory,
    created_at,
    updated_at
)
VALUES (?, ?, ?, ?, ?, ?)
ON CONFLICT(id) DO UPDATE SET
    name = excluded.name,
    description = excluded.description,
    base_directory = excluded.base_directory,
    updated_at = excluded.updated_at;
)SQL")
    );

    if (!statement.isValid()) {
        return setFailure(statement.lastErrorMessage());
    }

    if (!statement.bindText(1, project.id().toString()) ||
        !statement.bindText(2, project.name()) ||
        !statement.bindText(3, project.description()) ||
        !statement.bindText(4, project.baseDirectory()) ||
        !statement.bindText(5, dateTimeToText(project.createdAt())) ||
        !statement.bindText(6, dateTimeToText(project.updatedAt())) ||
        !statement.execute()) {
        return setFailure(statement.lastErrorMessage());
    }

    lastErrorMessage_.clear();
    return true;
}

std::optional<domain::Project> SqliteProjectRepository::findById(const domain::EntityId& id)
{
    database::SqliteStatement statement(
        connection_,
        QStringLiteral(R"SQL(
SELECT
    id,
    name,
    description,
    base_directory,
    created_at,
    updated_at
FROM projects
WHERE id = ?;
)SQL")
    );

    if (!statement.isValid()) {
        setFailure(statement.lastErrorMessage());
        return std::nullopt;
    }

    if (!statement.bindText(1, id.toString())) {
        setFailure(statement.lastErrorMessage());
        return std::nullopt;
    }

    if (statement.step()) {
        lastErrorMessage_.clear();
        return mapProject(statement);
    }

    if (statement.hasError()) {
        setFailure(statement.lastErrorMessage());
        return std::nullopt;
    }

    lastErrorMessage_.clear();
    return std::nullopt;
}

QVector<domain::Project> SqliteProjectRepository::findAll()
{
    QVector<domain::Project> projects;

    database::SqliteStatement statement(
        connection_,
        QStringLiteral(R"SQL(
SELECT
    id,
    name,
    description,
    base_directory,
    created_at,
    updated_at
FROM projects
ORDER BY updated_at DESC;
)SQL")
    );

    if (!statement.isValid()) {
        setFailure(statement.lastErrorMessage());
        return projects;
    }

    while (statement.step()) {
        projects.append(mapProject(statement));
    }

    if (statement.hasError()) {
        setFailure(statement.lastErrorMessage());
        return {};
    }

    lastErrorMessage_.clear();
    return projects;
}

QString SqliteProjectRepository::lastErrorMessage() const
{
    return lastErrorMessage_;
}

bool SqliteProjectRepository::setFailure(const QString& message)
{
    lastErrorMessage_ = message;
    return false;
}

} // namespace geovideo::infrastructure::repositories::sqlite