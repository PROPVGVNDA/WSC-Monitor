#include "NetworkManager.h"
#include "hooks.h"
#include "log.h"

namespace network {

    NetworkManager::NetworkManager() : hPipe(INVALID_HANDLE_VALUE) {}

    NetworkManager::~NetworkManager() {
        CloseConnection();
    }

    HANDLE NetworkManager::OpenConnection(const std::wstring& pipeName) {
        hPipe = Real_CreateFileW(pipeName.c_str(), GENERIC_WRITE | GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
        return hPipe;
    }

    void NetworkManager::CloseConnection() {
        if (hPipe != INVALID_HANDLE_VALUE) {
            CloseHandle(hPipe);
            hPipe = INVALID_HANDLE_VALUE;
        }
    }

    void NetworkManager::SendMessageToClient(const std::string& message) {
        std::lock_guard<std::mutex> guard(pipeMutex);
        DWORD bytesWritten;
        Log("Preparing to send: " + message);
        Real_WriteFile(hPipe, message.c_str(), static_cast<DWORD>(message.length()), &bytesWritten, NULL);
    }
    bool NetworkManager::IsValidHandle() const
    {
        return hPipe != INVALID_HANDLE_VALUE;
    }
    HANDLE& NetworkManager::GetHandle()
    {
        return hPipe;
    }
}
