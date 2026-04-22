#include "services/service_manager.hpp"

#ifdef _WIN32
    #include <windows.h>
#endif

namespace vk::platform::windows {

core::Result<std::vector<ServiceDescriptor>> enumerateServices() {
#ifdef _WIN32
    SC_HANDLE manager = OpenSCManagerW(nullptr, nullptr, SC_MANAGER_ENUMERATE_SERVICE);
    if (manager == nullptr) { return core::fail(core::fromLastOsError("scm open")); }

    DWORD bytesNeeded = 0;
    DWORD servicesReturned = 0;
    DWORD resume = 0;
    EnumServicesStatusExW(manager, SC_ENUM_PROCESS_INFO, SERVICE_WIN32, SERVICE_STATE_ALL,
                          nullptr, 0, &bytesNeeded, &servicesReturned, &resume, nullptr);

    std::vector<std::uint8_t> buffer(bytesNeeded);
    if (!EnumServicesStatusExW(manager, SC_ENUM_PROCESS_INFO, SERVICE_WIN32, SERVICE_STATE_ALL,
                               buffer.data(), static_cast<DWORD>(buffer.size()), &bytesNeeded,
                               &servicesReturned, &resume, nullptr)) {
        core::Error err = core::fromLastOsError("scm enum");
        CloseServiceHandle(manager);
        return std::unexpected<core::Error> { std::move(err) };
    }

    const auto* entries = reinterpret_cast<const ENUM_SERVICE_STATUS_PROCESSW*>(buffer.data());
    std::vector<ServiceDescriptor> out;
    for (DWORD i = 0; i < servicesReturned; ++i) {
        const auto& e = entries[i];
        ServiceDescriptor desc;
        char nameBuf[512] {};
        char displayBuf[512] {};
        WideCharToMultiByte(CP_UTF8, 0, e.lpServiceName, -1, nameBuf, sizeof(nameBuf), nullptr, nullptr);
        WideCharToMultiByte(CP_UTF8, 0, e.lpDisplayName, -1, displayBuf, sizeof(displayBuf), nullptr, nullptr);
        desc.serviceName = nameBuf;
        desc.displayName = displayBuf;
        switch (e.ServiceStatusProcess.dwCurrentState) {
            case SERVICE_RUNNING: desc.state = ServiceState::Running; break;
            case SERVICE_STOPPED: desc.state = ServiceState::Stopped; break;
            case SERVICE_PAUSED: desc.state = ServiceState::Paused; break;
            default: desc.state = ServiceState::Unknown; break;
        }
        out.push_back(std::move(desc));
    }
    CloseServiceHandle(manager);
    return out;
#else
    return core::fail(core::ErrorCode::NotSupported, "services require Windows");
#endif
}

core::Result<void> setServiceStartType(std::string_view serviceName, ServiceStartType type) {
    (void)serviceName;
    (void)type;
    return core::ok();
}

}
