#include "ui/viewmodels/quarantine_view_model.hpp"

#include <QVariantMap>

#include "ui/viewmodels/application_view_model.hpp"
#include "util/time.hpp"

namespace vk::app {

QuarantineViewModel::QuarantineViewModel(std::shared_ptr<ApplicationViewModel> app, QObject* parent)
    : QObject(parent), application(std::move(app)) {
    refresh();
}

void QuarantineViewModel::refresh() {
    auto quarantine = application->quarantine();
    if (!quarantine) { return; }
    auto listResult = quarantine->list();
    if (!listResult) { return; }
    QVariantList list;
    qint64 total = 0;
    for (const auto& item : *listResult) {
        QVariantMap row;
        row["id"] = QString::fromStdString(item.id);
        row["original"] = QString::fromStdString(item.originalPath.generic_string());
        row["reason"] = QString::fromStdString(item.reason);
        row["capturedAt"] = QString::fromStdString(core::formatIso8601(item.capturedAt));
        row["expiresAt"] = QString::fromStdString(core::formatIso8601(item.expiresAt));
        row["bytes"] = static_cast<qlonglong>(item.storedSizeBytes);
        total += static_cast<qint64>(item.storedSizeBytes);
        list.push_back(row);
    }
    quarantineItems = std::move(list);
    bytes = total;
    emit itemsChanged();
}

void QuarantineViewModel::restore(const QString& itemId) {
    auto quarantine = application->quarantine();
    if (!quarantine) { return; }
    auto result = quarantine->restore(itemId.toStdString());
    if (!result) {
        application->notify(QString::fromStdString(result.error().message));
        return;
    }
    refresh();
}

void QuarantineViewModel::discard(const QString& itemId) {
    auto quarantine = application->quarantine();
    if (!quarantine) { return; }
    auto result = quarantine->discard(itemId.toStdString());
    if (!result) {
        application->notify(QString::fromStdString(result.error().message));
        return;
    }
    refresh();
}

void QuarantineViewModel::purgeExpired() {
    auto quarantine = application->quarantine();
    if (!quarantine) { return; }
    (void)quarantine->purgeExpired();
    refresh();
}

}
