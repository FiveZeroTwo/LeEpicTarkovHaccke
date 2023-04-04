#pragma once

#include "IMemoryInterface.h"

class BasicMemoryInterface : public IMemoryInterface
{
public:
    BasicMemoryInterface() : handle(nullptr), pid(0) {
    }

    BasicMemoryInterface(const BasicMemoryInterface& basicMemoryInterface) = delete;
    ~BasicMemoryInterface();

    bool IsValid();
    bool UpdateProcessId(const wchar_t* processName);
    intptr_t GetBaseAddress();
    bool ReadRaw(intptr_t address, void* pBuffer, unsigned long size);
    bool WriteRaw(intptr_t address, void* pBuffer, unsigned long size);
    intptr_t GetModuleBase();
    bool SetTargetModule(wchar_t* moduleName);
private:
    void* handle;
    unsigned long pid;
};

