#include "bitlocker/bitlocker.hpp"

#include <charconv>
#include <string>

#include "wmi/wmi_query.hpp"

namespace vk::platform::windows {

namespace {

BitLockerProtection translateStatus(std::string_view raw) {
    int value = 0;
    std::from_chars(raw.data(), raw.data() + raw.size(), value);
    switch (value) {
        case 0: return BitLockerProtection::Off;
        case 1: return BitLockerProtection::On;
        case 2: return BitLockerProtection::Suspended;
        default: return BitLockerProtection::Unknown;
    }
}

std::string translateMethod(std::string_view raw) {
    int value = 0;
    std::from_chars(raw.data(), raw.data() + raw.size(), value);
    switch (value) {
        case 1: return "AES 128 with diffuser";
        case 2: return "AES 256 with diffuser";
        case 3: return "AES 128";
        case 4: return "AES 256";
        case 6: return "XTS AES 128";
        case 7: return "XTS AES 256";
        default: return "unknown";
    }
}

}

core::Result<std::vector<BitLockerStatus>> enumerateBitLockerVolumes() {
    std::vector<BitLockerStatus> out;
    auto rows = queryWmi(L"SELECT DriveLetter, ProtectionStatus, EncryptionMethod, EncryptionPercentage FROM Win32_EncryptableVolume",
                         L"ROOT\\CIMV2\\Security\\MicrosoftVolumeEncryption");
    if (!rows) { return out; }
    for (const auto& row : *rows) {
        BitLockerStatus status;
        if (auto it = row.find("DriveLetter"); it != row.end()) { status.driveLetter = it->second; }
        if (auto it = row.find("ProtectionStatus"); it != row.end()) { status.protection = translateStatus(it->second); }
        if (auto it = row.find("EncryptionMethod"); it != row.end()) { status.encryptionMethod = translateMethod(it->second); }
        if (auto it = row.find("EncryptionPercentage"); it != row.end()) {
            double v = 0.0;
            try { v = std::stod(it->second); } catch (...) { v = 0.0; }
            status.encryptionPercentage = v;
        }
        out.push_back(std::move(status));
    }
    return out;
}

}
