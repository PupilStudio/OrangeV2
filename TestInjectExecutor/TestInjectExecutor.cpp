// TestInjectExecutor.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <Windows.h>
#include "detours/include/detours.h"

int main()
{
    std::cout << "Hello World!\n";

    STARTUPINFO si = { sizeof(si) };
    PROCESS_INFORMATION pi;
    TCHAR testInjectExe[] = TEXT("TestInject.exe");
    LPCSTR secJudge = "SecurityJudge.dll";
    bool success = DetourCreateProcessWithDllEx(NULL, testInjectExe, NULL, NULL, false, 0, NULL, NULL, &si, &pi, secJudge, NULL);
    if (success) {
        std::cout << "Detour Success" << std::endl;
        return 0;
    }
    else {
        std::cout << "Detour Failed" << std::endl;
        return -1;
    }
    
}
