#include "src/core/domain/common/Severity.h"

namespace geovideo::domain {

QString severityToDisplayText(Severity severity)
{
    switch (severity) {
    case Severity::Low:
        return QStringLiteral("Low");
    case Severity::Medium:
        return QStringLiteral("Medium");
    case Severity::High:
        return QStringLiteral("High");
    case Severity::Critical:
        return QStringLiteral("Critical");
    }

    return QStringLiteral("Low");
}

Severity severityFromDisplayText(const QString& value, Severity fallback)
{
    const QString normalized = value.trimmed().toLower();

    // 字串轉 enum 會集中在 domain 層，避免 UI、資料庫、搜尋功能各自寫一套轉換。
    if (normalized == QStringLiteral("low")) {
        return Severity::Low;
    }

    if (normalized == QStringLiteral("medium")) {
        return Severity::Medium;
    }

    if (normalized == QStringLiteral("high")) {
        return Severity::High;
    }

    if (normalized == QStringLiteral("critical")) {
        return Severity::Critical;
    }

    return fallback;
}

} // namespace geovideo::domain