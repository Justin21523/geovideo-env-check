#pragma once

#include <QSize>
#include <QWidget>

class QPaintEvent;

namespace geovideo {

class EventTimelineWidget final : public QWidget {
    Q_OBJECT

public:
    explicit EventTimelineWidget(QWidget* parent = nullptr);

    QSize minimumSizeHint() const override;

protected:
    void paintEvent(QPaintEvent* event) override;
};

} // namespace geovideo
