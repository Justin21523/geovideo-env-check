#include "src/app/AppSettings.h"

#include <QSettings>

namespace geovideo::app_settings {
namespace {

QString restoreLayoutKey()
{
    return QStringLiteral("mainWindow/restoreLayout");
}

QString geometryKey()
{
    return QStringLiteral("mainWindow/geometry");
}

QString stateKey()
{
    return QStringLiteral("mainWindow/state");
}

} // namespace

bool shouldRestoreMainWindowLayout()
{
    QSettings settings;

    // 預設啟用 layout restore，讓桌面工具符合一般使用者預期。
    return settings.value(restoreLayoutKey(), true).toBool();
}

void setShouldRestoreMainWindowLayout(bool enabled)
{
    QSettings settings;
    settings.setValue(restoreLayoutKey(), enabled);
    settings.sync();
}

QByteArray mainWindowGeometry()
{
    QSettings settings;
    return settings.value(geometryKey()).toByteArray();
}

QByteArray mainWindowState()
{
    QSettings settings;
    return settings.value(stateKey()).toByteArray();
}

void saveMainWindowGeometry(const QByteArray& geometry)
{
    QSettings settings;
    settings.setValue(geometryKey(), geometry);
    settings.sync();
}

void saveMainWindowState(const QByteArray& state)
{
    QSettings settings;
    settings.setValue(stateKey(), state);
    settings.sync();
}

void clearMainWindowLayout()
{
    QSettings settings;

    // 只清除主視窗 layout，不影響未來其他設定，例如 theme 或最近開啟專案。
    settings.remove(geometryKey());
    settings.remove(stateKey());
    settings.sync();
}

} // namespace geovideo::app_settings