#include "src/core/domain/spatial/Roi.h"

namespace geovideo::domain {

QString roiTypeToDisplayText(RoiType type)
{
    switch (type) {
    case RoiType::Rectangle:
        return QStringLiteral("Rectangle");
    case RoiType::Polygon:
        return QStringLiteral("Polygon");
    case RoiType::LineCrossing:
        return QStringLiteral("Line Crossing");
    case RoiType::IgnoreZone:
        return QStringLiteral("Ignore Zone");
    case RoiType::RestrictedArea:
        return QStringLiteral("Restricted Area");
    case RoiType::CountingZone:
        return QStringLiteral("Counting Zone");
    }

    return QStringLiteral("Rectangle");
}

RoiType roiTypeFromDisplayText(const QString& value, RoiType fallback)
{
    const QString normalized = value.trimmed().toLower();

    // 這裡同時支援 UI 顯示文字與資料庫可能保存的簡化文字。
    if (normalized == QStringLiteral("rectangle")) {
        return RoiType::Rectangle;
    }

    if (normalized == QStringLiteral("polygon")) {
        return RoiType::Polygon;
    }

    if (normalized == QStringLiteral("line crossing") || normalized == QStringLiteral("linecrossing")) {
        return RoiType::LineCrossing;
    }

    if (normalized == QStringLiteral("ignore zone") || normalized == QStringLiteral("ignorezone")) {
        return RoiType::IgnoreZone;
    }

    if (normalized == QStringLiteral("restricted area") ||
        normalized == QStringLiteral("restrictedarea")) {
        return RoiType::RestrictedArea;
    }

    if (normalized == QStringLiteral("counting zone") || normalized == QStringLiteral("countingzone")) {
        return RoiType::CountingZone;
    }

    return fallback;
}

Roi Roi::create(const EntityId& videoSourceId, const QString& name, RoiType type)
{
    Roi roi;

    // ROI 綁定到影片來源，因為同一台 camera 的不同影片也可能有不同畫面裁切或解析度。
    roi.id_ = EntityId::create();
    roi.videoSourceId_ = videoSourceId;
    roi.name_ = name.trimmed();
    roi.type_ = type;

    return roi;
}

Roi Roi::reconstitute(
    const EntityId& id,
    const EntityId& videoSourceId,
    const QString& name,
    RoiType type,
    Severity severity,
    const QVector<RoiPoint>& points,
    const QString& ruleJson,
    bool enabled
)
{
    Roi roi;

    // ROI 從資料庫還原時，需要保留 shape points 與 ruleJson。
    roi.id_ = id;
    roi.videoSourceId_ = videoSourceId;
    roi.name_ = name.trimmed();
    roi.type_ = type;
    roi.severity_ = severity;
    roi.points_ = points;
    roi.ruleJson_ = ruleJson.trimmed().isEmpty() ? QStringLiteral("{}") : ruleJson.trimmed();
    roi.enabled_ = enabled;

    return roi;
}

const EntityId& Roi::id() const
{
    return id_;
}

const EntityId& Roi::videoSourceId() const
{
    return videoSourceId_;
}

QString Roi::name() const
{
    return name_;
}

RoiType Roi::type() const
{
    return type_;
}

Severity Roi::severity() const
{
    return severity_;
}

QVector<RoiPoint> Roi::points() const
{
    return points_;
}

QString Roi::ruleJson() const
{
    return ruleJson_;
}

bool Roi::isEnabled() const
{
    return enabled_;
}

void Roi::setName(const QString& name)
{
    name_ = name.trimmed();
}

void Roi::setType(RoiType type)
{
    type_ = type;
}

void Roi::setSeverity(Severity severity)
{
    severity_ = severity;
}

void Roi::setEnabled(bool enabled)
{
    enabled_ = enabled;
}

void Roi::setPoints(const QVector<RoiPoint>& points)
{
    points_ = points;
}

void Roi::addPoint(const RoiPoint& point)
{
    points_.append(point);
}

void Roi::clearPoints()
{
    points_.clear();
}

void Roi::setRuleJson(const QString& ruleJson)
{
    // ruleJson 先用字串保留擴充性，正式 schema 會在 ROI editor 階段再補。
    const QString trimmedRule = ruleJson.trimmed();
    ruleJson_ = trimmedRule.isEmpty() ? QStringLiteral("{}") : trimmedRule;
}

bool Roi::hasName() const
{
    return !name_.isEmpty();
}

bool Roi::hasDrawableShape() const
{
    // 不同 ROI 類型需要不同最低點數，避免後續 painter 或 OpenCV mask 建立時出錯。
    if (type_ == RoiType::LineCrossing) {
        return points_.size() >= 2;
    }

    if (type_ == RoiType::Rectangle) {
        return points_.size() >= 2;
    }

    return points_.size() >= 3;
}

QString Roi::displayName() const
{
    return hasName() ? name_ : QStringLiteral("Unnamed ROI");
}

} // namespace geovideo::domain