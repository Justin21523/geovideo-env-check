#pragma once

#include "src/video/VideoMetadata.h"

#include <QString>

#include <optional>

namespace geovideo::video {

class VideoMetadataReader {
public:
    virtual ~VideoMetadataReader() = default;

    virtual std::optional<VideoMetadata> read(const QString& filePath) = 0;
    virtual QString lastErrorMessage() const = 0;
};

} // namespace geovideo::video