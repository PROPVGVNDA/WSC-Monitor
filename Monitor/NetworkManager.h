#pragma once

#include <Windows.h>
#include <string>
#include <mutex>

namespace network {

    class NetworkManager {
    public:
        NetworkManager();
        ~NetworkManager();

        HANDLE OpenConnection(const std::wstring& pipeName);
        void CloseConnection();
        void SendMessageToClient(const std::string& message);
        bool IsValidHandle() const;
        HANDLE& GetHandle();
    private:
        HANDLE hPipe;
        std::mutex pipeMutex;
    };

}
