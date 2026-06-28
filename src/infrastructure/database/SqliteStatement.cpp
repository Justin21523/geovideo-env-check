#include "src/infrastructure/database/SqliteStatement.h"

#include "src/infrastructure/database/DatabaseConnection.h"

#include <sqlite3.h>

#include <QByteArray>

namespace geovideo::infrastructure::database {

SqliteStatement::SqliteStatement(DatabaseConnection& connection, const QString& sql)
    : connection_(connection)
    , sql_(sql)
{
    if (!connection_.isOpen()) {
        lastErrorMessage_ = QStringLiteral("Database is not open.");
        return;
    }

    const QByteArray sqlBytes = sql_.toUtf8();
    const int result = sqlite3_prepare_v2(
        connection_.handle(),
        sqlBytes.constData(),
        -1,
        &statement_,
        nullptr
    );

    if (result != SQLITE_OK) {
        setLastErrorFromSqlite(QStringLiteral("Failed to prepare SQLite statement."));
    }
}

SqliteStatement::~SqliteStatement()
{
    if (statement_ != nullptr) {
        sqlite3_finalize(statement_);
        statement_ = nullptr;
    }
}

bool SqliteStatement::isValid() const
{
    return statement_ != nullptr && lastErrorMessage_.isEmpty();
}

bool SqliteStatement::bindText(int index, const QString& value)
{
    if (statement_ == nullptr) {
        lastErrorMessage_ = QStringLiteral("SQLite statement is not prepared.");
        return false;
    }

    const QByteArray valueBytes = value.toUtf8();
    const int result = sqlite3_bind_text(
        statement_,
        index,
        valueBytes.constData(),
        valueBytes.size(),
        SQLITE_TRANSIENT
    );

    return checkBindResult(result, QStringLiteral("bind text"));
}

bool SqliteStatement::bindInt(int index, int value)
{
    if (statement_ == nullptr) {
        lastErrorMessage_ = QStringLiteral("SQLite statement is not prepared.");
        return false;
    }

    return checkBindResult(sqlite3_bind_int(statement_, index, value), QStringLiteral("bind int"));
}

bool SqliteStatement::bindInt64(int index, qint64 value)
{
    if (statement_ == nullptr) {
        lastErrorMessage_ = QStringLiteral("SQLite statement is not prepared.");
        return false;
    }

    return checkBindResult(
        sqlite3_bind_int64(statement_, index, value),
        QStringLiteral("bind int64")
    );
}

bool SqliteStatement::bindDouble(int index, double value)
{
    if (statement_ == nullptr) {
        lastErrorMessage_ = QStringLiteral("SQLite statement is not prepared.");
        return false;
    }

    return checkBindResult(
        sqlite3_bind_double(statement_, index, value),
        QStringLiteral("bind double")
    );
}

bool SqliteStatement::bindNull(int index)
{
    if (statement_ == nullptr) {
        lastErrorMessage_ = QStringLiteral("SQLite statement is not prepared.");
        return false;
    }

    return checkBindResult(sqlite3_bind_null(statement_, index), QStringLiteral("bind null"));
}

bool SqliteStatement::step()
{
    if (statement_ == nullptr) {
        lastErrorMessage_ = QStringLiteral("SQLite statement is not prepared.");
        return false;
    }

    const int result = sqlite3_step(statement_);

    if (result == SQLITE_ROW) {
        return true;
    }

    if (result == SQLITE_DONE) {
        return false;
    }

    setLastErrorFromSqlite(QStringLiteral("SQLite step failed."));
    return false;
}

bool SqliteStatement::execute()
{
    if (statement_ == nullptr) {
        lastErrorMessage_ = QStringLiteral("SQLite statement is not prepared.");
        return false;
    }

    const int result = sqlite3_step(statement_);

    if (result == SQLITE_DONE) {
        lastErrorMessage_.clear();
        return true;
    }

    setLastErrorFromSqlite(QStringLiteral("SQLite execute failed."));
    return false;
}

bool SqliteStatement::hasError() const
{
    return !lastErrorMessage_.isEmpty();
}

QString SqliteStatement::columnText(int index) const
{
    if (statement_ == nullptr || sqlite3_column_type(statement_, index) == SQLITE_NULL) {
        return {};
    }

    const auto* text = sqlite3_column_text(statement_, index);

    if (text == nullptr) {
        return {};
    }

    return QString::fromUtf8(reinterpret_cast<const char*>(text));
}

int SqliteStatement::columnInt(int index) const
{
    if (statement_ == nullptr) {
        return 0;
    }

    return sqlite3_column_int(statement_, index);
}

qint64 SqliteStatement::columnInt64(int index) const
{
    if (statement_ == nullptr) {
        return 0;
    }

    return sqlite3_column_int64(statement_, index);
}

double SqliteStatement::columnDouble(int index) const
{
    if (statement_ == nullptr) {
        return 0.0;
    }

    return sqlite3_column_double(statement_, index);
}

bool SqliteStatement::columnIsNull(int index) const
{
    return statement_ == nullptr || sqlite3_column_type(statement_, index) == SQLITE_NULL;
}

void SqliteStatement::reset()
{
    if (statement_ == nullptr) {
        return;
    }

    sqlite3_reset(statement_);
    sqlite3_clear_bindings(statement_);
    lastErrorMessage_.clear();
}

QString SqliteStatement::lastErrorMessage() const
{
    return lastErrorMessage_;
}

bool SqliteStatement::checkBindResult(int result, const QString& action)
{
    if (result == SQLITE_OK) {
        lastErrorMessage_.clear();
        return true;
    }

    setLastErrorFromSqlite(QStringLiteral("Failed to %1.").arg(action));
    return false;
}

void SqliteStatement::setLastErrorFromSqlite(const QString& fallbackMessage)
{
    const char* errorMessage = sqlite3_errmsg(connection_.handle());
    lastErrorMessage_ =
        errorMessage == nullptr ? fallbackMessage : QString::fromUtf8(errorMessage);
}

} // namespace geovideo::infrastructure::database