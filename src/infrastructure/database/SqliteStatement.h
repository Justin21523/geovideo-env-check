#pragma once

#include <QString>
#include <QtGlobal>

struct sqlite3_stmt;

namespace geovideo::infrastructure::database {

class DatabaseConnection;

class SqliteStatement final {
public:
    SqliteStatement(DatabaseConnection& connection, const QString& sql);
    ~SqliteStatement();

    SqliteStatement(const SqliteStatement&) = delete;
    SqliteStatement& operator=(const SqliteStatement&) = delete;

    bool isValid() const;

    bool bindText(int index, const QString& value);
    bool bindInt(int index, int value);
    bool bindInt64(int index, qint64 value);
    bool bindDouble(int index, double value);
    bool bindNull(int index);

    bool step();
    bool execute();
    bool hasError() const;

    QString columnText(int index) const;
    int columnInt(int index) const;
    qint64 columnInt64(int index) const;
    double columnDouble(int index) const;
    bool columnIsNull(int index) const;

    void reset();

    QString lastErrorMessage() const;

private:
    bool checkBindResult(int result, const QString& action);
    void setLastErrorFromSqlite(const QString& fallbackMessage);

private:
    DatabaseConnection& connection_;
    sqlite3_stmt* statement_ = nullptr;
    QString sql_;
    QString lastErrorMessage_;
};

} // namespace geovideo::infrastructure::database