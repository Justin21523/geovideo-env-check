#include "src/application/services/EventReviewService.h"

namespace geovideo::application::services {

EventReviewService::EventReviewService(core::repositories::EventRepository& eventRepository)
    : eventRepository_(eventRepository)
{
}

ServiceResult<std::optional<domain::Event>> EventReviewService::findEventById(
    const domain::EntityId& id
)
{
    if (!id.isValid()) {
        return ServiceResult<std::optional<domain::Event>>::failure(
            QStringLiteral("Event id is invalid.")
        );
    }

    std::optional<domain::Event> event = eventRepository_.findById(id);

    if (!event.has_value() && !eventRepository_.lastErrorMessage().isEmpty()) {
        return ServiceResult<std::optional<domain::Event>>::failure(
            eventRepository_.lastErrorMessage()
        );
    }

    return ServiceResult<std::optional<domain::Event>>::success(event);
}

ServiceResult<QVector<domain::Event>> EventReviewService::listEventsByProjectId(
    const domain::EntityId& projectId
)
{
    if (!projectId.isValid()) {
        return ServiceResult<QVector<domain::Event>>::failure(
            QStringLiteral("Project id is invalid.")
        );
    }

    QVector<domain::Event> events = eventRepository_.findByProjectId(projectId);

    if (!eventRepository_.lastErrorMessage().isEmpty()) {
        return ServiceResult<QVector<domain::Event>>::failure(
            eventRepository_.lastErrorMessage()
        );
    }

    return ServiceResult<QVector<domain::Event>>::success(events);
}

ServiceResult<QVector<domain::Event>> EventReviewService::listEventsByVideoSourceId(
    const domain::EntityId& videoSourceId
)
{
    if (!videoSourceId.isValid()) {
        return ServiceResult<QVector<domain::Event>>::failure(
            QStringLiteral("Video source id is invalid.")
        );
    }

    QVector<domain::Event> events = eventRepository_.findByVideoSourceId(videoSourceId);

    if (!eventRepository_.lastErrorMessage().isEmpty()) {
        return ServiceResult<QVector<domain::Event>>::failure(
            eventRepository_.lastErrorMessage()
        );
    }

    return ServiceResult<QVector<domain::Event>>::success(events);
}

} // namespace geovideo::application::services