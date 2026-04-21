#include "ui/viewmodels/scan_view_model.hpp"

#include <QMetaObject>
#include <QVariantMap>
#include <QtConcurrent/QtConcurrent>

#include "fs/volume.hpp"
#include "ui/viewmodels/application_view_model.hpp"

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
            for (const auto& v : result->volumes) {
                QVariantMap row;
                row["mount"] = QString::fromStdString(v.mountPoint.generic_string());
                row["total"] = static_cast<qlonglong>(v.totalBytes);
                row["scanned"] = static_cast<qlonglong>(v.scannedBytes);
                row["files"] = static_cast<qlonglong>(v.fileCount);
                summaries.push_back(row);
            }
            volumes = std::move(summaries);
            emit resultChanged();
            application->notify("Scan finished");
        }, Qt::QueuedConnection);
    });
}

void ScanViewModel::cancel() {
    if (engine) { engine->cancel(); }
}

}
