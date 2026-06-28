#pragma once

#include <QString>

namespace geovideo::domain {

enum class Severity {
    Low,
    Medium,
    High,
    Critical,
};

QString severityToDisplayText(Severity severity);
Severity severityFromDisplayText(const QString& value, Severity fallback = Severity::Low);

} // namespace geovideo::domain