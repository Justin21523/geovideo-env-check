#include "src/infrastructure/database/DatabaseConnection.h"

#include <sqlite3.h>

#include <QByteArray>
#include <QDebug>

namespace geovideo::infrastructure::database {
namespace {

int readIntegerCallback(void* output, int columnCount, char** values, char** columnNames)
{
    Q_UNUSED(columnNames);

    if (output == nullptr || columnCount <= 0 || values == nullptr || values[0] == nullptr) {
        return 0;
    }

    auto* result = static_cast<int*>(output);
    *result = QString::fromUtf8(values[0]).toInt();

    return 0;
}

} // namespace

DatabaseConnection::~DatabaseConnection()
{
    close();
}

bool DatabaseConnection::open(const QString& databasePath)
{
    close();

    if (databasePath.trimmed().isEmpty()) {
        lastErrorMessage_ = QStringLiteral("Database path is empty.");
        return false;
    }

    databasePath_ = databasePath;

    const QByteArray pathBytes = databasePath_.toUtf8();
    const int result = sqlite3_open_v2(
        pathBytes.constData(),
        &handle_,
        SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_FULLMUTEX,
        nullptr
    );

    if (result != SQLITE_OK) {
        setLastErrorFromSqlite(QStringLiteral("Failed to open SQLite database."));
        close();
        return false;
    }

    // busy timeout 可以降低短時間鎖定造成的失敗機率，桌面工具很常需要這個保險。
    sqlite3_busy_timeout(handle_, 5000);

    // SQLite 預設不一定啟用 foreign key，這裡每次連線都明確打開。
    if (!execute(QStringLiteral("PRAGMA foreign_keys = ON;"))) {
        close();
        return false;
    }

    qInfo().noquote() << "SQLite database opened:" << databasePath_;
    return true;
}

void DatabaseConnection::close()
{
    if (handle_ == nullptr) {
        return;
    }

    sqlite3_close(handle_);
    handle_ = nullptr;

    qInfo() << "SQLite database closed.";
}

bool DatabaseConnection::isOpen() const
{
    return handle_ != nullptr;
}

bool DatabaseConnection::execute(const QString& sql)
{
    if (!isOpen()) {
        lastErrorMessage_ = QStringLiteral("Database is not open.");
        return false;
    }

    char* errorMessage = nullptr;
    const QByteArray sqlBytes = sql.toUtf8();

    const int result = sqlite3_exec(
        handle_,
        sqlBytes.constData(),
        nullptr,
        nullptr,
        &errorMessage
    );

    if (result != SQLITE_OK) {
        if (errorMessage != nullptr) {
            lastErrorMessage_ = QString::fromUtf8(errorMessage);
            sqlite3_free(errorMessage);
        } else {
            setLastErrorFromSqlite(QStringLiteral("SQLite statement failed."));
        }

        return false;
    }

    lastErrorMessage_.clear();
    return true;
}

int DatabaseConnection::userVersion()
{
    if (!isOpen()) {
        lastErrorMessage_ = QStringLiteral("Database is not open.");
        return -1;
    }

    int version = -1;
    char* errorMessage = nullptr;

    const int result = sqlite3_exec(
        handle_,
        "PRAGMA user_version;",
        readIntegerCallback,
        &version,
        &errorMessage
    );

    if (result != SQLITE_OK) {
        if (errorMessage != nullptr) {
            lastErrorMessage_ = QString::fromUtf8(errorMessage);
            sqlite3_free(errorMessage);
        } else {
            setLastErrorFromSqlite(QStringLiteral("Failed to read SQLite user_version."));
        }

        return -1;
    }

    lastErrorMessage_.clear();
    return version;
}

bool DatabaseConnection::setUserVersion(int version)
{
    if (version < 0) {
        lastErrorMessage_ = QStringLiteral("Schema version cannot be negative.");
        return false;
    }

    return execute(QStringLiteral("PRAGMA user_version = %1;").arg(version));
}

QString DatabaseConnection::databasePath() const
{
    return databasePath_;
}

QString DatabaseConnection::lastErrorMessage() const
{
    return lastErrorMessage_;
}

sqlite3* DatabaseConnection::handle() const
{
    return handle_;
}

void DatabaseConnection::setLastErrorFromSqlite(const QString& fallbackMessage)
{
    if (handle_ == nullptr) {
        lastErrorMessage_ = fallbackMessage;
        return;
    }

    const char* errorMessage = sqlite3_errmsg(handle_);
    lastErrorMessage_ =
        errorMessage == nullptr ? fallbackMessage : QString::fromUtf8(errorMessage);
}

} // namespace geovideo::infrastructure::database