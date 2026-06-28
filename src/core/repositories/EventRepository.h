#pragma once

#include "src/core/domain/common/EntityId.h"
#include "src/core/domain/event/Event.h"

#include <QString>
#include <QVector>

#include <optional>

namespace geovideo::core::repositories {

class EventRepository {
public:
    virtual ~EventRepository() = default;

    virtual bool save(const domain::Event& event) = 0;
    virtual std::optional<domain::Event> findById(const domain::EntityId& id) = 0;
    virtual QVector<domain::Event> findByProjectId(const domain::EntityId& projectId) = 0;
    virtual QVector<domain::Event> findByVideoSourceId(const domain::EntityId& videoSourceId) = 0;

    virtual QString lastErrorMessage() const = 0;
};

} // namespace geovideo::core::repositories