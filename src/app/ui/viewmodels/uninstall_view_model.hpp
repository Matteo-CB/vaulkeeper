#pragma once

#include <memory>

#include <QObject>
#include <QVariantList>

namespace vk::app {

class ApplicationViewModel;

class UninstallViewModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QVariantList applications READ applications NOTIFY applicationsChanged)
    Q_PROPERTY(QString sortKey READ sortKey WRITE setSortKey NOTIFY sortChanged)
    Q_PROPERTY(bool sortDescending READ sortDescending WRITE setSortDescending NOTIFY sortChanged)
    Q_PROPERTY(QString totalSize READ totalSize NOTIFY applicationsChanged)

public:
    explicit UninstallViewModel(std::shared_ptr<ApplicationViewModel> app, QObject* parent = nullptr);

    [[nodiscard]] QVariantList applications() const { return visibleApps; }
    [[nodiscard]] QString sortKey() const { return sortKeyValue; }
    [[nodiscard]] bool sortDescending() const { return descendingOrder; }
    [[nodiscard]] QString totalSize() const { return totalSizeText; }

    void setSortKey(const QString& key);
    void setSortDescending(bool value);

public slots:
    void refresh();
    void uninstall(const QString& applicationId, bool silent, bool sweepResidues);

signals:
    void applicationsChanged();
    void sortChanged();

private:
    void resort();

    std::shared_ptr<ApplicationViewModel> application;
    QVariantList allApps;
    QVariantList visibleApps;
    QString sortKeyValue { "size" };
    bool descendingOrder { true };
    QString totalSizeText;
};

}
