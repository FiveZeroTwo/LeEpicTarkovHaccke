#pragma once

#include "IMemoryInterface.h"
#include <numeric>

class DriverInterop : public IMemoryInterface {
public:
    DriverInterop(wchar_t* name);
    DriverInterop(const DriverInterop& interop) = delete;
    ~DriverInterop();

    bool ReadRaw(intptr_t address, void* pBuffer, unsigned long size);
    bool WriteRaw(intptr_t address, void* pBuffer, unsigned long size);

    bool IsValid();
    bool UpdateProcessId(const wchar_t* processName);
    intptr_t GetBaseAddress();
    intptr_t GetModuleBase();
    bool SetTargetModule(wchar_t* moduleName);

private:
    void* handle;
    unsigned long pid;
};