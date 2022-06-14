#pragma once

class Logger
{
public:
	Logger();
	void WriteLog(const char* format, ...);

private:
	HANDLE logFile;
};