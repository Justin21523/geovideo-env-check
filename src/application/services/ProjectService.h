#pragma once

#include "src/application/common/ServiceResult.h"
#include "src/core/domain/common/EntityId.h"
#include "src/core/domain/project/Project.h"
#include "src/core/repositories/ProjectRepository.h"

#include <QString>
#include <QVector>

#include <optional>

namespace geovideo::application::services {

class ProjectService final {
public:
    explicit ProjectService(core::repositories::ProjectRepository& projectRepository);

    ServiceResult<domain::Project> createProject(
        const QString& name,
        const QString& baseDirectory = {},
        const QString& description = {}
    );

    ServiceResult<std::optional<domain::Project>> findProjectById(const domain::EntityId& id);
    ServiceResult<QVector<domain::Project>> listProjects();

private:
    core::repositories::ProjectRepository& projectRepository_;
};

} // namespace geovideo::application::services