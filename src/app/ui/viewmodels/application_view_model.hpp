#pragma once

#include <memory>

#include <QObject>
#include <QString>
#include <QStringList>

#include "config/app_config.hpp"
#include "config/paths.hpp"
#include "journal/journal.hpp"
#include "quarantine/quarantine.hpp"
#include "scan/scan_cache.hpp"
#include "util/thread_pool.hpp"

namespace vk::app {

class ApplicationViewModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString status READ status NOTIFY statusChanged)
    Q_PROPERTY(QString currentPage READ currentPage WRITE setCurrentPage NOTIFY currentPageChanged)
    Q_PROPERTY(QString localeTag READ localeTag WRITE setLocaleTag NOTIFY localeTagChanged)
    Q_PROPERTY(bool portableMode READ portableMode NOTIFY configChanged)
    Q_PROPERTY(bool firstRun READ firstRun NOTIFY firstRunChanged)
    Q_PROPERTY(QVariantList availableLocales READ availableLocales CONSTANT)

public:
    explicit ApplicationViewModel(core::AppConfig config, core::Paths paths, QObject* parent = nullptr);

    [[nodiscard]] QString status() const;
    [[nodiscard]] QString currentPage() const;
    void setCurrentPage(const QString& page);
    [[nodiscard]] QString localeTag() const;
    void setLocaleTag(const QString& tag);
    [[nodiscard]] bool portableMode() const;
    [[nodiscard]] bool firstRun() const;
    [[nodiscard]] QVariantList availableLocales() const;

    [[nodiscard]] std::shared_ptr<core::ThreadPool> threadPool() const { return pool; }
    [[nodiscard]] std::shared_ptr<core::ScanCache> scanCache() const { return cache; }
    [[nodiscard]] std::shared_ptr<core::Quarantine> quarantine() const { return quarantineStore; }
    [[nodiscard]] std::shared_ptr<core::Journal> journal() const { return journalStore; }
    [[nodiscard]] const core::AppConfig& config() const { return appConfig; }
    [[nodiscard]] const core::Paths& paths() const { return appPaths; }

public slots:
    void notify(const QString& message);
    void commitFirstRunComplete();

signals:
    void statusChanged();
    void currentPageChanged();
    void configChanged();
    void localeTagChanged();
    void firstRunChanged();

private:
    core::AppConfig appConfig;
    core::Paths appPaths;
    QString statusText;
    QString page;
    bool firstRunFlag { false };
    std::shared_ptr<core::ThreadPool> pool;
    std::shared_ptr<core::ScanCache> cache;
    std::shared_ptr<core::Quarantine> quarantineStore;
    std::shared_ptr<core::Journal> journalStore;

    void persistConfig() const;
};

}
