#include "src/infrastructure/repositories/sqlite/SqliteCameraRepository.h"

#include "src/infrastructure/database/DatabaseConnection.h"
#include "src/infrastructure/database/SqliteStatement.h"

namespace geovideo::infrastructure::repositories::sqlite {
namespace {

domain::Camera mapCamera(database::SqliteStatement& statement)
{
    return domain::Camera::reconstitute(
        domain::EntityId::fromString(statement.columnText(0)),
        domain::EntityId::fromString(statement.columnText(1)),
        statement.columnText(2),
        statement.columnText(3),
        statement.columnDouble(4),
        statement.columnDouble(5),
        statement.columnDouble(6),
        statement.columnDouble(7),
        statement.columnDouble(8),
        statement.columnDouble(9),
        statement.columnInt(10) != 0
    );
}

} // namespace

SqliteCameraRepository::SqliteCameraRepository(database::DatabaseConnection& connection)
    : connection_(connection)
{
}

bool SqliteCameraRepository::save(const domain::Camera& camera)
{
    database::SqliteStatement statement(
        connection_,
        QStringLiteral(R"SQL(
INSERT INTO cameras (
    id,
    project_id,
    name,
    location_name,
    map_x,
    map_y,
    map_z,
    yaw_degrees,
    pitch_degrees,
    fov_degrees,
    enabled
)
VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
ON CONFLICT(id) DO UPDATE SET
    name = excluded.name,
    location_name = excluded.location_name,
    map_x = excluded.map_x,
    map_y = excluded.map_y,
    map_z = excluded.map_z,
    yaw_degrees = excluded.yaw_degrees,
    pitch_degrees = excluded.pitch_degrees,
    fov_degrees = excluded.fov_degrees,
    enabled = excluded.enabled;
)SQL")
    );

    if (!statement.isValid()) {
        return setFailure(statement.lastErrorMessage());
    }

    if (!statement.bindText(1, camera.id().toString()) ||
        !statement.bindText(2, camera.projectId().toString()) ||
        !statement.bindText(3, camera.name()) ||
        !statement.bindText(4, camera.locationName()) ||
        !statement.bindDouble(5, camera.mapX()) ||
        !statement.bindDouble(6, camera.mapY()) ||
        !statement.bindDouble(7, camera.mapZ()) ||
        !statement.bindDouble(8, camera.yawDegrees()) ||
        !statement.bindDouble(9, camera.pitchDegrees()) ||
        !statement.bindDouble(10, camera.fieldOfViewDegrees()) ||
        !statement.bindInt(11, camera.isEnabled() ? 1 : 0) ||
        !statement.execute()) {
        return setFailure(statement.lastErrorMessage());
    }

    lastErrorMessage_.clear();
    return true;
}

std::optional<domain::Camera> SqliteCameraRepository::findById(const domain::EntityId& id)
{
    database::SqliteStatement statement(
        connection_,
        QStringLiteral(R"SQL(
SELECT
    id,
    project_id,
    name,
    location_name,
    map_x,
    map_y,
    map_z,
    yaw_degrees,
    pitch_degrees,
    fov_degrees,
    enabled
FROM cameras
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
        return mapCamera(statement);
    }

    if (statement.hasError()) {
        setFailure(statement.lastErrorMessage());
        return std::nullopt;
    }

    lastErrorMessage_.clear();
    return std::nullopt;
}

QVector<domain::Camera> SqliteCameraRepository::findByProjectId(
    const domain::EntityId& projectId
)
{
    QVector<domain::Camera> cameras;

    database::SqliteStatement statement(
        connection_,
        QStringLiteral(R"SQL(
SELECT
    id,
    project_id,
    name,
    location_name,
    map_x,
    map_y,
    map_z,
    yaw_degrees,
    pitch_degrees,
    fov_degrees,
    enabled
FROM cameras
WHERE project_id = ?
ORDER BY name ASC;
)SQL")
    );

    if (!statement.isValid()) {
        setFailure(statement.lastErrorMessage());
        return cameras;
    }

    if (!statement.bindText(1, projectId.toString())) {
        setFailure(statement.lastErrorMessage());
        return cameras;
    }

    while (statement.step()) {
        cameras.append(mapCamera(statement));
    }

    if (statement.hasError()) {
        setFailure(statement.lastErrorMessage());
        return {};
    }

    lastErrorMessage_.clear();
    return cameras;
}

QString SqliteCameraRepository::lastErrorMessage() const
{
    return lastErrorMessage_;
}

bool SqliteCameraRepository::setFailure(const QString& message)
{
    lastErrorMessage_ = message;
    return false;
}

} // namespace geovideo::infrastructure::repositories::sqlite