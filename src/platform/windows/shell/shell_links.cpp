#include "shell/shell_links.hpp"

#ifdef _WIN32
    #include <objbase.h>
    #include <shobjidl.h>
    #include <shlguid.h>
    #include <shlobj.h>
    #include <windows.h>
#endif

namespace vk::platform::windows {

namespace {

#ifdef _WIN32

std::string narrow(const wchar_t* value) {
    if (value == nullptr || *value == L'\0') { return {}; }
    const auto needed = WideCharToMultiByte(CP_UTF8, 0, value, -1, nullptr, 0, nullptr, nullptr);
    if (needed <= 1) { return {}; }
    std::string out(static_cast<std::size_t>(needed - 1), '\0');
    WideCharToMultiByte(CP_UTF8, 0, value, -1, out.data(), needed, nullptr, nullptr);
    return out;
}

bool resolveShortcut(const std::filesystem::path& path, Shortcut& shortcut) {
    IShellLinkW* shellLink = nullptr;
    if (FAILED(CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER, IID_IShellLinkW, reinterpret_cast<void**>(&shellLink)))) {
        return false;
    }
    IPersistFile* persist = nullptr;
    if (FAILED(shellLink->QueryInterface(IID_IPersistFile, reinterpret_cast<void**>(&persist)))) {
        shellLink->Release();
        return false;
    }

    bool ok = SUCCEEDED(persist->Load(path.c_str(), STGM_READ));
    if (ok) {
        wchar_t target[MAX_PATH] {};
        if (SUCCEEDED(shellLink->GetPath(target, MAX_PATH, nullptr, SLGP_RAWPATH))) {
            shortcut.target = std::filesystem::path { target };
            DWORD attrs = GetFileAttributesW(target);
            shortcut.broken = attrs == INVALID_FILE_ATTRIBUTES;
        }
        wchar_t arguments[INFOTIPSIZE] {};
        if (SUCCEEDED(shellLink->GetArguments(arguments, INFOTIPSIZE))) {
            shortcut.arguments = narrow(arguments);
        }
        wchar_t workingDir[MAX_PATH] {};
        if (SUCCEEDED(shellLink->GetWorkingDirectory(workingDir, MAX_PATH))) {
            shortcut.workingDirectory = narrow(workingDir);
        }
    }

    persist->Release();
    shellLink->Release();
    return ok;
}

#endif

}

core::Result<std::vector<Shortcut>> enumerateShortcuts(const std::filesystem::path& directory) {
    std::vector<Shortcut> out;
    std::error_code ec;
    if (!std::filesystem::exists(directory, ec)) { return out; }

#ifdef _WIN32
    const auto initResult = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    const bool initHere = SUCCEEDED(initResult);

    for (auto it = std::filesystem::recursive_directory_iterator(directory, ec);
         it != std::filesystem::recursive_directory_iterator(); ++it) {
        if (ec) { ec.clear(); continue; }
        if (!it->is_regular_file(ec)) { continue; }
        if (it->path().extension() != ".lnk") { continue; }

        Shortcut shortcut;
        shortcut.linkFile = it->path();
        if (!resolveShortcut(it->path(), shortcut)) { shortcut.broken = true; }
        out.push_back(std::move(shortcut));
    }

    if (initHere) { CoUninitialize(); }
#else
    for (auto it = std::filesystem::recursive_directory_iterator(directory, ec);
         it != std::filesystem::recursive_directory_iterator(); ++it) {
        if (ec) { ec.clear(); continue; }
        if (it->is_regular_file(ec) && it->path().extension() == ".lnk") {
            Shortcut shortcut;
            shortcut.linkFile = it->path();
            shortcut.broken = true;
            out.push_back(std::move(shortcut));
        }
    }
#endif
    return out;
}

}
