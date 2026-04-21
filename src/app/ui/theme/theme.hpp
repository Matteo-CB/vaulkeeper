#pragma once

#include <QColor>
#include <QObject>
#include <QString>

namespace vk::app {

class Theme : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString mode READ mode WRITE setMode NOTIFY modeChanged)
    Q_PROPERTY(QColor surface READ surface NOTIFY modeChanged)
    Q_PROPERTY(QColor surfaceAlt READ surfaceAlt NOTIFY modeChanged)
    Q_PROPERTY(QColor textPrimary READ textPrimary NOTIFY modeChanged)
    Q_PROPERTY(QColor textSecondary READ textSecondary NOTIFY modeChanged)
    Q_PROPERTY(QColor accent READ accent NOTIFY modeChanged)
    Q_PROPERTY(QColor danger READ danger NOTIFY modeChanged)
    Q_PROPERTY(QColor success READ success NOTIFY modeChanged)

public:
    explicit Theme(QObject* parent = nullptr);

    [[nodiscard]] QString mode() const;
    void setMode(const QString& value);

    [[nodiscard]] QColor surface() const;
    [[nodiscard]] QColor surfaceAlt() const;
    [[nodiscard]] QColor textPrimary() const;
    [[nodiscard]] QColor textSecondary() const;
    [[nodiscard]] QColor accent() const;
    [[nodiscard]] QColor danger() const;
    [[nodiscard]] QColor success() const;

    static Theme* instance();
    static void installDefault(const std::string& mode);

signals:
    void modeChanged();

private:
    QString currentMode;
};

}
