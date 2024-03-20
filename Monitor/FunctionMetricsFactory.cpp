#include "FunctionMetricsFactory.h"

std::unordered_map<std::string, std::shared_ptr<FunctionMetrics>> FunctionMetricsFactory::metricsMap;
std::mutex FunctionMetricsFactory::mapMutex;

std::shared_ptr<FunctionMetrics> FunctionMetricsFactory::CreateMetrics(const std::string& name) {
    std::lock_guard<std::mutex> lock(mapMutex);
    auto it = metricsMap.find(name);
    if (it != metricsMap.end()) {
        return it->second;
    }

    auto metrics = std::make_shared<FunctionMetrics>(name);
    metricsMap[name] = metrics;
    return metrics;
}

std::unordered_map<std::string, std::shared_ptr<FunctionMetrics>> FunctionMetricsFactory::GetMetricsMapCopy() {
    std::lock_guard<std::mutex> lock(mapMutex);
    return metricsMap;
}
