#pragma once

#include <QByteArray>

namespace geovideo::app_settings {

inline constexpr int MainWindowLayoutVersion = 1;

bool shouldRestoreMainWindowLayout();
void setShouldRestoreMainWindowLayout(bool enabled);

QByteArray mainWindowGeometry();
QByteArray mainWindowState();

void saveMainWindowGeometry(const QByteArray& geometry);
void saveMainWindowState(const QByteArray& state);

void clearMainWindowLayout();

} // namespace geovideo::app_settings