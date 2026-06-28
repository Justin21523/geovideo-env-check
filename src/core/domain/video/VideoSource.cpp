#include "src/core/domain/video/VideoSource.h"

#include <QFileInfo>

namespace geovideo::domain {

VideoSource VideoSource::create(const EntityId& projectId, const QString& filePath)
{
    VideoSource source;

    const QFileInfo fileInfo(filePath);
    const QString displayName = fileInfo.fileName().trimmed();

    source.id_ = EntityId::create();
    source.projectId_ = projectId;
    source.filePath_ = filePath;
    source.displayName_ =
        displayName.isEmpty() ? QStringLiteral("Unnamed Video Source") : displayName;
    source.importedAt_ = QDateTime::currentDateTimeUtc();

    return source;
}

VideoSource VideoSource::reconstitute(
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
)
{
    VideoSource source;

    // 從資料庫還原影片來源時，不重新產生 id，也不刷新 importedAt。
    source.id_ = id;
    source.projectId_ = projectId;
    source.cameraId_ = cameraId;
    source.filePath_ = filePath;
    source.displayName_ = displayName.trimmed().isEmpty()
        ? QStringLiteral("Unnamed Video Source")
        : displayName.trimmed();
    source.width_ = qMax(0, width);
    source.height_ = qMax(0, height);
    source.fps_ = qMax(0.0, fps);
    source.durationMs_ = qMax<qint64>(0, durationMs);
    source.frameCount_ = qMax<qint64>(0, frameCount);
    source.startTime_ = startTime;
    source.importedAt_ = importedAt;

    return source;
}

const EntityId& VideoSource::id() const
{
    return id_;
}

const EntityId& VideoSource::projectId() const
{
    return projectId_;
}

const EntityId& VideoSource::cameraId() const
{
    return cameraId_;
}

QString VideoSource::filePath() const
{
    return filePath_;
}

QString VideoSource::displayName() const
{
    return displayName_;
}

int VideoSource::width() const
{
    return width_;
}

int VideoSource::height() const
{
    return height_;
}

double VideoSource::fps() const
{
    return fps_;
}

qint64 VideoSource::durationMs() const
{
    return durationMs_;
}

qint64 VideoSource::frameCount() const
{
    return frameCount_;
}

QDateTime VideoSource::startTime() const
{
    return startTime_;
}

QDateTime VideoSource::importedAt() const
{
    return importedAt_;
}

void VideoSource::setCameraId(const EntityId& cameraId)
{
    cameraId_ = cameraId;
}

void VideoSource::setDisplayName(const QString& displayName)
{
    displayName_ = displayName.trimmed();

    if (displayName_.isEmpty()) {
        displayName_ = QStringLiteral("Unnamed Video Source");
    }
}

void VideoSource::setMediaInfo(
    int width,
    int height,
    double fps,
    qint64 durationMs,
    qint64 frameCount
)
{
    // 媒體資訊可能來自 Qt Multimedia 或 OpenCV，進 domain 前先保守修正成合理值。
    width_ = qMax(0, width);
    height_ = qMax(0, height);
    fps_ = qMax(0.0, fps);
    durationMs_ = qMax<qint64>(0, durationMs);
    frameCount_ = qMax<qint64>(0, frameCount);
}

void VideoSource::setStartTime(const QDateTime& startTime)
{
    startTime_ = startTime;
}

bool VideoSource::hasFilePath() const
{
    return !filePath_.trimmed().isEmpty();
}

bool VideoSource::hasValidMediaInfo() const
{
    return width_ > 0 && height_ > 0 && fps_ > 0.0;
}

QString VideoSource::resolutionText() const
{
    if (width_ <= 0 || height_ <= 0) {
        return QStringLiteral("Unknown Resolution");
    }

    return QStringLiteral("%1 x %2").arg(width_).arg(height_);
}

QString VideoSource::durationText() const
{
    const qint64 totalSeconds = durationMs_ / 1000;
    const qint64 hours = totalSeconds / 3600;
    const qint64 minutes = (totalSeconds % 3600) / 60;
    const qint64 seconds = totalSeconds % 60;

    return QStringLiteral("%1:%2:%3")
        .arg(hours, 2, 10, QLatin1Char('0'))
        .arg(minutes, 2, 10, QLatin1Char('0'))
        .arg(seconds, 2, 10, QLatin1Char('0'));
}

} // namespace geovideo::domain
