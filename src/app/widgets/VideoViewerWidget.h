#pragma once

#include <QString>
#include <QWidget>

class QLabel;

namespace geovideo::domain {
class VideoSource;
}

namespace geovideo {

class VideoViewerWidget final : public QWidget {
    Q_OBJECT

public:
    explicit VideoViewerWidget(QWidget* parent = nullptr);

    void setLoadedFileName(const QString& fileName);
    void setLoadedVideoSource(const domain::VideoSource& videoSource);
    void clearLoadedFile();

private:
    QLabel* titleLabel_ = nullptr;
    QLabel* hintLabel_ = nullptr;
};

} // namespace geovideo