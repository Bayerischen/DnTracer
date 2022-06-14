#include "pch.h"
#include "Logger.h"

Logger::Logger()
{
    logFile = CreateFileA("log.txt", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (logFile == INVALID_HANDLE_VALUE)
    {
        printf("Create log file log.txt failed, last error: %d", GetLastError());
        exit(0);
    }
}

void Logger::WriteLog(const char* format, ...)
{
    DWORD dwWrite;
    char buffer[256];
    va_list argptr;
    va_start(argptr, format);
    vsnprintf(buffer, 256, format, argptr);
    puts(buffer);
    
    DWORD bufLen = (DWORD)strlen(buffer);
    buffer[bufLen] = '\n';
    WriteFile(logFile, buffer, bufLen + 1, &dwWrite, NULL);
    va_end(argptr);
}