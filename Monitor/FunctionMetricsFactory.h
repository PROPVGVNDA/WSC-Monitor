#pragma once

#include "FunctionMetrics.h"

class FunctionMetricsFactory {
public:
    static std::shared_ptr<FunctionMetrics> CreateMetrics(const std::string& name);
    static std::unordered_map<std::string, std::shared_ptr<FunctionMetrics>> GetMetricsMapCopy();
private:
    static std::unordered_map<std::string, std::shared_ptr<FunctionMetrics>> metricsMap;
    static std::mutex mapMutex;

};
