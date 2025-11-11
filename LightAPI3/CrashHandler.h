#pragma once

#include <DbgHelp.h>
#pragma comment(lib, "dbghelp.lib")

//#include "MiniDumper.h"
namespace MiniDump {
    // https://gist.github.com/allenh1/38441719a2397b2be6176fa406e38d26
    template<typename Callback>
    struct ScopeExit
    {
        explicit ScopeExit(Callback p_func)
            : m_func(p_func) {
        }
        ~ScopeExit() { m_func(); }
    private:
        Callback m_func;
    };

    template<typename Callback>
    ScopeExit<Callback> make_scope_exit(Callback p_func)
    {
        return ScopeExit<Callback>(p_func);
    }

    inline auto load_write_minidump(HMODULE& lib)
    {
        /* TODO(hallen): gross */
        auto cast_version = reinterpret_cast<BOOL(*)(HANDLE, DWORD, HANDLE, MINIDUMP_TYPE, PMINIDUMP_EXCEPTION_INFORMATION, PMINIDUMP_USER_STREAM_INFORMATION, PMINIDUMP_CALLBACK_INFORMATION)>(::GetProcAddress(lib, "MiniDumpWriteDump"));
        return std::function<BOOL(HANDLE, DWORD, HANDLE, MINIDUMP_TYPE, PMINIDUMP_EXCEPTION_INFORMATION, PMINIDUMP_USER_STREAM_INFORMATION, PMINIDUMP_CALLBACK_INFORMATION)>(cast_version);
    }

    bool create_mini_dump(const LPCSTR& file_name) {
        HMODULE dbghelp_module = ::LoadLibrary(TEXT("dbghelp.dll"));
        HANDLE hFile =
            ::CreateFileA(file_name, GENERIC_READ | GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
        auto at_exit = make_scope_exit(
            [hFile, dbghelp_module]() {
                ::CloseHandle(hFile);
                ::FreeLibrary(dbghelp_module);
            });
        auto minidump_writer = load_write_minidump(dbghelp_module);
        if (nullptr == hFile || INVALID_HANDLE_VALUE == hFile) {
            std::cerr << "Failed to create MiniDump file!" << std::endl;
            return false;
        }
        // initialize the minidump
        MINIDUMP_TYPE type = MiniDumpNormal;
        BOOL ret = minidump_writer(::GetCurrentProcess(), ::GetCurrentProcessId(), hFile, type, nullptr, 0, 0);
        return ret;
    }
}

// https://github.com/MonkeyMethods/Dive/blob/de74d5a5c3f44c5f55cb7c951823bc0478f19845/Dive/SDK/Jobs/CrashHandler.h#L3
// Function to get the module name from an address
std::string GetModuleNameFromAddress(DWORD64 address) {
    IMAGEHLP_MODULE64 moduleInfo = { sizeof(IMAGEHLP_MODULE64) };
    HANDLE processHandle = GetCurrentProcess();

    if (SymGetModuleInfo64(processHandle, address, &moduleInfo)) {
        return moduleInfo.ModuleName;
    }
    else {
        return "Unknown Module";
    }
}

// Function to get symbol information from an address
std::string GetSymbolFromAddress(DWORD64 address) {
    HANDLE processHandle = GetCurrentProcess();
    DWORD64 displacement = 0;
    char buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)];
    PSYMBOL_INFO symbol = (PSYMBOL_INFO)buffer;

    symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
    symbol->MaxNameLen = MAX_SYM_NAME;

    if (SymFromAddr(processHandle, address, &displacement, symbol)) {
        std::stringstream ss;
        ss << symbol->Name << " [0x" << std::hex << symbol->Address << "]";
        return ss.str();
    }
    else {
        return "Unknown Symbol";
    }
}

// Function to initialize symbol handler
bool InitializeSymbolHandler() {
    HANDLE processHandle = GetCurrentProcess();
    if (!SymInitialize(processHandle, NULL, TRUE)) {
        std::cerr << "Failed to initialize symbol handler. Error: " << GetLastError() << std::endl;
        return false;
    }
    return true;
}

// Function to create crash dump
std::string CreateCrashDump(EXCEPTION_POINTERS* exceptionInfo) {
    auto dumpPath = GameConfig::getMinecraftFolderA() + "LightLoader-Crash.dmp";
    if (!MiniDump::create_mini_dump(dumpPath.c_str())) {
        std::cerr << "Failed to create mini dump." << std::endl;
    }
    return dumpPath;
}

// Function to build crash report
std::string BuildCrashReport(EXCEPTION_POINTERS* exceptionInfo) {
    std::stringstream errorDmp;
    errorDmp << "An unexpected error occurred and LightLoader needs to quit. We're sorry!\n"
        << "Here's some information about the crash:\n\n"
        << "Exception at address: " << std::hex << exceptionInfo->ExceptionRecord->ExceptionAddress << "\n"
        << "Exception code: " << std::hex << exceptionInfo->ExceptionRecord->ExceptionCode << "\n"
        << "Exception flags: " << std::hex << exceptionInfo->ExceptionRecord->ExceptionFlags << "\n";

    static bool init = false;
    if (!init) {
        if (InitializeSymbolHandler())
            init = true;
    }

    if (init) {
        std::string moduleName = GetModuleNameFromAddress((DWORD64)exceptionInfo->ExceptionRecord->ExceptionAddress);
        std::string symbolInfo = GetSymbolFromAddress((DWORD64)exceptionInfo->ExceptionRecord->ExceptionAddress);
        errorDmp << "Module: " << moduleName << ".dll/exe\n"
            << "Symbol: " << symbolInfo << "\n";

        if (moduleName == "Minecraft.Windows") {
            errorDmp << "\nSummary: Possibly an invalid VTable or function pointer was used.\n";
        }
        else if (moduleName == "winhttp") {
            errorDmp << "\nSummary: Possibly an issue inside of LightLoader's code\n";
        }
        // TODO: let the crashing mod handle this
        //else if (moduleName == "LightUt") {
        //    errorDmp << "\nSummary: Possibly an issue inside of LightLoader's code\n";
        //}
        else {
            errorDmp << "\nSummary: Possibly an error in a loaded mods code.\n"
                << "Please report this directly to them and explain what you were doing.\n";
        }
    }
    else {
        errorDmp << "Failed to initialize symbol handler. Detailed information not available.\n";
    }

    std::string dumpPath = CreateCrashDump(exceptionInfo);
    errorDmp << "\nCreated dump at:\n" << dumpPath << "\n\nPress OK to terminate the application (to avoid a zombie process).";

    return errorDmp.str();
}

namespace CrashHandler {
    static LONG WINAPI OnSehCrash(PEXCEPTION_POINTERS info) {
        std::print("SEH CRASH\n");
        std::string crashReport = BuildCrashReport(info);
        auto result = MessageBoxA(NULL, crashReport.c_str(), "LightLoader Crash", MB_RETRYCANCEL | MB_ICONERROR);

        if (result == 4) // RETRY
            return EXCEPTION_CONTINUE_EXECUTION;
    }

    static LONG CALLBACK OnVehCrash(PEXCEPTION_POINTERS info) {
        std::print("VEH CRASH\n");
        std::string crashReport = BuildCrashReport(info);
        MessageBoxA(NULL, crashReport.c_str(), "LightLoader Crash", MB_RETRYCANCEL | MB_ICONERROR);

        return EXCEPTION_CONTINUE_SEARCH;
    }

    static inline std::string BuildCppExceptionReport(const std::exception& e) {
        std::ostringstream oss;
        oss << "Uncaught C++ exception:\n"
            << "type: " << typeid(e).name() << "\n"
            << "what: " << e.what() << "\n";
        return oss.str();
    }

    static void OnTerminate() noexcept {
        std::print("CRASH (C++ Terminate)\n");

        std::exception_ptr eptr = std::current_exception();
        std::string report;

        if (eptr) {
            try { std::rethrow_exception(eptr); }
            catch (std::exception e) { report = BuildCppExceptionReport(e); }
            catch (...) { report = "Uncaught unknown C++ exception/\n"; }
        }
        else { report = "std::terminate called without an active exception. (??)\n"; }

        MessageBoxA(NULL, report.c_str(), "LightLoader Crash", MB_OK | MB_ICONERROR);
        std::_Exit(EXIT_FAILURE);
    }

    inline void InstallCrashHandler() {
        SetUnhandledExceptionFilter(CrashHandler::OnSehCrash);

        //AddVectoredExceptionHandler(1, OnVehCrash);

        std::set_terminate(OnTerminate);
    }
}
