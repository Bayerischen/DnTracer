#pragma once

class Logger
{
public:
	Logger();
	void Write(const char* log, DWORD length);
	void WriteFormat(const char* format, ...);
	void WriteLine(const char* log, DWORD length);

private:
	HANDLE logFile;
};