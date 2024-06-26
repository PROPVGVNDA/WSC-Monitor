#include <nlohmann/json.hpp>
#include "FunctionMetrics.h"
#include "FunctionMetricsFactory.h"

std::unordered_map<std::string, metrics_ptr> metricsMap = {};
std::mutex metricsMapMutex = {};  // Mutex for thread-safe access

metrics_ptr createFileWp;
metrics_ptr writeFilep;
metrics_ptr readFilep;
metrics_ptr virtualAllocp;
metrics_ptr virtualFreep;
metrics_ptr heapAllocp;
metrics_ptr heapFreep;

std::string FunctionMetrics::Serialize() const {
    nlohmann::json j;
    j["functionName"] = functionName;
    j["callCount"] = callCount;
    j["errorCount"] = errorCount;
    j["totalBytesWritten"] = totalBytesWritten;
    j["totalBytesRead"] = totalBytesRead;
    j["totalDuration"] = totalDuration;
    j["totalMemoryAllocated"] = totalMemoryAllocated;
    j["totalMemoryFreed"] = totalMemoryFreed;

    return j.dump();
}

void FunctionMetrics::InitializeMetrics() {
    createFileWp = FunctionMetricsFactory::CreateMetrics("CreateFileW");
    writeFilep = FunctionMetricsFactory::CreateMetrics("WriteFile");
    readFilep = FunctionMetricsFactory::CreateMetrics("ReadFile");
    virtualAllocp = FunctionMetricsFactory::CreateMetrics("VirtualAlloc");
    virtualFreep = FunctionMetricsFactory::CreateMetrics("VirtualFree");
    heapAllocp = FunctionMetricsFactory::CreateMetrics("HeapAlloc");
    heapFreep = FunctionMetricsFactory::CreateMetrics("HeapFree");
}
