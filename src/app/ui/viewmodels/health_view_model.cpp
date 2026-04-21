#include "ui/viewmodels/health_view_model.hpp"

#include <QVariantMap>

#include "fs/volume.hpp"
#include "health/disk_health.hpp"
#include "ui/viewmodels/application_view_model.hpp"
#include "util/bytes.hpp"

namespace vk::app {

HealthViewModel::HealthViewModel(std::shared_ptr<ApplicationViewModel> app, QObject* parent)
    : QObject(parent), application(std::move(app)) {
    refresh();
}

void HealthViewModel::refresh() {
    auto volumes = core::enumerateVolumes();
    volumeList.clear();
    if (volumes) {
        qint64 totalFree = 0;
        qint64 totalCapacity = 0;
        for (const auto& v : *volumes) {
            QVariantMap row;
            row["letter"] = QString::fromStdString(v.driveLetter);
            row["label"] = QString::fromStdString(v.label);
            row["total"] = QString::fromStdString(core::formatBytes(v.totalBytes));
            row["free"] = QString::fromStdString(core::formatBytes(v.freeBytes));
            row["used"] = QString::fromStdString(core::formatBytes(v.usedBytes));
            row["freeRatio"] = v.totalBytes == 0 ? 0.0 : static_cast<double>(v.freeBytes) / static_cast<double>(v.totalBytes);
            volumeList.push_back(row);
            totalFree += static_cast<qint64>(v.freeBytes);
            totalCapacity += static_cast<qint64>(v.totalBytes);
        }
        summaryText = QString::fromStdString(core::formatBytes(totalFree))
                      + " free of "
                      + QString::fromStdString(core::formatBytes(totalCapacity));
    } else {
        summaryText = QString::fromStdString(volumes.error().message);
    }

    auto disks = core::inspectPhysicalDisks();
    diskList.clear();
    if (disks) {
        for (const auto& d : *disks) {
            QVariantMap row;
            row["model"] = QString::fromStdString(d.deviceModel);
            row["capacity"] = QString::fromStdString(core::formatBytes(d.capacityBytes));
            row["hours"] = static_cast<qlonglong>(d.powerOnHours);
            row["wear"] = static_cast<qlonglong>(d.wearLevel);
            row["risk"] = QString::fromStdString(d.riskAssessment);
            diskList.push_back(row);
        }
    }

    emit volumesChanged();
    emit disksChanged();
    emit summaryChanged();
}

}
