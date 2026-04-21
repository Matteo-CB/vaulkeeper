#pragma once

#include <memory>

#include <QObject>
#include <QVariantList>

namespace vk::app {

class ApplicationViewModel;

class HealthViewModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QVariantList volumes READ volumes NOTIFY volumesChanged)
    Q_PROPERTY(QVariantList disks READ disks NOTIFY disksChanged)
    Q_PROPERTY(QString summary READ summary NOTIFY summaryChanged)

public:
    explicit HealthViewModel(std::shared_ptr<ApplicationViewModel> app, QObject* parent = nullptr);

    [[nodiscard]] QVariantList volumes() const { return volumeList; }
    [[nodiscard]] QVariantList disks() const { return diskList; }
    [[nodiscard]] QString summary() const { return summaryText; }

public slots:
    void refresh();

signals:
    void volumesChanged();
    void disksChanged();
    void summaryChanged();

private:
    std::shared_ptr<ApplicationViewModel> application;
    QVariantList volumeList;
    QVariantList diskList;
    QString summaryText;
};

}
