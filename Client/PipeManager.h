#pragma once

#include <Windows.h>
#include <string>

class PipeManager
{
public:
	PipeManager(const std::string& pipeName);
	bool Connect();
	HANDLE& GetHandle();
	~PipeManager();
private:
	HANDLE hPipe;
};

