#include "src/app/AppContext.h"

#include "src/video/opencv/OpenCvVideoMetadataReader.h"

#include <QDebug>

namespace geovideo {

bool AppContext::initialize()
{
    if (initialized_) {
        return true;
    }

    lastErrorMessage_.clear();

    if (!databaseManager_.initializeDefault()) {
        lastErrorMessage_ = databaseManager_.lastErrorMessage();
        return false;
    }

    createRepositories();
    createVideoReaders();
    createServices();

    initialized_ = true;

    qInfo().noquote()
        << "Application context initialized:"
        << databaseManager_.databasePath()
        << "| schema version:"
        << databaseManager_.schemaVersion();

    return true;
}

bool AppContext::isInitialized() const
{
    return initialized_;
}

QString AppContext::databasePath() const
{
    return databaseManager_.databasePath();
}

QString AppContext::lastErrorMessage() const
{
    return lastErrorMessage_;
}

int AppContext::schemaVersion() const
{
    return databaseManager_.schemaVersion();
}

application::services::ProjectService& AppContext::projectService()
{
    return *projectService_;
}

application::services::VideoImportService& AppContext::videoImportService()
{
    return *videoImportService_;
}

application::services::EventReviewService& AppContext::eventReviewService()
{
    return *eventReviewService_;
}

void AppContext::createRepositories()
{
    // Repository 實作集中在這裡組裝，避免 UI 或 service 直接依賴 SQLite 細節。
    projectRepository_ =
        std::make_unique<infrastructure::repositories::sqlite::SqliteProjectRepository>(
            databaseManager_.connection()
        );

    cameraRepository_ =
        std::make_unique<infrastructure::repositories::sqlite::SqliteCameraRepository>(
            databaseManager_.connection()
        );

    videoSourceRepository_ =
        std::make_unique<infrastructure::repositories::sqlite::SqliteVideoSourceRepository>(
            databaseManager_.connection()
        );

    roiRepository_ = std::make_unique<infrastructure::repositories::sqlite::SqliteRoiRepository>(
        databaseManager_.connection()
    );

    eventRepository_ =
        std::make_unique<infrastructure::repositories::sqlite::SqliteEventRepository>(
            databaseManager_.connection()
        );
}

void AppContext::createVideoReaders()
{
    // Video reader 也從 composition root 建立，application service 只依賴抽象介面。
    videoMetadataReader_ = std::make_unique<video::opencv::OpenCvVideoMetadataReader>();
}

void AppContext::createServices()
{
    // Service 只吃 repository interface 與 reader interface，保持 application layer 不綁死 SQLite/OpenCV。
    projectService_ =
        std::make_unique<application::services::ProjectService>(*projectRepository_);

    videoImportService_ = std::make_unique<application::services::VideoImportService>(
        *projectRepository_,
        *videoSourceRepository_,
        *videoMetadataReader_
    );

    eventReviewService_ =
        std::make_unique<application::services::EventReviewService>(*eventRepository_);
}

} // namespace geovideo