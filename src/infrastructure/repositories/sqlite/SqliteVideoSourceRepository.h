#pragma once

#include "src/core/repositories/VideoSourceRepository.h"

namespace geovideo::infrastructure::database {
class DatabaseConnection;
}

namespace geovideo::infrastructure::repositories::sqlite {

class SqliteVideoSourceRepository final : public core::repositories::VideoSourceRepository {
public:
    explicit SqliteVideoSourceRepository(database::DatabaseConnection& connection);

    bool save(const domain::VideoSource& videoSource) override;
    std::optional<domain::VideoSource> findById(const domain::EntityId& id) override;
    QVector<domain::VideoSource> findByProjectId(const domain::EntityId& projectId) override;

    QString lastErrorMessage() const override;

private:
    bool setFailure(const QString& message);

private:
    database::DatabaseConnection& connection_;
    QString lastErrorMessage_;
};

} // namespace geovideo::infrastructure::repositories::sqlite