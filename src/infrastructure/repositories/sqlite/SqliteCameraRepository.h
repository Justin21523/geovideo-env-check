#pragma once

#include "src/core/repositories/CameraRepository.h"

namespace geovideo::infrastructure::database {
class DatabaseConnection;
}

namespace geovideo::infrastructure::repositories::sqlite {

class SqliteCameraRepository final : public core::repositories::CameraRepository {
public:
    explicit SqliteCameraRepository(database::DatabaseConnection& connection);

    bool save(const domain::Camera& camera) override;
    std::optional<domain::Camera> findById(const domain::EntityId& id) override;
    QVector<domain::Camera> findByProjectId(const domain::EntityId& projectId) override;

    QString lastErrorMessage() const override;

private:
    bool setFailure(const QString& message);

private:
    database::DatabaseConnection& connection_;
    QString lastErrorMessage_;
};

} // namespace geovideo::infrastructure::repositories::sqlite