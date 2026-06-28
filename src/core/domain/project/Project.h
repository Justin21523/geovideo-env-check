#pragma once

#include "src/core/domain/common/EntityId.h"

#include <QDateTime>
#include <QString>

namespace geovideo::domain {

class Project final {
public:
    Project() = default;

    static Project create(
        const QString& name,
        const QString& baseDirectory = {},
        const QString& description = {}
    );

    static Project reconstitute(
        const EntityId& id,
        const QString& name,
        const QString& description,
        const QString& baseDirectory,
        const QDateTime& createdAt,
        const QDateTime& updatedAt
    );

    const EntityId& id() const;
    QString name() const;
    QString description() const;
    QString baseDirectory() const;
    QDateTime createdAt() const;
    QDateTime updatedAt() const;

    void setName(const QString& name);
    void setDescription(const QString& description);
    void setBaseDirectory(const QString& baseDirectory);

    bool hasName() const;
    QString displayTitle() const;

private:
    void touch();

private:
    EntityId id_;
    QString name_;
    QString description_;
    QString baseDirectory_;
    QDateTime createdAt_;
    QDateTime updatedAt_;
};

} // namespace geovideo::domain
