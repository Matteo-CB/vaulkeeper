#include "ui/viewmodels/application_view_model.hpp"

#include <array>

#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>
#include <QTextStream>
#include <QUrl>
#include <QVariantMap>

#include <spdlog/spdlog.h>

#include "fs/volume.hpp"
#include "health/disk_health.hpp"
#include "util/bytes.hpp"

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

QString ApplicationViewModel::suggestedHomeFolder() const {
    return QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
}

QString ApplicationViewModel::defaultDuplicatesRoot() const {
    return QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
}

namespace {

QJsonObject auditJsonObject() {
    QJsonObject report;
    report["schema"] = "vaulkeeper.audit.v1";
    report["generatedAt"] = QDateTime::currentDateTimeUtc().toString(Qt::ISODate);

    QJsonArray volumesArray;
    auto volumes = core::enumerateVolumes();
    if (volumes) {
        for (const auto& v : *volumes) {
            QJsonObject row;
            row["letter"] = QString::fromStdString(v.driveLetter);
            row["label"] = QString::fromStdString(v.label);
            row["totalBytes"] = static_cast<qint64>(v.totalBytes);
            row["freeBytes"] = static_cast<qint64>(v.freeBytes);
            row["usedBytes"] = static_cast<qint64>(v.usedBytes);
            row["total"] = QString::fromStdString(core::formatBytes(v.totalBytes));
            row["free"] = QString::fromStdString(core::formatBytes(v.freeBytes));
            row["used"] = QString::fromStdString(core::formatBytes(v.usedBytes));
            volumesArray.push_back(row);
        }
    }
    report["volumes"] = volumesArray;

    QJsonArray disksArray;
    auto disks = core::inspectPhysicalDisks();
    if (disks) {
        for (const auto& d : *disks) {
            QJsonObject row;
            row["model"] = QString::fromStdString(d.deviceModel);
            row["capacityBytes"] = static_cast<qint64>(d.capacityBytes);
            row["capacity"] = QString::fromStdString(core::formatBytes(d.capacityBytes));
            row["powerOnHours"] = static_cast<qint64>(d.powerOnHours);
            row["wearLevel"] = static_cast<qint64>(d.wearLevel);
            row["risk"] = QString::fromStdString(d.riskAssessment);
            disksArray.push_back(row);
        }
    }
    report["physicalDisks"] = disksArray;

    return report;
}

QString auditAsText(const QJsonObject& report) {
    QString out;
    QTextStream ss(&out);
    ss << "Vaulkeeper audit report\n";
    ss << "Generated: " << report.value("generatedAt").toString() << "\n\n";

    ss << "Volumes\n";
    const auto vols = report.value("volumes").toArray();
    for (const auto& v : vols) {
        const auto o = v.toObject();
        ss << "  " << o.value("letter").toString()
           << "  " << o.value("label").toString()
           << "  free " << o.value("free").toString()
           << " of " << o.value("total").toString() << "\n";
    }
    ss << "\nPhysical disks\n";
    const auto disks = report.value("physicalDisks").toArray();
    if (disks.isEmpty()) {
        ss << "  (no SMART data available)\n";
    }
    for (const auto& d : disks) {
        const auto o = d.toObject();
        ss << "  " << o.value("model").toString()
           << "  " << o.value("capacity").toString()
           << "  wear " << o.value("wearLevel").toInt() << "%"
           << "  hours " << o.value("powerOnHours").toInt()
           << "  risk " << o.value("risk").toString() << "\n";
    }
    return out;
}

QString auditAsHtml(const QJsonObject& report) {
    QString out;
    QTextStream ss(&out);
    ss << "<!doctype html><html><head><meta charset=\"utf-8\">";
    ss << "<title>Vaulkeeper audit</title>";
    ss << "<style>body{font-family:Inter,Arial,sans-serif;background:#0b0d10;color:#e6e6e6;margin:32px}"
          "h1{font-weight:600;margin:0 0 8px 0}"
          "h2{font-weight:600;margin-top:28px;border-bottom:1px solid #2a2f36;padding-bottom:6px}"
          "table{border-collapse:collapse;width:100%;margin-top:8px}"
          "th,td{text-align:left;padding:8px 12px;border-bottom:1px solid #1e2328}"
          "th{color:#9aa0aa;font-weight:500;font-size:12px;text-transform:uppercase;letter-spacing:.04em}"
          ".muted{color:#9aa0aa}"
          "</style></head><body>";
    ss << "<h1>Vaulkeeper audit</h1>";
    ss << "<div class=\"muted\">Generated " << report.value("generatedAt").toString() << "</div>";

    ss << "<h2>Volumes</h2><table><tr><th>Drive</th><th>Label</th><th>Free</th><th>Total</th></tr>";
    const auto vols = report.value("volumes").toArray();
    for (const auto& v : vols) {
        const auto o = v.toObject();
        ss << "<tr><td>" << o.value("letter").toString().toHtmlEscaped() << "</td>"
           << "<td>" << o.value("label").toString().toHtmlEscaped() << "</td>"
           << "<td>" << o.value("free").toString().toHtmlEscaped() << "</td>"
           << "<td>" << o.value("total").toString().toHtmlEscaped() << "</td></tr>";
    }
    ss << "</table>";

    ss << "<h2>Physical disks</h2>";
    const auto disks = report.value("physicalDisks").toArray();
    if (disks.isEmpty()) {
        ss << "<div class=\"muted\">No SMART data available.</div>";
    } else {
        ss << "<table><tr><th>Model</th><th>Capacity</th><th>Wear</th><th>Hours</th><th>Risk</th></tr>";
        for (const auto& d : disks) {
            const auto o = d.toObject();
            ss << "<tr><td>" << o.value("model").toString().toHtmlEscaped() << "</td>"
               << "<td>" << o.value("capacity").toString().toHtmlEscaped() << "</td>"
               << "<td>" << o.value("wearLevel").toInt() << "%</td>"
               << "<td>" << o.value("powerOnHours").toInt() << "</td>"
               << "<td>" << o.value("risk").toString().toHtmlEscaped() << "</td></tr>";
        }
        ss << "</table>";
    }
    ss << "</body></html>";
    return out;
}

QString normalizeLocalPath(const QString& raw) {
    QString path = raw;
    if (path.startsWith("file:///")) {
        path = QUrl(path).toLocalFile();
    } else if (path.startsWith("file://")) {
        path = QUrl(path).toLocalFile();
    }
    return path;
}

}

QString ApplicationViewModel::buildAuditReport(const QString& format) const {
    const auto obj = auditJsonObject();
    const auto f = format.toLower();
    if (f == "html") { return auditAsHtml(obj); }
    if (f == "text" || f == "txt") { return auditAsText(obj); }
    return QString::fromUtf8(QJsonDocument(obj).toJson(QJsonDocument::Indented));
}

bool ApplicationViewModel::promptAndExportAudit(const QString& format) {
    const auto f = format.toLower();
    QString filter = "JSON (*.json)";
    QString suffix = "json";
    if (f == "html") { filter = "HTML (*.html)"; suffix = "html"; }
    else if (f == "text" || f == "txt") { filter = "Text (*.txt)"; suffix = "txt"; }

    const auto initial = QDir(suggestedHomeFolder()).filePath("vaulkeeper-audit." + suffix);
    const auto picked = QFileDialog::getSaveFileName(nullptr, tr("Export audit"), initial, filter);
    if (picked.isEmpty()) {
        return false;
    }
    const bool ok = exportAuditReport(picked, format);
    const_cast<ApplicationViewModel*>(this)->notify(
        ok ? tr("Audit written to %1").arg(picked) : tr("Audit export failed"));
    return ok;
}

bool ApplicationViewModel::exportAuditReport(const QString& path, const QString& format) const {
    const auto localPath = normalizeLocalPath(path);
    QFile file(localPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
        spdlog::error("audit export failed to open {}", localPath.toStdString());
        return false;
    }
    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    out << buildAuditReport(format);
    file.close();
    spdlog::info("audit report written to {}", localPath.toStdString());
    return true;
}

}
