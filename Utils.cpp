#pragma once

#include <Windows.h>
#include <TlHelp32.h>

unsigned long GetProcessIdByName(const wchar_t* ProcName) {
    PROCESSENTRY32 pe32{ sizeof(PROCESSENTRY32) };
    HANDLE hSnapshot = NULL;
    unsigned long result = 0;
    hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (Process32First(hSnapshot, &pe32)) {
        do {
            if (wcscmp(ProcName, pe32.szExeFile) == 0) {
                result = pe32.th32ProcessID;
                break;
            }
        } while (Process32Next(hSnapshot, &pe32));
    }

    CloseHandle(hSnapshot);

    return result;
}
