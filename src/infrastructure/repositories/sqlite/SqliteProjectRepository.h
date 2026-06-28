#pragma once

#include "src/core/repositories/ProjectRepository.h"

namespace geovideo::infrastructure::database {
class DatabaseConnection;
}

namespace geovideo::infrastructure::repositories::sqlite {

class SqliteProjectRepository final : public core::repositories::ProjectRepository {
public:
    explicit SqliteProjectRepository(database::DatabaseConnection& connection);

    bool save(const domain::Project& project) override;
    std::optional<domain::Project> findById(const domain::EntityId& id) override;
    QVector<domain::Project> findAll() override;

    QString lastErrorMessage() const override;

private:
    bool setFailure(const QString& message);

private:
    database::DatabaseConnection& connection_;
    QString lastErrorMessage_;
};

} // namespace geovideo::infrastructure::repositories::sqlite