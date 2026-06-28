#pragma once

#include "src/core/domain/common/EntityId.h"

#include <QDateTime>
#include <QString>
#include <QtGlobal>

namespace geovideo::domain {

class VideoSource final {
public:
    VideoSource() = default;

    static VideoSource create(const EntityId& projectId, const QString& filePath);

    static VideoSource reconstitute(
        const EntityId& id,
        const EntityId& projectId,
        const EntityId& cameraId,
        const QString& filePath,
        const QString& displayName,
        int width,
        int height,
        double fps,
        qint64 durationMs,
        qint64 frameCount,
        const QDateTime& startTime,
        const QDateTime& importedAt
    );
    
    const EntityId& id() const;
    const EntityId& projectId() const;
    const EntityId& cameraId() const;

    QString filePath() const;
    QString displayName() const;

    int width() const;
    int height() const;
    double fps() const;
    qint64 durationMs() const;
    qint64 frameCount() const;

    QDateTime startTime() const;
    QDateTime importedAt() const;

    void setCameraId(const EntityId& cameraId);
    void setDisplayName(const QString& displayName);
    void setMediaInfo(int width, int height, double fps, qint64 durationMs, qint64 frameCount);
    void setStartTime(const QDateTime& startTime);

    bool hasFilePath() const;
    bool hasValidMediaInfo() const;

    QString resolutionText() const;
    QString durationText() const;

private:
    EntityId id_;
    EntityId projectId_;
    EntityId cameraId_;

    QString filePath_;
    QString displayName_;

    int width_ = 0;
    int height_ = 0;
    double fps_ = 0.0;
    qint64 durationMs_ = 0;
    qint64 frameCount_ = 0;

    QDateTime startTime_;
    QDateTime importedAt_;
};

} // namespace geovideo::domain