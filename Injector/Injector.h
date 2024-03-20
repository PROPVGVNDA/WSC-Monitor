#pragma once

#pragma once

#include <Windows.h>

#include <string>

namespace injector {
	const LPCWSTR dllName = L"wsc-monitor.dll";

	bool fileExists(const std::string& filename);
	void printLastError(const std::string& message);
	bool injectDLL(DWORD pid, const std::string& dllPath);
}  // namespace injector

