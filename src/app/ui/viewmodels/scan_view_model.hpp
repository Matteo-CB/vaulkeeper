#pragma once

#include <memory>

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariantList>

#include "scan/scan_engine.hpp"
#include "scan/scan_result.hpp"

namespace vk::app {

class ApplicationViewModel;

class ScanViewModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool running READ running NOTIFY runningChanged)
    Q_PROPERTY(double progress READ progress NOTIFY progressChanged)
    Q_PROPERTY(QString currentPath READ currentPath NOTIFY progressChanged)
    Q_PROPERTY(QVariantList volumeSummaries READ volumeSummaries NOTIFY resultChanged)
    Q_PROPERTY(qint64 filesIndexed READ filesIndexed NOTIFY progressChanged)
    Q_PROPERTY(qint64 bytesIndexed READ bytesIndexed NOTIFY progressChanged)

public:
    explicit ScanViewModel(std::shared_ptr<ApplicationViewModel> app, QObject* parent = nullptr);

    [[nodiscard]] bool running() const { return busy; }
    [[nodiscard]] double progress() const { return ratio; }
    [[nodiscard]] QString currentPath() const { return currentlyScanning; }
    [[nodiscard]] QVariantList volumeSummaries() const { return volumes; }
    [[nodiscard]] qint64 filesIndexed() const { return files; }
    [[nodiscard]] qint64 bytesIndexed() const { return bytes; }

public slots:
    void scanAll();
    void scanRoots(const QStringList& roots);
    void cancel();

signals:
    void runningChanged();
    void progressChanged();
    void resultChanged();

private:
    std::shared_ptr<ApplicationViewModel> application;
    std::shared_ptr<core::ScanEngine> engine;
    QVariantList volumes;
    QString currentlyScanning;
    double ratio { 0.0 };
    qint64 files { 0 };
    qint64 bytes { 0 };
    bool busy { false };
};

}
