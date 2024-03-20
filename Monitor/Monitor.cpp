#include <Windows.h>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>

#include "FunctionMetricsFactory.h"
#include "DetourManager.h"
#include "NetworkManager.h"

const LPCWSTR PIPE_NAME = L"\\\\.\\pipe\\MyPipe";
network::NetworkManager networkManager;
DetourManager detourManager;

std::atomic<bool> continueSendingMetrics{ true };

const size_t maxRetryCount = 5;
const byte retryDelaySeconds = 5;

void MetricsSenderThread() {
    while (continueSendingMetrics) {
        std::this_thread::sleep_for(std::chrono::seconds(5));

        nlohmann::json jsonArray;

        auto metricsCopy = FunctionMetricsFactory::GetMetricsMapCopy();

        for (const auto& pair : metricsCopy) {
            std::string serializedData = pair.second->Serialize();
            jsonArray.push_back(nlohmann::json::parse(serializedData));
        }

        nlohmann::json jsonObject;
        jsonObject["data"] = jsonArray;
        networkManager.SendMessageToClient(jsonObject.dump());
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
    detourManager.Attach();

    ThreadAttach(hDll);

    return TRUE;
}

BOOL ProcessDetach(HMODULE hDll) {
    ThreadDetach(hDll);

    detourManager.Detach();

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
        FunctionMetrics::InitializeMetrics();
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
