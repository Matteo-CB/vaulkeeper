#include "ui/viewmodels/application_view_model.hpp"

#include <array>

#include <QFileInfo>
#include <QVariantMap>

#include <spdlog/spdlog.h>

namespace vk::app {

namespace {

struct LocaleOption {
    const char* tag;
    const char* native;
    const char* english;
};

constexpr std::array<LocaleOption, 5> kLocales {{
    { "en", "English",  "English" },
    { "fr", "Francais", "French" },
    { "es", "Espanol",  "Spanish" },
    { "de", "Deutsch",  "German" },
    { "it", "Italiano", "Italian" }
}};

}

ApplicationViewModel::ApplicationViewModel(core::AppConfig config, core::Paths paths, QObject* parent)
    : QObject(parent),
      appConfig(std::move(config)),
      appPaths(std::move(paths)),
      statusText("Ready"),
      page("health") {
    const auto configPath = appPaths.configDir / "vaulkeeper.toml";
    firstRunFlag = !QFileInfo::exists(QString::fromStdString(configPath.generic_string()));

    pool = std::make_shared<core::ThreadPool>(appConfig.scan.workerThreads);
    cache = std::make_shared<core::ScanCache>();
    (void)cache->open(appPaths.cacheDir / "scan.db");

    quarantineStore = std::make_shared<core::Quarantine>();
    (void)quarantineStore->initialize(appPaths.quarantineDir, appConfig.quarantine.retention);

    journalStore = std::make_shared<core::Journal>();
    (void)journalStore->open(appPaths.dataDir / "journal.db");
}

QString ApplicationViewModel::status() const { return statusText; }
QString ApplicationViewModel::currentPage() const { return page; }

void ApplicationViewModel::setCurrentPage(const QString& value) {
    if (page == value) { return; }
    page = value;
    emit currentPageChanged();
}

QString ApplicationViewModel::localeTag() const { return QString::fromStdString(appConfig.ui.locale); }

void ApplicationViewModel::setLocaleTag(const QString& tag) {
    const auto asStd = tag.toStdString();
    if (appConfig.ui.locale == asStd) { return; }
    appConfig.ui.locale = asStd;
    persistConfig();
    emit localeTagChanged();
    emit configChanged();
}

bool ApplicationViewModel::portableMode() const { return false; }

bool ApplicationViewModel::firstRun() const { return firstRunFlag; }

QVariantList ApplicationViewModel::availableLocales() const {
    QVariantList out;
    for (const auto& locale : kLocales) {
        QVariantMap entry;
        entry["tag"] = QString::fromUtf8(locale.tag);
        entry["native"] = QString::fromUtf8(locale.native);
        entry["english"] = QString::fromUtf8(locale.english);
        out.push_back(entry);
    }
    return out;
}

void ApplicationViewModel::notify(const QString& message) {
    statusText = message;
    emit statusChanged();
    spdlog::info("ui status: {}", message.toStdString());
}

void ApplicationViewModel::commitFirstRunComplete() {
    if (!firstRunFlag) { return; }
    firstRunFlag = false;
    persistConfig();
    emit firstRunChanged();
}

void ApplicationViewModel::persistConfig() const {
    const auto configPath = appPaths.configDir / "vaulkeeper.toml";
    (void)appConfig.save(configPath);
}

}
