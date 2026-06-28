#include "src/app/AppTheme.h"

#include <QApplication>
#include <QPalette>
#include <QString>

namespace geovideo::app_theme {
namespace {

QPalette createDarkPalette()
{
    QPalette palette;

    // Palette 負責 Qt 原生元件的基礎色系，stylesheet 則負責細節外觀。
    palette.setColor(QPalette::Window, QColor(24, 32, 42));
    palette.setColor(QPalette::WindowText, QColor(236, 241, 245));
    palette.setColor(QPalette::Base, QColor(17, 24, 32));
    palette.setColor(QPalette::AlternateBase, QColor(28, 38, 49));
    palette.setColor(QPalette::Text, QColor(236, 241, 245));
    palette.setColor(QPalette::Button, QColor(35, 47, 60));
    palette.setColor(QPalette::ButtonText, QColor(236, 241, 245));
    palette.setColor(QPalette::BrightText, QColor(255, 107, 107));
    palette.setColor(QPalette::Highlight, QColor(94, 189, 255));
    palette.setColor(QPalette::HighlightedText, QColor(10, 16, 22));
    palette.setColor(QPalette::ToolTipBase, QColor(35, 47, 60));
    palette.setColor(QPalette::ToolTipText, QColor(236, 241, 245));
    palette.setColor(QPalette::PlaceholderText, QColor(145, 160, 170));

    return palette;
}

QString createStyleSheet()
{
    return QStringLiteral(R"(
QMainWindow {
    background-color: #18202A;
}

QMenuBar {
    background-color: #202A36;
    color: #ECF1F5;
    border-bottom: 1px solid #314150;
}

QMenuBar::item {
    padding: 6px 10px;
    background: transparent;
}

QMenuBar::item:selected {
    background-color: #2B3948;
}

QMenu {
    background-color: #202A36;
    color: #ECF1F5;
    border: 1px solid #314150;
}

QMenu::item {
    padding: 6px 24px 6px 24px;
}

QMenu::item:selected {
    background-color: #355066;
}

QToolBar {
    background-color: #202A36;
    border: 0;
    border-bottom: 1px solid #314150;
    spacing: 6px;
    padding: 6px;
}

QToolButton {
    background-color: #2B3948;
    color: #ECF1F5;
    border: 1px solid #3B4E60;
    border-radius: 5px;
    padding: 6px 10px;
}

QToolButton:hover {
    background-color: #355066;
}

QToolButton:pressed {
    background-color: #5EBDFF;
    color: #0A1016;
}

QDockWidget {
    color: #ECF1F5;
    titlebar-close-icon: none;
    titlebar-normal-icon: none;
}

QDockWidget::title {
    background-color: #202A36;
    padding: 7px;
    border: 1px solid #314150;
}

QStatusBar {
    background-color: #202A36;
    color: #B9C6D3;
    border-top: 1px solid #314150;
}

QLabel {
    color: #ECF1F5;
}

QFrame {
    color: #ECF1F5;
}

QLineEdit,
QTextEdit,
QPlainTextEdit,
QComboBox,
QSpinBox,
QDoubleSpinBox {
    background-color: #111820;
    color: #ECF1F5;
    border: 1px solid #3B4E60;
    border-radius: 5px;
    padding: 5px;
}

QTableView,
QTreeView,
QListView {
    background-color: #111820;
    alternate-background-color: #1C2631;
    color: #ECF1F5;
    gridline-color: #314150;
    border: 1px solid #314150;
}

QHeaderView::section {
    background-color: #202A36;
    color: #ECF1F5;
    border: 1px solid #314150;
    padding: 5px;
}

QScrollBar:vertical,
QScrollBar:horizontal {
    background-color: #18202A;
    border: none;
    margin: 0;
}

QScrollBar::handle:vertical,
QScrollBar::handle:horizontal {
    background-color: #3B4E60;
    border-radius: 4px;
}

QScrollBar::handle:vertical:hover,
QScrollBar::handle:horizontal:hover {
    background-color: #5EBDFF;
}
)");
}

} // namespace

void applyDefaultTheme(QApplication& app)
{
    // Fusion 搭配自訂 palette，讓 Linux 不同桌面環境下的外觀更一致。
    QApplication::setStyle(QStringLiteral("Fusion"));

    app.setPalette(createDarkPalette());
    app.setStyleSheet(createStyleSheet());
}

} // namespace geovideo::app_theme