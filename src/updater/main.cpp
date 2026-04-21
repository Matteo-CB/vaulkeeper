#include <filesystem>
#include <string>
#include <vector>

#include <fmt/core.h>
#include <spdlog/spdlog.h>

#include "config/paths.hpp"
#include "platform_init.hpp"
#include "signature.hpp"
#include "update_check.hpp"
#include "util/logger.hpp"
#include "util/version.hpp"

namespace {

void printUsage() {
    fmt::print("vk-updater {}\n\n", vk::version::String);
    fmt::print("usage: vk-updater <command>\n");
    fmt::print("commands:\n");
    fmt::print("  check <endpoint>        Report whether a newer version is available\n");
    fmt::print("  apply <path> [target]   Verify and replace the target binary with the downloaded file\n");
    fmt::print("  verify <path>           Verify Authenticode signature only\n");
}

int runCheck(std::string_view endpoint) {
    auto manifest = vk::updater::fetchManifest(endpoint);
    if (!manifest) {
        fmt::print(stderr, "check failed: {}\n", manifest.error().message);
        return 3;
    }
    const auto newer = vk::updater::newerThanCurrent(manifest->latestVersion, std::string(vk::version::String));
    fmt::print("latest   : {}\ncurrent  : {}\nnewer    : {}\nnotes    : {}\n",
               manifest->latestVersion, vk::version::String, newer ? "yes" : "no", manifest->releaseNotes);
    return newer ? 0 : 1;
}

int runVerify(const std::filesystem::path& binary) {
    auto result = vk::updater::verifyAuthenticode(binary);
    if (!result) {
        fmt::print(stderr, "verification failed: {}\n", result.error().message);
        return 3;
    }
    fmt::print("signature ok\n");
    return 0;
}

int runApply(const std::filesystem::path& incoming, const std::filesystem::path& target) {
    auto signature = vk::updater::verifyAuthenticode(incoming);
    if (!signature) {
        fmt::print(stderr, "will not apply unsigned payload: {}\n", signature.error().message);
        return 3;
    }
    auto hash = vk::updater::sha256OfFile(incoming);
    if (hash) { fmt::print("incoming sha256: {}\n", *hash); }

    std::error_code ec;
    const auto backup = target.parent_path() / (target.filename().string() + ".bak");
    if (std::filesystem::exists(target, ec)) {
        std::filesystem::rename(target, backup, ec);
        if (ec) {
            fmt::print(stderr, "cannot move existing binary: {}\n", ec.message());
            return 3;
        }
    }
    std::filesystem::copy_file(incoming, target, std::filesystem::copy_options::overwrite_existing, ec);
    if (ec) {
        fmt::print(stderr, "apply failed: {}\n", ec.message());
        return 3;
    }
    fmt::print("apply succeeded\n");
    return 0;
}

}

int main(int argc, char** argv) {
    vk::platform::windows::installProviders();

    auto paths = vk::core::Paths::resolveDefault(false);
    paths.ensureCreated();

    vk::core::LogOptions options;
    options.directory = paths.logDir;
    options.fileName = "updater.log";
    vk::core::Logger::initialize(options);

    if (argc < 2) { printUsage(); return 2; }
    const std::string command = argv[1];

    if (command == "check") {
        if (argc < 3) { printUsage(); return 2; }
        return runCheck(argv[2]);
    }
    if (command == "verify") {
        if (argc < 3) { printUsage(); return 2; }
        return runVerify(std::filesystem::path(argv[2]));
    }
    if (command == "apply") {
        if (argc < 3) { printUsage(); return 2; }
        const std::filesystem::path incoming(argv[2]);
        const std::filesystem::path target = argc >= 4 ? std::filesystem::path(argv[3]) : std::filesystem::path("vaulkeeper.exe");
        return runApply(incoming, target);
    }

    printUsage();
    vk::core::Logger::shutdown();
    return 2;
}
