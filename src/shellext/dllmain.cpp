#include <olectl.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <string>
#include <windows.h>

#include "class_factory.hpp"

namespace vk::shellext {

long g_moduleRefs { 0 };
HMODULE g_moduleHandle { nullptr };

static const GUID kClsidVaulkeeperContextMenu = {
    0xA3B5B7F1, 0x1234, 0x4ABF,
    { 0x8F, 0x1A, 0x5E, 0x6D, 0x90, 0x12, 0x34, 0x56 }
};

namespace {

constexpr const wchar_t* kClsidString = L"{A3B5B7F1-1234-4ABF-8F1A-5E6D90123456}";
constexpr const wchar_t* kFriendlyName = L"Vaulkeeper Context Menu";

HRESULT writeRegistryString(HKEY root, const wchar_t* path, const wchar_t* name, const wchar_t* value) {
    HKEY handle = nullptr;
    const auto rc = RegCreateKeyExW(root, path, 0, nullptr, 0, KEY_WRITE, nullptr, &handle, nullptr);
    if (rc != ERROR_SUCCESS) { return HRESULT_FROM_WIN32(rc); }
    const auto setRc = RegSetValueExW(handle, name, 0, REG_SZ,
                                      reinterpret_cast<const BYTE*>(value),
                                      static_cast<DWORD>((wcslen(value) + 1) * sizeof(wchar_t)));
    RegCloseKey(handle);
    return setRc == ERROR_SUCCESS ? S_OK : HRESULT_FROM_WIN32(setRc);
}

void deleteRegistryTree(HKEY root, const wchar_t* path) {
    SHDeleteKeyW(root, path);
}

}

}

extern "C" BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID) {
    if (reason == DLL_PROCESS_ATTACH) {
        vk::shellext::g_moduleHandle = instance;
        DisableThreadLibraryCalls(instance);
    }
    return TRUE;
}

extern "C" HRESULT __stdcall DllGetClassObject(REFCLSID clsid, REFIID riid, void** ppv) {
    if (ppv == nullptr) { return E_POINTER; }
    if (!IsEqualCLSID(clsid, vk::shellext::kClsidVaulkeeperContextMenu)) {
        return CLASS_E_CLASSNOTAVAILABLE;
    }
    auto* factory = new (std::nothrow) vk::shellext::ClassFactory;
    if (factory == nullptr) { return E_OUTOFMEMORY; }
    const auto hr = factory->QueryInterface(riid, ppv);
    factory->Release();
    return hr;
}

extern "C" HRESULT __stdcall DllCanUnloadNow() {
    return vk::shellext::g_moduleRefs == 0 ? S_OK : S_FALSE;
}

extern "C" HRESULT __stdcall DllRegisterServer() {
    wchar_t modulePath[MAX_PATH] {};
    if (GetModuleFileNameW(vk::shellext::g_moduleHandle, modulePath, MAX_PATH) == 0) {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    const auto clsRootKey = std::wstring(L"Software\\Classes\\CLSID\\") + vk::shellext::kClsidString;
    const auto clsInProcKey = clsRootKey + L"\\InprocServer32";
    const std::wstring contextMenuKey = L"Software\\Classes\\*\\shellex\\ContextMenuHandlers\\Vaulkeeper";
    const std::wstring directoryKey = L"Software\\Classes\\Directory\\shellex\\ContextMenuHandlers\\Vaulkeeper";

    HRESULT hr = vk::shellext::writeRegistryString(HKEY_LOCAL_MACHINE, clsRootKey.c_str(), nullptr, vk::shellext::kFriendlyName);
    if (FAILED(hr)) { return hr; }
    hr = vk::shellext::writeRegistryString(HKEY_LOCAL_MACHINE, clsInProcKey.c_str(), nullptr, modulePath);
    if (FAILED(hr)) { return hr; }
    hr = vk::shellext::writeRegistryString(HKEY_LOCAL_MACHINE, clsInProcKey.c_str(), L"ThreadingModel", L"Apartment");
    if (FAILED(hr)) { return hr; }
    hr = vk::shellext::writeRegistryString(HKEY_LOCAL_MACHINE, contextMenuKey.c_str(), nullptr, vk::shellext::kClsidString);
    if (FAILED(hr)) { return hr; }
    hr = vk::shellext::writeRegistryString(HKEY_LOCAL_MACHINE, directoryKey.c_str(), nullptr, vk::shellext::kClsidString);
    if (FAILED(hr)) { return hr; }

    SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, nullptr, nullptr);
    return S_OK;
}

extern "C" HRESULT __stdcall DllUnregisterServer() {
    const auto clsRootKey = std::wstring(L"Software\\Classes\\CLSID\\") + vk::shellext::kClsidString;
    const std::wstring contextMenuKey = L"Software\\Classes\\*\\shellex\\ContextMenuHandlers\\Vaulkeeper";
    const std::wstring directoryKey = L"Software\\Classes\\Directory\\shellex\\ContextMenuHandlers\\Vaulkeeper";

    vk::shellext::deleteRegistryTree(HKEY_LOCAL_MACHINE, clsRootKey.c_str());
    vk::shellext::deleteRegistryTree(HKEY_LOCAL_MACHINE, contextMenuKey.c_str());
    vk::shellext::deleteRegistryTree(HKEY_LOCAL_MACHINE, directoryKey.c_str());

    SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, nullptr, nullptr);
    return S_OK;
}
