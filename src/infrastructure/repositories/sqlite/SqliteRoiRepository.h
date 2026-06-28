#pragma once

#include "src/core/repositories/RoiRepository.h"

namespace geovideo::infrastructure::database {
class DatabaseConnection;
}

namespace geovideo::infrastructure::repositories::sqlite {

class SqliteRoiRepository final : public core::repositories::RoiRepository {
public:
    explicit SqliteRoiRepository(database::DatabaseConnection& connection);

    bool save(const domain::Roi& roi) override;
    std::optional<domain::Roi> findById(const domain::EntityId& id) override;
    QVector<domain::Roi> findByVideoSourceId(const domain::EntityId& videoSourceId) override;

    QString lastErrorMessage() const override;

private:
    bool setFailure(const QString& message);

private:
    database::DatabaseConnection& connection_;
    QString lastErrorMessage_;
};

} // namespace geovideo::infrastructure::repositories::sqlite