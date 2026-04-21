#include "commands/command.hpp"

#include "commands/duplicates_cmd.hpp"
#include "commands/health_cmd.hpp"
#include "commands/quarantine_cmd.hpp"
#include "commands/report_cmd.hpp"
#include "commands/rules_cmd.hpp"
#include "commands/scan_cmd.hpp"
#include "commands/uninstall_cmd.hpp"

namespace vk::cli {

std::vector<CommandPtr> registerAllCommands() {
    return {
        std::make_shared<ScanCommand>(),
        std::make_shared<DuplicatesCommand>(),
        std::make_shared<QuarantineCommand>(),
        std::make_shared<UninstallCommand>(),
        std::make_shared<RulesCommand>(),
        std::make_shared<HealthCommand>(),
        std::make_shared<ReportCommand>()
    };
}

}
