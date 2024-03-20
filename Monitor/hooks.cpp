#include <chrono>
#include <mutex>

#include "hooks.h"
#include "FunctionMetrics.h"

// assign original Winapi functions to pointers

HANDLE(WINAPI* Real_CreateFileW)
(LPCWSTR a0, DWORD a1, DWORD a2, LPSECURITY_ATTRIBUTES a3, DWORD a4, DWORD a5,
    HANDLE a6) = CreateFileW;

BOOL(WINAPI* Real_WriteFile)
(HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite,
    LPDWORD lpNumberOfBytesWritten, LPOVERLAPPED lpOverlapped) = WriteFile;

BOOL(WINAPI* Real_ReadFile)
(HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead,
    LPDWORD lpNumberOfBytesRead, LPOVERLAPPED lpOverlapped) = ReadFile;

LPVOID(WINAPI* Real_VirtualAlloc)
(LPVOID lpAddress, SIZE_T dwSize, DWORD  flAllocationType, DWORD  flProtect
    ) = VirtualAlloc;

BOOL(WINAPI* Real_VirtualFree)(
    LPVOID lpAddress,
    SIZE_T dwSize,
    DWORD  dwFreeType
    ) = VirtualFree;

BOOL(WINAPI* Real_HeapFree)(
    HANDLE                 hHeap,
    DWORD                  dwFlags,
    LPVOID lpMem
    ) = HeapFree;

LPVOID(WINAPI* Real_HeapAlloc)(
    HANDLE hHeap,
    DWORD  dwFlags,
    SIZE_T dwBytes
    ) = HeapAlloc;


// define replacing functions

HANDLE WINAPI Mine_CreateFileW(LPCWSTR a0, DWORD a1, DWORD a2,
    LPSECURITY_ATTRIBUTES a3, DWORD a4, DWORD a5,
    HANDLE a6) {
    auto startTime = std::chrono::high_resolution_clock::now();
    HANDLE result = Real_CreateFileW(a0, a1, a2, a3, a4, a5, a6);
    auto endTime = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> duration = endTime - startTime;

    std::lock_guard<std::mutex> lock(createFileWp->mutex);

    createFileWp->callCount++;
    if (result == INVALID_HANDLE_VALUE) createFileWp->errorCount++;
    createFileWp->totalDuration += duration.count();

    return result;
}

BOOL WINAPI Mine_WriteFile(HANDLE hFile, LPCVOID lpBuffer,
    DWORD nNumberOfBytesToWrite,
    LPDWORD lpNumberOfBytesWritten,
    LPOVERLAPPED lpOverlapped) {
    auto startTime = std::chrono::high_resolution_clock::now();
    BOOL result = Real_WriteFile(hFile, lpBuffer, nNumberOfBytesToWrite,
        lpNumberOfBytesWritten, lpOverlapped);
    auto endTime = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> duration = endTime - startTime;

    std::lock_guard<std::mutex> lock(writeFilep->mutex);

    writeFilep->callCount++;
    if (result == FALSE) writeFilep->errorCount++;
    writeFilep->totalBytesWritten += nNumberOfBytesToWrite;
    writeFilep->totalDuration += duration.count();

    return result;
}

BOOL WINAPI Mine_ReadFile(HANDLE hFile, LPVOID lpBuffer,
    DWORD nNumberOfBytesToRead,
    LPDWORD lpNumberOfBytesRead,
    LPOVERLAPPED lpOverlapped) {
    auto startTime = std::chrono::high_resolution_clock::now();
    BOOL result = Real_ReadFile(hFile, lpBuffer, nNumberOfBytesToRead,
        lpNumberOfBytesRead, lpOverlapped);
    auto endTime = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> duration = endTime - startTime;

    std::lock_guard<std::mutex> lock(readFilep->mutex);

    readFilep->callCount++;
    if (result == FALSE) readFilep->errorCount++;
    readFilep->totalBytesRead += nNumberOfBytesToRead;
    readFilep->totalDuration += duration.count();

    return result;
}

LPVOID WINAPI Mine_VirtualAlloc(LPVOID lpAddress, SIZE_T dwSize, DWORD  flAllocationType, DWORD  flProtect)
{
    auto startTime = std::chrono::high_resolution_clock::now();
    LPVOID result = Real_VirtualAlloc(lpAddress, dwSize, flAllocationType, flProtect);
    auto endTime = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> duration = endTime - startTime;

    std::lock_guard<std::mutex> lock(virtualAllocp->mutex);

    virtualAllocp->callCount++;
    if (!result) virtualAllocp->errorCount++;
    virtualAllocp->totalMemoryAllocated += dwSize;
    virtualAllocp->totalDuration += duration.count();

    return result;
}

BOOL WINAPI Mine_VirtualFree(
    LPVOID lpAddress,
    SIZE_T dwSize,
    DWORD  dwFreeType
)
{
    auto startTime = std::chrono::high_resolution_clock::now();
    BOOL result = Real_VirtualFree(lpAddress, dwSize, dwFreeType);
    auto endTime = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> duration = endTime - startTime;

    std::lock_guard<std::mutex> lock(virtualFreep->mutex);

    virtualFreep->callCount++;
    if (!result) virtualFreep->errorCount++;
    virtualFreep->totalMemoryFreed += dwSize;
    virtualFreep->totalDuration += duration.count();

    return result;
}

BOOL WINAPI Mine_HeapFree(
    HANDLE                 hHeap,
    DWORD                  dwFlags,
    LPVOID lpMem
)
{
    auto heapSize = HeapSize(hHeap, dwFlags, lpMem);
    if (heapSize != ((SIZE_T)-1)) heapFreep->totalMemoryFreed += heapSize;

    auto startTime = std::chrono::high_resolution_clock::now();
    BOOL result = Real_HeapFree(hHeap, dwFlags, lpMem);
    auto endTime = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> duration = endTime - startTime;

    std::lock_guard<std::mutex> lock(heapFreep->mutex);

    heapFreep->callCount++;
    if (!result) heapFreep->errorCount++;
    heapFreep->totalDuration += duration.count();


    return result;
}

LPVOID WINAPI Mine_HeapAlloc(
    HANDLE hHeap,
    DWORD  dwFlags,
    SIZE_T dwBytes
)
{
    auto startTime = std::chrono::high_resolution_clock::now();
    LPVOID result = nullptr;

    try {
        result = Real_HeapAlloc(hHeap, dwFlags, dwBytes);
    }
    catch (...) {}

    auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration = endTime - startTime;

    std::lock_guard<std::mutex> lock(heapAllocp->mutex);

    heapAllocp->callCount++;
    if (!result) {
        heapAllocp->errorCount++;
    }
    if (result) {
        heapAllocp->totalMemoryAllocated += dwBytes;
    }
    heapAllocp->totalDuration += duration.count();

    return result;
}

