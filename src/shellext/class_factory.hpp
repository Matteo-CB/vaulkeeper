#pragma once

#include <windows.h>
#include <shlobj.h>

namespace vk::shellext {

class ClassFactory : public IClassFactory {
public:
    ClassFactory();

    HRESULT __stdcall QueryInterface(REFIID riid, void** ppv) override;
    ULONG __stdcall AddRef() override;
    ULONG __stdcall Release() override;

    HRESULT __stdcall CreateInstance(IUnknown* outer, REFIID riid, void** ppv) override;
    HRESULT __stdcall LockServer(BOOL lock) override;

private:
    long refCount { 1 };
};

}
