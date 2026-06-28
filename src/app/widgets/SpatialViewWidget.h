#pragma once

#include <QOpenGLFunctions>
#include <QOpenGLWidget>
#include <QtGlobal>

class QPainter;
class QPointF;
class QRectF;

namespace geovideo {

class SpatialViewWidget final : public QOpenGLWidget, protected QOpenGLFunctions {
    Q_OBJECT

public:
    explicit SpatialViewWidget(QWidget* parent = nullptr);

protected:
    void initializeGL() override;
    void resizeGL(int width, int height) override;
    void paintGL() override;

private:
    void drawGrid(QPainter& painter, const QRectF& area) const;
    void drawCamera(QPainter& painter, const QPointF& cameraPosition, qreal angleDegrees) const;
    void drawEventPoints(QPainter& painter, const QRectF& area) const;
};

} // namespace geovideo
