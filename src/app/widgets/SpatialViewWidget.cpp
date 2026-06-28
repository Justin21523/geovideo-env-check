#include "src/app/widgets/SpatialViewWidget.h"

#include <QBrush>
#include <QColor>
#include <QFont>
#include <QPaintEvent>
#include <QPainter>
#include <QPen>
#include <QPointF>
#include <QPolygonF>
#include <QRectF>
#include <QString>
#include <QVector>
#include <QtMath>

#include <cmath>

namespace geovideo {

SpatialViewWidget::SpatialViewWidget(QWidget* parent)
    : QOpenGLWidget(parent)
{
    setMinimumSize(360, 260);
}

void SpatialViewWidget::initializeGL()
{
    // 初始化 Qt 包裝的 OpenGL function，後續要畫真正 3D 場域會用到。
    initializeOpenGLFunctions();

    // 設定 OpenGL 背景色，讓 Spatial View 與一般 QWidget 佔位區區隔出來。
    glClearColor(0.055F, 0.075F, 0.095F, 1.0F);
}

void SpatialViewWidget::resizeGL(int width, int height)
{
    // viewport 必須跟著 widget 尺寸更新，避免 OpenGL 畫面比例錯亂。
    glViewport(0, 0, width, height);
}

void SpatialViewWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    const QRectF contentArea = rect().adjusted(20, 48, -20, -20);

    painter.setPen(QPen(QColor(230, 238, 245), 1));
    painter.setFont(QFont(QStringLiteral("Sans Serif"), 12, QFont::Bold));
    painter.drawText(QPointF(20.0, 28.0), QStringLiteral("Spatial View"));

    painter.setPen(QPen(QColor(145, 160, 170), 1));
    painter.setFont(QFont(QStringLiteral("Sans Serif"), 9));
    painter.drawText(
        QPointF(120.0, 28.0),
        QStringLiteral("Camera coverage and event positions preview")
    );

    drawGrid(painter, contentArea);
    drawCamera(painter, QPointF(contentArea.left() + 70.0, contentArea.bottom() - 65.0), -38.0);
    drawEventPoints(painter, contentArea);
}

void SpatialViewWidget::drawGrid(QPainter& painter, const QRectF& area) const
{
    painter.save();

    painter.setPen(QPen(QColor(52, 70, 86), 1));
    painter.setBrush(QBrush(QColor(18, 28, 38)));
    painter.drawRoundedRect(area, 8.0, 8.0);

    // 格線代表未來的場域座標系，先用 2D 方式建立空間感。
    constexpr qreal gridSize = 32.0;

    for (qreal x = area.left(); x <= area.right(); x += gridSize) {
        painter.drawLine(QPointF(x, area.top()), QPointF(x, area.bottom()));
    }

    for (qreal y = area.top(); y <= area.bottom(); y += gridSize) {
        painter.drawLine(QPointF(area.left(), y), QPointF(area.right(), y));
    }

    painter.restore();
}

void SpatialViewWidget::drawCamera(
    QPainter& painter,
    const QPointF& cameraPosition,
    qreal angleDegrees
) const
{
    painter.save();

    const qreal angle = qDegreesToRadians(angleDegrees);
    const qreal leftAngle = angle - qDegreesToRadians(22.0);
    const qreal rightAngle = angle + qDegreesToRadians(22.0);
    constexpr qreal range = 180.0;

    const QPointF leftPoint(
        cameraPosition.x() + std::cos(leftAngle) * range,
        cameraPosition.y() + std::sin(leftAngle) * range
    );
    const QPointF rightPoint(
        cameraPosition.x() + std::cos(rightAngle) * range,
        cameraPosition.y() + std::sin(rightAngle) * range
    );

    QPolygonF frustum;
    frustum << cameraPosition << leftPoint << rightPoint;

    // 視角錐先用半透明多邊形，後續可替換成真正 OpenGL camera frustum。
    painter.setPen(QPen(QColor(94, 189, 255), 2));
    painter.setBrush(QColor(94, 189, 255, 38));
    painter.drawPolygon(frustum);

    painter.setBrush(QColor(94, 189, 255));
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(cameraPosition, 8.0, 8.0);

    painter.setPen(QPen(QColor(220, 235, 245), 1));
    painter.drawText(cameraPosition + QPointF(12.0, -10.0), QStringLiteral("Camera 01"));

    painter.restore();
}

void SpatialViewWidget::drawEventPoints(QPainter& painter, const QRectF& area) const
{
    painter.save();

    const QVector<QPointF> points = {
        QPointF(area.left() + area.width() * 0.42, area.top() + area.height() * 0.42),
        QPointF(area.left() + area.width() * 0.58, area.top() + area.height() * 0.36),
        QPointF(area.left() + area.width() * 0.68, area.top() + area.height() * 0.55),
    };

    // 事件點目前是示意資料，正式版會從 SQLite event table 讀取。
    for (const QPointF& point : points) {
        painter.setBrush(QColor(255, 107, 107, 85));
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(point, 16.0, 16.0);

        painter.setBrush(QColor(255, 107, 107));
        painter.drawEllipse(point, 5.0, 5.0);
    }

    painter.setPen(QPen(QColor(210, 220, 230), 1));
    painter.drawText(
        QPointF(area.left() + 12.0, area.bottom() - 12.0),
        QStringLiteral("Demo event points")
    );

    painter.restore();
}

} // namespace geovideo
