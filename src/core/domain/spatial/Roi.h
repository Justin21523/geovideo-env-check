#pragma once

#include "src/core/domain/common/EntityId.h"
#include "src/core/domain/common/Severity.h"

#include <QString>
#include <QVector>

namespace geovideo::domain {

enum class RoiType {
    Rectangle,
    Polygon,
    LineCrossing,
    IgnoreZone,
    RestrictedArea,
    CountingZone,
};

struct RoiPoint final {
    double x = 0.0;
    double y = 0.0;
};

QString roiTypeToDisplayText(RoiType type);
RoiType roiTypeFromDisplayText(const QString& value, RoiType fallback = RoiType::Rectangle);

class Roi final {
public:
    Roi() = default;

    static Roi create(
        const EntityId& videoSourceId,
        const QString& name,
        RoiType type = RoiType::Rectangle
    );
    
    static Roi reconstitute(
        const EntityId& id,
        const EntityId& videoSourceId,
        const QString& name,
        RoiType type,
        Severity severity,
        const QVector<RoiPoint>& points,
        const QString& ruleJson,
        bool enabled
    );

    const EntityId& id() const;
    const EntityId& videoSourceId() const;

    QString name() const;
    RoiType type() const;
    Severity severity() const;
    QVector<RoiPoint> points() const;
    QString ruleJson() const;

    bool isEnabled() const;

    void setName(const QString& name);
    void setType(RoiType type);
    void setSeverity(Severity severity);
    void setEnabled(bool enabled);
    void setPoints(const QVector<RoiPoint>& points);
    void addPoint(const RoiPoint& point);
    void clearPoints();
    void setRuleJson(const QString& ruleJson);

    bool hasName() const;
    bool hasDrawableShape() const;
    QString displayName() const;

private:
    EntityId id_;
    EntityId videoSourceId_;

    QString name_;
    RoiType type_ = RoiType::Rectangle;
    Severity severity_ = Severity::Medium;
    QVector<RoiPoint> points_;
    QString ruleJson_ = QStringLiteral("{}");

    bool enabled_ = true;
};

} // namespace geovideo::domain