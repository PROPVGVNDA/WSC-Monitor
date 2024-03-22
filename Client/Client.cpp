#include <iostream>
#include <Windows.h>
#include <prometheus/counter.h>
#include <prometheus/exposer.h>
#include <prometheus/registry.h>
#include <nlohmann/json.hpp>
#include "Injector/Injector.h"
#include "MetricsManager.h"
#include "PipeManager.h"

const std::string dllName = "Monitor.dll";
std::string dllPath = {};

bool findDLL()
{
    DWORD bufferSize = MAX_PATH;
    char buffer[MAX_PATH];

    GetCurrentDirectoryA(bufferSize, buffer);

    DWORD fileAttr = GetFileAttributesA(dllName.c_str());
    if (fileAttr == INVALID_FILE_ATTRIBUTES) {
        MessageBoxA(NULL, "Monitor.dll file could not be found", "Monitor", MB_ICONERROR | MB_OK);
        return false;
    }

    dllPath = buffer + std::string("\\" + dllName);
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

    _dupenv_s(&env_val, &size, "MONITOR_METRICS_DEBUG");

    bool monitor_metrics_debug = false;

    if (env_val != nullptr) {
        if (strcmp(env_val, "TRUE") == 0) {
            monitor_metrics_debug = true;
        }
        else if (strcmp(env_val, "FALSE") == 0) {
            monitor_metrics_debug = false;
        }
        else {
            std::cout << "INFO: Environment variable MONITOR_METRICS_DEBUG is not valid. Defaulting to FALSE\n";
        }
        free(env_val);
    }
    else {
        std::cout << "INFO: Environment variable MONITOR_METRICS_DEBUG is not set. Defaulting to FALSE\n";
    }

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

    prometheus::Exposer exposer{ "127.0.0.1:8080" };
    auto registry = std::make_shared<prometheus::Registry>();
    MetricsManager metricsManager = { registry };
    exposer.RegisterCollectable(metricsManager.registry);

    char buffer[4096];
    DWORD bytesRead;

    PipeManager pipeManager{ "MyPipe" };
    std::cout << "Waiting for DLL connection..." << std::endl;
    if (!pipeManager.Connect()) {
        return 1;
    }

    if (injectionThread.joinable()) {
        injectionThread.join();
    }

    while (ReadFile(pipeManager.GetHandle(), buffer, sizeof(buffer) - 1, &bytesRead, NULL) != FALSE) {
        buffer[bytesRead] = '\0';
        if (monitor_metrics_debug)
            std::cout << "Received: " << buffer << std::endl;

        auto jsonObject = nlohmann::json::parse(buffer);
        auto jsonArray = jsonObject["data"];

        for (auto& json : jsonArray) {
            std::string functionName = json["functionName"];
            auto callCount = json["callCount"].get<unsigned long long>();
            auto errorCount = json["errorCount"].get<unsigned long long>();
            auto totalMemoryAllocated = json["totalMemoryAllocated"].get<unsigned long long>();
            auto totalMemoryFreed = json["totalMemoryFreed"].get<unsigned long long>();
            auto totalBytesWritten = json["totalBytesWritten"].get<unsigned long long>();
            auto totalBytesRead = json["totalBytesRead"].get<unsigned long long>();
            auto totalDuration = json["totalDuration"].get<double>();

            auto currentCallCount = metricsManager.functionCall.Add({ {"function_name", functionName} }).Collect().counter.value;
            if (currentCallCount < callCount)
                metricsManager.functionCall.Add({ {"function_name", functionName} }).Increment(callCount - currentCallCount);
            auto currentErrorCount = metricsManager.functionError.Add({ {"function_name", functionName} }).Collect().counter.value;
            if (currentErrorCount < errorCount)
                metricsManager.functionError.Add({ {"function_name", functionName} }).Increment(errorCount - currentErrorCount);

            if (totalMemoryAllocated != 0)
                metricsManager.totalMemoryAllocated.Add({ {"function_name", functionName} }).Set((double)totalMemoryAllocated);
            if (totalMemoryFreed != 0)
                metricsManager.totalMemoryFreed.Add({ {"function_name", functionName} }).Set((double)totalMemoryFreed);
            if (totalBytesWritten != 0)
                metricsManager.bytesWritten.Add({ {"function_name", functionName} }).Set((double)totalBytesWritten);
            if (totalBytesRead != 0)
                metricsManager.bytesRead.Add({ {"function_name", functionName} }).Set((double)totalBytesRead);
            if (totalDuration != 0)
                metricsManager.duration.Add({ {"function_name", functionName} }).Set(totalDuration / callCount);
        }
    }

    return 0;
}
