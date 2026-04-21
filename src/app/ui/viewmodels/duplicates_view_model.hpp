#pragma once

#include <memory>

#include <QObject>
#include <QString>
#include <QVariantList>

namespace vk::app {

class ApplicationViewModel;

class DuplicatesViewModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QVariantList clusters READ clusters NOTIFY clustersChanged)
    Q_PROPERTY(qint64 recoverableBytes READ recoverableBytes NOTIFY clustersChanged)

public:
    explicit DuplicatesViewModel(std::shared_ptr<ApplicationViewModel> app, QObject* parent = nullptr);

    [[nodiscard]] QVariantList clusters() const { return clusterList; }
    [[nodiscard]] qint64 recoverableBytes() const { return totalRecoverable; }

public slots:
    void run(const QString& root);

signals:
    void clustersChanged();

private:
    std::shared_ptr<ApplicationViewModel> application;
    QVariantList clusterList;
    qint64 totalRecoverable { 0 };
};

}
