#include "smart/smart_reader.hpp"

#include <array>
#include <cstring>
#include <string>
#include <vector>

#ifdef _WIN32
    #include <windows.h>
    #include <winioctl.h>
#endif

namespace vk::platform::windows {

namespace {

#ifdef _WIN32

std::string trimmedAscii(const std::uint8_t* data, std::size_t length) {
    std::string out;
    out.reserve(length);
    for (std::size_t i = 0; i < length; ++i) {
        const char c = static_cast<char>(data[i]);
        if (c != '\0') { out.push_back(c); }
    }
    while (!out.empty() && out.back() == ' ') { out.pop_back(); }
    while (!out.empty() && out.front() == ' ') { out.erase(out.begin()); }
    return out;
}

core::DiskHealthReport queryPhysicalDrive(std::uint32_t index) {
    core::DiskHealthReport report;
    report.kind = core::DiskMediaKind::Unknown;

    const auto devicePath = std::wstring { L"\\\\.\\PhysicalDrive" } + std::to_wstring(index);
    HANDLE handle = CreateFileW(devicePath.c_str(), GENERIC_READ | GENERIC_WRITE,
                                FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr,
                                OPEN_EXISTING, 0, nullptr);
    if (handle == INVALID_HANDLE_VALUE) {
        handle = CreateFileW(devicePath.c_str(), 0,
                             FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr,
                             OPEN_EXISTING, 0, nullptr);
    }
    if (handle == INVALID_HANDLE_VALUE) { return report; }

    STORAGE_PROPERTY_QUERY query {};
    query.PropertyId = StorageDeviceProperty;
    query.QueryType = PropertyStandardQuery;
    std::array<std::uint8_t, 8192> buffer {};
    DWORD returned = 0;
    if (DeviceIoControl(handle, IOCTL_STORAGE_QUERY_PROPERTY,
                        &query, sizeof(query),
                        buffer.data(), static_cast<DWORD>(buffer.size()),
                        &returned, nullptr)) {
        const auto* descriptor = reinterpret_cast<STORAGE_DEVICE_DESCRIPTOR*>(buffer.data());
        if (descriptor->ProductIdOffset != 0) {
            report.deviceModel = trimmedAscii(buffer.data() + descriptor->ProductIdOffset,
                                              returned - descriptor->ProductIdOffset);
        }
        if (descriptor->SerialNumberOffset != 0) {
            report.serial = trimmedAscii(buffer.data() + descriptor->SerialNumberOffset,
                                         returned - descriptor->SerialNumberOffset);
        }
        if (descriptor->ProductRevisionOffset != 0) {
            report.firmware = trimmedAscii(buffer.data() + descriptor->ProductRevisionOffset,
                                           returned - descriptor->ProductRevisionOffset);
        }
        switch (descriptor->BusType) {
            case BusTypeNvme: report.kind = core::DiskMediaKind::Nvme; break;
            case BusTypeUsb: report.kind = core::DiskMediaKind::Usb; break;
            default: report.kind = core::DiskMediaKind::Hdd; break;
        }
    }

    STORAGE_PROPERTY_QUERY seekPenalty {};
    seekPenalty.PropertyId = StorageDeviceSeekPenaltyProperty;
    seekPenalty.QueryType = PropertyStandardQuery;
    DEVICE_SEEK_PENALTY_DESCRIPTOR penalty {};
    if (DeviceIoControl(handle, IOCTL_STORAGE_QUERY_PROPERTY,
                        &seekPenalty, sizeof(seekPenalty),
                        &penalty, sizeof(penalty), &returned, nullptr)) {
        if (!penalty.IncursSeekPenalty && report.kind != core::DiskMediaKind::Nvme) {
            report.kind = core::DiskMediaKind::Ssd;
        }
    }

    DISK_GEOMETRY_EX geometry {};
    if (DeviceIoControl(handle, IOCTL_DISK_GET_DRIVE_GEOMETRY_EX, nullptr, 0,
                        &geometry, sizeof(geometry), &returned, nullptr)) {
        report.capacityBytes = static_cast<std::uint64_t>(geometry.DiskSize.QuadPart);
    }

    STORAGE_PREDICT_FAILURE prediction {};
    if (DeviceIoControl(handle, IOCTL_STORAGE_PREDICT_FAILURE, nullptr, 0,
                        &prediction, sizeof(prediction), &returned, nullptr)) {
        if (prediction.PredictFailure != 0) {
            report.riskAssessment = "predicted failure";
        } else {
            report.riskAssessment = "healthy";
        }
    } else {
        report.riskAssessment = "unknown";
    }

    CloseHandle(handle);
    return report;
}

#endif

}

core::Result<std::vector<core::DiskHealthReport>> readSmartReports() {
    std::vector<core::DiskHealthReport> out;
#ifdef _WIN32
    for (std::uint32_t index = 0; index < 32; ++index) {
        auto report = queryPhysicalDrive(index);
        if (report.deviceModel.empty() && report.capacityBytes == 0) { continue; }
        out.push_back(std::move(report));
    }
#endif
    return out;
}

}
