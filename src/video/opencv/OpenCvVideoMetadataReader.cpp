#include "src/video/opencv/OpenCvVideoMetadataReader.h"

#include <opencv2/videoio.hpp>

#include <QDebug>
#include <QFile>
#include <QFileInfo>

#include <cmath>

namespace geovideo::video::opencv {
namespace {

int safeIntFromDouble(double value)
{
    // VideoCapture property 以 double 回傳，這裡避免 NaN 或負值進入系統。
    if (!std::isfinite(value) || value <= 0.0) {
        return 0;
    }

    return static_cast<int>(std::llround(value));
}

qint64 safeInt64FromDouble(double value)
{
    if (!std::isfinite(value) || value <= 0.0) {
        return 0;
    }

    return static_cast<qint64>(std::llround(value));
}

} // namespace

std::optional<VideoMetadata> OpenCvVideoMetadataReader::read(const QString& filePath)
{
    lastErrorMessage_.clear();

    const QFileInfo fileInfo(filePath);

    if (!fileInfo.exists() || !fileInfo.isFile()) {
        setFailure(QStringLiteral("Video file does not exist."));
        return std::nullopt;
    }

    // QFile::encodeName 會使用本機檔案系統編碼，對 Linux 本機路徑比較穩。
    const QByteArray encodedPath = QFile::encodeName(fileInfo.absoluteFilePath());

    cv::VideoCapture capture(encodedPath.constData());

    if (!capture.isOpened()) {
        setFailure(QStringLiteral("OpenCV could not open the video file."));
        return std::nullopt;
    }

    const int width = safeIntFromDouble(capture.get(cv::CAP_PROP_FRAME_WIDTH));
    const int height = safeIntFromDouble(capture.get(cv::CAP_PROP_FRAME_HEIGHT));
    const double fps = capture.get(cv::CAP_PROP_FPS);
    const qint64 frameCount = safeInt64FromDouble(capture.get(cv::CAP_PROP_FRAME_COUNT));

    VideoMetadata metadata = VideoMetadata::fromValues(width, height, fps, frameCount);

    if (!metadata.isValid()) {
        setFailure(QStringLiteral("OpenCV could not read valid video metadata."));
        return std::nullopt;
    }

    qInfo().noquote()
        << "Video metadata extracted:"
        << fileInfo.fileName()
        << "| resolution:"
        << metadata.resolutionText()
        << "| fps:"
        << metadata.fps()
        << "| frames:"
        << metadata.frameCount()
        << "| duration:"
        << metadata.durationText();

    return metadata;
}

QString OpenCvVideoMetadataReader::lastErrorMessage() const
{
    return lastErrorMessage_;
}

void OpenCvVideoMetadataReader::setFailure(const QString& message)
{
    lastErrorMessage_ = message;
    qWarning().noquote() << "Video metadata extraction failed:" << message;
}

} // namespace geovideo::video::opencv