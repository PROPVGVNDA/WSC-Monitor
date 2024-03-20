#include "MetricsManager.h"

metricCounter MetricsManager::BuildCounter(const std::string& name, const std::string& help)
{
    return prometheus::BuildCounter()
        .Name(name)
        .Help(help)
        .Register(*registry);
}

metricGauge MetricsManager::BuildGauge(const std::string& name, const std::string& help)
{
    return prometheus::BuildGauge()
        .Name(name)
        .Help(help)
        .Register(*registry);
}
