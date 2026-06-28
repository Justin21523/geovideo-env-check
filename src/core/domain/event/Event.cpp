#include "src/core/domain/event/Event.h"

#include <QtGlobal>

namespace geovideo::domain {

bool BoundingBox::isValid() const
{
    return width > 0 && height > 0;
}

QString eventTypeToDisplayText(EventType type)
{
    switch (type) {
    case EventType::Unknown:
        return QStringLiteral("Unknown");
    case EventType::MotionDetected:
        return QStringLiteral("Motion Detected");
    case EventType::RoiIntrusion:
        return QStringLiteral("ROI Intrusion");
    case EventType::LineCrossing:
        return QStringLiteral("Line Crossing");
    case EventType::ManualBookmark:
        return QStringLiteral("Manual Bookmark");
    case EventType::SnapshotCaptured:
        return QStringLiteral("Snapshot Captured");
    case EventType::ClipExported:
        return QStringLiteral("Clip Exported");
    case EventType::Loitering:
        return QStringLiteral("Loitering");
    }

    return QStringLiteral("Unknown");
}

EventType eventTypeFromDisplayText(const QString& value, EventType fallback)
{
    const QString normalized = value.trimmed().toLower();

    // 支援 UI 顯示文字與資料庫簡化名稱，讓未來搜尋與匯入更有彈性。
    if (normalized == QStringLiteral("motion detected") || normalized == QStringLiteral("motiondetected")) {
        return EventType::MotionDetected;
    }

    if (normalized == QStringLiteral("roi intrusion") || normalized == QStringLiteral("roiintrusion")) {
        return EventType::RoiIntrusion;
    }

    if (normalized == QStringLiteral("line crossing") || normalized == QStringLiteral("linecrossing")) {
        return EventType::LineCrossing;
    }

    if (normalized == QStringLiteral("manual bookmark") ||
        normalized == QStringLiteral("manualbookmark")) {
        return EventType::ManualBookmark;
    }

    if (normalized == QStringLiteral("snapshot captured") ||
        normalized == QStringLiteral("snapshotcaptured")) {
        return EventType::SnapshotCaptured;
    }

    if (normalized == QStringLiteral("clip exported") || normalized == QStringLiteral("clipexported")) {
        return EventType::ClipExported;
    }

    if (normalized == QStringLiteral("loitering")) {
        return EventType::Loitering;
    }

    if (normalized == QStringLiteral("unknown")) {
        return EventType::Unknown;
    }

    return fallback;
}

QString eventReviewStatusToDisplayText(EventReviewStatus status)
{
    switch (status) {
    case EventReviewStatus::Unreviewed:
        return QStringLiteral("Unreviewed");
    case EventReviewStatus::Confirmed:
        return QStringLiteral("Confirmed");
    case EventReviewStatus::FalsePositive:
        return QStringLiteral("False Positive");
    case EventReviewStatus::Ignored:
        return QStringLiteral("Ignored");
    }

    return QStringLiteral("Unreviewed");
}

EventReviewStatus eventReviewStatusFromDisplayText(
    const QString& value,
    EventReviewStatus fallback
)
{
    const QString normalized = value.trimmed().toLower();

    if (normalized == QStringLiteral("unreviewed")) {
        return EventReviewStatus::Unreviewed;
    }

    if (normalized == QStringLiteral("confirmed")) {
        return EventReviewStatus::Confirmed;
    }

    if (normalized == QStringLiteral("false positive") ||
        normalized == QStringLiteral("falsepositive")) {
        return EventReviewStatus::FalsePositive;
    }

    if (normalized == QStringLiteral("ignored")) {
        return EventReviewStatus::Ignored;
    }

    return fallback;
}

Event Event::create(
    const EntityId& projectId,
    const EntityId& videoSourceId,
    EventType type,
    const TimeRange& timeRange
)
{
    Event event;

    // Event 是後續檢索、時間軸、報告輸出的核心資料單位。
    event.id_ = EntityId::create();
    event.projectId_ = projectId;
    event.videoSourceId_ = videoSourceId;
    event.type_ = type;
    event.timeRange_ = timeRange;
    event.createdAt_ = QDateTime::currentDateTimeUtc();

    return event;
}

Event Event::reconstitute(
    const EntityId& id,
    const EntityId& projectId,
    const EntityId& videoSourceId,
    const EntityId& cameraId,
    const EntityId& roiId,
    EventType type,
    Severity severity,
    EventReviewStatus reviewStatus,
    const TimeRange& timeRange,
    qint64 frameStart,
    qint64 frameEnd,
    const BoundingBox& boundingBox,
    const QString& trackId,
    double motionScore,
    const QString& snapshotPath,
    const QString& clipPath,
    const QString& note,
    const QStringList& tags,
    const QDateTime& createdAt
)
{
    Event event;

    // Event 是檢索與報告核心，從資料庫還原時必須保留完整 metadata。
    event.id_ = id;
    event.projectId_ = projectId;
    event.videoSourceId_ = videoSourceId;
    event.cameraId_ = cameraId;
    event.roiId_ = roiId;
    event.type_ = type;
    event.severity_ = severity;
    event.reviewStatus_ = reviewStatus;
    event.timeRange_ = timeRange;
    event.frameStart_ = qMax<qint64>(0, frameStart);
    event.frameEnd_ = qMax<qint64>(event.frameStart_, frameEnd);
    event.boundingBox_ = boundingBox;
    event.trackId_ = trackId.trimmed();
    event.motionScore_ = qMax(0.0, motionScore);
    event.snapshotPath_ = snapshotPath;
    event.clipPath_ = clipPath;
    event.note_ = note.trimmed();
    event.tags_ = tags;
    event.createdAt_ = createdAt;

    return event;
}

const EntityId& Event::id() const
{
    return id_;
}

const EntityId& Event::projectId() const
{
    return projectId_;
}

const EntityId& Event::videoSourceId() const
{
    return videoSourceId_;
}

const EntityId& Event::cameraId() const
{
    return cameraId_;
}

const EntityId& Event::roiId() const
{
    return roiId_;
}

EventType Event::type() const
{
    return type_;
}

Severity Event::severity() const
{
    return severity_;
}

EventReviewStatus Event::reviewStatus() const
{
    return reviewStatus_;
}

TimeRange Event::timeRange() const
{
    return timeRange_;
}

qint64 Event::frameStart() const
{
    return frameStart_;
}

qint64 Event::frameEnd() const
{
    return frameEnd_;
}

BoundingBox Event::boundingBox() const
{
    return boundingBox_;
}

QString Event::trackId() const
{
    return trackId_;
}

double Event::motionScore() const
{
    return motionScore_;
}

QString Event::snapshotPath() const
{
    return snapshotPath_;
}

QString Event::clipPath() const
{
    return clipPath_;
}

QString Event::note() const
{
    return note_;
}

QStringList Event::tags() const
{
    return tags_;
}

QDateTime Event::createdAt() const
{
    return createdAt_;
}

void Event::setCameraId(const EntityId& cameraId)
{
    cameraId_ = cameraId;
}

void Event::setRoiId(const EntityId& roiId)
{
    roiId_ = roiId;
}

void Event::setType(EventType type)
{
    type_ = type;
}

void Event::setSeverity(Severity severity)
{
    severity_ = severity;
}

void Event::setReviewStatus(EventReviewStatus status)
{
    reviewStatus_ = status;
}

void Event::setTimeRange(const TimeRange& timeRange)
{
    timeRange_ = timeRange;
}

void Event::setFrames(qint64 frameStart, qint64 frameEnd)
{
    // frame index 不應該小於 0，這裡先修正，避免 timeline 或資料庫出現負 frame。
    frameStart_ = qMax<qint64>(0, frameStart);
    frameEnd_ = qMax<qint64>(frameStart_, frameEnd);
}

void Event::setBoundingBox(const BoundingBox& boundingBox)
{
    boundingBox_ = boundingBox;
}

void Event::setTrackId(const QString& trackId)
{
    trackId_ = trackId.trimmed();
}

void Event::setMotionScore(double motionScore)
{
    motionScore_ = qMax(0.0, motionScore);
}

void Event::setSnapshotPath(const QString& snapshotPath)
{
    snapshotPath_ = snapshotPath;
}

void Event::setClipPath(const QString& clipPath)
{
    clipPath_ = clipPath;
}

void Event::setNote(const QString& note)
{
    note_ = note.trimmed();
}

void Event::setTags(const QStringList& tags)
{
    tags_.clear();

    // tag 進 domain 前先去空白與空值，讓搜尋資料後續比較乾淨。
    for (const QString& tag : tags) {
        const QString normalizedTag = tag.trimmed();

        if (!normalizedTag.isEmpty()) {
            tags_.append(normalizedTag);
        }
    }

    tags_.removeDuplicates();
}

bool Event::hasSnapshot() const
{
    return !snapshotPath_.trimmed().isEmpty();
}

bool Event::hasClip() const
{
    return !clipPath_.trimmed().isEmpty();
}

bool Event::hasRoi() const
{
    return roiId_.isValid();
}

bool Event::hasCamera() const
{
    return cameraId_.isValid();
}

QString Event::title() const
{
    return QStringLiteral("%1 at %2 ms")
        .arg(eventTypeToDisplayText(type_))
        .arg(timeRange_.startMs());
}

} // namespace geovideo::domain