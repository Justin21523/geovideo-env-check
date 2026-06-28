#include "src/infrastructure/database/SchemaMigrator.h"

#include "src/infrastructure/database/DatabaseConnection.h"

#include <QDebug>

namespace geovideo::infrastructure::database {

bool SchemaMigrator::migrate(DatabaseConnection& connection)
{
    appliedSteps_.clear();
    lastErrorMessage_.clear();

    const int currentVersion = connection.userVersion();

    if (currentVersion < 0) {
        lastErrorMessage_ = connection.lastErrorMessage();
        return false;
    }

    if (currentVersion > CurrentSchemaVersion) {
        lastErrorMessage_ = QStringLiteral(
            "Database schema version is newer than this application supports."
        );
        return false;
    }

    if (currentVersion == CurrentSchemaVersion) {
        qInfo() << "Database schema is already up to date.";
        return true;
    }

    if (currentVersion < 1 && !migrateToVersion1(connection)) {
        return false;
    }

    qInfo().noquote() << "Database migration completed. Applied steps:" << appliedSteps_.join(", ");
    return true;
}

QString SchemaMigrator::lastErrorMessage() const
{
    return lastErrorMessage_;
}

QStringList SchemaMigrator::appliedSteps() const
{
    return appliedSteps_;
}

bool SchemaMigrator::migrateToVersion1(DatabaseConnection& connection)
{
    const QString sql = QStringLiteral(R"SQL(
CREATE TABLE IF NOT EXISTS schema_migrations (
    version INTEGER PRIMARY KEY,
    name TEXT NOT NULL,
    applied_at TEXT NOT NULL
);

CREATE TABLE IF NOT EXISTS projects (
    id TEXT PRIMARY KEY,
    name TEXT NOT NULL,
    description TEXT NOT NULL DEFAULT '',
    base_directory TEXT NOT NULL DEFAULT '',
    created_at TEXT NOT NULL,
    updated_at TEXT NOT NULL
);

CREATE TABLE IF NOT EXISTS cameras (
    id TEXT PRIMARY KEY,
    project_id TEXT NOT NULL,
    name TEXT NOT NULL,
    location_name TEXT NOT NULL DEFAULT '',
    map_x REAL NOT NULL DEFAULT 0,
    map_y REAL NOT NULL DEFAULT 0,
    map_z REAL NOT NULL DEFAULT 0,
    yaw_degrees REAL NOT NULL DEFAULT 0,
    pitch_degrees REAL NOT NULL DEFAULT 0,
    fov_degrees REAL NOT NULL DEFAULT 60,
    enabled INTEGER NOT NULL DEFAULT 1,
    FOREIGN KEY(project_id) REFERENCES projects(id) ON DELETE CASCADE
);

CREATE TABLE IF NOT EXISTS video_sources (
    id TEXT PRIMARY KEY,
    project_id TEXT NOT NULL,
    camera_id TEXT,
    file_path TEXT NOT NULL,
    display_name TEXT NOT NULL,
    width INTEGER NOT NULL DEFAULT 0,
    height INTEGER NOT NULL DEFAULT 0,
    fps REAL NOT NULL DEFAULT 0,
    duration_ms INTEGER NOT NULL DEFAULT 0,
    frame_count INTEGER NOT NULL DEFAULT 0,
    start_time TEXT NOT NULL DEFAULT '',
    imported_at TEXT NOT NULL,
    FOREIGN KEY(project_id) REFERENCES projects(id) ON DELETE CASCADE,
    FOREIGN KEY(camera_id) REFERENCES cameras(id) ON DELETE SET NULL
);

CREATE TABLE IF NOT EXISTS rois (
    id TEXT PRIMARY KEY,
    video_source_id TEXT NOT NULL,
    name TEXT NOT NULL,
    type TEXT NOT NULL,
    severity TEXT NOT NULL,
    points_json TEXT NOT NULL DEFAULT '[]',
    rule_json TEXT NOT NULL DEFAULT '{}',
    enabled INTEGER NOT NULL DEFAULT 1,
    FOREIGN KEY(video_source_id) REFERENCES video_sources(id) ON DELETE CASCADE
);

CREATE TABLE IF NOT EXISTS events (
    id TEXT PRIMARY KEY,
    project_id TEXT NOT NULL,
    video_source_id TEXT NOT NULL,
    camera_id TEXT,
    roi_id TEXT,
    type TEXT NOT NULL,
    severity TEXT NOT NULL,
    review_status TEXT NOT NULL,
    start_ms INTEGER NOT NULL DEFAULT 0,
    end_ms INTEGER NOT NULL DEFAULT 0,
    frame_start INTEGER NOT NULL DEFAULT 0,
    frame_end INTEGER NOT NULL DEFAULT 0,
    bbox_json TEXT NOT NULL DEFAULT '{}',
    track_id TEXT NOT NULL DEFAULT '',
    motion_score REAL NOT NULL DEFAULT 0,
    snapshot_path TEXT NOT NULL DEFAULT '',
    clip_path TEXT NOT NULL DEFAULT '',
    note TEXT NOT NULL DEFAULT '',
    tags_json TEXT NOT NULL DEFAULT '[]',
    created_at TEXT NOT NULL,
    FOREIGN KEY(project_id) REFERENCES projects(id) ON DELETE CASCADE,
    FOREIGN KEY(video_source_id) REFERENCES video_sources(id) ON DELETE CASCADE,
    FOREIGN KEY(camera_id) REFERENCES cameras(id) ON DELETE SET NULL,
    FOREIGN KEY(roi_id) REFERENCES rois(id) ON DELETE SET NULL
);

CREATE INDEX IF NOT EXISTS idx_cameras_project_id
    ON cameras(project_id);

CREATE INDEX IF NOT EXISTS idx_video_sources_project_id
    ON video_sources(project_id);

CREATE INDEX IF NOT EXISTS idx_video_sources_camera_id
    ON video_sources(camera_id);

CREATE INDEX IF NOT EXISTS idx_rois_video_source_id
    ON rois(video_source_id);

CREATE INDEX IF NOT EXISTS idx_events_project_id
    ON events(project_id);

CREATE INDEX IF NOT EXISTS idx_events_video_source_id
    ON events(video_source_id);

CREATE INDEX IF NOT EXISTS idx_events_camera_id
    ON events(camera_id);

CREATE INDEX IF NOT EXISTS idx_events_roi_id
    ON events(roi_id);

CREATE INDEX IF NOT EXISTS idx_events_time_range
    ON events(start_ms, end_ms);

CREATE INDEX IF NOT EXISTS idx_events_type
    ON events(type);

CREATE INDEX IF NOT EXISTS idx_events_review_status
    ON events(review_status);

INSERT OR IGNORE INTO schema_migrations(version, name, applied_at)
VALUES (1, 'initial domain schema', datetime('now'));
)SQL");

    return runInTransaction(connection, sql, 1);
}

bool SchemaMigrator::runInTransaction(
    DatabaseConnection& connection,
    const QString& sql,
    int targetVersion
)
{
    // migration 一定要包在 transaction 裡，避免建表做到一半就留下半套 schema。
    if (!connection.execute(QStringLiteral("BEGIN IMMEDIATE TRANSACTION;"))) {
        lastErrorMessage_ = connection.lastErrorMessage();
        return false;
    }

    if (!connection.execute(sql)) {
        lastErrorMessage_ = connection.lastErrorMessage();
        connection.execute(QStringLiteral("ROLLBACK;"));
        return false;
    }

    if (!connection.setUserVersion(targetVersion)) {
        lastErrorMessage_ = connection.lastErrorMessage();
        connection.execute(QStringLiteral("ROLLBACK;"));
        return false;
    }

    if (!connection.execute(QStringLiteral("COMMIT;"))) {
        lastErrorMessage_ = connection.lastErrorMessage();
        return false;
    }

    appliedSteps_.append(QStringLiteral("v%1").arg(targetVersion));
    return true;
}

} // namespace geovideo::infrastructure::database