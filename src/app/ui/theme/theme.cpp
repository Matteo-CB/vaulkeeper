#include "ui/theme/theme.hpp"

#include <QQmlEngine>

namespace vk::app {

namespace {

Theme*& globalInstance() {
    static Theme* instance = nullptr;
    return instance;
}

}

Theme::Theme(QObject* parent) : QObject(parent), currentMode("dark") {}

QString Theme::mode() const { return currentMode; }

void Theme::setMode(const QString& value) {
    if (currentMode == value) { return; }
    currentMode = value;
    emit modeChanged();
}

QColor Theme::surface() const { return currentMode == "dark" ? QColor { 0x0B, 0x0D, 0x10 } : QColor { 0xFA, 0xFB, 0xFC }; }
QColor Theme::surfaceAlt() const { return currentMode == "dark" ? QColor { 0x15, 0x18, 0x1D } : QColor { 0xF0, 0xF2, 0xF5 }; }
QColor Theme::textPrimary() const { return currentMode == "dark" ? QColor { 0xE6, 0xE8, 0xEC } : QColor { 0x10, 0x12, 0x16 }; }
QColor Theme::textSecondary() const { return currentMode == "dark" ? QColor { 0x9A, 0xA0, 0xAA } : QColor { 0x50, 0x56, 0x60 }; }
QColor Theme::accent() const { return QColor { 0x5B, 0x9E, 0xFF }; }
QColor Theme::danger() const { return QColor { 0xE8, 0x5A, 0x5A }; }
QColor Theme::success() const { return QColor { 0x5A, 0xC8, 0x8E }; }

Theme* Theme::instance() { return globalInstance(); }

void Theme::installDefault(const std::string& mode) {
    auto* theme = new Theme;
    theme->setMode(QString::fromStdString(mode));
    globalInstance() = theme;
    qmlRegisterSingletonInstance<Theme>("Vaulkeeper.Theme", 1, 0, "Theme", theme);
}

}
