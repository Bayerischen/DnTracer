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

void Logger::Write(const char* log, DWORD length)
{
    DWORD dwWrite;
    printf(log);
    WriteFile(logFile, log, strlen(log), &dwWrite, NULL);
}

void Logger::WriteFormat(const char* format, ...)
{
    int bufLen = 256;
    char* buffer = new char[bufLen];
    DWORD dwWrite;
    va_list argptr;
    va_start(argptr, format);
    
    //format the string until succeed
    int slen = 0;
    while(true) 
    {
        slen = vsnprintf(buffer, bufLen, format, argptr);
        if (slen < 0) //slen < 0 means buffer length is not enough
        {
            bufLen *= 2;
            delete[] buffer;
            buffer = new char[bufLen];
        }
        else
        {
            break;
        }
    }

    Write(buffer, slen);

    delete[] buffer;

    va_end(argptr);
}

void Logger::WriteLine(const char* log, DWORD length)
{
    Write(log, length);

    DWORD dwWrite;
    char newline = '\n';
    printf("\n");
    WriteFile(logFile, &newline, 1, &dwWrite, NULL);
}