// JudgeSandbox.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <Windows.h>
#include <string>
#include <exception>
#include <Psapi.h>
#include "detours/include/detours.h"

using std::wstring;
using std::string;

const UINT JUDGE_RESULT_NORMAL = 0x0;
const UINT JUDGE_RESULT_RE = 0xEE01ACAC;
const UINT JUDGE_RESULT_MLE = 0xEE02ACAC;
const UINT JUDGE_RESULT_TLE = 0xEE03ACAC;

int parseInt(wstring s) {
    int x = 0;
    for (int i = 0; i != s.size() - 1; ++i)
        x = 10 * x + s[i] - '0';
    return x;
}

void PrintUsage() {
    std::cout << "Usage: JudgeSandbox <ExePath> <TimeLimit:MilliumSecond> <MemoryLimit:KB> <JudgeType:STDIO>" << std::endl;
    ExitProcess(0xEE01ACAC);
}

int JudgeStdIO(wstring exePath, int timeLimit, int memLimit, wstring inputFile, wstring outputFile) {
    PROCESS_INFORMATION pi;
    STARTUPINFO si = { sizeof(STARTUPINFO) };
    SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES),NULL,TRUE };

    HANDLE hOutputFile = CreateFile(outputFile.c_str(), GENERIC_ALL,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        &sa, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    HANDLE hInputFile = CreateFile(inputFile.c_str(), GENERIC_ALL,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        &sa, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    si.hStdInput = hInputFile;
    si.hStdOutput = hOutputFile;
    si.dwFlags = STARTF_USESTDHANDLES;

    bool createProcessSuccess = 
        DetourCreateProcessWithDllEx(NULL, (LPWSTR)(exePath.c_str()), NULL, NULL, TRUE,
            0, NULL, NULL, &si, &pi, "SecurityJudge.dll", NULL);
    DWORD startTime = GetTickCount();

    bool stoped = false;
    PROCESS_MEMORY_COUNTERS memCounter;
    memCounter.cb = sizeof(PROCESS_MEMORY_COUNTERS);

    while (GetTickCount() <= startTime + timeLimit) {
        if (WaitForSingleObject(pi.hProcess, 0) == WAIT_OBJECT_0) {
            stoped = true;
            break;
        }
        GetProcessMemoryInfo(pi.hProcess, &memCounter, sizeof(PROCESS_MEMORY_COUNTERS));

        if (memCounter.PeakWorkingSetSize / 1024 > memLimit) {
            TerminateProcess(pi.hProcess, 0xACACE000);
            return JUDGE_RESULT_MLE;
        }
    }

    if (!stoped) {
        TerminateProcess(pi.hProcess, 0xACACE001);
        return JUDGE_RESULT_TLE;
    }
    return JUDGE_RESULT_NORMAL;
}

int main(int argc, wchar_t** argv)
{
    wstring exeFilePath, type;
    int timeLimit, memLimit; // ms, kb
    wstring inFile, outFile;

    try {
        if (argc < 8)
            throw std::exception();
        exeFilePath = argv[1];
        timeLimit = parseInt(argv[2]), memLimit = parseInt(argv[3]);
        type = argv[4];
        if (type != L"stdio")
            throw std::exception();
        inFile = argv[5];
        outFile = argv[6];
    }
    catch (std::exception e) {
        PrintUsage();
        return -1;
    }

    return JudgeStdIO(exeFilePath, timeLimit, memLimit, inFile, outFile);
}
