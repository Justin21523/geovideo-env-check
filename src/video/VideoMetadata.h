#pragma once

#include <QString>
#include <QtGlobal>

namespace geovideo::video {

class VideoMetadata final {
public:
    VideoMetadata() = default;

    static VideoMetadata fromValues(int width, int height, double fps, qint64 frameCount);

    int width() const;
    int height() const;
    double fps() const;
    qint64 frameCount() const;
    qint64 durationMs() const;

    bool isValid() const;
    bool hasTimingInfo() const;

    QString resolutionText() const;
    QString durationText() const;

private:
    int width_ = 0;
    int height_ = 0;
    double fps_ = 0.0;
    qint64 frameCount_ = 0;
    qint64 durationMs_ = 0;
};

} // namespace geovideo::video