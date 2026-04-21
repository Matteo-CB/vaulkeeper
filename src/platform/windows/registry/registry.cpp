#include "registry/registry.hpp"

#include <array>

#ifdef _WIN32
    #include <windows.h>
#endif

namespace vk::platform::windows {

namespace {

#ifdef _WIN32
HKEY translate(RegistryHive hive) {
    switch (hive) {
        case RegistryHive::LocalMachine: return HKEY_LOCAL_MACHINE;
        case RegistryHive::CurrentUser: return HKEY_CURRENT_USER;
        case RegistryHive::ClassesRoot: return HKEY_CLASSES_ROOT;
        case RegistryHive::Users: return HKEY_USERS;
        case RegistryHive::CurrentConfig: return HKEY_CURRENT_CONFIG;
    }
    return HKEY_LOCAL_MACHINE;
}

REGSAM access(bool wow64, REGSAM base) {
    return base | (wow64 ? KEY_WOW64_32KEY : KEY_WOW64_64KEY);
}
#endif

}

core::Result<std::vector<std::string>> enumerateSubkeys(RegistryHive hive, std::string_view path, bool wow64) {
#ifdef _WIN32
    HKEY handle {};
    if (RegOpenKeyExA(translate(hive), std::string(path).c_str(), 0, access(wow64, KEY_READ), &handle) != ERROR_SUCCESS) {
        return core::fromLastOsError("registry open");
    }
    std::vector<std::string> subkeys;
    std::array<char, 256> buffer {};
    DWORD size = 0;
    DWORD index = 0;
    while (true) {
        size = static_cast<DWORD>(buffer.size());
        const auto rc = RegEnumKeyExA(handle, index++, buffer.data(), &size, nullptr, nullptr, nullptr, nullptr);
        if (rc == ERROR_NO_MORE_ITEMS) { break; }
        if (rc != ERROR_SUCCESS) { break; }
        subkeys.emplace_back(buffer.data(), size);
    }
    RegCloseKey(handle);
    return subkeys;
#else
    (void)hive; (void)path; (void)wow64;
    return core::fail(core::ErrorCode::NotSupported, "registry requires Windows");
#endif
}

core::Result<std::vector<RegistryValue>> readValues(RegistryHive hive, std::string_view path, bool wow64) {
#ifdef _WIN32
    HKEY handle {};
    if (RegOpenKeyExA(translate(hive), std::string(path).c_str(), 0, access(wow64, KEY_READ), &handle) != ERROR_SUCCESS) {
        return core::fromLastOsError("registry open");
    }
    std::vector<RegistryValue> values;
    std::array<char, 1024> nameBuffer {};
    std::array<std::uint8_t, 8192> dataBuffer {};
    DWORD index = 0;
    while (true) {
        DWORD nameLen = static_cast<DWORD>(nameBuffer.size());
        DWORD dataLen = static_cast<DWORD>(dataBuffer.size());
        DWORD type = 0;
        const auto rc = RegEnumValueA(handle, index++, nameBuffer.data(), &nameLen, nullptr, &type, dataBuffer.data(), &dataLen);
        if (rc == ERROR_NO_MORE_ITEMS) { break; }
        if (rc != ERROR_SUCCESS) { continue; }
        RegistryValue value;
        value.name.assign(nameBuffer.data(), nameLen);
        switch (type) {
            case REG_SZ:
            case REG_EXPAND_SZ:
                value.type = "string";
                value.stringData.assign(reinterpret_cast<char*>(dataBuffer.data()), dataLen);
                break;
            case REG_DWORD:
                value.type = "dword";
                std::memcpy(&value.dwordData, dataBuffer.data(), sizeof(value.dwordData));
                break;
            case REG_QWORD:
                value.type = "qword";
                std::memcpy(&value.qwordData, dataBuffer.data(), sizeof(value.qwordData));
                break;
            default:
                value.type = "binary";
                break;
        }
        values.push_back(std::move(value));
    }
    RegCloseKey(handle);
    return values;
#else
    (void)hive; (void)path; (void)wow64;
    return core::fail(core::ErrorCode::NotSupported, "registry requires Windows");
#endif
}

core::Result<void> writeString(RegistryHive hive, std::string_view path, std::string_view valueName, std::string_view data) {
#ifdef _WIN32
    HKEY handle {};
    if (RegCreateKeyExA(translate(hive), std::string(path).c_str(), 0, nullptr, 0, KEY_WRITE, nullptr, &handle, nullptr) != ERROR_SUCCESS) {
        return core::fromLastOsError("registry create");
    }
    const auto rc = RegSetValueExA(handle, std::string(valueName).c_str(), 0, REG_SZ,
                                   reinterpret_cast<const BYTE*>(data.data()), static_cast<DWORD>(data.size() + 1));
    RegCloseKey(handle);
    if (rc != ERROR_SUCCESS) { return core::fromLastOsError("registry set"); }
    return core::ok();
#else
    (void)hive; (void)path; (void)valueName; (void)data;
    return core::fail(core::ErrorCode::NotSupported, "registry requires Windows");
#endif
}

core::Result<void> deleteKey(RegistryHive hive, std::string_view path, bool wow64) {
#ifdef _WIN32
    const auto rc = RegDeleteKeyExA(translate(hive), std::string(path).c_str(), wow64 ? KEY_WOW64_32KEY : KEY_WOW64_64KEY, 0);
    if (rc != ERROR_SUCCESS) { return core::fromLastOsError("registry delete"); }
    return core::ok();
#else
    (void)hive; (void)path; (void)wow64;
    return core::fail(core::ErrorCode::NotSupported, "registry requires Windows");
#endif
}

}
