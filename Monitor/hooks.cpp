#include <chrono>
#include <mutex>

#include "hooks.h"

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

// define replacing functions

HANDLE WINAPI Mine_CreateFileW(LPCWSTR a0, DWORD a1, DWORD a2,
    LPSECURITY_ATTRIBUTES a3, DWORD a4, DWORD a5,
    HANDLE a6) {

    HANDLE result = Real_CreateFileW(a0, a1, a2, a3, a4, a5, a6);
    return result;
}

BOOL WINAPI Mine_WriteFile(HANDLE hFile, LPCVOID lpBuffer,
    DWORD nNumberOfBytesToWrite,
    LPDWORD lpNumberOfBytesWritten,
    LPOVERLAPPED lpOverlapped) {

    BOOL result = Real_WriteFile(hFile, lpBuffer, nNumberOfBytesToWrite,
        lpNumberOfBytesWritten, lpOverlapped);
    return result;
}

BOOL WINAPI Mine_ReadFile(HANDLE hFile, LPVOID lpBuffer,
    DWORD nNumberOfBytesToRead,
    LPDWORD lpNumberOfBytesRead,
    LPOVERLAPPED lpOverlapped) {

    BOOL result = Real_ReadFile(hFile, lpBuffer, nNumberOfBytesToRead,
        lpNumberOfBytesRead, lpOverlapped);
    return result;
}
