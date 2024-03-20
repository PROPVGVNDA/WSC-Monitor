#pragma once

#include <prometheus/counter.h>
#include <prometheus/gauge.h>
#include <prometheus/exposer.h>
#include <prometheus/registry.h>

#define metricCounter prometheus::Family<prometheus::Counter>&
#define metricGauge prometheus::Family<prometheus::Gauge>&

class MetricsManager {
public:
    std::shared_ptr<prometheus::Registry> registry;
    metricCounter functionCall;
    metricCounter functionError;
    metricGauge totalMemoryAllocated;
    metricGauge totalMemoryFreed;
    metricGauge bytesWritten;
    metricGauge bytesRead;
    metricGauge duration;

public:
    MetricsManager(std::shared_ptr<prometheus::Registry> reg)
        : registry(reg),
        functionCall(BuildCounter("call_count", "Number of function calls")),
        functionError(BuildCounter("error_count", "Number of function errors")),
        totalMemoryAllocated(BuildGauge("total_memory_allocated", "Amount of memory allocated")),
        totalMemoryFreed(BuildGauge("total_memory_freed", "Amount of memory freed")),
        bytesWritten(BuildGauge("bytes_written", "Total number of bytes written by function")),
        bytesRead(BuildGauge("bytes_read", "Total number of bytes read by function")),
        duration(BuildGauge("duration_ms", "Average duration of execution"))
    {}

private:
    metricCounter BuildCounter(const std::string& name, const std::string& help);
    metricGauge BuildGauge(const std::string& name, const std::string& help);
};
