#pragma once

#include "src/core/domain/common/EntityId.h"
#include "src/core/domain/video/VideoSource.h"

#include <QString>
#include <QVector>

#include <optional>

namespace geovideo::core::repositories {

class VideoSourceRepository {
public:
    virtual ~VideoSourceRepository() = default;

    virtual bool save(const domain::VideoSource& videoSource) = 0;
    virtual std::optional<domain::VideoSource> findById(const domain::EntityId& id) = 0;
    virtual QVector<domain::VideoSource> findByProjectId(const domain::EntityId& projectId) = 0;

    virtual QString lastErrorMessage() const = 0;
};

} // namespace geovideo::core::repositories