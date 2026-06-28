#include "src/infrastructure/repositories/sqlite/SqliteVideoSourceRepository.h"

#include "src/infrastructure/database/DatabaseConnection.h"
#include "src/infrastructure/database/SqliteStatement.h"

namespace geovideo::infrastructure::repositories::sqlite {
namespace {

QString dateTimeToText(const QDateTime& value)
{
    if (!value.isValid()) {
        return {};
    }

    return value.toUTC().toString(Qt::ISODateWithMs);
}

QDateTime dateTimeFromText(const QString& value)
{
    return QDateTime::fromString(value, Qt::ISODateWithMs);
}

domain::VideoSource mapVideoSource(database::SqliteStatement& statement)
{
    return domain::VideoSource::reconstitute(
        domain::EntityId::fromString(statement.columnText(0)),
        domain::EntityId::fromString(statement.columnText(1)),
        domain::EntityId::fromString(statement.columnText(2)),
        statement.columnText(3),
        statement.columnText(4),
        statement.columnInt(5),
        statement.columnInt(6),
        statement.columnDouble(7),
        statement.columnInt64(8),
        statement.columnInt64(9),
        dateTimeFromText(statement.columnText(10)),
        dateTimeFromText(statement.columnText(11))
    );
}

} // namespace

SqliteVideoSourceRepository::SqliteVideoSourceRepository(database::DatabaseConnection& connection)
    : connection_(connection)
{
}

bool SqliteVideoSourceRepository::save(const domain::VideoSource& videoSource)
{
    database::SqliteStatement statement(
        connection_,
        QStringLiteral(R"SQL(
INSERT INTO video_sources (
    id,
    project_id,
    camera_id,
    file_path,
    display_name,
    width,
    height,
    fps,
    duration_ms,
    frame_count,
    start_time,
    imported_at
)
VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
ON CONFLICT(id) DO UPDATE SET
    camera_id = excluded.camera_id,
    file_path = excluded.file_path,
    display_name = excluded.display_name,
    width = excluded.width,
    height = excluded.height,
    fps = excluded.fps,
    duration_ms = excluded.duration_ms,
    frame_count = excluded.frame_count,
    start_time = excluded.start_time;
)SQL")
    );

    if (!statement.isValid()) {
        return setFailure(statement.lastErrorMessage());
    }

    const bool cameraBound = videoSource.cameraId().isValid()
        ? statement.bindText(3, videoSource.cameraId().toString())
        : statement.bindNull(3);

    if (!statement.bindText(1, videoSource.id().toString()) ||
        !statement.bindText(2, videoSource.projectId().toString()) ||
        !cameraBound ||
        !statement.bindText(4, videoSource.filePath()) ||
        !statement.bindText(5, videoSource.displayName()) ||
        !statement.bindInt(6, videoSource.width()) ||
        !statement.bindInt(7, videoSource.height()) ||
        !statement.bindDouble(8, videoSource.fps()) ||
        !statement.bindInt64(9, videoSource.durationMs()) ||
        !statement.bindInt64(10, videoSource.frameCount()) ||
        !statement.bindText(11, dateTimeToText(videoSource.startTime())) ||
        !statement.bindText(12, dateTimeToText(videoSource.importedAt())) ||
        !statement.execute()) {
        return setFailure(statement.lastErrorMessage());
    }

    lastErrorMessage_.clear();
    return true;
}

std::optional<domain::VideoSource> SqliteVideoSourceRepository::findById(
    const domain::EntityId& id
)
{
    database::SqliteStatement statement(
        connection_,
        QStringLiteral(R"SQL(
SELECT
    id,
    project_id,
    camera_id,
    file_path,
    display_name,
    width,
    height,
    fps,
    duration_ms,
    frame_count,
    start_time,
    imported_at
FROM video_sources
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
        return mapVideoSource(statement);
    }

    if (statement.hasError()) {
        setFailure(statement.lastErrorMessage());
        return std::nullopt;
    }

    lastErrorMessage_.clear();
    return std::nullopt;
}

QVector<domain::VideoSource> SqliteVideoSourceRepository::findByProjectId(
    const domain::EntityId& projectId
)
{
    QVector<domain::VideoSource> sources;

    database::SqliteStatement statement(
        connection_,
        QStringLiteral(R"SQL(
SELECT
    id,
    project_id,
    camera_id,
    file_path,
    display_name,
    width,
    height,
    fps,
    duration_ms,
    frame_count,
    start_time,
    imported_at
FROM video_sources
WHERE project_id = ?
ORDER BY imported_at DESC;
)SQL")
    );

    if (!statement.isValid()) {
        setFailure(statement.lastErrorMessage());
        return sources;
    }

    if (!statement.bindText(1, projectId.toString())) {
        setFailure(statement.lastErrorMessage());
        return sources;
    }

    while (statement.step()) {
        sources.append(mapVideoSource(statement));
    }

    if (statement.hasError()) {
        setFailure(statement.lastErrorMessage());
        return {};
    }

    lastErrorMessage_.clear();
    return sources;
}

QString SqliteVideoSourceRepository::lastErrorMessage() const
{
    return lastErrorMessage_;
}

bool SqliteVideoSourceRepository::setFailure(const QString& message)
{
    lastErrorMessage_ = message;
    return false;
}

} // namespace geovideo::infrastructure::repositories::sqlite