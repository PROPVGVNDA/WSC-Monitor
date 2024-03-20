#pragma once

#include <Windows.h>
#include <memoryapi.h>
#include <detours/detours.h>

// declare function pointers for storing original functions

extern "C" {
    extern HANDLE(WINAPI* Real_CreateFileW)(LPCWSTR a0, DWORD a1, DWORD a2,
        LPSECURITY_ATTRIBUTES a3, DWORD a4,
        DWORD a5, HANDLE a6);

    extern BOOL(WINAPI* Real_WriteFile)(HANDLE hFile, LPCVOID lpBuffer,
        DWORD nNumberOfBytesToWrite,
        LPDWORD lpNumberOfBytesWritten,
        LPOVERLAPPED lpOverlapped);

    extern BOOL(WINAPI* Real_ReadFile)(HANDLE hFile, LPVOID lpBuffer,
        DWORD nNumberOfBytesToRead,
        LPDWORD lpNumberOfBytesRead,
        LPOVERLAPPED lpOverlapped);

    extern LPVOID(WINAPI* Real_VirtualAlloc)(
        LPVOID lpAddress,
        SIZE_T dwSize,
        DWORD  flAllocationType,
        DWORD  flProtect);

    extern BOOL(WINAPI* Real_VirtualFree)(
        LPVOID lpAddress,
        SIZE_T dwSize,
        DWORD  dwFreeType
        );

    extern BOOL(WINAPI* Real_HeapFree)(
        HANDLE                 hHeap,
        DWORD                  dwFlags,
        LPVOID lpMem
        );

    extern LPVOID(WINAPI* Real_HeapAlloc)(
        HANDLE hHeap,
        DWORD  dwFlags,
        SIZE_T dwBytes
        );
}

// declare replacing function definitions

HANDLE WINAPI Mine_CreateFileW(LPCWSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE);
BOOL WINAPI Mine_WriteFile(HANDLE, LPCVOID, DWORD, LPDWORD, LPOVERLAPPED);
BOOL WINAPI Mine_ReadFile(HANDLE, LPVOID, DWORD, LPDWORD, LPOVERLAPPED);
LPVOID WINAPI Mine_VirtualAlloc(LPVOID, SIZE_T, DWORD, DWORD);
BOOL WINAPI Mine_VirtualFree(LPVOID, SIZE_T, DWORD);
BOOL WINAPI Mine_HeapFree(HANDLE, DWORD, LPVOID);
LPVOID WINAPI Mine_HeapAlloc(HANDLE, DWORD, SIZE_T);
