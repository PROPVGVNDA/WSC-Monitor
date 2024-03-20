#include "PipeManager.h"
#include <exception>
#include <iostream>

PipeManager::PipeManager(const std::string& pipeName)
{
    std::string winPipeName = "\\\\.\\pipe\\" + pipeName;
    hPipe = CreateNamedPipeA(winPipeName.c_str(),
        PIPE_ACCESS_DUPLEX,
        PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
        1,
        1024 * 16,
        1024,
        NMPWAIT_USE_DEFAULT_WAIT,
        NULL);

    if (hPipe == INVALID_HANDLE_VALUE) {
        auto err = GetLastError();
        std::cerr << "Failed to create named pipe.\nError code: " + err << std::endl;
        throw std::exception("Failed to create named pipe");
    }
}

bool PipeManager::Connect()
{
    if (ConnectNamedPipe(hPipe, NULL)) return true;

    auto err = GetLastError();
    std::cerr << "Failed to connect named pipe.\nError code: " + err << std::endl;
    return false;
}

HANDLE& PipeManager::GetHandle()
{
    return hPipe;
}

PipeManager::~PipeManager()
{
    CloseHandle(hPipe);
}

