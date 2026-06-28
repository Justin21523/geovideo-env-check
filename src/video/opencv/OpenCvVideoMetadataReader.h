#pragma once

#include "src/video/VideoMetadataReader.h"

namespace geovideo::video::opencv {

class OpenCvVideoMetadataReader final : public VideoMetadataReader {
public:
    std::optional<VideoMetadata> read(const QString& filePath) override;
    QString lastErrorMessage() const override;

private:
    void setFailure(const QString& message);

private:
    QString lastErrorMessage_;
};

} // namespace geovideo::video::opencv