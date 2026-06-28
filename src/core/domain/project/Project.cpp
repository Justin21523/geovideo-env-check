#include "src/core/domain/project/Project.h"

namespace geovideo::domain {

Project Project::create(
    const QString& name,
    const QString& baseDirectory,
    const QString& description
)
{
    Project project;

    project.id_ = EntityId::create();
    project.name_ = name.trimmed();
    project.description_ = description.trimmed();
    project.baseDirectory_ = baseDirectory;
    project.createdAt_ = QDateTime::currentDateTimeUtc();
    project.updatedAt_ = project.createdAt_;

    return project;
}

Project Project::reconstitute(
    const EntityId& id,
    const QString& name,
    const QString& description,
    const QString& baseDirectory,
    const QDateTime& createdAt,
    const QDateTime& updatedAt
)
{
    Project project;

    // Repository 從資料庫還原 entity 時，必須保留原本的 id 與時間欄位。
    project.id_ = id;
    project.name_ = name.trimmed();
    project.description_ = description.trimmed();
    project.baseDirectory_ = baseDirectory;
    project.createdAt_ = createdAt;
    project.updatedAt_ = updatedAt;

    return project;
}

const EntityId& Project::id() const
{
    return id_;
}

QString Project::name() const
{
    return name_;
}

QString Project::description() const
{
    return description_;
}

QString Project::baseDirectory() const
{
    return baseDirectory_;
}

QDateTime Project::createdAt() const
{
    return createdAt_;
}

QDateTime Project::updatedAt() const
{
    return updatedAt_;
}

void Project::setName(const QString& name)
{
    name_ = name.trimmed();
    touch();
}

void Project::setDescription(const QString& description)
{
    description_ = description.trimmed();
    touch();
}

void Project::setBaseDirectory(const QString& baseDirectory)
{
    baseDirectory_ = baseDirectory;
    touch();
}

bool Project::hasName() const
{
    return !name_.isEmpty();
}

QString Project::displayTitle() const
{
    // UI 顯示名稱由 domain 提供，避免每個 widget 都自己判斷空名稱。
    return hasName() ? name_ : QStringLiteral("Untitled Project");
}

void Project::touch()
{
    updatedAt_ = QDateTime::currentDateTimeUtc();
}

} // namespace geovideo::domain
