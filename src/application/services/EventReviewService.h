#pragma once

#include "src/application/common/ServiceResult.h"
#include "src/core/domain/common/EntityId.h"
#include "src/core/domain/event/Event.h"
#include "src/core/repositories/EventRepository.h"

#include <QVector>

#include <optional>

namespace geovideo::application::services {

class EventReviewService final {
public:
    explicit EventReviewService(core::repositories::EventRepository& eventRepository);

    ServiceResult<std::optional<domain::Event>> findEventById(const domain::EntityId& id);
    ServiceResult<QVector<domain::Event>> listEventsByProjectId(const domain::EntityId& projectId);
    ServiceResult<QVector<domain::Event>> listEventsByVideoSourceId(
        const domain::EntityId& videoSourceId
    );

private:
    core::repositories::EventRepository& eventRepository_;
};

} // namespace geovideo::application::services