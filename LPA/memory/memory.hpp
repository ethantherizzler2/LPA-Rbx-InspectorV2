#pragma once
#include <Windows.h>
#include <string>
#include <vector>
#include <optional>

namespace memory {

    class Process {
    private:
        HANDLE handle = nullptr;
        DWORD pid = 0;

    public:
        ~Process();

        bool find(const std::string& name);
        bool attach();
        void detach();

        HANDLE get() const;
        DWORD get_pid() const;
        bool valid() const;
    };

    extern Process proc;

    template<typename T>
    std::optional<T> read(uintptr_t address) {
        if (!proc.valid() || address < 0x10000)
            return std::nullopt;

        T value{};
        SIZE_T bytesRead = 0;

        if (!ReadProcessMemory(proc.get(), (LPCVOID)address, &value, sizeof(T), &bytesRead))
            return std::nullopt;

        if (bytesRead != sizeof(T))
            return std::nullopt;

        return value;
    }

    bool read_buffer(uintptr_t address, void* buffer, size_t size);

    std::string read_string(uintptr_t address, size_t max_len = 128);

    bool wait_for_window(const wchar_t* name = L"Roblox", int timeout_ms = 5000);

    uintptr_t get_base();
}