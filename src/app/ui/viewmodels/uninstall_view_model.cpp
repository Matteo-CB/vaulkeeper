#include "ui/viewmodels/uninstall_view_model.hpp"

#include <QVariantMap>

#include "ui/viewmodels/application_view_model.hpp"
#include "uninstall/uninstaller.hpp"

namespace vk::app {

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
    for (const auto& app : *result) {
        QVariantMap row;
        row["id"] = QString::fromStdString(app.id);
        row["name"] = QString::fromStdString(app.displayName);
        row["publisher"] = QString::fromStdString(app.publisher);
        row["version"] = QString::fromStdString(app.version);
        row["size"] = static_cast<qlonglong>(app.sizeBytes);
        list.push_back(row);
    }
    apps = std::move(list);
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
