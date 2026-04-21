#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QGuiApplication>
#include <QIcon>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <QStandardPaths>
#include <QTranslator>

#include <spdlog/spdlog.h>

#include "platform_init.hpp"
#include "config/paths.hpp"
#include "config/app_config.hpp"
#include "ui/theme/theme.hpp"
#include "ui/viewmodels/application_view_model.hpp"
#include "ui/viewmodels/duplicates_view_model.hpp"
#include "ui/viewmodels/health_view_model.hpp"
#include "ui/viewmodels/quarantine_view_model.hpp"
#include "ui/viewmodels/rules_view_model.hpp"
#include "ui/viewmodels/scan_view_model.hpp"
#include "ui/viewmodels/uninstall_view_model.hpp"
#include "util/logger.hpp"
#include "util/version.hpp"

namespace {

bool detectPortableMode(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        const QString arg = QString::fromLocal8Bit(argv[i]);
        if (arg == "--portable") { return true; }
    }
    return QFileInfo::exists(QCoreApplication::applicationDirPath() + "/portable.flag");
}

}

int main(int argc, char** argv) {
    QGuiApplication::setApplicationName("Vaulkeeper");
    QGuiApplication::setOrganizationName("Vaulkeeper");
    QGuiApplication::setApplicationVersion(QString::fromStdString(std::string(vk::version::String)));
    QGuiApplication::setWindowIcon(QIcon(":/icons/vaulkeeper.svg"));

    QGuiApplication app(argc, argv);
    QQuickStyle::setStyle("Basic");

    const bool portable = detectPortableMode(argc, argv);
    auto paths = vk::core::Paths::resolveDefault(portable);
    paths.ensureCreated();

    vk::platform::windows::installProviders();

    vk::core::LogOptions logOptions;
    logOptions.directory = paths.logDir;
    logOptions.level = vk::core::LogLevel::Info;
    vk::core::Logger::initialize(logOptions);
    spdlog::info("vaulkeeper {} ({} on {}) starting", vk::version::String, vk::version::GitSha, vk::version::GitBranch);

    const auto configPath = paths.configDir / "vaulkeeper.toml";
    auto config = vk::core::AppConfig::load(configPath).value_or(vk::core::AppConfig::defaults());

    QTranslator translator;
    const QString locale = QString::fromStdString(config.ui.locale);
    if (translator.load(":/i18n/vaulkeeper_" + locale)) {
        app.installTranslator(&translator);
    }

    vk::app::Theme::installDefault(config.ui.theme);

    auto applicationVm = std::make_shared<vk::app::ApplicationViewModel>(config, paths);
    auto scanVm = std::make_shared<vk::app::ScanViewModel>(applicationVm);
    auto duplicatesVm = std::make_shared<vk::app::DuplicatesViewModel>(applicationVm);
    auto uninstallVm = std::make_shared<vk::app::UninstallViewModel>(applicationVm);
    auto rulesVm = std::make_shared<vk::app::RulesViewModel>(applicationVm);
    auto quarantineVm = std::make_shared<vk::app::QuarantineViewModel>(applicationVm);
    auto healthVm = std::make_shared<vk::app::HealthViewModel>(applicationVm);

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("applicationVm", applicationVm.get());
    engine.rootContext()->setContextProperty("scanVm", scanVm.get());
    engine.rootContext()->setContextProperty("duplicatesVm", duplicatesVm.get());
    engine.rootContext()->setContextProperty("uninstallVm", uninstallVm.get());
    engine.rootContext()->setContextProperty("rulesVm", rulesVm.get());
    engine.rootContext()->setContextProperty("quarantineVm", quarantineVm.get());
    engine.rootContext()->setContextProperty("healthVm", healthVm.get());
    engine.rootContext()->setContextProperty("appVersion", QString::fromStdString(std::string(vk::version::String)));

    engine.loadFromModule("Vaulkeeper", "Main");
    if (engine.rootObjects().isEmpty()) {
        spdlog::critical("qml engine failed to create root window");
        return 1;
    }

    const auto exitCode = app.exec();
    vk::core::Logger::shutdown();
    return exitCode;
}
