#pragma once

#include "src/application/services/EventReviewService.h"
#include "src/application/services/ProjectService.h"
#include "src/application/services/VideoImportService.h"
#include "src/infrastructure/database/DatabaseManager.h"
#include "src/infrastructure/repositories/sqlite/SqliteCameraRepository.h"
#include "src/infrastructure/repositories/sqlite/SqliteEventRepository.h"
#include "src/infrastructure/repositories/sqlite/SqliteProjectRepository.h"
#include "src/infrastructure/repositories/sqlite/SqliteRoiRepository.h"
#include "src/infrastructure/repositories/sqlite/SqliteVideoSourceRepository.h"
#include "src/video/VideoMetadataReader.h"

#include <QString>

#include <memory>

namespace geovideo {

class AppContext final {
public:
    AppContext() = default;

    AppContext(const AppContext&) = delete;
    AppContext& operator=(const AppContext&) = delete;

    bool initialize();

    bool isInitialized() const;
    QString databasePath() const;
    QString lastErrorMessage() const;
    int schemaVersion() const;

    application::services::ProjectService& projectService();
    application::services::VideoImportService& videoImportService();
    application::services::EventReviewService& eventReviewService();

private:
    void createRepositories();
    void createVideoReaders();
    void createServices();

private:
    infrastructure::database::DatabaseManager databaseManager_;

    std::unique_ptr<infrastructure::repositories::sqlite::SqliteProjectRepository>
        projectRepository_;
    std::unique_ptr<infrastructure::repositories::sqlite::SqliteCameraRepository>
        cameraRepository_;
    std::unique_ptr<infrastructure::repositories::sqlite::SqliteVideoSourceRepository>
        videoSourceRepository_;
    std::unique_ptr<infrastructure::repositories::sqlite::SqliteRoiRepository> roiRepository_;
    std::unique_ptr<infrastructure::repositories::sqlite::SqliteEventRepository> eventRepository_;

    std::unique_ptr<video::VideoMetadataReader> videoMetadataReader_;

    std::unique_ptr<application::services::ProjectService> projectService_;
    std::unique_ptr<application::services::VideoImportService> videoImportService_;
    std::unique_ptr<application::services::EventReviewService> eventReviewService_;

    QString lastErrorMessage_;
    bool initialized_ = false;
};

} // namespace geovideo