#include "class_factory.hpp"

#include "context_menu.hpp"

namespace vk::shellext {

extern long g_moduleRefs;

ClassFactory::ClassFactory() {
    InterlockedIncrement(&g_moduleRefs);
}

HRESULT ClassFactory::QueryInterface(REFIID riid, void** ppv) {
    if (ppv == nullptr) { return E_POINTER; }
    if (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IClassFactory)) {
        *ppv = static_cast<IClassFactory*>(this);
        AddRef();
        return S_OK;
    }
    *ppv = nullptr;
    return E_NOINTERFACE;
}

ULONG ClassFactory::AddRef() {
    return static_cast<ULONG>(InterlockedIncrement(&refCount));
}

ULONG ClassFactory::Release() {
    const long value = InterlockedDecrement(&refCount);
    if (value == 0) {
        InterlockedDecrement(&g_moduleRefs);
        delete this;
    }
    return static_cast<ULONG>(value);
}

HRESULT ClassFactory::CreateInstance(IUnknown* outer, REFIID riid, void** ppv) {
    if (outer != nullptr) { return CLASS_E_NOAGGREGATION; }
    auto* menu = new (std::nothrow) ContextMenu;
    if (menu == nullptr) { return E_OUTOFMEMORY; }
    const auto hr = menu->QueryInterface(riid, ppv);
    menu->Release();
    return hr;
}

HRESULT ClassFactory::LockServer(BOOL lock) {
    if (lock != 0) {
        InterlockedIncrement(&g_moduleRefs);
    } else {
        InterlockedDecrement(&g_moduleRefs);
    }
    return S_OK;
}

}
