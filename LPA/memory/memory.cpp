#include "memory.hpp"
#include <TlHelp32.h>
#include <Psapi.h>
#include <iostream>

namespace memory {

    Process proc;

    Process::~Process() {
        detach();
    }

    bool Process::find(const std::string& name) {
        PROCESSENTRY32 entry{};
        entry.dwSize = sizeof(entry);

        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (snapshot == INVALID_HANDLE_VALUE)
            return false;

        while (Process32Next(snapshot, &entry)) {
            if (!_stricmp(entry.szExeFile, name.c_str())) {
                pid = entry.th32ProcessID;
                CloseHandle(snapshot);
                return true;
            }
        }

        CloseHandle(snapshot);
        return false;
    }

    bool Process::attach() {
        if (!pid) return false;

        handle = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, FALSE, pid);
        return handle && handle != INVALID_HANDLE_VALUE;
    }

    void Process::detach() {
        if (handle && handle != INVALID_HANDLE_VALUE)
            CloseHandle(handle);

        handle = nullptr;
        pid = 0;
    }

    HANDLE Process::get() const { return handle; }
    DWORD Process::get_pid() const { return pid; }

    bool Process::valid() const {
        return handle && handle != INVALID_HANDLE_VALUE;
    }


    template std::optional<int> read<int>(uintptr_t);
    template std::optional<float> read<float>(uintptr_t);
    template std::optional<uintptr_t> read<uintptr_t>(uintptr_t);
    template std::optional<bool> read<bool>(uintptr_t);

    bool read_buffer(uintptr_t address, void* buffer, size_t size) {
        SIZE_T bytesRead;
        return ReadProcessMemory(proc.get(), (LPCVOID)address, buffer, size, &bytesRead)
            && bytesRead == size;
    }

    std::string read_string(uintptr_t address, size_t max_len) {
        std::vector<char> buffer(max_len);

        if (!read_buffer(address, buffer.data(), max_len))
            return "";

        return std::string(buffer.data());
    }

    bool wait_for_window(const wchar_t* name, int timeout_ms) {
        int elapsed = 0;

        while (elapsed < timeout_ms) {
            if (FindWindowW(nullptr, name)) {
                std::cout << "[+] Roblox Window found\n";
                return true;
            }

            Sleep(200);
            elapsed += 200;
        }

        return false;
    }

    uintptr_t get_base() {
        if (!proc.valid())
            return 0;

        HMODULE module = nullptr;
        DWORD needed;

        if (EnumProcessModules(proc.get(), &module, sizeof(module), &needed)) {
            return (uintptr_t)module;
        }

        return 0;
    }

}