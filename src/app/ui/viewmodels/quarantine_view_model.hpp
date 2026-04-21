#pragma once

#include <memory>

#include <QObject>
#include <QVariantList>

namespace vk::app {

class ApplicationViewModel;

class QuarantineViewModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QVariantList items READ items NOTIFY itemsChanged)
    Q_PROPERTY(qint64 storedBytes READ storedBytes NOTIFY itemsChanged)

public:
    explicit QuarantineViewModel(std::shared_ptr<ApplicationViewModel> app, QObject* parent = nullptr);

    [[nodiscard]] QVariantList items() const { return quarantineItems; }
    [[nodiscard]] qint64 storedBytes() const { return bytes; }

public slots:
    void refresh();
    void restore(const QString& itemId);
    void discard(const QString& itemId);
    void purgeExpired();

signals:
    void itemsChanged();

private:
    std::shared_ptr<ApplicationViewModel> application;
    QVariantList quarantineItems;
    qint64 bytes { 0 };
};

}
