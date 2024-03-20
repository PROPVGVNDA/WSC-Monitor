#pragma once

#include <Windows.h>
#include <mutex>
#include <string>
#include <unordered_map>

#define metrics_ptr std::shared_ptr<FunctionMetrics>
#define make_metrics_ptr std::make_shared<FunctionMetrics>

class FunctionMetrics {
public:
    FunctionMetrics(const std::string& name)
        : functionName(name), callCount(0), errorCount(0),
        totalBytesWritten(0), totalBytesRead(0), totalDuration(0.0),
        totalMemoryAllocated(0), totalMemoryFreed(0) {}

    static void InitializeMetrics();
public:
    std::string functionName;
    unsigned long long callCount;
    unsigned long long errorCount;
    unsigned long long totalBytesWritten;
    unsigned long long totalBytesRead;
    unsigned long long totalMemoryAllocated;
    unsigned long long totalMemoryFreed;
    double totalDuration;
    std::mutex mutex;
};

extern metrics_ptr createFileWp;
extern metrics_ptr writeFilep;
extern metrics_ptr readFilep;
extern metrics_ptr virtualAllocp;
extern metrics_ptr virtualFreep;
extern metrics_ptr heapAllocp;
extern metrics_ptr heapFreep;
