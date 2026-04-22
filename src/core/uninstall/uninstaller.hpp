#pragma once

#include <memory>
#include <vector>
#include <filesystem>
#include <string>

#include "uninstall/application.hpp"
#include "util/result.hpp"

namespace vk::core {

class Quarantine;
class Journal;

enum class UninstallOutcome : std::uint8_t {
    Pending,
    Succeeded,
    Failed,
    CancelledByUser,
    RequiresReboot
};

struct UninstallPlan {
    Application target;
    bool runSilent { true };
    bool sweepResidues { true };
    bool scanRegistryResidues { true };
    bool forceIfUninstallerMissing { false };
};

struct UninstallReport {
    std::string applicationId;
    UninstallOutcome outcome { UninstallOutcome::Pending };
    std::vector<std::string> steps;
    std::vector<std::filesystem::path> residualFiles;
    std::vector<std::string> residualRegistryKeys;
    std::uint64_t reclaimedBytes { 0 };
    std::string errorMessage;
};

class Uninstaller {
public:
    Uninstaller(std::shared_ptr<Quarantine> quarantine, std::shared_ptr<Journal> journal);

    [[nodiscard]] Result<std::vector<Application>> enumerateInstalled() const;
    [[nodiscard]] Result<UninstallReport> execute(const UninstallPlan& plan);

private:
    std::shared_ptr<Quarantine> quarantine;
    std::shared_ptr<Journal> journal;
};

}
