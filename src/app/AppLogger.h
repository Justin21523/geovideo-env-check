#pragma once

#include <QString>

namespace geovideo::app_logger {

bool initialize();
void shutdown();
QString logFilePath();

} // namespace geovideo::app_logger