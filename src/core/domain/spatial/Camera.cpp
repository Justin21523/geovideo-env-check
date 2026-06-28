#include "src/core/domain/spatial/Camera.h"

#include <QtGlobal>

namespace geovideo::domain {

Camera Camera::create(const EntityId& projectId, const QString& name)
{
    Camera camera;

    // Camera 是場域視覺化與影片來源之間的橋，先只保留必要定位資訊。
    camera.id_ = EntityId::create();
    camera.projectId_ = projectId;
    camera.name_ = name.trimmed();

    return camera;
}

Camera Camera::reconstitute(
    const EntityId& id,
    const EntityId& projectId,
    const QString& name,
    const QString& locationName,
    double mapX,
    double mapY,
    double mapZ,
    double yawDegrees,
    double pitchDegrees,
    double fieldOfViewDegrees,
    bool enabled
)
{
    Camera camera;

    // Camera 需要從資料庫還原地圖位置與朝向，供 Spatial View 使用。
    camera.id_ = id;
    camera.projectId_ = projectId;
    camera.name_ = name.trimmed();
    camera.locationName_ = locationName.trimmed();
    camera.mapX_ = mapX;
    camera.mapY_ = mapY;
    camera.mapZ_ = mapZ;
    camera.yawDegrees_ = yawDegrees;
    camera.pitchDegrees_ = pitchDegrees;
    camera.fieldOfViewDegrees_ = qBound(1.0, fieldOfViewDegrees, 179.0);
    camera.enabled_ = enabled;

    return camera;
}

const EntityId& Camera::id() const
{
    return id_;
}

const EntityId& Camera::projectId() const
{
    return projectId_;
}

QString Camera::name() const
{
    return name_;
}

QString Camera::locationName() const
{
    return locationName_;
}

double Camera::mapX() const
{
    return mapX_;
}

double Camera::mapY() const
{
    return mapY_;
}

double Camera::mapZ() const
{
    return mapZ_;
}

double Camera::yawDegrees() const
{
    return yawDegrees_;
}

double Camera::pitchDegrees() const
{
    return pitchDegrees_;
}

double Camera::fieldOfViewDegrees() const
{
    return fieldOfViewDegrees_;
}

bool Camera::isEnabled() const
{
    return enabled_;
}

void Camera::setName(const QString& name)
{
    name_ = name.trimmed();
}

void Camera::setLocationName(const QString& locationName)
{
    locationName_ = locationName.trimmed();
}

void Camera::setMapPosition(double x, double y, double z)
{
    mapX_ = x;
    mapY_ = y;
    mapZ_ = z;
}

void Camera::setOrientation(double yawDegrees, double pitchDegrees)
{
    yawDegrees_ = yawDegrees;
    pitchDegrees_ = pitchDegrees;
}

void Camera::setFieldOfViewDegrees(double fieldOfViewDegrees)
{
    // FOV 給一個合理範圍，避免場域視覺化時視角錐畫到完全失真。
    fieldOfViewDegrees_ = qBound(1.0, fieldOfViewDegrees, 179.0);
}

void Camera::setEnabled(bool enabled)
{
    enabled_ = enabled;
}

bool Camera::hasName() const
{
    return !name_.isEmpty();
}

QString Camera::displayName() const
{
    return hasName() ? name_ : QStringLiteral("Unnamed Camera");
}

} // namespace geovideo::domain