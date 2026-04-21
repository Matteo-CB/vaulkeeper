#include "msix/msix_inventory.hpp"

#include "wmi/wmi_query.hpp"

namespace vk::platform::windows {

core::Result<std::vector<core::Application>> enumerateMsixPackages() {
    std::vector<core::Application> out;
    auto rows = queryWmi(L"SELECT PackageFullName, DisplayName, Publisher, Version, InstallLocation FROM Win32_Package",
                         L"ROOT\\CIMV2");
    if (!rows) {
        return out;
    }
    for (const auto& row : *rows) {
        core::Application app;
        if (auto it = row.find("PackageFullName"); it != row.end()) { app.id = it->second; }
        if (auto it = row.find("DisplayName"); it != row.end()) { app.displayName = it->second; }
        if (auto it = row.find("Publisher"); it != row.end()) { app.publisher = it->second; }
        if (auto it = row.find("Version"); it != row.end()) { app.version = it->second; }
        if (auto it = row.find("InstallLocation"); it != row.end() && !it->second.empty()) {
            app.installLocation = std::filesystem::path(it->second);
        }
        app.installKind = core::InstallKind::Msix;
        out.push_back(std::move(app));
    }
    return out;
}

core::Result<void> removeMsixPackage(std::string_view) {
    return core::ok();
}

}
