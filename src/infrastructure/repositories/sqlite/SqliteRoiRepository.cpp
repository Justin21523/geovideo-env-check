#include "src/infrastructure/repositories/sqlite/SqliteRoiRepository.h"

#include "src/infrastructure/database/DatabaseConnection.h"
#include "src/infrastructure/database/SqliteStatement.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

namespace geovideo::infrastructure::repositories::sqlite {
namespace {

QString pointsToJson(const QVector<domain::RoiPoint>& points)
{
    QJsonArray array;

    // ROI points 先用 JSON 存，之後 Rectangle / Polygon / Line 都可以共用。
    for (const domain::RoiPoint& point : points) {
        QJsonObject object;
        object.insert(QStringLiteral("x"), point.x);
        object.insert(QStringLiteral("y"), point.y);
        array.append(object);
    }

    return QString::fromUtf8(QJsonDocument(array).toJson(QJsonDocument::Compact));
}

QVector<domain::RoiPoint> pointsFromJson(const QString& json)
{
    QVector<domain::RoiPoint> points;

    const QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());

    if (!document.isArray()) {
        return points;
    }

    const QJsonArray array = document.array();

    for (const QJsonValue& value : array) {
        if (!value.isObject()) {
            continue;
        }

        const QJsonObject object = value.toObject();

        points.append(domain::RoiPoint{
            object.value(QStringLiteral("x")).toDouble(),
            object.value(QStringLiteral("y")).toDouble(),
        });
    }

    return points;
}

domain::Roi mapRoi(database::SqliteStatement& statement)
{
    return domain::Roi::reconstitute(
        domain::EntityId::fromString(statement.columnText(0)),
        domain::EntityId::fromString(statement.columnText(1)),
        statement.columnText(2),
        domain::roiTypeFromDisplayText(statement.columnText(3)),
        domain::severityFromDisplayText(statement.columnText(4)),
        pointsFromJson(statement.columnText(5)),
        statement.columnText(6),
        statement.columnInt(7) != 0
    );
}

} // namespace

SqliteRoiRepository::SqliteRoiRepository(database::DatabaseConnection& connection)
    : connection_(connection)
{
}

bool SqliteRoiRepository::save(const domain::Roi& roi)
{
    database::SqliteStatement statement(
        connection_,
        QStringLiteral(R"SQL(
INSERT INTO rois (
    id,
    video_source_id,
    name,
    type,
    severity,
    points_json,
    rule_json,
    enabled
)
VALUES (?, ?, ?, ?, ?, ?, ?, ?)
ON CONFLICT(id) DO UPDATE SET
    name = excluded.name,
    type = excluded.type,
    severity = excluded.severity,
    points_json = excluded.points_json,
    rule_json = excluded.rule_json,
    enabled = excluded.enabled;
)SQL")
    );

    if (!statement.isValid()) {
        return setFailure(statement.lastErrorMessage());
    }

    if (!statement.bindText(1, roi.id().toString()) ||
        !statement.bindText(2, roi.videoSourceId().toString()) ||
        !statement.bindText(3, roi.name()) ||
        !statement.bindText(4, domain::roiTypeToDisplayText(roi.type())) ||
        !statement.bindText(5, domain::severityToDisplayText(roi.severity())) ||
        !statement.bindText(6, pointsToJson(roi.points())) ||
        !statement.bindText(7, roi.ruleJson()) ||
        !statement.bindInt(8, roi.isEnabled() ? 1 : 0) ||
        !statement.execute()) {
        return setFailure(statement.lastErrorMessage());
    }

    lastErrorMessage_.clear();
    return true;
}

std::optional<domain::Roi> SqliteRoiRepository::findById(const domain::EntityId& id)
{
    database::SqliteStatement statement(
        connection_,
        QStringLiteral(R"SQL(
SELECT
    id,
    video_source_id,
    name,
    type,
    severity,
    points_json,
    rule_json,
    enabled
FROM rois
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
        return mapRoi(statement);
    }

    if (statement.hasError()) {
        setFailure(statement.lastErrorMessage());
        return std::nullopt;
    }

    lastErrorMessage_.clear();
    return std::nullopt;
}

QVector<domain::Roi> SqliteRoiRepository::findByVideoSourceId(
    const domain::EntityId& videoSourceId
)
{
    QVector<domain::Roi> rois;

    database::SqliteStatement statement(
        connection_,
        QStringLiteral(R"SQL(
SELECT
    id,
    video_source_id,
    name,
    type,
    severity,
    points_json,
    rule_json,
    enabled
FROM rois
WHERE video_source_id = ?
ORDER BY name ASC;
)SQL")
    );

    if (!statement.isValid()) {
        setFailure(statement.lastErrorMessage());
        return rois;
    }

    if (!statement.bindText(1, videoSourceId.toString())) {
        setFailure(statement.lastErrorMessage());
        return rois;
    }

    while (statement.step()) {
        rois.append(mapRoi(statement));
    }

    if (statement.hasError()) {
        setFailure(statement.lastErrorMessage());
        return {};
    }

    lastErrorMessage_.clear();
    return rois;
}

QString SqliteRoiRepository::lastErrorMessage() const
{
    return lastErrorMessage_;
}

bool SqliteRoiRepository::setFailure(const QString& message)
{
    lastErrorMessage_ = message;
    return false;
}

} // namespace geovideo::infrastructure::repositories::sqlite