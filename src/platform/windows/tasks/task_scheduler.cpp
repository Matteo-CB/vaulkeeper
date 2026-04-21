#include "tasks/task_scheduler.hpp"

#ifdef _WIN32
    #define _WIN32_DCOM
    #include <comdef.h>
    #include <taskschd.h>
    #include <windows.h>
    #pragma comment(lib, "taskschd.lib")
#endif

namespace vk::platform::windows {

namespace {

#ifdef _WIN32

std::string narrow(const BSTR value) {
    if (value == nullptr) { return {}; }
    const auto len = SysStringLen(value);
    if (len == 0) { return {}; }
    const auto needed = WideCharToMultiByte(CP_UTF8, 0, value, static_cast<int>(len), nullptr, 0, nullptr, nullptr);
    std::string result(static_cast<std::size_t>(needed), '\0');
    WideCharToMultiByte(CP_UTF8, 0, value, static_cast<int>(len), result.data(), needed, nullptr, nullptr);
    return result;
}

void walk(std::vector<ScheduledTaskDescriptor>& out, ITaskFolder* folder) {
    if (folder == nullptr) { return; }

    IRegisteredTaskCollection* collection = nullptr;
    if (SUCCEEDED(folder->GetTasks(TASK_ENUM_HIDDEN, &collection)) && collection != nullptr) {
        LONG count = 0;
        collection->get_Count(&count);
        for (LONG i = 1; i <= count; ++i) {
            IRegisteredTask* task = nullptr;
            if (FAILED(collection->get_Item(_variant_t(i), &task)) || task == nullptr) { continue; }

            ScheduledTaskDescriptor desc;
            BSTR name = nullptr;
            if (SUCCEEDED(task->get_Name(&name)) && name != nullptr) { desc.name = narrow(name); SysFreeString(name); }
            BSTR path = nullptr;
            if (SUCCEEDED(task->get_Path(&path)) && path != nullptr) { desc.path = narrow(path); SysFreeString(path); }

            VARIANT_BOOL enabled = VARIANT_FALSE;
            task->get_Enabled(&enabled);
            desc.enabled = enabled == VARIANT_TRUE;

            ITaskDefinition* definition = nullptr;
            if (SUCCEEDED(task->get_Definition(&definition)) && definition != nullptr) {
                IRegistrationInfo* regInfo = nullptr;
                if (SUCCEEDED(definition->get_RegistrationInfo(&regInfo)) && regInfo != nullptr) {
                    BSTR author = nullptr;
                    if (SUCCEEDED(regInfo->get_Author(&author)) && author != nullptr) { desc.author = narrow(author); SysFreeString(author); }
                    BSTR description = nullptr;
                    if (SUCCEEDED(regInfo->get_Description(&description)) && description != nullptr) { desc.description = narrow(description); SysFreeString(description); }
                    regInfo->Release();
                }

                IActionCollection* actions = nullptr;
                if (SUCCEEDED(definition->get_Actions(&actions)) && actions != nullptr) {
                    LONG actionCount = 0;
                    actions->get_Count(&actionCount);
                    if (actionCount > 0) {
                        IAction* action = nullptr;
                        if (SUCCEEDED(actions->get_Item(1, &action)) && action != nullptr) {
                            IExecAction* exec = nullptr;
                            if (SUCCEEDED(action->QueryInterface(IID_IExecAction, reinterpret_cast<void**>(&exec))) && exec != nullptr) {
                                BSTR program = nullptr;
                                if (SUCCEEDED(exec->get_Path(&program)) && program != nullptr) {
                                    desc.action = narrow(program);
                                    SysFreeString(program);
                                }
                                exec->Release();
                            }
                            action->Release();
                        }
                    }
                    actions->Release();
                }

                ITriggerCollection* triggers = nullptr;
                if (SUCCEEDED(definition->get_Triggers(&triggers)) && triggers != nullptr) {
                    LONG triggerCount = 0;
                    triggers->get_Count(&triggerCount);
                    desc.triggerSummary = triggerCount > 0 ? "configured" : "none";
                    triggers->Release();
                }

                definition->Release();
            }

            out.push_back(std::move(desc));
            task->Release();
        }
        collection->Release();
    }

    ITaskFolderCollection* children = nullptr;
    if (SUCCEEDED(folder->GetFolders(0, &children)) && children != nullptr) {
        LONG childCount = 0;
        children->get_Count(&childCount);
        for (LONG i = 1; i <= childCount; ++i) {
            ITaskFolder* child = nullptr;
            if (SUCCEEDED(children->get_Item(_variant_t(i), &child)) && child != nullptr) {
                walk(out, child);
                child->Release();
            }
        }
        children->Release();
    }
}

#endif

}

core::Result<std::vector<ScheduledTaskDescriptor>> enumerateScheduledTasks() {
    std::vector<ScheduledTaskDescriptor> out;
#ifdef _WIN32
    const auto initResult = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    const bool initHere = SUCCEEDED(initResult);

    if (FAILED(CoInitializeSecurity(nullptr, -1, nullptr, nullptr,
                                    RPC_C_AUTHN_LEVEL_PKT_PRIVACY, RPC_C_IMP_LEVEL_IMPERSONATE,
                                    nullptr, 0, nullptr)) && GetLastError() != RPC_E_TOO_LATE) {
        if (initHere) { CoUninitialize(); }
        return out;
    }

    ITaskService* service = nullptr;
    if (FAILED(CoCreateInstance(CLSID_TaskScheduler, nullptr, CLSCTX_INPROC_SERVER,
                                IID_ITaskService, reinterpret_cast<void**>(&service))) || service == nullptr) {
        if (initHere) { CoUninitialize(); }
        return out;
    }
    if (FAILED(service->Connect(_variant_t(), _variant_t(), _variant_t(), _variant_t()))) {
        service->Release();
        if (initHere) { CoUninitialize(); }
        return out;
    }

    ITaskFolder* rootFolder = nullptr;
    if (SUCCEEDED(service->GetFolder(_bstr_t(L"\\"), &rootFolder)) && rootFolder != nullptr) {
        walk(out, rootFolder);
        rootFolder->Release();
    }

    service->Release();
    if (initHere) { CoUninitialize(); }
#endif
    return out;
}

}
