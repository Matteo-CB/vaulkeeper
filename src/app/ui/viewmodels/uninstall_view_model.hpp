#pragma once

#include <memory>

#include <QObject>
#include <QVariantList>

namespace vk::app {

class ApplicationViewModel;

class UninstallViewModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QVariantList applications READ applications NOTIFY applicationsChanged)

public:
    explicit UninstallViewModel(std::shared_ptr<ApplicationViewModel> app, QObject* parent = nullptr);

    [[nodiscard]] QVariantList applications() const { return apps; }

public slots:
    void refresh();
    void uninstall(const QString& applicationId, bool silent, bool sweepResidues);

signals:
    void applicationsChanged();

private:
    std::shared_ptr<ApplicationViewModel> application;
    QVariantList apps;
};

}
