#pragma once

#include "src/core/domain/common/EntityId.h"
#include "src/core/domain/common/Severity.h"
#include "src/core/domain/common/TimeRange.h"

#include <QDateTime>
#include <QString>
#include <QStringList>
#include <QtGlobal>

namespace geovideo::domain {

enum class EventType {
    Unknown,
    MotionDetected,
    RoiIntrusion,
    LineCrossing,
    ManualBookmark,
    SnapshotCaptured,
    ClipExported,
    Loitering,
};

enum class EventReviewStatus {
    Unreviewed,
    Confirmed,
    FalsePositive,
    Ignored,
};

struct BoundingBox final {
    int x = 0;
    int y = 0;
    int width = 0;
    int height = 0;

    bool isValid() const;
};

QString eventTypeToDisplayText(EventType type);
EventType eventTypeFromDisplayText(const QString& value, EventType fallback = EventType::Unknown);

QString eventReviewStatusToDisplayText(EventReviewStatus status);
EventReviewStatus eventReviewStatusFromDisplayText(
    const QString& value,
    EventReviewStatus fallback = EventReviewStatus::Unreviewed
);

class Event final {
public:
    Event() = default;

    static Event create(
        const EntityId& projectId,
        const EntityId& videoSourceId,
        EventType type,
        const TimeRange& timeRange
    );

    static Event reconstitute(
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
    );

    const EntityId& id() const;
    const EntityId& projectId() const;
    const EntityId& videoSourceId() const;
    const EntityId& cameraId() const;
    const EntityId& roiId() const;

    EventType type() const;
    Severity severity() const;
    EventReviewStatus reviewStatus() const;
    TimeRange timeRange() const;

    qint64 frameStart() const;
    qint64 frameEnd() const;

    BoundingBox boundingBox() const;
    QString trackId() const;
    double motionScore() const;

    QString snapshotPath() const;
    QString clipPath() const;
    QString note() const;
    QStringList tags() const;
    QDateTime createdAt() const;

    void setCameraId(const EntityId& cameraId);
    void setRoiId(const EntityId& roiId);
    void setType(EventType type);
    void setSeverity(Severity severity);
    void setReviewStatus(EventReviewStatus status);
    void setTimeRange(const TimeRange& timeRange);
    void setFrames(qint64 frameStart, qint64 frameEnd);
    void setBoundingBox(const BoundingBox& boundingBox);
    void setTrackId(const QString& trackId);
    void setMotionScore(double motionScore);
    void setSnapshotPath(const QString& snapshotPath);
    void setClipPath(const QString& clipPath);
    void setNote(const QString& note);
    void setTags(const QStringList& tags);

    bool hasSnapshot() const;
    bool hasClip() const;
    bool hasRoi() const;
    bool hasCamera() const;

    QString title() const;

private:
    EntityId id_;
    EntityId projectId_;
    EntityId videoSourceId_;
    EntityId cameraId_;
    EntityId roiId_;

    EventType type_ = EventType::Unknown;
    Severity severity_ = Severity::Low;
    EventReviewStatus reviewStatus_ = EventReviewStatus::Unreviewed;
    TimeRange timeRange_;

    qint64 frameStart_ = 0;
    qint64 frameEnd_ = 0;

    BoundingBox boundingBox_;
    QString trackId_;
    double motionScore_ = 0.0;

    QString snapshotPath_;
    QString clipPath_;
    QString note_;
    QStringList tags_;
    QDateTime createdAt_;
};

} // namespace geovideo::domain