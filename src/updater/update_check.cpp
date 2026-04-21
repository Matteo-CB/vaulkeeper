#include "update_check.hpp"

#include <array>
#include <charconv>
#include <string>
#include <vector>

#ifdef _WIN32
    #include <windows.h>
    #include <winhttp.h>
    #pragma comment(lib, "winhttp.lib")
#endif

namespace vk::updater {

namespace {

std::vector<int> splitVersion(std::string_view version) {
    std::vector<int> parts;
    std::size_t start = 0;
    while (start < version.size()) {
        const auto dot = version.find('.', start);
        const auto end = dot == std::string_view::npos ? version.size() : dot;
        int value = 0;
        std::from_chars(version.data() + start, version.data() + end, value);
        parts.push_back(value);
        if (dot == std::string_view::npos) { break; }
        start = dot + 1;
    }
    while (parts.size() < 3) { parts.push_back(0); }
    return parts;
}

#ifdef _WIN32

std::wstring widen(std::string_view value) {
    if (value.empty()) { return {}; }
    const auto needed = MultiByteToWideChar(CP_UTF8, 0, value.data(), static_cast<int>(value.size()), nullptr, 0);
    std::wstring out(static_cast<std::size_t>(needed), L'\0');
    MultiByteToWideChar(CP_UTF8, 0, value.data(), static_cast<int>(value.size()), out.data(), needed);
    return out;
}

core::Result<std::string> httpGet(std::string_view endpoint) {
    URL_COMPONENTSW parsed {};
    parsed.dwStructSize = sizeof(parsed);
    std::wstring host(256, L'\0');
    std::wstring path(1024, L'\0');
    parsed.lpszHostName = host.data();
    parsed.dwHostNameLength = static_cast<DWORD>(host.size());
    parsed.lpszUrlPath = path.data();
    parsed.dwUrlPathLength = static_cast<DWORD>(path.size());

    const auto wide = widen(endpoint);
    if (!WinHttpCrackUrl(wide.c_str(), 0, 0, &parsed)) {
        return core::fail(core::ErrorCode::InvalidArgument, "cannot parse update endpoint");
    }

    HINTERNET session = WinHttpOpen(L"Vaulkeeper-Updater/1.0",
                                    WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                                    WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (session == nullptr) { return core::fail(core::ErrorCode::IoError, "WinHttpOpen failed"); }

    HINTERNET connection = WinHttpConnect(session, host.c_str(), parsed.nPort, 0);
    if (connection == nullptr) {
        WinHttpCloseHandle(session);
        return core::fail(core::ErrorCode::IoError, "WinHttpConnect failed");
    }

    const DWORD flags = parsed.nScheme == INTERNET_SCHEME_HTTPS ? WINHTTP_FLAG_SECURE : 0u;
    HINTERNET request = WinHttpOpenRequest(connection, L"GET", path.c_str(), nullptr,
                                           WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, flags);
    if (request == nullptr) {
        WinHttpCloseHandle(connection);
        WinHttpCloseHandle(session);
        return core::fail(core::ErrorCode::IoError, "WinHttpOpenRequest failed");
    }

    if (!WinHttpSendRequest(request, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0) ||
        !WinHttpReceiveResponse(request, nullptr)) {
        WinHttpCloseHandle(request);
        WinHttpCloseHandle(connection);
        WinHttpCloseHandle(session);
        return core::fail(core::ErrorCode::IoError, "http send/receive failed");
    }

    std::string body;
    DWORD available = 0;
    while (WinHttpQueryDataAvailable(request, &available) && available > 0) {
        std::string chunk(available, '\0');
        DWORD read = 0;
        if (!WinHttpReadData(request, chunk.data(), available, &read)) { break; }
        chunk.resize(read);
        body.append(chunk);
    }

    WinHttpCloseHandle(request);
    WinHttpCloseHandle(connection);
    WinHttpCloseHandle(session);
    return body;
}

#endif

std::string extractJsonField(const std::string& body, std::string_view key) {
    const auto keyStart = body.find("\"" + std::string(key) + "\"");
    if (keyStart == std::string::npos) { return {}; }
    const auto colon = body.find(':', keyStart);
    if (colon == std::string::npos) { return {}; }
    const auto quote1 = body.find('"', colon);
    if (quote1 == std::string::npos) { return {}; }
    const auto quote2 = body.find('"', quote1 + 1);
    if (quote2 == std::string::npos) { return {}; }
    return body.substr(quote1 + 1, quote2 - quote1 - 1);
}

}

core::Result<UpdateManifest> fetchManifest(std::string_view endpoint) {
#ifdef _WIN32
    auto body = httpGet(endpoint);
    if (!body) { return core::fail(body.error()); }

    UpdateManifest manifest;
    manifest.latestVersion = extractJsonField(*body, "version");
    manifest.downloadUrl = extractJsonField(*body, "download");
    manifest.sha256 = extractJsonField(*body, "sha256");
    manifest.releaseNotes = extractJsonField(*body, "notes");
    if (manifest.latestVersion.empty() || manifest.downloadUrl.empty()) {
        return core::fail(core::ErrorCode::CorruptedData, "manifest missing required fields");
    }
    return manifest;
#else
    (void)endpoint;
    return core::fail(core::ErrorCode::NotSupported, "update check requires Windows");
#endif
}

bool newerThanCurrent(std::string_view candidate, std::string_view current) {
    const auto a = splitVersion(candidate);
    const auto b = splitVersion(current);
    for (std::size_t i = 0; i < a.size(); ++i) {
        if (a[i] > b[i]) { return true; }
        if (a[i] < b[i]) { return false; }
    }
    return false;
}

}
