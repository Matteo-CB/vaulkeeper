#include "mft/usn_journal.hpp"

#ifdef _WIN32
    #include <windows.h>
    #include <winioctl.h>
#endif

namespace vk::platform::windows {

core::Result<std::vector<UsnChange>> readUsnJournal(const std::wstring& volume, std::uint64_t sinceUsn) {
#ifdef _WIN32
    HANDLE handle = CreateFileW(volume.c_str(), GENERIC_READ,
                                FILE_SHARE_READ | FILE_SHARE_WRITE,
                                nullptr, OPEN_EXISTING, 0, nullptr);
    if (handle == INVALID_HANDLE_VALUE) {
        return core::fail(core::fromLastOsError("usn open"));
    }

    USN_JOURNAL_DATA_V2 journalData {};
    DWORD returned = 0;
    if (!DeviceIoControl(handle, FSCTL_QUERY_USN_JOURNAL, nullptr, 0,
                         &journalData, sizeof(journalData), &returned, nullptr)) {
        core::Error err = core::fromLastOsError("usn query");
        CloseHandle(handle);
        return std::unexpected<core::Error> { std::move(err) };
    }

    READ_USN_JOURNAL_DATA_V1 readData {};
    readData.StartUsn = sinceUsn;
    readData.ReasonMask = 0xFFFFFFFF;
    readData.ReturnOnlyOnClose = 0;
    readData.Timeout = 0;
    readData.BytesToWaitFor = 0;
    readData.UsnJournalID = journalData.UsnJournalID;
    readData.MinMajorVersion = 2;
    readData.MaxMajorVersion = 3;

    std::vector<std::uint8_t> buffer(1 << 20);
    std::vector<UsnChange> changes;

    while (true) {
        DWORD bytesReturned = 0;
        if (!DeviceIoControl(handle, FSCTL_READ_USN_JOURNAL,
                             &readData, sizeof(readData),
                             buffer.data(), static_cast<DWORD>(buffer.size()),
                             &bytesReturned, nullptr)) {
            break;
        }
        if (bytesReturned <= sizeof(USN)) { break; }

        USN nextUsn = 0;
        std::memcpy(&nextUsn, buffer.data(), sizeof(USN));
        DWORD offset = sizeof(USN);
        while (offset < bytesReturned) {
            const auto* record = reinterpret_cast<const USN_RECORD_V2*>(buffer.data() + offset);
            UsnChange change;
            change.recordNumber = record->FileReferenceNumber;
            change.parentRecordNumber = record->ParentFileReferenceNumber;
            change.usn = record->Usn;
            change.reason = record->Reason;
            change.fileAttributes = record->FileAttributes;
            change.fileName.assign(record->FileName, record->FileNameLength / sizeof(WCHAR));
            changes.push_back(std::move(change));
            offset += record->RecordLength;
        }

        readData.StartUsn = nextUsn;
        if (nextUsn == 0) { break; }
    }

    CloseHandle(handle);
    return changes;
#else
    (void)volume;
    (void)sinceUsn;
    return core::fail(core::ErrorCode::NotSupported, "usn requires Windows");
#endif
}

}
