#include <iostream>
#include <thread>
#include <Windows.h>
#include "Injector/Injector.h"
#include "PipeManager.h"

std::string dllPath = {};

bool findDLL()
{
    DWORD bufferSize = MAX_PATH;
    char buffer[MAX_PATH];

    GetCurrentDirectoryA(bufferSize, buffer);

    DWORD fileAttr = GetFileAttributesA("dll.dll");
    if (fileAttr == INVALID_FILE_ATTRIBUTES) {
        MessageBoxA(NULL, "Monitor.dll file could not be found", "Monitor", MB_ICONERROR | MB_OK);
        return false;
    }

    dllPath = buffer + std::string("\\dll.dll");
    return true;
}

void InjectDLL(DWORD pid) {
    if (injector::injectDLL(pid, dllPath)) {
        std::cout << "DLL injection successful." << std::endl;
    }
    else {
        std::cerr << "Failed to inject DLL." << std::endl;
    }
}

int main() {
    char* env_val = nullptr;
    size_t size = 0;

    if (!findDLL()) return 1;
    DWORD pid;

    std::cout << "Enter the PID of the target process: ";
    if (!(std::cin >> pid)) {
        std::cerr << "Invalid input. Please enter a numerical PID.\n";
        return 1;
    }

    if (pid == 0) {
        std::cerr << "Invalid PID. PID must be greater than 0.\n";
        return 1;
    }

    std::thread injectionThread(InjectDLL, pid);

    PipeManager pipeManager{ "MyPipe" };
    std::cout << "Waiting for DLL connection..." << std::endl;
    if (!pipeManager.Connect()) {
        return 1;
    }

    if (injectionThread.joinable()) {
        injectionThread.join();
    }

    return 0;
}
