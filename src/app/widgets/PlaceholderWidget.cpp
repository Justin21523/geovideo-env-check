#include "src/app/widgets/PlaceholderWidget.h"

#include <QFrame>
#include <QLabel>
#include <QVBoxLayout>

namespace geovideo {

PlaceholderWidget::PlaceholderWidget(
    const QString& title,
    const QString& description,
    const QStringList& bulletItems,
    QWidget* parent
)
    : QWidget(parent)
{
    auto* rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(16, 16, 16, 16);
    rootLayout->setSpacing(12);

    // 用同一個 Placeholder 元件建立所有未完成面板，避免早期 UI 程式碼重複。
    rootLayout->addWidget(createTitleLabel(title));
    rootLayout->addWidget(createDescriptionLabel(description));
    rootLayout->addWidget(createBulletList(bulletItems));
    rootLayout->addStretch();
}

QLabel* PlaceholderWidget::createTitleLabel(const QString& title)
{
    auto* label = new QLabel(title, this);
    label->setStyleSheet(QStringLiteral("font-size: 16px; font-weight: 700;"));
    return label;
}

QLabel* PlaceholderWidget::createDescriptionLabel(const QString& description)
{
    auto* label = new QLabel(description, this);
    label->setWordWrap(true);
    label->setStyleSheet(QStringLiteral("color: #555555;"));
    return label;
}

QWidget* PlaceholderWidget::createBulletList(const QStringList& bulletItems)
{
    auto* frame = new QFrame(this);
    frame->setFrameShape(QFrame::StyledPanel);

    auto* layout = new QVBoxLayout(frame);
    layout->setContentsMargins(12, 12, 12, 12);
    layout->setSpacing(6);

    // 每個項目先用文字呈現，後續可以替換成真正的 model/view 清單。
    for (const QString& item : bulletItems) {
        auto* label = new QLabel(QStringLiteral("• %1").arg(item), frame);
        label->setWordWrap(true);
        layout->addWidget(label);
    }

    return frame;
}

} // namespace geovideo