#include "ui/viewmodels/duplicates_view_model.hpp"

#include <QMetaObject>
#include <QVariantMap>
#include <QtConcurrent/QtConcurrent>

#include "duplicates/duplicate_finder.hpp"
#include "ui/viewmodels/application_view_model.hpp"

namespace vk::app {

DuplicatesViewModel::DuplicatesViewModel(std::shared_ptr<ApplicationViewModel> app, QObject* parent)
    : QObject(parent), application(std::move(app)) {}

void DuplicatesViewModel::run(const QString& root) {
    QtConcurrent::run([this, root] {
        std::vector<std::filesystem::path> candidates;
        std::error_code ec;
        for (auto it = std::filesystem::recursive_directory_iterator(root.toStdString(), ec);
             it != std::filesystem::recursive_directory_iterator(); ++it) {
            if (ec) { ec.clear(); continue; }
            if (it->is_regular_file(ec)) { candidates.push_back(it->path()); }
        }

        core::DuplicateOptions options;
        auto result = core::findDuplicates(candidates, options);
        if (!result) {
            QMetaObject::invokeMethod(this, [this, msg = result.error().message] {
                application->notify(QString::fromStdString(msg));
            }, Qt::QueuedConnection);
            return;
        }

        QVariantList list;
        qint64 total = 0;
        for (const auto& cluster : *result) {
            QVariantMap entry;
            entry["size"] = static_cast<qlonglong>(cluster.fileSize);
            entry["digest"] = QString::fromStdString(cluster.digest.toHex());
            entry["recoverable"] = static_cast<qlonglong>(cluster.recoverableBytes());
            QStringList paths;
            for (const auto& f : cluster.files) {
                paths.push_back(QString::fromStdString(f.path.generic_string()));
            }
            entry["paths"] = paths;
            total += static_cast<qint64>(cluster.recoverableBytes());
            list.push_back(entry);
        }

        QMetaObject::invokeMethod(this, [this, list = std::move(list), total] {
            clusterList = std::move(list);
            totalRecoverable = total;
            emit clustersChanged();
            application->notify("Duplicates scan finished");
        }, Qt::QueuedConnection);
    });
}

}
