#pragma once

#include "src/application/common/ServiceResult.h"
#include "src/core/domain/common/EntityId.h"
#include "src/core/domain/video/VideoSource.h"
#include "src/core/repositories/ProjectRepository.h"
#include "src/core/repositories/VideoSourceRepository.h"
#include "src/video/VideoMetadataReader.h"

#include <QString>

namespace geovideo::application::services {

class VideoImportService final {
public:
    VideoImportService(
        core::repositories::ProjectRepository& projectRepository,
        core::repositories::VideoSourceRepository& videoSourceRepository,
        video::VideoMetadataReader& metadataReader
    );

    ServiceResult<domain::VideoSource> registerVideoSource(
        const domain::EntityId& projectId,
        const QString& filePath
    );

private:
    core::repositories::ProjectRepository& projectRepository_;
    core::repositories::VideoSourceRepository& videoSourceRepository_;
    video::VideoMetadataReader& metadataReader_;
};

} // namespace geovideo::application::services