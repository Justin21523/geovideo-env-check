#pragma once

#include "src/core/domain/common/EntityId.h"

#include <QString>

namespace geovideo::domain {

class Camera final {
public:
    Camera() = default;

    static Camera create(const EntityId& projectId, const QString& name);
    
    static Camera reconstitute(
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
    );

    const EntityId& id() const;
    const EntityId& projectId() const;

    QString name() const;
    QString locationName() const;

    double mapX() const;
    double mapY() const;
    double mapZ() const;
    double yawDegrees() const;
    double pitchDegrees() const;
    double fieldOfViewDegrees() const;

    bool isEnabled() const;

    void setName(const QString& name);
    void setLocationName(const QString& locationName);
    void setMapPosition(double x, double y, double z = 0.0);
    void setOrientation(double yawDegrees, double pitchDegrees = 0.0);
    void setFieldOfViewDegrees(double fieldOfViewDegrees);
    void setEnabled(bool enabled);

    bool hasName() const;
    QString displayName() const;

private:
    EntityId id_;
    EntityId projectId_;

    QString name_;
    QString locationName_;

    double mapX_ = 0.0;
    double mapY_ = 0.0;
    double mapZ_ = 0.0;
    double yawDegrees_ = 0.0;
    double pitchDegrees_ = 0.0;
    double fieldOfViewDegrees_ = 60.0;

    bool enabled_ = true;
};

} // namespace geovideo::domain