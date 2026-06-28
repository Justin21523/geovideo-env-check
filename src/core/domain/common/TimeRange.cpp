#include "src/core/domain/common/TimeRange.h"

#include <QtGlobal>

namespace geovideo::domain {

TimeRange::TimeRange(qint64 startMs, qint64 endMs)
    : startMs_(startMs)
    , endMs_(endMs)
{
}

TimeRange TimeRange::fromStartAndDuration(qint64 startMs, qint64 durationMs)
{
    // duration 小於 0 代表資料來源異常，這裡先保守修正成 0。
    const qint64 safeDurationMs = qMax<qint64>(0, durationMs);
    return TimeRange(startMs, startMs + safeDurationMs);
}

qint64 TimeRange::startMs() const
{
    return startMs_;
}

qint64 TimeRange::endMs() const
{
    return endMs_;
}

qint64 TimeRange::durationMs() const
{
    // 對外永遠回傳非負長度，避免 UI 時間軸畫出反向區間。
    return qMax<qint64>(0, endMs_ - startMs_);
}

bool TimeRange::isValid() const
{
    return startMs_ >= 0 && endMs_ >= startMs_;
}

bool TimeRange::contains(qint64 positionMs) const
{
    return isValid() && positionMs >= startMs_ && positionMs <= endMs_;
}

} // namespace geovideo::domain