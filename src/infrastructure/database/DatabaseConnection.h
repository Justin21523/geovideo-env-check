#pragma once

#include <QString>

struct sqlite3;

namespace geovideo::infrastructure::database {

class DatabaseConnection final {
public:
    DatabaseConnection() = default;
    ~DatabaseConnection();

    DatabaseConnection(const DatabaseConnection&) = delete;
    DatabaseConnection& operator=(const DatabaseConnection&) = delete;

    bool open(const QString& databasePath);
    void close();

    bool isOpen() const;
    bool execute(const QString& sql);

    int userVersion();
    bool setUserVersion(int version);

    QString databasePath() const;
    QString lastErrorMessage() const;

    sqlite3* handle() const;

private:
    void setLastErrorFromSqlite(const QString& fallbackMessage);

private:
    sqlite3* handle_ = nullptr;
    QString databasePath_;
    QString lastErrorMessage_;
};

} // namespace geovideo::infrastructure::database