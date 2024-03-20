#include "DetourManager.h"
#include "hooks.h"

DetourManager::DetourManager() {
    // Initialize hooks
    AddHook(reinterpret_cast<PVOID*>(&Real_CreateFileW), Mine_CreateFileW, "CreateFileW");
    AddHook(reinterpret_cast<PVOID*>(&Real_WriteFile), Mine_WriteFile, "WriteFile");
    AddHook(reinterpret_cast<PVOID*>(&Real_ReadFile), Mine_ReadFile, "ReadFile");
    AddHook(reinterpret_cast<PVOID*>(&Real_VirtualAlloc), Mine_VirtualAlloc, "VirtualAlloc");
    AddHook(reinterpret_cast<PVOID*>(&Real_VirtualFree), Mine_VirtualFree, "VirtualFree");
    AddHook(reinterpret_cast<PVOID*>(&Real_HeapAlloc), Mine_HeapAlloc, "HeapAlloc");
    AddHook(reinterpret_cast<PVOID*>(&Real_HeapFree), Mine_HeapFree, "HeapFree");
}

DetourManager::~DetourManager() {
    Detach(); // Ensure detours are detached when the manager is destroyed
}

void DetourManager::AddHook(PVOID* realFunction, PVOID detourFunction, const std::string& functionName) {
    HookInfo hook{ realFunction, detourFunction, functionName };
    hooks[functionName] = hook;
}

void DetourManager::Attach() {
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());

    for (auto& pair : hooks) {
        AttachDetour(pair.second);
    }

    DetourTransactionCommit();
}

void DetourManager::Detach() {
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());

    for (auto& pair : hooks) {
        DetachDetour(pair.second);
    }

    DetourTransactionCommit();
}

void DetourManager::AttachDetour(HookInfo& hook) {
    DetourAttach(hook.realFunction, hook.detourFunction);
}

void DetourManager::DetachDetour(HookInfo& hook) {
    DetourDetach(hook.realFunction, hook.detourFunction);
}

