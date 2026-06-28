#pragma once

#include "src/core/domain/common/EntityId.h"
#include "src/core/domain/spatial/Roi.h"

#include <QString>
#include <QVector>

#include <optional>

namespace geovideo::core::repositories {

class RoiRepository {
public:
    virtual ~RoiRepository() = default;

    virtual bool save(const domain::Roi& roi) = 0;
    virtual std::optional<domain::Roi> findById(const domain::EntityId& id) = 0;
    virtual QVector<domain::Roi> findByVideoSourceId(const domain::EntityId& videoSourceId) = 0;

    virtual QString lastErrorMessage() const = 0;
};

} // namespace geovideo::core::repositories