#include "src/app/widgets/EventTimelineWidget.h"

#include <QColor>
#include <QFont>
#include <QPaintEvent>
#include <QPainter>
#include <QPen>
#include <QPointF>
#include <QRectF>
#include <QString>
#include <QVector>

namespace geovideo {
namespace {

struct DemoTimelineMarker {
    double positionRatio;
    QString label;
    QColor color;
};

} // namespace

EventTimelineWidget::EventTimelineWidget(QWidget* parent)
    : QWidget(parent)
{
    setMinimumHeight(150);
}

QSize EventTimelineWidget::minimumSizeHint() const
{
    return QSize(900, 150);
}

void EventTimelineWidget::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    // 背景先用深色，讓事件 marker 與影片工具感更明顯。
    painter.fillRect(rect(), QColor(16, 24, 32));

    const QRectF contentArea = rect().adjusted(24, 24, -24, -24);
    const qreal timelineY = contentArea.center().y() + 12.0;
    const qreal startX = contentArea.left() + 80.0;
    const qreal endX = contentArea.right() - 32.0;

    painter.setPen(QPen(QColor(210, 220, 230), 1));
    painter.setFont(QFont(QStringLiteral("Sans Serif"), 11, QFont::Bold));
    painter.drawText(contentArea.left(), contentArea.top(), QStringLiteral("Event Timeline"));

    // 時間軸主線代表目前影片長度，之後會換成真實 duration。
    painter.setPen(QPen(QColor(120, 140, 155), 2));
    painter.drawLine(QPointF(startX, timelineY), QPointF(endX, timelineY));

    painter.setPen(QPen(QColor(170, 185, 195), 1));
    painter.setFont(QFont(QStringLiteral("Sans Serif"), 9));
    painter.drawText(QPointF(startX - 6.0, timelineY + 28.0), QStringLiteral("00:00"));
    painter.drawText(QPointF(endX - 36.0, timelineY + 28.0), QStringLiteral("10:00"));

    const QVector<DemoTimelineMarker> markers = {
        {0.16, QStringLiteral("Motion"), QColor(94, 189, 255)},
        {0.34, QStringLiteral("ROI"), QColor(255, 194, 87)},
        {0.47, QStringLiteral("High"), QColor(255, 107, 107)},
        {0.70, QStringLiteral("Note"), QColor(155, 126, 255)},
        {0.86, QStringLiteral("Clip"), QColor(96, 214, 160)},
    };

    // Demo marker 讓骨架階段就能看出未來事件時間軸的視覺方向。
    for (const DemoTimelineMarker& marker : markers) {
        const qreal x = startX + (endX - startX) * marker.positionRatio;

        painter.setBrush(marker.color);
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(QPointF(x, timelineY), 7.0, 7.0);

        painter.setPen(QPen(QColor(220, 228, 235), 1));
        painter.drawText(QPointF(x - 18.0, timelineY - 16.0), marker.label);
    }

    painter.setPen(QPen(QColor(145, 160, 170), 1));
    painter.drawText(
        QPointF(contentArea.left(), contentArea.bottom() - 4.0),
        QStringLiteral("Placeholder markers only. Real event indexing will be connected later.")
    );
}

} // namespace geovideo
