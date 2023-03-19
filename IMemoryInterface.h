#pragma once

#include <numeric>
#include <array>

__interface IMemoryInterface
{
public:
    bool IsValid();
    intptr_t GetBaseAddress();
    bool UpdateProcessId(const wchar_t* processName);
    bool ReadRaw(intptr_t address, void* pBuffer, unsigned long size);
    bool WriteRaw(intptr_t address, void* pBuffer, unsigned long size);
    intptr_t GetModuleBase();
    bool SetTargetModule(wchar_t* moduleName);
};

namespace Memory {
    template <typename T>
    bool Read(IMemoryInterface* pMemoryInterface, intptr_t address, T* data) {
        if (!address) {
            return false;
        }

        return pMemoryInterface->ReadRaw(address, data, sizeof(T));
    }

    template <typename T>
    T ReadValue(IMemoryInterface* pMemoryInterface, intptr_t address) {
        T t{};
        if (!address) {
            return t;
        }

        pMemoryInterface->ReadRaw(address, &t, sizeof(T));
        return t;
    }

    template <typename T>
    bool Write(IMemoryInterface* pMemoryInterface, intptr_t address, const T& data) {
        if (!address) {
            return false;
        }

        return pMemoryInterface->WriteRaw(address, (void*)&data, sizeof(T));
    }
}