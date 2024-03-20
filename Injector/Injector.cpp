#include "Injector.h"

#include <TlHelp32.h>
#include <Windows.h>

#include <fstream>
#include <iostream>

namespace injector
{
    bool fileExists(const std::string& filename)
    {
        std::ifstream file(filename);
        return file.good();
    }

    void printLastError(const std::string& message)
    {
        DWORD errorMessageID = ::GetLastError();
        LPSTR messageBuffer = nullptr;

        size_t size =
            FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

        std::cerr << message << ": " << messageBuffer << std::endl;
        LocalFree(messageBuffer);
    }

    bool injectDLL(DWORD pid, const std::string& dllPath)
    {
        if (pid == 0)
        {
            std::cerr << "Invalid PID. PID must be greater than 0.\n";
            return false;
        }

        if (!fileExists(dllPath))
        {
            std::cerr << "DLL not found at the specified path: " << dllPath << std::endl;
            return false;
        }

        HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
        if (hProcess == NULL)
        {
            printLastError("Failed to open process");
            return false;
        }

        LPVOID pDllPath = VirtualAllocEx(hProcess, 0, dllPath.length() + 1, MEM_COMMIT, PAGE_READWRITE);
        if (pDllPath == NULL)
        {
            printLastError("Failed to allocate memory in target process");
            CloseHandle(hProcess);
            return false;
        }

        if (!WriteProcessMemory(hProcess, pDllPath, (LPVOID)dllPath.c_str(), dllPath.length() + 1, NULL))
        {
            printLastError("Failed to write DLL path to target process memory");
            VirtualFreeEx(hProcess, pDllPath, 0, MEM_RELEASE);
            CloseHandle(hProcess);
            return false;
        }

        HANDLE hThread = CreateRemoteThread(
            hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandleA("Kernel32.dll"), "LoadLibraryA"),
            pDllPath, 0, NULL);
        if (hThread == NULL)
        {
            printLastError("Failed to create remote thread in target process");
            VirtualFreeEx(hProcess, pDllPath, 0, MEM_RELEASE);
            CloseHandle(hProcess);
            return false;
        }

        WaitForSingleObject(hThread, INFINITE);
        CloseHandle(hThread);
        VirtualFreeEx(hProcess, pDllPath, 0, MEM_RELEASE);
        CloseHandle(hProcess);

        return true;
    }
} // namespace injector
