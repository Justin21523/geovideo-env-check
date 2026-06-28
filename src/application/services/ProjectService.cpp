#include "src/application/services/ProjectService.h"

#include <QString>

namespace geovideo::application::services {

ProjectService::ProjectService(core::repositories::ProjectRepository& projectRepository)
    : projectRepository_(projectRepository)
{
}

ServiceResult<domain::Project> ProjectService::createProject(
    const QString& name,
    const QString& baseDirectory,
    const QString& description
)
{
    const QString trimmedName = name.trimmed();

    if (trimmedName.isEmpty()) {
        return ServiceResult<domain::Project>::failure(
            QStringLiteral("Project name cannot be empty.")
        );
    }

    // Service 負責使用情境規則，Domain 負責資料本身，Repository 負責保存。
    domain::Project project = domain::Project::create(trimmedName, baseDirectory, description);

    if (!projectRepository_.save(project)) {
        return ServiceResult<domain::Project>::failure(projectRepository_.lastErrorMessage());
    }

    return ServiceResult<domain::Project>::success(project);
}

ServiceResult<std::optional<domain::Project>> ProjectService::findProjectById(
    const domain::EntityId& id
)
{
    if (!id.isValid()) {
        return ServiceResult<std::optional<domain::Project>>::failure(
            QStringLiteral("Project id is invalid.")
        );
    }

    std::optional<domain::Project> project = projectRepository_.findById(id);

    if (!project.has_value() && !projectRepository_.lastErrorMessage().isEmpty()) {
        return ServiceResult<std::optional<domain::Project>>::failure(
            projectRepository_.lastErrorMessage()
        );
    }

    return ServiceResult<std::optional<domain::Project>>::success(project);
}

ServiceResult<QVector<domain::Project>> ProjectService::listProjects()
{
    QVector<domain::Project> projects = projectRepository_.findAll();

    if (!projectRepository_.lastErrorMessage().isEmpty()) {
        return ServiceResult<QVector<domain::Project>>::failure(
            projectRepository_.lastErrorMessage()
        );
    }

    return ServiceResult<QVector<domain::Project>>::success(projects);
}

} // namespace geovideo::application::services