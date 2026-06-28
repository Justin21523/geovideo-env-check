#include "src/infrastructure/repositories/sqlite/SqliteEventRepository.h"

#include "src/infrastructure/database/DatabaseConnection.h"
#include "src/infrastructure/database/SqliteStatement.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

namespace geovideo::infrastructure::repositories::sqlite {
namespace {

QString dateTimeToText(const QDateTime& value)
{
    return value.toUTC().toString(Qt::ISODateWithMs);
}

QDateTime dateTimeFromText(const QString& value)
{
    return QDateTime::fromString(value, Qt::ISODateWithMs);
}

QString boundingBoxToJson(const domain::BoundingBox& box)
{
    QJsonObject object;
    object.insert(QStringLiteral("x"), box.x);
    object.insert(QStringLiteral("y"), box.y);
    object.insert(QStringLiteral("width"), box.width);
    object.insert(QStringLiteral("height"), box.height);

    return QString::fromUtf8(QJsonDocument(object).toJson(QJsonDocument::Compact));
}

domain::BoundingBox boundingBoxFromJson(const QString& json)
{
    const QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());

    if (!document.isObject()) {
        return {};
    }

    const QJsonObject object = document.object();

    return domain::BoundingBox{
        object.value(QStringLiteral("x")).toInt(),
        object.value(QStringLiteral("y")).toInt(),
        object.value(QStringLiteral("width")).toInt(),
        object.value(QStringLiteral("height")).toInt(),
    };
}

QString tagsToJson(const QStringList& tags)
{
    QJsonArray array;

    // Tags 用 JSON array 保存，後續接 FTS 或篩選時比較容易轉換。
    for (const QString& tag : tags) {
        const QString normalizedTag = tag.trimmed();

        if (!normalizedTag.isEmpty()) {
            array.append(normalizedTag);
        }
    }

    return QString::fromUtf8(QJsonDocument(array).toJson(QJsonDocument::Compact));
}

QStringList tagsFromJson(const QString& json)
{
    QStringList tags;

    const QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());

    if (!document.isArray()) {
        return tags;
    }

    const QJsonArray array = document.array();

    for (const QJsonValue& value : array) {
        const QString tag = value.toString().trimmed();

        if (!tag.isEmpty()) {
            tags.append(tag);
        }
    }

    tags.removeDuplicates();
    return tags;
}

domain::Event mapEvent(database::SqliteStatement& statement)
{
    return domain::Event::reconstitute(
        domain::EntityId::fromString(statement.columnText(0)),
        domain::EntityId::fromString(statement.columnText(1)),
        domain::EntityId::fromString(statement.columnText(2)),
        domain::EntityId::fromString(statement.columnText(3)),
        domain::EntityId::fromString(statement.columnText(4)),
        domain::eventTypeFromDisplayText(statement.columnText(5)),
        domain::severityFromDisplayText(statement.columnText(6)),
        domain::eventReviewStatusFromDisplayText(statement.columnText(7)),
        domain::TimeRange(statement.columnInt64(8), statement.columnInt64(9)),
        statement.columnInt64(10),
        statement.columnInt64(11),
        boundingBoxFromJson(statement.columnText(12)),
        statement.columnText(13),
        statement.columnDouble(14),
        statement.columnText(15),
        statement.columnText(16),
        statement.columnText(17),
        tagsFromJson(statement.columnText(18)),
        dateTimeFromText(statement.columnText(19))
    );
}

QString eventSelectSql()
{
    return QStringLiteral(R"SQL(
SELECT
    id,
    project_id,
    video_source_id,
    camera_id,
    roi_id,
    type,
    severity,
    review_status,
    start_ms,
    end_ms,
    frame_start,
    frame_end,
    bbox_json,
    track_id,
    motion_score,
    snapshot_path,
    clip_path,
    note,
    tags_json,
    created_at
FROM events
)SQL");
}

} // namespace

SqliteEventRepository::SqliteEventRepository(database::DatabaseConnection& connection)
    : connection_(connection)
{
}

bool SqliteEventRepository::save(const domain::Event& event)
{
    database::SqliteStatement statement(
        connection_,
        QStringLiteral(R"SQL(
INSERT INTO events (
    id,
    project_id,
    video_source_id,
    camera_id,
    roi_id,
    type,
    severity,
    review_status,
    start_ms,
    end_ms,
    frame_start,
    frame_end,
    bbox_json,
    track_id,
    motion_score,
    snapshot_path,
    clip_path,
    note,
    tags_json,
    created_at
)
VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
ON CONFLICT(id) DO UPDATE SET
    camera_id = excluded.camera_id,
    roi_id = excluded.roi_id,
    type = excluded.type,
    severity = excluded.severity,
    review_status = excluded.review_status,
    start_ms = excluded.start_ms,
    end_ms = excluded.end_ms,
    frame_start = excluded.frame_start,
    frame_end = excluded.frame_end,
    bbox_json = excluded.bbox_json,
    track_id = excluded.track_id,
    motion_score = excluded.motion_score,
    snapshot_path = excluded.snapshot_path,
    clip_path = excluded.clip_path,
    note = excluded.note,
    tags_json = excluded.tags_json;
)SQL")
    );

    if (!statement.isValid()) {
        return setFailure(statement.lastErrorMessage());
    }

    const bool cameraBound = event.hasCamera()
        ? statement.bindText(4, event.cameraId().toString())
        : statement.bindNull(4);
    const bool roiBound =
        event.hasRoi() ? statement.bindText(5, event.roiId().toString()) : statement.bindNull(5);

    if (!statement.bindText(1, event.id().toString()) ||
        !statement.bindText(2, event.projectId().toString()) ||
        !statement.bindText(3, event.videoSourceId().toString()) ||
        !cameraBound ||
        !roiBound ||
        !statement.bindText(6, domain::eventTypeToDisplayText(event.type())) ||
        !statement.bindText(7, domain::severityToDisplayText(event.severity())) ||
        !statement.bindText(8, domain::eventReviewStatusToDisplayText(event.reviewStatus())) ||
        !statement.bindInt64(9, event.timeRange().startMs()) ||
        !statement.bindInt64(10, event.timeRange().endMs()) ||
        !statement.bindInt64(11, event.frameStart()) ||
        !statement.bindInt64(12, event.frameEnd()) ||
        !statement.bindText(13, boundingBoxToJson(event.boundingBox())) ||
        !statement.bindText(14, event.trackId()) ||
        !statement.bindDouble(15, event.motionScore()) ||
        !statement.bindText(16, event.snapshotPath()) ||
        !statement.bindText(17, event.clipPath()) ||
        !statement.bindText(18, event.note()) ||
        !statement.bindText(19, tagsToJson(event.tags())) ||
        !statement.bindText(20, dateTimeToText(event.createdAt())) ||
        !statement.execute()) {
        return setFailure(statement.lastErrorMessage());
    }

    lastErrorMessage_.clear();
    return true;
}

std::optional<domain::Event> SqliteEventRepository::findById(const domain::EntityId& id)
{
    database::SqliteStatement statement(
        connection_,
        eventSelectSql() + QStringLiteral(" WHERE id = ?;")
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
        return mapEvent(statement);
    }

    if (statement.hasError()) {
        setFailure(statement.lastErrorMessage());
        return std::nullopt;
    }

    lastErrorMessage_.clear();
    return std::nullopt;
}

QVector<domain::Event> SqliteEventRepository::findByProjectId(const domain::EntityId& projectId)
{
    QVector<domain::Event> events;

    database::SqliteStatement statement(
        connection_,
        eventSelectSql() + QStringLiteral(" WHERE project_id = ? ORDER BY start_ms ASC;")
    );

    if (!statement.isValid()) {
        setFailure(statement.lastErrorMessage());
        return events;
    }

    if (!statement.bindText(1, projectId.toString())) {
        setFailure(statement.lastErrorMessage());
        return events;
    }

    while (statement.step()) {
        events.append(mapEvent(statement));
    }

    if (statement.hasError()) {
        setFailure(statement.lastErrorMessage());
        return {};
    }

    lastErrorMessage_.clear();
    return events;
}

QVector<domain::Event> SqliteEventRepository::findByVideoSourceId(
    const domain::EntityId& videoSourceId
)
{
    QVector<domain::Event> events;

    database::SqliteStatement statement(
        connection_,
        eventSelectSql() + QStringLiteral(" WHERE video_source_id = ? ORDER BY start_ms ASC;")
    );

    if (!statement.isValid()) {
        setFailure(statement.lastErrorMessage());
        return events;
    }

    if (!statement.bindText(1, videoSourceId.toString())) {
        setFailure(statement.lastErrorMessage());
        return events;
    }

    while (statement.step()) {
        events.append(mapEvent(statement));
    }

    if (statement.hasError()) {
        setFailure(statement.lastErrorMessage());
        return {};
    }

    lastErrorMessage_.clear();
    return events;
}

QString SqliteEventRepository::lastErrorMessage() const
{
    return lastErrorMessage_;
}

bool SqliteEventRepository::setFailure(const QString& message)
{
    lastErrorMessage_ = message;
    return false;
}

} // namespace geovideo::infrastructure::repositories::sqlite