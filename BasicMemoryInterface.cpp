#include "BasicMemoryInterface.h"
#include <Windows.h>
#include <Psapi.h>
#include "Utils.h"

BasicMemoryInterface::~BasicMemoryInterface() {
    if (this->handle) {
        CloseHandle(this->handle);
    }
}

bool BasicMemoryInterface::IsValid() {
    return this->handle;
}

bool BasicMemoryInterface::UpdateProcessId(const wchar_t* processName) {
    this->pid = GetProcessIdByName(processName);
    if (this->handle) {
        CloseHandle(this->handle);
        this->handle = nullptr;
    }

    if (this->pid) {
        this->handle = OpenProcess(PROCESS_ALL_ACCESS, false, this->pid);
    }

    return this->pid && this->handle;
}

intptr_t BasicMemoryInterface::GetBaseAddress() {
    if (!this->pid || !this->handle) {
        return false;
    }

    DWORD_PTR baseAddress = 0;
    HMODULE* moduleArray = nullptr;
    LPBYTE moduleArrayBytes = nullptr;
    DWORD bytesRequired = 0;
    if (EnumProcessModules(this->handle, NULL, 0, &bytesRequired)) {
        if (bytesRequired) {
            moduleArrayBytes = (LPBYTE)LocalAlloc(LPTR, bytesRequired);
            if (moduleArrayBytes) {
                unsigned int moduleCount = 0;
                moduleCount = bytesRequired / sizeof(HMODULE);
                moduleArray = (HMODULE*)moduleArrayBytes;
                if (EnumProcessModules(this->handle, moduleArray, bytesRequired, &bytesRequired)) {
                    baseAddress = (DWORD_PTR)moduleArray[0];
                }

                LocalFree(moduleArrayBytes);
            }
        }
    }

    return baseAddress;
}

intptr_t BasicMemoryInterface::GetModuleBase() {
    return 0x7FFECC920000;
}

bool BasicMemoryInterface::SetTargetModule(wchar_t* moduleName) {
    throw("DRIVER USE ONLY!");
}

bool BasicMemoryInterface::ReadRaw(intptr_t address, void* pBuffer, unsigned long size) {
    if (!this->pid || !this->handle) {
        return false;
    }

    return ReadProcessMemory(this->handle, (LPCVOID)address, pBuffer, size, nullptr);
}

bool BasicMemoryInterface::WriteRaw(intptr_t address, void* pBuffer, unsigned long size) {
    if (!this->pid || !this->handle) {
        return false;
    }

    return WriteProcessMemory(this->handle, (LPVOID)address, pBuffer, size, nullptr);
}
