#pragma once

#include <windows.h>
#include <shlobj.h>
#include <string>

namespace vk::shellext {

class ContextMenu : public IShellExtInit, public IContextMenu {
public:
    ContextMenu();

    HRESULT __stdcall QueryInterface(REFIID riid, void** ppv) override;
    ULONG __stdcall AddRef() override;
    ULONG __stdcall Release() override;

    HRESULT __stdcall Initialize(LPCITEMIDLIST folder, IDataObject* dataObject, HKEY progId) override;

    HRESULT __stdcall QueryContextMenu(HMENU menu, UINT indexMenu, UINT idCmdFirst, UINT idCmdLast, UINT flags) override;
    HRESULT __stdcall InvokeCommand(CMINVOKECOMMANDINFO* info) override;
    HRESULT __stdcall GetCommandString(UINT_PTR idCmd, UINT type, UINT* reserved, CHAR* name, UINT maxChars) override;

private:
    long refCount { 1 };
    std::wstring selectedPath;
};

}
