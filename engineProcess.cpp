#include "engineProcess.h"
#include <iostream>

bool EngineProcess::start(const std::string& path) {
    SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
    HANDLE inRead, outWrite;

    // Parent writes -> engine stdin
    CreatePipe(&inRead, &inWrite, &sa, 0);
    SetHandleInformation(inWrite, HANDLE_FLAG_INHERIT, 0);

    // Engine stdout -> parent reads
    CreatePipe(&outRead, &outWrite, &sa, 0);
    SetHandleInformation(outRead, HANDLE_FLAG_INHERIT, 0);

    STARTUPINFOA si = { sizeof(si) };
    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdInput = inRead;
    si.hStdOutput = outWrite;
    si.hStdError = outWrite;

    if (!CreateProcessA(
            NULL,
            (LPSTR)path.c_str(),
            NULL, NULL, TRUE,
            0, NULL, NULL, &si, &pi))
    {
        return false;
    }

    CloseHandle(inRead);
    CloseHandle(outWrite);

    running = true;
    return true;
}

void EngineProcess::send(const std::string& cmd) {
    std::string c = cmd + "\n";
    DWORD written;
    WriteFile(inWrite, c.c_str(), (DWORD)c.size(), &written, NULL);
}

std::string EngineProcess::readLine() {
    char ch;
    DWORD read;
    std::string line;

    while (true) {
        if (!ReadFile(outRead, &ch, 1, &read, NULL) || read == 0)
            break;
        if (ch == '\n') break;
        line += ch;
    }
    return line;
}

void EngineProcess::stop() {
    if (running) {
        TerminateProcess(pi.hProcess, 0);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        running = false;
    }
}
