#pragma once

#include <QString>

namespace geovideo::domain {

class EntityId final {
public:
    EntityId() = default;
    explicit EntityId(QString value);

    static EntityId create();
    static EntityId fromString(const QString& value);

    bool isValid() const;
    QString toString() const;

    bool operator==(const EntityId& other) const;
    bool operator!=(const EntityId& other) const;

private:
    QString value_;
};

} // namespace geovideo::domain