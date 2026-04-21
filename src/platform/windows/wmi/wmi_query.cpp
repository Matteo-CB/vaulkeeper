#include "wmi/wmi_query.hpp"

#ifdef _WIN32
    #define _WIN32_DCOM
    #include <Wbemidl.h>
    #include <comdef.h>
    #include <windows.h>
#endif

namespace vk::platform::windows {

namespace {

#ifdef _WIN32
std::string narrow(const BSTR bstr) {
    if (bstr == nullptr) { return {}; }
    const auto length = SysStringLen(bstr);
    const auto needed = WideCharToMultiByte(CP_UTF8, 0, bstr, static_cast<int>(length), nullptr, 0, nullptr, nullptr);
    std::string out(static_cast<std::size_t>(needed), '\0');
    WideCharToMultiByte(CP_UTF8, 0, bstr, static_cast<int>(length), out.data(), needed, nullptr, nullptr);
    return out;
}
#endif

}

core::Result<std::vector<WmiRow>> queryWmi(std::wstring_view wqlQuery, std::wstring_view nameSpace) {
#ifdef _WIN32
    std::vector<WmiRow> rows;
    const auto hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    bool coInitializedHere = SUCCEEDED(hr);

    if (FAILED(CoInitializeSecurity(nullptr, -1, nullptr, nullptr,
                                    RPC_C_AUTHN_LEVEL_DEFAULT, RPC_C_IMP_LEVEL_IMPERSONATE,
                                    nullptr, EOAC_NONE, nullptr))) {
        if (coInitializedHere) { CoUninitialize(); }
        return core::fail(core::ErrorCode::IoError, "wmi CoInitializeSecurity failed");
    }

    IWbemLocator* locator = nullptr;
    if (FAILED(CoCreateInstance(CLSID_WbemLocator, nullptr, CLSCTX_INPROC_SERVER, IID_IWbemLocator, reinterpret_cast<LPVOID*>(&locator)))) {
        if (coInitializedHere) { CoUninitialize(); }
        return core::fail(core::ErrorCode::IoError, "wmi locator failed");
    }

    IWbemServices* services = nullptr;
    if (FAILED(locator->ConnectServer(_bstr_t(nameSpace.data()), nullptr, nullptr, 0, 0, nullptr, nullptr, &services))) {
        locator->Release();
        if (coInitializedHere) { CoUninitialize(); }
        return core::fail(core::ErrorCode::IoError, "wmi ConnectServer failed");
    }

    CoSetProxyBlanket(services, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, nullptr,
                      RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, nullptr, EOAC_NONE);

    IEnumWbemClassObject* enumerator = nullptr;
    if (FAILED(services->ExecQuery(_bstr_t(L"WQL"), _bstr_t(wqlQuery.data()),
                                   WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
                                   nullptr, &enumerator))) {
        services->Release();
        locator->Release();
        if (coInitializedHere) { CoUninitialize(); }
        return core::fail(core::ErrorCode::IoError, "wmi ExecQuery failed");
    }

    IWbemClassObject* object = nullptr;
    ULONG returned = 0;
    while (enumerator != nullptr && SUCCEEDED(enumerator->Next(WBEM_INFINITE, 1, &object, &returned)) && returned > 0) {
        WmiRow row;
        VARIANT value;
        VariantInit(&value);

        object->BeginEnumeration(0);
        BSTR propName = nullptr;
        while (object->Next(0, &propName, &value, nullptr, nullptr) == WBEM_S_NO_ERROR) {
            std::string key = narrow(propName);
            std::string content;
            if (value.vt == VT_BSTR && value.bstrVal != nullptr) {
                content = narrow(value.bstrVal);
            } else if (value.vt == VT_I4) {
                content = std::to_string(value.lVal);
            } else if (value.vt == VT_UI4) {
                content = std::to_string(value.ulVal);
            } else if (value.vt == VT_UI8 || value.vt == VT_I8) {
                content = std::to_string(value.llVal);
            } else if (value.vt == VT_BOOL) {
                content = value.boolVal == VARIANT_TRUE ? "true" : "false";
            }
            row.emplace(std::move(key), std::move(content));
            SysFreeString(propName);
            VariantClear(&value);
        }
        object->EndEnumeration();
        object->Release();
        rows.push_back(std::move(row));
    }

    if (enumerator != nullptr) { enumerator->Release(); }
    services->Release();
    locator->Release();
    if (coInitializedHere) { CoUninitialize(); }
    return rows;
#else
    (void)wqlQuery;
    (void)nameSpace;
    return core::fail(core::ErrorCode::NotSupported, "wmi requires Windows");
#endif
}

}
