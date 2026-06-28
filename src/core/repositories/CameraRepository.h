#pragma once

#include "src/core/domain/common/EntityId.h"
#include "src/core/domain/spatial/Camera.h"

#include <QString>
#include <QVector>

#include <optional>

namespace geovideo::core::repositories {

class CameraRepository {
public:
    virtual ~CameraRepository() = default;

    virtual bool save(const domain::Camera& camera) = 0;
    virtual std::optional<domain::Camera> findById(const domain::EntityId& id) = 0;
    virtual QVector<domain::Camera> findByProjectId(const domain::EntityId& projectId) = 0;

    virtual QString lastErrorMessage() const = 0;
};

} // namespace geovideo::core::repositories
