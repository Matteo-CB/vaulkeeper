#include "etw/etw_session.hpp"

#include <atomic>
#include <cstring>
#include <mutex>
#include <thread>
#include <vector>

#ifdef _WIN32
    #include <windows.h>
    #include <evntrace.h>
    #include <tdh.h>
    #pragma comment(lib, "tdh.lib")
    #pragma comment(lib, "advapi32.lib")
#endif

namespace vk::platform::windows {

namespace {

#ifdef _WIN32

struct EtwContext {
    EtwSession::Handler handler;
    std::atomic<bool> running { false };
};

std::mutex& contextMutex() {
    static std::mutex instance;
    return instance;
}

EtwContext*& currentContext() {
    static EtwContext* instance = nullptr;
    return instance;
}

VOID WINAPI eventRecordCallback(PEVENT_RECORD record) {
    if (record == nullptr || record->EventHeader.EventDescriptor.Opcode == 0) { return; }

    EtwContext* context = nullptr;
    {
        std::scoped_lock lock(contextMutex());
        context = currentContext();
    }
    if (context == nullptr || !context->running.load()) { return; }

    DiskIoEvent event {};
    event.processId = record->EventHeader.ProcessId;
    if (record->EventHeader.EventDescriptor.Id == 10) {
        event.bytesRead = record->EventHeader.EventDescriptor.Keyword;
    }
    if (context->handler) { context->handler(event); }
}

#endif

}

EtwSession::EtwSession(std::wstring sessionName) : name(std::move(sessionName)) {}

EtwSession::~EtwSession() {
    stop();
}

core::Result<void> EtwSession::start(Handler h) {
    if (running) { return core::fail(core::ErrorCode::AlreadyExists, "etw already running"); }
    handler = std::move(h);
    running = true;

#ifdef _WIN32
    auto* context = new EtwContext;
    context->handler = handler;
    context->running.store(true);
    {
        std::scoped_lock lock(contextMutex());
        currentContext() = context;
    }

    std::thread { [context] {
        EVENT_TRACE_LOGFILEW logfile {};
        logfile.LoggerName = const_cast<LPWSTR>(KERNEL_LOGGER_NAMEW);
        logfile.ProcessTraceMode = PROCESS_TRACE_MODE_REAL_TIME | PROCESS_TRACE_MODE_EVENT_RECORD;
        logfile.EventRecordCallback = eventRecordCallback;

        const auto session = OpenTraceW(&logfile);
        if (session != INVALID_PROCESSTRACE_HANDLE) {
            ProcessTrace(&session, 1, nullptr, nullptr);
            CloseTrace(session);
        }

        {
            std::scoped_lock lock(contextMutex());
            if (currentContext() == context) { currentContext() = nullptr; }
        }
        delete context;
    } }.detach();
#endif

    return core::ok();
}

void EtwSession::stop() {
    if (!running) { return; }
    running = false;
#ifdef _WIN32
    std::scoped_lock lock(contextMutex());
    if (currentContext() != nullptr) { currentContext()->running.store(false); }
#endif
}

}
