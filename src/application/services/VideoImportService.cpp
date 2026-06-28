#include "src/application/services/VideoImportService.h"

#include <QFileInfo>

namespace geovideo::application::services {

VideoImportService::VideoImportService(
    core::repositories::ProjectRepository& projectRepository,
    core::repositories::VideoSourceRepository& videoSourceRepository,
    video::VideoMetadataReader& metadataReader
)
    : projectRepository_(projectRepository)
    , videoSourceRepository_(videoSourceRepository)
    , metadataReader_(metadataReader)
{
}

ServiceResult<domain::VideoSource> VideoImportService::registerVideoSource(
    const domain::EntityId& projectId,
    const QString& filePath
)
{
    if (!projectId.isValid()) {
        return ServiceResult<domain::VideoSource>::failure(
            QStringLiteral("Project id is invalid.")
        );
    }

    const QFileInfo fileInfo(filePath);

    if (!fileInfo.exists() || !fileInfo.isFile()) {
        return ServiceResult<domain::VideoSource>::failure(
            QStringLiteral("Video file does not exist.")
        );
    }

    const std::optional<domain::Project> project = projectRepository_.findById(projectId);

    if (!project.has_value()) {
        const QString repositoryError = projectRepository_.lastErrorMessage();

        if (!repositoryError.isEmpty()) {
            return ServiceResult<domain::VideoSource>::failure(repositoryError);
        }

        return ServiceResult<domain::VideoSource>::failure(QStringLiteral("Project was not found."));
    }

    const std::optional<video::VideoMetadata> metadata =
        metadataReader_.read(fileInfo.absoluteFilePath());

    if (!metadata.has_value()) {
        return ServiceResult<domain::VideoSource>::failure(
            QStringLiteral("Failed to read video metadata: %1")
                .arg(metadataReader_.lastErrorMessage())
        );
    }

    domain::VideoSource videoSource =
        domain::VideoSource::create(projectId, fileInfo.absoluteFilePath());

    // Phase 1.1 正式把影片 metadata 寫入 VideoSource，後續 UI 與 database 都會使用這份資料。
    videoSource.setMediaInfo(
        metadata->width(),
        metadata->height(),
        metadata->fps(),
        metadata->durationMs(),
        metadata->frameCount()
    );

    if (!videoSourceRepository_.save(videoSource)) {
        return ServiceResult<domain::VideoSource>::failure(
            videoSourceRepository_.lastErrorMessage()
        );
    }

    return ServiceResult<domain::VideoSource>::success(videoSource);
}

} // namespace geovideo::application::services