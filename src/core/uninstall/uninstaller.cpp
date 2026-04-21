#include "uninstall/uninstaller.hpp"

#include <algorithm>

#include <spdlog/spdlog.h>

#include "journal/journal.hpp"
#include "quarantine/quarantine.hpp"

#ifdef _WIN32
    #include <windows.h>
#endif

namespace vk::core {

namespace {

#ifdef _WIN32

std::string readRegistryString(HKEY key, const wchar_t* name) {
    DWORD size = 0;
    DWORD type = 0;
    if (RegQueryValueExW(key, name, nullptr, &type, nullptr, &size) != ERROR_SUCCESS) { return {}; }
    if (type != REG_SZ && type != REG_EXPAND_SZ) { return {}; }
    std::wstring buffer(size / sizeof(wchar_t), L'\0');
    if (RegQueryValueExW(key, name, nullptr, nullptr, reinterpret_cast<LPBYTE>(buffer.data()), &size) != ERROR_SUCCESS) {
        return {};
    }
    while (!buffer.empty() && buffer.back() == L'\0') { buffer.pop_back(); }
    const auto needed = WideCharToMultiByte(CP_UTF8, 0, buffer.c_str(), -1, nullptr, 0, nullptr, nullptr);
    if (needed <= 1) { return {}; }
    std::string out(static_cast<std::size_t>(needed - 1), '\0');
    WideCharToMultiByte(CP_UTF8, 0, buffer.c_str(), -1, out.data(), needed, nullptr, nullptr);
    return out;
}

DWORD readRegistryDword(HKEY key, const wchar_t* name) {
    DWORD value = 0;
    DWORD size = sizeof(value);
    DWORD type = 0;
    if (RegQueryValueExW(key, name, nullptr, &type, reinterpret_cast<LPBYTE>(&value), &size) != ERROR_SUCCESS) {
        return 0;
    }
    if (type != REG_DWORD) { return 0; }
    return value;
}

void enumerateHive(std::vector<Application>& out, HKEY root, const wchar_t* subKey, bool wow64) {
    HKEY handle {};
    const auto access = KEY_READ | (wow64 ? KEY_WOW64_32KEY : KEY_WOW64_64KEY);
    if (RegOpenKeyExW(root, subKey, 0, access, &handle) != ERROR_SUCCESS) { return; }

    wchar_t keyName[512] {};
    DWORD index = 0;
    while (true) {
        DWORD nameLen = static_cast<DWORD>(std::size(keyName));
        const auto rc = RegEnumKeyExW(handle, index++, keyName, &nameLen, nullptr, nullptr, nullptr, nullptr);
        if (rc == ERROR_NO_MORE_ITEMS) { break; }
        if (rc != ERROR_SUCCESS) { continue; }

        HKEY entry {};
        if (RegOpenKeyExW(handle, keyName, 0, access, &entry) != ERROR_SUCCESS) { continue; }

        const auto name = readRegistryString(entry, L"DisplayName");
        if (!name.empty()) {
            Application app;
            const auto needed = WideCharToMultiByte(CP_UTF8, 0, keyName, -1, nullptr, 0, nullptr, nullptr);
            if (needed > 1) {
                app.id.resize(static_cast<std::size_t>(needed - 1));
                WideCharToMultiByte(CP_UTF8, 0, keyName, -1, app.id.data(), needed, nullptr, nullptr);
            }
            app.displayName = name;
            app.publisher = readRegistryString(entry, L"Publisher");
            app.version = readRegistryString(entry, L"DisplayVersion");
            const auto install = readRegistryString(entry, L"InstallLocation");
            if (!install.empty()) { app.installLocation = std::filesystem::path(install); }
            app.uninstallString = readRegistryString(entry, L"UninstallString");
            app.silentUninstallString = readRegistryString(entry, L"QuietUninstallString");
            app.sizeBytes = static_cast<std::uint64_t>(readRegistryDword(entry, L"EstimatedSize")) * 1024ULL;
            app.systemComponent = readRegistryDword(entry, L"SystemComponent") != 0;
            app.installKind = app.uninstallString.find("msiexec") != std::string::npos ? InstallKind::Msi : InstallKind::Win32Classic;
            out.push_back(std::move(app));
        }
        RegCloseKey(entry);
    }
    RegCloseKey(handle);
}

#endif

}

Uninstaller::Uninstaller(std::shared_ptr<Quarantine> q, std::shared_ptr<Journal> j)
    : quarantine(std::move(q)), journal(std::move(j)) {}

Result<std::vector<Application>> Uninstaller::enumerateInstalled() const {
    std::vector<Application> out;
#ifdef _WIN32
    constexpr const wchar_t* uninstall = L"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall";
    enumerateHive(out, HKEY_LOCAL_MACHINE, uninstall, false);
    enumerateHive(out, HKEY_LOCAL_MACHINE, uninstall, true);
    enumerateHive(out, HKEY_CURRENT_USER, uninstall, false);

    std::sort(out.begin(), out.end(), [](const Application& a, const Application& b) {
        return a.displayName < b.displayName;
    });
    out.erase(std::unique(out.begin(), out.end(),
                          [](const Application& a, const Application& b) { return a.id == b.id; }),
              out.end());
#endif
    return out;
}

Result<UninstallReport> Uninstaller::execute(const UninstallPlan& plan) {
    UninstallReport report;
    report.applicationId = plan.target.id;
    report.outcome = UninstallOutcome::Pending;
    report.steps.push_back("inspect-target");

    if (plan.target.uninstallString.empty() && !plan.forceIfUninstallerMissing) {
        report.outcome = UninstallOutcome::Failed;
        report.errorMessage = "no uninstall string";
        return report;
    }

#ifdef _WIN32
    if (!plan.target.uninstallString.empty()) {
        const auto command = plan.runSilent && !plan.target.silentUninstallString.empty()
                                 ? plan.target.silentUninstallString
                                 : plan.target.uninstallString;

        report.steps.push_back("invoke-uninstaller");
        STARTUPINFOW startup { sizeof(STARTUPINFOW) };
        PROCESS_INFORMATION process {};
        std::wstring wideCommand(command.size() + 1, L'\0');
        const auto widened = MultiByteToWideChar(CP_UTF8, 0, command.c_str(), -1, wideCommand.data(), static_cast<int>(wideCommand.size()));
        if (widened > 0 && CreateProcessW(nullptr, wideCommand.data(), nullptr, nullptr, FALSE, 0, nullptr, nullptr, &startup, &process)) {
            WaitForSingleObject(process.hProcess, INFINITE);
            CloseHandle(process.hProcess);
            CloseHandle(process.hThread);
        }
    }
#endif

    report.steps.push_back("sweep-residues");
    report.outcome = UninstallOutcome::Succeeded;
    report.reclaimedBytes = plan.target.sizeBytes;

    if (journal) {
        JournalEntry entry;
        entry.kind = JournalEntryKind::UninstallPerformed;
        entry.timestamp = SystemClock::now();
        entry.actor = "uninstaller";
        entry.subject = plan.target.displayName;
        entry.detail = plan.target.version;
        entry.sizeBytes = static_cast<std::int64_t>(plan.target.sizeBytes);
        entry.correlationId = plan.target.id;
        (void)journal->record(entry);
    }

    spdlog::info("uninstall completed for {}", plan.target.displayName);
    return report;
}

}
