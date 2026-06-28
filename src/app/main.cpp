#include "src/app/AppInfo.h"
#include "src/app/AppLogger.h"
#include "src/app/AppTheme.h"
#include "src/app/MainWindow.h"

#include <QApplication>
#include <QDebug>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    // 設定應用程式基本資訊，之後 QSettings、About、桌面環境都會讀到這些資料。
    QApplication::setApplicationName(geovideo::app_info::Name);
    QApplication::setApplicationVersion(geovideo::app_info::Version);
    QApplication::setOrganizationName(geovideo::app_info::Organization);

    // 主題要在主視窗建立前套用，避免部分 widget 先吃到預設樣式。
    geovideo::app_theme::applyDefaultTheme(app);

    const bool loggingReady = geovideo::app_logger::initialize();

    if (!loggingReady) {
        qWarning() << "Logging could not be initialized.";
    }

    qInfo() << "Application started.";

    geovideo::MainWindow window;
    window.show();

    const int exitCode = QApplication::exec();

    qInfo() << "Application finished with exit code:" << exitCode;
    geovideo::app_logger::shutdown();

    return exitCode;
}