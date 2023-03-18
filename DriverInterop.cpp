#include "DriverInterop.h"
#include <Windows.h>
#include <string>
#include "Utils.h"

#define SIOCTL_TYPE 40000
#define IO_READ_REQUEST CTL_CODE(SIOCTL_TYPE, 0x903, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IO_WRITE_REQUEST CTL_CODE(SIOCTL_TYPE, 0x904, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IO_GET_BASE_ADDRESS_REQUEST CTL_CODE(SIOCTL_TYPE, 0x905, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IO_GET_MODULE_BASE_REQUEST CTL_CODE(SIOCTL_TYPE, 0x906, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IO_SET_TARGET_MODULE_REQUEST CTL_CODE(SIOCTL_TYPE, 0x907, METHOD_BUFFERED, FILE_ANY_ACCESS)

struct IOReadRequest {
    unsigned long ProcessId;
    intptr_t Address;
    SIZE_T Size;
    unsigned char* Value;
};

struct IOWriteRequest {
    unsigned long ProcessId;
    intptr_t Address;
    SIZE_T Size;
    unsigned char* Value;
};

struct IOGetBaseAddressRequest {
    DWORD ProcessId;
    intptr_t BaseAddress;
};

bool DriverInterop::ReadRaw(intptr_t address, void* data, unsigned long size) {
    IOReadRequest request{ this->pid, address, size, (unsigned char*)data };

    return DeviceIoControl(this->handle, IO_READ_REQUEST, &request, sizeof(IOReadRequest), &request, sizeof(IOReadRequest), nullptr, nullptr);
}

bool DriverInterop::WriteRaw(intptr_t address, void* data, unsigned long size) {
    IOWriteRequest request{ this->pid, address, size, (unsigned char*)data};

    return DeviceIoControl(this->handle, IO_WRITE_REQUEST, &request, sizeof(IOWriteRequest), &request, sizeof(IOWriteRequest), nullptr, nullptr);
}

bool DriverInterop::IsValid() {
    return this->handle;
}

bool DriverInterop::UpdateProcessId(const wchar_t* processName) {
    this->pid = GetProcessIdByName(processName);

    return this->pid != 0;
}

intptr_t DriverInterop::GetBaseAddress() {
    IOGetBaseAddressRequest request{ this->pid, 0 };

    return 
        DeviceIoControl(this->handle,
            IO_GET_BASE_ADDRESS_REQUEST,
            &request, sizeof(IOGetBaseAddressRequest),
            &request, sizeof(IOGetBaseAddressRequest),
            nullptr, nullptr
        ) ? (intptr_t)request.BaseAddress : 0;
}

intptr_t DriverInterop::GetModuleBase() {
    intptr_t request = 0;

    return DeviceIoControl(this->handle,
        IO_GET_MODULE_BASE_REQUEST,
        &request, sizeof(intptr_t),
        &request, sizeof(intptr_t),
        nullptr, nullptr
    ) ? request : 0;
}

bool DriverInterop::SetTargetModule(wchar_t* moduleName) {
    return DeviceIoControl(this->handle,
        IO_SET_TARGET_MODULE_REQUEST,
        (LPVOID)moduleName, DWORD(sizeof(wchar_t) * wcslen(moduleName)),
        nullptr, NULL,
        nullptr, nullptr
    );
}

DriverInterop::DriverInterop(wchar_t* name) : pid(0) {
    this->handle = CreateFileW(std::wstring(L"\\\\.\\").append(name).c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
}

DriverInterop::~DriverInterop() {
    if (this->handle) {
        CloseHandle(this->handle);
    }
}