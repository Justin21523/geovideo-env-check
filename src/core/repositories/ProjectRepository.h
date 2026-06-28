#pragma once

#include "src/core/domain/common/EntityId.h"
#include "src/core/domain/project/Project.h"

#include <QString>
#include <QVector>

#include <optional>

namespace geovideo::core::repositories {

class ProjectRepository {
public:
    virtual ~ProjectRepository() = default;

    virtual bool save(const domain::Project& project) = 0;
    virtual std::optional<domain::Project> findById(const domain::EntityId& id) = 0;
    virtual QVector<domain::Project> findAll() = 0;

    virtual QString lastErrorMessage() const = 0;
};

} // namespace geovideo::core::repositories