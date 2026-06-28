#pragma once

#include <QtGlobal>

namespace geovideo::domain {

class TimeRange final {
public:
    TimeRange() = default;
    TimeRange(qint64 startMs, qint64 endMs);

    static TimeRange fromStartAndDuration(qint64 startMs, qint64 durationMs);

    qint64 startMs() const;
    qint64 endMs() const;
    qint64 durationMs() const;

    bool isValid() const;
    bool contains(qint64 positionMs) const;

private:
    qint64 startMs_ = 0;
    qint64 endMs_ = 0;
};

} // namespace geovideo::domain