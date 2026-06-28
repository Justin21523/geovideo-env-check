#pragma once

#include "src/core/repositories/EventRepository.h"

namespace geovideo::infrastructure::database {
class DatabaseConnection;
}

namespace geovideo::infrastructure::repositories::sqlite {

class SqliteEventRepository final : public core::repositories::EventRepository {
public:
    explicit SqliteEventRepository(database::DatabaseConnection& connection);

    bool save(const domain::Event& event) override;
    std::optional<domain::Event> findById(const domain::EntityId& id) override;
    QVector<domain::Event> findByProjectId(const domain::EntityId& projectId) override;
    QVector<domain::Event> findByVideoSourceId(const domain::EntityId& videoSourceId) override;

    QString lastErrorMessage() const override;

private:
    bool setFailure(const QString& message);

private:
    database::DatabaseConnection& connection_;
    QString lastErrorMessage_;
};

} // namespace geovideo::infrastructure::repositories::sqlite