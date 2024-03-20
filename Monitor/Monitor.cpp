#include <Windows.h>
#include <fstream>
#include <iostream>
#include <string>
#include "NetworkManager.h"

const LPCWSTR PIPE_NAME = L"\\\\.\\pipe\\MyPipe";
network::NetworkManager networkManager;

std::atomic<bool> continueSendingMetrics{ true };

const size_t maxRetryCount = 5;
const byte retryDelaySeconds = 5;

void MetricsSenderThread() {
    while (continueSendingMetrics) {
        std::this_thread::sleep_for(std::chrono::seconds(5));
        networkManager.SendMessageToClient("TEST MESSAGE");
    }
}

BOOL ThreadAttach(HMODULE hDll) {
    (void)hDll;
    return TRUE;
}

BOOL ThreadDetach(HMODULE hDll) {
    (void)hDll;
    return TRUE;
}

BOOL ProcessAttach(HMODULE hDll) {
    ThreadAttach(hDll);

    return TRUE;
}

BOOL ProcessDetach(HMODULE hDll) {
    ThreadDetach(hDll);

    return TRUE;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call,
    LPVOID lpReserved) {
    static std::thread metricsThread;

    (void)hModule;
    (void)lpReserved;

    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        for (int attempt = 0; attempt < maxRetryCount; ++attempt) {
            if (networkManager.OpenConnection(PIPE_NAME) != INVALID_HANDLE_VALUE) {
                break;
            }
            std::this_thread::sleep_for(std::chrono::seconds(retryDelaySeconds));
        }
        if (!networkManager.IsValidHandle()) {
            return FALSE;
        }
        metricsThread = std::thread(MetricsSenderThread);
        return ProcessAttach(hModule);
    case DLL_PROCESS_DETACH:
        continueSendingMetrics = false;
        if (metricsThread.joinable()) metricsThread.join();
        networkManager.CloseConnection();
        return ProcessDetach(hModule);
    case DLL_THREAD_ATTACH:
        return ThreadAttach(hModule);
    case DLL_THREAD_DETACH:
        return ThreadDetach(hModule);
    }
    return TRUE;
}
