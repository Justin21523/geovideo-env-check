#include "src/app/widgets/VideoViewerWidget.h"

#include "src/core/domain/video/VideoSource.h"

#include <QFrame>
#include <QLabel>
#include <QVBoxLayout>

namespace geovideo {

VideoViewerWidget::VideoViewerWidget(QWidget* parent)
    : QWidget(parent)
{
    auto* rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(12, 12, 12, 12);

    auto* viewerFrame = new QFrame(this);
    viewerFrame->setFrameShape(QFrame::StyledPanel);
    viewerFrame->setStyleSheet(QStringLiteral(
        "QFrame {"
        "  background-color: #101820;"
        "  border: 1px solid #2D3A45;"
        "  border-radius: 8px;"
        "}"
        "QLabel {"
        "  color: #F4F7FA;"
        "}"
    ));

    auto* frameLayout = new QVBoxLayout(viewerFrame);
    frameLayout->setContentsMargins(32, 32, 32, 32);
    frameLayout->setSpacing(12);

    titleLabel_ = new QLabel(QStringLiteral("Video Viewer"), viewerFrame);
    titleLabel_->setAlignment(Qt::AlignCenter);
    titleLabel_->setStyleSheet(QStringLiteral("font-size: 24px; font-weight: 700;"));

    hintLabel_ = new QLabel(viewerFrame);
    hintLabel_->setAlignment(Qt::AlignCenter);
    hintLabel_->setWordWrap(true);
    hintLabel_->setStyleSheet(QStringLiteral("font-size: 14px; color: #B9C6D3;"));

    frameLayout->addStretch();
    frameLayout->addWidget(titleLabel_);
    frameLayout->addWidget(hintLabel_);
    frameLayout->addStretch();

    rootLayout->addWidget(viewerFrame);

    clearLoadedFile();
}

void VideoViewerWidget::setLoadedFileName(const QString& fileName)
{
    // 保留簡化版 API，方便未來某些流程只需要顯示檔名。
    titleLabel_->setText(QStringLiteral("Video Source Loaded"));
    hintLabel_->setText(
        QStringLiteral(
            "Loaded source: %1\n\n"
            "Playback, frame stepping, ROI overlay, and OpenCV frame extraction "
            "will be connected in the next phases."
        )
            .arg(fileName)
    );
}

void VideoViewerWidget::setLoadedVideoSource(const domain::VideoSource& videoSource)
{
    titleLabel_->setText(QStringLiteral("Video Source Loaded"));

    // Phase 1.1 開始顯示真正 metadata，讓匯入影片有可見成果。
    hintLabel_->setText(
        QStringLiteral(
            "Name: %1\n"
            "Resolution: %2\n"
            "FPS: %3\n"
            "Duration: %4\n"
            "Frame Count: %5\n\n"
            "Playback, frame stepping, ROI overlay, and OpenCV frame extraction "
            "will be connected in the next phases."
        )
            .arg(videoSource.displayName())
            .arg(videoSource.resolutionText())
            .arg(QString::number(videoSource.fps(), 'f', 2))
            .arg(videoSource.durationText())
            .arg(videoSource.frameCount())
    );
}

void VideoViewerWidget::clearLoadedFile()
{
    titleLabel_->setText(QStringLiteral("Video Viewer"));
    hintLabel_->setText(QStringLiteral(
        "No video source loaded.\n\n"
        "Use File > Import Video to prepare a source for event review."
    ));
}

} // namespace geovideo