#include "ui/viewmodels/uninstall_view_model.hpp"

#include <algorithm>
#include <chrono>

#include <QDateTime>
#include <QVariantMap>

#include "ui/viewmodels/application_view_model.hpp"
#include "uninstall/uninstaller.hpp"
#include "util/bytes.hpp"

namespace vk::app {

namespace {

QString installKindLabel(core::InstallKind kind) {
    switch (kind) {
        case core::InstallKind::Msi: return "MSI";
        case core::InstallKind::Msix: return "MSIX";
        case core::InstallKind::Win32Classic: return "Win32";
        case core::InstallKind::Steam: return "Steam";
        case core::InstallKind::EpicGames: return "Epic";
        case core::InstallKind::Gog: return "GOG";
        case core::InstallKind::XboxGamePass: return "Xbox";
        case core::InstallKind::EaApp: return "EA";
        case core::InstallKind::UbisoftConnect: return "Ubisoft";
        case core::InstallKind::Portable: return "Portable";
        case core::InstallKind::Winget: return "winget";
        case core::InstallKind::Scoop: return "Scoop";
        case core::InstallKind::Chocolatey: return "Chocolatey";
        default: return "Unknown";
    }
}

QString installedAtText(std::chrono::system_clock::time_point t) {
    if (t.time_since_epoch().count() == 0) { return {}; }
    const auto msSinceEpoch = std::chrono::duration_cast<std::chrono::milliseconds>(t.time_since_epoch()).count();
    return QDateTime::fromMSecsSinceEpoch(msSinceEpoch).toString("yyyy-MM-dd");
}

}

UninstallViewModel::UninstallViewModel(std::shared_ptr<ApplicationViewModel> app, QObject* parent)
    : QObject(parent), application(std::move(app)) {}

void UninstallViewModel::refresh() {
    core::Uninstaller uninstaller(application->quarantine(), application->journal());
    auto result = uninstaller.enumerateInstalled();
    if (!result) {
        application->notify(QString::fromStdString(result.error().message));
        return;
    }

    QVariantList list;
    std::uint64_t totalBytes = 0;
    for (const auto& app : *result) {
        QVariantMap row;
        row["id"] = QString::fromStdString(app.id);
        row["name"] = QString::fromStdString(app.displayName);
        row["publisher"] = QString::fromStdString(app.publisher);
        row["version"] = QString::fromStdString(app.version);
        row["sizeBytes"] = static_cast<qlonglong>(app.sizeBytes);
        row["size"] = app.sizeBytes > 0
                          ? QString::fromStdString(core::formatBytes(app.sizeBytes))
                          : QStringLiteral("unknown");
        row["kind"] = installKindLabel(app.installKind);
        row["location"] = QString::fromStdString(app.installLocation.generic_string());
        row["installedAt"] = installedAtText(app.installedAt);
        row["installedAtMs"] = static_cast<qlonglong>(
            std::chrono::duration_cast<std::chrono::milliseconds>(app.installedAt.time_since_epoch()).count());
        list.push_back(row);
        totalBytes += app.sizeBytes;
    }

    allApps = std::move(list);
    totalSizeText = QString::fromStdString(core::formatBytes(totalBytes));
    resort();
}

void UninstallViewModel::setSortKey(const QString& key) {
    if (key == sortKeyValue) { return; }
    sortKeyValue = key;
    emit sortChanged();
    resort();
}

void UninstallViewModel::setSortDescending(bool value) {
    if (value == descendingOrder) { return; }
    descendingOrder = value;
    emit sortChanged();
    resort();
}

void UninstallViewModel::resort() {
    visibleApps = allApps;
    const auto key = sortKeyValue;
    const bool desc = descendingOrder;
    std::sort(visibleApps.begin(), visibleApps.end(), [&](const QVariant& lhs, const QVariant& rhs) {
        const auto a = lhs.toMap();
        const auto b = rhs.toMap();
        if (key == "size") {
            const auto av = a.value("sizeBytes").toLongLong();
            const auto bv = b.value("sizeBytes").toLongLong();
            return desc ? av > bv : av < bv;
        }
        if (key == "installedAt") {
            const auto av = a.value("installedAtMs").toLongLong();
            const auto bv = b.value("installedAtMs").toLongLong();
            return desc ? av > bv : av < bv;
        }
        if (key == "publisher") {
            const auto av = a.value("publisher").toString();
            const auto bv = b.value("publisher").toString();
            const auto cmp = av.localeAwareCompare(bv);
            return desc ? cmp > 0 : cmp < 0;
        }
        if (key == "kind") {
            const auto av = a.value("kind").toString();
            const auto bv = b.value("kind").toString();
            const auto cmp = av.localeAwareCompare(bv);
            return desc ? cmp > 0 : cmp < 0;
        }
        const auto av = a.value("name").toString();
        const auto bv = b.value("name").toString();
        const auto cmp = av.localeAwareCompare(bv);
        return desc ? cmp > 0 : cmp < 0;
    });
    emit applicationsChanged();
}

void UninstallViewModel::uninstall(const QString& applicationId, bool silent, bool sweepResidues) {
    core::Uninstaller uninstaller(application->quarantine(), application->journal());
    core::UninstallPlan plan;
    plan.target.id = applicationId.toStdString();
    plan.runSilent = silent;
    plan.sweepResidues = sweepResidues;
    auto report = uninstaller.execute(plan);
    if (!report) {
        application->notify(QString::fromStdString(report.error().message));
        return;
    }
    application->notify("Uninstall sequence recorded");
}

}
