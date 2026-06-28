#pragma once

#include <QString>
#include <QStringList>
#include <QWidget>

class QLabel;

namespace geovideo {

class PlaceholderWidget final : public QWidget {
    Q_OBJECT

public:
    explicit PlaceholderWidget(
        const QString& title,
        const QString& description,
        const QStringList& bulletItems,
        QWidget* parent = nullptr
    );

private:
    QLabel* createTitleLabel(const QString& title);
    QLabel* createDescriptionLabel(const QString& description);
    QWidget* createBulletList(const QStringList& bulletItems);
};

} // namespace geovideo