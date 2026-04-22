#include "ui/viewmodels/scan_view_model.hpp"

#include <QMetaObject>
#include <QVariantMap>
#include <QtConcurrent/QtConcurrent>

#include "fs/volume.hpp"
#include "ui/viewmodels/application_view_model.hpp"
#include "util/bytes.hpp"

namespace vk::app {

ScanViewModel::ScanViewModel(std::shared_ptr<ApplicationViewModel> app, QObject* parent)
    : QObject(parent), application(std::move(app)) {
    engine = std::make_shared<core::ScanEngine>(application->threadPool(), application->scanCache());
    engine->setProgressHandler([this](const core::ScanProgress& progress) {
        QMetaObject::invokeMethod(this, [this, progress] {
            currentlyScanning = QString::fromStdString(progress.currentPath);
            files = static_cast<qint64>(progress.filesVisited);
            bytes = static_cast<qint64>(progress.bytesIndexed);
            ratio = progress.ratio;
            emit progressChanged();
        }, Qt::QueuedConnection);
    });
}

void ScanViewModel::scanAll() {
    auto volumesResult = core::enumerateVolumes();
    if (!volumesResult) {
        application->notify(QString::fromStdString(volumesResult.error().message));
        return;
    }

    QStringList roots;
    for (const auto& v : *volumesResult) {
        roots.push_back(QString::fromStdString(v.mountPoint.generic_string()));
    }
    scanRoots(roots);
}

void ScanViewModel::scanRoots(const QStringList& roots) {
    if (busy) { return; }
    busy = true;
    emit runningChanged();

    core::ScanOptions options;
    for (const auto& r : roots) {
        options.roots.push_back(std::filesystem::path(r.toStdString()));
    }
    options.useMftFastPath = application->config().scan.useMftFastPath;
    options.workerThreads = application->config().scan.workerThreads;

    QtConcurrent::run([this, options] {
        auto result = engine->run(options);
        QMetaObject::invokeMethod(this, [this, result = std::move(result)] {
            busy = false;
            emit runningChanged();
            if (!result) {
                application->notify(QString::fromStdString(result.error().message));
                return;
            }
            QVariantList summaries;
            std::uint64_t totalScanned = 0;
            for (const auto& v : result->volumes) {
                totalScanned += v.scannedBytes;
            }
            for (const auto& v : result->volumes) {
                QVariantMap row;
                const auto mount = v.mountPoint.generic_string();
                row["name"] = QString::fromStdString(mount);
                row["mount"] = QString::fromStdString(mount);
                row["total"] = static_cast<qlonglong>(v.totalBytes);
                row["scanned"] = static_cast<qlonglong>(v.scannedBytes);
                row["bytes"] = QString::fromStdString(core::formatBytes(v.scannedBytes));
                row["files"] = static_cast<qlonglong>(v.fileCount);
                row["ratio"] = totalScanned == 0
                                   ? 0.0
                                   : static_cast<double>(v.scannedBytes) / static_cast<double>(totalScanned);
                summaries.push_back(row);
            }
            volumes = std::move(summaries);
            emit resultChanged();
            application->notify(QString("Scan finished: %1 files, %2")
                                    .arg(result->stats.filesIndexed)
                                    .arg(QString::fromStdString(core::formatBytes(result->stats.bytesIndexed))));
        }, Qt::QueuedConnection);
    });
}

void ScanViewModel::cancel() {
    if (engine) { engine->cancel(); }
}

}
