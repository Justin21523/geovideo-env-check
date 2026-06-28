#include "src/app/AppLogger.h"

#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QStandardPaths>
#include <QTextStream>

#include <cstdio>
#include <memory>
#include <mutex>

namespace geovideo::app_logger {
namespace {

std::unique_ptr<QFile> logFile;
std::mutex logMutex;
QtMessageHandler previousMessageHandler = nullptr;
QString activeLogFilePath;

QString messageTypeToText(QtMsgType type)
{
    switch (type) {
    case QtDebugMsg:
        return QStringLiteral("DEBUG");
    case QtInfoMsg:
        return QStringLiteral("INFO");
    case QtWarningMsg:
        return QStringLiteral("WARNING");
    case QtCriticalMsg:
        return QStringLiteral("CRITICAL");
    case QtFatalMsg:
        return QStringLiteral("FATAL");
    }

    return QStringLiteral("UNKNOWN");
}

QString createLogDirectory()
{
    // 優先使用系統建議的應用程式資料目錄，避免把 log 寫進專案根目錄。
    QString basePath = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);

    // 某些極簡環境可能拿不到資料目錄，這時退回目前工作目錄。
    if (basePath.isEmpty()) {
        basePath = QDir::currentPath();
    }

    const QString logDirectoryPath = QDir(basePath).filePath(QStringLiteral("logs"));
    QDir().mkpath(logDirectoryPath);

    return logDirectoryPath;
}

QString formatSourceLocation(const QMessageLogContext& context)
{
    // 沒有來源資訊時就不要硬塞空欄位，讓 log 保持乾淨。
    if (context.file == nullptr || context.line <= 0) {
        return {};
    }

    const QFileInfo fileInfo(QString::fromUtf8(context.file));
    const QString functionName =
        context.function == nullptr ? QString() : QString::fromUtf8(context.function);

    return QStringLiteral(" (%1:%2 %3)")
        .arg(fileInfo.fileName())
        .arg(context.line)
        .arg(functionName);
}

void writeToConsole(const QString& line)
{
    // 自訂 handler 會接管 Qt 預設輸出，所以手動補回 console 顯示。
    const QByteArray localLine = line.toLocal8Bit();
    std::fprintf(stderr, "%s\n", localLine.constData());
}

void writeToFile(const QString& line)
{
    std::lock_guard<std::mutex> lock(logMutex);

    if (logFile == nullptr || !logFile->isOpen()) {
        return;
    }

    QTextStream stream(logFile.get());
    stream << line << Qt::endl;
    stream.flush();
}

void messageHandler(QtMsgType type, const QMessageLogContext& context, const QString& message)
{
    const QString timestamp = QDateTime::currentDateTime().toString(Qt::ISODateWithMs);

    const QString line = QStringLiteral("[%1] [%2] %3%4")
                             .arg(timestamp)
                             .arg(messageTypeToText(type))
                             .arg(message)
                             .arg(formatSourceLocation(context));

    writeToConsole(line);
    writeToFile(line);
}

} // namespace

bool initialize()
{
    if (logFile != nullptr && logFile->isOpen()) {
        return true;
    }

    const QString logDirectoryPath = createLogDirectory();
    activeLogFilePath = QDir(logDirectoryPath).filePath(QStringLiteral("geovideo.log"));

    auto file = std::make_unique<QFile>(activeLogFilePath);

    if (!file->open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        activeLogFilePath.clear();
        return false;
    }

    {
        std::lock_guard<std::mutex> lock(logMutex);
        logFile = std::move(file);
    }

    // 安裝 Qt message handler，之後 qInfo / qWarning 都會進入我們的 log 系統。
    previousMessageHandler = qInstallMessageHandler(messageHandler);

    qInfo().noquote() << "Logging initialized:" << activeLogFilePath;
    return true;
}

void shutdown()
{
    qInfo() << "Logging shutdown.";

    // 先還原 Qt 原本的 handler，再釋放檔案資源。
    qInstallMessageHandler(previousMessageHandler);
    previousMessageHandler = nullptr;

    std::lock_guard<std::mutex> lock(logMutex);

    if (logFile != nullptr) {
        logFile->close();
        logFile.reset();
    }

    activeLogFilePath.clear();
}

QString logFilePath()
{
    return activeLogFilePath;
}

} // namespace geovideo::app_logger