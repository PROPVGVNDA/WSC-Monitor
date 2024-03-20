#pragma once

#include <Windows.h>
#include <detours/detours.h>
#include <string>
#include <unordered_map>

class DetourManager {
public:
    DetourManager();
    ~DetourManager();

    void Attach();
    void Detach();

private:
    struct HookInfo {
        PVOID* realFunction;
        PVOID detourFunction;
        std::string functionName;
    };

    std::unordered_map<std::string, HookInfo> hooks;
    void AddHook(PVOID* realFunction, PVOID detourFunction, const std::string& functionName);
    void AttachDetour(HookInfo& hook);
    void DetachDetour(HookInfo& hook);
};
