#include "src/video/VideoMetadata.h"

#include <QtGlobal>

#include <cmath>

namespace geovideo::video {
namespace {

double sanitizeFps(double fps)
{
    // OpenCV 讀不到 FPS 時可能回傳 0、NaN 或異常值，進 domain 前先保守修正。
    if (!std::isfinite(fps) || fps < 0.0) {
        return 0.0;
    }

    return fps;
}

qint64 calculateDurationMs(qint64 frameCount, double fps)
{
    // duration 由 frame count 和 FPS 推估，無法推估時就回傳 0。
    if (frameCount <= 0 || fps <= 0.0) {
        return 0;
    }

    return static_cast<qint64>(std::llround((static_cast<double>(frameCount) / fps) * 1000.0));
}

} // namespace

VideoMetadata VideoMetadata::fromValues(int width, int height, double fps, qint64 frameCount)
{
    VideoMetadata metadata;

    // Metadata 是跨 video module 與 application service 的資料物件，這裡集中做合理化。
    metadata.width_ = qMax(0, width);
    metadata.height_ = qMax(0, height);
    metadata.fps_ = sanitizeFps(fps);
    metadata.frameCount_ = qMax<qint64>(0, frameCount);
    metadata.durationMs_ = calculateDurationMs(metadata.frameCount_, metadata.fps_);

    return metadata;
}

int VideoMetadata::width() const
{
    return width_;
}

int VideoMetadata::height() const
{
    return height_;
}

double VideoMetadata::fps() const
{
    return fps_;
}

qint64 VideoMetadata::frameCount() const
{
    return frameCount_;
}

qint64 VideoMetadata::durationMs() const
{
    return durationMs_;
}

bool VideoMetadata::isValid() const
{
    // 有解析度就代表至少成功讀到影片基本資訊，FPS 缺失不直接判定失敗。
    return width_ > 0 && height_ > 0;
}

bool VideoMetadata::hasTimingInfo() const
{
    return fps_ > 0.0 && frameCount_ > 0 && durationMs_ > 0;
}

QString VideoMetadata::resolutionText() const
{
    if (width_ <= 0 || height_ <= 0) {
        return QStringLiteral("Unknown Resolution");
    }

    return QStringLiteral("%1 x %2").arg(width_).arg(height_);
}

QString VideoMetadata::durationText() const
{
    const qint64 totalSeconds = durationMs_ / 1000;
    const qint64 hours = totalSeconds / 3600;
    const qint64 minutes = (totalSeconds % 3600) / 60;
    const qint64 seconds = totalSeconds % 60;

    return QStringLiteral("%1:%2:%3")
        .arg(hours, 2, 10, QLatin1Char('0'))
        .arg(minutes, 2, 10, QLatin1Char('0'))
        .arg(seconds, 2, 10, QLatin1Char('0'));
}

} // namespace geovideo::video