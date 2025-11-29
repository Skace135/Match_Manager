// EngineProcess.h
#pragma once
#include <windows.h>
#include <string>

class EngineProcess {
public:
    HANDLE inWrite = NULL;
    HANDLE outRead = NULL;
    PROCESS_INFORMATION pi = {};
    bool running = false;

    bool start(const std::string& path);
    void send(const std::string& cmd);
    std::string readLine();
    void stop();
};
