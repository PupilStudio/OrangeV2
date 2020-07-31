// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
#include "detours/include/detours.h"
#include <Windows.h>
#include <string>
#include <cstring>

extern "C" BOOL (WINAPI* beep)(DWORD dwFreq, DWORD dwDuration) = Beep;
extern "C" HMODULE (WINAPI * loadLibraryA)(LPCSTR lpLibFileName) = LoadLibraryA;
extern "C" HMODULE (WINAPI * loadLibraryW)(LPCWSTR lpLibFileNameW) = LoadLibraryW;

extern "C" VOID __declspec(dllexport) WINAPI PrintText(std::wstring s) {
    HANDLE hStdOutHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    WriteConsole(hStdOutHandle, s.c_str(), s.size(), NULL, NULL);
}
extern "C" BOOL __declspec(dllexport) WINAPI MyBeep(DWORD dwFreq, DWORD dwDuration) {
    PrintText(L"Don't Try To Beep!\n");
    ExitProcess(0xACACEE00);
    return true;
}
extern "C" HMODULE __declspec(dllexport) WINAPI MyLoadLibraryA(LPCSTR lpLibFileName) {
    PrintText(L"Don't Try To Load DLL!");
    ExitProcess(0xACACEE01);
    return 0;
}
extern "C" HMODULE _declspec(dllexport) WINAPI MyLoadLibraryW(LPCWSTR lpLibFileNameW) {
    PrintText(L"Don't Try To Load DLL!");
    ExitProcess(0xACACEE02);
    return 0;
}

extern "C" BOOL __declspec(dllexport) WINAPI OnDetourError() {
    HANDLE hStdOutHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    WriteConsole(hStdOutHandle, L"Detour Error!\n", 11, NULL, NULL);
    return true;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:        
        DisableThreadLibraryCalls(hModule);
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());

        DetourAttach(&(PVOID&)beep, MyBeep);
        DetourAttach(&(PVOID&)loadLibraryA, MyLoadLibraryA);
        DetourAttach(&(PVOID&)loadLibraryW, MyLoadLibraryW);

        if (DetourTransactionCommit() != NO_ERROR) {
            OnDetourError();
        }
        break;

    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

