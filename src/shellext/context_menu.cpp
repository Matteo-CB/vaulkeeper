#include "context_menu.hpp"

#include <shellapi.h>
#include <strsafe.h>

namespace vk::shellext {

extern long g_moduleRefs;

ContextMenu::ContextMenu() {
    InterlockedIncrement(&g_moduleRefs);
}

HRESULT ContextMenu::QueryInterface(REFIID riid, void** ppv) {
    if (ppv == nullptr) { return E_POINTER; }
    if (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IShellExtInit)) {
        *ppv = static_cast<IShellExtInit*>(this);
        AddRef();
        return S_OK;
    }
    if (IsEqualIID(riid, IID_IContextMenu)) {
        *ppv = static_cast<IContextMenu*>(this);
        AddRef();
        return S_OK;
    }
    *ppv = nullptr;
    return E_NOINTERFACE;
}

ULONG ContextMenu::AddRef() { return static_cast<ULONG>(InterlockedIncrement(&refCount)); }

ULONG ContextMenu::Release() {
    const long value = InterlockedDecrement(&refCount);
    if (value == 0) {
        InterlockedDecrement(&g_moduleRefs);
        delete this;
    }
    return static_cast<ULONG>(value);
}

HRESULT ContextMenu::Initialize(LPCITEMIDLIST, IDataObject* dataObject, HKEY) {
    if (dataObject == nullptr) { return E_INVALIDARG; }
    FORMATETC format { CF_HDROP, nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
    STGMEDIUM medium {};
    if (FAILED(dataObject->GetData(&format, &medium))) { return E_FAIL; }
    auto* drop = static_cast<HDROP>(GlobalLock(medium.hGlobal));
    if (drop != nullptr) {
        wchar_t buffer[MAX_PATH] {};
        if (DragQueryFileW(drop, 0, buffer, MAX_PATH) > 0) {
            selectedPath = buffer;
        }
        GlobalUnlock(medium.hGlobal);
    }
    ReleaseStgMedium(&medium);
    return S_OK;
}

HRESULT ContextMenu::QueryContextMenu(HMENU menu, UINT indexMenu, UINT idCmdFirst, UINT idCmdLast, UINT flags) {
    if ((flags & CMF_DEFAULTONLY) != 0) { return MAKE_HRESULT(SEVERITY_SUCCESS, 0, 0); }
    if (idCmdFirst + 1 > idCmdLast) { return MAKE_HRESULT(SEVERITY_SUCCESS, 0, 0); }
    InsertMenuW(menu, indexMenu, MF_BYPOSITION, idCmdFirst, L"Analyze with Vaulkeeper");
    return MAKE_HRESULT(SEVERITY_SUCCESS, 0, 1);
}

HRESULT ContextMenu::InvokeCommand(CMINVOKECOMMANDINFO* info) {
    if (info == nullptr) { return E_INVALIDARG; }
    if (HIWORD(info->lpVerb) != 0) { return E_INVALIDARG; }

    std::wstring arguments = L"\"" + selectedPath + L"\"";
    SHELLEXECUTEINFOW exec { sizeof(SHELLEXECUTEINFOW) };
    exec.fMask = SEE_MASK_NOASYNC;
    exec.lpVerb = L"open";
    exec.lpFile = L"vaulkeeper.exe";
    exec.lpParameters = arguments.c_str();
    exec.nShow = SW_SHOWNORMAL;
    ShellExecuteExW(&exec);
    return S_OK;
}

HRESULT ContextMenu::GetCommandString(UINT_PTR, UINT type, UINT*, CHAR* name, UINT maxChars) {
    if (type == GCS_VERBA) {
        StringCchCopyA(name, maxChars, "vaulkeeper.analyze");
    }
    return S_OK;
}

}
