#include "config/app_config.hpp"

#include <fstream>
#include <sstream>

#include <toml++/toml.h>

namespace vk::core {

Result<AppConfig> AppConfig::load(const std::filesystem::path& path) {
    if (!std::filesystem::exists(path)) {
        return defaults();
    }
    try {
        auto table = toml::parse_file(path.string());
        AppConfig cfg = defaults();

        if (auto quarantine = table["quarantine"].as_table()) {
            if (auto v = (*quarantine)["retention_hours"].value<int64_t>()) {
                cfg.quarantine.retention = std::chrono::hours { *v };
            }
            if (auto v = (*quarantine)["max_total_bytes"].value<int64_t>()) {
                cfg.quarantine.maxTotalBytes = static_cast<std::uint64_t>(*v);
            }
            if (auto v = (*quarantine)["compress_stored_items"].value<bool>()) {
                cfg.quarantine.compressStoredItems = *v;
            }
        }

        if (auto scan = table["scan"].as_table()) {
            if (auto v = (*scan)["use_mft_fast_path"].value<bool>()) cfg.scan.useMftFastPath = *v;
            if (auto v = (*scan)["hash_large_files"].value<bool>()) cfg.scan.hashLargeFiles = *v;
            if (auto v = (*scan)["large_file_threshold_bytes"].value<int64_t>()) {
                cfg.scan.largeFileThresholdBytes = static_cast<std::uint64_t>(*v);
            }
            if (auto v = (*scan)["worker_threads"].value<int64_t>()) {
                cfg.scan.workerThreads = static_cast<std::size_t>(*v);
            }
            if (auto v = (*scan)["persist_cache"].value<bool>()) cfg.scan.persistCache = *v;
        }

        if (auto ui = table["ui"].as_table()) {
            if (auto v = (*ui)["theme"].value<std::string>()) cfg.ui.theme = *v;
            if (auto v = (*ui)["density"].value<std::string>()) cfg.ui.density = *v;
            if (auto v = (*ui)["locale"].value<std::string>()) cfg.ui.locale = *v;
            if (auto v = (*ui)["reduced_motion"].value<bool>()) cfg.ui.reducedMotion = *v;
        }

        if (auto tele = table["telemetry"].as_table()) {
            if (auto v = (*tele)["enabled"].value<bool>()) cfg.telemetry.enabled = *v;
            if (auto v = (*tele)["endpoint"].value<std::string>()) cfg.telemetry.endpoint = *v;
        }

        return cfg;
    } catch (const toml::parse_error& e) {
        return fail(ErrorCode::CorruptedData, e.what());
    }
}

Result<void> AppConfig::save(const std::filesystem::path& path) const {
    toml::table root;

    toml::table quar;
    quar.insert("retention_hours", static_cast<int64_t>(quarantine.retention.count()));
    quar.insert("max_total_bytes", static_cast<int64_t>(quarantine.maxTotalBytes));
    quar.insert("compress_stored_items", quarantine.compressStoredItems);
    root.insert("quarantine", std::move(quar));

    toml::table sc;
    sc.insert("use_mft_fast_path", scan.useMftFastPath);
    sc.insert("hash_large_files", scan.hashLargeFiles);
    sc.insert("large_file_threshold_bytes", static_cast<int64_t>(scan.largeFileThresholdBytes));
    sc.insert("worker_threads", static_cast<int64_t>(scan.workerThreads));
    sc.insert("persist_cache", scan.persistCache);
    root.insert("scan", std::move(sc));

    toml::table u;
    u.insert("theme", ui.theme);
    u.insert("density", ui.density);
    u.insert("locale", ui.locale);
    u.insert("reduced_motion", ui.reducedMotion);
    root.insert("ui", std::move(u));

    toml::table t;
    t.insert("enabled", telemetry.enabled);
    t.insert("endpoint", telemetry.endpoint);
    root.insert("telemetry", std::move(t));

    std::error_code ec;
    std::filesystem::create_directories(path.parent_path(), ec);

    std::ofstream os(path);
    if (!os) {
        return fail(ErrorCode::IoError, "failed to open config file for writing");
    }
    os << root;
    return ok();
}

AppConfig AppConfig::defaults() {
    return AppConfig {};
}

}
